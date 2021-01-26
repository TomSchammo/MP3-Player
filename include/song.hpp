#ifndef SONG_HPP
#define SONG_HPP

#include <string>

class song
{
public:
    std::string title;
    std::string album;
    std::string artist;
    std::string path;

public:
    song ();
    virtual ~song ();

};

#endif /* ifndef SONG_HPP */
