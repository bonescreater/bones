#include "xml_controller.h"
#include "encoding.h"
#include "helper.h"
#include "area.h"
#include "rich_edit.h"
#include "image.h"
#include "text.h"
#include "shape.h"

#include "panel.h"
#include "logging.h"
#include "panel_manager.h"
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
    kLABEL_PANEL,
    kLABEL_AREA,
    kLABEL_RICHEDIT,
    kLABEL_IMAGE,
    kLABEL_TEXT,
    kLABEL_SHAPE,
};

enum Bone
{
    kBONE_UNKNOWN,
    kBONE_PANEL,
    kBONE_AREA,
    kBONE_IMAGE,
    kBONE_TEXT,
    kBONE_SHAPE,
    kBONE_RICHEDIT,
};

const char * kStrType = "type";
const char * kStrModule = "module";
const char * kStrFile = "file";
const char * kStrCSS = "css";
const char * kStrXML = "xml";
const char * kStrID = "id";
const char * kStrClass = "class";
const char * kStrGroup = "group";


Label LabelFromName(const char * name);

Bone BoneFromName(const char * name);

void AttachToParentView(View * child, Ref * parent);

XMLController::XMLController()
{
    ;
}

XMLController::~XMLController()
{

}

bool XMLController::loadString(const char * data)
{
    size_t len = 0;
    if ( !data || !(len = strlen(data)) )
        return false;

    reset();
    main_module_.xml_file.resize(len + 1, 0);
    memcpy(&main_module_.xml_file[0], data, len);

    return loadMainModule();
}

bool XMLController::loadMainModule()
{
    auto & doc = main_module_.doc;

    if (!doc.parse(main_module_.xml_file.data()))
        return false;

    auto root = doc.root();

    if (!checkFormat(doc))
        return false;

    parseModuleHead(main_module_);
    parseModuleBody(main_module_);
    return true;
}

void XMLController::reset()
{
    main_module_.reset();
    modules_.clear();

    ob2id_.clear();
    auto panel = Core::GetPanelManager()->begin();
    while (panel)
    {
        panel->destroy();
        Core::GetPanelManager()->remove(panel);
        panel = Core::GetPanelManager()->next();
    }

    Core::GetCSSManager()->clean();
}

Ref * XMLController::getRefByID(const char * id)
{
    if (!id)
        return nullptr;
    //看是否是窗口ID
    RefPtr<Ref> rp;
    auto panel = Core::GetPanelManager()->begin();
    while (panel)
    {
        rp.reset(panel);
        auto ob = ob2id_.find(rp);
        if (ob != ob2id_.end())
        {
            if (ob->second == id)
                return panel;
        }
        panel = Core::GetPanelManager()->next();
    }

    //看是不是窗口下的view
    Ref * target = nullptr;
    panel = Core::GetPanelManager()->begin();
    while (panel)
    {
        target = getViewByID(panel->getRootView(), id);
        if (target)
            break;
        panel = Core::GetPanelManager()->next();
    }
    return target;
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
    while (node)
    {
        switch (LabelFromName(node.name()))
        {
        case kLABEL_STYLE:
            handleStyle(node, mod);
            break;
        case kLABEL_LINK:
            handleLink(node, mod);
            break;
        default:
            //不认识的标签 放过
            break;
        }
        node = node.nextSibling();
    }
}

bool XMLController::handleStyle(XMLNode node, Module & mod)
{
    if (node.valueSize() > 0 && node.value())
        Core::GetCSSManager()->append(node.value());
    return true;
}

/*
<link type = "css" module = "test"/>  link test.css
<link type = "xml" module = "test"/>  link test.xml
*/

