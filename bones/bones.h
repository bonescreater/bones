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
    bool aa_enable;//anti alias
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
             config.aa_enable true 启用抗锯齿
             config.cef_enable true xml中遇到webview标签则创建浏览器\n
                               false 忽略webview标签\n
             config.cef_locate cef_enable为true时有效
  @return 成功返回BonesCore *, 否则返回0 
  @warning BonesStartUp BonesShutDown应该成对使用 通常应用程序只需要初始化一次
  @see BonesCore
  @code
  BonesConfig config
  config.log_level = BonesConfig::kVerbose;
  config.aa_enable = true;
  config.cef_enable = false;//不支持webview
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
/*!字体描述*/
struct BonesFont
{
    const char * family;//!<family name like "Microsoft YaHei"
    BonesScalar size;//!<字体大小
    bool bold;//!<true为粗体
    bool italic;//!<true为斜体
    bool underline;//!<true字体有下划线
    bool strike;//!<true字体有删除线
};
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
/*!颜色描述 ARGB格式 A:31-24 R:23-16 G:15-8 B:7-0 预乘后的颜色*/
typedef uint32_t BonesPMColor;
/*!BonesPixmap位图接口该接口可以对位图进行操作 位图存放预乘后的像素
   @note BonesPixmap通过BonesCore来创建和销毁
   @see BonesCore::createPixmap
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
        kCapturing,
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

/*!基类 所有标签继承自BonesObject*/
class BonesObject
{
public:
    typedef void * Animation;
    enum AnimationAction
    {
        kStart,//!启动
        kStop,//!<停止
        kPause,//!<暂停
        kResume,//!<恢复
    };
    /*!定时器运行时的回调接口*/
    class AnimationRunListener
    {
    public:
        /*!定时器运行时的回调函数
        @param[in] ani 定时器ID
        @param[in] ob 与定时器关联的标签
        @param[in] progress 进度0~1.0f 当进度为1.0f之后 定时器会调用kStop的回调
        */
        virtual void onEvent(Animation ani, BonesObject * ob, float progress) = 0;
    };
    /*!定时器控制接口的回调*/
    class AnimationActionListener
    {
    public:
        /*!定时器运行时的回调函数
        @param[in] ani 定时器ID
        @param[in] ob 与定时器关联的标签
        @param[in] action 当标签调用pause stop resume会触发对应的回调
        */
        virtual void onEvent(Animation ani, BonesObject * ob, AnimationAction action) = 0;
    };
public:
    /*!标签的具体类型描述
    @return 标签的类型 与xml 标签一致 如BonesRoot 返回"root" BonesImage 返回"image"
    */
    virtual const char * getClassName() = 0;
    /*!从基类获得子类标签指针
    @return 标签的指针
    */
    virtual void * cast() = 0;
    /*!得到标签ID
    @return 标签ID
    */
    virtual const char * getID() = 0;
    /*!得到标签的根节点
    @return 标签的根节点
    */
    virtual BonesObject * getRoot() = 0;

    virtual void listen(const char * name, BonesScriptListener * listener) = 0;

