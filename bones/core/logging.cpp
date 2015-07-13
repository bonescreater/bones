#include "logging.h"

namespace bones
{

LogLevel Log::log_level = kLOG_LEVEL_NONE;

std::ofstream Log::log;

bool Log::StartUp(const wchar_t * file_name, LogLevel level)
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
    log_level = kLOG_LEVEL_NONE;
    log.close();
}

Log::Log(LogLevel level)
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