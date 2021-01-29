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

    return static_cast<std::uint8_t>(size);
}


std::uint8_t getFlags(Filehandler &handler) {
    auto ptr = handler.read(LOCATION_FLAGS, SIZE_OF_FLAGS);
    auto flags = *ptr;
    delete[] ptr;

    return static_cast<std::uint8_t>(flags);
}


std::uint16_t getSize(Filehandler &handler, const bool extended) {
    auto ptr = handler.read(extended ? LOCATION_EXTENDED_HEADER : LOCATION_SIZE, SIZE_OF_SIZE);
    std::uint16_t size = 0;

    for (int i = 0; i < SIZE_OF_SIZE; ++i) {
        size += static_cast<std::uint16_t>(ptr[i]);
    }

    delete[] ptr;

    return size;
}

}


