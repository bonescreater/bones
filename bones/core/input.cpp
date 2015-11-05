#include "input.h"
#include "helper.h"
#include "SkShader.h"
#include "SkCanvas.h"


namespace bones
{

Input::Input()
:delegate_(nullptr), color_(BONES_RGB_BLACK), shader_(nullptr),
password_(L'*'), cache_dirty_(false), pw_valid(false)
{

}

Input::~Input()
{
    if (shader_)
        shader_->unref();
}

void Input::setColor(Color c)
{
    //使用简单颜色
    color_ = c;
    if (shader_)
        shader_->unref();
    shader_ = nullptr;

    inval();
}

void Input::setColor(SkShader * shader)
{
    //使用渐变色  渐变色的alpha受到paint控制
    if (shader_)
        shader_->unref();
    shader_ = shader;
    if (shader_)
        shader_->ref();
    color_ = BONES_RGB_BLACK;

    inval();
}

void Input::setFont(const Font & font)
{
    font_ = font;
    cache_dirty_ = true;
    inval();
}

void Input::set(const wchar_t * text)
{
    content_.clear();
    if (text)
        content_ = text;
    cache_dirty_ = true;
    inval();
}

void Input::setPassword(bool pw, wchar_t password)
{
    pw_valid = pw;
    password_ = password;
    cache_dirty_ = true;
    inval();
}

void Input::setDelegate(Delegate * delegate)
{
    delegate_ = delegate;
}

const char * Input::getClassName() const
{
    return kClassInput;
}

Input::DelegateBase * Input::delegate()
{
    return delegate_;
}

void Input::onDraw(SkCanvas & canvas, const Rect & inval, float opacity)
{
    adjustCache();

    if (content_.empty())
        return;

    if (opacity == 0)
        return;
    SkPaint paint;
    ToSkPaint(paint);
    paint.setAlpha(ClampAlpha(opacity, ColorGetA(color_)));
    //绘制单个字符
    Scalar left = 0;
    //垂直居中
    SkPaint::FontMetrics fm;
    paint.getFontMetrics(&fm);
    Scalar text_height = fm.fBottom - fm.fTop;
    Scalar baseline = getHeight() / 2 - text_height / 2 - fm.fTop;

    for (size_t i = 0; i < content_.size(); ++i)
    {
        wchar_t ch = content_[i];
        if (pw_valid)
            ch = password_;

        canvas.drawText(&ch, sizeof(ch), left, baseline, paint);
        left += content_widths_[i];

    }
}


void Input::adjustCache()
{
    if (!cache_dirty_)
        return;
    cache_dirty_ = false;

    content_widths_.clear();
    if (content_.size())
    {
        auto size = content_.size();
        content_widths_.resize(size);
        SkPaint paint;
        ToSkPaint(paint);
        if (pw_valid)
        {//密码有效 则测量 密码字符串的宽
            std::wstring tmp;
            tmp.resize(size, password_);
            paint.getTextWidths(&tmp[0], sizeof(tmp[0]) * size,
                &content_widths_[0]);
        }
        else
            paint.getTextWidths(&content_[0], sizeof(content_[0]) * size,
                &content_widths_[0]);
    }
}

void Input::ToSkPaint(SkPaint & paint) const
{
    Helper::ToSkPaint(font_, paint);
    paint.setColor(color_);
    paint.setShader(shader_);
}

}