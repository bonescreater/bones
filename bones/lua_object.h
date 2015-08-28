#ifndef BONES_LUA_OBJECT_H_
#define BONES_LUA_OBJECT_H_

#include "bones_internal.h"
#include "core/view.h"
#include "core/logging.h"
#include "core/css_manager.h"
#include "script_parser.h"

namespace bones
{


template<class Base, class T>
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

    void stopAllAnimate(BonesAnimation * ani, bool toend)  override
    {
        GetCoreInstance()->stopAllAnimate(ani, toend);
    }
protected:
    RefPtr<T> object_;
};

}
#endif