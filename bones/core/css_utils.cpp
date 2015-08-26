#include "css_utils.h"
#include "rect.h"
#include "res_manager.h"

namespace bones
{


CSSText::CSSText(const CSSString & src)
{
    size_t i = 0;
    while (i < src.length)
    {
        if ('\\' == src.begin[i])
            break;
        i++;
    }
    data_.resize(src.length + 1, 0);
    begin = &data_[0];
    memcpy(&data_[0], src.begin, i);
    length = i;

    if (i != src.length)
    {//找到转义字符
        size_t j = i;
        while (i < src.length)
        {
            if ('\\' == src.begin[i])
            {
                i++;
                if ('n' == src.begin[i])
                    data_[j++] = 10;
                else if ('t' == src.begin[i])
                    data_[j++] = 9;
                else if ('\\' == src.begin[i])
                    data_[j++] = '\\';
                else if ('r' == src.begin[i])
                    continue;//\r跳过

                i++;
            }
            else
                data_[j++] = src.begin[i++];
        }
        length = j;
    }

}

//CSSString 指向的内容 一直保存在内存中
Scalar CSSUtils::CSSStrToPX(const CSSString & str)
{
    const char * begin = nullptr;
    size_t len = 0;
    if (!(begin = str.begin) || !(len = str.length))
        return 0;

    if (len < 3)
        return 0;
    if ('p' != begin[len - 2] || 'x' != begin[len - 1])
        return 0;
    //节省一次字符串复制
    char * cs = const_cast<char *>(begin);
    cs[len - 2] = '\0';
    auto f = static_cast<Scalar>(atof(begin));
    cs[len - 2] = 'p';
    return f;
}

Rect CSSUtils::CSSStrToPX(const CSSString & left,
    const CSSString & top,
    const CSSString & right,
    const CSSString & bottom)
{
    return Rect::MakeLTRB(CSSStrToPX(left),
        CSSStrToPX(top),
        CSSStrToPX(right),
        CSSStrToPX(bottom));
}

Point CSSUtils::CSSStrToPX(const CSSString & x, const CSSString & y)
{
    return Point::Make(CSSStrToPX(x), CSSStrToPX(y));
}



Scalar CSSUtils::CSSStrToScalar(const CSSString & str)
{
    const char * begin = nullptr;
    size_t len = 0;
    if (!(begin = str.begin) || !(len = str.length))
        return 0;
    char * css = const_cast<char *>(begin);
    char old = css[len];
    css[len] = '\0';
    auto f = static_cast<Scalar>(atof(begin));
    css[len] = old;
    return f;
}

float CSSUtils::CSSStrToFloat(const CSSString & str)
{
    const char * begin = nullptr;
    size_t len = 0;
    if (!(begin = str.begin) || !(len = str.length))
        return 0;
    char * css = const_cast<char *>(begin);
    char old = css[len];
    css[len] = '\0';
    auto f = static_cast<Scalar>(atof(begin));
    css[len] = old;
    return f;
}

Color CSSUtils::CSSStrToColor(const CSSString & str)
{
    const char * begin = nullptr;
    size_t len = 0;
    if (!(begin = str.begin) || !(len = str.length))
        return 0;

    if (len > 9 || len < 2)
        return 0;
    if ('#' != begin[0])
        return 0;
    int value = 0;
    auto missing_count = 9 - len;
    //颜色不足八位 以高位以f补足
    while (missing_count--)
    {
        value <<= 4;
        value += 15;
    }

    for (size_t i = 1; i < len; ++i)
    {
        value <<= 4;
        char c = tolower(begin[i]);
        if (c >= '0' && c <= '9')
            value += c - '0';
        else if (c >= 'a' && c <= 'f')
            value += 10 + c - 'a';
        else
            return 0;
    }
    return value;
}

Cursor CSSUtils::CSSStrToCursor(const CSSString & str)
{
    const wchar_t * cursor = nullptr;
    if (str == "arrow")
        cursor = IDC_ARROW;
    else if (str == "ibeam")
        cursor = IDC_IBEAM;
    else if (str == "wait")
        cursor = IDC_WAIT;
    else if (str == "cross")
        cursor = IDC_CROSS;
    else if (str == "up-arrow")
        cursor = IDC_UPARROW;
    else if (str == "size")
        cursor = IDC_SIZE;
    else if (str == "icon")
        cursor = IDC_ICON;
    else if (str == "size-nwse")
        cursor = IDC_SIZENWSE;
    else if (str == "size-nesw")
        cursor = IDC_SIZENESW;
    else if (str == "size-we")
        cursor = IDC_SIZEWE;
    else if (str == "size-ns")
        cursor = IDC_SIZENS;
    else if (str == "size-all")
        cursor = IDC_SIZEALL;
    else if (str == "no")
        cursor = IDC_NO;
    else if (str == "hand")
        cursor = IDC_HAND;
    else if (str == "app-starting")
        cursor = IDC_APPSTARTING;
    else if (str == "help")
        cursor = IDC_HELP;

    if (cursor)
        return ::LoadImage(NULL, cursor, IMAGE_CURSOR, 0, 0, LR_SHARED);
    return Core::GetResManager()->getCursor(std::string(str.begin, str.length).data());
}

Pixmap CSSUtils::CSSStrToPixmap(const CSSString & str)
{
    return Core::GetResManager()->getPixmap(std::string(str.begin, str.length).data());
}

static Shader::TileMode CSSStrToShaderTileMode(const CSSString & str)
{
    if (str == "mirror")
        return Shader::kMirror;
    else if (str == "repeat")
        return Shader::kRepeat;
    else
        return Shader::kClamp;
}

Shader CSSUtils::CSSParamsToLinearGradientShader(const CSSParams & params)
{
    //线性渐变至少6个参数
    Shader shader;
    if (params.size() < 6)
        return shader;

    Point pt[2] = { { CSSStrToPX(params[0]), CSSStrToPX(params[1]) },
    { CSSStrToPX(params[2]), CSSStrToPX(params[3]) } };
    auto m = CSSStrToShaderTileMode(params[4]);
    std::vector<Color> colors;
    std::vector<float> pos;
    if (params.size() == 6)
    {//处理只有1个颜色
        colors.push_back(CSSStrToColor(params[5]));
        pos.push_back(1.f);
    }
    else if (params.size() == 7)
    {//处理只有2个颜色
        colors.resize(2);
        pos.resize(2);
        colors[0] = CSSStrToColor(params[5]);
        pos[0] = 0.f;
        colors[1] = CSSStrToColor(params[6]);
        pos[1] = 1.f;
    }
    else
    {//处理多个颜色
        int count = (params.size() - 5) / 2;
        colors.resize(count);
        pos.resize(count);
        for (auto i = 0; i < count; i = i++)
        {
            int j = 2 * i;
            colors[i] = CSSStrToColor(params[5 + j]);
            pos[i] = CSSStrToFloat(params[5 + j + 1]);
        }
    }
    shader.setGradient(pt[0], pt[1], &colors[0], &pos[0], colors.size(), m);
    return shader;
}

Shader CSSUtils::CSSParamsToRadialGradientShader(const CSSParams & params)
{
    Shader shader;
    //至少5个参数
    if (params.size() < 5)
        return shader;

    Point pt = { CSSStrToPX(params[0]), CSSStrToPX(params[1]) };
    Scalar radius = CSSStrToScalar(params[2]);

    auto m = CSSStrToShaderTileMode(params[3]);

    std::vector<Color> colors;
    std::vector<float> pos;
    if (params.size() == 5)
    {//处理只有1个颜色
        colors.push_back(CSSStrToColor(params[5]));
        pos.push_back(1.f);
    }
    else if (params.size() == 6)
    {//处理只有2个颜色
        colors.resize(2);
        pos.resize(2);
        colors[0] = CSSStrToColor(params[5]);
        pos[0] = 0.f;
        colors[1] = CSSStrToColor(params[6]);
        pos[1] = 1.f;
    }
    else
    {//处理多个颜色
        int count = (params.size() - 4) / 2;
        colors.resize(count);
        pos.resize(count);
        for (auto i = 0; i < count; i = i++)
        {
            int j = 2 * i;
            colors[i] = CSSStrToColor(params[4 + j]);
            pos[i] = CSSStrToFloat(params[4 + j + 1]);
        }
    }
    shader.setGradient(pt, radius, &colors[0], &pos[0], colors.size(), m);
    return shader;
}

}