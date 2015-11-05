#include "script_parser.h"
#include "lua_context.h"
#include "lua_check.h"

#include "core/core_imp.h"
#include "core/root.h"
#include "core/logging.h"
#include "core/image.h"
#include "core/text.h"
#include "core/shape.h"
#include "core/rich_edit.h"
#include "core/scroller.h"

#include "core/res_manager.h"
#include "core/animation_manager.h"
#include "core/animation.h"

#include "lua_root.h"
#include "lua_input.h"
#include "lua_shape.h"
#include "lua_image.h"
#include "lua_text.h"
#include "lua_rich_edit.h"
#include "lua_animation.h"
#include "lua_web_view.h"
#include "lua_scroller.h"

namespace bones
{
static const char * kStrPhase = "phase";
static const char * kStrFunc = "func";
static const char * kStrScript = "script";
static const char * kStrEvent = "event";
static const char * kStrNotify = "notify";

////(self, arg1, arg2)
static int ScriptCB(lua_State * l)
{
    static const int kStatckArgCount = 10;
    BonesObject::ScriptArg stack_args[kStatckArgCount];
    BonesObject::ScriptArg * args = stack_args;

    auto param_count = lua_gettop(l);
    if (param_count <= 0)
    {
        BLG_VERBOSE << "script callback has no sender Ref???\n";
        return 0;
    }
        
    size_t arg_count = param_count - 1;
    if (arg_count > kStatckArgCount)
        args = new BonesObject::ScriptArg[arg_count];

    memset(args, 0, sizeof(args[0]) * arg_count);
        
    //参数转化
    int j = 0;
    for (auto i = 2; i <= param_count; ++i)
    {
        args[j] = LuaContext::GetScriptArg(l, i);
        j++;
    }
    //获取sender
    lua_pushnil(l);
    lua_copy(l, 1, -1);
    auto * sender = static_cast<BonesObject *>(LuaContext::CallGetCObject(l));

    assert(sender);
    auto lis = (BonesObject::ScriptListener *)lua_touserdata(l, lua_upvalueindex(1));
    BonesObject::ScriptArg * ret = nullptr;
    size_t ret_count = 0;
    lis->onEvent(sender, args, arg_count, &ret, ret_count);
    for (size_t i = 0; i < ret_count; ++i)
        LuaContext::PushScriptArg(l, ret[i]);

    if (args != stack_args)
        delete[] args;

    return ret_count;
}

ScriptParser::ScriptParser()
:listener_(nullptr), ob_listener_(nullptr)
{
    Core::GetXMLController()->setDelegate(this);
}

ScriptParser::~ScriptParser()
{
    cleanXML();
    Core::GetXMLController()->setDelegate(nullptr);
}

void ScriptParser::setXMLListener(BonesXMLListener * listener)
{
    listener_ = listener;
}

bool ScriptParser::loadXMLString(const char * data)
{
    return Core::GetXMLController()->loadString(data);
}

bool ScriptParser::loadXMLFile(const wchar_t * file)
{
    return Core::GetXMLController()->loadFile(file);
}

void ScriptParser::cleanXML()
{
    Core::GetXMLController()->clean();
}

BonesObject * ScriptParser::getObject(const char * id)
{
    if (!id)
        return nullptr;
    auto v = Core::GetXMLController()->getViewByID(id);
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
        v = Core::GetXMLController()->getViewByID(v, id);
        if (v)
        {
            auto iter = v2bo_.find(v);
            if (iter != v2bo_.end())
                return iter->second;
        }
    }
    return nullptr;
}

BonesObject * ScriptParser::createObject(BonesObject * parent,
                                         const char * label,
                                         const char * id,
                                         const char * class_name,
                                         const char * group_id,
                                         BonesObjectListener * listener)
{
    ob_listener_ = listener;
    auto ob =  getObject(
        Core::GetXMLController()->createView(getObject(parent), label, id, group_id, class_name));
    ob_listener_ = nullptr;
    return ob;
}

void ScriptParser::cleanObject(BonesObject * bo)
{
    if (!bo)
        return;
    Core::GetXMLController()->clean(getObject(bo));
}

BonesResourceManager * ScriptParser::getResourceManager()
{
    return &resource_manager_;
}

BonesPathProxy * ScriptParser::getPathProxy()
{
    return &path_proxy_;
}

BonesShaderProxy * ScriptParser::getShaderProxy()
{
    return &shader_proxy_;
}