bool XMLController::handleLink(XMLNode node, Module & mod)
{
    Attribute link[] = 
    { 
        { kStrType, nullptr }, { kStrModule, nullptr }, { kStrFile, nullptr } 
    };
    acquireAttrs(node, link, sizeof(link) / sizeof(link[0]));

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

        mod.css_files.push_back(FileStream());
        auto & file_stream = mod.css_files.back();
        std::string path;
        if (isAbsolutePath(file))
            path = file;
        else
        {
            auto dir = getPathFromFullName(mod.xml_fullname.data());
            const char * parts[] = { dir.data(), file };
            path = joinPath(parts, sizeof(parts) / sizeof(parts[0]));
        }
        if (readFile(path.data(), file_stream))
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
        if (isAbsolutePath(file))
            sub_mod.xml_fullname = file;
        else
        {            
            auto path = getPathFromFullName(mod.xml_fullname.data());
            const char * parts[] = { path.data(), file };
            sub_mod.xml_fullname = joinPath(parts, sizeof(parts) / sizeof(parts[0]));
        }

        //读取xml文件内容
        if (!readFile(sub_mod.xml_fullname.data(), sub_mod.xml_file))
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

void XMLController::parseModuleBody(const Module & mod)
{
    auto body = getBody(mod.doc);
    if (!body)
        return;
    auto node = body.firstChild();
    while (node)
    {//遍历node 所有的兄弟
        //迭代每个node
        createRefFromNode(node, nullptr, mod, nullptr);
        node = node.nextSibling();
    }
}

bool XMLController::createRefFromNode(XMLNode node, Ref * parent_ob, const Module & mod, Ref ** ob)
{
    if (!node)
        return false;

    bool bret = false;
    Ref * node_ob = nullptr;
    bool extend_node = false;
    switch (LabelFromName(node.name()))
    {
    case kLABEL_PANEL:
        //创建窗口或者rootview
        bret = handlePanel(node, parent_ob, mod, &node_ob);
        break;
    case kLABEL_AREA:
        bret = handleArea(node, parent_ob, mod, &node_ob);
        break;
    case kLABEL_RICHEDIT:
        bret = handleRichEdit(node, parent_ob, mod, &node_ob);
        break;
    case kLABEL_IMAGE:
        bret = handleImage(node, parent_ob, mod, &node_ob);
        break;
    case kLABEL_TEXT:
        bret = handleText(node, parent_ob, mod, &node_ob);
        break;
    case kLABEL_SHAPE:
        bret = handleShape(node, parent_ob, mod, &node_ob);
        break;
    default:
        extend_node = true;
        bret = handleExtendLabel(node, parent_ob, mod, &node_ob);
        break;
    }
    if (bret)
    {
        auto child = node.firstChild();
        while (child)
        {
            createRefFromNode(child, node_ob, mod, nullptr);
            child = child.nextSibling();
        }
        if (!extend_node)
            handleNodeOnPrepare(node, node_ob);
        if (ob)
            *ob = node_ob;
    }
    return bret;
}

void XMLController::handleNodeOnPrepare(XMLNode node, Ref * ob)
{
    ;
}

bool XMLController::handleExtendLabel(XMLNode node, Ref * parent_ob, const Module & mod, Ref ** ob)
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
    Ref * node_ob = nullptr;
    bool bret = createRefFromNode(body.firstChild(), parent_ob, other_mod, &node_ob);
    if (bret && node_ob)
    {
        Attribute attrs[] =
        {
            { kStrClass, nullptr }, { kStrID, nullptr }, { kStrGroup, nullptr }
        };
        acquireAttrs(node, attrs, sizeof(attrs) / sizeof(attrs[0]));
        applyClass(node_ob, attrs[0].value);
        applyID(node_ob, attrs[1].value);
        if (ob)
            *ob = node_ob;
    }
    return bret;
}

