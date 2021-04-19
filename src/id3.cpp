#include <id3.hpp>
#include <picture.hpp>


bool detectID3(Filehandler& t_handler) noexcept {

    std::string s;
    t_handler.readString(s, LOCATION_START, 3);

    bool result = s.compare("ID3") == 0;

    // TODO log debug
    std::cout << (result ? "Found ID3 header!" : "No ID3 header present in file") << std::endl;

    return result;
}


bool detectID3Footer(Filehandler& t_handler) noexcept {

    std::string s;
    t_handler.readString(s, LOCATION_START, std::ios::end, 3);


    bool result = s.compare("3DI") == 0;

    // TODO log debug
    std::cout << (result ? "Found ID3 footer!" : "No ID3 footer present in file") << std::endl;

    return result;
}


std::uint8_t getVersion(Filehandler& t_handler) noexcept {

    char buffer[SIZE_OF_VERSION];

    t_handler.readBytes(buffer, LOCATION_VERSION, SIZE_OF_VERSION);

    std::uint8_t version = static_cast<std::uint8_t>(*buffer);

    // TODO log debug
    std::cout << "ID3 file has version: 2." << int(version) << std::endl;

    return version;
}


std::uint8_t getFlags(Filehandler& t_handler) noexcept {
    char buffer[SIZE_OF_FLAGS];

    t_handler.readBytes(buffer, LOCATION_FLAGS, SIZE_OF_FLAGS);

    return static_cast<std::uint8_t>(*buffer);
}


std::uint32_t getSize(Filehandler& t_handler, const bool t_extended) noexcept {

    const unsigned char BUFFER_LOCATION = t_extended ? SIZE_OF_HEADER : LOCATION_SIZE;

    // Have to do this, since C++ doesn't support variable length arrays
    std::vector<char> buffer(SIZE_OF_SIZE);

    t_handler.readBytes(buffer.data(), BUFFER_LOCATION, SIZE_OF_SIZE);


    // TODO max size of tag (update this as well as position)
    std::uint32_t size = static_cast<std::uint32_t>(convert_bytes(buffer.data(), SIZE_OF_SIZE, true));

    return size;
}


void synchronize(const char* t_data, std::uint32_t t_size) noexcept {

    std::vector<char> bytes;

    bool sync = true;

    for (std::uint32_t i = 0; i < t_size; ++i) {
        if (sync) {
            bytes.push_back(t_data[i]);
            sync = (static_cast<unsigned char>(t_data[i]) != 0xff);
        }

        else {
            if (t_data[i] != 0x00)
                bytes.push_back(t_data[i]);

            sync = true;
        }

    }

    t_data = bytes.data();
}


void increment_pc(Filehandler& t_handler, std::uint32_t t_position) noexcept {

    // TODO this needs some testing and fixing

    // saving the position of the start of the header, as I will need this later
    std::uint32_t original_position = t_position;

    std::string frame_id;

    // read counter and convert it from a null terminated hex string to a number
    // TODO this does not work yet since, readFrame will interpret is at a string, not bytes,
    //      and the conversion will fail (since I'll expect a string of hex numbers)
    if (auto data = readFrame(t_handler, frame_id, t_position)) {

        // frame data is never more than std::uint32_t
        std::uint32_t size = static_cast<std::uint32_t>((*data)->size());

        // read counter (and convert it from base 16 to base 10)
        std::uint64_t counter = convert_bytes(((*data)->data()), size, false);

        counter += 1;

        // this evaluates to true, if counter only consists of 0xff bytes
        bool f = (counter + 1) % 16 == 0;

        std::uint32_t data_size = size;

        // if the data only consists of 0xff bytes, a 0x00 byte will be prepended
        // and therefore the size needs to be increased by 1
        if (f)
            data_size += 1;

        // TODO log debug
        std::cout << "Increased play counter" << std::endl;
        std::cout << "Writing play counter: 0x" << std::hex << counter << " and size: " << size << " back to file" << std::endl;

        char size_bytes[4];

        convert_size(size, size_bytes);

        // create buffer with enough room for size (4), flags (2) and data (data_size)
        char* payload = new char[4 + 2 + data_size];

        int i = 0;

        // copy all the size bytes into 1 buffer
        for (char byte : size_bytes) {
            payload[i] = byte;
            ++i;
        }

        std::uint32_t offset = original_position + 4;

        char buffer_flags[2];

        // read flags
        t_handler.readBytes(buffer_flags, offset + 4, 2);

        // copy flags over to that buffer as well
        for (char byte : buffer_flags) {
            payload[i] = byte;
            ++i;
        }

        // prepending 1 zero byte if the counter only
        // consists of 0xff bytes
        if (f) {
            payload[i] = 0x00;
            ++i;
        }

        // copy data to buffer
        auto counter_buffer = convert_dec(counter);

        for (char num : *counter_buffer) {
            payload[i] = num;
            ++i;
        }

        // TODO log debug
        std::cout << "Writing bytes:\n";
        for (std::uint32_t cnt = 0; cnt < data_size; ++cnt) {
            std::cout <<  std::hex << payload[cnt] << " ";
        }
        std::cout << "\n to file."<< std::endl;


        // TODO data not null terminated, is it supposed to be?
        // TODO currently it's a string, we might not want this
        t_handler.deleteBytes(offset, 4 + 2 + size);
        t_handler.writeBytes(offset, payload, 4 + 2 + data_size);

        delete [] payload;
    }

}


