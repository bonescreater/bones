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
    //rapid xml 删除节点时 不释放内存
    //因此使用Node结构来描述 方便动态创建标签时释放内存
    typedef std::map<std::string, std::string> NodeAttrs;

    class Delegate
    { 
    public:
        virtual void onCreating(View * v) = 0;

        virtual void onDestroying(View * v) = 0;
        //XML刚载入时触发 返回false 自动clean
        virtual bool onLoad() = 0;

        virtual void onUnload() = 0;

        virtual void onPrepare(View * v) = 0;
        //节点初始化完毕触发 此时禁止clean
        virtual void onCreate(View * v) = 0;

        virtual void onDestroy(View * v) = 0;
        //xml解析过程中调用
        virtual bool preprocessHead(XMLNode node, const char * label, const char * full_path) = 0;

        virtual void postprocessHead(XMLNode node, const char * label, const char * full_path) = 0;

        virtual bool preprocessBody(XMLNode node, const char * label, View * parent_ob, View ** ob) = 0;

        virtual void postprocessBody(XMLNode node, const char * label, View * parent_ob, View * ob) = 0;
    };
public:
    typedef std::vector<char> FileStream;
    //xml 文件和CSS文件常驻内存
    struct Module
    {
        void clean()
        {
            xml_file.clear();
            xml_fullname.clear();
        }
        FileStream xml_file;//xml文件内容
        std::string xml_fullname;
        XMLDocument doc;
    };

public:
    static bool ReadFile(const char * file_path, FileStream & file);
    //确保0结尾
    static bool ReadString(const char * file_path, FileStream & file);

    static std::string GetRealPath(const char * file_value, const char * module_path);
public:
    XMLController();

    virtual ~XMLController();

    void setDelegate(Delegate * delegate);
    //zero-terminated XML string
    bool loadString(const char * data);

    bool loadFile(const wchar_t * file);

    void clean();

    View * getViewByID(const char * id);

    View * getViewByID(View * ob, const char * id);

    View * createView(View * parent,
                      const char * label,
                      const char * id,
                      const char * group_id,
                      const char * class_name);

    void clean(View * v);

    const char * getID(View * v);
protected:
    bool handleStyle(XMLNode node, const char * full_path);

    bool handleLink(XMLNode node, const char * full_path);

    bool handleRoot(XMLNode node, View * parent_ob, View ** ob);

    bool handleRichEdit(XMLNode node, View * parent_ob, View ** ob);

    bool handleWebView(XMLNode node, View * parent_ob, View ** ob);

    bool handleImage(XMLNode node, View * parent_ob, View ** ob);

    bool handleText(XMLNode node, View * parent_ob, View ** ob);

    bool handleInput(XMLNode node, View * parent_ob, View ** ob);

    bool handleShape(XMLNode node, View * parent_ob, View ** ob);

    bool handleScroller(XMLNode node, View * parent_ob, View ** ob);

    bool handleExtendLabel(XMLNode node, const char * label, View * parent_ob, View ** ob);
protected:
    //返回指定节点树 根节点的对象
    bool createViewFromNode(XMLNode node, const char * label, View * parent_ob, View ** ob);
private:
    void recursiveClear(View * v);

    void clear();

    bool loadMainModule(Module & mod);

    void parseModuleHead(Module & mod);

    void parseModuleBody(const Module & mod);

    bool checkFormat(XMLDocument & doc);

    bool checkBones(XMLNode node);

    bool checkHead(XMLNode node);

    bool checkBody(XMLNode node);

    XMLNode getBody(const XMLDocument & doc);

    XMLNode getHead(const XMLDocument & doc);

    void notifyCreate();

    void notifyCreate(View * ob);

    void notifyDestroy();

    void notifyDestroy(View * ob);

    void applyClass();

    void applyClass(View * ob);

    void notifyPrepare();

    void notifyPrepare(View * ob);
private:
    static std::string GetPathFromFullName(const char * fullname);

    static bool IsAbsolutePath(const char * path);

    static bool ReadFile(const wchar_t * file_path, FileStream & file);

    const char * acquire(NodeAttrs & n, const char * attr_name);

    void saveNode(View * v, XMLNode node);
private:
    //delegate
    Delegate * delegate_;
    //存放所有的view 
    std::map<RefPtr<View>, NodeAttrs> ob2node_;
    //模块的描述
    Module main_module_;
    std::map<std::string, Module> modules_;
    std::vector<FileStream> css_files_;//多个css文件内容
};

//字符串常量
extern const char * kStrType;
extern const char * kStrModule;
extern const char * kStrName;
extern const char * kStrCSS;
extern const char * kStrXML;
extern const char * kStrID;
extern const char * kStrClass;
extern const char * kStrGroup;
extern const char * kStrFile;
extern const char * kStrPixmap;
extern const char * kStrBones;
extern const char * kStrHead;
extern const char * kStrBody;
extern const char * kStrStyle;
extern const char * kStrLink;
}

#endif