bool XMLController::handlePanel(XMLNode node, Ref * parent_ob, const Module & mod, Ref ** ob)
{//目前只支持创建窗口
    auto sheet = AdoptRef(new Panel);
    

    //窗口的父只能是窗口
    Bone bone = kBONE_PANEL;
    Panel * parent_panel = nullptr;
    if (parent_ob)
    {
        if(kBONE_PANEL == BoneFromName(parent_ob->getClassName()))
            parent_panel = static_cast<Panel *>(parent_ob);
    }
    sheet->create(parent_panel);
    Core::GetPanelManager()->add(sheet.get());
    sheet->show(true);
    
    //显示在桌面中间
    if (!parent_panel)
    {//如果是顶级窗口 将窗口默认设置为 桌面的一半 而且 居中
        Rect work = Helper::GetPrimaryWorkArea();
        Rect sc;
        sheet->setSize(Size::Make(work.width() / 2, work.height() / 2));
        sheet->getClientRect(sc);
        sheet->setLoc(Point::Make(work.width() / 4, work.height() / 4));
    }


    //保存窗口
    Attribute attrs[] =
    {
        { kStrClass, nullptr }, { kStrID, nullptr }, { kStrGroup, nullptr }
    };
    acquireAttrs(node, attrs, sizeof(attrs) / sizeof(attrs[0]));
    applyClass(sheet.get(), attrs[0].value);
    applyID(sheet.get(), attrs[1].value);

    if (ob)
        *ob = sheet.get();
    return sheet != nullptr;
}

bool XMLController::handleText(XMLNode node, Ref * parent_ob, const Module & mod, Ref ** ob)
{
    auto v = AdoptRef(new Text);
    Attribute attrs[] =
    {
        { kStrClass, nullptr }, { kStrID, nullptr }, { kStrGroup, nullptr }
    };
    acquireAttrs(node, attrs, sizeof(attrs) / sizeof(attrs[0]));
    applyClass(v.get(), attrs[0].value);
    applyID(v.get(), attrs[1].value);

    AttachToParentView(v.get(), parent_ob);
    if (ob)
        *ob = v.get();
    return v != nullptr;
}

bool XMLController::handleShape(XMLNode node, Ref * parent_ob, const Module & mod, Ref ** ob)
{
    auto v = AdoptRef(new Shape);
    Attribute attrs[] =
    {
        { kStrClass, nullptr }, { kStrID, nullptr }, { kStrGroup, nullptr }
    };
    acquireAttrs(node, attrs, sizeof(attrs) / sizeof(attrs[0]));
    applyClass(v.get(), attrs[0].value);
    applyID(v.get(), attrs[1].value);
    AttachToParentView(v.get(), parent_ob);
    if (ob)
        *ob = v.get();

    return v != nullptr;
}


bool XMLController::handleImage(XMLNode node, Ref * parent_ob, const Module & mod, Ref ** ob)
{
    auto block = AdoptRef(new Image);
    Attribute attrs[] =
    {
        { kStrClass, nullptr }, { kStrID, nullptr }, { kStrGroup, nullptr }
    };
    acquireAttrs(node, attrs, sizeof(attrs) / sizeof(attrs[0]));
    applyClass(block.get(), attrs[0].value);
    applyID(block.get(), attrs[1].value);

    AttachToParentView(block.get(), parent_ob);
    if (ob)
        *ob = block.get();
    return block != nullptr;
}

bool XMLController::handleArea(XMLNode node, Ref * parent_ob, const Module & mod, Ref ** ob)
{
    auto area = AdoptRef(new Area);
    Attribute attrs[] =
    {
        { kStrClass, nullptr }, { kStrID, nullptr }, { kStrGroup, nullptr }
    };
    acquireAttrs(node, attrs, sizeof(attrs) / sizeof(attrs[0]));
    applyClass(area.get(), attrs[0].value);
    applyID(area.get(), attrs[1].value);
    AttachToParentView(area.get(), parent_ob);
    if (ob)
        *ob = area.get();
    return area != nullptr;
}