    virtual void push(BonesScriptArg * arg) = 0;
    /*!设置标签位置
    @param[in] loc 位置 相对于父的左上角 如果是root 则相对于(0, 0)
    @see BonesPoint
    */
    virtual void setLoc(const BonesPoint & loc) = 0;
    /*!设置标签尺寸
    @param[in] size 尺寸
    @see BonesSize
    */
    virtual void setSize(const BonesSize & size) = 0;
    /*!设置标签透明度
    @param[in] opacity 透明度0~1.0f
    */
    virtual void setOpacity(float opacity) = 0;
    /*!得到标签透明度
    @return 透明度0~1.0f
    @note 透明度永远不能超过父的透明度 如父的透明度0.5 标签透明度1.0f
          标签会以0.5的透明度显示出来，透明度为0时并不影响visible属性
    */
    virtual float getOpacity() const = 0;
    /*!得到标签当前位置
    @return 标签位置 相对于父的左上角
    */
    virtual BonesPoint getLoc() const = 0;
    /*!得到标签当前尺寸
    @return 标签尺寸 
    */
    virtual BonesSize getSize() const = 0;
    /*!设置标签是否可见
    @param[in] visible true则可见 false不可见
    @note 如果不可见则鼠标和绘制都会忽略该标签以及该标签的子，默认可见
    */
    virtual void setVisible(bool visible) = 0;
    /*!设置标签是否可接收焦点
    @param[in] focusable true则可接收 false不可接收
    @note 默认接收焦点
    */
    virtual void setFocusable(bool focusable) = 0;
    /*!测试点是否在标签内
    @param[in] x 水平方向坐标 相对于标签左上角
    @param[in] y 垂直方向坐标 相对于标签左上角
    @return true则代表在标签内 false代表不在
    */
    virtual bool contains(BonesScalar x, BonesScalar y) = 0;
    /*!得到指定索引的子标签
    @param[in] index 子标签的索引
    @return 找到则返回标签 否则返回空 
    @note 索引与xml节点顺序相同
    */
    virtual BonesObject * getChildAt(size_t index) = 0;
    /*!得到父标签
    @return 父标签
    */
    virtual BonesObject * getParent() = 0;

    virtual void applyCSS(const char * css) = 0;

    virtual void applyClass(const char * name) = 0;
    /*!创建并启动定时器
    @param[in] interval 定时器运行间隔
    @param[in] due 定时器运行总时长 -1则代表一直运行
    @param[in] run 定时器正常运行时的回调函数
    @param[in] stop 调用stopAnimate 和stopAllAnimate时的回调函数 可以为空
    @param[in] start 定时器启动时回调函数 可以为空
    @param[in] pause 调用pauseAnimte时的回调函数
    @param[in] resume 调用resumeAnimate时的回调函数
    @return 定时器ID
    @warning 定时器ID在stop以后失效 如果有变量保存了定时器ID 那么最好在stop回调中将变量置空
    */
    virtual Animation animate(
        uint64_t interval, uint64_t due, 
        AnimationRunListener * run,
        AnimationActionListener * stop,
        AnimationActionListener * start,
        AnimationActionListener * pause,
        AnimationActionListener * resume) = 0;
    /*!停止定时器
    @param[in] ani 定时器ID
    @param[in] toend true则以1.0f的progress 调用run，确保定时器停止前工作已经完成
                     false 停止定时器
    */
    virtual void stopAnimate(Animation ani, bool toend) = 0;
    /*!暂停定时器
    @param[in] ani 定时器ID
    */
    virtual void pauseAnimate(Animation ani) = 0;
    /*!恢复运行定时器
    @param[in] ani 定时器ID
    */
    virtual void resumeAnimate(Animation ani) = 0;
    /*!停止与标签关联的所有定时器
    @param[in] toend true则以1.0f的progress 调用run，确保定时器停止前工作已经完成
                     false 停止定时器
    */
    virtual void stopAllAnimate(bool toend) = 0;
};

