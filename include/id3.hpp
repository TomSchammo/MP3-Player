/******************************************************************************
* File:             id3.hpp
*
* Author:           Tom Schammo
* Created:          26/01/2021
* Description:      File responsible for dealing with ID3 tags.
*****************************************************************************/


#ifndef ID3_HPP
#define ID3_HPP

#include <array>
#include <bits/c++config.h>
#include <filehandler.hpp>
#include <song.hpp>
#include <iostream>
#include <log.hpp>
#include <fmt/format.h>


namespace ID3 {

    using byte = std::uint8_t;


    // constants
    constexpr byte LOCATION_START = 0;
    constexpr byte LOCATION_VERSION = 3;
    constexpr byte LOCATION_FLAGS = 5;
    constexpr byte LOCATION_SIZE = 6;
    constexpr byte SIZE_OF_FLAGS = 1;
    constexpr byte SIZE_OF_HEADER = 10;

    // TODO Only reading major, don't care about revision just now
    constexpr byte SIZE_OF_VERSION = 1;

    constexpr byte SIZE_OF_SIZE = 4;

    // TODO reconsider readID3 return type




    /**
     * Struct containing the data of a frame header
     *
     * id:              4 bytes containing the frame id as a null terminated string
     * size;            4 bytes containing the size of the tag as an unsigned, syncsafe 32 bit integer
     * status_flags:    One byte containing the frame status flags where msb and the 4 lower bits are always set to 0 (0b0xx0000)
     * format_flags:    One byte containing the frame format flags with the msb and the 5th and 6th bit always set to 0 (0b0x00xxxx)
     *
     */
    struct FrameHeader {
        const std::string id;
        const std::uint32_t size;
        const byte status_flags;
        const byte format_flags;
    };


    /**
     * Container for text read from a buffer, the current position in
     * the buffer (position of the end of the text) and an error flag.
     *
     * If the error flag is set to true, the text variable should contain
     * the error message and the position should be set to 0.
     *
     * text:     The text extracted from said buffer
     * position; Position at the end of the text
     * bool:     true if there was an error when decoding, false otherwise
     */
    struct TextAndPositionContainer {

        std::string text;
        std::uint32_t position;
        bool error;
    };


    /**
     * Checks whether ID3 metadata prepended to the file.
     *
     * @param t_handler A reference to a Filehandler object to read from the file
     * @return true if an ID3 tag is prepended to the file, false otherwise
     */
    bool detectID3(Filehandler& t_handler) noexcept;


    /**
     * Reads the byte that contains the version of the ID3 Tag
     *
     * @param t_handler A reference to a Filehandler object to read from the file
     * @return a byte that contains the ID3 major version
     */
    std::uint8_t getVersion(Filehandler& t_handler) noexcept;


    /**
     * Reads the byte that contains the flags in the ID3 header, and returns it
     *
     * @param t_handler A reference to a Filehandler object to read from the file
     * @return a byte that contains the flags of the ID3 header
     */
    std::uint8_t getFlags(Filehandler& t_handler) noexcept;


    /**
     * Reads the 4 bytes that contain the size of the ID3 tag (without the header and the footer) or the extended header.
     *
     * @param t_handler  A reference to a Filehandler object to read from the file
     * @param t_extended A boolean indicating whether I'm looking for the size of a standard or extended header
     *
     * @return 4 bytes that contain the size of the ID3 tag or the extended header
     */
    std::uint32_t getSize(Filehandler& t_handler, const bool t_extended) noexcept;


    /**
     * Converts data in a char buffer into a base 10 number that can be worked with.
     *
     * So {0, 0, 1, 63} ({0x00, 0x00, 0x01, 0x3f}) will be converted to 319 in case of normal encoding.
     *
     * If a synchsafe encoding is used the same will be converted to 0b10111111 which equals 191 in decimal.
     *
     * @param t_buffer   is a char array that contains the data
     * @param t_size     is the size of the char array
     * @param t_syncsafe indicates whether synchsafe encoding used or not
     *
     * @return The unsigned 64 bit base 10 representation of the number stored in the buffer
     */
    inline std::uint64_t convert_bytes(const char t_buffer[], std::uint32_t t_size, const bool t_syncsafe) noexcept {

        std::vector<byte> byte_buffer{};
        byte_buffer.reserve(t_size);

        // I have to do this, because I can't read the contents of a file as unsigned chars
        // but working with signed chars will fuck up conversions, so I have to convert it all to
        // an unsigned representation and I have to accept the overhead.
        for (std::uint_fast32_t i = 0; i < t_size; ++i) {
           byte_buffer.push_back(static_cast<byte>(t_buffer[i]));
        }


        std::uint_fast8_t factor = 0;
        std::uint64_t number = 0;

        // going from last to first assuming that lsb is in the back
        for (std::int64_t i = t_size - 1; i >= 0; --i) {

            std::uint64_t n = static_cast<std::uint64_t>(byte_buffer[i]) << factor;

            number |= n;

            // increasing the factor by 8 each time
            // as we are reading 8 bits at a time
            // unless the integer is synchsafe, then
            // the msb of every byte is omitted, so we
            // only increase the factor by 7 bits.
            factor += t_syncsafe ? 7 : 8;
        }

        log::debug(fmt::format("Converted bytes {:#04x} to {}", fmt::join(byte_buffer, ", "), number));

        return number;
    }


