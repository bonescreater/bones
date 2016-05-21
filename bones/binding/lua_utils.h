#ifndef BONES_BINDING_LUA_UTILS_H_
#define BONES_BINDING_LUA_UTILS_H_

#include "bones.h"
#include "lua.hpp"

namespace bones
{

class LuaUtils
{
public:
    static void PushContext(lua_State * l);

    static void PopContext(lua_State * l);

    static void CreateContext(lua_State * l);

    static void DestroyContext(lua_State * l);

    static BonesScalar ToScalar(lua_Number t);

    static BonesColor ToColor(lua_Integer t);

    static int ToInt(lua_Integer t);
    //
    static int SafePCall(lua_State * l, int nargs);
    //create table rect
    static void PushRect(lua_State * l, const BonesRect & r);

    static void GetRect(lua_State * l, int idx, BonesRect & r);

    static void PopRect(lua_State * l, BonesRect & r);
    //create table point
    static void PushPoint(lua_State * l, const BonesPoint & pt);

    static void GetPoint(lua_State * l, int idx, BonesPoint & pt);

    static void PopPoint(lua_State * l, BonesPoint & pt);

    //create table size
    static void PushSize(lua_State * l, const BonesSize & se);

    static void GetSize(lua_State * l, int idx, BonesSize & se);

    static void PopSize(lua_State * l, BonesSize & se);

    //create table colormatrix
    static const int kColorMatrxSize = 20;

    static void PushColorMatrix(lua_State * l, BonesScalar * cm,
        int len = kColorMatrxSize);

    static void GetColorMatrix(lua_State * l, int idx, BonesScalar * cm,
        int len = kColorMatrxSize);

    static void PopColorMatrix(lua_State * l, BonesScalar * cm,
        int len = kColorMatrxSize);

    static void PushColorArray(lua_State * l, const BonesColor * colors, int len);

    static void GetColorArray(lua_State * l, int idx, BonesColor * colors, int len);

    static void PopColorArray(lua_State * l, BonesColor * colors, int len);

    static void PushScalarArray(lua_State * l, const BonesScalar * scalars, int len);

    static void GetScalarArray(lua_State * l, int idx, BonesScalar * scalars, int len);

    static void PopScalarArray(lua_State * l, BonesScalar * scalars, int len);
};


}

#endif