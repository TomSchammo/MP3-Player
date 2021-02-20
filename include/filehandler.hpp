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
        Filehandler(std::string filename);


        /**
         * Checks whether a file exists or not.
         *
         * @return true if the file exists, false otherwise.
         */
        inline bool exists() {
            std::filesystem::path f(m_filename);
            return std::filesystem::exists(f);
        }


        /**
         * Reads "bytes" bytes from a stream, starting at byte "position".
         *
         * @param buffer    A reference to a char array, that the bytes will be written into
         * @param position  The starting position of the pointer
         * @param bytes     The number of bytes that should be read
         */
        void readBytes(char (&buffer)[], const std::uint16_t position, const unsigned char bytes);


        /**
         * Reads "bytes" bytes from a stream, starting at byte "position".
         *
         * @param buffer    A reference to a char array, that the bytes will be written into
         * @param position  The starting position of the pointer
         * @param way       The point of the file that the position is relative  to (eg. start, or end)
         * @param bytes     The number of bytes that should be read
         */
        void readBytes(char (&buffer)[], const std::uint16_t position, std::_Ios_Seekdir way, const unsigned char bytes);


        /**
         * Reads "bytes" bytes from a stream, starting at byte "position" and null terminates them.
         *
         * @param s         A reference to a string that will contain the data after reading it from the file
         * @param position  The starting position of the pointer
         * @param bytes     The number of bytes that should be read
         */
        void readString(std::string &s, const std::uint16_t position, const unsigned char bytes);


        /**
         * Reads "bytes" bytes from a stream, starting at byte "position" and null terminates them.
         *
         * @param s         A reference to a string that will contain the data after reading it from the file
         * @param position  The starting position of the pointer
         * @param way       The point of the file that the position is relative  to (eg. start, or end)
         * @param bytes     The number of bytes that should be read
         */
        void readString(std::string &s, const std::uint16_t position, std::_Ios_Seekdir way, const unsigned char bytes);


        /**
         * Reads the whole (text) file and puts every line into a vector of strings.
         * Then a shared_ptr to that vector is returned.
         */
        std::shared_ptr<std::vector<std::string>> read();


        /**
         * Class destructor, closes the filestream if open.
         */
        ~Filehandler();


    private:
        std::string m_filename;
        std::ifstream m_stream;


};

#endif // FILEHANDLER_HPP
