#include <id3.hpp>
#include <picture.hpp>

using namespace ID3;

bool ID3::detectID3(Filehandler& t_handler) noexcept {

    std::string s;
    t_handler.readString(s, LOCATION_START, 3);

    bool result = s == "ID3";

    log::debug(result ? "Found ID3 header!" : "No ID3 header present in file");


    return result;
}


bool detectID3Footer(Filehandler& t_handler) noexcept {

    std::string s;
    t_handler.readString(s, LOCATION_START, std::ios::end, 3);

    bool result = s == "3DI";


    log::debug(result ? "Found ID3 footer!" : "No ID3 footer present in file");

    return result;
}


std::uint8_t ID3::getVersion(Filehandler& t_handler) noexcept {

    char buffer[SIZE_OF_VERSION];

    t_handler.readBytes(buffer, LOCATION_VERSION, SIZE_OF_VERSION);

    auto version = static_cast<std::uint8_t>(*buffer);


    log::debug(fmt::format("ID3 file has version: 2.{:d}", version));

    return version;
}


std::uint8_t ID3::getFlags(Filehandler& t_handler) noexcept {
    char buffer[SIZE_OF_FLAGS];

    t_handler.readBytes(buffer, LOCATION_FLAGS, SIZE_OF_FLAGS);

    return static_cast<std::uint8_t>(*buffer);
}


std::uint32_t ID3::getSize(Filehandler& t_handler, const bool t_extended) noexcept {

    const unsigned char BUFFER_LOCATION = t_extended ? SIZE_OF_HEADER : LOCATION_SIZE;

    std::array<char, SIZE_OF_SIZE> buffer{};

    t_handler.readBytes(buffer.data(), BUFFER_LOCATION, SIZE_OF_SIZE);


    // TODO max size of tag (update this as well as position)
    auto size = static_cast<std::uint32_t>(convert_bytes(buffer.data(), SIZE_OF_SIZE, true));

    return size;
}


void ID3::synchronize(std::vector<char>& t_data) noexcept {

    bool sync = true;

    for (std::uint_fast32_t i = 0; i < t_data.size(); ++i) {
        if (sync) {
            sync = (static_cast<unsigned char>(t_data.at(i)) != 0xff);
        }

        else {
            if (t_data.at(i) == 0x00)
                t_data.erase(t_data.begin() + static_cast<long>(i));

            sync = true;
        }

    }

}


/*
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
        auto size = static_cast<std::uint32_t>(data->size());

        // read counter (and convert it from base 16 to base 10)
        std::uint64_t counter = convert_bytes((data->data()), size);

        counter += 1;

        // this evaluates to true, if counter only consists of 0xff bytes
        bool f = (counter + 1) % 16 == 0;

        std::uint32_t data_size = size;

        // if the data only consists of 0xff bytes, a 0x00 byte will be prepended
        // and therefore the size needs to be increased by 1
        if (f)
            data_size += 1;

        // TODO log debug
        // std::cout << "Increased play counter" << std::endl;
        // std::cout << "Writing play counter: 0x" << std::hex << counter << " and size: " << size << " back to file" << std::endl;

        std::array<std::uint8_t, 4> size_bytes {};

        convert_size(size, size_bytes);

        // create buffer with enough room for size (4), flags (2) and data (data_size)
        char* payload = new char[4 + 2 + data_size];

        int i = 0;

        // copy all the size bytes into 1 buffer
        for (auto byte : size_bytes) {
            payload[i] = static_cast<char>(byte);
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
        // std::cout << "Writing bytes:\n";
        // for (std::uint32_t cnt = 0; cnt < data_size; ++cnt) {
            // std::cout <<  std::hex << payload[cnt] << " ";
        // }
        // std::cout << "\n to file."<< std::endl;


        // TODO data not null terminated, is it supposed to be?
        // TODO currently it's a string, we might not want this
        t_handler.deleteBytes(offset, 4 + 2 + size);
        t_handler.writeBytes(offset, payload, 4 + 2 + data_size);

        delete [] payload;
    }

}
 */


