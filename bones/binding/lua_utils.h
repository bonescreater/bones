#ifndef BONES_BINDING_LUA_UTILS_H_
#define BONES_BINDING_LUA_UTILS_H_

#include "core/core.h"
#include "lua.hpp"

namespace bones
{

class LuaUtils
{
public:
    static Scalar ToScalar(lua_Number t);
    //
    static int SafePCall(lua_State * l, int nargs);
    //create table rect
    static void PushRect(lua_State * l, Scalar left, Scalar top, Scalar right, Scalar bottom);

    static void GetRect(lua_State * l, Scalar & left, Scalar & top, Scalar & right, Scalar & bottom);

    static void PopRect(lua_State * l, Scalar & left, Scalar & top, Scalar & right, Scalar & bottom);
    //create table point
    static void PushPoint(lua_State * l, Scalar x, Scalar y);

    static void GetPoint(lua_State * l, Scalar & x, Scalar & y);

    static void PopPoint(lua_State * l, Scalar & x, Scalar & y);

    //create table size
    static void PushSize(lua_State * l, Scalar w, Scalar h);

    static void GetSize(lua_State * l, Scalar & w, Scalar & h);

    static void PopSize(lua_State * l, Scalar & w, Scalar & h);

    //create table colormatrix
    static const int kColorMatrxSize = 20;

    static void PushColorMatrix(lua_State * l, Scalar * cm,
        size_t len = kColorMatrxSize);

    static void GetColorMatrix(lua_State * l, Scalar * cm, 
        size_t len = kColorMatrxSize);

    static void PopColorMatrix(lua_State * l, Scalar * cm, 
        size_t len = kColorMatrxSize);
    //static int ToInt(lua_Number i);
    //static BonesColor ToBonesColor(lua_Integer i);
};


}

#endif