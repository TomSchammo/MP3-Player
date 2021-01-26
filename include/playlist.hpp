/******************************************************************************
* File:             playlist.hpp
*
* Author:           Tom Schammo
* Created:          26/01/2021
* Description:      File that contains code to deal with playlists
*****************************************************************************/

#ifndef PLAYLIST_HPP
#define PLAYLIST_HPP

#include <vector>
#include <song.hpp>

namespace Playlist {


    /**
     * Function to read an m3u file and construct a std::vector containing the songs
     *
     * @param filename The name of the playlist
     * @param songlist A vector that is to be filled with the content of the playlist
     */
    void readM3U(const char* filename, std::vector<song> &songlist);


    /**
     * Function to shuffle a playlist based on modern Fisher-Yates algorithm.
     *
     * @param playlist The playlist that is supposed to be shuffled
     */
    void shuffle(std::vector<song> &playlist);
}

#endif /* ifndef PLAYLIST_HPP */
