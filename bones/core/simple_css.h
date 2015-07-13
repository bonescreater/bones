#ifndef BONES_CORE_SIMPLE_CSS_H_
#define BONES_CORE_SIMPLE_CSS_H_

#include "core.h"
#include "xml_types.h"

namespace bones
{
/*类似css 目前仅支持
.class
{
a:b;
b:c d e;
m:n;
}
注释暂时不支持 有空再加
*/
class SimpleCSS
{
public:
    //{css:mmm;}
    static void Parse(const char * css, CSSEntries & entries);
public:
    SimpleCSS();

    void reset();

    void append(const char * buf);

    const CSSEntries * getEntries(const CSSString & style_name) const;
private:
    CSSStyles styles_;
};

}


#endif