#ifndef BONES_CORE_H_
#define BONES_CORE_H_


#include <stdint.h>
#include <memory.h>
#include <assert.h>

//gtest单元测试
#include "../gtest/include/gtest/gtest_prod.h"

namespace bones
{
extern const char * kClassRoot;
extern const char * kClassView;
extern const char * kClassImage;
extern const char * kClassShape;
extern const char * kClassText;
extern const char * kClassInput;
extern const char * kClassRichEdit;
extern const char * kClassWebView;
extern const char * kClassScroller;

typedef float Scalar;

#define BONES_CLASS_CALLBACK_0(__selector__, __target__, ...) std::bind(__selector__, __target__, ##__VA_ARGS__)
#define BONES_CLASS_CALLBACK_1(__selector__, __target__, ...) std::bind(__selector__, __target__, std::placeholders::_1, ##__VA_ARGS__)
#define BONES_CLASS_CALLBACK_2(__selector__, __target__, ...) std::bind(__selector__, __target__, std::placeholders::_1, std::placeholders::_2, ##__VA_ARGS__)
#define BONES_CLASS_CALLBACK_3(__selector__, __target__, ...) std::bind(__selector__, __target__, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, ##__VA_ARGS__)
#define BONES_CLASS_CALLBACK_4(__selector__, __target__, ...) std::bind(__selector__, __target__, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, ##__VA_ARGS__)

#define BONES_CALLBACK_0(__selector__, ...) std::bind(__selector__, ##__VA_ARGS__)
#define BONES_CALLBACK_1(__selector__, ...) std::bind(__selector__, std::placeholders::_1, ##__VA_ARGS__)
#define BONES_CALLBACK_2(__selector__, ...) std::bind(__selector__, std::placeholders::_1, std::placeholders::_2, ##__VA_ARGS__)
#define BONES_CALLBACK_3(__selector__, ...) std::bind(__selector__, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, ##__VA_ARGS__)
#define BONES_CALLBACK_4(__selector__, ...) std::bind(__selector__, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, ##__VA_ARGS__)

class NoncopyableObject
{
protected:
    NoncopyableObject();
private:
    NoncopyableObject(const NoncopyableObject&);
    NoncopyableObject& operator=(const NoncopyableObject&);
};

class ThreadDispatcher;
class AnimationManager;

class ThreadContext
{
public:
    virtual ThreadDispatcher & getDispatcher() = 0;

    virtual AnimationManager & getAnimationManager() = 0;
};

}
#endif