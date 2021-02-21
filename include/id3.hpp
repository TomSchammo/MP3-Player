/******************************************************************************
* File:             id3.hpp
*
* Author:           Tom Schammo
* Created:          26/01/2021
* Description:      File responsible for dealing with ID3 tags.
*****************************************************************************/


#ifndef ID3_HPP
#define ID3_HPP

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
 * @return a byte that contains the flags of the ID3 header.
 */
std::uint8_t getFlags(Filehandler &handler);


/**
 * Reads the 4 bytes that contain the size of the ID3 tag (without the header and the footer) or the extended header.
 *
 * @param handler A reference to a Filehandler object to read from the file
 * @return 4 bytes that contain the size of the ID3 tag or the extended header.
 */
std::uint16_t getSize(Filehandler &handler, const bool extended);

/**
 * Synchronizes unsynchronized Data
 *
 * @param  data The data that is supposed to be synchronized
 * @return A pointer to the data that has been synchronized
 */
void synchronize(char* data, std::uint16_t size);

/**
 * Reads the content of a frame and returns the data (the whole frame minus the header)
 * so that it can be parsed by another function.
 *
 * @param handler  A reference to a Filehandler object to read from the file
 * @param position The starting position of the frame in the file
 * @param frame_id A reference to a string that will be set to the frame id
 *
 * @return A pointer to a array of data (with each element containing 1 byte)
 */
std::string readFrame(Filehandler &handler, std::uint16_t position, std::string &frame_id, std::uint16_t &bytes_read);


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
