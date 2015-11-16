#include "simple_proxy.h"
#include "core/core_imp.h"
#include "core/point.h"
#include "core/pixmap.h"
#include "core/res_manager.h"

#include "utils.h"
#include "SkPath.h"



namespace bones
{


#define IF_NULL_RETURN(a) if(!a)return

BonesPath PathProxy::create()
{
    return Utils::ToBonesPath(new SkPath);
}

void PathProxy::moveTo(BonesPath path, const BonesPoint & p)
{
    IF_NULL_RETURN(path);
    Utils::ToSkPath(path)->moveTo(p.x, p.y);
}

void PathProxy::lineTo(BonesPath path, const BonesPoint & p)
{
    IF_NULL_RETURN(path);
    Utils::ToSkPath(path)->lineTo(p.x, p.y);
}

void PathProxy::quadTo(BonesPath path, const BonesPoint & p1,
    const BonesPoint & p2)
{
    IF_NULL_RETURN(path);
    Utils::ToSkPath(path)->quadTo(p1.x, p1.y, p2.x, p2.y);
}

void PathProxy::conicTo(BonesPath path, const BonesPoint & p1,
    const BonesPoint & p2, BonesScalar w)
{
    IF_NULL_RETURN(path);
    Utils::ToSkPath(path)->conicTo(p1.x, p1.y, p2.x, p2.y, w);
}

void PathProxy::cubicTo(BonesPath path, const BonesPoint & p1,
    const BonesPoint & p2, const BonesPoint & p3)
{
    IF_NULL_RETURN(path);
    Utils::ToSkPath(path)->cubicTo(p1.x, p1.y, p2.x, p2.y, p3.x, p3.y);
}

void PathProxy::arcTo(BonesPath path, const BonesRect & oval,
    BonesScalar startAngle, BonesScalar sweepAngle)
{
    IF_NULL_RETURN(path);
    Utils::ToSkPath(path)->arcTo(SkRect::MakeLTRB(oval.left, oval.top, oval.right, oval.bottom),
        startAngle, sweepAngle, false);
}

void PathProxy::arcTo(BonesPath path, const BonesPoint & p1,
    const BonesPoint & p2, BonesScalar radius)
{
    IF_NULL_RETURN(path);
    Utils::ToSkPath(path)->arcTo(p1.x, p1.y, p2.x, p2.y, radius);
}

void PathProxy::close(BonesPath path)
{
    IF_NULL_RETURN(path);
    Utils::ToSkPath(path)->close();
}

void PathProxy::release(BonesPath path)
{
    IF_NULL_RETURN(path);
    delete Utils::ToSkPath(path);
}

Core::TileMode ToCoreTileMode(BonesShaderProxy::TileMode mode)
{
    if (BonesShaderProxy::kRepeat == mode)
        return Core::kRepeat;
    if (BonesShaderProxy::kMirror == mode)
        return Core::kMirror;
    return Core::kClamp;
}

BonesShader ShaderProxy::createLinearGradient(
    const BonesPoint & begin,
    const BonesPoint & end,
    TileMode mode,
    size_t count, BonesColor * color,
    BonesScalar * pos)
{
    Point bpt, ept;
    bpt.set(begin.x, begin.y);
    ept.set(end.x, end.y);

    return Core::createLinearGradient(bpt, ept, count, color, pos,
        ToCoreTileMode(mode));
}

BonesShader ShaderProxy::createRadialGradient(
    const BonesPoint & center,
    BonesScalar radius,
    TileMode mode,
    size_t count, BonesColor * color,
    float * pos)
{
    Point pt;
    pt.set(center.x, center.y);
    return Core::createRadialGradient(pt, radius, count, color, pos,
        ToCoreTileMode(mode));
}

void ShaderProxy::release(BonesShader shader)
{
    IF_NULL_RETURN(shader);
    Core::destroyShader(Utils::ToSkShader(shader));
}

BonesPixmap PixmapProxy::create()
{
    return Utils::ToBonesPixmap(new Pixmap);
}

bool PixmapProxy::alloc(BonesPixmap pm, int width, int height)
{
    IF_NULL_RETURN(pm) false;
    return Utils::ToPixmap(pm)->allocate(width, height);
}

bool PixmapProxy::decode(BonesPixmap pm, const void * data, size_t len)
{
    IF_NULL_RETURN(pm) false;
    return Utils::ToPixmap(pm)->decode(data, len);
}

int PixmapProxy::getWidth(BonesPixmap pm) const
{
    IF_NULL_RETURN(pm) 0;
    return Utils::ToPixmap(pm)->getWidth();
}

int PixmapProxy::getHeight(BonesPixmap pm) const
{
    IF_NULL_RETURN(pm) 0;
    return Utils::ToPixmap(pm)->getHeight();
}

bool PixmapProxy::lock(BonesPixmap pm, LockRec & rec)
{
    IF_NULL_RETURN(pm) false;
    Pixmap::LockRec lr;
    bool ret = Utils::ToPixmap(pm)->lock(lr);
    if (ret)
    {
        rec.bits = lr.bits;
        rec.pitch = lr.pitch;
        rec.subset.left = lr.subset.left();
        rec.subset.top = lr.subset.top();
        rec.subset.right = lr.subset.right();
        rec.subset.bottom = lr.subset.bottom();
    }
    return ret;
}

void PixmapProxy::unlock(BonesPixmap pm)
{
    IF_NULL_RETURN(pm);
    Utils::ToPixmap(pm)->unlock();
}

void PixmapProxy::erase(BonesPixmap pm, BonesColor color)
{
    IF_NULL_RETURN(pm);
    Utils::ToPixmap(pm)->erase(color);
}

void PixmapProxy::extractSubset(BonesPixmap dst,
    BonesPixmap src, const BonesRect & subset)
{
    IF_NULL_RETURN(dst);
    IF_NULL_RETURN(src);
    auto pdst = Utils::ToPixmap(dst);
    *pdst = Utils::ToPixmap(src)->extractSubset(Utils::ToRect(subset));
}

bool PixmapProxy::isTransparent(BonesPixmap pm, int x, int y)
{
    IF_NULL_RETURN(pm) false;
    return !(ColorGetA(Utils::ToPixmap(pm)->getPMColor(x, y)) & 0xff);
}

void PixmapProxy::release(BonesPixmap pm)
{
    IF_NULL_RETURN(pm);
    delete Utils::ToPixmap(pm);
}

void ResourceManager::addPixmap(const char * key, BonesPixmap pm)
{
    IF_NULL_RETURN(key);
    IF_NULL_RETURN(pm);
    Core::GetResManager()->addPixmap(key, *Utils::ToPixmap(pm));
}

BonesPixmap ResourceManager::getPixmap(const char * key)
{
    IF_NULL_RETURN(key) nullptr;
    return Utils::ToBonesPixmap(Core::GetResManager()->getPixmap(key));
}

void ResourceManager::clean()
{
    Core::GetResManager()->clean();
}

}