#ifndef BONES_BONES_H_
#define BONES_BONES_H_

#include <Windows.h>
#include <stdint.h>

#ifdef __cplusplus
#define BONES_EXTERN_C extern "C"
#else
#define BONES_EXTERN_C
#endif

#ifdef BONES_BUILD_DLL
    #define BONES_DECLSPEC_EXPORT __declspec(dllexport)
    #define BONES_API(type) BONES_EXTERN_C BONES_DECLSPEC_EXPORT type __cdecl
#elif defined BONES_BUILD_LIB
    #define BONES_API(type)
#else
    #define BONES_DECLSPEC_IMPORT __declspec(dllimport)
    #define BONES_API(type) BONES_EXTERN_C BONES_DECLSPEC_IMPORT type __cdecl
#endif

/*!bones初始化参数*/
struct BonesConfig
{
    enum LogLevel
    {
        kNone = 0,
        kError = 1,// only error
        kVerbose = 2,// everything
    };
    LogLevel log_level;
    bool cef_enable;
    const char * cef_locate;//"zh-CN"
};

class BonesCore;
/*!bones初始化
  @param[in] config \n
             config.log_level 输出的日志等级 默认会在当前目录下生成日志文件\n
                              kNone无日志输出\n
                              kError只输出错误\n
                              kVerbose输出所有信息\n
             config.cef_enable true xml中遇到webview标签则创建浏览器\n
                               false 忽略webview标签\n
             config.cef_locate cef_enable为true时有效
  @return 成功返回BonesCore *, 否则返回0 
  @warning BonesStartUp BonesShutDown应该成对使用 通常应用程序只需要初始化一次
  @see BonesCore
  @code
  BonesConfig config
  config.log_level = BonesConfig::kVerbose;
  config.cef_enable = true;
  config.cef_locate = "zh-CN";
  BonesStartUp(config);
  @endcode
 */
BONES_API(BonesCore *) BonesStartUp(const BonesConfig & config);
/*!bones反初始化
   @warning BonesStartUp BonesShutDown应该成对使用
*/
BONES_API(void) BonesShutDown();
/*!bones空闲例程 通常在消息循环空闲时调用
   @warning 不调用BonesUpdate，浏览器和定时器无法正常工作
*/
BONES_API(void) BonesUpdate();
/*!获取当前BonesCore* 
   @return 成功返回当前BonesCore* 失败返回0
   @warning 必须在BonesStartUp后才能调用 在BonesShutDown后不应该再调用该函数
*/
BONES_API(BonesCore *) BonesGetCore();

//基础类型声明
typedef float BonesScalar;
typedef HANDLE BonesCursor;
typedef HBITMAP BonesCaret;
typedef HWND BonesWidget;
typedef void * BonesShader;

/*!矩形描述*/
typedef struct
{
    BonesScalar left;
    BonesScalar top;
    BonesScalar right;
    BonesScalar bottom;
}BonesRect;
/*!尺寸描述*/
typedef struct
{
    BonesScalar width;
    BonesScalar height;
}BonesSize;
/*!点描述*/
typedef struct
{
    BonesScalar x;
    BonesScalar y;
}BonesPoint;
/*!颜色矩阵 4X5\n
    m=[a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t]\n
      |a, b, c, d, e|       |R|      |aR + bG + cB + dA + e|     |R1|\n
      |f, g, h, i, j|       |G|      |fR + gG + hB + iA + j|     |G1|\n
      |-------------|       |B|      |---------------------|     |--|\n
      |k, l, m, n, o|       |A|      |kR + lG + mB + nA + o|     |B1|\n
      |p, q, r, s, t|       |1|      |pR + qG + rB + sA + t|     |A1|\n
*/
typedef struct
{
    BonesScalar mat[20];
} BonesColorMatrix;
/*!颜色描述 ARGB格式 A:31-24 R:23-16 G:15-8 B:7-0*/
typedef uint32_t BonesColor;

