#include <config.h>

#ifdef TEST

#define CATCH_CONFIG_MAIN
#include "../test/test.cpp"

// TODO disable logging

#else

#include <iostream>
#include <string>
#include <song.hpp>
#include <id3.hpp>
// #include <chrono>


int main(int arc, char* agrv[]) {

    if (arc > 1) {

        for(int i = 1; i < arc; i++) {
            std::string filename = agrv[i];

            // auto start = std::chrono::high_resolution_clock::now();

            Song song(filename);

            ID3::readID3(song);

            // auto stop = std::chrono::high_resolution_clock::now();

            song.print();

            // auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);

            // std::cout << "Execution took " << duration.count() << " milliseconds" << std::endl;

        }


    }

    else {
        std::cerr << "You need to provide at least one filename!" << std::endl;
    }


    return 0;
}

#endif // TEST
