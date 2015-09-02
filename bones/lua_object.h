#ifndef BONES_LUA_OBJECT_H_
#define BONES_LUA_OBJECT_H_

#include "bones_internal.h"
#include "core/view.h"
#include "core/logging.h"
#include "core/css_manager.h"
#include "script_parser.h"

namespace bones
{


template<class Base, class Listener, class T>
class LuaObject : public Base, public Ref
{
public:
    LuaObject(T * v, const char * meta)
    {
        object_.reset(v);
        LuaMetaTable::CreatLuaTable(LuaContext::State(), meta, this);
        LOG_VERBOSE << "create:" << object_->getClassName() << object_.get();
    }

    virtual ~LuaObject()
    {
        LuaMetaTable::RemoveLuaTable(LuaContext::State(), this);
        LOG_VERBOSE << "destroy:" << object_->getClassName() << object_.get();
    }

    void notifyPrepare()
    {
        bool stop = false;
        getNotify() ? getNotify()->onPrepare(this, stop) : 0;
        if (stop)
            return;
        auto l = LuaContext::State();
        LUA_STACK_AUTO_CHECK(l);
        LuaContext::GetLOFromCO(l, this);
        lua_getfield(l, -1, kNotifyOrder);
        if (lua_istable(l, -1))
        {
            lua_getfield(l, -1, kMethodOnPrepare);
            lua_pushnil(l);
            lua_copy(l, -4, -1);
            auto count = LuaContext::SafeLOPCall(l, 1, 0);
            lua_pop(l, count);
        }
        lua_pop(l, 2);
    }

    void addNotify(const char * notify_name,
        const char * mod,
        const char * func)
    {
        if (!notify_name || !func)
            return;
        auto l = LuaContext::State();
        LUA_STACK_AUTO_CHECK(l);
        LuaContext::GetLOFromCO(l, this);
        assert(lua_istable(l, -1));
        lua_getfield(l, -1, kNotifyOrder);
        if (!lua_istable(l, -1))
        {
            lua_pop(l, 1);
            lua_newtable(l);
            assert(lua_istable(l, -2));
            lua_setfield(l, -2, kNotifyOrder);
            lua_getfield(l, -1, kNotifyOrder);
        }
        lua_pushnil(l);
        if (mod)
        {
            lua_getglobal(l, mod);
            assert(lua_istable(l, -1));
            lua_getfield(l, -1, func);
            lua_copy(l, -1, -3);
            lua_pop(l, 2);
        }
        else
        {
            lua_getglobal(l, func);
            lua_copy(l, -1, -2);
            lua_pop(l, 1);
        }
        assert(lua_isfunction(l, -1));
        lua_setfield(l, -2, notify_name);
        lua_pop(l, 2);
    }

    virtual Listener * getNotify() const = 0;

    void retain() override
    {
        Ref::retain();
    }

    void release() override
    {
        Ref::release();
    }

    const char * getClassName() override
    {
        return object_ ? object_->getClassName() : nullptr;
    }

    void * cast() override
    {
        return static_cast<Base *>(this);
    }

    void listen(const char * name, BonesScriptListener * listener) override
    {
        GetCoreInstance()->listen(this, name, listener);
    }

    void push(BonesScriptArg * arg) override
    {
        GetCoreInstance()->push(arg);
    }

    float getOpacity() const override
    {
        return object_->getOpacity();
    }

    BonesPoint getLoc() const override
    {
        BonesPoint bp = { object_->getLeft(), object_->getTop() };
        return bp;
    }

    BonesSize getSize() const override
    {
        BonesSize bs = { object_->getWidth(), object_->getHeight() };
        return bs;
    }

    void setVisible(bool visible) override
    {
        object_->setVisible(visible);
    }

    void setFocusable(bool focusable)
    {
        object_->setFocusable(focusable);
    }

    bool contains(Scalar x, Scalar y) override
    {
        return object_->contains(Point::Make(x, y));
    }

    BonesObject * getChildAt(size_t index) override
    {
        auto child = object_->getChildAt(index);
        if (child)
            return GetCoreInstance()->getObject(child);
        return nullptr;
    }

    void applyCSS(const char * css) override
    {
        Core::GetCSSManager()->applyCSS(object_.get(), css);
    }

    void applyClass(const char * name) override
    {
        Core::GetCSSManager()->applyClass(object_.get(), name);
    }

    BonesAnimation * animate(
        uint64_t interval, uint64_t due,
        BonesAnimation::RunListener * run,
        BonesAnimation::ActionListener * stop,
        BonesAnimation::ActionListener * start,
        BonesAnimation::ActionListener * pause,
        BonesAnimation::ActionListener * resume)  override
    {
        auto ani = GetCoreInstance()->createAnimate(
            this, interval, due, run, stop, start, pause, resume, 
            kANI_NATIVE);
        GetCoreInstance()->startAnimate(ani);
        return ani;
    }

    void stopAnimate(BonesAnimation * ani, bool toend)
    {
        GetCoreInstance()->stopAnimate(ani, toend);
    }

    void pauseAnimate(BonesAnimation * ani)  override
    {
        GetCoreInstance()->pauseAnimate(ani);
    }

    void resumeAnimate(BonesAnimation * ani)  override
    {
        GetCoreInstance()->resumeAnimate(ani);
    }

    void stopAllAnimate(bool toend)  override
    {
        GetCoreInstance()->stopAllAnimate(this, toend);
    }
protected:
    RefPtr<T> object_;
};

}
#endif