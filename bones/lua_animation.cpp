﻿#include "lua_animation.h"
#include "core/animation.h"
#include "core/view.h"
#include "lua_context.h"
#include "lua_check.h"
#include "script_parser.h"
#include "lua_meta_table.h"

namespace bones
{

LuaAnimation::LuaAnimation(
    View * target,
    uint64_t interval, uint64_t due,
    BonesAnimation::RunListener * run,
    BonesAnimation::ActionListener * stop,
    BonesAnimation::ActionListener * start,
    BonesAnimation::ActionListener * pause,
    BonesAnimation::ActionListener * resume,
    AnimationType type)
    :run_(run), stop_(stop), start_(start), pause_(pause), resume_(resume),
    type_(type)
{
    animation_ = new Animation(target, interval, due);
    animation_->bind(
        BONES_CLASS_CALLBACK_3(&LuaAnimation::run, this));
    animation_->bind(Animation::kStop, 
        BONES_CLASS_CALLBACK_2(&LuaAnimation::stop, this));
    animation_->bind(Animation::kStart,
        BONES_CLASS_CALLBACK_2(&LuaAnimation::start, this));
    animation_->bind(Animation::kPause,
        BONES_CLASS_CALLBACK_2(&LuaAnimation::pause, this));
    animation_->bind(Animation::kResume,
        BONES_CLASS_CALLBACK_2(&LuaAnimation::resume, this));

    LuaMetaTable::CreateLuaTable(LuaContext::State(), this);
}

LuaAnimation::~LuaAnimation()
{
    LuaMetaTable::RemoveLuaTable(LuaContext::State(), this);
    typedef void(*CFRUN)(Animation * sender, View * target, float progress);
    typedef void(*CFROUTINE)(Animation * sender, View * target);
    animation_->bind(
        BONES_CALLBACK_3((CFRUN)nullptr));
    animation_->bind(Animation::kStop,
        BONES_CALLBACK_2((CFROUTINE)nullptr));
    animation_->bind(Animation::kStart,
        BONES_CALLBACK_2((CFROUTINE)nullptr));
    animation_->bind(Animation::kPause,
        BONES_CALLBACK_2((CFROUTINE)nullptr));
    animation_->bind(Animation::kResume,
        BONES_CALLBACK_2((CFROUTINE)nullptr));

    animation_->release();
}

void LuaAnimation::retain()
{
    Ref::retain();
}

void LuaAnimation::release()
{
    Ref::release();
}

Animation * LuaAnimation::ani()
{
    return animation_;
}

void LuaAnimation::run(Animation * sender, View * target, float progress)
{
    auto bo = GetCoreInstance()->getObject(animation_->target());
    assert(bo);
    if (kANI_NATIVE == type_)
        run_? run_->onEvent(this, bo, progress) : 0;
    else if (kANI_SCRIPT == type_)
    {
        auto l = LuaContext::State();
        LUA_STACK_AUTO_CHECK(l);
        LuaContext::GetLOFromCO(l, this);
        lua_getfield(l, -1, kMethodAnimateRun);
        lua_pushnil(l);
        LuaContext::GetLOFromCO(l, bo);
        lua_pushnumber(l, progress);
        lua_copy(l, -5, -3);
        LuaContext::SafeLOPCall(l, 3, 0);
        lua_pop(l, 1);
    }

}

void LuaAnimation::stop(Animation * sender, View * target)
{
    auto bo = GetCoreInstance()->getObject(animation_->target());
    assert(bo);
    if (kANI_NATIVE == type_)
        stop_ ? stop_->onEvent(this, bo, BonesAnimation::kStop) : 0;
    else if (kANI_SCRIPT == type_)
    {
        auto l = LuaContext::State();
        LUA_STACK_AUTO_CHECK(l);
        LuaContext::GetLOFromCO(l, this);
        lua_getfield(l, -1, kMethodAnimateStop);
        lua_pushnil(l);
        LuaContext::GetLOFromCO(l, bo);
        lua_copy(l, -4, -2);
        LuaContext::SafeLOPCall(l, 2, 0);
        lua_pop(l, 1);
    }
    release();
}

void LuaAnimation::start(Animation * sender, View * target)
{
    auto bo = GetCoreInstance()->getObject(animation_->target());
    assert(bo);
    if (kANI_NATIVE == type_)
        start_ ? start_->onEvent(this, bo, BonesAnimation::kStart) : 0;
    else if (kANI_SCRIPT == type_)
    {
        auto l = LuaContext::State();
        LUA_STACK_AUTO_CHECK(l);
        LuaContext::GetLOFromCO(l, this);
        lua_getfield(l, -1, kMethodAnimateStart);
        lua_pushnil(l);
        LuaContext::GetLOFromCO(l, bo);
        lua_copy(l, -4, -2);
        LuaContext::SafeLOPCall(l, 2, 0);
        lua_pop(l, 1);
    }
}

void LuaAnimation::pause(Animation * sender, View * target)
{
    auto bo = GetCoreInstance()->getObject(animation_->target());
    assert(bo);
    if (kANI_NATIVE == type_)
        pause_ ? pause_->onEvent(this, bo, BonesAnimation::kPause) : 0;
    else if (kANI_SCRIPT == type_)
    {
        auto l = LuaContext::State();
        LUA_STACK_AUTO_CHECK(l);
        LuaContext::GetLOFromCO(l, this);
        lua_getfield(l, -1, kMethodAnimatePause);
        lua_pushnil(l);
        LuaContext::GetLOFromCO(l, bo);
        lua_copy(l, -4, -2);
        LuaContext::SafeLOPCall(l, 2, 0);
        lua_pop(l, 1);
    }
}

void LuaAnimation::resume(Animation * sender, View * target)
{
    auto bo = GetCoreInstance()->getObject(animation_->target());
    assert(bo);
    if (kANI_NATIVE == type_)
        resume_ ? resume_->onEvent(this, bo, BonesAnimation::kResume) : 0;
    else if (kANI_SCRIPT == type_)
    {
        auto l = LuaContext::State();
        LUA_STACK_AUTO_CHECK(l);
        LuaContext::GetLOFromCO(l, this);
        lua_getfield(l, -1, kMethodAnimateResume);
        lua_pushnil(l);
        LuaContext::GetLOFromCO(l, bo);
        lua_copy(l, -4, -2);
        LuaContext::SafeLOPCall(l, 2, 0);
        lua_pop(l, 1);
    }
}


}