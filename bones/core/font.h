#ifndef BONES_CORE_FONT_H_
#define BONES_CORE_FONT_H_

#include "core.h"

namespace bones
{


class Font
{
public:
    enum Style//字体样式
    {
        kNormal = 0,
        kBold = 1 << 0,//!<粗体
        kItalic = 1 << 1,//!<倾斜
        kUnderline = 1 << 2,//下划线
        kStrikeOut = 1 << 3,//删除线
    };
public:
    Font();

    void setFamily(const char * family);

    void setStyle(uint32_t st);

    void setSize(Scalar s);

    const char * getFamily() const;

    uint32_t getStyle() const;

    Scalar getSize() const;
private:
    std::string family_;
    uint32_t st_;
    Scalar size_;
};

}

#endif