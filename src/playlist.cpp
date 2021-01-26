#include <playlist.hpp>
#include <filehandler.hpp>

void Playlist::readM3U(const char *filename, std::vector<song> &songlist) {

    Filehandler filehandler(filename);

    if (filehandler.exists()) {

        auto paths = filehandler.read();

        for (auto path : *paths) {
            song song;
            song.path = path;
            songlist.push_back(song);;
        }

    }

}
