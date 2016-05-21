#include "logging.h"

namespace bones
{

Log::Level Log::log_level = kNone;

std::ofstream Log::log;

bool Log::StartUp(const wchar_t * file_name, Log::Level level)
{
    log_level = level;
    bool bret = true;
    if (file_name)
    {
        log.open(file_name);
        bret = !!log;
    }
    return bret;
}

void Log::ShutDown()
{
    log_level = kNone;
    log.close();
}

Log::Log(Log::Level level)
:level_(level)
{

}

Log::~Log()
{
    if (log)
    {
        log << "\n";
        log.flush();
    }
        
}

}