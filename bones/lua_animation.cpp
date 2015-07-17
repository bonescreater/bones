#include "lua_animation.h"
#include "core/animation.h"
#include "core/logging.h"
#include "lua_context.h"
#include "lua_check.h"

namespace bones
{

struct  LuaAnimationMethod
{
    std::string run;
    std::string start;
    std::string stop;
    std::string pause;
    std::string resume;
};

//(self, interval, due, method_name)
void AnimateRun(Ref * sender, Ref * target, uint64_t due, void * user_data)
{
    if (!user_data)
        return;
    auto & method = (static_cast<LuaAnimationMethod *>(user_data))->run;
    if (method.empty())
        return;
    auto l = LuaContext::State();
    LUA_STACK_AUTO_CHECK(l);
    //call lua method
    LuaContext::GetLOFromCO(l, target);
    lua_getfield(l, -1, method.data());
    assert(lua_isfunction(l, -1));

    lua_pushlightuserdata(l, sender);
    lua_pushnil(l);
    lua_copy(l, -4, -1);
    lua_pushinteger(l, due);
    //no user data like c++
    LuaContext::SafeLOPCall(l, 3, 0);

    lua_pop(l, 1);
}

void AnimateStart(Ref * sender, Ref * target, void * user_data)
{
    if (!user_data)
        return;
    auto & method = (static_cast<LuaAnimationMethod *>(user_data))->start;
    if (method.empty())
        return;
    auto l = LuaContext::State();
    LUA_STACK_AUTO_CHECK(l);
    //call lua method
    LuaContext::GetLOFromCO(l, target);
    lua_getfield(l, -1, method.data());
    assert(lua_isfunction(l, -1));
    lua_pushlightuserdata(l, sender);
    lua_pushnil(l);
    lua_copy(l, -4, -1);
    //no user data like c++
    LuaContext::SafeLOPCall(l, 2, 0);
    lua_pop(l, 1);
}

void AnimateStop(Ref * sender, Ref * target, void * user_data)
{
    if (!user_data)
        return;
    auto & method = (static_cast<LuaAnimationMethod *>(user_data))->stop;
    if (method.empty())
        return;
    auto l = LuaContext::State();
    LUA_STACK_AUTO_CHECK(l);
    //call lua method
    LuaContext::GetLOFromCO(l, target);
    lua_getfield(l, -1, method.data());
    assert(lua_isfunction(l, -1));
    lua_pushlightuserdata(l, sender);
    lua_pushnil(l);
    lua_copy(l, -4, -1);
    //no user data like c++
    LuaContext::SafeLOPCall(l, 2, 0);
    lua_pop(l, 1);

    if (user_data)
        delete static_cast<LuaAnimationMethod *>(user_data);
}

void AnimatePause(Ref * sender, Ref * target, void * user_data)
{
    if (!user_data)
        return;
    auto & method = (static_cast<LuaAnimationMethod *>(user_data))->pause;
    if (method.empty())
        return;
    auto l = LuaContext::State();
    LUA_STACK_AUTO_CHECK(l);
    //call lua method
    LuaContext::GetLOFromCO(l, target);
    lua_getfield(l, -1, method.data());
    assert(lua_isfunction(l, -1));
    lua_pushlightuserdata(l, sender);
    lua_pushnil(l);
    lua_copy(l, -4, -1);
    //no user data like c++
    LuaContext::SafeLOPCall(l, 2, 0);
    lua_pop(l, 1);
}

void AnimateResume(Ref * sender, Ref * target, void * user_data)
{
    if (!user_data)
        return;
    auto & method = (static_cast<LuaAnimationMethod *>(user_data))->resume;
    if (method.empty())
        return;
    auto l = LuaContext::State();
    LUA_STACK_AUTO_CHECK(l);
    //call lua method
    LuaContext::GetLOFromCO(l, target);
    lua_getfield(l, -1, method.data());
    assert(lua_isfunction(l, -1));
    lua_pushlightuserdata(l, sender);
    lua_pushnil(l);
    lua_copy(l, -4, -1);
    //no user data like c++
    LuaContext::SafeLOPCall(l, 2, 0);
    lua_pop(l, 1);
}

Animation * LuaAnimation::Create(
                                Ref * target, uint64_t interval, uint64_t due,
                                const char * run,
                                const char * stop,
                                const char * start,
                                const char * pause,
                                const char * resume)
{
    auto ani = new CommonAnimation(target, interval, due);
    if (!run && !start && !stop && !pause && !resume)
        return ani;
    auto method = new LuaAnimationMethod;

    if (run && strlen(run))
    {
        method->run = run;
        ani->bind(BONES_CALLBACK_4(&AnimateRun), method);
    }

    if (stop && strlen(stop))
    {
        method->stop = stop;
        ani->bind(CommonAnimation::kStop, BONES_CALLBACK_3(&AnimateStop), method);
    }
        
    if (start && strlen(start))
    {
        method->start = start;
        ani->bind(CommonAnimation::kStart, BONES_CALLBACK_3(&AnimateStart), method);
    }

    if (pause && strlen(pause))
    {
        method->pause = pause;
        ani->bind(CommonAnimation::kPause, BONES_CALLBACK_3(&AnimatePause), method);
    }

    if (resume && strlen(resume))
    {
        method->resume = resume;
        ani->bind(CommonAnimation::kResume, BONES_CALLBACK_3(&AnimateResume), method);
    }
    return ani;
}


}