std::unique_ptr<std::vector<char>> ID3::prepareFrameData(Filehandler& t_handler, FrameHeader& t_frame_header, std::uint32_t& t_position) noexcept {

    log::debug(fmt::format("Reading {} bytes of Frame with ID {}", t_frame_header.size, t_frame_header.id));

    auto frame_content = readFrame(t_handler, t_position, t_frame_header.size);

    // synchronizing frame data
    if (t_frame_header.format_flags & (1 << 1))
        synchronize(*frame_content);


    if (t_frame_header.format_flags & (1 << 2)) {
        log::debug(fmt::format("{} is an encrypted frame...", t_frame_header.id));
        // TODO frame is encrypted
        // TODO 1 byte with encryption method is added
        // TODO see ENCR frame
        // TODO decrypt after sync
    }

    if (t_frame_header.format_flags & (1 << 3)) {
        log::debug(fmt::format("{} is a compressed frame...", t_frame_header.id));
        // TODO frame is compressed with zlib
        // TODO decompress after sync
    }

    return frame_content;

}


bool ID3::parseFrame(Filehandler& t_handler, FrameHeader& t_frame_header, std::uint32_t& t_position, Song& t_song) noexcept {

    // this frame is a padding frame, skipping
    if (t_frame_header.id[0] == 0x00) {

        log::debug("Encountered a frame id starting with 0x00 which is probably due to padding, skipping to the end of the tag...");

        // TODO this does not work, there is no such things as 'padding frames'
        // TODO I have to skip to the end of the tag
        t_position += t_frame_header.size;

        return false;
    }

    else {


        if (t_frame_header.id == "TIT2") {

            auto data = *prepareFrameData(t_handler, t_frame_header, t_position);

            // TODO deal with possibility of having an error
            std::string content = decode_text(data.at(0), data, 1);

            log::info(fmt::format("Found a TIT2 frame, setting song title to: {}", content));

            t_song.m_title = content;

        } else if (t_frame_header.id == "TALB") {

            auto data = *prepareFrameData(t_handler, t_frame_header, t_position);

            // TODO deal with possibility of having an error
            std::string content = decode_text(data.at(0), data, 1);

            log::info(fmt::format("Found a TALB frame, setting album title to: {}", content));

            t_song.m_album = content;

        } else if (t_frame_header.id == "TPE1") {

            auto data = *prepareFrameData(t_handler, t_frame_header, t_position);

            // TODO deal with possibility of having an error
            std::string content = decode_text(data.at(0), data, 1);

            log::info(fmt::format("Found a TPE1 frame, setting artist to: {}", content));

            t_song.m_artist = content;

        } else if (t_frame_header.id == "TDRL") {

            auto data = *prepareFrameData(t_handler, t_frame_header, t_position);


            // starting from 0, five characters (4 + '\0')
            // TODO is this right?
            // TODO deal with possibility of having an error
            std::string content = decode_text(data.at(0), data, 1).substr(0, 5);

            log::info(fmt::format("Found a TDRL frame, setting release year to: {}", content));

            t_song.m_release = content;

        } else if (t_frame_header.id == "TDRC") {

            // release year has precedence over recording year but if
            // there is no TDRL frame, this frame will be used for the date instead
            if (t_song.m_release.empty()) {

                auto data = *prepareFrameData(t_handler, t_frame_header, t_position);

                // starting from 0, five characters (4 + '\0')
                // TODO is this right?
                // TODO deal with possibility of having an error
                std::string content = decode_text(data.at(0), data, 1).substr(0, 5);

                log::info(fmt::format("Found a TDRC frame, setting release year to: {}", content));

                t_song.m_release = content;
            }

            else {

                log::info("Found a TDRC frame, but release year has already been set. Skipping...");
            }

        } else if (t_frame_header.id == "TLEN") {

            auto data = *prepareFrameData(t_handler, t_frame_header, t_position);

            auto len = convert_bytes(data.data(), static_cast<std::uint32_t>(data.size()), false);

            log::info(fmt::format("Found a TLEN frame, setting track length to: {}", len));

            t_song.m_duration = len;

        } else if (t_frame_header.id == "TDLY") {

            auto data = *prepareFrameData(t_handler, t_frame_header, t_position);

            auto delay = convert_bytes(data.data(), static_cast<std::uint32_t>(data.size()), false);

            log::info(fmt::format("Found a TDLY frame, setting delay to: {}ms", delay));

            t_song.m_delay = delay;

        } else if (t_frame_header.id == "TCON") {

            // custom genres have precedence over content type but if
            // there is no custom genre set, this frame will be used for the genre instead
            // TODO this is different for older tag versions
            if (t_song.m_genre == "Unknown Genre") {

                auto data = *prepareFrameData(t_handler, t_frame_header, t_position);

                // TODO deal with possibility of having an error
                std::string content = decode_text(data.at(0), data, 1);

                log::info(fmt::format("Found a TCON frame, setting genre to: {}", content));

                // TODO not sure if this is always fine (as there can also be numbers apparently)
                //      so maybe this is not the correct way to "parse" the data, but I'll have to
                //      check with more files.
                t_song.m_genre = content;
            }

        } else if (t_frame_header.id == "TRCK") {

            auto data = *prepareFrameData(t_handler, t_frame_header, t_position);

            std::string track_number;

            // TRCK frame can contain a / with the total amount of tracks
            // after the track number. I don't care about that
            for (std::int_fast8_t c : data) {
                if (c == '/')
                    break;

                else
                    track_number += c;
            }

            log::info(fmt::format("Found a TRCK frame, setting track number to: {}", track_number));

            t_song.m_track_number = track_number;


        } else if (t_frame_header.id == "APIC") {

            log::info("Found an APIC frame");

            auto data = *prepareFrameData(t_handler, t_frame_header, t_position);

            std::uint32_t iterator = 0;

            // byte indicating text encoding
            std::int8_t text_encoding = data.at(iterator++);

            std::string mime_type;

            char c = data.at(iterator);

            while (c != 0) {
                mime_type += c;
                c = data.at(++iterator);
            }

            log::debug(fmt::format("Found picture with MIME type: {}", mime_type));

            // MIME Type is not a link, continuing as planned
            if (mime_type != "-->") {

                auto pic_type = static_cast<ID3::PictureType>(data.at(iterator++));

                auto container = decode_text_retain_position(text_encoding, data, iterator);

                if (!container.error) {

                    // TODO this is never used
                    // std::string description = container.text;

                    iterator = container.position;

                    auto pic_data = std::make_shared<std::vector<char>>();
                    pic_data->reserve(data.size() - iterator);


                    // extracting picture data
                    while (iterator < data.size()) {
                        pic_data->push_back(data.at(iterator++));
                    }


                    ID3::Picture art = ID3::Picture(pic_data, mime_type, pic_type);

                    // TODO not sure if a copy is the best idea here, but I'll leave
                    //      it like this for now
                    t_song.m_art.push_back(art);

                }

                else {
                    // TODO deal with error

                    log::error(container.text);
                }
            }

            // MIME Type is a link
            // Links are ignored because this code will not run on a network capable device,
            // so there is no way it will ever make use of that information.
            else {

                log::warn("Found APIC frames containing links, those are ignored as they are of no use for the purpose of this device.");
            }

        } else if (t_frame_header.id == "PCNT") {

            auto data = *prepareFrameData(t_handler, t_frame_header, t_position);

            // TODO Play counter can be bigger than 64 bits
            std::uint64_t play_counter = convert_bytes(data.data(), static_cast<std::uint32_t>(data.size()), false);

            log::info(fmt::format("Found an PCNT frame, setting play counter to: {}", play_counter));


            t_song.m_play_counter = play_counter;

        } else {
            log::warn(fmt::format("FrameID: {} is not supported yet, skipping frame...", t_frame_header.id));
        }

        // TODO TFLT (audio type, default is MPEG)
        // TODO MLLT (MPEG location lookup table (do I need this) (4.6), mentions player counter (4.16))
        // TODO ENCR and AENC frames?

        return true;
    }


}