/*!root 标签提供了与native window 交互的接口*/
class BonesRoot : public BonesObject
{
public:
    /*!使用root要响应的通知
    @note root通常是与本地窗口交互
          由于脚本层没有调用windows api扩展库 大多数的通知没有必要传到脚本层 即stop 返回true
    */
    class NotifyListener
    {
    public:
        /*!内部标签需要获取焦点
        @param[in] sender 标签
        @param[out] stop 是否将通知传递到脚本层处理 true则不传递 false传递
        */
        virtual void requestFocus(BonesRoot * sender, bool & stop) = 0;
        /*!root需要重绘
        @param[in] sender 标签
        @param[in] rect 需要重绘的区域
        @param[out] stop 是否将通知传递到脚本层处理 true则不传递 false传递
        */
        virtual void invalidRect(BonesRoot * sender, const BonesRect & rect, bool & stop) = 0;
        /*!root鼠标样式改变
        @param[in] sender 标签
        @param[in] cursor 新的鼠标样式
        @param[out] stop 是否将通知传递到脚本层处理 true则不传递 false传递
        */
        virtual void changeCursor(BonesRoot * sender, BonesCursor cursor, bool & stop) = 0;
        /*!root需要显示光标
        @param[in] sender 标签
        @param[in] fshow true 显示光标 false隐藏光标
        @param[out] stop 是否将通知传递到脚本层处理 true则不传递 false传递
        */
        virtual void showCaret(BonesRoot * sender, bool fshow, bool & stop) = 0;
        /*!root需要创建光标
        @param[in] sender 标签
        @param[in] hbmp 光标位图
        @param[in] size 光标的宽高 如hbmp不为空则忽略
        @param[out] stop 是否将通知传递到脚本层处理 true则不传递 false传递
        */
        virtual void createCaret(BonesRoot * sender, BonesCaret hbmp, const BonesSize & size, bool & stop) = 0;
        /*!root需要改变光标位置
        @param[in] sender 标签
        @param[in] pt 光标位置
        @param[out] stop 是否将通知传递到脚本层处理 true则不传递 false传递
        */
        virtual void changeCaretPos(BonesRoot * sender, const BonesPoint & pt, bool & stop) = 0;
        /*!尺寸改变后收到该通知
        @param[in] sender 标签
        @param[in] size 标签改变后的尺寸
        @param[out] stop 是否将通知传递到脚本层处理 true则不传递 false传递
        @see BonesSize
        */
        virtual void onSizeChanged(BonesRoot * sender, const BonesSize & size, bool & stop) = 0;
        /*!位置改变后收到该通知
        @param[in] sender 标签
        @param[in] loc 标签改变后的位置
        @param[out] stop 是否将通知传递到脚本层处理 true则不传递 false传递
        @see BonesPoint
        */
        virtual void onPositionChanged(BonesRoot * sender, const BonesPoint & loc, bool & stop) = 0;
        /*!创建后收到该通知
        @param[in] sender 标签
        @param[out] stop 是否将通知传递到脚本层处理 true则不传递 false传递
        */
        virtual void onCreate(BonesRoot * sender, bool & stop) = 0;
        /*!即将销毁收到该通知
        @param[in] sender 标签
        @param[out] stop 是否将通知传递到脚本层处理 true则不传递 false传递
        */
        virtual void onDestroy(BonesRoot * sender, bool & stop) = 0;
    };
    /*!设置通知回调*/
    virtual void setListener(NotifyListener * listener) = 0;
    /*!设置root的颜色
    @param[in] color root的颜色
    */
    virtual void setColor(BonesColor color) = 0;
    /*!设置root关联的窗口
    @param[in] hwnd 窗口
    @note 目前除了richedit标签外都不需要依赖窗口如果不使用richedit 则可以不关联窗口
    */
    virtual void attachTo(BonesWidget hwnd) = 0;
    /*!root是否需要重绘
    @return true则需要重绘 false 不需要
    */
    virtual bool isDirty() const = 0;
    /*!root需要绘制的区域
    @return 重绘区域
    */
    virtual BonesRect getDirtyRect() const = 0;
    /*!root发起一次绘制 绘制后当前的脏区清空*/
    virtual void draw() = 0;
    /*!得到root的位图
    @return 位图 可以方便绘制到窗口
    */
    virtual HBITMAP getBackBuffer() const = 0;
    /*!得到root的位图像素
    @param[out] 得到位图像素的首地址
    @param[out] 得到位图每一行占用的字节数
    @note 位图的尺寸即为root的尺寸,位图像素为预乘后的像素
    */
    virtual void getBackBuffer(const void * & data, size_t & pitch) const = 0;
    /*!传递mouse消息
    @return 通常返回true
    */
    virtual bool handleMouse(UINT msg, WPARAM wparam, LPARAM lparam) = 0;
    /*!传递key消息
    @return 通常返回true
    */
    virtual bool handleKey(UINT msg, WPARAM wparam, LPARAM lparam) = 0;
    /*!传递focus消息
    @return 通常返回true
    */
    virtual bool handleFocus(UINT msg, WPARAM wparam, LPARAM lparam) = 0;
    /*!传递ime消息
    @return 返回false则需要自己处理
    @note 由于webview使用的cef浏览器 暂时没有找到处理IME的接口
          所以返回false时需要自己处理 通常是调用DefWindowProc
    */
    virtual bool handleComposition(UINT msg, WPARAM wparam, LPARAM lparam) = 0;
    /*!传递wheel消息
    @return 通常返回true
    */
    virtual bool handleWheel(UINT msg, WPARAM wparam, LPARAM lparam) = 0;
};

