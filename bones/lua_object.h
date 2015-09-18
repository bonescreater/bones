#ifndef BONES_LUA_OBJECT_H_
#define BONES_LUA_OBJECT_H_

#include "bones_internal.h"
#include "core/view.h"
#include "core/logging.h"
#include "core/css_manager.h"
#include "script_parser.h"
#include "lua_context.h"
#include "lua_check.h"

namespace bones
{


template<class Base, class Listener, class T>
class LuaObject : public Base, public Ref
{
public:
    LuaObject(T * v)
    {
        object_.reset(v);
        BLG_VERBOSE << "create:" << object_->getClassName() << object_.get();
    }

    virtual ~LuaObject()
    {
        BLG_VERBOSE << "destroy:" << object_->getClassName() << object_.get();
    }

    virtual void notifyCreate()
    {
        bool stop = false;
        getNotify() ? getNotify()->onCreate(this, stop) : 0;
        if (stop)
            return;
        auto l = LuaContext::State();
        LUA_STACK_AUTO_CHECK(l);
        LuaContext::GetLOFromCO(l, this);
        lua_getfield(l, -1, kNotifyOrder);
        if (lua_istable(l, -1))
        {
            lua_getfield(l, -1, kMethodOnCreate);
            lua_pushnil(l);
            lua_copy(l, -4, -1);
            auto count = LuaContext::SafeLOPCall(l, 1, 0);
            lua_pop(l, count);
        }
        lua_pop(l, 2);
    }