FrameHeader ID3::readFrameHeader(Filehandler& t_handler, std::uint32_t& t_position, const bool t_syncsafe) noexcept {

    char buffer[SIZE_OF_HEADER]{};

    t_handler.readBytes(buffer, t_position, static_cast<std::uint32_t>(SIZE_OF_HEADER));

    t_position += SIZE_OF_HEADER;

    std::string frame_id;

    frame_id = {buffer[0], buffer[1], buffer[2], buffer[3]};


    const char size_bytes[4] = {buffer[4], buffer[5], buffer[6], buffer[7]};

    auto size = static_cast<std::uint32_t>(convert_bytes(size_bytes, SIZE_OF_SIZE, t_syncsafe));

    auto status_flags = static_cast<byte>(buffer[8]);
    auto format_flags = static_cast<byte>(buffer[9]);


    return {frame_id, size, status_flags, format_flags};
}


std::unique_ptr<std::vector<char>> ID3::readFrame(Filehandler& t_handler, std::uint32_t& t_position, const std::uint32_t t_bytes) noexcept {

    auto frame_content = std::make_unique<std::vector<char>>(t_bytes);

    t_handler.readBytes(frame_content->data(), t_position, t_bytes);

    // taking frame data in account when updating position
    t_position += t_bytes;

    return frame_content;
}


