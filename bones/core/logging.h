#ifndef BONES_CORE_LOGGING_H_
#define BONES_CORE_LOGGING_H_

#include <sstream>

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
    static void AlterLog(Level l);
private:
    static Level global_level;
public:
    Log(Level level);

    ~Log();

    template<typename T>
    Log & operator<<(const T & t)
    {
        if (log && )
            log << t;
        return *this;
    }
private:
    void writeToFile();
private:
    Level level_;
    std::ostringstream * log;
};


extern std::string GetTimeStamp();

#define BLG_ERROR Log (Log::kError) <<"[LEVEL:ERROR] " << ::GetThreadId() <<"Time:"<< GetTimeStamp()<<" "
#define BLG_VERBOSE Log (Log::kVerbose) <<"[LEVEL:VERBOSE] " << ::GetThreadId() <<"Time:"<< GetTimeStamp()<<" "

}


#endif