    /**
     * Decodes text according to the specified encoding and returns
     * a string containing said text.
     *
     * The method used to encode the text is supplied to the function
     * as a parameter as 1 byte.
     *
     * That byte can have the following values:
     *
     * 0x00:
     *  The ISO-8859-1 standard is used to encode the text, and
     *  the string is terminated by one 0x00 byte.
     *
     * 0x01:
     *  The text contains UTF-16 encoded Unicode with BOM.
     *  If the BOM bytes are ff ff, the byte order is big endian.
     *  If the BOM bytes are ff fe, the byte order is little endian.
     *
     *  The string is null terminated by 00 00 bytes.
     *
     * 0x02:
     *  The text contains UTF-16B encoded Unicode without BOM and
     *  the string is null terminated by 00 00 bytes.
     *
     * 0x03:
     *  The text contains UTF-8 encoded Unicode and is terminated
     *  by one 0x00 byte.
     *
     *
     * If the value of that byte is not valid (meaning none of the above)
     * a flag indicating an error will be set to true along with an error
     * message instead of the decoded text and a position of 0.
     *
     * @param t_text_encoding The method that is used to encode the text
     * @param t_data          A const l-value reference to a std::vector containing the bytes of the text
     * @param t_position      An unsigned 32 bit integer indicating the start of the string
     *
     * @return a container struct that contains the decoded text and the updated value of
     *         the position argument as well as an error flag that should be false.
     */
    inline ID3::TextAndPositionContainer decode_text_retain_position(std::int8_t t_text_encoding,
                                                                     std::vector<char> const& t_data,
                                                                     std::uint32_t t_position) noexcept {

        char c = t_data.at(t_position++);

        std::string text;

        // Text is encoded using ISO-8859-1 standard
        // and using null terminated by 0x00,
        // (1 'zero' byte).
        if (t_text_encoding == 0x00) {

            log::debug("Decoding ISO-8859-1 encoded text");

            while (c != 0x00 && t_position != t_data.size()) {
                text += c;
                c = t_data.at(t_position++);
            }

            // string is not null terminated
            if (c != 0x00) {

                log::warn("String is not null terminated");

                text += c;

            }
        }

            // Text is UTF-16 encoded Unicode with BOM.
            // The first text byte is 0xff followed by either
            // another 0xff byte or one 0xfe byte.
            //
            // If the second byte is 0xff the byte order is
            // big endian.
            // If the second byte is 0xfe the byte order is
            // little endian.
            //
            // The text is null terminated by 0x0000
            // (2 'zero' bytes).
        else if (t_text_encoding == 0x01) {

            log::debug("Decoding UTF-16 encoded Unicode");
            log::error("UTF-16 has not been implemented yet");

            char terminated = 0;

            if (static_cast<std::uint8_t>(c) != 0xff) {
                log::error(fmt::format("Text data is supposed to be UTF-16 encoded Unicode with BOM, but BOM does not appear to be present.\n Expected 0xff, found {:#04x}", c));

                return {"BOM missing in UTF-16 encoded Unicode", 0, true};
            }

            else {

                c = t_data.at(t_position++);

                // big endian
                if (static_cast<std::uint8_t>(c) == 0xff) {

                    log::info("Byte order is big endian");

                }

                    // little endian
                else if (static_cast<std::uint8_t>(c) == 0xfe) {

                    log::info("Byte order is big endian");
                }

                else {

                    log::error("Text data is supposed to be UTF-16 encoded Unicode with BOM, but BOM does not appear to be present.");
                    log::error(fmt::format("First byte was 0xff, but expected 0xff or 0xfe for the second byte, found {:#04x}", c));

                    return {"Second byte of BOM missing in UTF-16 encoded Unicode", 0, true};

                }

                text = "UTF-16 has not been implemented yet";

                // iterating until 2 consecutive 0x00 bytes are found
                while (terminated < 2 and t_position < t_data.size()) {
                    terminated = (c == 0x00 ? terminated + 1 : 0);
                    c =  t_data.at(t_position);
                    // log::info(fmt::format("Read byte {:#04x} at position {}/{}", c, t_position, t_data.size()));
                    t_position++;
                }

                if (terminated < 2)
                    log::warn("UTF-16 byte string is not 00 00 terminated");

            }

        }

            // The text UTF-16B encoded Unicode without BOM.
            // It is null terminated by 0x0000 (2 'zero' bytes)
        else if (t_text_encoding == 0x02) {

            log::debug("Decoding UTF-16B encoded Unicode");
            log::error("UTF-16B has not been implemented yet");

            char terminated = 0;

            // iterating until 2 consecutive 0x00 bytes are found
            while (terminated < 2 and t_position < t_data.size()) {
                terminated = (c == 0x00 ? terminated + 1 : 0);
                c =  t_data.at(t_position);
                // log::info(fmt::format("Read byte {:#04x} at position {}/{}", c, t_position, t_data.size()));
                t_position++;
            }

            if (terminated < 2)
                log::warn("UTF-16 byte string is not 00 00 terminated");

            text = "UTF-16B has not been implemented yet";
        }

            // The text is UTF-8 encoded Unicode.
            // It is terminated by 0x00 (1 'zero' byte)
        else if (t_text_encoding == 0x03) {

            log::debug("Decoding UTF-8 encoded Unicode");

            while (c != 0x00 && t_position != t_data.size()) {
                text += c;
                c = t_data.at(t_position++);
            }

            // string is not null terminated
            if (c != 0x00) {

                log::warn("String is not null terminated");

                text += c;

            }
        }

            // Value of text_encoding is not valid
        else {

            std::string message = fmt::format("{:#04x} is not a valid value for text_encoding", t_text_encoding);

            log::error(message);

            return {message, 0, true};
        }

        return {text, t_position, false};

    }


