#ifndef BONES_SCRIPT_PARSER_H_
#define BONES_SCRIPT_PARSER_H_

#include "bones.h"
#include "core/xml_controller.h"
#include "lua_animation.h"


namespace bones
{

class Ref;
class Shape;
class Image;
class Text;
class Area;
class RichEdit;

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

    BonesObject * createObject(BonesObject * parent,
                               const char * label,
                               const char * id,
                               const char * group_id,
                               const char * class_name) override;

    void cleanObject(BonesObject * bo, bool recursive) override;

    BonesResManager * getResManager() override;
    //res manager
    void add(const char * key, BonesPixmap & pm) override;

    void add(const char * key, BonesCursor cursor) override;

    void clean() override;

    
    //xml controller 
    //XML刚载入时触发 返回false 自动clean
    bool onLoad() override;

    void onUnload() override;
    //节点初始化完毕触发 此时禁止clean
    void onPrepare(View * v) override;

    bool preprocessHead(XMLNode node, const char * label, const char * full_path) override;

    void postprocessHead(XMLNode node, const char * label, const char * full_path) override;

    bool preprocessBody(XMLNode node, const char * label, View * parent_ob, View ** ob) override;

    void postprocessBody(XMLNode node, const char * label, View * parent_ob, View * ob) override;

    BonesObject * getObject(View *);

    View * getObject(BonesObject *);

    void listen(BonesObject * bo, const char * name, BonesScriptListener * listener);

    void push(BonesScriptArg * arg);

    BonesAnimation * createAnimate(
        BonesObject * target,
        uint64_t interval, uint64_t due,
        BonesAnimation::RunListener * run,
        BonesAnimation::ActionListener * stop,
        BonesAnimation::ActionListener * start,
        BonesAnimation::ActionListener * pause,
        BonesAnimation::ActionListener * resume,
        AnimationType type);

    void startAnimate(BonesAnimation * ani);

    void stopAnimate(BonesAnimation * ani, bool toend);

    void pauseAnimate(BonesAnimation * ani);

    void resumeAnimate(BonesAnimation * ani);

    void stopAllAnimate(BonesObject * ani, bool toend);
private:
    void handleRoot(Root * ob);

    void handleShape(Shape * ob);

    void handleImage(Image * ob);

    void handleText(Text * ob);

    void handleRichEdit(RichEdit * ob);

    void handleArea(Area * ob);

    bool handleNotify(XMLNode node, View * parent_ob, View ** ob);

    bool handleEvent(XMLNode node, View * parent_ob, View ** ob);
private:
    XMLController xml_;
    BonesXMLListener * listener_;
    BonesXMLListener * last_listener_;
    std::map<View *, BonesObject *>v2bo_;
};

}


#endif