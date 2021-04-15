/******************************************************************************
* File:             filehandler.hpp
*
* Author:           Tom Schammo
* Created:          26/01/2021
* Description:      Class responsible for file interactions
*****************************************************************************/


#ifndef FILEHANDLER_HPP
#define FILEHANDLER_HPP

#include <cstdint>
#include <string>
#include <sys/stat.h>
#include <fstream>
#include <memory>
#include <vector>
#include <filesystem>

class Filehandler {

    public:

        /**
         * Class constructor, initializes filename and opens a read only stream
         * for the file.
         *
         * @param filename name of the file
         */
        Filehandler(std::string filename) noexcept;

        // Enabling move operations
        Filehandler(Filehandler &&) = default;

        /**
         * Checks whether a file exists or not.
         *
         * @return true if the file exists, false otherwise.
         */
        inline bool exists() const noexcept {
            std::filesystem::path f(m_filename);
            return std::filesystem::exists(f);
        }


        /**
         * Reads "bytes" bytes from a stream, starting at byte "position".
         *
         * @param buffer    A char array, that the bytes will be written into
         * @param position  The starting position of the pointer
         * @param bytes     The number of bytes that should be read
         */
        void readBytes(char buffer[], const std::uint32_t position, const std::uint32_t bytes) const noexcept;


        /**
         * Reads "bytes" bytes from a stream, starting at byte "position".
         *
         * @param buffer    A char array, that the bytes will be written into
         * @param position  The starting position of the pointer
         * @param way       The point of the file that the position is relative  to (eg. start, or end)
         * @param bytes     The number of bytes that should be read
         */
        void readBytes(char buffer[], const std::uint32_t position, std::_Ios_Seekdir way, const std::uint32_t bytes) const noexcept;


        /**
         * Writes "bytes" to a file at offset "position" relative to the start of the file.
         * The bytes are appended at said position.
         *
         * @param position is the offset relative to the start of the file where the data should be written to
         * @param bytes    are the bytes that should be written to the file
         * @param size     contains how many bytes should be written to the file
         */
        void writeBytes(const std::uint32_t position, const char bytes[], std::uint32_t size) const noexcept;


        /**
         * Deletes 'bytes' bytes from a file.
         *
         * Basically copies all the bytes from the file to a temporary file, except the
         * bytes are supposed to be deleted. Those are skipped.
         *
         * After successfully copying everything over, the old file is then deleted, and
         * the temporary file is renamed to the name of the original file.
         *
         * @param position  The relative offset to the start of the file of the first byte
         * @param bytes     The amount of bytes that should be deleted
         */
        void deleteBytes(std::uint32_t position, std::uint32_t bytes) const noexcept;


        /**
         * Reads "bytes" bytes from a stream, starting at byte "position" and null terminates them.
         *
         * @param s         A reference to a string that will contain the data after reading it from the file
         * @param position  The starting position of the pointer
         * @param bytes     The number of bytes that should be read
         */
        void readString(std::string &s, const std::uint32_t position, const unsigned char bytes) const noexcept;


        /**
         * Reads "bytes" bytes from a stream, starting at byte "position" and null terminates them.
         *
         * @param s         A reference to a string that will contain the data after reading it from the file
         * @param position  The starting position of the pointer
         * @param way       The point of the file that the position is relative  to (eg. start, or end)
         * @param bytes     The number of bytes that should be read
         */
        void readString(std::string &s, const std::uint32_t position, std::_Ios_Seekdir way, const unsigned char bytes) const noexcept;


        /**
         * Reads the whole (text) file and puts every line into a vector of strings.
         * Then a shared_ptr to that vector is returned.
         */
        std::shared_ptr<std::vector<std::string>> read() const noexcept;


        /**
         * Class destructor, closes the filestream if open.
         */
        ~Filehandler() noexcept;


    private:
        std::string m_filename;
        mutable std::ifstream m_stream;


};

#endif // FILEHANDLER_HPP