    virtual void notifyDestroy()
    {
        bool stop = false;
        getNotify() ? getNotify()->onDestroy(this, stop) : 0;
        if (stop)
            return;
        auto l = LuaContext::State();
        LUA_STACK_AUTO_CHECK(l);
        LuaContext::GetLOFromCO(l, this);
        lua_getfield(l, -1, kNotifyOrder);
        if (lua_istable(l, -1))
        {
            lua_getfield(l, -1, kMethodOnDestroy);
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
        if (kClassScroller == object_->getClassName())
            return GetCoreInstance()->getObject(
                object_->getChildAt(0)->getChildAt(index));
        else
            return GetCoreInstance()->getObject(object_->getChildAt(index));
    }

    BonesObject * getParent() override
    {
        auto parent = object_->parent();
        auto pparent = parent->parent();
        if (pparent && kClassScroller == pparent->getClassName())
            parent = pparent;
        return GetCoreInstance()->getObject(parent);
    }

    void applyCSS(const char * css) override
    {
        Core::GetCSSManager()->applyCSS(object_.get(), css);
    }

    void applyClass(const char * name) override
    {
        Core::GetCSSManager()->applyClass(object_.get(), name);
    }

    BonesObject::Animation animate(
        uint64_t interval, uint64_t due,
        BonesObject::AnimationRunListener * run,
        BonesObject::AnimationActionListener * stop,
        BonesObject::AnimationActionListener * start,
        BonesObject::AnimationActionListener * pause,
        BonesObject::AnimationActionListener * resume)  override
    {
        auto ani = GetCoreInstance()->createAnimate(
            this, interval, due, run, stop, start, pause, resume, 
            kANI_NATIVE);
        GetCoreInstance()->startAnimate(ani);
        return ani;
    }

    void stopAnimate(BonesObject::Animation ani, bool toend) override
    {
        GetCoreInstance()->stopAnimate(ani, toend);
    }

    void pauseAnimate(BonesObject::Animation ani)  override
    {
        GetCoreInstance()->pauseAnimate(ani);
    }

    void resumeAnimate(BonesObject::Animation ani)  override
    {
        GetCoreInstance()->resumeAnimate(ani);
    }

    void stopAllAnimate(bool toend)  override
    {
        GetCoreInstance()->stopAllAnimate(this, toend);
    }
    // lua 封装模版
    void createLuaTable()
    {
        //将自己添加到LuaTable中去
        auto l = LuaContext::State();
        LUA_STACK_AUTO_CHECK(l);
        LuaContext::GetCO2LOTable(l);
        lua_pushlightuserdata(l, this);
        lua_newtable(l);//1
        createMetaTable(l);
        lua_setmetatable(l, -2);
        //lua table增加引用计数
        this->retain();
        LuaContext::SetGetCObject(l, this);
        lua_settable(l, -3);
        lua_pop(l, 1);
    }

    virtual void createMetaTable(lua_State * l) = 0;

    bool createMetaTable(lua_State * l, const char * meta)
    {
        LUA_STACK_AUTO_CHECK_COUNT(l, 1);
        luaL_getmetatable(l, meta);
        bool exist = lua_istable(l, -1);
        if (!exist)
        {
            lua_pop(l, 1);
            luaL_newmetatable(l, meta);
            lua_pushnil(l);
            lua_copy(l, -2, -1);
            lua_setfield(l, -2, kMethodIndex);
            lua_pushcfunction(l, &GC);
            lua_setfield(l, -2, kMethodGC);

            lua_pushcfunction(l, &GetOpacity);
            lua_setfield(l, -2, kMethodGetOpacity);

            lua_pushcfunction(l, &GetLoc);
            lua_setfield(l, -2, kMethodGetLoc);

            lua_pushcfunction(l, &GetSize);
            lua_setfield(l, -2, kMethodGetSize);

            lua_pushcfunction(l, &Contains);
            lua_setfield(l, -2, kMethodContains);

            lua_pushcfunction(l, &GetChildAt);
            lua_setfield(l, -2, kMethodGetChildAt);

            lua_pushcfunction(l, &GetParent);
            lua_setfield(l, -2, kMethodGetParent);

            //css method
            lua_pushcfunction(l, &ApplyCSS);
            lua_setfield(l, -2, kMethodApplyCSS);
            lua_pushcfunction(l, &ApplyClass);
            lua_setfield(l, -2, kMethodApplyClass);
            //animate method
            lua_pushcfunction(l, &Animate);
            lua_setfield(l, -2, kMethodAnimate);
            lua_pushcfunction(l, &StopAnimate);
            lua_setfield(l, -2, kMethodStop);
            lua_pushcfunction(l, &PauseAnimate);
            lua_setfield(l, -2, kMethodPause);
            lua_pushcfunction(l, &ResumeAnimate);
            lua_setfield(l, -2, kMethodResume);
            lua_pushcfunction(l, &StopAllAnimate);
            lua_setfield(l, -2, kMethodStopAll);
        }    
        return exist;
    }

    static int GC(lua_State * l)
    {
        int count = lua_gettop(l);
        if (count == 1)
        {
            lua_pushnil(l);
            lua_copy(l, 1, -1);
            LuaObject * ref = static_cast<LuaObject *>(
                LuaContext::CallGetCObject(l));
            if (ref)
                ref->release();
        }
        return 0;
    }
    static int GetOpacity(lua_State * l)
    {
        lua_settop(l, 1);
        lua_pushnil(l);

        lua_pushnil(l);
        lua_copy(l, 1, -1);
        LuaObject * bob = static_cast<LuaObject *>(
            LuaContext::CallGetCObject(l));
        if (bob)
            lua_pushnumber(l, bob->getOpacity());

        return 1;
    }

    static int GetLoc(lua_State * l)
    {
        lua_settop(l, 1);
        lua_pushnil(l);
        lua_pushnil(l);

        lua_pushnil(l);
        lua_copy(l, 1, -1);
        LuaObject * bob = static_cast<LuaObject *>(
            LuaContext::CallGetCObject(l));
        if (bob)
        {
            BonesPoint bp = bob->getLoc();
            lua_pushnumber(l, bp.x);
            lua_pushnumber(l, bp.y);
        }
        return 2;
    }

    static int GetSize(lua_State * l)
    {
        lua_settop(l, 1);
        lua_pushnil(l);
        lua_pushnil(l);

        lua_pushnil(l);
        lua_copy(l, 1, -1);
        LuaObject * bob = static_cast<LuaObject *>(
            LuaContext::CallGetCObject(l));
        if (bob)
        {
            BonesSize bs = bob->getSize();
            lua_pushnumber(l, bs.width);
            lua_pushnumber(l, bs.height);
        }
        return 2;
    }

    static int Contains(lua_State * l)
    {
        int count = lua_gettop(l);
        lua_pushnil(l);
        if (count == 3)
        {
            Scalar x = static_cast<Scalar>(lua_tonumber(l, 2));
            Scalar y = static_cast<Scalar>(lua_tonumber(l, 3));
            lua_pushnil(l);
            lua_copy(l, 1, -1);
            LuaObject * bob = static_cast<LuaObject *>(
                LuaContext::CallGetCObject(l));
            if (bob)
                lua_pushboolean(l, bob->contains(x, y));
        }
        return 1;
    }

    //(self, index)
    static int GetChildAt(lua_State * l)
    {
        int count = lua_gettop(l);
        lua_pushnil(l);
        if (count == 2)
        {
            size_t index = static_cast<size_t>(lua_tonumber(l, 2));
            lua_pushnil(l);
            lua_copy(l, 1, -1);
            LuaObject * bob = static_cast<LuaObject *>(
                LuaContext::CallGetCObject(l));
            if (bob)
            {
                auto child = bob->getChildAt(index);             
                if (child)
                    LuaContext::GetLOFromCO(l, child);
            }

        }
        return 1;
    }

    static int GetParent(lua_State * l)
    {
        lua_settop(l, 1);
        lua_pushnil(l);

        lua_pushnil(l);
        lua_copy(l, 1, -1);
        LuaObject * bob = static_cast<LuaObject *>(
            LuaContext::CallGetCObject(l));
        if (bob)
        {
            auto parent = bob->getParent();
            if (parent)
                LuaContext::GetLOFromCO(l, parent);
        }

        return 1;
    }

    //(self, css)
    static int ApplyCSS(lua_State * l)
    {
        lua_settop(l, 2);

        auto css = lua_tostring(l, 2);
        lua_pushnil(l);
        lua_copy(l, 1, -1);
        LuaObject * bob = static_cast<LuaObject *>(
            LuaContext::CallGetCObject(l));
        if (bob)
            bob->applyCSS(css);
        return 0;
    }

    //(self, class, [mod])
    static int ApplyClass(lua_State * l)
    {
        lua_settop(l, 2);

        const char * class_name = lua_tostring(l, 2);
        lua_pushnil(l);
        lua_copy(l, 1, -1);
        LuaObject * bob = static_cast<LuaObject *>(
            LuaContext::CallGetCObject(l));
        if (bob)
            bob->applyClass(class_name);
        return 0;
    }

    //(self interval due [run stop start  pause resume])
    static int Animate(lua_State * l)
    {
        lua_settop(l, 8);
        lua_pushnil(l);

        lua_pushnil(l);
        lua_copy(l, 1, -1);
        auto bo = static_cast<LuaObject *>(LuaContext::CallGetCObject(l));
        uint64_t interval = lua_tointeger(l, 2);
        uint64_t due = lua_tointeger(l, 3);

        auto ani = GetCoreInstance()->createAnimate(
            bo, interval, due, 0, 0, 0, 0, 0, kANI_SCRIPT);

        LuaContext::GetLOFromCO(l, ani);
        lua_pushnil(l);
        lua_copy(l, 4, -1);
        lua_setfield(l, -2, kMethodAnimateRun);
        lua_pushnil(l);
        lua_copy(l, 5, -1);
        lua_setfield(l, -2, kMethodAnimateStop);
        lua_pushnil(l);
        lua_copy(l, 6, -1);
        lua_setfield(l, -2, kMethodAnimateStart);
        lua_pushnil(l);
        lua_copy(l, 7, -1);
        lua_setfield(l, -2, kMethodAnimatePause);
        lua_pushnil(l);
        lua_copy(l, 8, -1);
        lua_setfield(l, -2, kMethodAnimateResume);

        GetCoreInstance()->startAnimate(ani);

        return 1;
    }

    //(self, animate, bool)
    static int StopAnimate(lua_State * l)
    {
        lua_settop(l, 3);
        lua_pushnil(l);
        lua_copy(l, 2, -1);
        auto ani = static_cast<BonesObject::Animation>(
            LuaContext::CallGetCObject(l));
        GetCoreInstance()->stopAnimate(ani, !!lua_toboolean(l, 3));
        return 0;
    }

    //(self, ani)
    static int PauseAnimate(lua_State * l)
    {
        lua_settop(l, 2);
        lua_pushnil(l);
        lua_copy(l, 2, -1);
        auto ani = static_cast<BonesObject::Animation>(
            LuaContext::CallGetCObject(l));
        GetCoreInstance()->pauseAnimate(ani);
        return 0;
    }

    //(self, ani)
    static int ResumeAnimate(lua_State * l)
    {
        lua_settop(l, 2);
        lua_pushnil(l);
        lua_copy(l, 2, -1);
        auto ani = static_cast<BonesObject::Animation>(
            LuaContext::CallGetCObject(l));
        GetCoreInstance()->resumeAnimate(ani);
        return 0;
    }

    //(self, bool)
    static int StopAllAnimate(lua_State * l)
    {
        lua_settop(l, 2);
        lua_pushnil(l);
        lua_copy(l, 1, -1);
        auto bo = static_cast<LuaObject *>(
            LuaContext::CallGetCObject(l));
        GetCoreInstance()->stopAllAnimate(bo, !!lua_toboolean(l, 2));
        return 0;
    }
protected:
    RefPtr<T> object_;
};

}
#endif