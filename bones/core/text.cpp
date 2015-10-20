#include "text.h"
#include "encoding.h"
#include "helper.h"
#include "rect.h"
#include "SkCanvas.h"
#include "SkShader.h"

#include "css_utils.h"


namespace bones
{

static const wchar_t * kStrEllipsis = L"...";

static Scalar GetTextHeight(SkPaint & paint)
{
    SkPaint::FontMetrics fm;
    paint.getFontMetrics(&fm);
    return fm.fBottom - fm.fTop;
}

static SkPaint::Align ToSkPaintStyle(Text::Align align)
{
    SkPaint::Align skalign = SkPaint::kLeft_Align;;
    switch (align)
    {
    case Text::kCenter:
        skalign = SkPaint::kCenter_Align;
        break;
    case Text::kRight:
        skalign = SkPaint::kRight_Align;
        break;
    }
    return skalign;
}

Text::Text()
:delegate_(nullptr), cache_dirty_(false), of_(kNone), align_(kCenter),
color_(BONES_RGB_BLACK), shader_(nullptr), mode_(kAuto), line_space_(0),
pts_(nullptr), path_(nullptr)
{

}

Text::~Text()
{
    if (shader_)
        shader_->unref();
    if (pts_)
        delete pts_;
    if (path_)
        delete path_;
}

void Text::setDelegate(Delegate * delegate)
{
    delegate_ = delegate;
}

Rect Text::getAutoBounds() const
{
    Rect bounds;
    if (!content_.empty() && kAuto == mode_)
    {
        SkPaint paint;
        Helper::ToSkPaint(font_, paint);
        paint.setColor(color_);
        paint.setShader(shader_);
        paint.setTextAlign(ToSkPaintStyle(align_));

        Scalar max_width = 0;
        Scalar width = 0;
        size_t line_size = 0;
        size_t i = 0;
        size_t j = 0;
        for (; i < content_.size(); ++i)
        {
            if ('\n' == content_[i])
            {
                width = paint.measureText(content_.data() + j, 
                    (i - j) * sizeof(content_[0]));
                if (width > max_width)
                    max_width = width;
                line_size++;
                j = i + 1;
            }
        }
        width = paint.measureText(content_.data() + j, 
            (i - j) * sizeof(content_[0]));
        if (width > max_width)
            max_width = width;
        line_size++;

        Scalar height = GetTextHeight(paint) * line_size +
            line_space_ * (line_size - 1);
        bounds.setXYWH(0, 0, max_width, height);
    }
    return bounds;
}

const char * Text::getClassName() const
{
    return kClassText;
}

Text::DelegateBase * Text::delegate()
{
    return delegate_;
}

void Text::set(const wchar_t * text)
{
    content_.clear();
    if (text)
        content_ = text;

    cache_dirty_ = true;
    inval();
}

void Text::setAuto(Align align, Overflow of, Scalar space)
{
    mode_ = kAuto;

    of_ = of;
    align_ = align;
    line_space_ = space;
    cache_dirty_ = true;
    inval();
}

void Text::setPos(const Point * ps, size_t count)
{
    mode_ = kPos;

    if (!pts_)
        pts_ = new std::vector<SkPoint>;
    else
        pts_->clear();

    if (ps)
    {
        for (size_t i = 0; i < count; ++i)
        {
            SkPoint pt = { ps[i].x(), ps[i].y() };
            pts_->push_back(pt);
        }
    }
    cache_dirty_ = true;
    inval();
}

void Text::setPath(const SkPath & path)
{
    mode_ = kPath;

    if (!path_)
        path_ = new SkPath;
    *path_ = path;

    cache_dirty_ = true;
    inval();
}

void Text::setFont(const Font & font)
{
    font_ = font;
    cache_dirty_ = true;
    inval();
}

void Text::setColor(Color c)
{
    //使用简单颜色
    color_ = c;
    if (shader_)
        shader_->unref();
    shader_ = nullptr;

    inval();
}

void Text::setColor(SkShader * shader)
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

void Text::onDraw(SkCanvas & canvas, const Rect & inval, float opacity)
{
    adjustCache();

    if (content_.empty())
        return;

    if (opacity == 0)
        return;

    SkPaint paint;
    Helper::ToSkPaint(font_, paint);
    paint.setColor(color_);
    paint.setShader(shader_);
    paint.setAlpha(ClampAlpha(opacity, ColorGetA(color_)));
    paint.setTextAlign(ToSkPaintStyle(align_));

    if (kAuto == mode_)
        drawAuto(canvas, paint);
    else if (kPos == mode_)
        drawPos(canvas, paint);
    else if (kPath == mode_)
        drawPath(canvas, paint);
}

void Text::drawPos(SkCanvas & canvas, SkPaint & paint)
{
    if (content_.size() != pts_->size())
        return;
    canvas.drawPosText(content_.data(), 
        content_.size() * sizeof(content_[0]), &(*pts_)[0], paint);
}

void Text::drawPath(SkCanvas & canvas, SkPaint & paint)
{
    canvas.drawPath(*path_, paint);
}

void Text::drawAuto(SkCanvas & canvas, SkPaint & paint)
{
    auto line_height = GetTextHeight(paint);
    auto total_height = line_height * lines_.size() + 
        line_space_ * (lines_.size() - 1);
    //垂直居中
    SkPaint::FontMetrics fm;
    paint.getFontMetrics(&fm);
    auto base_line = getHeight() / 2 - total_height / 2 - fm.fTop;
    Scalar x = 0;
    switch (align_)
    {
    case kRight:
        x = getWidth();
        break;
    case kCenter:
        x = getWidth() / 2;
        break;
    case kLeft:
        x = 0;
        break;
    default:
        assert(0);
        break;
    }
    for (auto iter = lines_.begin(); iter != lines_.end(); ++iter)
    {
        auto bytelength = iter->size() * sizeof(wchar_t);
        if (bytelength != 0)
            canvas.drawText(iter->data(), bytelength, x, base_line, paint);
        base_line += line_height + line_space_;
    }
}



void Text::onSizeChanged()
{
    if (kAuto == mode_)
        cache_dirty_ = true;
    Area::onSizeChanged();
}

void Text::adjustCache()
{
    if (!cache_dirty_)
        return;
    cache_dirty_ = false;
    lines_.clear();
    if (content_.empty())
        return;
    if (kAuto == mode_)
        breakToLine();
    //kPos kPath不需要计算缓存
}

void Text::breakToLine()
{
    //支持\n换行
    size_t i = 0;
    size_t j = 0;
    for (; i < content_.size(); ++i)
    {
        if ('\n' == content_[i])
        {
            if (kEllipsis == of_)
                appendEllipsis(j, i - j);
            else if (kWordWrap == of_)
                wordWrap(j, i - j);
            else
                lines_.push_back(std::wstring(content_.data() + j, i - j));

            j = i + 1;
        }
    }

    if (kEllipsis == of_)
        appendEllipsis(j, i - j);
    else if (kWordWrap == of_)
        wordWrap(j, i - j);
    else
        lines_.push_back(std::wstring(content_.data() + j, i - j));
}

void Text::appendEllipsis(size_t begin, size_t length)
{
    if (0 == length)
    {
        lines_.push_back(std::wstring());
        return;
    }
    SkPaint paint;
    Helper::ToSkPaint(font_, paint);
    paint.setColor(color_);
    paint.setShader(shader_);
    paint.setTextAlign(ToSkPaintStyle(align_));

    std::vector<Scalar> widths;
    widths.resize(length);
    paint.getTextWidths(content_.data() + begin,
        sizeof(wchar_t)* length, &widths[0]);
    Scalar line_width = 0;
    Scalar max_width = getWidth();
    
    for (auto iter = widths.begin(); iter != widths.end(); ++iter)
        line_width += *iter;
    if (line_width <= max_width)
        lines_.push_back(std::wstring(content_.data() + begin, length));
    else
    {
        auto ellipsis_length = paint.measureText(kStrEllipsis,
                              sizeof(kStrEllipsis[0]) * wcslen(kStrEllipsis));
        if (ellipsis_length > max_width)
            lines_.push_back(kStrEllipsis);
        else
        {
            max_width -= ellipsis_length;
            line_width = 0;
            auto iter = widths.begin();
            for (; iter != widths.end(); ++iter)
            {
                line_width += *iter;
                if (line_width > max_width)
                    break;
            }
            lines_.push_back(std::wstring());
            lines_.back().append(content_.data() + begin, iter - widths.begin());
            lines_.back().append(kStrEllipsis);
        }
    }
       
}

void Text::wordWrap(size_t begin, size_t length)
{
    SkPaint paint;
    Helper::ToSkPaint(font_, paint);
    paint.setColor(color_);
    paint.setShader(shader_);
    paint.setTextAlign(ToSkPaintStyle(align_));

    std::vector<Scalar> widths;
    widths.resize(length);
    paint.getTextWidths(content_.data() + begin,
        sizeof(wchar_t)* length, &widths[0]);
    Scalar line_width = 0;
    Scalar max_width = getWidth();
    size_t line_start = 0;
    for (size_t i = 0; i < widths.size();  ++i)
    {
        line_width += widths[i];
        if (line_width > max_width)
        {
            size_t line_length = i - line_start;
            std::wstring line;
            if (0 == line_length )
            {
                line.append(content_.data() + begin + line_start, 1);
                line_start = i + 1; 
                line_width = 0;
            }
            else if (line_length > 0)
            {
                line.append(content_.data() + begin + line_start, line_length);
                line_start = i;
                line_width = widths[i];
            }
            lines_.push_back(line);                
        }
    }

    size_t line_length = widths.size() - line_start;
    if (0 != line_length)
    {
        std::wstring line;
        line.append(content_.data() + begin + line_start, line_length);
        lines_.push_back(line);
    }
        
}

BONES_CSS_TABLE_BEGIN(Text, View)
BONES_CSS_SET_FUNC("color", &Text::setColor)
BONES_CSS_SET_FUNC("font", &Text::setFont)
BONES_CSS_TABLE_END()

void Text::setColor(const CSSParams & params)
{
    if (params.empty())
        return;
    setColor(CSSUtils::CSSStrToColor(params[0]));
}

//void Text::setContent(const CSSParams & params)
//{
//    if (params.empty())
//        return;
//    CSSText content(params[0]);
//    set(Encoding::FromUTF8(content.begin, content.length).data());
//}
void Text::setFont(const CSSParams & params)
{
    if (params.empty())
        return;
    setFont(CSSUtils::CSSParamsToFont(params));
}


}