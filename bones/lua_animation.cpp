#include "lua_animation.h"
#include "core/animation.h"
#include "core/view.h"
#include "lua_context.h"
#include "lua_check.h"
#include "script_parser.h"

namespace bones
{
static const char * kMetaTableAnimation = "__mt_animation";

LuaAnimation::LuaAnimation(
    View * target,
    uint64_t interval, uint64_t due,
    BonesObject::AnimationRunListener * run,
    BonesObject::AnimationActionListener * stop,
    BonesObject::AnimationActionListener * start,
    BonesObject::AnimationActionListener * pause,
    BonesObject::AnimationActionListener * resume,
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

    createLuaTable();
}

LuaAnimation::~LuaAnimation()
{
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
    auto l = LuaContext::State();
    if (kANI_NATIVE == type_)
        stop_ ? stop_->onEvent(this, bo, BonesObject::kStop) : 0;
    else if (kANI_SCRIPT == type_)
    {       
        LUA_STACK_AUTO_CHECK(l);
        LuaContext::GetLOFromCO(l, this);
        lua_getfield(l, -1, kMethodAnimateStop);
        lua_pushnil(l);
        LuaContext::GetLOFromCO(l, bo);
        lua_copy(l, -4, -2);
        LuaContext::SafeLOPCall(l, 2, 0);
        lua_pop(l, 1);
    }
    LuaContext::ClearLOFromCO(l, this);
    release();
}

void LuaAnimation::start(Animation * sender, View * target)
{
    auto bo = GetCoreInstance()->getObject(animation_->target());
    assert(bo);
    if (kANI_NATIVE == type_)
        start_ ? start_->onEvent(this, bo, BonesObject::kStart) : 0;
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
        pause_ ? pause_->onEvent(this, bo, BonesObject::kPause) : 0;
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
        resume_ ? resume_->onEvent(this, bo, BonesObject::kResume) : 0;
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

static int GC(lua_State * l)
{
    int count = lua_gettop(l);
    if (count == 1)
    {
        lua_pushnil(l);
        lua_copy(l, 1, -1);
        LuaAnimation * ref = static_cast<LuaAnimation *>(
            LuaContext::CallGetCObject(l));
        if (ref)
            ref->release();
    }
    return 0;
}

void LuaAnimation::createLuaTable()
{
    //将自己添加到LuaTable中去
    auto l = LuaContext::State();
    LUA_STACK_AUTO_CHECK(l);
    LuaContext::GetCO2LOTable(l);
    lua_pushlightuserdata(l, this);
    lua_newtable(l);//1
    luaL_getmetatable(l, kMetaTableAnimation);
    if (!lua_istable(l, -1))
    {
        lua_pop(l, 1);
        luaL_newmetatable(l, kMetaTableAnimation);
        lua_pushnil(l);
        lua_copy(l, -2, -1);
        lua_setfield(l, -2, kMethodIndex);
        lua_pushcfunction(l, &GC);
        lua_setfield(l, -2, kMethodGC);
    }
    lua_setmetatable(l, -2);
    this->retain();
    //防止返回的table 被脚本保存  而自己stop以后内存释放
    //新启动的luaAnimation 有可能内存地址还是一样的 必须table释放之后
    //才能释放luaAnimation
    LuaContext::SetGetCObject(l, this);

    lua_settable(l, -3);
    lua_pop(l, 1);
}


}