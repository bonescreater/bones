#ifndef BONES_CORE_SIZE_H_
#define BONES_CORE_SIZE_H_

#include "core.h"

namespace bones
{

class Size
{
public:
    static Size Make(Scalar w, Scalar h);

    Size();

    Size(Scalar w, Scalar h);

    void set(Scalar w, Scalar h);

    Scalar width() const;

    Scalar height() const;

    bool operator == (const Size & rh);

    bool operator != (const Size & rh);
private:
    Scalar width_;
    Scalar height_;
};

}
#endif