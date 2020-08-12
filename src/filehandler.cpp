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
        this->stream.open(filename, std::ios::in);
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
 * Class destructor, closes the filestream if open.
 */
Filehandler::~Filehandler() {
    if(stream.is_open())
        stream.close();
}
