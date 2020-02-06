#ifndef _LOGGER_H_
#define _LOGGER_H_

#include <fstream>
#include <iomanip>

class Logger
{
public:
    Logger(const std::string& filename) : m_i(0) { m_fout.open(filename); }
    ~Logger() { m_fout.close(); }
    
    void log(const std::string& msg)
    {
        const auto t = std::time(nullptr);
        m_fout << m_i++ << ": " << std::put_time(std::localtime(&t), "%F %T%z") << "    " << msg << std::endl; 
    };

private:
    std::ofstream m_fout;
    unsigned long m_i;
};

#endif
