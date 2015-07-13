#ifndef BONES_CORE_LOGGING_H_
#define BONES_CORE_LOGGING_H_

#include "core.h"
#include <fstream>

namespace bones
{


class Log
{
public:
    static bool StartUp(const wchar_t * file_name, LogLevel l);
    
    static void ShutDown();

    Log(LogLevel level);

    ~Log();

    template<typename T>
    Log & operator<<(const T & t)
    {
        if (log && level_ <= log_level)
            log << t;
        return *this;
    }
private:
    LogLevel level_;
private:
    static LogLevel log_level;
    static std::ofstream log;
};

#define LOG_ERROR Log (kLOG_LEVEL_ERROR) <<"[LEVEL:ERROR]"<< __FILE__ <<" " <<__LINE__<<" "
#define LOG_VERBOSE Log (kLOG_LEVEL_VERBOSE) <<"[LEVEL:VERBOSE]"<< __FILE__ <<" " <<__LINE__<< " "

}


#endif