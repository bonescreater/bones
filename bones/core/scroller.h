#ifndef BONES_CORE_SCROLLER_H_
#define BONES_CORE_SCROLLER_H_

#include "view.h"

namespace bones
{

class Scroller : public View
{//scroller自己的宽高是 ViewPort
public:
    class Delegate
    {
    public:
        virtual void onScrollRange(Scroller * sender, 
                                   Scalar min_pos, 
                                   Scalar max_pos, 
                                   Scalar view_port,
                                   bool horizontal) = 0;

        virtual void onScrollPos(Scroller * sender, 
                                 Scalar cur_pos, 
                                 bool horizontal) = 0;
    };
private:
    struct ScrollInfo
    {
        Scalar min_pos;
        Scalar max_pos;
        Scalar cur_pos;
        Scalar total;
    };
public:
    Scroller();

    ~Scroller();

    void setDelegate(Delegate * delegate);
    //设置滚动属性
    void setScrollInfo(Scalar total, bool horizontal);

    void setScrollPos(Scalar cur, bool horizontal);

    View * getContainer();

    const char * getClassName() const override;
protected:
    void onSizeChanged() override;
    //支持滚动
    void onWheel(WheelEvent & e) override;
private:
    void updateVInfo();
    void updateHInfo();
private:
    View * container_;
    Delegate * delegate_;
    ScrollInfo v_info_;
    ScrollInfo h_info_;
};


}


#endif