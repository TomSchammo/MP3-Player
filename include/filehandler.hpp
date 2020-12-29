#ifndef FILEHANDLER_HPP
#define FILEHANDLER_HPP

#include <sys/stat.h>
#include <fstream>

class Filehandler {

    public:
        Filehandler(const char* filename);
        inline bool exists();
        char* read(const unsigned int &position, const unsigned char &bytes);
        char* read(const unsigned int &position, std::_Ios_Seekdir way, const unsigned char &bytes);
        ~Filehandler();

    private:
        const char* filename;
        std::ifstream stream;


};

#endif // FILEHANDLER_HPP
