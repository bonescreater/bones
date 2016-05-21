#include "script_context.h"
#include "path_proxy.h"
#include "shader_proxy.h"
#include "lua_utils.h"

namespace bones
{

ScriptContext::ScriptContext()
{
    InitState();
    path_proxy_ = new PathProxy(state_);
    shader_proxy_ = new ShaderProxy(state_);
}

ScriptContext::~ScriptContext()
{
    delete shader_proxy_;
    delete path_proxy_;
    FiniState();
}

BonesPathProxy * ScriptContext::getPathProxy()
{
    return path_proxy_;
}

BonesShaderProxy * ScriptContext::getShaderProxy()
{
    return shader_proxy_;
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

void ScriptContext::InitState()
{
    state_ = luaL_newstate();
    luaL_openlibs(state_);
    LuaUtils::CreateContext(state_);
}

void ScriptContext::FiniState()
{
    LuaUtils::DestroyContext(state_);
    lua_close(state_);
}

}