#ifndef BONES_BINDING_SCRIPT_CONTEXT_H_
#define BONES_BINDING_SCRIPT_CONTEXT_H_

#include "bones.h"
#include "path_proxy.h"
#include "lua.hpp"

namespace bones
{

class ScriptContext : public BonesContext, 
                      public BonesPathProxy
{
public:
    ScriptContext();

    ~ScriptContext();

    BonesPathProxy * getPathProxy() override;

    bool loadXMLString(const char * data) override;

    bool loadXMLFile(const char * file) override;

    void clean() override;

    void update() override;
public:
    //pathproxy
   BonesPath create() override;

    void moveTo(BonesPath path, const BonesPoint & p) override;

    void lineTo(BonesPath path, const BonesPoint & p) override;

    void quadTo(BonesPath path, const BonesPoint & p1,
        const BonesPoint & p2) override;

    void conicTo(BonesPath path, const BonesPoint & p1,
        const BonesPoint & p2, BonesScalar w) override;

    void cubicTo(BonesPath path, const BonesPoint & p1,
        const BonesPoint & p2, const BonesPoint & p3) override;

    void arcTo(BonesPath path, const BonesRect & oval,
        BonesScalar startAngle, BonesScalar sweepAngle) override;

    void arcTo(BonesPath path, const BonesPoint & p1,
        const BonesPoint & p2, BonesScalar radius) override;

    void close(BonesPath path) override;

    void release(BonesPath path) override;
private:
    void InitState();

    void FiniState();
private:
    lua_State * state_;
    PathProxy * path_proxy_;
};

}

#endif