void ID3::readID3(Song& t_song) noexcept {

    Filehandler handler = Filehandler(t_song.m_path);

    if (detectID3(handler)) {

        // retrieve ID3 version
        auto version = getVersion(handler);

        log::info(fmt::format("ID3v2.{:d}", version));

        // TODO proceed with extracting metadata
        auto flags = getFlags(handler);

        // TODO size is without 10 bytes of header
        auto size = getSize(handler, false);

        log::info(fmt::format("ID3 Tag has {} bytes", size));

        std::uint32_t extended_size = 0;

        // Not supported ID3 version, skipping the tag
        // TODO Implement ID3v2.2 and below
        if (version != 4 && version != 3) {
            log::error(fmt::format("This software does not support ID3 version ID3v2.{:d}", version));

            // TODO set filepointer to after tag?
            // TODO if I'd do that, I'd have to know whether it contains extended headers though (for complete size)
        }

        else {

            // TODO synchronize
            if (flags & (1 << 7)) {
                log::debug("This tag is unsynchronised...");
            }

            // Extended header is present, skipping it...
            if (flags & (1 << 6)) {
                extended_size = getSize(handler, true);

                log::debug("This tag has an extended header. Skipping it for now...");

            }

            // TODO footer present (do I care?)
            // TODO probably need to tell the mp3 decoder to stop 10 bytes before EOF
            if (flags & (1 << 4)) {
                log::debug("This tag has a footer. Skipping it for now...");

                // size += 10;
                // don't do this, just set a footer bit to true or something
            }


            std::int64_t size_remaining = size + extended_size;

            std::uint32_t position = SIZE_OF_HEADER + extended_size;

            log::debug(fmt::format("Starting to read frames at position {}", position));

            while (size_remaining > 0) {
                log::info(fmt::format("{} bytes remaining...", size_remaining));
                log::info(fmt::format("{} > 0", size_remaining));
                log::info(fmt::format("Continuing to read at position: {}", position));

                // TODO I should probably choose less ambiguous names
                std::uint32_t original_position_file = position;
                auto frame_header = readFrameHeader(handler, position, version == 4);
                auto result = parseFrame(handler, frame_header, position, t_song);

                // There are no frames left, the rest is padding
                if (!result) {

                    log::debug("Result has no value, so read a frame_id starting with 0x00");

                    // this is alright because size_remaining > 0
                    position = static_cast<std::uint32_t>(size_remaining);
                    size_remaining = 0;
                }

                else {

                    if (frame_header.id == "PCNT") {

                        // setting position of start of play counter frame
                        t_song.m_counter_offset = original_position_file;

                    }

                    // TODO this seems wrong
                    // size_remaining -= (position - original_position_file);
                    size_remaining -= (position);

                    log::info(fmt::format("Size remaining: {}", size_remaining));


                    // frame_id has been not been set properly,
                    // still parsing the frame as I can't skip it
                    // without knowing it's size
                    // TODO is this ever empty?
                    if (frame_header.id.empty()) {
                        log::error("frame_id has not been set properly");
                    }


                }
            }

        }


    }

    else {
        log::debug(fmt::format("No ID3 Tag has been prepended to file: {}", t_song.m_path));
    }
}


