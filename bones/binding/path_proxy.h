#ifndef BONES_BINDING_PATH_PROXY_H_
#define BONES_BINDING_PATH_PROXY_H_

#include "bones.h"
#include "engine_context.h"

namespace bones
{
class PathProxy : public BonesPathProxy
{
public:
    PathProxy(EngineContext * ctx);

    ~PathProxy();
public:
    //pathproxy
    BonesPath * create() override;

    void release(BonesPath * path) override;
public:
    EngineContext * ctx();
private:
    EngineContext * ctx_;
};

}

#endif