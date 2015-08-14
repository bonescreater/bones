#ifndef BONES_CORE_CSS_MANAGER_H_
#define BONES_CORE_CSS_MANAGER_H_

#include "core.h"
#include "css_types.h"

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

class CSSManager
{
public:
    void clean();

    void append(const char * buf);

    void applyCSS(Ref * ob, const char * css);

    void applyClass(Ref * ob, const char * class_name);
protected:
    CSSManager();
private:
    void applyEntries(Ref * ob, const CSSEntries & entries);
    //{css:mmm;}
    void parse(const char * css, CSSEntries & entries);
private:
    CSSStyles styles_;

    friend class Core;
};

}


#endif