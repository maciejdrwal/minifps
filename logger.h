#ifndef _LOGGER_H_
#define _LOGGER_H_

#include <fstream>
#include <iomanip>

class Logger
{
public:
    Logger(const char * filename) : _i(0) { this->fout.open(filename); }
    ~Logger() { this->fout.close(); }
    
    void log(const std::string & msg) 
    {
        auto t = std::time(nullptr);
        this->fout << _i++ << ": " << std::put_time(std::localtime(&t), "%F %T%z") << "    " << msg << std::endl; 
    };

private:
    std::ofstream fout;
    unsigned long _i;
};

#endif