template <class T>
class BonesHandler : public BonesObject
{
public:
    /*!除root外每一个标签支持的通知*/
    class NotifyListener
    {
    public:
        /*!在节点创建后收到该通知
           @param[in] sender 标签
           @param[out] stop 是否将通知传递到脚本层处理 true则不传递 false传递
           @note 通知顺序是先发送子节点的onCreate 然后是父节点的onCreate,这意味着
                 当接收到一个节点的onCreate通知时 意味着它的子节点已经处理过onCreate通知
                 并初始化完毕，所以此时可以安全的设置子节点属性
        */
        virtual void onCreate(T * sender, bool & stop) = 0;
        /*!在节点即将销毁收到该通知
        @param[in] sender 标签
        @param[out] stop 是否将通知传递到脚本层处理 true则不传递 false传递
        @note 通知顺序是先发送父节点的onDestroy 然后是子节点的onDestroy,这意味着
        当接收到一个节点的onDestroy通知时 意味着它的子节点还没有处理过onDestroy通知
        所以此时可以安全的设置子节点属性 通常是恢复onCreate中设置的属性
        */
        virtual void onDestroy(T * sender, bool & stop) = 0;
        /*!在节点尺寸改变后收到该通知
        @param[in] sender 标签
        @param[in] size 标签改变后的尺寸
        @param[out] stop 是否将通知传递到脚本层处理 true则不传递 false传递
        @see BonesSize
        */
        virtual void onSizeChanged(T * sender, const BonesSize & size, bool & stop) = 0;
        /*!在节点位置改变后收到该通知
        @param[in] sender 标签
        @param[in] loc 标签改变后的位置
        @param[out] stop 是否将通知传递到脚本层处理 true则不传递 false传递
        @see BonesPoint
        */
        virtual void onPositionChanged(T * sender, const BonesPoint & loc, bool & stop) = 0;
        /*!在节点接收鼠标事件前会先收到onHitTest通知，该通知决定了标签是否接收鼠标事件
        @param[in] sender 标签
        @param[in] loc 鼠标在标签中的位置
        @param[out] stop 是否将通知传递到脚本层处理 true则不传递 false传递
        @return 返回true 则代表接收鼠标事件 返回false 代表不接受鼠标事件
        @see BonesPoint
        */
        virtual bool onHitTest(T * sender, const BonesPoint & loc, bool & stop) = 0;
    };

