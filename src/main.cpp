#include <iostream>
#include <string>
#include <song.hpp>
#include <id3.hpp>
#include <chrono>


int main(int arc, char *agrv[])
{

    if (arc > 1) {

        std::string filename = agrv[1];

        auto start = std::chrono::high_resolution_clock::now();

        Song song(filename);

        readID3(song);

        auto stop = std::chrono::high_resolution_clock::now();

        song.print();

        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);

        std::cout << "Execution took " << duration.count() << " milliseconds" << std::endl;

    }

    else {
        std::cerr << "You need to provide a filename!" << std::endl;
    }


    return 0;
}
