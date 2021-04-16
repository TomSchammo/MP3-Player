/******************************************************************************
* File:             song.hpp
*
* Author:           Tom Schammo
* Created:          26/01/2021
* Description:      Class wrapper for song data
*****************************************************************************/


#ifndef SONG_HPP
#define SONG_HPP

#include <picture.hpp>


/**
 * Class wrapper for song data.
 *
 * This class contains necessary information about a song like metadata,
 * a filepath, an offset to the start of the audio data, etc.
 *
 * Member variables:
 *  m_title:         The song title
 *  m_album:         The name of the album
 *  m_artist:        The name of the artist
 *  m_genre:         The name of the genre that the song belongs to
 *  m_release:       The year of when the song was released
 *  m_track_number:  The number of this track in the album
 *  m_duration:      The song duration in milliseconds
 *  m_delay:         The amount of silence that should be put in front of the track (in ms)
 *  m_path:          The path to the MP3 file
 *  m_audio_start:   An offset to the start of the actual audio data
 */
class Song
{
public:
    std::string m_title = "Unknown Title";
    std::string m_album = "Unknown Album";
    std::string m_artist = "Unknown Artist";
    std::string m_genre = "Unknown Genre";
    std::string m_release = "";
    std::string m_track_number = "";

    // TODO consider making these strings as well
    std::uint32_t m_audio_start = 0;
    std::uint64_t m_duration = 0;
    std::uint64_t m_delay = 0;

    std::uint64_t m_play_counter = 0;

    // offset of the player counter
    std::uint32_t m_counter_offset = 0;

    std::string m_path = "";

    std::vector<ID3::Picture> m_art{};

public:
    Song() = delete;
    Song(std::string& t_path);

    /**
     * Print information contained in song object to standard out.
     */
    void print();

    // TODO decrease play counter?
    virtual ~Song();

};

#endif /* ifndef SONG_HPP */