    /*!除root外每一个标签支持的mouse event*/
    class MouseListener
    {
    public:
        /*!鼠标进入事件
        @param[in] sender 标签
        @param[in] e 鼠标事件
        @param[out] stop 是否将通知传递到脚本层处理 true则不传递 false传递
        @see BonesMouseEvent
        */
        virtual void onMouseEnter(T * sender, BonesMouseEvent & e, bool & stop) = 0;
        /*!鼠标移动事件
        @param[in] sender 标签
        @param[in] e 鼠标事件
        @param[out] stop 是否将通知传递到脚本层处理 true则不传递 false传递
        @note 鼠标默认在左键按下时启用capture模式，在capture时鼠标事件的位置有可能在标签外部
        @see BonesMouseEvent
        */
        virtual void onMouseMove(T * sender, BonesMouseEvent & e, bool & stop) = 0;
        /*!鼠标按下事件
        @param[in] sender 标签
        @param[in] e 鼠标事件
        @param[out] stop 是否将通知传递到脚本层处理 true则不传递 false传递
        @note 左键按下时启用capture模式
        @see BonesMouseEvent
        */
        virtual void onMouseDown(T * sender, BonesMouseEvent & e, bool & stop) = 0;
        /*!鼠标弹起事件
        @param[in] sender 标签
        @param[in] e 鼠标事件
        @param[out] stop 是否将通知传递到脚本层处理 true则不传递 false传递
        @note 左键弹起时关闭capture模式 如果弹起时 鼠标不再标签内部 则产生enter 和leave事件
        @see BonesMouseEvent
        */
        virtual void onMouseUp(T * sender, BonesMouseEvent & e, bool & stop) = 0;
        /*!鼠标点击事件
        @param[in] sender 标签
        @param[in] e 鼠标事件
        @param[out] stop 是否将通知传递到脚本层处理 true则不传递 false传递
        @warning 目前没有针对事件发生的时间来判断鼠标是点击还是双击，除非是由root直接传递
        click和dclick事件，否则不会自动产生click 和dclick事件
        @see BonesMouseEvent
        */
        virtual void onMouseClick(T * sender, BonesMouseEvent & e, bool & stop) = 0;
        /*!鼠标双击事件
        @param[in] sender 标签
        @param[in] e 鼠标事件
        @param[out] stop 是否将通知传递到脚本层处理 true则不传递 false传递
        @warning 目前没有针对事件发生的时间来判断鼠标是点击还是双击，除非是由root直接传递
        click和dclick事件，否则不会自动产生click 和dclick事件
        @see BonesMouseEvent
        */
        virtual void onMouseDClick(T * sender, BonesMouseEvent & e, bool & stop) = 0;
        /*!鼠标离开事件
        @param[in] sender 标签
        @param[in] e 鼠标事件
        @param[out] stop 是否将通知传递到脚本层处理 true则不传递 false传递
        @note capture模式下 离开标签不会产生leave事件，在鼠标左键弹起 关闭capture后 才产生leave消息
        */
        virtual void onMouseLeave(T * sender, BonesMouseEvent & e, bool & stop) = 0;
    };

    /*!除root外每一个标签支持的key event*/
    class KeyListener
    {
    public:
        /*!键盘按下事件
        @param[in] sender 标签
        @param[in] e 键盘事件
        @param[out] stop 是否将通知传递到脚本层处理 true则不传递 false传递
        @note 键盘事件默认发送给当前焦点标签
        @see BonesKeyEvent
        */
        virtual void onKeyDown(T * sender, BonesKeyEvent & e, bool & stop) = 0;
        /*!键盘弹起事件
        @param[in] sender 标签
        @param[in] e 键盘事件
        @param[out] stop 是否将通知传递到脚本层处理 true则不传递 false传递
        @note 键盘事件默认发送给当前焦点标签
        @see BonesKeyEvent
        */
        virtual void onKeyUp(T * sender, BonesKeyEvent & e, bool & stop) = 0;
        /*!键盘字符事件
        @param[in] sender 标签
        @param[in] e 键盘事件
        @param[out] stop 是否将通知传递到脚本层处理 true则不传递 false传递
        @note 键盘事件默认发送给当前焦点标签 ch()函数返回字符
        @see BonesKeyEvent::ch
        */
        virtual void onChar(T * sender, BonesKeyEvent & e, bool & stop) = 0;
    };

    /*!除root外每一个标签支持的focus event*/
    class FocusListener
    {
    public:
        /*!焦点即将离开
        @param[in] sender 标签
        @param[in] e 焦点事件
        @param[out] stop 是否将通知传递到脚本层处理 true则不传递 false传递
        @see BonesFocusEvent
        */
        virtual void onFocusOut(T * sender, BonesFocusEvent & e, bool & stop) = 0;
        /*!焦点即将进入
        @param[in] sender 标签
        @param[in] e 焦点事件
        @param[out] stop 是否将通知传递到脚本层处理 true则不传递 false传递
        @see BonesFocusEvent
        */
        virtual void onFocusIn(T * sender, BonesFocusEvent & e, bool & stop) = 0;
        /*!焦点离开
        @param[in] sender 标签
        @param[in] e 焦点事件
        @param[out] stop 是否将通知传递到脚本层处理 true则不传递 false传递
        @see BonesFocusEvent
        */
        virtual void onBlur(T * sender, BonesFocusEvent & e, bool & stop) = 0;
        /*!焦点进入
        @param[in] sender 标签
        @param[in] e 焦点事件
        @param[out] stop 是否将通知传递到脚本层处理 true则不传递 false传递
        @see BonesFocusEvent
        */
        virtual void onFocus(T * sender, BonesFocusEvent & e, bool & stop) = 0;
    };

