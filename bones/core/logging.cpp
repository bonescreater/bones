#include "logging.h"
#include "core/mutex.h"
#include <time.h>
#include <fstream>
#include <Windows.h>

namespace bones
{

Log::Level Log::global_level = kNone;

Mutex mutex;

void Log::AlterLog(Level l)
{
    global_level = l;
}

Log::Log(Log::Level level)
:level_(level), log(nullptr)
{
    if (level_ <= global_level)
        log = new std::ostringstream();
}

Log::~Log()
{
    writeToFile();
    if (log)
        delete log;
}

void Log::writeToFile()
{
    if (!log)
        return;
    static std::string file_path = ".\\bones.log";
    mutex.acquire();

    std::ofstream ofs;
    ofs.open(file_path);
    if (ofs)
    {
        ofs << log;
        ofs.flush();
        ofs.close();
    }
    mutex.release();
}

std::string GetTimeStamp()
{
    time_t currentTime = time(NULL);
    tm t;
    localtime_s(&t, &currentTime);
    char buf[100] = { 0 };
    memset(buf, 0, sizeof(buf));
    sprintf_s(buf, sizeof(buf),
        "[%04d-%02d-%02d %02d:%02d:%02d]", 1900 + t.tm_year, 1 + t.tm_mon, t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec);
    return std::string(buf);
}

}