BonesPixmapProxy *ScriptParser::getPixmapProxy()
{
    return &pixmap_proxy_;
}

bool ScriptParser::onLoad()
{
    return listener_ ? listener_->onLoad(this) : true;
}

void ScriptParser::onUnload()
{
    listener_ ? listener_->onUnload(this) : 0;
}

void ScriptParser::onPrepare(View * v)
{
    auto ob = getObject(v);
    if (!ob)
        return;
    BonesObjectListener * lis = ob_listener_ ? ob_listener_ : listener_;
    lis ? lis->onPrepare(ob) : 0;
}

void ScriptParser::onCreate(View * v)
{
    if (!v)
        return;
    auto ob = getObject(v);
    if (!ob)
        return;
    if (ob->getClassName() == kClassRoot)
        static_cast<LuaRoot *>(ob)->notifyCreate();
    else if (ob->getClassName() == kClassShape)
        static_cast<LuaShape *>(ob)->notifyCreate();
    else if (ob->getClassName() == kClassImage)
        static_cast<LuaImage *>(ob)->notifyCreate();
    else if (ob->getClassName() == kClassText)
        static_cast<LuaText *>(ob)->notifyCreate();
    else if (ob->getClassName() == kClassInput)
        static_cast<LuaInput *>(ob)->notifyCreate();
    else if (ob->getClassName() == kClassRichEdit)
        static_cast<LuaRichEdit *>(ob)->notifyCreate();
    else if (ob->getClassName() == kClassWebView)
        static_cast<LuaWebView *>(ob)->notifyCreate();
    else if (ob->getClassName() == kClassScroller)
        static_cast<LuaScroller *>(ob)->notifyCreate();
}

void ScriptParser::onDestroy(View * v)
{
    if (!v)
        return;
    auto ob = getObject(v);
    if (!ob)
        return;
    if (ob->getClassName() == kClassRoot)
        static_cast<LuaRoot *>(ob)->notifyDestroy();
    else if (ob->getClassName() == kClassShape)
        static_cast<LuaShape *>(ob)->notifyDestroy();
    else if (ob->getClassName() == kClassImage)
        static_cast<LuaImage *>(ob)->notifyDestroy();
    else if (ob->getClassName() == kClassText)
        static_cast<LuaText *>(ob)->notifyDestroy();
    else if (ob->getClassName() == kClassInput)
        static_cast<LuaInput *>(ob)->notifyDestroy();
    else if (ob->getClassName() == kClassRichEdit)
        static_cast<LuaRichEdit *>(ob)->notifyDestroy();
    else if (ob->getClassName() == kClassWebView)
        static_cast<LuaWebView *>(ob)->notifyDestroy();
    else if (ob->getClassName() == kClassScroller)
        static_cast<LuaScroller *>(ob)->notifyDestroy();
}

void ScriptParser::onCreating(View * v)
{
    if (v->getClassName() == kClassRoot)
        handleView<LuaRoot, Root>(v);
    else if (v->getClassName() == kClassShape)
        handleView<LuaShape, Shape>(v);
    else if (v->getClassName() == kClassImage)
        handleView<LuaImage, Image>(v);
    else if (v->getClassName() == kClassText)
        handleView<LuaText, Text>(v);
    else if (v->getClassName() == kClassInput)
        handleView<LuaInput, Input>(v);
    else if (v->getClassName() == kClassRichEdit)
        handleView<LuaRichEdit, RichEdit>(v);
    else if (v->getClassName() == kClassWebView)
        handleView<LuaWebView, WebView>(v);
    else if (v->getClassName() == kClassScroller)
        handleView<LuaScroller, Scroller>(v);
}

void ScriptParser::onDestroying(View * v)
{
    LuaContext::ClearLOFromCO(LuaContext::State(), v);
    v2bo_.erase(v);
}

