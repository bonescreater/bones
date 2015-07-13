#ifndef BONES_CORE_SIZE_H_
#define BONES_CORE_SIZE_H_

#include "core.h"

namespace bones
{

class Size
{
public:
    static Size Make(float w, float h);

    Size();

    Size(float w, float h);

    void set(float w, float h);

    float width() const;

    float height() const;

    bool operator == (const Size & rh);

    bool operator != (const Size & rh);
private:
    float width_;
    float height_;
};

}
#endif