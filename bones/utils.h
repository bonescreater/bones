#ifndef BONES_UTILS_H_
#define BONES_UTILS_H_

#include "bones.h"
#include "lua.hpp"

class SkPath;
class SkShader;


namespace bones
{
class Pixmap;
class Rect;

class Utils
{
public:
    static SkPath * ToSkPath(BonesPath path);

    static BonesPath ToBonesPath(SkPath * path);

    static SkShader * ToSkShader(BonesShader shader);

    static BonesShader ToBonesShader(SkShader * shader);

    static Pixmap * ToPixmap(BonesPixmap pm);

    static BonesPixmap ToBonesPixmap(Pixmap * bm);

    static Rect ToRect(const BonesRect & rect);

    static BonesRect ToBonesRect(const Rect & rect);

    template<class T>
    static BonesScalar ToBonesScalar(T t)
    {
        return static_cast<BonesScalar>(t);
    }
    static BonesColor ToBonesColor(lua_Integer i);

    static int ToInt(lua_Integer i);
};
}

#endif