    /*!除root外每一个标签支持的wheel event*/
    class WheelListener
    {
    public:
        /*!响应滚轮事件
        @param[in] sender 标签
        @param[in] e 滚轮事件
        @param[out] stop 是否将通知传递到脚本层处理 true则不传递 false传递
        @note 滚轮事件传递给当前鼠标所在的标签 而不是windows默认的传递给当前焦点标签
        @see BonesWheelEvent
        */
        virtual void onWheel(T * sender, BonesWheelEvent & e, bool & stop) = 0;
    };
};
/*!shape标签 提供了几何形状的绘制*/
class BonesShape : public BonesHandler<BonesShape>
{
public:
    /*!设置模式
    @param[in] stroke true为线框模式 false为填充模式
    @note 默认为填充模式
    */
    virtual void setStroke(bool stroke) = 0;
    /*!设置线框模式效果
    @param[in] count number of elements in the intervals array
    @param[in] intervals array containing an even number of entries (>=2), with
         the even indices specifying the length of "on" intervals, and the odd
         indices specifying the length of "off" intervals
    @param[in] offset offset into the intervals array 
         (mod the sum of all of the intervals).
    @note 仅线框模式下有效，目前仅支持dash效果
    @see BonesShape::setStroke
    */
    virtual void setStrokeEffect(size_t count, 
                                 BonesScalar * intervals, 
                                 BonesScalar offset) = 0;
    /*!设置线框模式下线框的粗细
    @param[in] stroke_width 线框宽度
    @note 仅线框模式下有效
    @see BonesShape::setStroke
    */
    virtual void setStrokeWidth(BonesScalar stroke_width) = 0;
    /*!设置颜色
    @param[in] color 颜色
    @warning shader 和 color 不能同时起效，后设置的为准
    */
    virtual void setColor(BonesColor color) = 0;
    /*!设置颜色
    @param[in] shader 目前只支持渐变色
    @note 必须通过BonesCore::createLinearGradient 或者createRadialGradient 创建
    @warning shader 和 color 不能同时使用，后设置的有效
    @see BonesCore
    */
    virtual void setColor(BonesShader shader) = 0;
    /*!设置几何形状为圆形
    @param[in] center 圆心
    @param[in] radius 半径
    @warning 几何形状的设置不能同时起效 以最后设置的为准
    */
    virtual void setCircle(const BonesPoint & center, BonesScalar radius) = 0;
    /*!设置几何形状为矩形
    @param[in] rx 矩形圆角 x方向上的半径
    @param[in] ry 矩形圆角 y方向上的半径
    @param[in] rect 指向一个矩形区域，为0则矩形区域为整个标签
    @note rx ry为0时代表矩形是一个直角矩形
    @warning 几何形状的设置不能同时起效 以最后设置的为准
    */
    virtual void setRect(BonesScalar rx, BonesScalar ry, 
                         const BonesRect * rect) = 0;
    /*!设置几何形状为线段
    @param[in] start 线段起点
    @param[in] end 线段终点
    @warning 几何形状的设置不能同时起效 以最后设置的为准
    */
    virtual void setLine(const BonesPoint & start, const BonesPoint & end) = 0;
    /*!设置几何形状为点
    @param[in] pt 点的位置
    @warning 几何形状的设置不能同时起效 以最后设置的为准
    */
    virtual void setPoint(const BonesPoint & pt) = 0;
    /*!设置几何形状为椭圆
    @param[in] oval 指向椭圆的外接矩形 为空则为标签的尺寸
    @warning 几何形状的设置不能同时起效 以最后设置的为准
    */
    virtual void setOval(const BonesRect * oval) = 0;
    /*!设置几何形状为弧线
    @param[in] start 起始角度
    @param[in] sweep 弧线旋转角度
    @param[in] use_center true 绘制的图形包含中心点 false 不包含
    @param[in] oval 指向定义弧线的矩形 为空则为标签的尺寸
    @warning 几何形状的设置不能同时起效 以最后设置的为准
    */
    virtual void setArc(BonesScalar start, 
                        BonesScalar sweep, 
                        bool use_center, 
                        const BonesRect * oval) = 0;
    /*!设置鼠标事件回调
    @param[in] phase 事件阶段 仅监听指定阶段的事件
    @param[in] lis 事件监听接口
    @see BonesEvent::Phase
    @see BonesHandler::MouseListener
    */
    virtual void setListener(BonesEvent::Phase phase, MouseListener * lis) = 0;
    /*!设置键盘事件回调
    @param[in] phase 事件阶段 仅监听指定阶段的事件
    @param[in] lis 事件监听接口
    @see BonesEvent::Phase
    @see BonesHandler::KeyListener
    */
    virtual void setListener(BonesEvent::Phase phase, KeyListener * lis) = 0;
    /*!设置焦点事件回调
    @param[in] phase 事件阶段 仅监听指定阶段的事件
    @param[in] lis 事件监听接口
    @see BonesEvent::Phase
    @see BonesHandler::FocusListener
    */
    virtual void setListener(BonesEvent::Phase phase, FocusListener * lis) = 0;
    /*!设置滚轮事件回调
    @param[in] phase 事件阶段 仅监听指定阶段的事件
    @param[in] lis 事件监听接口
    @see BonesEvent::Phase
    @see BonesHandler::WheelListener
    */
    virtual void setListener(BonesEvent::Phase phase, WheelListener * lis) = 0;
    /*!设置通知事件回调
    @param[in] lis 事件监听接口
    @see BonesHandler::NotifyListener
    */
    virtual void setListener(NotifyListener * lis) = 0;
};

