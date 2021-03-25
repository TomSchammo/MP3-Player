#include <cstdio>
#include <filehandler.hpp>
#include <fstream>
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


void Filehandler::readBytes(char buffer[], const std::uint32_t position, const std::uint32_t bytes) {

    // TODO log debug
    std::cout << "Reading " << bytes << " bytes starting at offset " << position << " from file: " << m_filename << std::endl;

    m_stream.seekg(position, std::ios::beg);
    m_stream.read(buffer, bytes);

}


void Filehandler::readBytes(char buffer[], const std::uint32_t position, enum std::_Ios_Seekdir way, const std::uint32_t bytes) {

    // TODO log debug
    std::cout << "Reading " << bytes << " bytes starting at offset " << position
        << " relative to the " << (way ==  std::ios_base::beg ? "beginning" : "end")
        << " of the file: " << m_filename << std::endl;

    m_stream.seekg(position, way);
    m_stream.read(buffer, bytes);

}


void Filehandler::writeBytes(const std::uint32_t position, const char* bytes, std::uint32_t size) {

    std::ofstream stream;

    // open stream in binary write mode
    stream.open(m_filename, std::ios::binary | std::ios::out);

    // place position at specified offset
    stream.seekp(position, std::ios::beg);

    // write bytes to the file
    stream.write(bytes, size);


    stream.close();
}

void Filehandler::deleteBytes(std::uint32_t position, std::uint32_t bytes) {

    // TODO assert that all read/write operations were successful before deleting file at the end

    // opening a temporary file to write the content of the original file to
    std::ofstream stream;
    stream.open(m_filename + ".tmp", std::ios::binary | std::ios::out);

    if (stream.is_open() && m_stream.is_open()) {

        // retrieving end of file, this will be important later
        stream.seekp(0, std::ios::end);
        const std::uint32_t _EOF = stream.tellp();


        // creating buffer to read/write from/to
        const std::uint32_t SIZE_OF_BUFFER = 1024;
        char buffer[SIZE_OF_BUFFER];


        // putting the pointer of both files at the start
        stream.seekp(0, std::ios::beg);
        m_stream.seekg(0, std::ios::beg);


        // while bytes left > size of buffer
        while (static_cast<std::uint32_t>(m_stream.tellg()) - position >= SIZE_OF_BUFFER) {
            m_stream.read(buffer, SIZE_OF_BUFFER);
            stream.write(buffer, SIZE_OF_BUFFER);
        }

        // dynamically creating buffer to get the last portion of the file
        char* pbuffer = new char[position % SIZE_OF_BUFFER];

        m_stream.read(pbuffer, position % SIZE_OF_BUFFER);
        stream.write(pbuffer, position % SIZE_OF_BUFFER);

        delete [] pbuffer;

        // skipping the bytes that are supposed to be deleted
        m_stream.seekg(bytes, std::ios::cur);
        auto current = m_stream.tellg();
        auto size_remaining = _EOF - current;


        // if the last byte(s) have been removed there is no point in continueing
        if (size_remaining > 0) {
            m_stream.seekg(bytes, std::ios::cur);

            // copying the rest of the file
            while (size_remaining >= SIZE_OF_BUFFER) {
                m_stream.read(buffer, SIZE_OF_BUFFER);
                stream.write(buffer, SIZE_OF_BUFFER);
                size_remaining = _EOF - m_stream.tellg();
            }

            pbuffer = new char[size_remaining];

            m_stream.read(pbuffer, size_remaining);
            stream.write(pbuffer, size_remaining);

            delete [] pbuffer;

        }

        // copying is done, closing streams
        m_stream.close();
        stream.close();

        bool error = false;

        if (m_stream.is_open()) {
            // TODO log error
            std::cout << "Error when closing streams" << std::endl;
            std::cout << "m_stream (" << &m_stream << ") is expected to be closed, but is open" << std::endl;
            error = true;

        }

        if (stream.is_open()) {
            // TODO log error
            std::cout << "Error when closing streams" << std::endl;
            std::cout << "stream (" << &stream << ") is expected to be closed, but is open" << std::endl;
            error = true;

        }

        if (!error) {

            // replacing old file with new one
            if (std::remove(m_filename.c_str()) == 0) {
                if (std::rename((m_filename + ".tmp").c_str(), m_filename.c_str()) != 0) {
                    // TODO log error
                    std::cout << "Could not rename " << m_filename << ".tmp to " << m_filename << std::endl;
                }

                else {
                    // TODO log info
                    std::cout << "Successfully renamed " << m_filename << ".tmp to " << m_filename << std::endl;

                    // reopening stream
                    m_stream.open(m_filename, std::ios::binary | std::ios::in);

                    if (m_stream.is_open()) {
                        // TODO log info
                        std::cout << "Successfully re-opened filestream for file " << m_filename << std::endl;
                    }

                    else {
                        // TODO log error
                        std::cout << "Failure when re-opening filestream for file " << m_filename << std::endl;
                    }

                }
            }

            else {
                // TODO log error
                std::cout << "Could not remove " << m_filename << std::endl;
            }
        }

        // error when closing streams after copying
        else {
            // TODO how to proceed??
            // TODO could use number instead of bool and set bits for different streams
            // TODO then check which stream is open and try to close??
            // TODO if m_stream is open but stream is closed, could just return error?
            // TODO stream has to be closed though
        }
    }

    // one file could not be opened
    else {

        if (!stream.is_open()) {

            // TODO log error
            std::cout << "Could not open " << m_filename << ".tmp" << std::endl;
            std::cout << "Cleaning up..." << std::endl;

            if (std::remove((m_filename + ".tmp").c_str()) == 0) {
                // TODO log info
                std::cout << "Successfully deleted " << m_filename << ".tmp" << std::endl;

            } else {
                // TODO log error
                std::cout << "Could not remove " << m_filename << ".tmp" << std::endl;
            }
        }

        if (!m_stream.is_open()) {
            // TODO log error
            std::cout << "Expected opened filestream for file " << m_filename << " but stream was closed..."<< std::endl;
        }
    }
}


void Filehandler::readString(std::string &s, const std::uint32_t position, const unsigned char bytes) {

    // TODO log debug
    std::cout << "Reading " << int(bytes) << " bytes starting at offset " << position << " from the file: " << m_filename << std::endl;

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


void Filehandler::readString(std::string &s, const std::uint32_t position, enum std::_Ios_Seekdir way, const unsigned char bytes) {

    // TODO log debug
    std::cout << "Reading " << int(bytes) << " bytes starting at offset " << position
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
