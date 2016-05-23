#ifndef BONES_BINDING_SCRIPT_CONTEXT_H_
#define BONES_BINDING_SCRIPT_CONTEXT_H_

#include "bones.h"
#include "engine_context.h"

namespace bones
{

class PathProxy;
class ShaderProxy;
class PixmapProxy;

class ScriptContext : public BonesContext, public EngineContext
{
public:
    ScriptContext();

    ~ScriptContext();

    lua_State * State() override;

    BonesPathProxy * getPathProxy() override;

    BonesShaderProxy * getShaderProxy() override;

    bool loadXMLString(const char * data) override;

    bool loadXMLFile(const char * file) override;

    void clean() override;

    void update() override;
private:
    void InitState();

    void FiniState();
private:
    lua_State * state_;
    PathProxy * path_proxy_;
    ShaderProxy * shader_proxy_;
    PixmapProxy * pixmap_proxy_;
};

}

#endif