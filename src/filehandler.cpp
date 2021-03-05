#include <filehandler.hpp>
#include <vector>
#include <iostream>


Filehandler::Filehandler(std::string filename) {
    this->m_filename = filename;

    if (exists()) {

        // TODO log debug
        std::cout << "Creating file handler object for file: " << filename << std::endl;

        this->m_stream.open(m_filename, std::ios::binary | std::ios::in);
    }

    else {
        // TODO log warn
        std::cout << "File " << m_filename << " does not exist!" << std::endl;
    }
}


void Filehandler::readBytes(char buffer[], const std::uint16_t position, const unsigned char bytes) {

    // TODO log debug
    std::cout << "Reading " << bytes << " bytes starting at offset " << position << " from file: " << m_filename << std::endl;

    m_stream.seekg(position, std::ios::beg);
    m_stream.read(buffer, bytes);

}


void Filehandler::readBytes(char buffer[], const std::uint16_t position, enum std::_Ios_Seekdir way, const unsigned char bytes) {

    // TODO log debug
    std::cout << "Reading " << bytes << " bytes starting at offset " << position
        << " relative to the " << (way ==  std::ios_base::beg ? "beginning" : "end")
        << " of the file: " << m_filename << std::endl;

    m_stream.seekg(position, way);
    m_stream.read(buffer, bytes);

}


void Filehandler::writeBytes(const std::uint16_t position, const char* bytes, std::uint16_t size) {

    std::ofstream stream;

    // open stream in binary write mode
    stream.open(m_filename, std::ios::binary | std::ios::out);

    // place position at specified offset
    stream.seekp(position, std::ios::beg);

    // write bytes to the file
    stream.write(bytes, size);


    stream.close();
}


void Filehandler::readString(std::string &s, const std::uint16_t position, const unsigned char bytes) {

    // TODO log debug
    std::cout << "Reading " << bytes << " bytes starting at offset " << position << " from the file: " << m_filename << std::endl;

    char* buffer = new char[bytes];

    m_stream.seekg(position, std::ios::beg);
    m_stream.read(buffer, bytes);

    // if the string read is not null terminated its contents are copied into a new
    // buffer that is one byte longer, and a '\0' byte is added at the end
    if (buffer[bytes - 1] != '\0') {
        // TODO log debug
        std::cout << "String is not null terminated..." << std::endl;

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

    // TODO log debug
    std::cout << "Reading " << bytes << " bytes starting at offset " << position
        << " relative to the " << (way ==  std::ios_base::beg ? "beginning" : "end")
        << " of the file: " << m_filename << std::endl;

    char* buffer = new char[bytes];

    m_stream.seekg(position, way);
    m_stream.read(buffer, bytes);

    // if the string read is not null terminated its contents are copied into a new
    // buffer that is one byte longer, and a '\0' byte is added at the end
    if (buffer[bytes - 1] != '\0') {
        // TODO log info
        std::cout << "String is not null terminated..." << std::endl;

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

    // TODO log info
    std::cout << "Reading file: " << m_filename << std::endl;

    char data[1];

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

    // TODO log info
    std::cout << "Read " << lines->size() << " lines in file " << m_filename << std::endl;

    return lines;
}

Filehandler::~Filehandler() {
    if(m_stream.is_open()) {
        std::cout << "Closing stream of " << m_filename << std::endl;
        m_stream.close();
    }

    std::cout << "Destorying filehandler object for file: " << m_filename << std::endl;
}
