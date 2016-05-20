#include "script_context.h"

namespace bones
{

ScriptContext::ScriptContext()
{
    InitState();
    path_proxy_ = new PathProxy(state_);
}

ScriptContext::~ScriptContext()
{
    delete path_proxy_;
    FiniState();
}

BonesPathProxy * ScriptContext::getPathProxy()
{
    return this;
}

bool ScriptContext::loadXMLString(const char * data)
{
    return false;
}

bool ScriptContext::loadXMLFile(const char * file)
{
    return false;
}

void ScriptContext::clean()
{

}

void ScriptContext::update()
{

}

BonesPath ScriptContext::create()
{
    return path_proxy_->create();
}

void ScriptContext::moveTo(BonesPath path, const BonesPoint & p)
{
    path_proxy_->moveTo(path, p.x, p.y);
}

void ScriptContext::lineTo(BonesPath path, const BonesPoint & p)
{
    path_proxy_->lineTo(path, p.x, p.y);
}

void ScriptContext::quadTo(BonesPath path, const BonesPoint & p1,
    const BonesPoint & p2)
{
    path_proxy_->quadTo(path, p1.x, p1.y, p2.x, p2.y);
}

void ScriptContext::conicTo(BonesPath path, const BonesPoint & p1,
    const BonesPoint & p2, BonesScalar w)
{
    path_proxy_->conicTo(path, p1.x, p1.y, p2.x, p2.y, w);
}

void ScriptContext::cubicTo(BonesPath path, const BonesPoint & p1,
    const BonesPoint & p2, const BonesPoint & p3)
{
    path_proxy_->cubicTo(path, p1.x, p1.y, p2.x, p2.y, p3.x, p3.y);
}

void ScriptContext::arcTo(BonesPath path, const BonesRect & oval,
    BonesScalar startAngle, BonesScalar sweepAngle)
{
    path_proxy_->arcTo(path, oval.left, oval.top, oval.right, oval.bottom, 
        startAngle, sweepAngle);
}

void ScriptContext::arcTo(BonesPath path, const BonesPoint & p1,
    const BonesPoint & p2, BonesScalar radius)
{
    path_proxy_->arcTo(path, p1.x, p1.y, p2.x, p2.y, radius);
}

void ScriptContext::close(BonesPath path)
{
    path_proxy_->close(path);
}

void ScriptContext::release(BonesPath path)
{
    path_proxy_->release(path);
}

void ScriptContext::InitState()
{
    state_ = luaL_newstate();
    luaL_openlibs(state_);
}

void ScriptContext::FiniState()
{
    lua_close(state_);
}

}