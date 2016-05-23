#ifndef BONES_BINDING_PIXMAP_PROXY_H_
#define BONES_BINDING_PIXMAP_PROXY_H_

#include "bones.h"
#include "engine_context.h"

namespace bones
{

class PixmapProxy : public BonesPixmapProxy
{
public:
    PixmapProxy(EngineContext & ctx);

    ~PixmapProxy();

public:
    BonesPixmap * create() override;

    void release(BonesPixmap * pixmap) override;
public:
    EngineContext & ctx();
private:
    EngineContext * ctx_;

};

}

#endif