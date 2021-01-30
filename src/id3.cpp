#include <id3.hpp>
#include <iostream>


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


void parseFrameData(char* data, std::string frameID, Song &song) {

    if (frameID.compare("TIT2") == 0) {
        // TODO log verbose
        std::cout << "Found a TIT2 frame, setting song title to: " << data << std::endl;

        song.m_title = data;

    } else if (frameID.compare("TALB") == 0) {
        // TODO log verbose
        std::cout << "Found a TALB frame, setting song title to: " << data << std::endl;

        song.m_album = data;

    } else if (frameID.compare("TPE1") == 0) {
        // TODO log verbose
        std::cout << "Found a TPE1 frame, setting artist to: " << data << std::endl;

        song.m_artist = data;

    } else if (frameID.compare("TLEN") == 0) {
        // TODO log verbose
        std::cout << "Found a TLEN frame, setting track length to: " << *data << std::endl;

        song.m_duration = static_cast<std::uint16_t>(*data);

    } else if (frameID.compare("TDLY") == 0) {
        // TODO log verbose
        std::cout << "Found a TDLY frame, setting delay to: " << *data << "ms" << std::endl;

        song.m_delay = static_cast<std::uint16_t>(*data);

    } else {
        // TODO log warn
        std::cout << "frameID not supported yet";
    }

    // TODO TFLT (audio type, default is MPEG)
    // TODO MLLT (MPEG location lookup table (do I need this) (4.6), mentions player counter (4.16))
    // TODO APIC (Album art (section 4.14))
    // TODO PCNT (Player counter (4.16), should be incremented)
}


}


