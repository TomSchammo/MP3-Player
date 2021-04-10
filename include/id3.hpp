/******************************************************************************
* File:             id3.hpp
*
* Author:           Tom Schammo
* Created:          26/01/2021
* Description:      File responsible for dealing with ID3 tags.
*****************************************************************************/


#ifndef ID3_HPP
#define ID3_HPP

#include <optional>
#include <filehandler.hpp>
#include <song.hpp>
#include <iostream>
#include <string>

// constants
constexpr std::uint8_t LOCATION_START = 0;
constexpr std::uint8_t LOCATION_VERSION = 3;
constexpr std::uint8_t LOCATION_FLAGS = 5;
constexpr std::uint8_t LOCATION_SIZE = 6;
constexpr std::uint8_t SIZE_OF_FLAGS = 1;
constexpr std::uint8_t SIZE_OF_HEADER = 10;

// TODO Only reading major, don't care about revision just now
constexpr std::uint8_t SIZE_OF_VERSION = 1;

constexpr std::uint8_t SIZE_OF_SIZE = 4;
constexpr std::uint8_t SIZE_OF_FRAME_ID = 4;

// TODO reconsider readID3 return type
// TODO consider wrapping everything in namespace

struct TextAndPositionContainer {

    std::string text;
    std::uint32_t position;
    bool error;
};



/**
 * Checks whether ID3 metadata prepended to the file.
 *
 * @param handler A reference to a Filehandler object to read from the file
 * @return true if an ID3 tag is prepended to the file, false otherwise
 */
bool dectectID3(Filehandler &handler);


/**
 * Checks whether ID3 metadata appended to the file.
 *
 * @param handler A reference to a Filehandler object to read from the file
 * @return true if an ID3 tag is prepended to the file, false otherwise
 */
bool detectID3Footer(Filehandler &handler);


/**
 * Reads the byte that contains the version of the ID3 Tag
 *
 * @param handler A reference to a Filehandler object to read from the file
 * @return a byte that contains the ID3 major version
 */
std::uint8_t getVersion(Filehandler &handler);


/**
 * Reads the byte that contains the flags in the ID3 header, and returns it
 *
 * @param handler A reference to a Filehandler object to read from the file
 * @return a byte that contains the flags of the ID3 header
 */
std::uint8_t getFlags(Filehandler &handler);


/**
 * Reads the 4 bytes that contain the size of the ID3 tag (without the header and the footer) or the extended header.
 *
 * @param handler A reference to a Filehandler object to read from the file
 * @return 4 bytes that contain the size of the ID3 tag or the extended header
 */
std::uint32_t getSize(Filehandler &handler, const bool extended);


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
 * message instead of the decoded text and a posistion of 0.
 *
 * @param text_encoding The method that is used to encode the text
 * @param text          A pointer to a std::vector containing the text
 * @param position      An unsigned 32 bit integer indicating the start of the string
 *
 * @return a container struct that contains the decoded text and the updated value of
 *         the position argument as well as an error flag that should be false.
 */
