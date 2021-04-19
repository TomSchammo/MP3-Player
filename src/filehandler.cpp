#include <filehandler.hpp>
#include <iostream>


Filehandler::Filehandler(std::string t_filename) noexcept : m_filename(t_filename) {

    if (exists()) {

        // TODO log debug
        std::cout << "Creating file handler object for file: " << m_filename << std::endl;

        this->m_stream.open(m_filename, std::ios::binary | std::ios::in);
    }

    else {
        // TODO log warn
        std::cout << "File " << m_filename << " does not exist!" << std::endl;
    }
}


void Filehandler::readBytes(char t_buffer[], const std::uint32_t t_position, const std::uint32_t t_bytes) const noexcept {

    // TODO log debug
    std::cout << "Reading " << t_bytes << " bytes starting at offset " << t_position << " from file: " << m_filename << std::endl;

    m_stream.seekg(t_position, std::ios::beg);
    m_stream.read(t_buffer, t_bytes);

}


void Filehandler::readBytes(char t_buffer[], const std::uint32_t t_position, enum std::_Ios_Seekdir t_way, const std::uint32_t t_bytes) const noexcept {

    // TODO log debug
    std::cout << "Reading " << t_bytes << " bytes starting at offset " << t_position
        << " relative to the " << (t_way ==  std::ios_base::beg ? "beginning" : "end")
        << " of the file: " << m_filename << std::endl;

    m_stream.seekg(t_position, t_way);
    m_stream.read(t_buffer, t_bytes);

}


void Filehandler::writeBytes(const std::uint32_t t_position, const char* t_bytes, std::uint32_t t_size) const noexcept {

    std::ofstream stream;

    // open stream in binary write mode
    stream.open(m_filename, std::ios::binary | std::ios::out);

    // place position at specified offset
    stream.seekp(t_position, std::ios::beg);

    // write bytes to the file
    stream.write(t_bytes, t_size);


    stream.close();
}

void Filehandler::deleteBytes(std::uint32_t t_position, std::uint32_t t_bytes) const noexcept {

    // TODO assert that all read/write operations were successful before deleting file at the end

    // opening a temporary file to write the content of the original file to
    std::ofstream stream;
    stream.open(m_filename + ".tmp", std::ios::binary | std::ios::out);

    if (stream.is_open() && m_stream.is_open()) {

        // retrieving end of file, this will be important later
        stream.seekp(0, std::ios::end);

        if (stream.eof()) {

            const auto EOF_ = stream.tellp();



            // creating buffer to read/write from/to
            const std::uint32_t SIZE_OF_BUFFER = 1024;
            char buffer[SIZE_OF_BUFFER];


            // putting the pointer of both files at the start
            stream.seekp(0, std::ios::beg);
            m_stream.seekg(0, std::ios::beg);


            // while bytes left > size of buffer
            while (static_cast<std::uint32_t>(m_stream.tellg()) - t_position >= SIZE_OF_BUFFER) {
                m_stream.read(buffer, SIZE_OF_BUFFER);
                stream.write(buffer, SIZE_OF_BUFFER);
            }

            // dynamically creating buffer to get the last portion of the file
            char* pbuffer = new char[t_position % SIZE_OF_BUFFER];

            m_stream.read(pbuffer, t_position % SIZE_OF_BUFFER);
            stream.write(pbuffer, t_position % SIZE_OF_BUFFER);

            delete [] pbuffer;

            // skipping the bytes that are supposed to be deleted
            m_stream.seekg(t_bytes, std::ios::cur);
            auto current = m_stream.tellg();

            // TODO make sure this is always > 0, and then make it unsigned
            auto size_remaining = EOF_ - current;


            // if the last byte(s) have been removed there is no point in continueing
            if (size_remaining > 0) {
                m_stream.seekg(t_bytes, std::ios::cur);

                // copying the rest of the file
                while (size_remaining >= SIZE_OF_BUFFER) {
                    m_stream.read(buffer, SIZE_OF_BUFFER);
                    stream.write(buffer, SIZE_OF_BUFFER);
                    size_remaining = EOF_ - m_stream.tellg();
                }

                // TODO size_remaining has to be > 0
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
        else {
            // TODO log error
            std::cout << "[ERROR] stream position is supposed to be at the end of the file" << std::endl;

            // TODO deal with this
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


void Filehandler::readString(std::string& t_string, const std::uint32_t t_position, const unsigned char t_bytes) const noexcept {

    // TODO log debug
    std::cout << "Reading " << int(t_bytes) << " bytes starting at offset " << t_position << " from the file: " << m_filename << std::endl;

    char* buffer = new char[t_bytes];

    m_stream.seekg(t_position, std::ios::beg);
    m_stream.read(buffer, t_bytes);

    // if the string read is not null terminated its contents are copied into a new
    // buffer that is one byte longer, and a '\0' byte is added at the end
    if (buffer[t_bytes - 1] != '\0') {
        // TODO log debug
        std::cout << "String is not null terminated..." << std::endl;

        char* newBuffer = new char[t_bytes + 1];
        for (int i = 0; i < t_bytes; ++i) {
           newBuffer[i] = buffer[i];
        }

        newBuffer[t_bytes] = '\0';

        delete [] buffer;
        buffer = newBuffer;
    }

    t_string = buffer;
    delete []  buffer;
}


void Filehandler::readString(std::string& t_string, const std::uint32_t t_position, enum std::_Ios_Seekdir t_way, const unsigned char t_bytes) const noexcept {

    // TODO log debug
    std::cout << "Reading " << int(t_bytes) << " bytes starting at offset " << t_position
        << " relative to the " << (t_way ==  std::ios_base::beg ? "beginning" : "end")
        << " of the file: " << m_filename << std::endl;

    char* buffer = new char[t_bytes];

    m_stream.seekg(t_position, t_way);
    m_stream.read(buffer, t_bytes);

    // if the string read is not null terminated its contents are copied into a new
    // buffer that is one byte longer, and a '\0' byte is added at the end
    if (buffer[t_bytes - 1] != '\0') {
        // TODO log info
        std::cout << "String is not null terminated..." << std::endl;

        char* newBuffer = new char[t_bytes + 1];
        for (int i = 0; i < t_bytes; ++i) {
           newBuffer[i] = buffer[i];
        }

        newBuffer[t_bytes] = '\0';

        delete [] buffer;
        buffer = newBuffer;
    }

    t_string = buffer;
    delete [] buffer;
}


std::unique_ptr<std::vector<std::string>> Filehandler::read() const noexcept {

    // TODO log info
    std::cout << "Reading file: " << m_filename << std::endl;

    // TODO can this be improved?
    char data[1];

    m_stream.seekg(0, std::ios::beg);

    auto lines = std::make_unique<std::vector<std::string>>();

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


Filehandler::~Filehandler() noexcept {

    if(m_stream.is_open()) {
        std::cout << "Closing stream of " << m_filename << std::endl;
        m_stream.close();
    }

    std::cout << "Destorying filehandler object for file: " << m_filename << std::endl;
}
