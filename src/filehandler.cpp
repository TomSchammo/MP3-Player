#include <filehandler.hpp>


Filehandler::Filehandler(const char* filename) {
    this->filename = filename;

    if (exists()) {
        this->stream.open(filename, std::ios::binary | std::ios::in);
    }
}


char* Filehandler::read(const unsigned int position, const unsigned char bytes) {
    char *buffer = new char[bytes];

    stream.seekg(position, std::ios::beg);
    stream.read(buffer, bytes);

    return buffer;
}


char* Filehandler::read(const unsigned int position, enum std::_Ios_Seekdir way, const unsigned char bytes) {
    char *buffer = new char[bytes];

    stream.seekg(position, way);
    stream.read(buffer, bytes);

    return buffer;
}


Filehandler::~Filehandler() {
    if(stream.is_open())
        stream.close();
}
