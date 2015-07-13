#ifndef BONES_LUA_PANEL_H_
#define BONES_LUA_PANEL_H_

#include "bones.h"

namespace bones
{
class Panel;
class LuaPanel
{
public:
    static void Create(Panel * panel);

    static void RegisterEvent(Panel * co,
                              const char * name,
                              const char * phase,
                              const char * module,
                              const char * func);
};

}
#endif