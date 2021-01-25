#ifndef FILEHANDLER_HPP
#define FILEHANDLER_HPP

#include <sys/stat.h>
#include <fstream>

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
        inline bool exists();


        /**
         * Reads "bytes" bytes from a stream, starting at byte "position".
         *
         * @param position  The starting position of the pointer
         * @param bytes     The number of bytes that should be read
         * @return a pointer to the first byte of a char array containing the requested bytes
         */
        char* read(const unsigned int position, const unsigned char bytes);


        /**
         * Reads "bytes" bytes from a stream, starting at byte "position".
         *
         * @param position  The starting position of the pointer
         * @param bytes     The number of bytes that should be read
         * @return a pointer to the first byte of a char array containing the requested bytes
         */
        char* read(const unsigned int position, std::_Ios_Seekdir way, const unsigned char bytes);


        /**
         * Class destructor, closes the filestream if open.
         */
        ~Filehandler();


    private:
        const char* filename;
        std::ifstream stream;


};

#endif // FILEHANDLER_HPP