/*!BonesPixmap位图接口该接口可以对位图进行操作 位图存放预乘后的像素
   @warning BonesPixmap通过BonesCore来创建和销毁
   @see BonesCore
   @see
*/
class BonesPixmap
{
public:
    struct LockRec
    {
        void * bits;//!<内存首地址
        size_t pitch;//!<每一行的字节数
        BonesRect subset;//!<位图在内存中的矩形区域
    };
public:
    /*!分配一块内存位图
       @param[in] width 位图宽
       @param[in] height 位图高
       @return 成功返回true否则返回false
    */
    virtual bool alloc(int width, int height) = 0;
    /*!解码图片
       @param[in] data 图片内存首地址
       @param[in] len 图片内存大小
       @note 内部使用wic解码 支持png bmp等格式
    */
    virtual bool decode(const void * data, size_t len) = 0;
    /*!得到位图宽
       @return 位图宽
    */
    virtual int getWidth() const = 0;
    /*!得到位图高
       @return 位图高
    */
    virtual int getHeight() const = 0;
    /*!锁住位图 
       @param[out] rec 返回位图信息
       @return 成功返回true 失败返回false
       @warning 必须和unlock成对使用
    */
    virtual bool lock(LockRec & rec) = 0;
    /*!解锁位图
    */
    virtual void unlock() = 0;
    /*!用指定颜色填充位图
       @param[in] color 填充位图的颜色
    */
    virtual void erase(BonesColor color) = 0;
    /*!裁剪子位图
       @param[out] pm 存放裁剪后的子位图
       @param[in] subset 子位图的矩形区域
    */
    virtual void extractSubset(BonesPixmap & pm, const BonesRect & subset) = 0;
};

struct BonesScriptArg
{
    enum Type
    {
        kNill,
        kString,
        kNumber,
        kBoolean,
        kUserData,
    };

    Type type;
    union
    {
        const char * str;
        double number;
        bool boolean;
        void * ud;
    };
};

class BonesObject;
class BonesScriptListener
{
public:
    virtual int onEvent(BonesObject * ob, BonesScriptArg * arg, size_t arg_count) = 0;
};

class BonesEvent
{
public:
    enum Phase
    {
        kCapture,
        kTarget,
        kBubbling,

        kPhaseCount,
    };
public:
    virtual void stopPropagation() = 0;

    virtual void preventDefault() = 0;

    virtual bool canceled() = 0;

    virtual BonesObject * target() = 0;

    virtual Phase phase() = 0;
};

class BonesMouseEvent : public BonesEvent
{
public:
    enum Type
    {
        kMouseEnter,
        kMouseMove,
        kMouseDown,
        kMouseUp,
        kMouseClick,
        kMouseDClick,
        kMouseLeave,

        kTypeCount,
    };

    virtual Type type() const = 0;

    virtual bool isLeftMouse() const = 0; 

    virtual bool isMiddleMouse() const = 0;

    virtual bool isRightMouse() const = 0;

    virtual BonesPoint getLoc() const = 0;

    virtual BonesPoint getRootLoc() const = 0;

    virtual bool isShiftDown() const = 0;

    virtual bool isControlDown() const = 0;

    virtual bool isAltDown() const = 0;

    virtual bool isLeftMouseDown() const = 0;

    virtual bool isMiddleMouseDown() const = 0;

    virtual bool isRightMouseDown() const = 0;

    virtual bool isCapsLockOn() const = 0;

    virtual bool isNumLockOn() const = 0;
};

class BonesKeyEvent : public BonesEvent
{
public:
    enum Type
    {
        kKeyDown,
        kChar,
        kKeyUp,

        kTypeCount,
    };

    struct KeyState
    {
        union
        {
            struct
            {
                uint32_t repeat_count : 16;
                uint32_t scan_code : 8;
                uint32_t extended_key : 1;
                uint32_t reserved : 4;
                uint32_t context_code : 1;
                uint32_t previous_state : 1;
                uint32_t transition_state : 1;
            };
            uint32_t state;
        };
    };