bool XMLController::handleRichEdit(XMLNode node, Ref * parent_ob, const Module & mod, Ref ** ob)
{
    auto rich = AdoptRef(new RichEdit);

    //rich edit 需要handle 所以 必须先attach后再设置属性
    AttachToParentView(rich.get(), parent_ob);
    Attribute attrs[] =
    {
        { kStrClass, nullptr }, { kStrID, nullptr }, { kStrGroup, nullptr }
    };
    acquireAttrs(node, attrs, sizeof(attrs) / sizeof(attrs[0]));
    applyClass(rich.get(), attrs[0].value);
    applyID(rich.get(), attrs[1].value);

    rich->setText(L"This is a 测RichEdit");
    rich->setOpacity(1.0f);
    Color bg_color = 0xffffff00;
    rich->setBackground(false, &bg_color);
    if (ob)
        *ob = rich.get();
    return rich != nullptr;
}

bool XMLController::readFile(const char * file_path, FileStream & file)
{
    using namespace std;
    if (!file_path)
        return false;
    auto wfile_path = Encoding::FromUTF8(file_path);
    return readFile(wfile_path.data(), file);
}

bool XMLController::readFile(const wchar_t * file_path, FileStream & file)
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

void XMLController::applyClass(Ref *ob, const char * class_name)
{
    if (!class_name || !ob)
        return;
    Core::GetCSSManager()->applyClass(ob, class_name);
}

void XMLController::applyID(Ref * ob, const char * id_name)
{
    if (!ob)
        return;
    RefPtr<Ref> rp;
    rp.reset(ob);
    auto & id = ob2id_[rp];
    if (id_name)
        id = id_name;
}

View * XMLController::getViewByID(View * parent, const char * id)
{
    if (!id || !parent)
        return nullptr;
    RefPtr<View> pa;
    pa.reset(parent);
    auto iter = ob2id_.find(pa);
    if (iter != ob2id_.end()) //root view 存放在panel中 所以 rootview 在ob2id中找不到
    {
        if (iter->second == id)
            return parent;
    }


    //自己的id不对 就看看子的id
    View * target = nullptr;
    View * child = parent->getFirstChild();
    while (child)
    {
        target = getViewByID(child, id);
        if (target)
            break;
        child = child->getNextSibling();
    }
    return target;
}

void XMLController::acquireAttrs(XMLNode node, Attribute * attrs, int count)
{
    if (!attrs || !count)
        return;
    for (auto i = 0; i < count; ++i)
        attrs[i].value = nullptr;

    if (!node)
        return;

    auto node_attr = node.firstAttribute();
    while (node_attr)
    {
        char * name = node_attr.name();
        for (auto i = 0; i < count; ++i)
        {
            if (strcmp(attrs[i].key, name))
                continue;

            attrs[i].value = node_attr.value();
            break;
        }
        node_attr = node_attr.nextSibling();
    }
}

std::string XMLController::getPathFromFullName(const char * fullname)
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

std::string XMLController::joinPath(const char ** path, int count)
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

bool XMLController::isAbsolutePath(const char * path)
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
    else if (!strcmp("panel", name))
        return kLABEL_PANEL;
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

Bone BoneFromName(const char * name)
{
    if (!name)
        return kBONE_UNKNOWN;
    if (!strcmp(kClassPanel, name))
        return kBONE_PANEL;
    else if (!strcmp(kClassArea, name))
        return kBONE_AREA;
    else if (!strcmp(kClassRichEdit, name))
        return kBONE_RICHEDIT;
    else if (!strcmp(kClassImage, name))
        return kBONE_IMAGE;
    else if (!strcmp(kClassText, name))
        return kBONE_TEXT;
    else if (!strcmp(kClassShape, name))
        return kBONE_TEXT;
    assert(0);
    return kBONE_UNKNOWN;
}

void AttachToParentView(View * child, Ref * parent)
{
    if (!child || !parent)
        return;
    auto bone = BoneFromName(parent->getClassName());
    View * pv = nullptr;
    switch (bone)
    {
    case kBONE_PANEL:
        parent = static_cast<Panel *>(parent)->getRootView();
    case kBONE_AREA:
    case kBONE_IMAGE:
    case kBONE_TEXT:
    case kBONE_SHAPE:
        pv = static_cast<View *>(parent);
        break;
    default:
        assert(0);
        break;
    }
    if (pv)
        pv->attachChildToBack(child);
}

}