void parseFrameData(std::shared_ptr<std::vector<char>> t_data, std::string t_frame_id, Song& t_song) noexcept {

    if (t_frame_id.compare("TIT2") == 0) {

        // TODO deal with possibility of having an error
        std::string content = decode_text(t_data->at(0), t_data, 1);

        // TODO log verbose
        std::cout << "Found a TIT2 frame, setting song title to: " << content << std::endl;

        t_song.m_title = content;

    } else if (t_frame_id.compare("TALB") == 0) {

        // TODO deal with possibility of having an error
        std::string content = decode_text(t_data->at(0), t_data, 1);

        // TODO log verbose
        std::cout << "Found a TALB frame, setting album title to: " << content << std::endl;

        t_song.m_album = content;

    } else if (t_frame_id.compare("TPE1") == 0) {

        // TODO deal with possibility of having an error
        std::string content = decode_text(t_data->at(0), t_data, 1);

        // TODO log verbose
        std::cout << "Found a TPE1 frame, setting artist to: " << content << std::endl;

        t_song.m_artist = content;

    } else if (t_frame_id.compare("TDRL") == 0) {

        // starting from 0, five characters (4 + '\0')
        // TODO is this right?
        // TODO deal with possibility of having an error
        std::string content = decode_text(t_data->at(0), t_data, 1).substr(0, 5);

        // TODO log verbose
        std::cout << "Found a TDRL frame setting release year to: " << content << std::endl;

        t_song.m_release = content;

    } else if (t_frame_id.compare("TDRC") == 0) {

        // release year has precedence over recording year but if
        // there is no TDRL frame, this frame will be used for the date instead
        if (t_song.m_release.compare("") == 0) {

            // starting from 0, five characters (4 + '\0')
            // TODO is this right?
            // TODO deal with possibility of having an error
            std::string content = decode_text(t_data->at(0), t_data, 1).substr(0, 5);

            // TODO log verbose
            std::cout << "Found a TDRC frame setting release year to: " << content << std::endl;

            t_song.m_release = content;
        }

        else {

            // TODO log debug
            std::cout << "Found a TDRC frame, but release year has already been set. Skipping..." << std::endl;
        }

    } else if (t_frame_id.compare("TLEN") == 0) {

        auto len = convert_bytes(t_data->data(), static_cast<std::uint32_t>(t_data->size()), false);

        // TODO log verbose
        std::cout << "Found a TLEN frame, setting track length to: " << len << std::endl;

        t_song.m_duration = len;

    } else if (t_frame_id.compare("TDLY") == 0) {

        auto delay = convert_bytes(t_data->data(), static_cast<std::uint32_t>(t_data->size()), false);

        // TODO log verbose
        std::cout << "Found a TDLY frame, setting delay to: " << delay << "ms" << std::endl;

        t_song.m_delay = delay;

    } else if (t_frame_id.compare("TCON") == 0) {

        // custom genres have precedence over content type but if
        // there is no custom genre set, this frame will be used for the genre instead
        // TODO this is different for older tag versions
        if (t_song.m_genre.compare("Unknown Genre") == 0) {

            // TODO deal with possibility of having an error
            std::string content = decode_text(t_data->at(0), t_data, 1);

            // TODO log verbose
            std::cout << "Found a TCON frame, setting genre to: " << content << std::endl;

            // TODO not sure if this is always fine (as there can also be numbers apparently)
            //      so maybe this is not the correct way to "parse" the data, but I'll have to
            //      check with more files.
            t_song.m_genre = content;
        }

    } else if (t_frame_id.compare("TRCK") == 0) {

        // TODO log info
        std::cout << "Found a TRCK frame, setting track number to: ";

        std::string track_number = "";

        // TRCK frame can contain a / with the total amount of tracks
        // after the track number. I don't care about that
        for (char c : *t_data) {
            if (c == '/')
                break;
            else
                track_number += c;
        }

        // TODO log info
        std::cout << track_number << std::endl;

        t_song.m_track_number = track_number;

    } else if (t_frame_id.compare("APIC") == 0) {

        // TODO log info
        std::cout << "Found an APIC frame" << std::endl;

        std::uint32_t iterator = 0;

        // byte indicating text encoding
        std::uint8_t text_encoding = t_data->at(iterator++);

        std::string mime_type = "";

        char c = t_data->at(iterator);

        while (c != 0) {
            mime_type += c;
            c = t_data->at(++iterator);
        }

        // TODO log debug
        std::cout << "Found picture with MIME type: " << mime_type << std::endl;

        ID3::PictureType pic_type = static_cast<ID3::PictureType>(t_data->at(iterator++));

        auto container = decode_text_retain_position(text_encoding, t_data, iterator);

        if (!container.error) {

            std::string description = container.text;

            iterator = container.position;

            auto pic_data = std::make_unique<std::vector<char>>();


            // extracting picture data
            while (iterator < t_data->size()) {
                pic_data->push_back(t_data->at(iterator++));
            }


            ID3::Picture art = ID3::Picture(std::move(pic_data), mime_type, pic_type);

            // TODO not sure if a copy is the best idea here, but I'll leave
            //      it like this for now
            t_song.m_art.push_back(art);

        }

        else {
            // TODO deal with error

            // TODO log error
            std::cout << container.text << std::endl;
        }

    } else if (t_frame_id.compare("PCNT") == 0) {

        std::uint64_t play_counter = convert_bytes(t_data->data(), static_cast<std::uint32_t>(t_data->size()), false);

        // TODO log info
        std::cout << "Found an PCNT frame, setting play counter to: " << play_counter << std::endl;

        t_song.m_play_counter = play_counter;

    } else {
        // TODO log warn
        std::cout << "frame id: " << t_frame_id << " is not supported yet" << std::endl;
    }

    // TODO TFLT (audio type, default is MPEG)
    // TODO MLLT (MPEG location lookup table (do I need this) (4.6), mentions player counter (4.16))

}

