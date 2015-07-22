#include "animation_manager.h"
#include "animation.h"
#include "logging.h"

namespace bones
{

AnimationManager::AnimationManager()
{
    ;
}

AnimationManager::~AnimationManager()
{//不需要在调用animation的stop 因为AnimationManager是在ShutDown后才析构 
 //此时调用stop 可能 Ref 的数值 结构已经被破坏了 如view的父子结构
    ;
}

//remove 操作只是清空RefPtr<Animation>
//不操作list 防止在loop的过程中 进行remove
void AnimationManager::removeAll(bool end)
{
    for (auto iter = animations_.begin(); iter != animations_.end(); ++iter)
    {
        auto & anis = iter->second;
        for (auto ani_iter = anis.begin(); ani_iter != anis.end(); ++ani_iter)
        {
            auto & a = *ani_iter;
            if (a)
            {
                a->stop(end);
                a.clear();
            }
        }
    }
}

void AnimationManager::remove(Ref * ref, bool end)
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
        if (a)
        {
            a->stop(end);
            a.clear();
        }
    }
}

void AnimationManager::add(Animation * ani)
{
    if (!ani)
        return;
    RefPtr<Ref> rp;
    rp.reset(ani->target());
    RefPtr<Animation> rpa;
    rpa.reset(ani);
    auto & anis = animations_[rp];
    auto iter = std::find(anis.begin(), anis.end(), rpa);
    if (iter == anis.end())
    {
        anis.push_back(rpa);
        rpa->start();
    }
    else
        LOG_VERBOSE << "Animation already exits";

}

void AnimationManager::remove(Animation * ani, bool end)
{
    if (!ani)
        return;
    RefPtr<Ref> key;
    key.reset(ani->target());
    auto iter = animations_.find(key);
    if (iter == animations_.end())
        return;
    RefPtr<Animation> rpa;
    rpa.reset(ani);
    auto ani_iter = std::find(iter->second.begin(), iter->second.end(), rpa);
    if (ani_iter != iter->second.end())
    {
        auto & a = *ani_iter;
        a->stop(end);
        a.clear();
    }
}

void AnimationManager::pause(Animation * ani)
{
    if (!ani)
        return;
    RefPtr<Ref> key;
    key.reset(ani->target());
    auto iter = animations_.find(key);
    if (iter == animations_.end())
        return;
    RefPtr<Animation> rpa;
    rpa.reset(ani);
    auto ani_iter = std::find(iter->second.begin(), iter->second.end(), rpa);
    if (ani_iter != iter->second.end())
    {
        auto & a = *ani_iter;
        a->pause();
    }
}

void AnimationManager::resume(Animation * ani)
{
    if (!ani)
        return;
    RefPtr<Ref> key;
    key.reset(ani->target());
    auto iter = animations_.find(key);
    if (iter == animations_.end())
        return;
    RefPtr<Animation> rpa;
    rpa.reset(ani);
    auto ani_iter = std::find(iter->second.begin(), iter->second.end(), rpa);
    if (ani_iter != iter->second.end())
    {
        auto & a = *ani_iter;
        a->resume();
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