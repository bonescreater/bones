#include "lua_animation.h"
#include "core/animation.h"
#include "core/logging.h"
#include "core/animation_manager.h"

#include "lua_context.h"
#include "lua_check.h"
#include "lua_meta_table.h"

namespace bones
{

struct  LuaAnimationMethod
{
    std::string run;
    std::string run_module;
    std::string start;
    std::string start_module;
    std::string stop;
    std::string stop_module;
    std::string pause;
    std::string pause_module;
    std::string resume;
    std::string resume_module;
};

//(self, interval, due, method_name)
void AnimateRun(Ref * sender, Ref * target, float progress, void * user_data)
{
    if (!user_data)
        return;
    auto & method = (static_cast<LuaAnimationMethod *>(user_data))->run;
    auto & method_module = (static_cast<LuaAnimationMethod *>(user_data))->run_module;
    if (method.empty())
        return;
    auto l = LuaContext::State();
    LUA_STACK_AUTO_CHECK(l);
    //call lua method
    if (method_module.empty())
    {//没有模块 则方法就是全局变量里的
        lua_getglobal(l, method.data());
    }
    else
    {//有模块
        lua_getglobal(l, method_module.data());
        assert(lua_istable(l, -1));
        if (!lua_istable(l, -1))
        {
            LOG_ERROR << "can't find animate callback module: " << method_module.data();
            lua_pop(l, 1);
            return;
        }
        lua_getfield(l, -1, method.data());
    }
    assert(lua_isfunction(l, -1));
    LuaContext::GetLOFromCO(l, sender);
    LuaContext::GetLOFromCO(l, target);
    lua_pushnumber(l, progress);
    //no user data like c++
    LuaContext::SafeLOPCall(l, 3, 0);
    if (!method_module.empty())
        lua_pop(l, 1);
}

void AnimateStart(Ref * sender, Ref * target, void * user_data)
{
    if (!user_data)
        return;
    auto & method = (static_cast<LuaAnimationMethod *>(user_data))->start;
    auto & method_module = (static_cast<LuaAnimationMethod *>(user_data))->start_module;
    if (method.empty())
        return;
    auto l = LuaContext::State();
    LUA_STACK_AUTO_CHECK(l);
    //call lua method
    if (method_module.empty())
    {//没有模块 则方法就是全局变量里的
        lua_getglobal(l, method.data());
    }
    else
    {//有模块
        lua_getglobal(l, method_module.data());
        assert(lua_istable(l, -1));
        if (!lua_istable(l, -1))
        {
            LOG_ERROR << "can't find animate callback module: " << method_module.data();
            lua_pop(l, 1);
            return;
        }
        lua_getfield(l, -1, method.data());
    }
    assert(lua_isfunction(l, -1));
    LuaContext::GetLOFromCO(l, sender);
    LuaContext::GetLOFromCO(l, target);
    //no user data like c++
    LuaContext::SafeLOPCall(l, 2, 0);

    if (!method_module.empty())
        lua_pop(l, 1);
}

void AnimateStop(Ref * sender, Ref * target, void * user_data)
{
    if (!user_data)
        return;
    auto & method = (static_cast<LuaAnimationMethod *>(user_data))->stop;
    auto & method_module = (static_cast<LuaAnimationMethod *>(user_data))->stop_module;
    if (method.empty())
        return;
    auto l = LuaContext::State();
    LUA_STACK_AUTO_CHECK(l);
    //call lua method
    if (method_module.empty())
    {//没有模块 则方法就是全局变量里的
        lua_getglobal(l, method.data());
    }
    else
    {//有模块
        lua_getglobal(l, method_module.data());
        assert(lua_istable(l, -1));
        if (!lua_istable(l, -1))
        {
            LOG_ERROR << "can't find animate callback module: " << method_module.data();
            lua_pop(l, 1);
            return;
        }
        lua_getfield(l, -1, method.data());
    }
    assert(lua_isfunction(l, -1));
    LuaContext::GetLOFromCO(l, sender);
    LuaContext::GetLOFromCO(l, target);
    //no user data like c++
    LuaContext::SafeLOPCall(l, 2, 0);
    if (!method_module.empty())
        lua_pop(l, 1);

    //
    LuaContext::GetCO2LOTable(l);
    lua_pushlightuserdata(l, sender);
    lua_pushnil(l);
    lua_settable(l, -3);
    lua_pop(l, 1);

    if (user_data)
        delete static_cast<LuaAnimationMethod *>(user_data);
}

void AnimatePause(Ref * sender, Ref * target, void * user_data)
{
    if (!user_data)
        return;
    auto & method = (static_cast<LuaAnimationMethod *>(user_data))->pause;
    auto & method_module = (static_cast<LuaAnimationMethod *>(user_data))->pause_module;
    if (method.empty())
        return;
    auto l = LuaContext::State();
    LUA_STACK_AUTO_CHECK(l);
    //call lua method
    if (method_module.empty())
    {//没有模块 则方法就是全局变量里的
        lua_getglobal(l, method.data());
    }
    else
    {//有模块
        lua_getglobal(l, method_module.data());
        assert(lua_istable(l, -1));
        if (!lua_istable(l, -1))
        {
            LOG_ERROR << "can't find animate callback module: " << method_module.data();
            lua_pop(l, 1);
            return;
        }
        lua_getfield(l, -1, method.data());
    }
    assert(lua_isfunction(l, -1));
    LuaContext::GetLOFromCO(l, sender);
    LuaContext::GetLOFromCO(l, target);
    //no user data like c++
    LuaContext::SafeLOPCall(l, 2, 0);
    if (!method_module.empty())
        lua_pop(l, 1);
}

void AnimateResume(Ref * sender, Ref * target, void * user_data)
{
    if (!user_data)
        return;
    auto & method = (static_cast<LuaAnimationMethod *>(user_data))->resume;
    auto & method_module = (static_cast<LuaAnimationMethod *>(user_data))->resume_module;
    if (method.empty())
        return;
    auto l = LuaContext::State();
    LUA_STACK_AUTO_CHECK(l);
    //call lua method
    if (method_module.empty())
    {//没有模块 则方法就是全局变量里的
        lua_getglobal(l, method.data());
    }
    else
    {//有模块
        lua_getglobal(l, method_module.data());
        assert(lua_istable(l, -1));
        if (!lua_istable(l, -1))
        {
            LOG_ERROR << "can't find animate callback module: " << method_module.data();
            lua_pop(l, 1);
            return;
        }
        lua_getfield(l, -1, method.data());
    }
    assert(lua_isfunction(l, -1));
    LuaContext::GetLOFromCO(l, sender);
    LuaContext::GetLOFromCO(l, target);
    //no user data like c++
    LuaContext::SafeLOPCall(l, 2, 0);
    if (!method_module.empty())
        lua_pop(l, 1);
}

void LuaAnimation::Create(Animation * co,
                          const char * run,
                          const char * run_module,
                          const char * stop,
                          const char * stop_module,
                          const char * start,
                          const char * start_module,
                          const char * pause,
                          const char * pause_module,
                          const char * resume,
                          const char * resume_module)
{
    auto l = LuaContext::State();
    LUA_STACK_AUTO_CHECK(l);
    LuaContext::GetCO2LOTable(l);
    lua_pushlightuserdata(l, co);
    lua_newtable(l);
    LuaMetaTable::GetAnimation(l);
    lua_setmetatable(l, -2);
    LuaMetaTable::SetClosureCObject(l, co);
    lua_settable(l, -3);
    lua_pop(l, 1);
    if (run || start || stop || pause || resume)
    {
        auto method = new LuaAnimationMethod;

        if (run && strlen(run))
        {
            method->run = run;
            if (run_module && strlen(run_module))
                method->run_module = run_module;
            co->bind(BONES_CALLBACK_4(&AnimateRun), method);
        }

        if (stop && strlen(stop))
        {
            method->stop = stop;
            if (stop_module && strlen(stop_module))
                method->stop_module = stop_module;
            co->bind(Animation::kStop, BONES_CALLBACK_3(&AnimateStop), method);
        }

        if (start && strlen(start))
        {
            method->start = start;
            if (start_module && strlen(start_module))
                method->start_module = start_module;
            co->bind(Animation::kStart, BONES_CALLBACK_3(&AnimateStart), method);
        }

        if (pause && strlen(pause))
        {
            method->pause = pause;
            if (pause_module && strlen(pause_module))
                method->pause_module = pause_module;
            co->bind(Animation::kPause, BONES_CALLBACK_3(&AnimatePause), method);
        }

        if (resume && strlen(resume))
        {
            method->resume = resume;
            if (resume_module && strlen(resume_module))
                method->resume_module = resume_module;
            co->bind(Animation::kResume, BONES_CALLBACK_3(&AnimateResume), method);
        }
    }

    Core::GetAnimationManager()->add(co);
    co->release();
}


}