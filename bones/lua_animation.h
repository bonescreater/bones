#ifndef BONES_LUA_ANIMATION_H_
#define BONES_LUA_ANIMATION_H_

#include "bones_internal.h"
#include "core/ref.h"


namespace bones
{
class View;
class Animation;

class LuaAnimation :public Ref
{

public:
    LuaAnimation(
        View * target,
        uint64_t interval, uint64_t due,
        BonesObject::AnimationRunListener * run,
        BonesObject::AnimationActionListener * stop,
        BonesObject::AnimationActionListener * start,
        BonesObject::AnimationActionListener * pause,
        BonesObject::AnimationActionListener * resume,
        AnimationType type);

    ~LuaAnimation();
public:
    Animation * ani();
protected:
    void run(Animation * sender, View * target, float progress);

    void stop(Animation * sender, View * target);

    void start(Animation * sender, View * target);

    void pause(Animation * sender, View * target);

    void resume(Animation * sender, View * target);
private:
    void createLuaTable();
private:
    AnimationType type_;
    Animation * animation_;
    BonesObject::AnimationRunListener * run_;
    BonesObject::AnimationActionListener * stop_;
    BonesObject::AnimationActionListener * start_;
    BonesObject::AnimationActionListener * pause_;
    BonesObject::AnimationActionListener * resume_;
};

}


#endif