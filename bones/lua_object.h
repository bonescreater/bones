#ifndef BONES_LUA_OBJECT_H_
#define BONES_LUA_OBJECT_H_

#include "bones_internal.h"
#include "core/view.h"
#include "core/logging.h"
#include "core/css_manager.h"

namespace bones
{


template<class Base, class T>
class LuaObject : public Base, public Ref
{
public:
    LuaObject(T * v)
    {
        LOG_VERBOSE << "create:" << v->getClassName() << v;
        object_.reset(v);
    }

    virtual ~LuaObject()
    {
        LOG_VERBOSE << "destroy:" << this->getClassName() << this;
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

    void listen(const char * name, BonesScriptListener * listener)
    {
        GetCoreInstance()->listen(this, name, listener);
    }

    void push(BonesScriptArg * arg)
    {
        GetCoreInstance()->push(arg);
    }

    float getOpacity() const
    {
        return object_->getOpacity();
    }

    void getLoc(Scalar & x, Scalar & y)
    {
        x = object_->getLeft();
        y = object_->getTop();
    }

    void getSize(Scalar & w, Scalar & h)
    {
        w = object_->getWidth();
        h = object_->getHeight();
    }

    bool contains(Scalar x, Scalar y)
    {
        return object_->contains(Point::Make(x, y));
    }

    BonesObject * getChildAt(size_t index)
    {
        auto child = object_->getChildAt(index);
        if (child)
            return GetCoreInstance()->getObject(child);
        return nullptr;
    }

    void applyCSS(const char * css)
    {
        Core::GetCSSManager()->applyCSS(object_.get(), css);
    }

    void applyClass(const char * name)
    {
        Core::GetCSSManager()->applyClass(object_.get(), name);
    }
protected:
    RefPtr<T> object_;
};

}
#endif