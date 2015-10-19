#ifndef BONES_SCRIPT_PARSER_H_
#define BONES_SCRIPT_PARSER_H_

#include "bones_internal.h"
#include "core/xml_controller.h"
#include "simple_proxy.h"

namespace bones
{

class Shape;
class Image;
class Text;
class RichEdit;
class WebView;
class Scroller;

class ScriptParser : public BonesCore, 
                     public XMLController::Delegate
{
public:
    ScriptParser();

    ~ScriptParser();
public:
    void setXMLListener(BonesXMLListener * listener) override;

    bool loadXMLString(const char * data) override;

    bool loadXMLFile(const wchar_t * file) override;

    void cleanXML() override;

    BonesObject * getObject(const char * id) override;

    BonesObject * getObject(BonesObject * ob, const char * id) override;

    BonesObject * createObject(BonesObject * parent,
                               const char * label,
                               const char * id,
                               const char * class_name,
                               const char * group_id,
                               BonesObjectListener * listener) override;

    void cleanObject(BonesObject * bo) override;

    BonesResourceManager * getResourceManager() override;

    BonesPathProxy * getPathProxy() override;

    BonesShaderProxy * getShaderProxy() override;

    BonesPixmapProxy * getPixmapProxy() override;
    
    //xml controller 
    //XML刚载入时触发 返回false 自动clean
    bool onLoad() override;

    void onUnload() override;

    void onPrepare(View * v) override;
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

    void handleWebView(WebView * ob);

    void handleScroller(Scroller * ob);

    bool handleNotify(XMLNode node, View * parent_ob, View ** ob);

    bool handleEvent(XMLNode node, View * parent_ob, View ** ob);
private:
    BonesXMLListener * listener_;
    BonesObjectListener * ob_listener_;
    std::map<View *, BonesObject *>v2bo_;
    PathProxy path_proxy_;
    ShaderProxy shader_proxy_;
    PixmapProxy pixmap_proxy_;
    ResourceManager resource_manager_;
};

}


#endif