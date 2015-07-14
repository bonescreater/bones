#include "animation_manager.h"
#include "animation.h"

namespace bones
{

AnimationManager * AnimationManager::Get()
{
    static AnimationManager * ani_manager = nullptr;
    if (!ani_manager)
        ani_manager = new AnimationManager;
    return ani_manager;
}

AnimationManager::AnimationManager()
{
    ;
}

void AnimationManager::add(Ref * ref, Animation * ani)
{
    if (!ani)
        return;
    RefPtr<Ref> rp;
    rp.reset(ref);
    RefPtr<Animation> rpa;
    rpa.reset(ani);
    animations_[rp].push_back(rpa);
    rpa->start();
}

void AnimationManager::removeAll()
{
    for (auto iter = animations_.begin(); iter != animations_.end(); ++iter)
    {
        auto & anis = iter->second;
        for (auto ani_iter = anis.begin(); ani_iter != anis.end(); ++ani_iter)
        {
            (*ani_iter)->stop();
        }
    }
    animations_.clear();
}

void AnimationManager::run(uint64_t delta)
{
    for (auto iter = animations_.begin(); iter != animations_.end(); ++iter)
    {
        auto & anis = iter->second;
        for (auto ani_iter = anis.begin(); ani_iter != anis.end();)
        {
            auto & ani = *ani_iter;
            if (ani->isStopped())
            {
                ani_iter = anis.erase(ani_iter);
                continue;
            }
            if (!ani->isPaused())
                ani->run(delta);

            ani_iter++;
        }
    }
}

}