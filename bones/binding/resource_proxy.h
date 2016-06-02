#ifndef BONES_BINDING_RESOURCE_PROXY_H_
#define BONES_BINDING_RESOURCE_PROXY_H_

#include "bones.h"
#include "engine_context.h"
#include "bitmap.h"
#include <map>

namespace bones
{
class ResourceProxy : public BonesResourceProxy
{
public:
    ResourceProxy(EngineContext & ctx);

    ~ResourceProxy();

public:
    void add(const char * key, BonesPixmap & pm) override;

    void remove(const char * key) override;

    void clean() override;

    bool get(const char * key, BonesPixmap & pm) override;
public:
    void invokeAdd(const char * key, Bitmap & pm);

    void invokeRemove(const char * key);

    void invokeClean();

    bool invokeGet(const char * key, Bitmap & pm);
private:
    EngineContext * ctx_;
    std::map<std::string, Bitmap *> pixmaps_;
};

}

#endif