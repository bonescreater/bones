#ifndef BONES_CORE_ANIMATION_MANAGER_H_
#define BONES_CORE_ANIMATION_MANAGER_H_


#include "ref.h"
#include <map>
#include <list>

namespace bones
{
class Animation;
class View;

class AnimationManager
{
public:
    ~AnimationManager();

    void removeAll(bool end);

    void remove(View * ref, bool end);

    void add(Animation * ani);

    void remove(Animation * ani, bool end);

    void pause(Animation * ani);

    void resume(Animation * ani);

    void run(uint64_t delta);
protected:
    AnimationManager();
private:
    std::map<RefPtr<View>, std::list<RefPtr<Animation>>> animations_;

    friend class Core;
};

}

#endif