    virtual Type type() const = 0;

    virtual wchar_t ch() const = 0;

    virtual KeyState state() const = 0;

    virtual bool system() const = 0;

    virtual bool isShiftDown() const = 0;

    virtual bool isControlDown() const = 0;

    virtual bool isAltDown() const = 0;

    virtual bool isCapsLockOn() const = 0;

    virtual bool isNumLockOn() const = 0;

    virtual bool isKeyPad() const = 0;

    virtual bool isLeft() const = 0;

    virtual bool isRight() const = 0;
};

class BonesFocusEvent : public BonesEvent
{
public:
    enum Type
    {
        kFocusIn,
        kFocusOut,
        kBlur,
        kFocus,

        kTypeCount,
    };

    virtual Type type() const = 0;

    virtual bool isTabTraversal() const = 0;
};

class BonesWheelEvent : public BonesEvent
{
public:
    virtual int dx() const = 0;

    virtual int dy() const = 0;

    virtual bool isLeftMouse() const = 0;

    virtual bool isMiddleMouse() const = 0;

    virtual bool isRightMouse() const = 0;

    virtual BonesPoint getLoc() const = 0;

    virtual BonesPoint getRootLoc() const = 0;

    virtual bool isShiftDown() const = 0;

    virtual bool isControlDown() const = 0;

    virtual bool isAltDown() const = 0;

    virtual bool isLeftMouseDown() const = 0;

    virtual bool isMiddleMouseDown() const = 0;

    virtual bool isRightMouseDown() const = 0;

    virtual bool isCapsLockOn() const = 0;

    virtual bool isNumLockOn() const = 0;
};

class BonesObject
{
public:
    typedef void * Animation;
    enum AnimationAction
    {
        kStart,
        kStop,
        kPause,
        kResume,
    };
    class AnimationRunListener
    {
    public:
        virtual void onEvent(Animation ani, BonesObject * ob, float progress) = 0;
    };

    class AnimationActionListener
    {
    public:
        virtual void onEvent(Animation ani, BonesObject * ob, AnimationAction action) = 0;
    };
public:
    virtual const char * getClassName() = 0;

    virtual void * cast() = 0;

    virtual const char * getID() = 0;

    virtual BonesObject * getRoot() = 0;

    virtual void listen(const char * name, BonesScriptListener * listener) = 0;

    virtual void push(BonesScriptArg * arg) = 0;

    virtual void setLoc(const BonesPoint & loc) = 0;

    virtual void setSize(const BonesSize & size) = 0;

    virtual float getOpacity() const = 0;

    virtual BonesPoint getLoc() const = 0;

    virtual BonesSize getSize() const = 0;

    virtual void setVisible(bool visible) = 0;

    virtual void setFocusable(bool focusable) = 0;

    virtual bool contains(BonesScalar x, BonesScalar y) = 0;

    virtual BonesObject * getChildAt(size_t index) = 0;

    virtual BonesObject * getParent() = 0;

    virtual void applyCSS(const char * css) = 0;

    virtual void applyClass(const char * name) = 0;

    virtual Animation animate(
        uint64_t interval, uint64_t due, 
        AnimationRunListener * run,
        AnimationActionListener * stop,
        AnimationActionListener * start,
        AnimationActionListener * pause,
        AnimationActionListener * resume) = 0;

    virtual void stopAnimate(Animation ani, bool toend) = 0;

    virtual void pauseAnimate(Animation ani) = 0;

    virtual void resumeAnimate(Animation ani) = 0;

    virtual void stopAllAnimate(bool toend) = 0;
};

class BonesRoot : public BonesObject
{
public:
    class NotifyListener
    {
    public:
        virtual void requestFocus(BonesRoot * sender, bool & stop) = 0;

