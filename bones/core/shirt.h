#ifndef BONES_CORE_SHIRT_H_
#define BONES_CORE_SHIRT_H_

#include "view.h"

namespace bones
{
//外观
class Shirt : public View
{
public:
    void setOpacity(float opacity);//0~1.f

    float getOpacity() const;
protected:
    Shirt();

    bool isFocusable() const override;
protected:
    float opacity_;
};

}
#endif