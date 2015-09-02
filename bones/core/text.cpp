#include "text.h"
#include "encoding.h"
#include "helper.h"

#include "SkCanvas.h"
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

static void AdjustSkPaint(SkPaint & paint, Color color, float opacity, Text::Align align)
{
    paint.setColor(color);
    paint.setAlpha(ClampAlpha(opacity, ColorGetA(color)));

    SkPaint::Align skalign;
    switch (align)
    {
    case Text::kLeft:
        skalign = SkPaint::kLeft_Align;
        break;
    case Text::kCenter:
        skalign = SkPaint::kCenter_Align;
        break;
    case Text::kRight:
        skalign = SkPaint::kRight_Align;
        break;
    default:
        assert(0);
        skalign = SkPaint::kLeft_Align;
        break;
    }
    paint.setTextAlign(skalign);
}

Text::Text()
:cache_dirty_(false), of_(kNone), text_color_(0xff000000),
text_align_(kCenter)
{
    setFocusable(false);
}

const char * Text::getClassName() const
{
    return kClassText;
}

void Text::set(const wchar_t * text)
{
    content_.clear();
    if (text)
        content_ = text;
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
    text_color_ = c;
    inval();
}

void Text::setAlign(Align align)
{
    text_align_ = align;
    inval();
}

void Text::setOverflow(Overflow of)
{
    if (of_ != of)
    {
        of_ = of;
        cache_dirty_ = true;
        inval();
    }
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
    AdjustSkPaint(paint, text_color_, opacity, text_align_);
    auto line_height = GetTextHeight(paint);
    auto total_height = line_height * lines_.size();
    //垂直居中
    SkPaint::FontMetrics fm;
    paint.getFontMetrics(&fm);
    auto base_line = getHeight() / 2 - total_height / 2 - fm.fTop;
    Scalar x = 0;
    switch (text_align_)
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
        base_line += line_height;
    }
}



void Text::onSizeChanged()
{
    cache_dirty_ = true;
}

bool Text::onHitTest(const Point & pt)
{
    return false;
}

void Text::adjustCache()
{
    if (!cache_dirty_)
        return;
    cache_dirty_ = false;
    lines_.clear();
    if (content_.empty())
        return;
    breakToLine();
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
    AdjustSkPaint(paint, text_color_, 1.0f, text_align_);

    std::vector<Scalar> widths;
    widths.resize(length);
    paint.getTextWidths(content_.data() + begin, sizeof(wchar_t)* length, &widths[0]);
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
    AdjustSkPaint(paint, text_color_, 1.0f, text_align_);

    std::vector<Scalar> widths;
    widths.resize(length);
    paint.getTextWidths(content_.data() + begin, sizeof(wchar_t)* length, &widths[0]);
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
BONES_CSS_SET_FUNC("content", &Text::setContent)
BONES_CSS_SET_FUNC("overflow", &Text::setOverflow)
BONES_CSS_SET_FUNC("font", &Text::setFont)
BONES_CSS_SET_FUNC("align", &Text::setAlign)
BONES_CSS_TABLE_END()

void Text::setColor(const CSSParams & params)
{
    if (params.empty())
        return;
    setColor(CSSUtils::CSSStrToColor(params[0]));
}

void Text::setContent(const CSSParams & params)
{
    if (params.empty())
        return;
    CSSText content(params[0]);
    set(Encoding::FromUTF8(content.begin, content.length).data());
}

void Text::setOverflow(const CSSParams & params)
{
    if (params.empty())
        return;
    auto & str = params[0];
    Overflow of = kNone;
    if (str == "word-wrap")
        of = kWordWrap;
    else if (str == "ellipsis")
        of = kEllipsis;
    setOverflow(of);
}

void Text::setFont(const CSSParams & params)
{
    if (params.empty())
        return;
    setFont(CSSUtils::CSSParamsToFont(params));
}

void Text::setAlign(const CSSParams & params)
{
    if (params.empty())
        return;
    Text::Align align = kLeft;
    auto & str = params[0];
    if (str == "center")
        align = Text::kCenter;
    else if (str == "right")
        align = Text::kRight;

    setAlign(align);
}

}