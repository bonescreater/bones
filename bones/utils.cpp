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

BonesRect Utils::ToBonesRect(const Rect & rect)
{
    BonesRect br;
    br.left = rect.left();
    br.top = rect.top();
    br.right = rect.right();
    br.bottom = rect.bottom();
    return br;
}

BonesColor Utils::ToBonesColor(lua_Integer i)
{
    return static_cast<BonesColor>(i);
}

BonesScalar Utils::ToBonesScalar(lua_Number t)
{
    return static_cast<BonesScalar>(t);
}

int Utils::ToInt(lua_Number i)
{
    return static_cast<int>(i);
}

void Utils::ToEMForMouse(BonesRoot::MouseMessage msg, EventType & type, MouseButton & mb)
{
    type = kET_COUNT;
    mb = kMB_NONE;

    switch (msg)
    {
    case BonesRoot::kMouseMove:
        type = kET_MOUSE_MOVE;
        break;
    case BonesRoot::kLButtonDClick:
    {
        type = kET_MOUSE_DCLICK;
        mb = kMB_LEFT;
    }
        break;
    case BonesRoot::kLButtonDown:
    {
        type = kET_MOUSE_DOWN;
        mb = kMB_LEFT;
    }
        break;
    case BonesRoot::kLButtonUp:
    {
        type = kET_MOUSE_UP;
        mb = kMB_LEFT;
    }
        break;
    case BonesRoot::kRButtonDClick:
    {
        type = kET_MOUSE_DCLICK;
        mb = kMB_RIGHT;
    }
        break;
    case BonesRoot::kRButtonDown:
    {
        type = kET_MOUSE_DOWN;
        mb = kMB_RIGHT;
    }
        break;
    case BonesRoot::kRButtonUp:
    {
        type = kET_MOUSE_UP;
        mb = kMB_RIGHT;
    }
        break;
    case BonesRoot::kMButtonDown:
    {
        type = kET_MOUSE_DOWN;
        mb = kMB_MIDDLE;
    }
        break;
    case BonesRoot::kMButtonUp:
    {
        type = kET_MOUSE_UP;
        mb = kMB_MIDDLE;
    }
        break;
    case BonesRoot::kMButtonDClick:
    {
        type = kET_MOUSE_DCLICK;
        mb = kMB_MIDDLE;
    }
        break;
    case BonesRoot::kMouseLeave:
        type = kET_MOUSE_LEAVE;
        break;
    default:
        return;
    }
}


}