        virtual void invalidRect(BonesRoot * sender, const BonesRect & rect, bool & stop) = 0;

        virtual void changeCursor(BonesRoot * sender, BonesCursor cursor, bool & stop) = 0;

        virtual void showCaret(BonesRoot * sender, bool fshow, bool & stop) = 0;

        virtual void createCaret(BonesRoot * sender, BonesCaret hbmp, const BonesSize & size, bool & stop) = 0;

        virtual void changeCaretPos(BonesRoot * sender, const BonesPoint & pt, bool & stop) = 0;

        virtual void onSizeChanged(BonesRoot * sender, const BonesSize & size, bool & stop) = 0;

        virtual void onPositionChanged(BonesRoot * sender, const BonesPoint & loc, bool & stop) = 0;

        virtual void onCreate(BonesRoot * sender, bool & stop) = 0;

        virtual void onDestroy(BonesRoot * sender, bool & stop) = 0;
    };

    virtual void setListener(NotifyListener * listener) = 0;

    virtual void setColor(BonesColor color) = 0;

    virtual void attachTo(BonesWidget hwnd) = 0;

    virtual bool isDirty() const = 0;

    virtual BonesRect getDirtyRect() const = 0;

    virtual void draw() = 0;

    virtual HBITMAP getBackBuffer() const = 0;

    virtual void getBackBuffer(const void * & data, size_t & pitch) const = 0;

    virtual bool handleMouse(UINT msg, WPARAM wparam, LPARAM lparam) = 0;

    virtual bool handleKey(UINT msg, WPARAM wparam, LPARAM lparam) = 0;

    virtual bool handleFocus(UINT msg, WPARAM wparam, LPARAM lparam) = 0;

    virtual bool handleComposition(UINT msg, WPARAM wparam, LPARAM lparam) = 0;

    virtual bool handleWheel(UINT msg, WPARAM wparam, LPARAM lparam) = 0;
};

template <class T>
class BonesHandler : public BonesObject
{
public:
    //除root外每一个标签支持的notify
    class NotifyListener
    {
    public:
        virtual void onCreate(T * sender, bool & stop) = 0;

        virtual void onDestroy(T * sender, bool & stop) = 0;

        virtual void onSizeChanged(T * sender, const BonesSize & size, bool & stop) = 0;

        virtual void onPositionChanged(T * sender, const BonesPoint & loc, bool & stop) = 0;

        virtual bool onHitTest(T * sender, const BonesPoint & loc, bool & stop) = 0;
    };

    //除root外每一个标签支持的mouse event
    class MouseListener
    {
    public:
        virtual void onMouseEnter(T * sender, BonesMouseEvent & e, bool & stop) = 0;

        virtual void onMouseMove(T * sender, BonesMouseEvent & e, bool & stop) = 0;

        virtual void onMouseDown(T * sender, BonesMouseEvent & e, bool & stop) = 0;

        virtual void onMouseUp(T * sender, BonesMouseEvent & e, bool & stop) = 0;

        virtual void onMouseClick(T * sender, BonesMouseEvent & e, bool & stop) = 0;

        virtual void onMouseDClick(T * sender, BonesMouseEvent & e, bool & stop) = 0;

        virtual void onMouseLeave(T * sender, BonesMouseEvent & e, bool & stop) = 0;
    };

    //除root外每一个标签支持的key event
    class KeyListener
    {
    public:
        virtual void onKeyDown(T * sender, BonesKeyEvent & e, bool & stop) = 0;

        virtual void onKeyUp(T * sender, BonesKeyEvent & e, bool & stop) = 0;

        virtual void onChar(T * sender, BonesKeyEvent & e, bool & stop) = 0;
    };

    //除root外每一个标签支持的focus event
    class FocusListener
    {
    public:
        virtual void onFocusOut(T * sender, BonesFocusEvent & e, bool & stop) = 0;

