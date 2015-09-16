﻿#ifndef BONES_CORE_H_
#define BONES_CORE_H_


#include <stdint.h>
#include <memory.h>
#include <assert.h>
#include <functional>

#include <Windows.h>


class SkPathEffect;
namespace bones
{
extern const char * kClassRoot;
extern const char * kClassView;
extern const char * kClassArea;
extern const char * kClassImage;
extern const char * kClassShape;
extern const char * kClassText;
extern const char * kClassRichEdit;
extern const char * kClassWebView;
extern const char * kClassScroller;

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



typedef float Scalar;
typedef HANDLE Cursor;
typedef HBITMAP Caret;

class AnimationManager;
class PanelManager;
class CSSManager;
class ResManager;
class XMLController;

enum LogLevel
{
    kLOG_LEVEL_NONE = 0,
    kLOG_LEVEL_ERROR = 1,// only error
    kLOG_LEVEL_VERBOSE = 2,// everything
};

class Core
{
public:
    struct Config
    {
        const wchar_t * log_file;
        LogLevel log_level;
        bool enable_cef;
        const char * cef_locate;//"zh-CN"
    };
public:
    static bool StartUp(const Config & config);

    static void ShutDown();

    static void Update();

    static bool EnableCEF();

    static AnimationManager * GetAnimationManager();

    static PanelManager * GetPanelManager();

    static CSSManager * GetCSSManager();

    static ResManager * GetResManager();

    static SkPathEffect * GetDashEffect();

    static XMLController * GetXMLController();
};

}
#endif