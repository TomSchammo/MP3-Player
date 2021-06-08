#include <song.hpp>
#include <fstream>
#include <iostream>
#include <log.hpp>

using namespace logging;


Song::Song(const std::string& t_path) : m_path(t_path) {
    log<LogLevel::INFO>("Creating song object for file: " + t_path);
}

void write_img(Song &song) {

    for (const auto& art : song.m_art) {

        std::ofstream stream;
        stream.open(song.m_title + ".png", std::ios::binary | std::ios::out);
        stream.write(art.m_data->data(), static_cast<long>(art.m_data->size()));
        stream.close();


        // size is not what is supposed to be
        log<LogLevel::DDEBUG>("Album art has size: " + std::to_string(art.m_data->size()));
    }
}

void Song::print() {
    std::cout << "\033[0m";
    std::cout << "Title: " << m_title << std::endl;
    std::cout << "Album: " << m_album << std::endl;
    std::cout << "Track number: " << m_track_number << std::endl;
    std::cout << "Artist: " << m_artist << std::endl;
    std::cout << "Genre: " << m_genre << std::endl;
    std::cout << "Release Year: " << m_release << std::endl;
    std::cout << "Song duration: " << m_duration << std::endl;
    std::cout << "Filepath: " << m_path << std::endl;
    std::cout << "\n----- Audio information -----\n" << std::endl;
    std::cout << "Audio start: " << m_audio_start << std::endl;
    std::cout << "Audio duration: " << m_delay << std::endl;


    write_img(*this);
}

Song::~Song() {
    log<LogLevel::INFO>("Destroying song with path: " + m_path);
}
