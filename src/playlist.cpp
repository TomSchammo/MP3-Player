#include <cstdlib>
#include <playlist.hpp>
#include <filehandler.hpp>

void Playlist::readM3U(const char *filename, std::vector<Song> &songlist) {

    Filehandler filehandler(filename);

    if (filehandler.exists()) {

        auto paths = filehandler.read();

        for (auto path : *paths) {
            Song song(path);
            songlist.push_back(song);;
        }

    }

}


void Playlist::shuffle(std::vector<Song> &playlist) {

    srand(time(nullptr));

    for (auto i = playlist.size(); i > 1; --i) {

        auto j = rand() % (i+1);

        auto temp = playlist[i];
        playlist[i] = playlist[j];
        playlist[j] = temp;
    }

}