bool ScriptParser::preprocessHead(XMLNode node, const char * label, const char * full_path)
{
    //只处理link
    if (!label)
        return false;
    if (strcmp(label, kStrLink))
        return false;
    XMLNode::Attribute attrs[] = 
    {
        { kStrType, nullptr }, { kStrModule, nullptr }, { kStrFile, nullptr }
    };
    node.acquire(attrs, sizeof(attrs) / sizeof(attrs[0]));
    auto & type = attrs[0].value;
    auto & name = attrs[1].value;
    auto & file = attrs[2].value;

    if (!type || strcmp(type, kStrScript))
        return false;
    if (!name)
        return true;

    //link script module
    auto l = LuaContext::State();
    LUA_STACK_AUTO_CHECK(l);
    lua_getglobal(l, name);
    if (lua_istable(l, -1))
    {//模块已经load
        lua_pop(l, 1);
        return true;
    }
    lua_pop(l, 1);
    //没有指定路径忽略
    if (!file)
        return true;

    std::string path = XMLController::GetRealPath(file, full_path);
    //dofile 只支持 char * 路径不清楚会不会支持中文 所以自己读取文件
    XMLController::FileStream  fs;
    if (!XMLController::ReadString(path.data(), fs))
    {
        BLG_ERROR << path.data() << " read fail";
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
            BLG_ERROR << "lua module must return a table";
        if (lua_istable(l, -1))
        {
            lua_setglobal(l, name);
            count--;
        }
        lua_pop(l, count);
    }
    else
    {
        BLG_ERROR << "lua module " << name << " load fail " << lua_tostring(l, -1);
        lua_pop(l, 1);
    }

    return true;
}

void ScriptParser::postprocessHead(XMLNode node, const char * label, const char * full_path)
{
    ;
}

bool ScriptParser::preprocessBody(XMLNode node, const char * label, View * parent_ob, View ** ob)
{
    if (!label)
        return false;
    if (!strcmp(kStrNotify, label))
        return handleNotify(node, parent_ob, ob);
    else if (!strcmp(kStrEvent, label))
        return handleEvent(node, parent_ob, ob);
    return false;
}

