#ifndef FILEHANDLER_HPP
#define FILEHANDLER_HPP

#include <sys/stat.h>
#include <fstream>

class Filehandler {

    public:
        Filehandler(const char* filename);
        inline bool exists();
        ~Filehandler();

    private:
        const char* filename;
        std::ifstream stream;


};

#endif // FILEHANDLER_HPP