        virtual void onFocusIn(T * sender, BonesFocusEvent & e, bool & stop) = 0;

        virtual void onBlur(T * sender, BonesFocusEvent & e, bool & stop) = 0;

        virtual void onFocus(T * sender, BonesFocusEvent & e, bool & stop) = 0;
    };

    //除root外每一个标签支持的wheel event
    class WheelListener
    {
    public:
        virtual void onWheel(T * sender, BonesWheelEvent & e, bool & stop) = 0;
    };
};

class BonesShape : public BonesHandler<BonesShape>
{
public:
    virtual void setStroke(bool stroke) = 0;

    virtual void setStrokeEffect(bool dash, size_t count, 
                                 BonesScalar * interval, 
                                 BonesScalar offset) = 0;

    virtual void setStrokeWidth(BonesScalar stroke_width) = 0;

    virtual void setColor(BonesColor color) = 0;

    virtual void setColor(BonesShader shader) = 0;

    virtual void setCircle(const BonesPoint & center, BonesScalar radius) = 0;

    virtual void setRect(BonesScalar rx, BonesScalar ry, 
                         const BonesRect * rect) = 0;

    virtual void setLine(const BonesPoint & start, const BonesPoint & end) = 0;

    virtual void setPoint(const BonesPoint & pt) = 0;

    virtual void setListener(BonesEvent::Phase phase, MouseListener * lis) = 0;

    virtual void setListener(BonesEvent::Phase phase, KeyListener * lis) = 0;

    virtual void setListener(BonesEvent::Phase phase, FocusListener * lis) = 0;

    virtual void setListener(BonesEvent::Phase phase, WheelListener * lis) = 0;

    virtual void setListener(NotifyListener * lis) = 0;
};

class BonesImage : public BonesHandler<BonesImage>
{
public:
    virtual void setDirect(const BonesPoint * bp) = 0;

    virtual void setStretch(const BonesRect * dst) = 0;

    virtual void setNine(const BonesRect * dst, const BonesRect * center) = 0;

    virtual void setContent(const BonesPixmap & pm) = 0;

    virtual void setContent(const char * key) = 0;

    virtual void setColorMatrix(const BonesColorMatrix & cm) = 0;

    virtual void setListener(BonesEvent::Phase phase, MouseListener * lis) = 0;

    virtual void setListener(BonesEvent::Phase phase, KeyListener * lis) = 0;

    virtual void setListener(BonesEvent::Phase phase, FocusListener * lis) = 0;

    virtual void setListener(BonesEvent::Phase phase, WheelListener * lis) = 0;

    virtual void setListener(NotifyListener * lis) = 0;
};

class BonesText : public BonesHandler<BonesText>
{
public:
    virtual void setListener(BonesEvent::Phase phase, MouseListener * lis) = 0;

    virtual void setListener(BonesEvent::Phase phase, KeyListener * lis) = 0;

    virtual void setListener(BonesEvent::Phase phase, FocusListener * lis) = 0;

    virtual void setListener(BonesEvent::Phase phase, WheelListener * lis) = 0;

    virtual void setListener(NotifyListener * lis) = 0;
};

class BonesRichEdit : public BonesHandler<BonesRichEdit>
{
public:
    class NotifyListener : public BonesHandler::NotifyListener
    {
    public:
        virtual HRESULT txNotify(BonesRichEdit * sender, DWORD iNotify, void  *pv, bool & stop) = 0;
    };
    virtual void setListener(BonesEvent::Phase phase, MouseListener * lis) = 0;

    virtual void setListener(BonesEvent::Phase phase, KeyListener * lis) = 0;

    virtual void setListener(BonesEvent::Phase phase, FocusListener * lis) = 0;

    virtual void setListener(BonesEvent::Phase phase, WheelListener * lis) = 0;

    virtual void setListener(NotifyListener * listener) = 0;
};

class BonesWebView : public BonesHandler<BonesWebView>
{
public:
    virtual bool open() = 0;

