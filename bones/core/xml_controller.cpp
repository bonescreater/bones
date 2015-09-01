#include "xml_controller.h"
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

#include <functional>
#include <fstream>

namespace bones
{

enum Label
{
    kLABEL_UNKNOWN,
    kLABEL_BONES,
    kLABEL_HEAD,
    kLABEL_LINK,
    kLABEL_STYLE,
    kLABEL_BODY,
    kLABEL_ROOT,
    kLABEL_AREA,
    kLABEL_RICHEDIT,
    kLABEL_IMAGE,
    kLABEL_TEXT,
    kLABEL_SHAPE,
};

const char * kStrType = "type";
const char * kStrModule = "module";
const char * kStrFile = "file";
const char * kStrCSS = "css";
const char * kStrLink = "link";
const char * kStrXML = "xml";
const char * kStrID = "id";
const char * kStrClass = "class";
const char * kStrGroup = "group";


Label LabelFromName(const char * name);

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
    
    main_module_.xml_file.resize(len + 1, 0);
    memcpy(&main_module_.xml_file[0], data, len);

    return loadMainModule(main_module_);
}

bool XMLController::loadMainModule(Module & mod)
{
    auto & doc = mod.doc;

    if (!doc.parse(mod.xml_file.data()))
        return false;

    auto root = doc.root();

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
            XMLNode::Attribute attr = { kStrID, nullptr };
            iter->second.acquire(&attr, 1);

            if (attr.value && !strcmp(attr.value, id))
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


void XMLController::parseModuleHead(Module & mod)
{
    using namespace rapidxml;
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
        bool bret = delegate_ ? delegate_->preprocessHead(node, mod_fullname) : false;
        if (!bret)
        {
            switch (LabelFromName(node.name()))
            {
            case kLABEL_STYLE:
                bret = handleStyle(node, mod.xml_fullname.data());
                break;
            case kLABEL_LINK:
                bret = handleLink(node, mod.xml_fullname.data());
                break;
            default:
                //不认识的标签 放过
                break;
            }
        }
        if (bret)
            delegate_ ? delegate_->postprocessHead(node, mod_fullname) : 0;
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
        { kStrType, nullptr }, { kStrModule, nullptr }, { kStrFile, nullptr } 
    };
    node.acquire(link, sizeof(link) / sizeof(link[0]));

    auto & type = link[0].value;
    auto & module = link[1].value;
    auto & file = link[2].value;
    if (!type)
        return false;

    if (!strcmp(kStrCSS, type))
    {//css文件 直接添加到当前的info中去
        //link css 必须指定file file为空就忽略
        if (!file)
            return true;

        css_files_.push_back(FileStream());
        auto & file_stream = css_files_.back();
        std::string path;
        if (IsAbsolutePath(file))
            path = file;
        else
        {
            auto dir = GetPathFromFullName(full_path);
            const char * parts[] = { dir.data(), file };
            path = JoinPath(parts, sizeof(parts) / sizeof(parts[0]));
        }
        if (ReadFile(path.data(), file_stream))
            Core::GetCSSManager()->append(file_stream.data());
        return true;
    }
    else if (!strcmp(kStrXML, type))
    {//xml 文件 作为一个新的module
        //link xml 必须指明 module 否则忽略掉
        if (!module)
            return true;
        //module已经存在直接跳过
        if (modules_.find(module) != modules_.end())
            return true;
        //file 不存在 module link失败
        if (!file)
            return true;
        auto & sub_mod = modules_[module];
        if (IsAbsolutePath(file))
            sub_mod.xml_fullname = file;
        else
        {            
            auto path = GetPathFromFullName(full_path);
            const char * parts[] = { path.data(), file };
            sub_mod.xml_fullname = JoinPath(parts, sizeof(parts) / sizeof(parts[0]));
        }

        //读取xml文件内容
        if (!ReadFile(sub_mod.xml_fullname.data(), sub_mod.xml_file))
            return false;
        auto & sub_doc = sub_mod.doc;
        if (!sub_doc.parse(sub_mod.xml_file.data()))
            return false;

        if (!checkFormat(sub_doc))
            return false;
        parseModuleHead(sub_mod);
        return true;
    }
    return false;
}

