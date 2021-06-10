#include <filehandler.hpp>
#include <iostream>
#include <log.hpp>


Filehandler::Filehandler(std::string  t_filename) noexcept : m_filename(std::move(t_filename)) {

    if (exists()) {

        log::info(fmt::format("Creating file handler object for file {}", m_filename));

        this->m_stream.open(m_filename, std::ios::binary | std::ios::in);
    }

    else
        log::warn(fmt::format("File {}  does not exist!", m_filename));
}


void Filehandler::readBytes(char t_buffer[], const std::uint32_t t_position, const std::uint32_t t_bytes) const noexcept {

    log::debug(fmt::format("Reading {} bytes starting at offset {} from file: ", t_bytes, t_position, m_filename));

    m_stream.seekg(t_position, std::ios::beg);
    m_stream.read(t_buffer, t_bytes);

}


void Filehandler::readBytes(char t_buffer[], const std::uint32_t t_position, enum std::_Ios_Seekdir t_way, const std::uint32_t t_bytes) const noexcept {

    log::debug(fmt::format("Reading {} bytes starting at offset {} relative to the {} of the file: {}",
                           t_bytes, t_position, (t_way == std::ios_base::beg ? "beginning" : "end"), m_filename));

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

            auto size_remaining = EOF_ - current;


            // if the last byte(s) have been removed there is no point in continuing
            if (size_remaining > 0) {
                m_stream.seekg(t_bytes, std::ios::cur);

                // copying the rest of the file
                while (size_remaining >= SIZE_OF_BUFFER) {
                    m_stream.read(buffer, SIZE_OF_BUFFER);
                    stream.write(buffer, SIZE_OF_BUFFER);
                    size_remaining = EOF_ - m_stream.tellg();
                }

                pbuffer = new char[static_cast<unsigned long>(size_remaining)];

                m_stream.read(pbuffer, size_remaining);
                stream.write(pbuffer, size_remaining);

                delete [] pbuffer;

            }

            // copying is done, closing streams
            m_stream.close();
            stream.close();

            bool error = false;

            if (m_stream.is_open()) {
                log::error("Error when closing streams: m_stream is expected to be closed but is open!");
                error = true;

            }

            if (stream.is_open()) {
                log::error("Error when closing streams: m_stream is expected to be closed but is open!");
                error = true;

            }

            if (!error) {

                // replacing old file with new one
                if (std::remove(m_filename.c_str()) == 0) {
                    if (std::rename((m_filename + ".tmp").c_str(), m_filename.c_str()) != 0) {
                        log::error(fmt::format("Could not rename {0}.tmp to {0}", m_filename));
                    }

                    else {
                        log::info(fmt::format("Successfully renamed {0}.tmp to {0}" , m_filename));

                        // reopening stream
                        m_stream.open(m_filename, std::ios::binary | std::ios::in);

                        if (m_stream.is_open())
                            log::info(fmt::format("Successfully re-opened filestream for file {}", m_filename));

                        else
                            log::error(fmt::format("Failure when re-opening filestream for file {}", m_filename));

                    }
                }

                else {
                    log::error(fmt::format("Could not remove {}", m_filename));
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
            log::error("Stream position is supposed to be at the end of the file");

            // TODO deal with this
        }
    }

    // one file could not be opened
    else {

        if (!stream.is_open()) {

            log::error(fmt::format("Could not open {}.tmp", m_filename));
            log::error("Cleaning up...");

            if (std::remove((m_filename + ".tmp").c_str()) == 0)
                log::info(fmt::format("Successfully deleted {}.tmp", m_filename));

            else
                log::error(fmt::format("Could not remove {}.tmp", m_filename));
        }

        if (!m_stream.is_open()) {
            log::error(fmt::format("Expected opened filestream for file {} but stream was closed...", m_filename));
        }
    }
}


void Filehandler::readString(std::string& t_string, const std::uint32_t t_position, const std::uint32_t t_bytes) const noexcept {

    log::debug(fmt::format("Reading {:d} bytes starting at offset {} from file: {}", t_bytes, t_position, m_filename));

    std::vector<char> buffer(t_bytes);

    // reserving enough space for the string and a potentially missing  null terminator to avoid reallocation
    buffer.reserve(t_bytes + 1);

    m_stream.seekg(t_position, std::ios::beg);
    m_stream.read(buffer.data(), t_bytes);

    // if the string read is not null terminated its contents are copied into a new
    // buffer that is one byte longer, and a '\0' byte is added at the end
    if (buffer.at(t_bytes - 1) != '\0') {

        log::debug("String is not null terminated...");

        buffer.push_back('\0');

    }

    // TODO
    t_string = std::string(buffer.data());
}


void Filehandler::readString(std::string& t_string, const std::uint32_t t_position, enum std::_Ios_Seekdir t_way, const std::uint32_t t_bytes) const noexcept {


    log::debug(fmt::format("Reading {} bytes starting at offset {} relative to the {} of the file: {}",
                           t_bytes, t_position, (t_way == std::ios_base::beg ? "beginning" : "end"), m_filename));

    std::vector<char> buffer(t_bytes);

    // reserving enough space for the string and a potentially missing null terminator to avoid reallocation
    buffer.reserve(t_bytes + 1);

    m_stream.seekg(t_position, t_way);
    m_stream.read(buffer.data(), t_bytes);

    // if the string read is not null terminated its contents are copied into a new
    // buffer that is one byte longer, and a '\0' byte is added at the end
    if (buffer.at(t_bytes - 1) != '\0') {
        log::info("String is not null terminated...");

        buffer.push_back('\0');
    }

    t_string = std::string(buffer.data());
}


std::unique_ptr<std::vector<std::string>> Filehandler::read() const noexcept {

    log::info(fmt::format("Reading file: {}", m_filename));

    // TODO can this be improved?
    char data[1];

    m_stream.seekg(0, std::ios::beg);

    auto lines = std::make_unique<std::vector<std::string>>();

    while (!m_stream.eof()) {

        m_stream.read(data, 1);
        std::string line;

        // while char read not equal "\n"
        while (data[0] != 10) {
            line += data[0];
            m_stream.read(data, 1);
        }

        lines->push_back(line);
    }

    log::info(fmt::format("Read {} lines in file {}", lines->size(), m_filename));

    return lines;
}


Filehandler::~Filehandler() noexcept {

    if(m_stream.is_open()) {
        log::info(fmt::format("Closing stream of ", m_filename));
        m_stream.close();
    }

    log::info(fmt::format("Destroying filehandler object for file {}", m_filename));
}
