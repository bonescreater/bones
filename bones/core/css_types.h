#ifndef BONES_CSS_TYPES_H_
#define BONES_CSS_TYPES_H_

#include "core.h"
#include "color.h"

#include <vector>
#include <map>
#include "shader.h"

namespace bones
{


struct CSSString
{
    CSSString();

    CSSString(const char * b);

    CSSString(const char * b, size_t l);

    const char * begin;
    size_t length;

    operator bool() const;

    bool operator<(const CSSString & right) const;

    bool operator==(const CSSString & right) const;
};

class Ref;

typedef std::vector<CSSString> CSSParams;//16px solid #0xffffffff
typedef std::pair<CSSString, CSSParams> CSSEntry;//font
typedef std::vector<CSSEntry> CSSEntries;//顺序存储
typedef std::map<CSSString, CSSEntries> CSSStyles;

typedef void(Ref::*CSSCLASS_FUNC)(const CSSParams & params);

typedef std::map<CSSString, CSSCLASS_FUNC> CSSClassTable;

struct CSSClassTables
{
    const CSSClassTables *(*base)();
    CSSClassTable * table;
};

#define BONES_CSS_TABLE_DECLARE()\
protected:\
    static const CSSClassTables * GetCSSClassTables(); \
    virtual const CSSClassTables * getCSSClassTables() const; \
    friend class CSSManager; \

#define BONES_CSS_BASE_TABLE_BEGIN(the) \
    const CSSClassTables * the::getCSSClassTables() const\
{\
    return GetCSSClassTables(); \
}\
    const CSSClassTables * the::GetCSSClassTables()\
{\
    static CSSClassTable table; \
if (table.empty())\
{\

#define BONES_CSS_SET_FUNC(a, b)\
    table[a] = static_cast<CSSCLASS_FUNC>(b); \

#define BONES_CSS_BASE_TABLE_END()\
}\
    static CSSClassTables tables = { nullptr, &table }; \
    return &tables; \
} \

#define BONES_CSS_TABLE_BEGIN(the, base) \
    const CSSClassTables * the::getCSSClassTables() const\
{\
    return GetCSSClassTables(); \
}\
    const CSSClassTables * the::GetCSSClassTables()\
{\
    typedef base BaseClass; \
    static CSSClassTable table; \
    if(table.empty())\
    {\

#define BONES_CSS_TABLE_END()\
    }\
    static CSSClassTables tables = { &BaseClass::GetCSSClassTables, &table }; \
    return &tables; \
} \



}
#endif