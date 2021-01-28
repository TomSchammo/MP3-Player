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


std::uint8_t getVersion(Filehandler &handler) {
    auto ptr = handler.read(LOCATION_VERSION, SIZE_OF_VERSION);
    auto size = *ptr;
    delete[] ptr;

    return std::uint8_t(size);
}


std::uint8_t getFlags(Filehandler &handler) {
    auto ptr = handler.read(LOCATION_FLAGS, SIZE_OF_FLAGS);
    auto flags = *ptr;
    delete[] ptr;

    return std::uint8_t(flags);
}


