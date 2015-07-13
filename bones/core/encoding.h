#ifndef BONES_CORE_ENCODING_H_
#define BONES_CORE_ENCODING_H_

#include "core.h"
#include <string>

namespace bones
{


class Encoding
{
public:
    static std::string ToUTF8(const wchar_t * wide);

    static std::string ToUTF8(const wchar_t * wide, size_t length);

    static std::wstring FromUTF8(const char * utf8);

    static std::wstring FromUTF8(const char * utf8, size_t length);
};

}


#endif