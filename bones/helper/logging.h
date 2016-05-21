#ifndef BONES_HELPER_LOGGING_H_
#define BONES_HELPER_LOGGING_H_

#include <fstream>

namespace bones
{


class Log
{
public:
    enum Level
    {
        kNone = 0,
        kError = 1,// only error
        kVerbose = 2,// everything
    };
public:
    static bool StartUp(const wchar_t * file_name, Level l);
    
    static void ShutDown();

    Log(Level level);

    ~Log();

    template<typename T>
    Log & operator<<(const T & t)
    {
        if (log && level_ <= log_level)
            log << t;
        return *this;
    }
private:
    Level level_;
private:
    static Level log_level;
    static std::ofstream log;
};

#define BLG_ERROR Log (Log::kError) <<"[LEVEL:ERROR]"<< " "
#define BLG_VERBOSE Log (Log::kVerbose) <<"[LEVEL:VERBOSE]"<< " "

}


#endif