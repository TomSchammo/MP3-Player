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

// constants
constexpr unsigned char LOCATION_START = 0;
constexpr unsigned char LOCATION_VERSION = 3;
constexpr unsigned char LOCATION_FLAGS = 5;
constexpr unsigned char LOCATION_SIZE = 6;
constexpr unsigned char LOCATION_EXTENDED_HEADER = 10;
constexpr unsigned char SIZE_OF_FLAGS = 1;
constexpr unsigned char SIZE_OF_VERSION = 1;
constexpr unsigned char SIZE_OF_SIZE = 4;
constexpr unsigned char SIZE_OF_FRAME_ID = 4;

// TODO reconsider readID3 return type
// TODO consider wrapping everything in namespace

/**
 * Enum containing every frame ID specified in the documentation for
 * ID3v2.4 Frames. See: https://id3.org/id3v2.4.0-frames
 */
enum class FrameID {
    AENC, APIC, ASPI, COMM, COMR, ENCR, EQU2, ETCO, GEOB, GRID,
    LINK, MCDI, MLLT, OWNE, PRIV, PCNT, POPM, POSS,
    RBUF, RVA2, RVRB, SEEK, SIGN, SYLT, SYTC,
    TALB, TBPM, TCOM, TCON, TCOP, TDEN, TDLY, TDOR, TDRC, TDRL,
    TDTG, TENC, TEXT, TFLT, TIPL, TIT1, TIT2, TIT3, TKEY, TLAN,
    TLEN, TMCL, TMED, TMOO, TOAL, TOFN, TOLY, TOPE, TOWN, TPE1,
    TPE2, TPE3, TPE4, TPOS, TPRO, TPUB, TRCK, TRSN, TRSO, TSOA,
    TSOP, TSOT, TSRC, TSSE, TSST, TXXX, UFID, USER, USLT,
    WCOM, WCOP, WOAF, WOAR, WOAS, WORS, WPAY, WPUB, WXXX
};

/**
 * Struct to hold the necessary ID3 metadata that the player is
 * supposed to display.
 * It also contains information that is important for decoding/playback.
 */
typedef struct {
    std::string title;
    std::string album;
    std::string artist;
    unsigned long length;
    unsigned int delay;
    char* picture;

} ID3Tag;

/**
 * Checks whether ID3 metadata prepended to the file.
 *
 * @param handler A Filehandler object to read from the file
 * @return true if an ID3 tag is prepended to the file, false otherwise
 */
bool dectectID3(Filehandler &handler);


/**
 * Checks whether ID3 metadata appended to the file.
 *
 * @param handler A Filehandler object to read from the file
 * @return true if an ID3 tag is prepended to the file, false otherwise
 */
bool detectID3Footer(Filehandler &handler);
unsigned char getVersion(Filehandler &handler);
unsigned char getFlags(Filehandler &handler);
void readFrame(Filehandler &handler, unsigned char position);
void parseFrameData(char* data, FrameID frameID);
unsigned int getSize(Filehandler &handler, const bool extended);
void readID3(const char* name);


#endif // ID3_HPP
