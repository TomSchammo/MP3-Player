#include <id3.hpp>
#include <iostream>
#include <vector>


bool detectID3(Filehandler &handler) {

    std::string s;
    handler.readString(s, LOCATION_START, 3);

    bool result = s.compare("ID3") == 0;

    // TODO log debug
    std::cout << (result ? "Found ID3 header!" : "No ID3 header present in file") << std::endl;

    return result;
}


bool detectID3Footer(Filehandler &handler) {

    std::string s;
    handler.readString(s, LOCATION_START, std::ios::end, 3);


    bool result = s.compare("3DI") == 0;

    // TODO log debug
    std::cout << (result ? "Found ID3 footer!" : "No ID3 footer present in file") << std::endl;

    return result;
}


std::uint8_t getVersion(Filehandler &handler) {

    char buffer[SIZE_OF_VERSION];

    handler.readBytes(buffer, LOCATION_VERSION, SIZE_OF_VERSION);

    std::uint8_t version = static_cast<std::uint8_t>(*buffer);

    // TODO log debug
    std::cout << "ID3 file has version: 2." << int(version) << std::endl;

    return version;
}


std::uint8_t getFlags(Filehandler &handler) {
    char buffer[SIZE_OF_FLAGS];

    handler.readBytes(buffer, LOCATION_FLAGS, SIZE_OF_FLAGS);

    return static_cast<std::uint8_t>(*buffer);
}


std::uint16_t getSize(Filehandler &handler, const bool extended) {

    const unsigned char BUFFER_SIZE = extended ? SIZE_OF_HEADER : LOCATION_SIZE;

    // Have to do this, since C++ doesn't support variable length arrays
    std::vector<char> buffer(BUFFER_SIZE);

    handler.readBytes(buffer.data(), BUFFER_SIZE, SIZE_OF_SIZE);

    std::uint16_t size = 0;

    for (int i = 0; i < SIZE_OF_SIZE; ++i) {
        size += static_cast<std::uint16_t>(buffer[i]);
    }

    return size;
}


void synchronize(unsigned char* data, std::uint16_t size) {

    std::vector<unsigned char> bytes;

    bool sync = true;

    for (std::uint16_t i = 0; i < size; ++i) {
        if (sync) {
            bytes.push_back(data[i]);
            sync = (data[i] != 0xff);
        }

        else {
            if (data[i] != 0x00)
                bytes.push_back(data[i]);

            sync = true;
        }

    }

    data = bytes.data();
}


void parseFrameData(std::string data, std::string frame_id, Song &song) {

    if (frame_id.compare("TIT2") == 0) {

        // TODO log verbose
        std::cout << "Found a TIT2 frame, setting song title to: " << data << std::endl;

        song.m_title = data;

    } else if (frame_id.compare("TALB") == 0) {


        // TODO log verbose
        std::cout << "Found a TALB frame, setting album title to: " << data << std::endl;

        song.m_album = data;

    } else if (frame_id.compare("TPE1") == 0) {

        // TODO log verbose
        std::cout << "Found a TPE1 frame, setting artist to: " << data << std::endl;

        song.m_artist = data;

    } else if (frame_id.compare("TDRL") == 0) {

        // TODO log verbose
        std::cout << "Found a TDRL frame setting release year to: " << data << std::endl;

        // starting from 0, five characters (4 + '\0')
        song.m_release = std::string(data).substr(0, 5);

    } else if (frame_id.compare("TDRC") == 0) {

        // release year has precedence over recording year but if
        // there is no TDRL frame, this frame will be used for the date instead
        if (song.m_release.compare("") == 0) {

            // TODO log verbose
            std::cout << "Found a TDRC frame setting release year to: " << data << std::endl;

            // starting from 0, five characters (4 + '\0')
            song.m_release = std::string(data).substr(0, 5);
        }

        else {

            // TODO log debug
            std::cout << "Found a TDRC frame, but release year has already been set. Skipping..." << std::endl;
        }

    } else if (frame_id.compare("TLEN") == 0) {
        // TODO log verbose
        std::cout << "Found a TLEN frame, setting track length to: " << data << std::endl;

        // TODO track length not necessarily included, nice if it is, but should not depend

        // TODO need to check if this works
        // TODO is this a string or bytes???
        // song.m_duration = static_cast<std::uint16_t>(*data);

    } else if (frame_id.compare("TDLY") == 0) {
        // TODO log verbose
        std::cout << "Found a TDLY frame, setting delay to: " << data << "ms" << std::endl;

        // TODO need to check if this works
        // TODO is this a string or bytes???
        // song.m_delay = static_cast<std::uint16_t>(*data);

    } else if (frame_id.compare("TCON") == 0) {

        // custom genres have precedence over content type but if
        // there is no custom genre set, this frame will be used for the genre instead
        // TODO this is different for older tag versions
        if (song.m_genre.compare("Unknown Genre") == 0) {

            // TODO log verbose
            std::cout << "Found a TCON frame, setting genre to: " << data << std::endl;

            // TODO not sure if this is always fine (as there can also be numbers apparently)
            //      so maybe this is not the correct way to "parse" the data, but I'll gave to
            //      check with more files.
            song.m_genre = data;
        }

    } else {
        // TODO log warn
        std::cout << "frame id: " << frame_id << " is not supported yet" << std::endl;
    }

    // TODO TFLT (audio type, default is MPEG)
    // TODO MLLT (MPEG location lookup table (do I need this) (4.6), mentions player counter (4.16))
    // TODO APIC (Album art (section 4.14))
    // TODO PCNT (Player counter (4.16), should be incremented)
    // TODO TRCK (Track number)

}


