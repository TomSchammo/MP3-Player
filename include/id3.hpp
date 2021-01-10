#ifndef ID3_HPP
#define ID3_HPP
#include <filehandler.hpp>

// constants
constexpr unsigned char LOCATION_START = 0;
constexpr unsigned char LOCATION_VERSION = 3;
constexpr unsigned char LOCATION_FLAGS = 5;
constexpr unsigned char LOCATION_SIZE = 6;
constexpr unsigned char LOCATION_EXTENDED_HEADER = 10;
constexpr unsigned char SIZE_OF_FLAGS = 1;
constexpr unsigned char SIZE_OF_VERSION = 1;
constexpr unsigned char SIZE_OF_SIZE = 4;
constexpr unsigned char SIZE_OF_FRAME_ID = 4;



#endif // ID3_HPP
