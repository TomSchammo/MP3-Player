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
    constexpr byte SIZE_OF_FRAME_ID = 4;

    // TODO reconsider readID3 return type




    /**
     * Struct containing the data of a frame header
     *
     * id:              4 bytes containing the frame id as a null terminated string
     * size;            4 bytes containing the size of the tag as a syncsafe 32 bit integer
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
     * Converts data in a char buffer into a base 10 number that can be worked with.
     *
     * So {0, 0, 1, 63} ({0x00, 0x00, 0x01, 0x3f}) will be converted to 319.
     *
     * @param t_buffer   is a char array that contains the data
     * @param t_size     is the size of the char array
     *
     * @return The unsigned 64 bit base 10 representation of the number stored in the buffer
     */
     inline std::uint64_t convert_bytes(const char t_buffer[], std::uint32_t t_size) noexcept {

         std::uint64_t factor = 0;

         std::uint64_t number = 0;

         // going from last to first assuming that lsb is in the back
         for (std::int64_t i = t_size - 1; i >= 0; --i) {

             std::uint64_t n = static_cast<std::uint64_t>(t_buffer[i]) << factor;

             number |= n;

             // increasing the factor by 8 each time
             // as we are reading 8 bits at a time
             factor += 8;
         }

         return number;
     }

     /**
      * Reads the content of a frame header, converts the 4 byte ID to a null-terminated string and puts the 4 size bytes
      * into an unsigned 32 bit integer and saves that, along with the flags into a FrameHeader struct.
      *
      * @param t_handler    A reference to the file handler object for this file to read the frame header
      * @param t_position   A reference to the position of the file pointer so that it knows where to start reading the 10 bytes
      *
      * @return A FrameHeader struct containing the frame ID, the size, the status- and format flags of the current frame
      */
     FrameHeader readFrameHeader(Filehandler& t_handler, std::uint32_t& t_position) noexcept;


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
}

/*
 * Container for text read from a buffer, the current position in
 * the buffer (position of the end of the text) and an error flag.
 *
 * If the error flag is set to true, the text variable should contain
 * the error message and the position should be set to 0.
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
bool dectectID3(Filehandler& t_handler) noexcept;


/**
 * Checks whether ID3 metadata appended to the file.
 *
 * @param t_handler A reference to a Filehandler object to read from the file
 * @return true if an ID3 tag is prepended to the file, false otherwise
 */
bool detectID3Footer(Filehandler& t_handler) noexcept;


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
inline TextAndPositionContainer decode_text_retain_position(std::int8_t t_text_encoding,
                                                            std::vector<char> const& t_data,
                                                            std::uint32_t t_position) noexcept {

    char c = t_data.at(t_position++);

    std::string text;

    // Text is encoded using ISO-8859-1 standard
    // and using null terminated by 0x00,
    // (1 'zero' byte).
    if (t_text_encoding == 0x00) {

        logging::log<logging::LogLevel::DDEBUG>("Decoding ISO-8859-1 encoded text");

        while (c != 0x00 && t_position != t_data.size()) {
            text += c;
            c = t_data.at(t_position++);
        }

        // string is not null terminated
        if (c != 0x00) {

            logging::log<logging::LogLevel::WARNING>("String is not null terminated");

            text += c;

        }
    }

    // Text is UTF-16 endcoded Unicode with BOM.
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

        logging::log<logging::LogLevel::DDEBUG>("Decoding UTF-16 encoded Unicode");
        logging::log<logging::LogLevel::ERROR>("UTF-16 has not been implemented yet");

        char terminated = 0;

        // iterating until 2 consecutive 0x00 bytes are found
        while (terminated < 2) {

            terminated = c == 0x00 ? terminated + 1 : 0;

            c = data->at(position++);
        }

    }

    // The text UTF-16B encoded Unicode without BOM.
    // It is null terminated by 0x0000 (2 'zero' bytes)
    else if (t_text_encoding == 0x02) {

        logging::log<logging::LogLevel::DDEBUG>("Decoding UTF-16B encoded Unicode");
        logging::log<logging::LogLevel::ERROR>("UTF-16B has not been implemented yet");

        char terminated = 0;

        // iterating until 2 consecutive 0x00 bytes are found
        // TODO idk, this might work for common 1 byte characters
        //      but 2 byte characters will be wrong
        while (terminated < 2) {

            terminated = (c == 0x00 ? terminated + 1 : 0);

            c = t_data.at(t_position++);
        }
    }

    // The text is UTF-8 encoded Unicode.
    // It is terminated by 0x00 (1 'zero' byte)
    else if (t_text_encoding == 0x03) {

        logging::log<logging::LogLevel::DDEBUG>("Decoding UTF-8 encoded Unicode");

        while (c != 0x00 && t_position != t_data.size()) {
            text += c;
            c = t_data.at(t_position++);
        }

        // string is not null terminated
        if (c != 0x00) {

            logging::log<logging::LogLevel::WARNING>("String is not null terminated");

            text += c;

        }
    }

    // Value of text_encoding is not valid
    else {

        // TODO log error
        std::cout << "'0x" << std::hex << int(t_text_encoding) << "' is not a valid value for text_encoding" << std::endl;

        return {"0x" + std::to_string(int(t_text_encoding)) + " is not a valid encoding method", 0, true};
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

    auto result = decode_text_retain_position(t_text_encoding, t_data, t_position);

    if (!result.error)
        return result.text;

    // TODO deal with error
    else
        return nullptr;
}


/**
 * Converts data in a char buffer into a base 10 number that can be worked with.
 *
 * So {0, 0, 1, 63} ({0x00, 0x00, 0x01, 0x3f}) will be converted to 319.
 *
 * @param t_buffer   is a char array that contains the data
 * @param t_size     is the size of the char array
 *
 * @return The unsigned 64 bit base 10 representation of the number stored in the buffer
 */
inline std::uint64_t convert_bytes(const char t_buffer[], std::uint32_t t_size) noexcept {

    std::uint64_t factor = 0;

    std::uint64_t number = 0;

    // going from last to first assuming that lsb is in the back
    for (std::int64_t i = t_size - 1; i >= 0; --i) {

        std::uint64_t n = static_cast<std::uint64_t>(t_buffer[i]) << factor;

        number |= n;

        // increasing the factor by 8 each time
        // as we are reading 8 bits at a time
        factor += 8;
    }

    return number;
}


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
        // TODO log error
        std::cout << "t_size has to be a syncsafe integer" << std::endl;
    }


    else {

        for (std::uint8_t factor = 0; factor < 4; factor++) {
            auto n = static_cast<std::uint8_t>((t_size & (0x7f << (factor << 3))) >> (factor << 3));
            t_arr[factor] = n;
        }

    }
}


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
 * Increments the play counter.
 *
 * Reads the content of a PCNT frame and increases it by 1.
 *
 * @param t_handler  A reference to a Filehandler object to read/write to the file
 * @param t_position is the offset of the start of the frame relative to the start of the file
 */
void increment_pc(Filehandler& t_handler, std::uint32_t t_position) noexcept;


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
 * Function to extract ID3 encapsulated metadata from an mp3 file.
 *
 * @param t_song is a reference to a song object that represents the mp3 file.
 */
void readID3(Song& t_song) noexcept;


#endif // ID3_HPP