inline TextAndPositionContainer decode_text_retain_position(std::uint8_t text_encoding, std::shared_ptr<std::vector<char>> data, std::uint32_t position) {

    char c = data->at(position++);

    std::string text = "";

    // Text is encoded using ISO-8859-1 standard
    // and using null terminated by 0x00,
    // (1 'zero' byte).
    if (text_encoding == 0x00) {

        // TODO log debug
        std::cout << "Decoding ISO-8859-1 encoded text" << std::endl;

        while (c != 0x00) {
            text += c;
            c = data->at(position++);
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
    else if (text_encoding == 0x01) {

        // TODO log debug
        std::cout << "Decoding UTF-16 encoded Unicode" << std::endl;

        std::cout << "[Error] UTF-16 has not been implemented yet" << std::endl;

        char terminated = 0;

        // iterating until 2 consecutive 0x00 bytes are found
        while (terminated < 2) {

            terminated = c == 0x00 ? terminated + 1 : 0;

            c = data->at(position++);
        }
    }

    // The text UTF-16B encoded Unicode without BOM.
    // It is null terminated by 0x0000 (2 'zero' bytes)
    else if (text_encoding == 0x02) {

        // TODO log debug
        std::cout << "Decoding UTF-16B encoded Unicode" << std::endl;

        std::cout << "[Error] UTF-16B has not been implemented yet" << std::endl;

        char terminated = 0;

        // iterating until 2 consecutive 0x00 bytes are found
        while (terminated < 2) {

            terminated = c == 0x00 ? terminated + 1 : 0;

            c = data->at(position++);
        }
    }

    // The text is UTF-8 encoded Unicode.
    // It is terminated by 0x00 (1 'zero' byte)
    else if (text_encoding == 0x03) {

        // TODO log debug
        std::cout << "Decoding UTF-8 encoded Unicode" << std::endl;

        while (c != 0x00) {
            text += c;
            c = data->at(position++);
        }
    }

    // Value of text_encoding is not valid
    else {

        // TODO log error
        std::cout << "'0x" << std::hex << int(text_encoding) << "' is not a valid value for text_encoding" << std::endl;

        return {"0x" + std::to_string(int(text_encoding)) + " is not a valid encoding method", 0, true};
    }

    return {text, position, false};

}


inline std::string decode_text(std::uint8_t text_encoding, std::shared_ptr<std::vector<char>> data, std::uint32_t position) {

    auto result = decode_text_retain_position(text_encoding, data, position);

    if (!result.error)
        return result.text;

    // TODO deal with error
    else
        return nullptr;
}


/**
 * Converts data in a char buffer into a base 10 number that can be worked with.
 *
 * So {0, 0, 1, 63} (coming from {0x00, 0x00, 0x01, 0x3f}) will be converted to 319.
 *
 * @param buffer   is a char array that contains the data
 * @param size     is the size of the char array
 * @param syncsafe is true if the data should be converted into a syncsafe integer
 *
 * @return The unsigned 64 bit base 10 representation of the number stored in the buffer
 */
constexpr inline std::uint64_t convert_bytes(char buffer[], std::uint32_t size, bool syncsafe) {

    std::uint64_t factor = 0;

    std::uint64_t number = 0;

    // going from last to first assuming that lsb is in the back
    for (std::int32_t i = size - 1; i >= 0; --i) {

        auto n = buffer[i] << factor;

        if (syncsafe)
            n = (n & (0x7f << (factor >> 0))) >> (factor >> 3);

        number |= n;

        // increasing the factor by 2 each time
        factor += 8;
    }

    return number;
}


/**
 * Splits a decimal number into byte sized chunks and puts them in a vector.
 *
 * A unique_ptr to that vector is then returned.
 *
 * @param number is the number that should be converted
 * @return a std::unique_ptr to a std::vector that contains the bytes
 */
inline std::unique_ptr<std::vector<char>> convert_dec(std::uint64_t number) {

    std::unique_ptr<std::vector<char>> bytes = std::make_unique<std::vector<char>>();

    while (number > (16*16)) {
        std::uint8_t byte = number % (16*16);
        bytes->insert(bytes->begin(), static_cast<char>(byte));

        number = number >> 8;
    }

    return bytes;
}


/**
 * Converts an integer into 4 separate bytes with the msb beeing a 0.
 *
 * So 128 will be converted to 0b00000000, 0b00000000, 0b00000001, 0b01111111
 * for example.
 *
 * @param size is the integer that will be converted
 * @param arr  is an array of std::uint8_ts with a length of 4 that will be filled with the bytes
 */
inline void convert_size(std::uint32_t size, char arr[4]) {

    int i = 0;
    while (size > 127) {
        arr[i] = 127;
        size -= 127;
        ++i;
    }

    arr[i+1] = static_cast<char>(size);
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
 * @param  data The data that is supposed to be synchronized
 * @return A pointer to the data that has been synchronized
 */
void synchronize(const unsigned char* data, std::uint32_t size);


/**
 * Increments the play counter.
 *
 * Reads the content of a PCNT frame and increases it by 1.
 *
 * @param handler  A reference to a Filehandler object to read/write to the file
 * @param position is the offset of the start of the frame relative to the start of the file
 */
void increment_pc(Filehandler &handler, std::uint32_t position);


/**
 * Reads the content of a frame and returns the data (the whole frame minus the header),
 * as a shared pointer to a vector that contains the raw bytes,
 * so that it can be parsed by another function.
 *
 * @param handler          A reference to a Filehandler object to read from the file
 * @param frame_id         A reference to a string that will be set to the frame id
 * @param position         The starting position of the frame in the file
 *
 * @return a shared pointer to a vector that contains the data of the frame
 */
std::optional<std::shared_ptr<std::vector<char>>> readFrame(Filehandler &handler, std::string &frame_id, std::uint32_t &position);


/**
 * Parses the data from an ID3 frame, and saves it to the respective member
 * variable of the Song object included in in the parameters.
 *
 * See: {@link https://id3.org/id3v2.4.0-frames} for all frames
 *
 * @param data     The data of the ID3 frame
 * @param frame_id The frameID of the frame
 * @param song     A reference to a {@class Song} object
 */
void parseFrameData(std::unique_ptr<std::vector<char>> data, std::string frame_id, Song &song);


/**
 * Function to extract ID3 encapsulated metadata from an mp3 file.
 *
 * @param song is a reference to a song object that represents the mp3 file.
 */
void readID3(Song &song);


#endif // ID3_HPP
