#include "encoding.h"

namespace bones
{

std::string Encoding::ToUTF8(const wchar_t * wide)
{
    return ToUTF8(wide, -1);
}

std::string Encoding::ToUTF8(const wchar_t * wide, size_t length)
{
    if (!wide || !length)
        return "";

    int need_len = WideCharToMultiByte(CP_UTF8, 0, wide, length,
        NULL, 0, NULL, NULL);
    if (0 == need_len)
        return "";
    std::string utf8;
    utf8.resize(length == -1 ? need_len - 1 : need_len);
    WideCharToMultiByte(CP_UTF8, 0, wide, length,
        &utf8[0], need_len, NULL, NULL);

    return utf8;
}

std::wstring Encoding::FromUTF8(const char * utf8)
{
    return FromUTF8(utf8, -1);
}

std::wstring Encoding::FromUTF8(const char * utf8, size_t length)
{
    if (!utf8 || !length)
        return L"";
    int need_len = MultiByteToWideChar(CP_UTF8, 0, utf8, length, NULL, 0);
    if (0 == need_len)
        return L"";
    std::wstring wide;
    wide.resize(length == -1 ? need_len - 1 : need_len);

    MultiByteToWideChar(CP_UTF8, 0, utf8, length, &wide[0], need_len);
    return wide;
}

}