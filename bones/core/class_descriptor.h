#ifndef BONES_CORE_CLASS_DESCRIPTOR_H_
#define BONES_CORE_CLASS_DESCRIPTOR_H_

#include "core.h"
#include "xml_types.h"


namespace bones
{

class Object;

class ClassDescriptor
{
    
public:
    ClassDescriptor();

    ~ClassDescriptor();

    CSSClassTable * getFunc(const char * class_name);
private:
    void registerView();

    void registerArea();

    void registerRichEdit();

    void registerImage();

    void registerText();

    void registerShape();

    void registerPanel();

    void registerView(CSSClassTable & table);

    void registerShirt(CSSClassTable & table);

    void registerWidget(CSSClassTable & table);
private:
    std::map<const char *, CSSClassTable> multi_class_tables_;
};

}



#endif