#ifndef BONES_CORE_ANIMATION_MANAGER_H_
#define BONES_CORE_ANIMATION_MANAGER_H_


#include "ref.h"
#include <map>
#include <list>

namespace bones
{
class Animation;

class AnimationManager
{
public:
    ~AnimationManager();

    void removeAll();

    void remove(Ref * ref);

    void add(Animation * ani);

    void remove(Animation * ani);

    void pause(Animation * ani);

    void resume(Animation * ani);

    void run(uint64_t delta);
protected:
    AnimationManager();
private:
    std::map<RefPtr<Ref>, std::list<RefPtr<Animation>>> animations_;

    friend class Core;
};

}

#endif