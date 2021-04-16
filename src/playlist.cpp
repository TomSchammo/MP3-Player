#include <playlist.hpp>
#include <filehandler.hpp>

void Playlist::readM3U(const char* t_filename, std::vector<Song>& t_songlist) {

    Filehandler filehandler(t_filename);

    if (filehandler.exists()) {

        auto paths = filehandler.read();

        for (auto path : *paths) {
            Song song(path);
            t_songlist.push_back(song);;
        }

    }

}


void Playlist::shuffle(std::vector<Song>& t_playlist) {

    srand(static_cast<unsigned int>(time(nullptr)));

    for (auto i = t_playlist.size(); i > 1; --i) {

        auto j = static_cast<unsigned long>(rand()) % (i+1);

        auto temp = t_playlist[i];
        t_playlist[i] = t_playlist[j];
        t_playlist[j] = temp;
    }

}
