#ifndef BONES_SCRIPT_PARSER_H_
#define BONES_SCRIPT_PARSER_H_

#include "bones_internal.h"
#include "core/xml_controller.h"


namespace bones
{

class Ref;
class Shape;
class Image;
class Text;
class Area;
class RichEdit;
class WebView;
class Scroller;

class ScriptParser : public BonesCore, 
                     public BonesResManager,
                     public XMLController::Delegate
{
public:
    ScriptParser();

    ~ScriptParser();
public:
    bool loadXMLString(const char * data, BonesXMLListener * listener) override;

    void cleanXML() override;

    BonesPixmap * createPixmap() override;

    void destroyPixmap(BonesPixmap *) override;

    BonesObject * getObject(const char * id) override;

    BonesObject * getObject(BonesObject * ob, const char * id) override;

    BonesRoot * createRoot(const char * id,
                             const char * group_id,
                             const char * class_name) override;

    BonesObject * createObject(BonesObject * parent,
                               const char * label,
                               const char * id,
                               const char * group_id,
                               const char * class_name) override;

    void cleanObject(BonesObject * bo) override;

    BonesResManager * getResManager() override;
    //res manager
    void clonePixmap(const char * key, BonesPixmap & pm) override;

    void cloneCursor(const char * key, BonesCursor cursor) override;

    void getPixmap(const char * key, BonesPixmap & pm) override;

    void getCursor(const char * key, BonesCursor & cursor) override;

    void clean() override;

    
    //xml controller 
    //XML刚载入时触发 返回false 自动clean
    bool onLoad() override;

    void onUnload() override;
    //节点初始化完毕触发 此时禁止clean
    void onCreate(View * v) override;

    void onDestroy(View * v) override;

    void onCreating(View * v) override;

    void onDestroying(View * v) override;

    bool preprocessHead(XMLNode node, const char * label, const char * full_path) override;

    void postprocessHead(XMLNode node, const char * label, const char * full_path) override;

    bool preprocessBody(XMLNode node, const char * label, View * parent_ob, View ** ob) override;

    void postprocessBody(XMLNode node, const char * label, View * parent_ob, View * ob) override;

    BonesObject * getObject(View *);

    View * getObject(BonesObject *);

    void listen(BonesObject * bo, const char * name, BonesScriptListener * listener);

    void push(BonesScriptArg * arg);

    BonesObject::Animation createAnimate(
        BonesObject * target,
        uint64_t interval, uint64_t due,
        BonesObject::AnimationRunListener * run,
        BonesObject::AnimationActionListener * stop,
        BonesObject::AnimationActionListener * start,
        BonesObject::AnimationActionListener * pause,
        BonesObject::AnimationActionListener * resume,
        AnimationType type);

    void startAnimate(BonesObject::Animation ani);

    void stopAnimate(BonesObject::Animation ani, bool toend);

    void pauseAnimate(BonesObject::Animation ani);

    void resumeAnimate(BonesObject::Animation ani);

    void stopAllAnimate(BonesObject * ani, bool toend);

    const char * getID(BonesObject * bob);
private:
    void handleRoot(Root * ob);

    void handleShape(Shape * ob);

    void handleImage(Image * ob);

    void handleText(Text * ob);

    void handleRichEdit(RichEdit * ob);

    void handleArea(Area * ob);

    void handleWebView(WebView * ob);

    void handleScroller(Scroller * ob);

    bool handleNotify(XMLNode node, View * parent_ob, View ** ob);

    bool handleEvent(XMLNode node, View * parent_ob, View ** ob);
private:
    BonesXMLListener * listener_;
    std::map<View *, BonesObject *>v2bo_;
};

}


#endif