std::string readFrame(Filehandler &handler, std::uint16_t position, std::string &frame_id, std::uint16_t &bytes_read) {

    handler.readString(frame_id, position, SIZE_OF_FRAME_ID);

    // std::uint16_t start_of_frame = position;
    // TODO if frame_id == PCNT, set song.m_counter_offset = position;
    // if (frame_id.compare("PCNT") == 0)
        // return (char*)(static_cast<std::uint64_t>(start_of_frame));

    position += SIZE_OF_FRAME_ID;

    char size_buffer[SIZE_OF_SIZE];

    handler.readBytes(size_buffer, position, SIZE_OF_SIZE);

    for (int i = 0; i < SIZE_OF_SIZE; ++i) {
        bytes_read += static_cast<std::uint16_t>(size_buffer[i]);
    }

    position += SIZE_OF_SIZE;

    char flags_buffer[2];

    handler.readBytes(flags_buffer, position, 2);

    std::uint8_t status_flags = flags_buffer[0];

    if (status_flags & (1 << 5)) {
        // TODO frame should be discarded (bit == 1) if frame is unknown and tag is altered
    }

    if (status_flags & (1 << 4)) {
        // TODO frame should be discarded (bit == 1) if frame is unknown and file is altered
    }

    if (status_flags & (1 << 3)) {
        // TODO frame is read only (don't increase counter)
    }

    // TODO relevant bits: 6, 3, 2, 1, 0 (all)
    std::uint8_t format_flags = flags_buffer[1];

    if (format_flags & 1) {
        // TODO Data length indicator has been added
        // TODO idk what this is even for
    }

    if (format_flags & (1 << 1)) {
        // TODO frame is unsynchronised (end of header to end of frame)
        // TODO %11111111 111xxxxx should be replaced with %11111111 00000000 111xxxxx
        // TODO So $FF 00 should be replaced with $FF 00 00 (?)
        // TODO
    }

    if (format_flags & (1 << 2)) {
        // TODO frame is encrypted
        // TODO 1 byte with encryption method is added
        // TODO see ENCR frame
    }

    if (format_flags & (1 << 3)) {
        // TODO frame is compressed with zlib
    }

    if (format_flags & (1 << 6)) {
        // TODO frame belongs to group
        // TODO read group identifier
        // TODO put data of group frames together
    }

    // TODO remove, this is just to avoid errors
    status_flags = format_flags = 0;

    position += 2;

    std::cout << "reading " << bytes_read << " bytes of frame with id " << frame_id << std::endl;
    // reading the data of the frame
    // TODO why do I need this + 1, it doesn't make sense...
    // TODO apparently size assumes null terminated string, but apparently
    //      not every string in a frame is null terminated, so that can fuck things up
    // TODO is the size wrong then?
    // also TODO pls fix
    std::string s;
    handler.readString(s, position+1, bytes_read-1);

    bytes_read += 10;

    return s;
    // TODO subtract size + 10 from size left in readID3

}


void readID3(Song &song) {


    Filehandler handler(song.m_path);

    if (detectID3(handler)) {
        auto version = getVersion(handler);

        // TODO verbose logging instead of this
        std::cout << "ID3v2." << static_cast<std::uint16_t>(version) << std::endl;

        // TODO proceed with extracting metadata
        auto flags = getFlags(handler);

        // TODO size is without 10 bytes of header
        auto size = getSize(handler, false);

        std::uint16_t extended_size = 0;

        // Not supported ID3 version, skipping the tag
        // TODO Implement ID3v2.2 and below
        if (version != 4 && version != 3) {
            // TODO log error
            std::cout << "This software does not support ID3 version ID3v2." << version << std::endl;

            // TODO set filepointer to after tag?
            // TODO if I'd do that, I'd have to know whether it contains extended headers though (for complete size)
        }

        else {

            // TODO synchronize
            if (flags & (1 << 7)) {
                std::cout << "This tag is unsynchronised..." << std::endl;
            }

            // Extended header is present, skipping it...
            if (flags & (1 << 6)) {
                extended_size = getSize(handler, true);

                // TODO log verbose
                std::cout << "This tag contains and extended header. Skipping it for now..." << std::endl;

            }

            // TODO footer present (do I care?)
            // TODO probably need to tell the mp3 decoder to stop 10 bytes before EOF
            if (flags & (1 << 4)) {
                // size += 10;
                // don't do this, just set a footer bit to true or something
                std::cout << "footer" << std::endl;
            }


            auto size_remaining = size + extended_size;

            std::uint16_t position = SIZE_OF_HEADER + extended_size;

            while (size_remaining > 0) {
                std::string frame_id = "";

                std::uint16_t size_read = 0;
                std::string frame_content = readFrame(handler, position, frame_id, size_read);

                // TODO size is weird sometimes (by weird I mean 0 >)
                std::cout << "Size: " << size_remaining << " - " << size_read << " = ";
                size_remaining -= size_read;
                std::cout << size_remaining << std::endl;

                std::cout << "Position: " << position << " + " << size_read << " = ";
                position += size_read;
                std::cout << position << std::endl;

                // frame_id has been set properly
                if (frame_id.compare("") == 0)
                    // TODO log error
                    std::cout << "frame_id has not been set properly" << std::endl;

                parseFrameData(frame_content, frame_id, song);
            }



        }


    }
    else {
        // TODO log debug
        std::cout << "No ID3 Tag has been prepended to file: " << song.m_path << std::endl;
    }
}


