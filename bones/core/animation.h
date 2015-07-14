#ifndef BONES_CORE_ANIMATION_H_
#define BONES_CORE_ANIMATION_H_

#include "ref.h"

namespace bones
{

class Animation : public Ref
{
public:
    void start();

    void stop();

    bool isStopped() const;

    void pause();

    void resume();

    bool isPaused() const;

    void run(uint64_t delta);

    virtual const char * getClassName() const override;
protected:
    Animation();

    virtual ~Animation();

    virtual void onRun(uint64_t running);
private:
    uint64_t running_count_;
    bool stopped_;
    bool paused_;

};


}

#endif