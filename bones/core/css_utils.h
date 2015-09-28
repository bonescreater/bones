#ifndef BONES_CORE_CSS_UTILS_H_
#define BONES_CORE_CSS_UTILS_H_

#include "core.h"
#include "css_types.h"

namespace bones
{

//CSSText主要是方便文本处理转义字符
class CSSText : public CSSString
{
public:
    CSSText(const CSSString & src);
private:
    std::vector<char> data_;
};

class Rect;
class Point;
class Pixmap;
class Font;

class CSSUtils
{
public:
    static Scalar CSSStrToPX(const CSSString & str);

    static Rect CSSStrToPX(const CSSString & left, 
                           const CSSString & top,
                           const CSSString & right,
                           const CSSString & bottom);

    static Point CSSStrToPX(const CSSString & x, const CSSString & y);

    static Cursor CSSStrToCursor(const CSSString & str);

    static Pixmap CSSStrToPixmap(const CSSString & str);

    static Color  CSSStrToColor(const CSSString & str);

    static float CSSStrToFloat(const CSSString & str);

    static Scalar CSSStrToScalar(const CSSString & str);

    //static Shader CSSParamsToLinearGradientShader(const CSSParams & params);

    //static Shader CSSParamsToRadialGradientShader(const CSSParams & params);

    static Font CSSParamsToFont(const CSSParams & params);
};


}



#endif