﻿#include "xml_controller.h"
#include "encoding.h"
#include "helper.h"
#include "root.h"
#include "area.h"
#include "rich_edit.h"
#include "image.h"
#include "text.h"
#include "shape.h"

#include "logging.h"
#include "css_manager.h"
#include "res_manager.h"

#include <fstream>

namespace bones
{

const char * kStrType = "type";
const char * kStrModule = "module";
const char * kStrName = "name";
const char * kStrFile = "file";
const char * kStrCSS = "css";
const char * kStrXML = "xml";
const char * kStrID = "id";
const char * kStrClass = "class";
const char * kStrGroup = "group";
const char * kStrPixmap = "pixmap";
const char * kStrBones = "bones";
const char * kStrHead = "head";
const char * kStrBody = "body";
const char * kStrStyle = "style";
const char * kStrLink = "link";
const char * kStrRoot = "root";
const char * kStrArea = "area";
const char * kStrRichEdit = "richedit";
const char * kStrImage = "image";
const char * kStrText = "text";
const char * kStrShape = "shape";

XMLController::XMLController()
:delegate_(nullptr)
{
    ;
}

XMLController::~XMLController()
{

}

void XMLController::setDelegate(Delegate * delegate)
{
    delegate_ = delegate;
}

bool XMLController::loadString(const char * data)
{
    clean(true);
    size_t len = 0;
    if ( !data || !(len = strlen(data)) )
        return false;
    
    main_module_.xml_file.resize(len + 1);
    memcpy(&main_module_.xml_file[0], data, len);
    main_module_.xml_file[len] = 0;

    return loadMainModule(main_module_);
}

bool XMLController::loadMainModule(Module & mod)
{
    auto & doc = mod.doc;

    if (!doc.parse(mod.xml_file.data()))
        return false;

    if (!checkFormat(doc))
        return false;

    parseModuleHead(mod);
    parseModuleBody(mod);
    return true;
}

View * XMLController::getViewByID(const char * id)
{
    return getViewByID(nullptr, id);
}

View * XMLController::getViewByID(View * ob, const char * id)
{
    if (!id)
        return nullptr;

    if (!ob)
    {
        for (auto iter = roots_.begin(); iter != roots_.end(); ++iter)
        {
            auto target = getViewByID(iter->get(), id);
            if (target)
                return target;
        }
    }
    else
    {
        RefPtr<View> pa;
        pa.reset(ob);
        auto iter = ob2node_.find(pa);

        if (iter != ob2node_.end())
        {
            auto attr = acquire(iter->second, kStrID);
            if (attr && !strcmp(attr, id))
                return iter->first.get();
            else
            {//找子
                auto child = pa->getFirstChild();
                while (child)
                {
                    auto target = getViewByID(child, id);
                    if (target)
                        return target;
                    child = child->getNextSibling();
                }
            }
        }
    }
    return nullptr;
}

View * XMLController::createView(View * parent,
                                 const char * label,
                                 const char * id,
                                 const char * group_id,
                                 const char * class_name)

{
    if (!label)
        return nullptr;

    View * v = nullptr;
    bool bret = createViewFromNode(XMLNode(), label, parent, &v);
    if (bret)
    {
        RefPtr<View> rv;
        rv.reset(v);
        auto & attrs = ob2node_[rv];
        attrs[kStrID] = id ? id : "";
        attrs[kStrGroup] = group_id ? group_id : "";
        attrs[kStrClass] = class_name ? class_name : "";
        //设置通知
        notifyPrepare(v);
        applyClass(v);
        return v;
    }
    return nullptr;
}

void XMLController::clean(View * v)
{
    //释放指定节点
    if (!v)
        return;
    RefPtr<View> rv;
    rv.reset(v);
    //解除父子结构
    if (kClassRoot == rv->getClassName())
    {
        auto iter = roots_.begin();
        for (; iter != roots_.end(); ++iter)
        {
            if ((*iter).get() != v)
                continue;
            NativeEvent n = { WM_MOUSELEAVE, 0, 0, 0 };
            (*iter)->handleMouse(n);
            roots_.erase(iter);
            break;
        }
    }
    rv->detachChildren();
    rv->detachFromParent();
    ob2node_.erase(rv);
    //删除xml中的节点
    //rapidxml并不会释放node的内存所以 删除xml节点没意义了
}

void XMLController::parseModuleHead(Module & mod)
{
    auto & doc = mod.doc;
    if (!checkFormat(doc))
        return;
    auto head = getHead(doc);
    if (!head)
        return;
    auto node = head.firstChild();
    auto mod_fullname = mod.xml_fullname.data();
    while (node)
    {
        //如果delegate处理 则自己不处理
        auto name = node.name();
        bool bret = delegate_ ? delegate_->preprocessHead(node, name, mod_fullname) : false;
        if (!bret && name)
        {
            if (!strcmp(name, kStrStyle))
                bret = handleStyle(node, mod.xml_fullname.data());
            else if (!strcmp(name, kStrLink))
                bret = handleLink(node, mod.xml_fullname.data());
        }
        if (bret)
            delegate_ ? delegate_->postprocessHead(node, name, mod_fullname) : 0;
        node = node.nextSibling();
    }
}

bool XMLController::handleStyle(XMLNode node, const char * full_path)
{
    if (node.valueSize() > 0 && node.value())
        Core::GetCSSManager()->append(node.value());
    return true;
}

bool XMLController::handleLink(XMLNode node, const char * full_path)
{
    XMLNode::Attribute link[] =
    { 
        { kStrType, nullptr }, { kStrModule, nullptr }, { kStrFile, nullptr },
        { kStrName, nullptr}
    };
    node.acquire(link, sizeof(link) / sizeof(link[0]));

    auto & type = link[0].value;
    auto & mod = link[1].value;
    auto & file = link[2].value;
    auto & name = link[3].value;

    if (!type)
        return false;

    if (!strcmp(kStrCSS, type))
    {//css文件 直接添加到当前的info中去
        //link css 必须指定file file为空就忽略
        if (!file)
            return true;

        css_files_.push_back(FileStream());
        auto & file_stream = css_files_.back();
        if (ReadString(GetRealPath(file, full_path).data(), file_stream))
            Core::GetCSSManager()->append(file_stream.data());
        return true;
    }
    else if (!strcmp(kStrXML, type))
    {//xml 文件 作为一个新的module
        //link xml 必须指明 module 否则忽略掉
        if (!mod)
            return true;
        //module已经存在直接跳过
        if (modules_.find(mod) != modules_.end())
            return true;
        //file 不存在 module link失败
        if (!file)
            return true;

        auto & sub_mod = modules_[mod];
        sub_mod.xml_fullname = GetRealPath(file, full_path);
        //读取xml文件内容
        if (!ReadString(sub_mod.xml_fullname.data(), sub_mod.xml_file))
            return false;
        auto & sub_doc = sub_mod.doc;
        if (!sub_doc.parse(sub_mod.xml_file.data()))
            return false;

        if (!checkFormat(sub_doc))
            return false;
        parseModuleHead(sub_mod);
        return true;
    }
    else if (!strcmp(kStrPixmap, type))
    {//必须指定name
        if (!name)
            return true;
        //查找name是否存在
        auto pm = Core::GetResManager()->getPixmap(name);
        if (pm.isValid())
            return true;
        //位图不存在则载入
        if (!file)
            return true;
        auto path = GetRealPath(file, full_path);
        FileStream fs;
        if (ReadFile(path.data(), fs))
        {//增加1个位图
            if (pm.decode(&fs[0], fs.size()))
                Core::GetResManager()->add(name, pm);
            else
                LOG_ERROR << path << " can't decode to pixmap";
        }
        else
            LOG_ERROR << path << " read fail";
    }
    return false;
}

bool XMLController::checkFormat(XMLDocument & doc)
{
    auto root = doc.root();
    if (!checkBones(root))
        return false;
    auto node = root.firstChild();
    if (checkHead(node))
        node = node.nextSibling();
    return checkBody(node);
}

bool XMLController::checkBones(XMLNode root)
{
    return root && !strcmp(root.name(), kStrBones);
}

bool XMLController::checkHead(XMLNode node)
{
    return node && !strcmp(node.name(), kStrHead);
}

bool XMLController::checkBody(XMLNode node)
{
    return node && !strcmp(node.name(), kStrBody);
}

XMLNode XMLController::getBody(const XMLDocument & doc)
{
    auto root = doc.root();
    if (!checkBones(root))
        return nullptr;
    auto node = root.firstChild();
    if (checkHead(node))
        node = node.nextSibling();
    if (!checkBody(node))
        return nullptr;
    return node;
}

XMLNode XMLController::getHead(const XMLDocument & doc)
{
    auto root = doc.root();
    if (!checkBones(root))
        return nullptr;
    auto head = root.firstChild();
    if (!checkHead(head))
        return nullptr;
    return head;
}

void XMLController::notifyPrepare()
{//prepare过程中不要clean
    for (auto iter = roots_.begin(); iter != roots_.end(); ++iter)
        notifyPrepare(iter->get());
}

void XMLController::notifyPrepare(View * ob)
{
    RefPtr<View> pv;
    auto child = ob->getFirstChild();
    while (child)
    {
        pv.reset(child);
        notifyPrepare(child);
        child = child->getNextSibling();
    }
    delegate_ ? delegate_->onPrepare(ob) : 0;
}

void XMLController::applyClass()
{
    for (auto iter = roots_.begin(); iter != roots_.end(); ++iter)
        applyClass(iter->get());
}

void XMLController::applyClass(View * ob)
{
    if (!ob)
        return;
    RefPtr<View> pv;
    auto child = ob->getFirstChild();
    while (child)
    {
        pv.reset(child);
        applyClass(child);
        child = child->getNextSibling();
    }

    pv.reset(ob);
    auto iter = ob2node_.find(pv);
    if (iter == ob2node_.end())
        return;

    auto attr = acquire(iter->second, kStrClass);
    if (attr)
        Core::GetCSSManager()->applyClass(ob, attr);
}

void XMLController::clean(bool notify)
{
    //停止所有动画
    Core::Clean();
    //发送通知
    if (notify)
        delegate_ ? delegate_->onUnload() : 0;
    //删除节点父子结构
    for (auto iter = roots_.begin(); iter != roots_.end(); ++iter)
    {
        (*iter)->recursiveDetachChildren();
        NativeEvent n = { WM_MOUSELEAVE, 0, 0, 0 };
        (*iter)->handleMouse(n);
    }
    ob2node_.clear();
    roots_.clear();

    modules_.clear();
    main_module_.clean();
}

void XMLController::parseModuleBody(const Module & mod)
{
    auto body = getBody(mod.doc);
    if (!body)
        return;
    auto node = body.firstChild();
    while (node)
    {//遍历node 所有的兄弟
        //迭代每个node
        createViewFromNode(node, node.name(), nullptr, nullptr);
        node = node.nextSibling();
    }
    //创建XML 节点完毕
    //notify load
    auto load = delegate_ ? delegate_->onLoad() : true;
    if (load)
    {
        //prepare 和class 都是 先子后父
        //prepare在前 是因为应用class时 可能有事件回调
        //prepare中可以注册好回调等待处理
        notifyPrepare();
        applyClass();        
    }
    else
        clean(false);
}

bool XMLController::createViewFromNode(XMLNode node, const char * label, View * parent_ob, View ** ob)
{
    if (!label)
        return false;

    View * node_ob = nullptr;
    bool bret = delegate_ ? delegate_->preprocessBody(node, label, parent_ob, &node_ob) : 0;
    if (!bret)
    {//没有被预处理
        bool extend = false;

        if (!strcmp(label, kStrRoot))
            bret = handleRoot(node, parent_ob, &node_ob);
        else if (!strcmp(label, kStrArea))
            bret = handleArea(node, parent_ob, &node_ob);
        else if (!strcmp(label, kStrRichEdit))
            bret = handleRichEdit(node, parent_ob, &node_ob);
        else if (!strcmp(label, kStrImage))
            bret = handleImage(node, parent_ob, &node_ob);
        else if (!strcmp(label, kStrText))
            bret = handleText(node, parent_ob, &node_ob);
        else if (!strcmp(label, kStrShape))
            bret = handleShape(node, parent_ob, &node_ob);
        else
        {
            extend = true;
            bret = handleExtendLabel(node, label, parent_ob, &node_ob);
        }
        if (bret && !extend)
            delegate_ ? delegate_->postprocessBody(node, label, parent_ob, node_ob) : 0;
    }

    if (bret)
    {
        auto child = node.firstChild();
        while (child)
        {
            createViewFromNode(child, child.name(), node_ob, nullptr);
            child = child.nextSibling();
        }
        if (ob)
            *ob = node_ob;
    }

    return bret;
}

bool XMLController::handleExtendLabel(XMLNode node, const char * label, View * parent_ob, View ** ob)
{
    if (!label)
        return false;

    auto iter = modules_.find(label);
    if (iter == modules_.end())
        return false;
    auto & other_mod = iter->second;
    auto body = getBody(other_mod.doc);
    if (!body)
    {
        LOG_VERBOSE << "module: " << label << "empty ????";
        return false;
    }
    View * node_ob = nullptr;
    auto child = body.firstChild();

    bool bret = createViewFromNode(child, child.name(), parent_ob, &node_ob);
    if (bret && node_ob)
    {//如果是扩展标签的话 node 应该是当前的node
        saveNode(node_ob, node);
        if (ob)
            *ob = node_ob;
    }
    return bret;
}

bool XMLController::handleRoot(XMLNode node, View * parent_ob, View ** ob)
{
    //root 不能添加到父上
    assert(!parent_ob);
    auto v = AdoptRef(new Root);
    saveNode(v.get(), node);
    roots_.push_back(v);
    if (ob)
        *ob = v.get();
    return v != nullptr;
}

bool XMLController::handleText(XMLNode node, View * parent_ob, View ** ob)
{
    assert(parent_ob);
    auto v = AdoptRef(new Text);
    saveNode(v.get(), node);

    if (parent_ob)
        ((View *)parent_ob)->attachChildToBack(v.get());
    if (ob)
        *ob = v.get();
    return v != nullptr;
}

bool XMLController::handleShape(XMLNode node, View * parent_ob, View ** ob)
{
    auto v = AdoptRef(new Shape);
    saveNode(v.get(), node);
    if (parent_ob)
        ((View *)parent_ob)->attachChildToBack(v.get());
    if (ob)
        *ob = v.get();

    return v != nullptr;
}


bool XMLController::handleImage(XMLNode node, View * parent_ob, View ** ob)
{
    auto v = AdoptRef(new Image);
    saveNode(v.get(), node);
    if (parent_ob)
        ((View *)parent_ob)->attachChildToBack(v.get());
    if (ob)
        *ob = v.get();
    return v != nullptr;
}

bool XMLController::handleArea(XMLNode node, View * parent_ob, View ** ob)
{
    auto v = AdoptRef(new Area);
    saveNode(v.get(), node);
    if (parent_ob)
        ((View *)parent_ob)->attachChildToBack(v.get());
    if (ob)
        *ob = v.get();
    return v != nullptr;
}

bool XMLController::handleRichEdit(XMLNode node, View * parent_ob, View ** ob)
{
    auto v = AdoptRef(new RichEdit);
    saveNode(v.get(), node);
    if (parent_ob)
        ((View *)parent_ob)->attachChildToBack(v.get());

    if (ob)
        *ob = v.get();
    return v != nullptr;
}

bool XMLController::ReadString(const char * file_path, FileStream & file)
{
    using namespace std;
    if (!file_path)
        return false;
    auto wfile_path = Encoding::FromUTF8(file_path);
    if (ReadFile(wfile_path.data(), file))
    {
        file.push_back(0);
        return true;
    }
    return false;
}

bool XMLController::ReadFile(const char * file_path, FileStream & file)
{
    using namespace std;
    if (!file_path)
        return false;
    auto wfile_path = Encoding::FromUTF8(file_path);
    return ReadFile(wfile_path.data(), file);
}

bool XMLController::ReadFile(const wchar_t * file_path, FileStream & file)
{
    using namespace std;
    if (!file_path)
        return false;
    ifstream stream(file_path, ios::binary);
    if (!stream)
    {
        LOG_ERROR << file_path << "read fail";
        return false;
    }
        
    stream.unsetf(ios::skipws);
    auto begin = stream.tellg();
    stream.seekg(0, ios::end);
    size_t size = static_cast<size_t>(stream.tellg() - begin);
    stream.seekg(0);
    file.clear();
    file.resize(size);
    stream.read(&file[0], size);
    return true;
}

const char * XMLController::acquire(NodeAttrs & attrs, const char * attr_name)
{
    auto iter = attrs.find(attr_name);
    if (iter != attrs.end())
        return iter->second.data();
    return nullptr;
}

void XMLController::saveNode(View * v, XMLNode node)
{
    RefPtr<View> rv;
    rv.reset(v);
    auto & attrs = ob2node_[rv];
    if (!node)
        attrs.clear();
    else
    {
        auto attr = node.firstAttribute();
        while (attr)
        {
            auto name = attr.name();
            auto value = attr.value();
            if (name && value)
                attrs[name] = value;
            attr = attr.nextSibling();
        }
    }
}

std::string XMLController::GetRealPath(const char * file_value, const char * module_path)
{
    if (!file_value)
        return "";

    if (IsAbsolutePath(file_value))
        return file_value;
    else
    {
        auto dir = GetPathFromFullName(module_path);
        const char * parts[] = { dir.data(), file_value };
        return JoinPath(parts, sizeof(parts) / sizeof(parts[0]));
    }
}

std::string XMLController::GetPathFromFullName(const char * fullname)
{
    if (!fullname || !strlen(fullname))
    {//直接返回当前目录
        //std::wstring wpath;
        //auto len = ::GetCurrentDirectory(0, NULL);
        //wpath.resize(len);
        //auto result = ::GetCurrentDirectory(len, &wpath[0]);
        //if (!result)
        //    LOG_VERBOSE << "current directory is empty???";
        //return Encoding::ToUTF8(wpath.data());
        return ".";
    }
    std::string path;
    auto last_backslash = std::strrchr(fullname, '\\');
    auto last_slash = std::strrchr(fullname, '/');
    auto slash = last_backslash > last_slash ? last_backslash : last_slash;
    if (!slash)
        return path;
    return path.assign(fullname, slash - fullname);
}

std::string XMLController::JoinPath(const char ** path, int count)
{
    if (!path || !count)
        return "";
    std::string paths;
    for (auto i = 0; i < count; ++i)
    {
        if (nullptr == path[i])
            break;

        if (0 == i)
        {
            paths = path[0];
            continue;
        }
        //如果path 末尾不是斜杆 那么加1个斜杆
        char last_char = paths[paths.length() - 1];
        if ('\\' != last_char || '/' != last_char)
            paths.append("\\");
        paths.append(path[i]);
    }
    return paths;
}

bool XMLController::IsAbsolutePath(const char * path)
{
    if (!path)
        return false;
    if (strlen(path) < 2)
        return false;
    return isalpha(path[0]) && ':' == path[1];
}

}