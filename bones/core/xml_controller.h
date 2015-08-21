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
class Root;

class XMLController
{
public:
    class Delegate
    { 
    public:
        //XML刚载入时触发 返回false 自动clean
        virtual bool onLoad() = 0;
        //节点初始化完毕触发 此时禁止clean
        virtual void onPrepare(View * v, XMLNode node) = 0;

        virtual bool preprocessHead(XMLNode node, const char * full_path) = 0;

        virtual void postprocessHead(XMLNode node, const char * full_path) = 0;

        virtual bool preprocessBody(XMLNode node, View * parent_ob, View ** ob) = 0;

        virtual void postprocessBody(XMLNode node, View * parent_ob, View * ob) = 0;
    };
public:
    typedef std::vector<char> FileStream;
    //xml 文件和CSS文件常驻内存
    struct Module
    {
        FileStream xml_file;//xml文件内容
        std::string xml_fullname;
        XMLDocument doc;
    };
public:
    static std::string GetPathFromFullName(const char * fullname);

    static std::string JoinPath(const char ** path, int count);

    static bool IsAbsolutePath(const char * path);

    static bool ReadFile(const char * file_path, FileStream & file);

    static bool ReadFile(const wchar_t * file_path, FileStream & file);
public:
    XMLController();

    virtual ~XMLController();

    void setDelegate(Delegate * delegate);
    //zero-terminated XML string
    bool loadString(const char * data);

    void clean();

    View * getViewByID(const char * id);

    View * getViewByID(View * ob, const char * id);
protected:
    virtual bool handleStyle(XMLNode node, const char * full_path);

    virtual bool handleLink(XMLNode node, const char * full_path);

    virtual bool handleRoot(XMLNode node, View * parent_ob, View ** ob);

    virtual bool handleArea(XMLNode node, View * parent_ob, View ** ob);

    virtual bool handleRichEdit(XMLNode node, View * parent_ob, View ** ob);

    virtual bool handleImage(XMLNode node, View * parent_ob, View ** ob);

    virtual bool handleText(XMLNode node, View * parent_ob, View ** ob);

    virtual bool handleShape(XMLNode node, View * parent_ob, View ** ob);

    virtual bool handleExtendLabel(XMLNode node, View * parent_ob, View ** ob);
protected:
    //返回指定节点树 根节点的对象
    bool createViewFromNode(XMLNode node, View * parent_ob, View ** ob);
private:
    bool loadMainModule(Module & mod);

    void parseModuleHead(Module & mod);

    void parseModuleBody(const Module & mod);

    bool checkFormat(XMLDocument & doc);

    bool checkRoot(XMLNode node);

    bool checkHead(XMLNode node);

    bool checkBody(XMLNode node);

    XMLNode getBody(const XMLDocument & doc);

    XMLNode getHead(const XMLDocument & doc);

    void notifyPrepare();

    void notifyPrepare(View * ob, XMLNode node);

    void applyClass();

    void applyClass(View * ob);
private:
    //delegate
    Delegate * delegate_;
    //存放所有的view 
    std::map<RefPtr<View>, XMLNode> ob2node_;
    std::vector<RefPtr<Root>> roots_;
    //模块的描述
    std::vector<Module> main_modules_;
    std::map<std::string, Module> modules_;
    std::vector<FileStream> css_files_;//多个css文件内容
};

//字符串常量
extern const char * kStrType;
extern const char * kStrModule;
extern const char * kStrCSS;
extern const char * kStrLink;
extern const char * kStrXML;
extern const char * kStrID;
extern const char * kStrClass;
extern const char * kStrGroup;
extern const char * kStrFile;

}

#endif