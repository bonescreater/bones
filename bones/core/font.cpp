#include "font.h"

namespace bones
{

Font::Font()
:family_("Microsoft Yahei"), st_(kNormal), size_(12)
{

}
void Font::setFamily(const char * family)
{
    family_.clear();
    if (family)
        family_ = family;
}

void Font::setStyle(uint32_t st)
{
    st_ = st;
}

void Font::setSize(Scalar s)
{
    size_ = s;
}

const char * Font::getFamily() const
{
    return family_.data();
}

uint32_t Font::getStyle() const
{
    return st_;
}

Scalar Font::getSize() const
{
    return size_;
}

}