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

//remove 操作只是清空RefPtr<Animation>
//不操作list 防止在loop的过程中 进行remove
void AnimationManager::removeAll()
{
    for (auto iter = animations_.begin(); iter != animations_.end(); ++iter)
    {
        auto & anis = iter->second;
        for (auto ani_iter = anis.begin(); ani_iter != anis.end(); ++ani_iter)
        {
            auto & a = *ani_iter;
            a->stop();
            a.clear();
        }
    }
}

void AnimationManager::remove(Ref * ref, Animation * ani)
{
    if (!ani)
        return;
    RefPtr<Ref> key;
    key.reset(ref);
    auto iter = animations_.find(key);
    if (iter == animations_.end())
        return;

    auto & anis = iter->second;
    for (auto ani_iter = anis.begin(); ani_iter != anis.end(); ++ani_iter)
    {
        auto & a = *ani_iter;
        if (a.get() == ani)
        {
            a->stop();
            a.clear();
        }
    }
}

void AnimationManager::remove(Ref * ref)
{
    RefPtr<Ref> key;
    key.reset(ref);
    auto iter = animations_.find(key);
    if (iter == animations_.end())
        return;
    auto & anis = iter->second;
    for (auto ani_iter = anis.begin(); ani_iter != anis.end(); ++ani_iter)
    {
        auto & a = *ani_iter;
        a->stop();
        a.clear();
    }
}

//仅在run里才操作 list 进行remove
void AnimationManager::run(uint64_t delta)
{
    for (auto iter = animations_.begin(); iter != animations_.end();)
    {
        auto & anis = iter->second;

        for (auto ani_iter = anis.begin(); ani_iter != anis.end();)
        {//运行animation 并移除 stopped 和空指针
            auto & ani = *ani_iter;
            if (ani && ani->isRunning())
                ani->run(delta);

            if (!ani || ani->isStopped())
                ani_iter = anis.erase(ani_iter);
            else
                ani_iter++;
        }

        if (anis.empty())
            iter = animations_.erase(iter);
        else
            iter++;
    }
}

}