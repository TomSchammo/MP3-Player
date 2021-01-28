/******************************************************************************
* File:             song.hpp
*
* Author:           Tom Schammo
* Created:          26/01/2021
* Description:      Class wrapper for song data
*****************************************************************************/


#ifndef SONG_HPP
#define SONG_HPP

#include <cstdint>
#include <string>

class song
{
public:
    std::string title = "Unknown Title";
    std::string album = "Unknown Album";
    std::string artist = "Unknown Artist";
    std::string genre = "Unknown Genre";
    std::uint16_t release = 0;
    std::uint16_t duration = 0;
    std::string path;

public:
    song ();
    virtual ~song ();

};

#endif /* ifndef SONG_HPP */