void ScriptParser::postprocessBody(XMLNode node, const char * label, View * parent_ob, View * ob)
{
    ;
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

void ScriptParser::scriptSet(BonesObject * bo, const char * name,
    BonesObject::ScriptListener * listener)
{
    if (!bo || !name)
        return;

    auto l = LuaContext::State();
    LUA_STACK_AUTO_CHECK(l);
    LuaContext::GetLOFromCO(l, bo);
    if (lua_istable(l, -1))
    {
        lua_pushstring(l, name);
        if (listener)
        {
            //创建1个闭包
            lua_pushlightuserdata(l, listener);
            lua_pushcclosure(l, &ScriptCB, 1);
        }
        else
            lua_pushnil(l);
        lua_settable(l, -3);
    }
    else
        BLG_ERROR << "scriptSet::unknown bones object";

    lua_pop(l, 1);
}

void ScriptParser::scriptSet(BonesObject * bo, const char * name,
    BonesObject::ScriptArg arg)
{
    if (!bo || !name)
        return;

    using namespace bones;
    auto l = LuaContext::State();
    LUA_STACK_AUTO_CHECK(l);
    LuaContext::GetLOFromCO(l, bo);
    if (lua_istable(l, -1))
    {
        lua_pushstring(l, name);
        LuaContext::PushScriptArg(l, arg);
        lua_settable(l, -3);
    }
    else
        BLG_ERROR << "scriptSet::unknown bones object";

    lua_pop(l, 1);
}

void ScriptParser::scriptInvoke(BonesObject * bo, const char * name,
    BonesObject::ScriptArg * param, size_t param_count,
    BonesObject::ScriptArg * ret, size_t ret_count)
{
    if (!bo || !name)
        return;

    using namespace bones;
    auto l = LuaContext::State();
    LUA_STACK_AUTO_CHECK(l);
    LuaContext::GetLOFromCO(l, bo);
    if (lua_istable(l, -1))
    {
        lua_getfield(l, -1, name);
        if (param && param_count)
        {
            for (size_t i = 0; i < param_count; ++i)
                LuaContext::PushScriptArg(l, param[i]);
        }
        auto count = LuaContext::SafeLOPCall(l, param_count, LUA_MULTRET);
        if (ret && ret_count)
        {
            if (ret_count >= (size_t)count)
                ret_count = count;
            else//ret数组过小
                BLG_ERROR << "scriptInvoke 'ret' overflow";
            for (size_t i = 0; i < ret_count; ++i)
                ret[i] = LuaContext::GetScriptArg(l, -count + i);
        } 
        lua_pop(l, count);
    }
    else
        BLG_ERROR << "scriptInvoke::unknown bones object";

    lua_pop(l, 1);
}

void ScriptParser::scriptInvoke(BonesObject * bo, const char * name,
    BonesObject::ScriptListener * lis,
    BonesObject::ScriptArg * ret, size_t ret_count)
{
    if (!bo || !name)
        return;

    using namespace bones;
    auto l = LuaContext::State();
    LUA_STACK_AUTO_CHECK(l);
    LuaContext::GetLOFromCO(l, bo);
    if (lua_istable(l, -1))
    {
        lua_getfield(l, -1, name);
        //创建一个闭包
        lua_pushlightuserdata(l, lis);
        lua_pushcclosure(l, &ScriptCB, 1);

        auto count = LuaContext::SafeLOPCall(l, 1, LUA_MULTRET);
        if (ret && ret_count)
        {
            if (ret_count >= (size_t)count)
                ret_count = count;
            else//ret数组过小
                BLG_ERROR << "scriptInvoke 'ret' overflow";

            for (size_t i = 0; i < ret_count; ++i)
                ret[i] = LuaContext::GetScriptArg(l, -count + i);
        }
        lua_pop(l, count);
    }
    else
        BLG_ERROR << "scriptInvoke::unknown bones object";

    lua_pop(l, 1);
}

BonesObject::Animation ScriptParser::createAnimate(
    BonesObject * target,
    uint64_t interval, uint64_t due,
    BonesObject::AnimationRunListener * run,
    BonesObject::AnimationActionListener * stop,
    BonesObject::AnimationActionListener * start,
    BonesObject::AnimationActionListener * pause,
    BonesObject::AnimationActionListener * resume,
    AnimationType type)
{
    return new LuaAnimation(getObject(target), interval, due,
        run, stop, start, pause, resume, type);
}

void ScriptParser::startAnimate(BonesObject::Animation ani)
{
    Core::GetAnimationManager()->add(
        static_cast<LuaAnimation *>(ani)->ani());
}

void ScriptParser::stopAnimate(BonesObject::Animation ani, bool toend)
{
    Core::GetAnimationManager()->remove(
        static_cast<LuaAnimation *>(ani)->ani(), toend);
}

void ScriptParser::pauseAnimate(BonesObject::Animation ani)
{
    Core::GetAnimationManager()->pause(
        static_cast<LuaAnimation *>(ani)->ani());
}

void ScriptParser::resumeAnimate(BonesObject::Animation ani)
{
    Core::GetAnimationManager()->resume(
        static_cast<LuaAnimation *>(ani)->ani());
}

void ScriptParser::stopAllAnimate(BonesObject * bo, bool toend)
{
    Core::GetAnimationManager()->remove(getObject(bo), toend);
}

const char * ScriptParser::getID(BonesObject * bob)
{
    return Core::GetXMLController()->getID(getObject(bob));
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
    if (!bo)
        return false;

    if (bo->getClassName() == kClassRoot)
        static_cast<LuaRoot *>(bo)->addNotify(name, module, func);
    else if (bo->getClassName() == kClassShape)
        static_cast<LuaShape *>(bo)->addNotify(name, module, func);
    else if (bo->getClassName() == kClassImage)
        static_cast<LuaImage *>(bo)->addNotify(name, module, func);
    else if (bo->getClassName() == kClassText)
        static_cast<LuaText *>(bo)->addNotify(name, module, func);
    else if (bo->getClassName() == kClassRichEdit)
        static_cast<LuaRichEdit *>(bo)->addNotify(name, module, func);
    else if (bo->getClassName() == kClassWebView)
        static_cast<LuaWebView *>(bo)->addNotify(name, module, func);
    else if (bo->getClassName() == kClassScroller)
        static_cast<LuaScroller *>(bo)->addNotify(name, module, func);
    else
        assert(0);

    return true;
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
    if (!bo)
        return false;

    if (bo->getClassName() == kClassRoot)
        static_cast<LuaRoot *>(bo)->addEvent(name, phase, module, func);
    else if (bo->getClassName() == kClassShape)
        static_cast<LuaShape *>(bo)->addEvent(name, phase, module, func);
    else if (bo->getClassName() == kClassImage)
        static_cast<LuaImage *>(bo)->addEvent(name, phase, module, func);
    else if (bo->getClassName() == kClassText)
        static_cast<LuaText *>(bo)->addEvent(name, phase, module, func);
    else if (bo->getClassName() == kClassRichEdit)
        static_cast<LuaRichEdit *>(bo)->addEvent(name, phase, module, func);
    else if (bo->getClassName() == kClassWebView)
        static_cast<LuaWebView *>(bo)->addEvent(name, phase, module, func);
    else if (bo->getClassName() == kClassScroller)
        static_cast<LuaScroller *>(bo)->addEvent(name, phase, module, func);

    return true;
}














}