bool XMLController::checkFormat(XMLDocument & doc)
{
    auto root = doc.root();
    if (!checkRoot(root))
        return false;
    auto node = root.firstChild();
    if (checkHead(node))
        node = node.nextSibling();
    return checkBody(node);
}

bool XMLController::checkRoot(XMLNode root)
{
    return root && LabelFromName(root.name()) == kLABEL_BONES;
}

bool XMLController::checkHead(XMLNode node)
{
    return node && LabelFromName(node.name()) == kLABEL_HEAD;
}

bool XMLController::checkBody(XMLNode node)
{
    return node && LabelFromName(node.name()) == kLABEL_BODY;
}

XMLNode XMLController::getBody(const XMLDocument & doc)
{
    auto root = doc.root();
    if (!checkRoot(root))
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
    if (!checkRoot(root))
        return nullptr;
    auto head = root.firstChild();
    if (!checkHead(head))
        return nullptr;
    return head;
}

void XMLController::notifyPrepare()
{//prepare过程中不要clean
    for (auto iter = roots_.begin(); iter != roots_.end(); ++iter)
        notifyPrepare(iter->get(), ob2node_[*iter]);
}

void XMLController::notifyPrepare(View * ob, XMLNode node)
{
    RefPtr<View> pv;
    auto child = ob->getFirstChild();
    while (child)
    {
        pv.reset(child);
        notifyPrepare(child, ob2node_[pv]);
        child = child->getNextSibling();
    }
    delegate_ ? delegate_->onPrepare(ob, node) : 0;
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

    XMLNode::Attribute attrs[] =
    {
        { kStrClass, nullptr }, { kStrID, nullptr }, { kStrGroup, nullptr }
    };
    iter->second.acquire(attrs, sizeof(attrs) / sizeof(attrs[0]));

    if (attrs[0].value)
        Core::GetCSSManager()->applyClass(ob, attrs[0].value);
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
        createViewFromNode(node, nullptr, nullptr);
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

bool XMLController::createViewFromNode(XMLNode node, View * parent_ob, View ** ob)
{
    if (!node)
        return false;

    View * node_ob = nullptr;
    bool bret = delegate_ ? delegate_->preprocessBody(node, parent_ob, &node_ob) : 0;
    if (!bret)
    {//没有被预处理
        bool extend = false;
        switch (LabelFromName(node.name()))
        {
        case kLABEL_ROOT:
            //创建窗口或者rootview
            bret = handleRoot(node, parent_ob, &node_ob);           
            break;
        case kLABEL_AREA:
            bret = handleArea(node, parent_ob, &node_ob);
            break;
        case kLABEL_RICHEDIT:
            bret = handleRichEdit(node, parent_ob, &node_ob);
            break;
        case kLABEL_IMAGE:
            bret = handleImage(node, parent_ob, &node_ob);
            break;
        case kLABEL_TEXT:
            bret = handleText(node, parent_ob, &node_ob);
            break;
        case kLABEL_SHAPE:
            bret = handleShape(node, parent_ob, &node_ob);
            break;
        default:
            extend = true;
            bret = handleExtendLabel(node, parent_ob, &node_ob);
            break;
        }
        if (bret && !extend)
            delegate_ ? delegate_->postprocessBody(node, parent_ob, node_ob) : 0;
    }
    

    if (bret)
    {
        auto child = node.firstChild();
        while (child)
        {
            createViewFromNode(child, node_ob, nullptr);
            child = child.nextSibling();
        }
        if (ob)
            *ob = node_ob;
    }
    return bret;
}

bool XMLController::handleExtendLabel(XMLNode node, View * parent_ob, View ** ob)
{
    if (!node.name() || !node.nameSize())
        return false;
    auto iter = modules_.find(node.name());
    if (iter == modules_.end())
        return false;
    auto & other_mod = iter->second;
    auto body = getBody(other_mod.doc);
    if (!body)
    {
        LOG_VERBOSE << "module: " << node.name() << "empty ????";
        return false;
    }
    View * node_ob = nullptr;
    bool bret = createViewFromNode(body.firstChild(), parent_ob, &node_ob);
    if (bret && node_ob)
    {//如果是扩展标签的话 node 应该是当前的node
        RefPtr<View> rv;
        rv.reset(node_ob);
        ob2node_[rv] = node;
        if (ob)
            *ob = node_ob;
    }
    return bret;
}

bool XMLController::handleRoot(XMLNode node, View * parent_ob, View ** ob)
{
    //root 不能添加到父上
    assert(!parent_ob);
    auto root = AdoptRef(new Root);

    ob2node_[root] = node;
    roots_.push_back(root);
    if (ob)
        *ob = root.get();
    return root != nullptr;
}

bool XMLController::handleText(XMLNode node, View * parent_ob, View ** ob)
{
    assert(parent_ob);
    auto v = AdoptRef(new Text);
    ob2node_[v] = node;

    if (parent_ob)
        ((View *)parent_ob)->attachChildToBack(v.get());
    if (ob)
        *ob = v.get();
    return v != nullptr;
}

bool XMLController::handleShape(XMLNode node, View * parent_ob, View ** ob)
{
    auto v = AdoptRef(new Shape);
    ob2node_[v] = node;
    if (parent_ob)
        ((View *)parent_ob)->attachChildToBack(v.get());
    if (ob)
        *ob = v.get();

    return v != nullptr;
}


bool XMLController::handleImage(XMLNode node, View * parent_ob, View ** ob)
{
    auto v = AdoptRef(new Image);
    ob2node_[v] = node;
    if (parent_ob)
        ((View *)parent_ob)->attachChildToBack(v.get());
    if (ob)
        *ob = v.get();
    return v != nullptr;
}

bool XMLController::handleArea(XMLNode node, View * parent_ob, View ** ob)
{
    auto v = AdoptRef(new Area);
    ob2node_[v] = node;
    if (parent_ob)
        ((View *)parent_ob)->attachChildToBack(v.get());
    if (ob)
        *ob = v.get();
    return v != nullptr;
}

bool XMLController::handleRichEdit(XMLNode node, View * parent_ob, View ** ob)
{
    auto v = AdoptRef(new RichEdit);
    ob2node_[v] = node;
    if (parent_ob)
        ((View *)parent_ob)->attachChildToBack(v.get());

    if (ob)
        *ob = v.get();
    return v != nullptr;
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
    file.resize(size + 1);
    stream.read(&file[0], size);
    file[size] = 0;
    return true;
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



Label LabelFromName(const char * name)
{
    if (!name)
        return kLABEL_UNKNOWN;
    if (!strcmp("bones", name))
        return kLABEL_BONES;
    else if (!strcmp("head", name))
        return kLABEL_HEAD;
    else if (!strcmp("link", name))
        return kLABEL_LINK;
    else if (!strcmp("style", name))
        return kLABEL_STYLE;
    else if (!strcmp("body", name))
        return kLABEL_BODY;
    else if (!strcmp("root", name))
        return kLABEL_ROOT;
    else if (!strcmp("area", name))
        return kLABEL_AREA;
    else if (!strcmp("richedit", name))
        return kLABEL_RICHEDIT;
    else if (!strcmp("image", name))
        return kLABEL_IMAGE;
    else if (!strcmp("text", name))
        return kLABEL_TEXT;
    else if (!strcmp("shape", name))
        return kLABEL_SHAPE;
    else
        return kLABEL_UNKNOWN;
}

}