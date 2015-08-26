#ifndef BONES_SCRIPT_PARSER_H_
#define BONES_SCRIPT_PARSER_H_

#include "bones.h"
#include "core/xml_controller.h"



namespace bones
{

class Ref;
class Shape;
class Image;
class Text;
class Area;

class ScriptParser : public BonesCore, 
                     public BonesResManager,
                     public XMLController::Delegate
{
public:
    ScriptParser();
public:
    bool loadXMLString(const char * data, BonesXMLListener * listener) override;

    void cleanXML() override;

    BonesPixmap * create() override;

    BonesObject * getObject(const char * id) override;

    BonesObject * getObject(BonesObject * ob, const char * id) override;

    BonesResManager * getResManager() override;
    //res manager
    void add(const char * key, BonesPixmap & pm) override;

    void add(const char * key, BonesCursor cursor) override;

    void clean() override;

    
    //xml controller 
    //XML刚载入时触发 返回false 自动clean
    bool onLoad() override;
    //节点初始化完毕触发 此时禁止clean
    void onPrepare(View * v, XMLNode node) override;

    bool preprocessHead(XMLNode node, const char * full_path) override;

    void postprocessHead(XMLNode node, const char * full_path) override;

    bool preprocessBody(XMLNode node, View * parent_ob, View ** ob) override;

    void postprocessBody(XMLNode node, View * parent_ob, View * ob) override;

    BonesObject * getObject(View *);

    void listen(BonesObject * bo, const char * name, BonesScriptListener * listener);

    void push(BonesScriptArg * arg);
private:
    void handleRoot(Root * ob);

    void handleShape(Shape * ob);

    void handleImage(Image * ob);

    void handleText(Text * ob);

    void handleRichEdit(Text * ob);

    void handleArea(Area * ob);

    bool handleNotify(XMLNode node, View * parent_ob, View ** ob);
private:
    XMLController xml_;
    BonesXMLListener * listener_;
    std::map<View *, BonesObject *>v2bo_;
};

}


#endif