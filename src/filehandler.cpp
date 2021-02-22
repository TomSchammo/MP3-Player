#include <filehandler.hpp>
#include <memory>
#include <vector>


Filehandler::Filehandler(std::string filename) {
    this->m_filename = filename;

    if (exists()) {
        this->m_stream.open(m_filename, std::ios::binary | std::ios::in);
    }
}


void Filehandler::readBytes(char buffer[], const std::uint16_t position, const unsigned char bytes) {

    m_stream.seekg(position, std::ios::beg);
    m_stream.read(buffer, bytes);

}


void Filehandler::readBytes(char buffer[], const std::uint16_t position, enum std::_Ios_Seekdir way, const unsigned char bytes) {

    m_stream.seekg(position, way);
    m_stream.read(buffer, bytes);

}


void Filehandler::readString(std::string &s, const std::uint16_t position, const unsigned char bytes) {

    char* buffer = new char[bytes];

    m_stream.seekg(position, std::ios::beg);
    m_stream.read(buffer, bytes);

    // if the string read is not null terminated its contents are copied into a new
    // buffer that is one byte longer, and a '\0' byte is added at the end
    if (buffer[bytes - 1] != '\0') {
        char* newBuffer = new char[bytes + 1];
        for (int i = 0; i < bytes; ++i) {
           newBuffer[i] = buffer[i];
        }

        newBuffer[bytes] = '\0';

        delete [] buffer;
        buffer = newBuffer;
    }

    s = buffer;
    delete []  buffer;
}


void Filehandler::readString(std::string &s, const std::uint16_t position, enum std::_Ios_Seekdir way, const unsigned char bytes) {

    char* buffer = new char[bytes];

    m_stream.seekg(position, way);
    m_stream.read(buffer, bytes);

    // if the string read is not null terminated its contents are copied into a new
    // buffer that is one byte longer, and a '\0' byte is added at the end
    if (buffer[bytes - 1] != '\0') {
        char* newBuffer = new char[bytes + 1];
        for (int i = 0; i < bytes; ++i) {
           newBuffer[i] = buffer[i];
        }

        newBuffer[bytes] = '\0';

        delete [] buffer;
        buffer = newBuffer;
    }

    s = buffer;
    delete [] buffer;
}


std::shared_ptr<std::vector<std::string>> Filehandler::read() {

    char* data = new char[1];

    m_stream.seekg(0, std::ios::beg);

    auto lines = std::make_shared<std::vector<std::string>>();

    while (!m_stream.eof()) {

        m_stream.read(data, 1);
        std::string line = "";

        // while char read not equal "\n"
        while (data[0] != 10) {
            line += data[0];
            m_stream.read(data, 1);
        }

        lines->push_back(line);
    }


    return lines;
}

Filehandler::~Filehandler() {
    if(m_stream.is_open())
        m_stream.close();
}