std::optional<std::shared_ptr<std::vector<char>>> readFrame(Filehandler& t_handler, std::string& t_frame_id, std::uint32_t& t_position) noexcept {

    t_handler.readString(t_frame_id, t_position, SIZE_OF_FRAME_ID);

    if (t_frame_id[0] == 0x00) {
        // TODO log debug
        std::cout << "Encountered a frame id starting with 0x00 which is probably due to padding...\n"
                  <<  "Skipping to the end of the tag." << std::endl;

        return {};
    }

    t_position += SIZE_OF_FRAME_ID;

    char size_buffer[SIZE_OF_SIZE];

    t_handler.readBytes(size_buffer, t_position, SIZE_OF_SIZE);

    std::uint32_t frame_data_size = static_cast<std::uint32_t>(convert_bytes(size_buffer, SIZE_OF_SIZE, true));

    t_position += SIZE_OF_SIZE;

    char flags_buffer[2];

    t_handler.readBytes(flags_buffer, t_position, 2);

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

    t_position += 2;

    std::cout << "reading " << frame_data_size << " bytes of frame with id " << t_frame_id << std::endl;

    auto frame_content = std::make_unique<std::vector<char>>(frame_data_size);

    t_handler.readBytes(frame_content->data(), t_position, frame_data_size);

    // taking frame data in account when updating position
    t_position += frame_data_size;

    // synchronizing frame data
    if (format_flags & (1 << 1)) {

        synchronize(frame_content->data(), static_cast<std::uint32_t>(frame_content->size()));

    }

    return frame_content;
}


void readID3(Song& t_song) noexcept {

    Filehandler handler = Filehandler(t_song.m_path);

    if (detectID3(handler)) {
        auto version = getVersion(handler);

        // TODO verbose logging instead of this
        std::cout << "ID3v2." << static_cast<std::uint16_t>(version) << std::endl;

        // TODO proceed with extracting metadata
        auto flags = getFlags(handler);

        // TODO size is without 10 bytes of header
        auto size = getSize(handler, false);

        // TODO log info
        std::cout << "Size of ID3 tag: " << size << std::endl;

        std::uint32_t extended_size = 0;

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


            // TODO are 32 bits enough, if not, need to up position and this to uint64
            std::int32_t size_remaining = size + extended_size;

            std::uint32_t position = SIZE_OF_HEADER + extended_size;

            // TODO log debug
            std::cout << "Starting to read frames at position " << position << std::endl;

            while (size_remaining > 0) {
                std::cout << size_remaining << " bytes remaining..." << std::endl;
                std::cout << size_remaining << " > 0" << std::endl;
                std::string frame_id = "";

                // TODO I should probably choose less ambiguous names
                std::uint32_t original_position_file = position;
                auto result = readFrame(handler, frame_id, position);

                // There are no frames left, the rest is padding
                if (!result.has_value()) {
                    // TODO log debug
                    std::cout << "Result has no value, so read a frame_id starting with 0x00" << std::endl;

                    position = size_remaining;
                    size_remaining = 0;
                }

                else {
                    auto frame_content = result.value();

                    if (frame_id.compare("PCNT") == 0) {

                        // setting posistion of start of play counter frame
                        t_song.m_counter_offset = original_position_file;

                    }

                    size_remaining -= (position - original_position_file);

                    // TODO log info
                    std::cout << "Size remaining: " << size_remaining << std::endl;

                    // TODO log info
                    std::cout << "Continuing to read at position: " << position << std::endl;

                    // frame_id has been not been set properly,
                    // still parsing the frame as I can't skip it
                    // without knowing it's size
                    if (frame_id.compare("") == 0) {
                        // TODO log error
                        std::cout << "frame_id has not been set properly" << std::endl;
                    }

                    parseFrameData(frame_content, frame_id, t_song);

                }
            }

        }


    }
    else {
        // TODO log debug
        std::cout << "No ID3 Tag has been prepended to file: " << t_song.m_path << std::endl;
    }
}


