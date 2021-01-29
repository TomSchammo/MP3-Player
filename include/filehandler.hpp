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
#include <sys/stat.h>
#include <fstream>
#include <memory>
#include <vector>

class Filehandler {

    public:

        /**
         * Class constructor, initializes filename and opens a read only stream
         * for the file.
         *
         * @param filename name of the file
         */
        Filehandler(const char* filename);


        /**
         * Checks whether a file exists or not.
         *
         * @return true if the file exists, false otherwise.
         */
        inline bool exists() {
            struct stat buffer;
            return (stat (filename, &buffer) == 0);
        }


        /**
         * Reads "bytes" bytes from a stream, starting at byte "position".
         *
         * @param position  The starting position of the pointer
         * @param bytes     The number of bytes that should be read
         * @return a pointer to the first byte of a char array containing the requested bytes
         */
        char* read(const std::uint16_t position, const unsigned char bytes);


        /**
         * Reads "bytes" bytes from a stream, starting at byte "position".
         *
         * @param position  The starting position of the pointer
         * @param bytes     The number of bytes that should be read
         * @return a pointer to the first byte of a char array containing the requested bytes
         */
        char* read(const std::uint16_t position, std::_Ios_Seekdir way, const unsigned char bytes);


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
        const char* filename;
        std::ifstream stream;


};

#endif // FILEHANDLER_HPP
