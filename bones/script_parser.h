#ifndef BONES_SCRIPT_PARSER_H_
#define BONES_SCRIPT_PARSER_H_

#include "bones.h"
#include "core/xml_controller.h"


namespace bones
{

class Ref;

class ScriptParser : public XMLController
{
public:
    static ScriptParser * Get();
public:
    void regScriptCallback(Ref * ob, const char * event_name, ScriptCallBack cb, void * userdata);

    void unregScriptCallback(Ref * ob, const char * event_name);

    void clean();
protected:
    ScriptParser();

    bool handleLink(XMLNode node, Module & mod) override;

    bool handlePanel(XMLNode node, Ref * parent_ob, const Module & mod, Ref ** ob) override;

    bool handleArea(XMLNode node, Ref * parent_ob, const Module & mod, Ref ** ob) override;

    bool handleRichEdit(XMLNode node, Ref * parent_ob, const Module & mod, Ref ** ob) override;

    bool handleImage(XMLNode node, Ref * parent_ob, const Module & mod, Ref ** ob) override;

    bool handleText(XMLNode node, Ref * parent_ob, const Module & mod, Ref ** ob) override;

    bool handleShape(XMLNode node, Ref * parent_ob, const Module & mod, Ref ** ob) override;

    bool handleExtendLabel(XMLNode node, Ref * parent_ob, const Module & mod, Ref ** ob) override;

    void handleNodeOnPrepare(XMLNode node, Ref * ob) override;
    //处理event标签
    bool handleELEvent(XMLNode node, Ref * parent_ob, const Module & info, Ref ** ob);
};

}


#endif