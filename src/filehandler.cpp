#include <filehandler.hpp>
#include <memory>
#include <vector>


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


std::shared_ptr<std::vector<std::string>> Filehandler::read() {

    char* data = new char[1];

    stream.seekg(0, std::ios::beg);

    auto lines = std::make_shared<std::vector<std::string>>();

    while (!stream.eof()) {

        stream.read(data, 1);
        std::string line = "";

        // while char read not equal "\n"
        while (data[0] != 10) {
            line += data[0];
            stream.read(data, 1);
        }

        lines->push_back(line);
    }


    return lines;
}

Filehandler::~Filehandler() {
    if(stream.is_open())
        stream.close();
}
