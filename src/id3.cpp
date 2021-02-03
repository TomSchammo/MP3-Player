#include <id3.hpp>
#include <iostream>


bool detectID3(Filehandler &handler) {

    auto ptr = handler.readString(LOCATION_START, 3);

    auto returnValue = std::string(ptr).compare("ID3") == 0;

    delete[] ptr;
    return returnValue;
}


bool detectID3Footer(Filehandler &handler) {

    auto ptr = handler.readString(LOCATION_START, std::ios::end, 3);


    auto returnValue = std::string(ptr).compare("3DI") == 0;

    delete[] ptr;
    return returnValue;
}


std::uint8_t getVersion(Filehandler &handler) {
    auto ptr = handler.readBytes(LOCATION_VERSION, SIZE_OF_VERSION);
    auto version = *ptr;
    delete[] ptr;

    return static_cast<std::uint8_t>(version);
}


std::uint8_t getFlags(Filehandler &handler) {
    auto ptr = handler.readBytes(LOCATION_FLAGS, SIZE_OF_FLAGS);
    auto flags = *ptr;
    delete[] ptr;

    return static_cast<std::uint8_t>(flags);
}


std::uint16_t getSize(Filehandler &handler, const bool extended) {
    auto ptr = handler.readBytes(extended ? SIZE_OF_HEADER : LOCATION_SIZE, SIZE_OF_SIZE);
    std::uint16_t size = 0;

    for (int i = 0; i < SIZE_OF_SIZE; ++i) {
        size += static_cast<std::uint16_t>(ptr[i]);
    }

    delete[] ptr;

    return size;
}


void parseFrameData(const char* data, std::string frameID, Song &song) {

    if (frameID.compare("TIT2") == 0) {

        // TODO log verbose
        std::cout << "Found a TIT2 frame, setting song title to: " << data << std::endl;

        song.m_title = data;

    } else if (frameID.compare("TALB") == 0) {


        // TODO log verbose
        std::cout << "Found a TALB frame, setting album title to: " << data << std::endl;

        song.m_album = data;

    } else if (frameID.compare("TPE1") == 0) {

        // TODO log verbose
        std::cout << "Found a TPE1 frame, setting artist to: " << data << std::endl;

        song.m_artist = data;

    } else if (frameID.compare("TDRL") == 0) {

        // TODO log verbose
        std::cout << "Found a TDRL frame setting release year to: " << data << std::endl;

        // starting from 0, five characters (4 + '\0')
        song.m_release = std::string(data).substr(0, 5);

    } else if (frameID.compare("TDRC") == 0) {

        // if release year has precedence over recording year but if
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

    } else if (frameID.compare("TLEN") == 0) {
        // TODO log verbose
        std::cout << "Found a TLEN frame, setting track length to: " << data << std::endl;

        // TODO track length not necessarily included, nice if it is, but should not depend

        song.m_duration = static_cast<std::uint16_t>(*data);

    } else if (frameID.compare("TDLY") == 0) {
        // TODO log verbose
        std::cout << "Found a TDLY frame, setting delay to: " << data << "ms" << std::endl;

        song.m_delay = static_cast<std::uint16_t>(*data);

    } else {
        // TODO log warn
        std::cout << "frameID: " << frameID << " is not supported yet" << std::endl;
    }

    // TODO TFLT (audio type, default is MPEG)
    // TODO MLLT (MPEG location lookup table (do I need this) (4.6), mentions player counter (4.16))
    // TODO APIC (Album art (section 4.14))
    // TODO PCNT (Player counter (4.16), should be incremented)
    // TODO TRCK (Track number)
    // TODO TDRC (Record time (if TDRL is not available))
    // TODO TCON (content type, technically genre?) used when genre is not available, match with list

    delete [] data;
}


const char* readFrame(Filehandler &handler, std::uint16_t position, std::string &frame_id, std::uint16_t &bytes_read) {

    auto ptr = handler.readString(position, SIZE_OF_FRAME_ID);

    frame_id = std::string(ptr, SIZE_OF_FRAME_ID);

    position += SIZE_OF_FRAME_ID;

    ptr = handler.readBytes(position, SIZE_OF_SIZE);

    for (int i = 0; i < SIZE_OF_SIZE; ++i) {
        bytes_read += static_cast<std::uint16_t>(ptr[i]);
    }

    position += SIZE_OF_SIZE;

    ptr = handler.readBytes(position, 2);

    std::uint8_t status_flags = ptr[0];

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
    std::uint8_t format_flags = ptr[1];

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

    // reading the data of the frame
    // TODO why do I need this + 1, it doesn't make sense...
    ptr = handler.readString(position+1, bytes_read);

    bytes_read += 10;

    return ptr;
    // TODO parse data of frame
    // TODO either need to pass reference to song object
    // TODO or return the pointer at function call and then call parser from different function
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

        // TODO is that +10 necessary here?
        // TODO since the header is already read when it matters, it should be fine without it
        auto size = getSize(handler, false) + 10;

        std::uint16_t extended_size = 0;

        // Not supported ID3 version, skipping the tag
        // TODO check if lower versions are supported
        if (version > 4) {
            // TODO log error
            std::cout << "This software does not support ID3 version ID3v2." << version << std::endl;

            // TODO set filepointer to after tag?
            // TODO if I'd do that, I'd have to know whether it contains extended headers though
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
                const char* ptr = readFrame(handler, position, frame_id, size_read);

                // TODO size is weird sometimes
                size_remaining -= size_read;
                position += size_read;

                // frame_id has been set properly
                if (frame_id.compare("") == 0)
                    // TODO log error
                    std::cout << "frame_id has not been set properly" << std::endl;

                parseFrameData(ptr, frame_id, song);
            }



        }


    }
    else {
        // TODO log debug
        std::cout << "No ID3 Tag has been prepended to file: " << song.m_path << std::endl;
    }
}


