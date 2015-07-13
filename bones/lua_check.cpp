#include "lua_check.h"
#include "core/logging.h"

namespace bones
{


LuaStackAutoCheck::LuaStackAutoCheck(lua_State * l, int count)
:l_(l), count_(count)
{
    top_ = lua_gettop(l_);
}

LuaStackAutoCheck::~LuaStackAutoCheck()
{
    auto top = lua_gettop(l_);
    if (top - top_ != count_)
    {
        LOG_ERROR <<"stack error\n";
        assert(0);
    }

}

}