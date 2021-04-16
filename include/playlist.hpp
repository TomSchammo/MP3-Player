/******************************************************************************
* File:             playlist.hpp
*
* Author:           Tom Schammo
* Created:          26/01/2021
* Description:      File that contains code to deal with playlists
*****************************************************************************/

#ifndef PLAYLIST_HPP
#define PLAYLIST_HPP

#include <song.hpp>

namespace Playlist {


    /**
     * Function to read an m3u file and construct a std::vector containing the songs
     *
     * @param t_filename The name of the playlist
     * @param t_songlist A vector that is to be filled with the content of the playlist
     */
    void readM3U(const char* t_filename, std::vector<Song>& t_songlist);


    /**
     * Function to shuffle a playlist based on the modern Fisher-Yates algorithm.
     * The algorithm has O(n) time complexity.
     *
     * @param t_playlist The playlist that is supposed to be shuffled
     */
    void shuffle(std::vector<Song>& t_playlist);
}

#endif /* ifndef PLAYLIST_HPP */
