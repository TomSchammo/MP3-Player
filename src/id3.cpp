#include <id3.hpp>


bool detectID3(Filehandler &handler) {

    auto ptr = handler.read(LOCATION_START, 3);
    auto returnValue = std::string(ptr).compare("ID3") == 0;
    delete[] ptr;

    return returnValue;
}


bool detectID3Footer(Filehandler &handler) {

    auto ptr = handler.read(LOCATION_START, std::ios::end, 3);
    auto returnValue = std::string(ptr).compare("3DI") == 0;
    delete[] ptr;

    return returnValue;
}