/*!image标签 提供了对位图的处理*/
class BonesImage : public BonesHandler<BonesImage>
{
public:
    /*!将位图绘制到指定位置 不对位图进行缩放操作
      @param[in] bp 位图左上角的位置 如果为空则将位图绘制到(0, 0)
      @see BonesPoint
    */
    virtual void setDirect(const BonesPoint * bp) = 0;
    /*!将位图缩放绘制到指定区域 
      @param[in] dst 绘制位图的目标区域 如果为空则目标区域就是BonesImage大小
      @see BoensRect
    */
    virtual void setStretch(const BonesRect * dst) = 0;
    /*!将位图九宫格缩放绘制到指定区域
       @param[in] dst 绘制位图的目标区域 如果为空则目标区域就是BonesImage大小
       @param[in] center 九宫格拉伸时位图的中心区域 如果为空 则center为位图的(1/3, 1/3, 2/3, 2/3)
       @see BonesRect
    */
    virtual void setNine(const BonesRect * dst, const BonesRect * center) = 0;
    /*!设置位图
       @param[in] pm 被绘制的位图
       @see BonesPixmap
    */
    virtual void setContent(const BonesPixmap & pm) = 0;
    /*!设置位图
      @param[in] key 被绘制的位图在资源管理器里的key
      @see BonesResManager
    */
    virtual void setContent(const char * key) = 0;
    /*!指定位置的像素是否透明
    @param[in] x 位图x方向的偏移
    @param[in] y 位图y方向的偏移
    @note 只判断位图的像素是否透明，不受opacity的影响
    */
    virtual bool isTransparent(int x, int y) = 0;
    /*!设置颜色矩阵
      @param[in] cm 颜色矩阵 如果为空则清空当前使用的颜色矩阵
      @see BonesColorMatrix
    */
    virtual void setColorMatrix(const BonesColorMatrix * cm) = 0;
    /*!设置鼠标事件回调
      @param[in] phase 事件阶段 仅监听指定阶段的事件
      @param[in] lis 事件监听接口
      @see BonesEvent::Phase
      @see BonesHandler::MouseListener
    */
    virtual void setListener(BonesEvent::Phase phase, MouseListener * lis) = 0;
    /*!设置键盘事件回调
    @param[in] phase 事件阶段 仅监听指定阶段的事件
    @param[in] lis 事件监听接口
    @see BonesEvent::Phase
    @see BonesHandler::KeyListener
    */
    virtual void setListener(BonesEvent::Phase phase, KeyListener * lis) = 0;
    /*!设置焦点事件回调
    @param[in] phase 事件阶段 仅监听指定阶段的事件
    @param[in] lis 事件监听接口
    @see BonesEvent::Phase
    @see BonesHandler::FocusListener
    */
    virtual void setListener(BonesEvent::Phase phase, FocusListener * lis) = 0;
    /*!设置滚轮事件回调
    @param[in] phase 事件阶段 仅监听指定阶段的事件
    @param[in] lis 事件监听接口
    @see BonesEvent::Phase
    @see BonesHandler::WheelListener
    */
    virtual void setListener(BonesEvent::Phase phase, WheelListener * lis) = 0;
    /*!设置通知事件回调
    @param[in] lis 事件监听接口
    @see BonesHandler::NotifyListener
    */
    virtual void setListener(NotifyListener * lis) = 0;
};