    /**
     * A wrapper for decode_text_retain_position for when the position is actually not of any interest after
     * the function finished executing.
     *
     *
     * @param t_text_encoding The method that is used to encode the text
     * @param t_data          A const l-value reference to a std::vector containing the bytes of the text
     * @param t_position      An unsigned 32 bit integer indicating the start of the string
     *
     * @return a std::string containing the text, nullptr if there is an error (for now) TODO pls fix
     */
    inline std::string decode_text(std::int8_t t_text_encoding, std::vector<char> const& t_data, std::uint32_t t_position) noexcept {

        auto result = ID3::decode_text_retain_position(t_text_encoding, t_data, t_position);

        if (!result.error)
            return result.text;

            // TODO deal with error
        else {
            std::cout << "Got an error in decode_text" << std::endl;

            return nullptr;
        }
    }


    /**
     * Reads the content of a frame header, converts the 4 byte ID to a null-terminated string and puts the 4 size bytes
     * into an unsigned 32 bit integer and saves that, along with the flags into a FrameHeader struct.
     *
     * @param t_handler    A reference to the file handler object for this file to read the frame header
     * @param t_position   A reference to the position of the file pointer so that it knows where to start reading the 10 bytes
     * @param t_syncsafe   True if the size is syncsafe (so if the frame is an ID3v2.4 frame), false otherwise
     *
     * @return A FrameHeader struct containing the frame ID, the size, the status- and format flags of the current frame
     */
    FrameHeader readFrameHeader(Filehandler& t_handler, std::uint32_t& t_position, const bool t_syncsafe) noexcept;


    /**
     * The readFrame function is called to read the contents of a frame.
     *
     * After that that data is prepared to be parsed. This includes synchronizing it if has been desynchronized,
     * decompressing it if it has been compressed and decrypting it if it has been encrypted.
     *
     * TODO decompression has not yet been implemented
     * TODO decryption has not yet been implemented
     *
     *
     * @param t_handler        A reference to the file handler object for this file to pass it on to the readFrame function
     * @param t_frame_header   A reference to the frame header struct for this frame
     * @param t_position       A reference to the position of the file pointer to pass it on the readFrame function
     *
     * @return A std::unique_ptr of a std::vector<char> containing the 'prepared' data
     */
    std::unique_ptr<std::vector<char>> prepareFrameData(Filehandler& t_handler, FrameHeader& t_frame_header, std::uint32_t& t_position) noexcept;


