#ifndef BONES_BINDING_SHADER_PROXY_H_
#define BONES_BINDING_SHADER_PROXY_H_

#include "bones.h"
#include "lua.hpp"

namespace bones
{

class ShaderProxy : public BonesShaderProxy
{
public:
    ShaderProxy(lua_State * s);

    ~ShaderProxy();
public:
    BonesShader createLinearGradient(
        const BonesPoint & begin,
        const BonesPoint & end,
        int count, BonesColor * color,
        BonesScalar * pos, const char * mode) override;

    BonesShader createRadialGradient(
        const BonesPoint & center,
        BonesScalar radius,
        int count, BonesColor * color,
        float * pos, const char * mode) override;

    void release(BonesShader shader) override;
private:
    lua_State * state_;
};


}


#endif