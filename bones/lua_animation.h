#ifndef BONES_LUA_ANIMATION_H_
#define BONES_LUA_ANIMATION_H_

#include "bones_internal.h"
#include "core/ref.h"


namespace bones
{
class View;
class Animation;

class LuaAnimation : public BonesAnimation, public Ref
{

public:
    LuaAnimation(
        View * target,
        uint64_t interval, uint64_t due,
        BonesAnimation::RunListener * run,
        BonesAnimation::ActionListener * stop,
        BonesAnimation::ActionListener * start,
        BonesAnimation::ActionListener * pause,
        BonesAnimation::ActionListener * resume,
        AnimationType type);

    ~LuaAnimation();
public:
    void retain() override;

    void release() override;

    Animation * ani();
protected:
    void run(Animation * sender, View * target, float progress);

    void stop(Animation * sender, View * target);

    void start(Animation * sender, View * target);

    void pause(Animation * sender, View * target);

    void resume(Animation * sender, View * target);
private:
    AnimationType type_;
    Animation * animation_;
    BonesAnimation::RunListener * run_;
    BonesAnimation::ActionListener * stop_;
    BonesAnimation::ActionListener * start_;
    BonesAnimation::ActionListener * pause_;
    BonesAnimation::ActionListener * resume_;
};

}


#endif