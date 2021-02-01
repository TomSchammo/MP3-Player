#include <iostream>
#include <string>
#include <song.hpp>
#include <id3.hpp>

int main()
{
    std::cout << "Please enter the directory of the file that should be played" << std::endl;

    std::string filename;

    std::cin >> filename;

    Song song(filename);

    readID3(song);

    song.print();


    return 0;
}
