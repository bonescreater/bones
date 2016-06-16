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

Text::Text(ThreadContext & context)
:Area(context), delegate_(nullptr), cache_dirty_(false), align_(kCenter),
color_(BONES_RGB_BLACK), shader_(nullptr), mode_(kAuto), line_space_(0),
pts_(nullptr), path_(nullptr), ellipsis_(false)
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

Rect Text::getFloatBounds(Scalar max_width) const
{
    Rect bounds;
    if (!content_.empty() && kFloat == mode_)
    {
        SkPaint paint;
        ToSkPaint(paint);
        bool max_width_enable = max_width > 0;
        Scalar bounds_height = 0;
        Scalar bounds_width = 0;        
        Scalar width = 0;
        if (!max_width_enable)
        {//如果没有设置最大宽度
            size_t i = 0;
            size_t j = 0;
            Scalar line_size = 0;
            for (; i < content_.size(); ++i)
            {
                if ('\n' == content_[i])
                {
                    width = paint.measureText(content_.data() + j, 
                        (i - j) * sizeof(content_[0]));
                    if (bounds_width < width)
                        bounds_width = width;
                    line_size++;

                    j = i + 1;
                }
            }
            width = paint.measureText(content_.data() + j,
                (i - j) * sizeof(content_[0]));
            if (bounds_width < width)
                bounds_width = width;
            line_size++;

            bounds_height = GetTextHeight(paint) * line_size +
                line_space_ * (line_size - 1);
            
        }
        else
        {//设置了最大宽度
            Lines lines;
            size_t i = 0;
            size_t j = 0;
            for (; i < content_.size(); ++i)
            {
                if ('\n' == content_[i])
                {
                    width = wordWrap(j, i - j, max_width, lines);
                    if (bounds_width < width)
                        bounds_width = width;

                    j = i + 1;
                }
            }
            width = wordWrap(j, i - j, max_width, lines);
            if (bounds_width < width)
                bounds_width = width;
            bounds_height = GetTextHeight(paint) * lines_.size() +
                line_space_ * (lines_.size() - 1);
        }
        bounds.setXYWH(0, 0, bounds_width, bounds_height);
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

void Text::setLineSpace(Scalar space)
{
    line_space_ = space;
    cache_dirty_ = true;
    inval();
}

void Text::setAuto(Align align, bool ellipsis)
{
    mode_ = kAuto;

    ellipsis_ = ellipsis;
    align_ = align;
    
    cache_dirty_ = true;
    inval();
}

void Text::setFloat()
{
    mode_ = kFloat;
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
    ToSkPaint(paint);
    paint.setAlpha(ClampAlpha(opacity, ColorGetA(color_)));

    if (kAuto == mode_)
        drawAuto(canvas, paint);
    else if (kFloat == mode_)
        drawFloat(canvas, paint);
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
        auto bytelength = iter->size() * sizeof(content_[0]);
        if (bytelength != 0)
            canvas.drawText(iter->data(), bytelength, x, base_line, paint);
        base_line += line_height + line_space_;
    }
}

void Text::drawFloat(SkCanvas & canvas, SkPaint & paint)
{
    SkPaint::FontMetrics fm;
    paint.getFontMetrics(&fm);
    auto line_height = GetTextHeight(paint);
    auto base_line = 0 - fm.fTop;
    
    for (size_t i = 0; i < lines_.size(); ++i)
    {
        auto & line = lines_[i];
        auto bytelength = line.size() * sizeof(content_[0]);

        if (bytelength != 0)
            canvas.drawText(line.data(), bytelength, 0, base_line, paint);

        base_line += line_height + line_space_;
    }
}



void Text::onSizeChanged()
{
    if (kAuto == mode_ || kFloat == mode_)
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
        adjustEllipsisCache();
    else if (kFloat == mode_)
        adjustWordWrapCache();
    //kPos kPath不需要计算缓存
}

void Text::adjustWordWrapCache()
{
    //支持\n换行
    size_t i = 0;
    size_t j = 0;
    for (; i < content_.size(); ++i)
    {
        if ('\n' == content_[i])
        {
            wordWrap(j, i - j, getWidth(), lines_);
            j = i + 1;
        }
    }
    wordWrap(j, i - j, getWidth(), lines_);
}

void Text::adjustEllipsisCache()
{
    //支持\n换行
    size_t i = 0;
    size_t j = 0;
    for (; i < content_.size(); ++i)
    {
        if ('\n' == content_[i])
        {
            if (ellipsis_)
                appendEllipsis(j, i - j);
            else
                lines_.push_back(std::wstring(content_.data() + j, i - j));
            j = i + 1;
        }
    }
    if (ellipsis_)
        appendEllipsis(j, i - j);
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
    ToSkPaint(paint);

    std::vector<Scalar> widths;
    widths.resize(length);
    paint.getTextWidths(content_.data() + begin,
        sizeof(content_[0])* length, &widths[0]);
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

Scalar Text::wordWrap(size_t begin, size_t length,
    Scalar max_width, Lines & lines) const
{
    if (max_width <= 0)
        return 0;

    bool word_wrap = false;

    Scalar my_max_width = max_width;
    SkPaint paint;
    ToSkPaint(paint);

    std::vector<Scalar> widths;
    widths.resize(length);
    paint.getTextWidths(content_.data() + begin,
        sizeof(content_[0])* length, &widths[0]);
    Scalar line_width = 0;

    size_t line_start = 0;
    for (size_t i = 0; i < widths.size();  ++i)
    {
        line_width += widths[i];
        if (line_width > my_max_width)
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
            lines.push_back(line);
            word_wrap = true;
        }
    }

    size_t line_length = widths.size() - line_start;
    if (0 != line_length)
    {
        std::wstring line;
        line.append(content_.data() + begin + line_start, line_length);
        lines.push_back(line);
    }
    return word_wrap ? max_width : line_width;
}

void Text::ToSkPaint(SkPaint & paint) const
{
    Helper::ToSkPaint(font_, paint);
    paint.setColor(color_);
    paint.setShader(shader_);
    if (kAuto == mode_)
        paint.setTextAlign(ToSkPaintStyle(align_));
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