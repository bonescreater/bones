#ifndef BONES_CORE_SCROLLER_H_
#define BONES_CORE_SCROLLER_H_

#include "area.h"

namespace bones
{

class Scroller : public Area<Scroller>
{//scroller自己的宽高是 ViewPort
public:
    class Delegate : public DelegateBase
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
    Scroller(ThreadContext & context);

    ~Scroller();

    void setDelegate(Delegate * delegate);
    //设置滚动属性
    void setScrollInfo(Scalar total, bool horizontal);

    void Page(bool up);

    void setScrollPos(Scalar cur, bool horizontal);

    Scalar getScrollPos(bool horizontal);

    View * getContainer();

    const char * getClassName() const override;
protected:
    DelegateBase * delegate() override;

    void onSizeChanged() override;
    //支持滚动
    //void onWheel(WheelEvent & e) override;
    //支持pagedown pageup
    //void onKeyDown(KeyEvent & e) override;
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