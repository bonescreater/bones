#ifndef BONES_UTILS_H_
#define BONES_UTILS_H_

#include "bones.h"
#include "lua.hpp"
#include "core/event.h"

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

    static BonesScalar ToBonesScalar(lua_Number t);

    static BonesColor ToBonesColor(lua_Integer i);

    static int ToInt(lua_Number i);

    static void ToEMForMouse(BonesRoot::MouseMessage msg, EventType & type, MouseButton & mb);
};
}

#endif