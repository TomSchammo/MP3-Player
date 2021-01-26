#ifndef SONG_HPP
#define SONG_HPP

class song
{
public:
    const char* title;
    const char* album;
    const char* artist;
    const char* path;

public:
    song ();
    virtual ~song ();

};

#endif /* ifndef SONG_HPP */
