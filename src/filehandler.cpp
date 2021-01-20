#include <filehandler.hpp>

/**
 * Class constructor, initializes filename and opens a read only stream
 * for the file.
 *
 * @param filename name of the file
 */
Filehandler::Filehandler(const char* filename) {
    this->filename = filename;

    if (exists()) {
        this->stream.open(filename, std::ios::binary | std::ios::in);
    }
}


/**
 * Checks whether a file exists or not.
 *
 * @return true if the file exists, false otherwise.
 */
inline bool Filehandler::exists() {
    struct stat buffer;
    return (stat (filename, &buffer) == 0);
}

/**
 * Reads "bytes" bytes from a stream, starting at byte "position".
 *
 * @param position  The starting position of the pointer
 * @param bytes     The number of bytes that should be read
 * @return a pointer to the first byte of a char array containing the requested bytes
 */
char* Filehandler::read(const unsigned int position, const unsigned char bytes) {
    char *buffer = new char[bytes];

    stream.seekg(position, std::ios::beg);
    stream.read(buffer, bytes);

    return buffer;
}

/**
 * Reads "bytes" bytes from a stream, starting at byte "position".
 *
 * @param position  The starting position of the pointer
 * @param bytes     The number of bytes that should be read
 * @return a pointer to the first byte of a char array containing the requested bytes
 */
char* Filehandler::read(const unsigned int position, enum std::_Ios_Seekdir way, const unsigned char bytes) {
    char *buffer = new char[bytes];

    stream.seekg(position, way);
    stream.read(buffer, bytes);

    return buffer;
}

/**
 * Class destructor, closes the filestream if open.
 */
Filehandler::~Filehandler() {
    if(stream.is_open())
        stream.close();
}
