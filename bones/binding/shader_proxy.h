#ifndef BONES_BINDING_SHADER_PROXY_H_
#define BONES_BINDING_SHADER_PROXY_H_

#include "bones.h"
#include "engine_context.h"

namespace bones
{

class ShaderProxy : public BonesShaderProxy
{
public:
    ShaderProxy(EngineContext & ctx);

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
    EngineContext * ctx_;
};


}


#endif