class BonesText : public BonesHandler<BonesText>
{
public:
    /*!文字自动布局时水平方向对齐方式*/
    enum Align
    {
        kLeft = 0,//!<左对齐
        kCenter,//!<中间对齐
        kRight,//!<右对齐
    };
    /*!文字自动布局时单行超出时的处理方式*/
    enum OverFlow
    {
        kNone = 0,//!<超长不特殊处理即截断不显示
        kWordWrap,//!<超长自动换行
        kEllipsis,//!<超长以省略号代替
    };
public:
    /*!设置文字字体
    @param[in] font 字体
    */
    virtual void setFont(const BonesFont & font) = 0;
    /*!设置文字颜色
    @param[in] color 文字颜色
    */
    virtual void setColor(BonesColor color) = 0;
    /*!设置文字颜色
    @param[in] shader 文字颜色为渐变色
    */
    virtual void setColor(BonesShader shader) = 0;
    /*!设置文字
    @param[in] str 字符串 支持\n换行
    @param[in] align 字符串水平方向的对齐方式
    @param[in] of 单行字符串超出后的处理方式
    @warning set*Content不能同时起效 以最后设置的为准
    */
    virtual void setAutoContent(const wchar_t * str, Align align, OverFlow of) = 0;
    /*!设置文字
    @param[in] str 字符串 不支持\n换行
    @param[in] pts 字符串中每个字符显示的位置, pts的数组长度应该和str的字符长度相同
    @warning set*Content不能同时起效 以最后设置的为准
    */
    virtual void setPosContent(const wchar_t * str, const BonesPoint * pts) = 0;
    /*!设置鼠标事件回调
    @param[in] phase 事件阶段 仅监听指定阶段的事件
    @param[in] lis 事件监听接口
    @see BonesEvent::Phase
    @see BonesHandler::MouseListener
    */
    virtual void setListener(BonesEvent::Phase phase, MouseListener * lis) = 0;
    /*!设置键盘事件回调
    @param[in] phase 事件阶段 仅监听指定阶段的事件
    @param[in] lis 事件监听接口
    @see BonesEvent::Phase
    @see BonesHandler::KeyListener
    */
    virtual void setListener(BonesEvent::Phase phase, KeyListener * lis) = 0;
    /*!设置焦点事件回调
    @param[in] phase 事件阶段 仅监听指定阶段的事件
    @param[in] lis 事件监听接口
    @see BonesEvent::Phase
    @see BonesHandler::FocusListener
    */
    virtual void setListener(BonesEvent::Phase phase, FocusListener * lis) = 0;
    /*!设置滚轮事件回调
    @param[in] phase 事件阶段 仅监听指定阶段的事件
    @param[in] lis 事件监听接口
    @see BonesEvent::Phase
    @see BonesHandler::WheelListener
    */
    virtual void setListener(BonesEvent::Phase phase, WheelListener * lis) = 0;
    /*!设置通知事件回调
    @param[in] lis 事件监听接口
    @see BonesHandler::NotifyListener
    */
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
        TileMode mode, 
        size_t count, BonesColor * color,
        BonesScalar * pos) = 0;

    virtual BonesShader createRadialGradient(
        const BonesPoint & center, 
        BonesScalar radius,
        TileMode mode,
        size_t count, BonesColor * color,
        float * pos) = 0;

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