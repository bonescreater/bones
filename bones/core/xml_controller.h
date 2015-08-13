#ifndef BONES_CORE_XML_CONTROLLER_H_
#define BONES_CORE_XML_CONTROLLER_H_

#include "core.h"
#include "ref.h"
#include "xml.h"
#include <vector>
#include <map>



namespace bones
{
class View;
class Panel;

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
        }
        FileStream xml_file;//xml文件内容
        std::string xml_fullname;
        std::vector<FileStream> css_files;//多个css文件内容
        XMLDocument doc;
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

    Ref * getRefByID(const char * id);
protected:
    virtual bool handleStyle(XMLNode node, Module & mod);

    virtual bool handleLink(XMLNode node, Module & mod);

    virtual bool handlePanel(XMLNode node, Ref * parent_ob, const Module & mod, Ref ** ob);

    virtual bool handleArea(XMLNode node, Ref * parent_ob, const Module & mod, Ref ** ob);

    virtual bool handleRichEdit(XMLNode node, Ref * parent_ob, const Module & mod, Ref ** ob);

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

    void applyClass(Ref *ob, const char * class_name);

    void applyID(Ref * ob, const char * id_name);

    bool checkFormat(XMLDocument & doc);

    bool checkRoot(XMLNode node);

    bool checkHead(XMLNode node);

    bool checkBody(XMLNode node);

    XMLNode getBody(const XMLDocument & doc);

    XMLNode getHead(const XMLDocument & doc);

    View * getViewByID(View * parent, const char * id);
private:
    //存放所有的view
    std::map<RefPtr<Ref>, std::string> ob2id_;

    //模块的描述
    Module main_module_;
    std::map<std::string, Module> modules_;
};

//字符串常量
extern const char * kStrType;
extern const char * kStrModule;
extern const char * kStrCSS;
extern const char * kStrXML;
extern const char * kStrID;
extern const char * kStrClass;
extern const char * kStrGroup;
extern const char * kStrFile;

}

#endif