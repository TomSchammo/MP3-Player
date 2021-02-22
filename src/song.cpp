#include <song.hpp>
#include <iostream>


Song::Song(std::string &path) {
    // TODO log debug (or info?)
    std::cout << "Created song object for file: " << path << std::endl;
    m_path = path;
}

void Song::print() {
    std::cout << "Title: " << m_title << std::endl;
    std::cout << "Album: " << m_album << std::endl;
    std::cout << "Artist: " << m_artist << std::endl;
    std::cout << "Genre: " << m_genre << std::endl;
    std::cout << "Release Year: " << m_release << std::endl;
    std::cout << "Song duration: " << m_duration << std::endl;
    std::cout << "Filepath: " << m_path << std::endl;
    std::cout << "\n----- Audio information -----\n" << std::endl;
    std::cout << "Audio start: " << m_audio_start << std::endl;
    std::cout << "Audio duration: " << m_delay << std::endl;
}

Song::~Song() {
    std::cout << "Destroying song with path: " << m_path << std::endl;
}
