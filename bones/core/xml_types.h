#ifndef BONES_CORE_XML_TYPES_H_
#define BONES_CORE_XML_TYPES_H_

#include "core.h"
#include <vector>
#include <map>
#include <functional>
#include <string>

namespace bones
{
class Ref;

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

typedef std::vector<CSSString> CSSParams;//16px solid #0xffffffff
typedef std::pair<CSSString, CSSParams> CSSEntry;//font
typedef std::vector<CSSEntry> CSSEntries;//顺序存储
typedef std::map<CSSString, CSSEntries> CSSStyles;

typedef void(*CSSCLASS_FUNC)(Ref * ob, const CSSParams & params);
typedef std::map<CSSString, CSSCLASS_FUNC> CSSClassTable;

//字符串常量
extern const char * kStrType;
extern const char * kStrModule;
extern const char * kStrCSS;
extern const char * kStrXML;
extern const char * kStrID;
extern const char * kStrClass;
extern const char * kStrGroup;
extern const char * kStrFile;

}

#endif