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
std::uint16_t getSize(Filehandler &handler, const bool extended);


/**
 * Converts data in a char buffer into a number that one can work with.
 *
 * So {0, 0, 1, 63} (coming from {0x00, 0x00, 0x01, 0x3f}) will be converted to 319.
 *
 * @param buffer is a char array that contains the data
 * @param size   is the size of the char array
 *
 * @return The unsigned 64 bit base 10 representation of the number stored in the buffer
 */
inline std::uint64_t convert_bytes(char buffer[], std::uint16_t size) {

    std::uint8_t factor = 1;

    std::uint64_t number = 0;

    // going from last to first assuming that lsb is in the back
    for (std::int32_t i = size - 1; i >= 0; --i) {

        number += buffer[i] * factor;

        // increasing the factor by 2 each time
        factor *= (16*16);
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
inline void convert_size(std::uint16_t size, char arr[4]) {

    int i = 0;
    while (size > 127) {
        arr[i] = 127;
        size -= 127;
        ++i;
    }

    arr[i+1] = size;
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
void synchronize(const unsigned char* data, std::uint16_t size);


/**
 * Increments the play counter.
 *
 * Reads the content of a PCNT frame and increases it by 1.
 *
 * @param handler  A reference to a Filehandler object to read/write to the file
 * @param position is the offset of the start of the frame relative to the start of the file
 */
void increment_pc(Filehandler &handler, std::uint16_t position);


/**
 * Reads the content of a frame and returns the data (the whole frame minus the header),
 * as a null terminated string, so that it can be parsed by another function.
 *
 * @param handler          A reference to a Filehandler object to read from the file
 * @param position         The starting position of the frame in the file
 * @param frame_id         A reference to a string that will be set to the frame id
 * @param frame_data_size  The size of the frame data
 *
 * @return The data of the frame as a null terminated string
 */
std::optional<std::string> readFrameStr(Filehandler &handler, std::uint16_t position, std::string &frame_id, std::uint16_t &frame_data_size);


/**
 * Reads the content of a frame and returns the data (the whole frame minus the header),
 * as binary data (char array), so that it can be parsed by another function.
 *
 * @param handler          A reference to a Filehandler object to read from the file
 * @param position         The starting position of the frame in the file
 * @param frame_id         A reference to a string that will be set to the frame id
 * @param frame_data_size  The size of the frame data
 *
 * @return The data of the frame as a char array
 */
std::optional<std::unique_ptr<char>> readFrameBytes(Filehandler &handler, std::uint16_t position, std::string &frame_id, std::uint16_t &frame_data_size);


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
void parseFrameData(std::string data, std::string frame_id, Song &song);


/**
 * Function to extract ID3 encapsulated metadata from an mp3 file.
 *
 * @param song is a reference to a song object that represents the mp3 file.
 */
void readID3(Song &song);


#endif // ID3_HPP
