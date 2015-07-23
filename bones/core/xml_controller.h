#ifndef BONES_CORE_XML_CONTROLLER_H_
#define BONES_CORE_XML_CONTROLLER_H_

#include "core.h"
#include "simple_css.h"
#include "class_descriptor.h"
#include "ref.h"

namespace rapidxml
{
    template <class ch> class xml_node;
    template <class ch> class xml_attribute;
    template <class ch> class xml_document;
}


namespace bones
{
class Ref;
class View;
class Panel;

class XMLAttribute
{
public:
    XMLAttribute(rapidxml::xml_attribute<char> * attr);

    XMLAttribute nextSibling()  const;

    XMLAttribute prevSibling()  const;

    char * name() const;

    size_t nameSize() const;

    char * value() const;

    size_t valueSize() const;

    operator bool()  const;
private:
    rapidxml::xml_attribute<char> * attr_;
};

class XMLNode
{
public:
    XMLNode(rapidxml::xml_node<char> * n);

    XMLNode firstChild() const;

    XMLNode nextSibling() const;

    XMLNode prevSibling() const;

    XMLAttribute firstAttribute() const;

    char * name() const;

    size_t nameSize() const;

    char * value() const;

    size_t valueSize() const;

    operator bool()  const;
private:
    rapidxml::xml_node<char> * node_;
};

class XMLDocument
{
public:
    XMLDocument();

    ~XMLDocument();

    bool parse(char * str);

    XMLNode root() const;
private:
    rapidxml::xml_document<char> * document_;
};

class XMLController
{
public:
    typedef std::vector<char> FileStream;
    //xml 文件和CSS文件常驻内存
    struct Module
    {
        void reset()
        {
            xml_file.clear();
            css_files.clear();
            css.reset();
        }
        FileStream xml_file;//xml文件内容
        std::string xml_fullname;
        std::vector<FileStream> css_files;//多个css文件内容
        XMLDocument doc;
        SimpleCSS css;//解析出的CSS结果放在这里
    };
    typedef struct
    {
        const char * key;
        const char * value;
    } Attribute;
public:
    XMLController();

    virtual ~XMLController();
    //zero-terminated XML string
    bool loadString(const char * data);

    void applyCSS(Ref * ob, const char * css);

    void applyClass(Ref * ob, const char * class_name, const char * mod_name);

    Ref * getRefByID(const char * id);
protected:
    virtual bool handleStyle(XMLNode node, Module & mod);

    virtual bool handleLink(XMLNode node, Module & mod);

    virtual bool handlePanel(XMLNode node, Ref * parent_ob, const Module & mod, Ref ** ob);

    virtual bool handleArea(XMLNode node, Ref * parent_ob, const Module & mod, Ref ** ob);

    virtual bool handleImage(XMLNode node, Ref * parent_ob, const Module & mod, Ref ** ob);

    virtual bool handleText(XMLNode node, Ref * parent_ob, const Module & mod, Ref ** ob);

    virtual bool handleShape(XMLNode node, Ref * parent_ob, const Module & mod, Ref ** ob);

    virtual bool handleExtendLabel(XMLNode node, Ref * parent_ob, const Module & mod, Ref ** ob);

    virtual void handleNodeOnPrepare(XMLNode node, Ref * ob);
protected:
    void reset();

    //返回指定节点树 根节点的对象
    bool createRefFromNode(XMLNode node, Ref * parent_ob, const Module & mod, Ref ** ob);

    void acquireAttrs(XMLNode node, Attribute * attrs, int count);

    std::string getPathFromFullName(const char * fullname);

    std::string joinPath(const char ** path, int count);

    bool isAbsolutePath(const char * path);

    bool readFile(const char * file_path, FileStream & file);

    bool readFile(const wchar_t * file_path, FileStream & file);
private:
    bool loadMainModule();

    void parseModuleHead(Module & mod);

    void parseModuleBody(const Module & mod);

    void applyClass(Ref *ob, const Module & mod, const char * class_name);

    void applyClass(Ref * ob, const SimpleCSS & css, const CSSString & class_name);

    void applyID(Ref * ob, const char * id_name);

    void applyEntries(Ref * ob, const CSSEntries & entries);

    void applyEntry(Ref * ob, const CSSEntry & entry);

    bool checkFormat(XMLDocument & doc);

    bool checkRoot(XMLNode node);

    bool checkHead(XMLNode node);

    bool checkBody(XMLNode node);

    XMLNode getBody(const XMLDocument & doc);

    XMLNode getHead(const XMLDocument & doc);

    View * getViewByID(View * parent, const char * id);
private:
    ClassDescriptor descriptor_;
    //窗口管理器存放所有的窗口
    std::vector<Panel *> panels_;
    //存放所有的view
    std::map<RefPtr<Ref>, std::string> ob2id_;

    //模块的描述
    Module main_module_;
    std::map<std::string, Module> modules_;
};

}

#endif