#include "script_parser.h"
#include "lua_context.h"

#include "lua_check.h"
#include "lua_panel.h"
#include "lua_area.h"
#include "lua_block.h"
#include "lua_text.h"
#include "lua_shape.h"
#include "lua_meta_table.h"

#include "core/logging.h"
#include "core/area.h"
#include "core/block.h"
#include "core/text.h"
#include "core/shape.h"
#include "core/panel.h"

namespace bones
{

static const char * kStrName = "name";
static const char * kStrPhase = "phase";
static const char * kStrFunc = "func";

//(self, arg1, arg2)
static int ScriptCB(lua_State * l)
{
    static const int kStatckArgCount = 10;
    bool bret = false;
    ScriptArg stack_args[kStatckArgCount] = { 0 };
    ScriptArg * args = stack_args;
    auto param_count = lua_gettop(l);
    if (param_count <= 0)
    {
        LOG_VERBOSE << "script callback has no sender Ref???\n";
        lua_pushboolean(l, bret);
        return 1;
    }
        
    size_t arg_count = param_count - 1;
    if (arg_count > kStatckArgCount)
    {
        args = new ScriptArg[arg_count];
        memset(args, 0, sizeof(args[0]) * arg_count);
    }
        
    //参数转化
    int j = 0;
    for (auto i = 2; i <= param_count; ++i)
    {
        switch (lua_type(l, i))
        {
        case LUA_TBOOLEAN:
            args[j].b = !!lua_toboolean(l, i);
            break;
        case LUA_TSTRING:
            args[j].c = lua_tostring(l, i);
            break;
        case LUA_TNUMBER:
            args[j].f = (Scalar)lua_tonumber(l, i);
            break;
        case LUA_TFUNCTION:
            args[j].v = (void *)lua_tocfunction(l, i);
            break;
        case LUA_TLIGHTUSERDATA:
        case LUA_TUSERDATA:
            args[j].v = (void *)lua_touserdata(l, i);
            break;
        default:
            break;
        }
        j++;
    }
    //获取sender
    lua_pushnil(l);
    lua_copy(l, 1, -1);
    Ref * sender = (View *)LuaMetaTable::CallGetCObject(l);

    assert(sender);
    ScriptCallBack cb = (ScriptCallBack)lua_touserdata(l, lua_upvalueindex(1));
    void * userdata = lua_touserdata(l, lua_upvalueindex(2));
    bret = (*cb)(sender, args, arg_count, userdata);
    lua_pushboolean(l, bret);

    if (args != stack_args)
        delete[] args;

    return 1;
}

static const char * kStrScript = "script";

ScriptParser * ScriptParser::Get()
{
    static ScriptParser * parser = nullptr;
    if (!parser)
        parser = new ScriptParser;
    return parser;
}

ScriptParser::ScriptParser()
{
    ;
}

void ScriptParser::regScriptCallback(Ref * ob, const char * event_name, ScriptCallBack cb, void * userdata)
{
    using namespace bones;
    auto l = LuaContext::State();
    LUA_STACK_AUTO_CHECK(l);

    LuaContext::GetLOFromCO(l, ob);
    lua_pushstring(l, event_name);
    //创建1个闭包
    lua_pushlightuserdata(l, cb);
    lua_pushlightuserdata(l, userdata);
    lua_pushcclosure(l, ScriptCB, 2);
    lua_settable(l, -3);
    lua_pop(l, 1);
}

void ScriptParser::unregScriptCallback(Ref * ob, const char * event_name)
{
    using namespace bones;
    auto l = LuaContext::State();
    LUA_STACK_AUTO_CHECK(l);

    LuaContext::GetLOFromCO(l, ob);
    lua_pushstring(l, event_name);
    lua_pushnil(l);
    lua_settable(l, -3);
    lua_pop(l, 1);
}

void ScriptParser::clean()
{
    reset();
    LuaContext::Reset();
}

bool ScriptParser::handleLink(XMLNode node, Module & mod)
{
    if (XMLController::handleLink(node, mod))
        return true;
    if (!node)
        return false;
    Attribute link[] =
    {
        { kStrType, nullptr }, { kStrModule, nullptr }, { kStrFile, nullptr }
    };
    acquireAttrs(node, link, sizeof(link) / sizeof(link[0]));
    auto & type = link[0].value;
    auto & module = link[1].value;
    auto & file = link[2].value;

    if (!type || strcmp(type, kStrScript))
        return false;
    if (!module)
        return true;

    //link script module
    auto l = LuaContext::State();
    LUA_STACK_AUTO_CHECK(l);
    lua_getglobal(l, module);
    if (lua_istable(l, -1))
    {//模块已经load
        lua_pop(l, 1);
        return true;
    }
    lua_pop(l, 1);
    //没有指定路径忽略
    if (!file)
        return true;

    std::string path;
    if (isAbsolutePath(file))
        path = file;
    else
    {
        auto dir = getPathFromFullName(mod.xml_fullname.data());
        const char * parts[] = { dir.data(), file };
        path = joinPath(parts, sizeof(parts) / sizeof(parts[0]));
    }
    //dofile 只支持 char * 路径不清楚会不会支持中文 所以自己读取文件
    FileStream  fs;
    if (!readFile(path.data(), fs))
    {
        LOG_ERROR << path << " read fail";
        return true;
    }
        
    //skip bom
    int start_count = 0;
    if (fs.size() >= 3)
    {
        if (static_cast<unsigned char>(fs[0]) == 0xEF &&
            static_cast<unsigned char>(fs[1]) == 0xBB &&
            static_cast<unsigned char>(fs[2]) == 0xBF)
        {
            start_count = 3;      // Skup utf-8 bom
        }
    }

    if (LUA_OK == luaL_loadstring(l, &fs[start_count]))
    {
        auto count = LuaContext::SafeLOPCall(l, 0, LUA_MULTRET);
        if (count != 1)
            LOG_ERROR << "lua module must return a table";
        if (lua_istable(l, -1))
        {
            lua_setglobal(l, module);
            count--;
        }
        lua_pop(l, count);
    }
    else
    {
        LOG_ERROR << "lua module " << module << " load fail " << lua_tostring(l, -1);
        lua_pop(l, 1);
    }
        
    return true;
}

bool ScriptParser::handlePanel(XMLNode node, Ref * parent_ob, const Module & mod, Ref ** ob)
{
    bool result = XMLController::handlePanel(node, parent_ob, mod, ob);
    if (result)
        LuaPanel::Create(static_cast<Panel *>(*ob));
    return result;
}

bool ScriptParser::handleArea(XMLNode node, Ref * parent_ob, const Module & mod, Ref ** ob)
{
    bool result = XMLController::handleArea(node, parent_ob, mod, ob);
    if (result)
        LuaArea::Create(static_cast<Area *>(*ob));
    return result;
}

bool ScriptParser::handleBlock(XMLNode node, Ref * parent_ob, const Module & mod, Ref ** ob)
{
    bool result = XMLController::handleBlock(node, parent_ob, mod, ob);
    if (result)
        LuaBlock::Create(static_cast<Block *>(*ob));
    return result;
}

bool ScriptParser::handleText(XMLNode node, Ref * parent_ob, const Module & mod, Ref ** ob)
{
    bool result = XMLController::handleText(node, parent_ob, mod, ob);
    if (result)
        LuaText::Create(static_cast<Text *>(*ob));
    return result;
}

bool ScriptParser::handleShape(XMLNode node, Ref * parent_ob, const Module & mod, Ref ** ob)
{
    bool result = XMLController::handleShape(node, parent_ob, mod, ob);
    if (result)
        LuaShape::Create(static_cast<Shape *>(*ob));
    return result;
}


bool ScriptParser::handleExtendLabel(XMLNode node, Ref * parent_ob, const Module & mod, Ref ** ob)
{
    if (XMLController::handleExtendLabel(node, parent_ob, mod, ob))
        return true;
    auto name = node.name();
    if (!name)
        return false;

    if (!strcmp("event", name))
        return handleELEvent(node, parent_ob, mod, ob);

    return false;
}

void ScriptParser::handleNodeOnPrepare(XMLNode node, Ref * ob)
{
    if (!node || !ob)
        return;
    auto l = LuaContext::State();
    LUA_STACK_AUTO_CHECK(l);
    LuaContext::GetLOFromCO(l, ob);
    if (lua_istable(l, -1))
    {
        lua_getfield(l, -1, "onPrepare");
        assert(lua_isfunction(l, -1) || lua_isnil(l, -1));
        lua_pushnil(l);
        lua_copy(l, -3, -1);
        LuaContext::SafeLOPCall(l, 1, 0);
    }
    lua_pop(l, 1);
}

//给父对象添加事件绑定
bool ScriptParser::handleELEvent(XMLNode node, Ref * parent_ob, const Module & mod, Ref ** ob)
{
    if (!parent_ob)
        return false;
    Attribute attrs[] =
    {
        { kStrName, nullptr },
        { kStrPhase, nullptr },
        { kStrModule, nullptr },
        { kStrFunc, nullptr },
    };

    acquireAttrs(node, attrs, sizeof(attrs) / sizeof(attrs[0]));
    if (kClassArea == parent_ob->getClassName())
        LuaArea::RegisterEvent(static_cast<Area *>(parent_ob),
        attrs[0].value,
        attrs[1].value,
        attrs[2].value,
        attrs[3].value);
    else if (kClassPanel == parent_ob->getClassName())
    LuaPanel::RegisterEvent(static_cast<Panel *>(parent_ob),
             attrs[0].value,
             attrs[1].value,
             attrs[2].value,
             attrs[3].value);
    return true;
}














}