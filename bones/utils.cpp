#include "utils.h"
#include "core/rect.h"

namespace bones
{


SkPath * Utils::ToSkPath(BonesPath path)
{
    return static_cast<SkPath *>(path);
}

BonesPath Utils::ToBonesPath(SkPath * path)
{
    return path;
}

SkShader * Utils::ToSkShader(BonesShader shader)
{
    return static_cast<SkShader *>(shader);
}

BonesShader Utils::ToBonesShader(SkShader * shader)
{
    return shader;
}

Pixmap * Utils::ToPixmap(BonesPixmap pm)
{
    return static_cast<Pixmap *>(pm);
}

BonesPixmap Utils::ToBonesPixmap(Pixmap * bm)
{
    return bm;
}

Rect Utils::ToRect(const BonesRect & rect)
{
    return Rect::MakeLTRB(rect.left, rect.top, rect.right, rect.bottom);
}

BonesColor Utils::ToBonesColor(lua_Integer i)
{
    return static_cast<BonesColor>(i);
}
int Utils::ToInt(lua_Integer i)
{
    return static_cast<int>(i);
}


}