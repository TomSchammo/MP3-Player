#include <iostream>
#include <string>
#include <song.hpp>
#include <id3.hpp>


int main(int arc, char *agrv[])
{

    if (arc > 1) {

        std::string filename = agrv[1];

        Song song(filename);

        readID3(song);

        song.print();

    }

    else {
        std::cerr << "You need to provide a filename!" << std::endl;
    }


    return 0;
}