    /**
     * Reads the content of a frame and returns the data (the whole frame minus the header),
     * as a unique pointer to a vector that contains the raw bytes,so that it can be parsed by another function.
     *
     * @param t_handler          A reference to a Filehandler object to read from the file
     * @param t_position         The starting position of the frame in the file
     * @param t_bytes            The amount of bytes that should be read
     *
     * @return a unique pointer to a vector that contains the data of the frame or a nullptr
     */
    std::unique_ptr<std::vector<char>> readFrame(Filehandler& t_handler, std::uint32_t& t_position, const std::uint32_t t_bytes) noexcept;


    /**
     * Checks if the frame ID is valid.
     *
     * If it is valid, it proceeds by calling the prepareFrameData function, which calls the the readFrame function,
     * reads the frame data, and edits the data buffer so that it can be parsed (synchronization and decompression are
     * examples of things that need to be done before the data can be read.
     * After that the data is parsed and saved in the respective member variable in the Song object.
     *
     * If it is not, there has either been a mistake, or there is only padding left.
     *
     * See: {@link https://id3.org/id3v2.4.0-frames} for all frames
     *
     * @param t_handler        A reference to the file handler object for this file to pass it on to the readFrame function
     * @param t_frame_header   A reference to the frame header struct for this frame
     * @param t_position       A reference to the position of the file pointer to pass it on the readFrame function
     * @param t_song           A reference to the current song object to set the song data
     *
     * @return true if the frame is not padding frame, false if it is
     */
    bool parseFrame(Filehandler& t_handler, FrameHeader& t_frame_header, std::uint32_t& t_position, Song& t_song) noexcept;


    /**
     * Synchronizes unsynchronized data.
     *
     * When applying unsynchronization, a 0x00 is inserted after every 0xff byte.
     * This function reverses that scheme, so if there is a 0x00 byte after a 0xff,
     * it is removed.
     *
     *
     * See https://id3.org/id3v2.4.0-structure section 6.1 for more information.
     *
     * @param  t_data A reference to a std::unique_ptr of an std::vector<char> with data that is supposed to be synchronized
     */
    void synchronize(std::vector<char>& t_data) noexcept;


    /**
     * Function to extract ID3 encapsulated metadata from an mp3 file.
     *
     * @param t_song is a reference to a song object that represents the mp3 file.
     */
    void readID3(Song& t_song) noexcept;
}


/**
 * Checks whether ID3 metadata appended to the file.
 *
 * @param t_handler A reference to a Filehandler object to read from the file
 * @return true if an ID3 tag is prepended to the file, false otherwise
 */
bool detectID3Footer(Filehandler& t_handler) noexcept;


/**
 * Splits a decimal number into byte sized chunks and puts them in a vector.
 *
 * A unique_ptr to that vector is then returned.
 *
 * TODO what about syncsafety???
 * TODO can I reserve space beforehand?
 *
 * @param t_number is the number that should be converted
 * @return a std::unique_ptr to a std::vector that contains the bytes
 */
inline std::unique_ptr<std::vector<char>> convert_dec(std::uint64_t t_number) noexcept {

    std::unique_ptr<std::vector<char>> bytes = std::make_unique<std::vector<char>>();

    while (t_number > (16*16)) {
        auto byte = static_cast<std::uint8_t>(t_number % (16*16));
        bytes->insert(bytes->begin(), static_cast<char>(byte));

        t_number = t_number >> 8;
    }

    return bytes;
}


/**
 * Converts an integer into 4 separate bytes with the msb being a 0.
 *
 * So 128 will be converted to 0b00000000, 0b00000000, 0b00000001, 0b01111111
 * for example.
 *
 * @param t_size is the integer that will be converted
 * @param t_arr  is an array of std::uint8_ts with a length of 4 that will be filled with the bytes
 */
inline void convert_size(std::uint32_t t_size, std::array<std::uint8_t, 4>& t_arr) noexcept {

    if (t_size <= 2139062143){
        log::error("t_size has to be a syncsafe integer");
    }


    else {

        for (std::uint8_t factor = 0; factor < 4; factor++) {
            auto n = static_cast<std::uint8_t>((t_size & (0x7f << (factor << 3))) >> (factor << 3));
            t_arr[factor] = n;
        }

    }
}


/**
 * Increments the play counter.
 *
 * Reads the content of a PCNT frame and increases it by 1.
 *
 * @param t_handler  A reference to a Filehandler object to read/write to the file
 * @param t_position is the offset of the start of the frame relative to the start of the file
 */
void increment_pc(Filehandler& t_handler, std::uint32_t t_position) noexcept;


#endif // ID3_HPP
