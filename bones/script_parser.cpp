#include "script_parser.h"
#include "picture.h"
#include "lua_context.h"
#include "lua_check.h"

#include "core/root.h"
#include "core/logging.h"
#include "core/area.h"
#include "core/image.h"
#include "core/text.h"
#include "core/shape.h"
#include "core/rich_edit.h"
#include "core/res_manager.h"
#include "core/animation_manager.h"
#include "core/animation.h"

#include "lua_root.h"
#include "lua_shape.h"
#include "lua_image.h"
#include "lua_text.h"
#include "lua_rich_edit.h"
#include "lua_area.h"
#include "lua_meta_table.h"
#include "lua_animation.h"


namespace bones
{

static const char * kStrName = "name";
static const char * kStrPhase = "phase";
static const char * kStrFunc = "func";
static const char * kStrScript = "script";
static const char * kStrEvent = "event";
static const char * kStrNotify = "notify";
////(self, arg1, arg2)
static int ScriptCB(lua_State * l)
{
    static const int kStatckArgCount = 10;
    BonesScriptArg stack_args[kStatckArgCount] = { (BonesScriptArg::Type)0, 0 };
    BonesScriptArg * args = stack_args;

    auto param_count = lua_gettop(l);
    if (param_count <= 0)
    {
        LOG_VERBOSE << "script callback has no sender Ref???\n";
        return 0;
    }
        
    size_t arg_count = param_count - 1;
    if (arg_count > kStatckArgCount)
    {
        args = new BonesScriptArg[arg_count];
        memset(args, 0, sizeof(args[0]) * arg_count);
    }
        
    //参数转化
    int j = 0;
    for (auto i = 2; i <= param_count; ++i)
    {
        switch (lua_type(l, i))
        {
        case LUA_TBOOLEAN:
            args[j].boolean = !!lua_toboolean(l, i);
            args[j].type = BonesScriptArg::kBoolean;
            break;
        case LUA_TSTRING:
            args[j].str = lua_tostring(l, i);
            args[j].type = BonesScriptArg::kString;
            break;
        case LUA_TNUMBER:
            args[j].number = lua_tonumber(l, i);
            args[j].type = BonesScriptArg::kNumber;
            break;
        case LUA_TLIGHTUSERDATA:
        case LUA_TUSERDATA:
            args[j].ud = (void *)lua_touserdata(l, i);
            args[j].type = BonesScriptArg::kUserData;
            break;
        default:
            break;
        }
        j++;
    }
    //获取sender
    lua_pushnil(l);
    lua_copy(l, 1, -1);
    auto * sender = static_cast<BonesObject *>(LuaMetaTable::CallGetCObject(l));

    assert(sender);
    BonesScriptListener * lis = (BonesScriptListener *)lua_touserdata(l, lua_upvalueindex(1));
    auto count = lis->onEvent(sender, args, arg_count);

    if (args != stack_args)
        delete[] args;

    return count;
}

ScriptParser::ScriptParser()
:listener_(nullptr), last_listener_(nullptr)
{
    xml_.setDelegate(this);
}

bool ScriptParser::loadXMLString(const char * data, BonesXMLListener * listener)
{
    Core::GetAnimationManager()->removeAll(false);
    LuaContext::Reset();
    v2bo_.clear();

    last_listener_ = listener_;
    listener_ = listener;
    return xml_.loadString(data);
}

void ScriptParser::cleanXML()
{
    loadXMLString(nullptr, nullptr);
}

BonesObject * ScriptParser::getObject(const char * id)
{
    if (!id)
        return nullptr;
    auto v = xml_.getViewByID(id);
    if (v)
    {
        auto iter = v2bo_.find(v);
        if (iter != v2bo_.end())
            return iter->second;
    }
    return nullptr;
}

BonesObject * ScriptParser::getObject(BonesObject * ob, const char * id)
{
    if (!ob || !id)
        return nullptr;
    View * v = getObject(ob);
    if (v)
    {
        v = xml_.getViewByID(v, id);
        if (v)
        {
            auto iter = v2bo_.find(v);
            if (iter != v2bo_.end())
                return iter->second;
        }
    }
    return nullptr;
}

BonesResManager * ScriptParser::getResManager()
{
    return this;
}
//res manager
void ScriptParser::add(const char * key, BonesPixmap & pm)
{
    Core::GetResManager()->add(key, static_cast<Picture *>(&pm)->getPixmap());
}

void ScriptParser::add(const char * key, BonesCursor cursor)
{
    Core::GetResManager()->add(key, cursor);
}

void ScriptParser::clean()
{
    Core::GetResManager()->clean();
}

BonesPixmap * ScriptParser::create()
{ 
    return new Picture();
}

bool ScriptParser::onLoad()
{
    return listener_ ? listener_->onLoad(this) : true;
}

void ScriptParser::onUnload()
{
    last_listener_ ? last_listener_->onUnload(this) : 0;
}

void ScriptParser::onPrepare(View * v, XMLNode node)
{
    if (!v)
        return;
    //调用每个节点的onPrepare函数
    auto l = LuaContext::State();
    LUA_STACK_AUTO_CHECK(l);
    LuaContext::GetLOFromCO(l, getObject(v));
    lua_getfield(l, -1, kNotifyOrder);
    if (lua_istable(l, -1))
    {
        lua_getfield(l, -1, "onPrepare");
        lua_pushnil(l);
        lua_copy(l, -4, -1);
        auto count = LuaContext::SafeLOPCall(l, 1, 0);
        lua_pop(l, count);
    }
    lua_pop(l, 2);
}

bool ScriptParser::preprocessHead(XMLNode node, const char * full_path)
{
    //只处理link
    if (!node || !node.name())
        return false;
    if (strcmp(node.name(), kStrLink))
        return false;
    XMLNode::Attribute attrs[] = 
    {
        { kStrType, nullptr }, { kStrModule, nullptr }, { kStrFile, nullptr }
    };
    node.acquire(attrs, sizeof(attrs) / sizeof(attrs[0]));
    auto & type = attrs[0].value;
    auto & module = attrs[1].value;
    auto & file = attrs[2].value;

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
    if (XMLController::IsAbsolutePath(file))
        path = file;
    else
    {
        auto dir = XMLController::GetPathFromFullName(full_path);
        const char * parts[] = { dir.data(), file };
        path = XMLController::JoinPath(parts, sizeof(parts) / sizeof(parts[0]));
    }
    //dofile 只支持 char * 路径不清楚会不会支持中文 所以自己读取文件
    XMLController::FileStream  fs;
    if (!XMLController::ReadFile(path.data(), fs))
    {
        LOG_ERROR << path.data() << " read fail";
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

void ScriptParser::postprocessHead(XMLNode node, const char * full_path)
{
    ;
}

bool ScriptParser::preprocessBody(XMLNode node, View * parent_ob, View ** ob)
{
    if (!node || !node.name())
        return false;
    if (!strcmp(kStrNotify, node.name()))
        return handleNotify(node, parent_ob, ob);
    else if (!strcmp(kStrEvent, node.name()))
        return handleEvent(node, parent_ob, ob);
    return false;
}

void ScriptParser::postprocessBody(XMLNode node, View * parent_ob, View * ob)
{
    if (!ob)
        return;
    if (ob->getClassName() == kClassRoot)
        handleRoot(static_cast<Root *>(ob));
    else if (ob->getClassName() == kClassShape)
        handleShape(static_cast<Shape *>(ob));
    else if (ob->getClassName() == kClassImage)
        handleImage(static_cast<Image *>(ob));
    else if (ob->getClassName() == kClassText)
        handleText(static_cast<Text *>(ob));
    else if (ob->getClassName() == kClassRichEdit)
        handleRichEdit(static_cast<RichEdit *>(ob));
    else if (ob->getClassName() == kClassArea)
        handleArea(static_cast<Area *>(ob));
}

BonesObject * ScriptParser::getObject(View * v)
{
    auto iter = v2bo_.find(v);
    if (iter != v2bo_.end())
        return iter->second;
    return nullptr;
}

View * ScriptParser::getObject(BonesObject * bo)
{
    if (!bo)
        return nullptr;
    for (auto iter = v2bo_.begin(); iter != v2bo_.end(); ++iter)
    {
        if (iter->second == bo)
            return iter->first;
    }
    return nullptr;
}

void ScriptParser::listen(BonesObject * bo, const char * name, BonesScriptListener * listener)
{
    if (!bo || !name)
        return;

    using namespace bones;
    auto l = LuaContext::State();
    LUA_STACK_AUTO_CHECK(l);
    LuaContext::GetLOFromCO(l, bo);
    lua_pushstring(l, name);
    if (listener)
    {
        //创建1个闭包
        lua_pushlightuserdata(l, listener);
        lua_pushcclosure(l, ScriptCB, 1);

    }
    else
        lua_pushnil(l);

    lua_settable(l, -3);
    lua_pop(l, 1);
}

void ScriptParser::push(BonesScriptArg * arg)
{
    if (!arg)
        return;
    auto l = LuaContext::State();
    LUA_STACK_AUTO_CHECK_COUNT(l, 1);
    switch (arg->type)
    {
    case BonesScriptArg::kString:
        lua_pushstring(l, arg->str);
        break;
    case BonesScriptArg::kNumber:
        lua_pushnumber(l, arg->number);
        break;
    case BonesScriptArg::kBoolean:
        lua_pushboolean(l, arg->boolean);
        break;
    case BonesScriptArg::kUserData:
        lua_pushlightuserdata(l, arg->ud);
        break;
    }
}

BonesAnimation * ScriptParser::createAnimate(
    BonesObject * target,
    uint64_t interval, uint64_t due,
    BonesAnimation::RunListener * run,
    BonesAnimation::ActionListener * stop,
    BonesAnimation::ActionListener * start,
    BonesAnimation::ActionListener * pause,
    BonesAnimation::ActionListener * resume, 
    AnimationType type)
{
    return new LuaAnimation(getObject(target), interval, due,
        run, stop, start, pause, resume, type);
}

void ScriptParser::startAnimate(BonesAnimation * ani)
{
    Core::GetAnimationManager()->add(
        static_cast<LuaAnimation *>(ani)->ani());
}

void ScriptParser::stopAnimate(BonesAnimation * ani, bool toend)
{
    Core::GetAnimationManager()->remove(
        static_cast<LuaAnimation *>(ani)->ani(), toend);
}

void ScriptParser::pauseAnimate(BonesAnimation * ani)
{
    Core::GetAnimationManager()->pause(
        static_cast<LuaAnimation *>(ani)->ani());
}

void ScriptParser::resumeAnimate(BonesAnimation * ani)
{
    Core::GetAnimationManager()->resume(
        static_cast<LuaAnimation *>(ani)->ani());
}

void ScriptParser::stopAllAnimate(BonesObject * bo, bool toend)
{
    Core::GetAnimationManager()->remove(getObject(bo), toend);
}

void ScriptParser::handleRoot(Root * ob)
{
    auto lo = AdoptRef(new LuaRoot(ob));
    v2bo_[ob] = lo.get();
}

void ScriptParser::handleShape(Shape * ob)
{
    auto lo = AdoptRef(new LuaShape(ob));
    v2bo_[ob] = lo.get();
}

void ScriptParser::handleImage(Image * ob)
{
    auto lo = AdoptRef(new LuaImage(ob));
    v2bo_[ob] = lo.get();
}

void ScriptParser::handleText(Text * ob)
{
    auto lo = AdoptRef(new LuaText(ob));
    v2bo_[ob] = lo.get();
}

void ScriptParser::handleRichEdit(RichEdit * ob)
{
    auto lo = AdoptRef(new LuaRichEdit(ob));
    v2bo_[ob] = lo.get();
}

void ScriptParser::handleArea(Area * ob)
{
    auto lo = AdoptRef(new LuaArea(ob));
    v2bo_[ob] = lo.get();
}

bool ScriptParser::handleNotify(XMLNode node, View * parent_ob, View ** ob)
{
    if (!parent_ob)
        return false;
    XMLNode::Attribute attrs[] =
    {
        { kStrName, nullptr }, { kStrModule, nullptr }, { kStrFunc, nullptr }
    };

    node.acquire(attrs, sizeof(attrs) / sizeof(attrs[0]));
    auto & name = attrs[0].value;
    auto & module = attrs[1].value;
    auto & func = attrs[2].value;
    BonesObject * bo = getObject(parent_ob);

    bool handle = true;
    if (kClassRoot == bo->getClassName())
        (static_cast<LuaRoot *>(bo))->addNotify(name, module, func);
    else if (kClassArea == bo->getClassName())
        (static_cast<LuaArea *>(bo))->addNotify(name, module, func);
    else
        handle = false;

    return handle;
}

bool ScriptParser::handleEvent(XMLNode node, View * parent_ob, View ** ob)
{
    if (!parent_ob)
        return false;
    XMLNode::Attribute attrs[] =
    {
        { kStrName, nullptr }, 
        { kStrModule, nullptr }, 
        { kStrFunc, nullptr },
        { kStrPhase, nullptr },
    };

    node.acquire(attrs, sizeof(attrs) / sizeof(attrs[0]));
    auto & name = attrs[0].value;
    auto & module = attrs[1].value;
    auto & func = attrs[2].value;
    auto & phase = attrs[3].value;

    BonesObject * bo = getObject(parent_ob);

    bool handle = true;
    if (kClassArea == bo->getClassName())
        (static_cast<LuaArea *>(bo))->addEvent(name, phase, module, func);
    else
        handle = false;

    return handle;
}

//void ScriptParser::handleNodeOnPrepare(XMLNode node, View * ob)
//{
//    if (!node || !ob)
//        return;
//    auto l = LuaContext::State();
//    LUA_STACK_AUTO_CHECK(l);
//    LuaContext::GetLOFromCO(l, ob);
//    if (lua_istable(l, -1))
//    {
//        lua_getfield(l, -1, "onPrepare");
//        assert(lua_isfunction(l, -1) || lua_isnil(l, -1));
//        lua_pushnil(l);
//        lua_copy(l, -3, -1);
//        LuaContext::SafeLOPCall(l, 1, 0);
//    }
//    lua_pop(l, 1);
//}














}