    virtual void close() = 0;

    virtual void loadURL(const wchar_t * url) = 0;

    virtual void executeJS(const wchar_t * code, 
                            const wchar_t * url, 
                            int start_line) = 0;

    virtual void setListener(BonesEvent::Phase phase, MouseListener * lis) = 0;

    virtual void setListener(BonesEvent::Phase phase, KeyListener * lis) = 0;

    virtual void setListener(BonesEvent::Phase phase, FocusListener * lis) = 0;

    virtual void setListener(BonesEvent::Phase phase, WheelListener * lis) = 0;

    virtual void setListener(NotifyListener * listener) = 0;
};

class BonesScroller : public BonesHandler<BonesScroller>
{
public:
    class NotifyListener : public BonesHandler::NotifyListener
    {
    public:
        virtual void onScrollRange(BonesScroller * sender,
                                   BonesScalar min_pos,
                                   BonesScalar max_pos,
                                   BonesScalar view_port,
                                   bool horizontal) = 0;

        virtual void onScrollPos(BonesScroller * sender,
                                 BonesScalar cur_pos,
                                 bool horizontal) = 0;
    };

    virtual void setScrollInfo(BonesScalar total, bool horizontal) = 0;

    virtual void setScrollPos(BonesScalar cur, bool horizontal) = 0;

    virtual void setListener(BonesEvent::Phase phase, MouseListener * lis) = 0;

    virtual void setListener(BonesEvent::Phase phase, KeyListener * lis) = 0;

    virtual void setListener(BonesEvent::Phase phase, FocusListener * lis) = 0;

    virtual void setListener(BonesEvent::Phase phase, WheelListener * lis) = 0;

    virtual void setListener(NotifyListener * lis) = 0;
};


class BonesResManager
{
public:
    virtual void clonePixmap(const char * key, BonesPixmap & pm) = 0;

    virtual void cloneCursor(const char * key, BonesCursor cursor) = 0;

    virtual void getPixmap(const char * key, BonesPixmap & pm) = 0;

    virtual void getCursor(const char * key, BonesCursor & cursor) = 0;

    virtual void clean() = 0;
};

class BonesObjectListener
{
public:
    virtual void onPrepare(BonesObject *) = 0;
};

class BonesXMLListener : public BonesObjectListener
{
public:
    virtual bool onLoad(BonesCore *) = 0;

    virtual void onUnload(BonesCore *) = 0;
};

class BonesCore
{
public:
    enum TileMode
    {
        kClamp = 0,
        kRepeat,
        kMirror,
    };

    virtual BonesResManager * getResManager() = 0;

    virtual BonesPixmap * createPixmap() = 0;

    virtual void destroyPixmap(BonesPixmap *) = 0;

    virtual BonesShader createLinearGradient(
        const BonesPoint & begin, 
        const BonesPoint & end,
        size_t count, BonesColor * color,
        BonesScalar * pos, TileMode mode) = 0;

    virtual BonesShader createRadialGradient(
        const BonesPoint & center, 
        BonesScalar radius,
        size_t count, BonesColor * color,
        float * pos, TileMode mode) = 0;

    virtual void destroyShader(BonesShader shader) = 0;

    virtual void setXMLListener(BonesXMLListener * listener) = 0;

    virtual bool loadXMLString(const char * data) = 0;

    virtual bool loadXMLFile(const wchar_t * file) = 0;

    virtual void cleanXML() = 0;

    virtual BonesObject * getObject(const char * id) = 0;

    virtual BonesObject * getObject(BonesObject * bob, const char * id) = 0;

    virtual BonesObject * createObject(BonesObject * parent,
                                       const char * label,
                                       const char * id,
                                       const char * class_name,
                                       const char * group_id,
                                       BonesObjectListener * listener) = 0;

    virtual void cleanObject(BonesObject * bob) = 0;
};



#endif