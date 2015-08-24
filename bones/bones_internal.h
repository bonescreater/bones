#ifndef BONES_INTERNAL_H_
#define BONES_INTERNAL_H_

#include "bones.h"
#include "script_parser.h"

namespace bones
{
extern ScriptParser * GetCoreInstance();

extern const char * kNotifyOrder;
}

#endif