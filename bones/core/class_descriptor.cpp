#include "class_descriptor.h"
#include "image.h"
#include "text.h"
#include "shape.h"
#include "panel.h"
#include "encoding.h"
#include "logging.h"

namespace bones
{

static CSSString kDescLeft("left");
static CSSString kDescTop("top");
static CSSString kDescWidth("width");
static CSSString kDescHeight("height");
static CSSString kDescRight("right");
static CSSString kDescBottom("bottom");
static CSSString kDescOpacity("opacity");
static CSSString kDescColor("color");
static CSSString kDescLinearGradient("linear-gradient");
static CSSString kDescRadialGradient("radial-gradient");
static CSSString kDescContent("content");
//static CSSString kDescMode("mode");
//static CSSString kDescStyle("style");
static CSSString kDescStrokeWidth("stroke-width");
static CSSString kDescRect("rect");
static CSSString kDescCircle("circle");
static CSSString kDescBorder("border");
static CSSString kDescCursor("cursor");
static CSSString kDescHitTest("hit-test");
static CSSString kDescLayered("layered");
//CSSText主要是方便文本处理转义字符
class CSSText : public CSSString
{
public:
    CSSText(const CSSString & src)
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
private:
    std::vector<char> data_;
};

//CSSString 指向的内容 一直保存在内存中
static Scalar CSSStrToPX(const CSSString & str)
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

static int CSSStrToInt(const CSSString & str)
{
    const char * begin = nullptr;
    size_t len = 0;
    if (!(begin = str.begin) || !(len = str.length))
        return 0;
    char * css = const_cast<char *>(begin);
    char old = css[len];
    css[len] = '\0';
    auto f = atoi(begin);
    css[len] = old;
    return f;
}

static Scalar CSSStrToScalar(const CSSString & str)
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

static bool CSSStrToBool(const CSSString & str)
{
    if (str == "true")
        return true;
    else
        return false;
}

static float CSSStrToFloat(const CSSString & str)
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

static Color  CSSStrToColor(const CSSString & str)
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

static Cursor CSSStrToCursor(const CSSString & str)
{
    const wchar_t * cursor = nullptr;
    if (str == "arrow" || str == "default")
        cursor = IDC_ARROW;
    else if (str == "ibeam")
        cursor = IDC_IBEAM;
    else if (str == "wait")
        cursor = IDC_WAIT;
    else if (str == "cross")
        cursor = IDC_CROSS;
    else if (str == "uparrow")
        cursor = IDC_UPARROW;
    else if (str == "size")
        cursor = IDC_SIZE;
    else if (str == "icon")
        cursor = IDC_ICON;
    else if (str == "sizenwse")
        cursor = IDC_SIZENWSE;
    else if (str == "sizenesw")
        cursor = IDC_SIZENESW;
    else if (str == "sizewe")
        cursor = IDC_SIZEWE;
    else if (str == "sizens")
        cursor = IDC_SIZENS;
    else if (str == "sizeall")
        cursor = IDC_SIZEALL;
    else if (str == "no")
        cursor = IDC_NO;
    else if (str == "hand")
        cursor = IDC_HAND;
    else if (str == "appstarting")
        cursor = IDC_APPSTARTING;
    else if (str == "help")
        cursor = IDC_HELP;

    if (cursor)
        return ::LoadImage(NULL, cursor, IMAGE_CURSOR, 0, 0, LR_SHARED);
    return (Cursor)CSSStrToInt(str);
}

Shader::TileMode CSSStrToShaderTileMode(const CSSString & str)
{
    if (str == "mirror")
        return Shader::kMirror;
    else if (str == "repeat")
        return Shader::kRepeat;
    else
        return Shader::kClamp;
}

Shader CSSParamsToLinearGradientShader(const CSSParams & params)
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

Shader CSSParamsToRadialGradientShader(const CSSParams & params)
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

static void ViewSetLeft(Ref * ob, const CSSParams & params)
{
    if (params.empty() || !ob)
        return;
    auto v = static_cast<View *>(ob);
    v->setLeft(CSSStrToPX(params[0]));
}

static void ViewSetTop(Ref * ob, const CSSParams & params)
{
    if (params.empty() || !ob)
        return;
    auto v = static_cast<View *>(ob);
    v->setTop(CSSStrToPX(params[0]));
}

static void ViewSetWidth(Ref * ob, const CSSParams & params)
{
    if (params.empty() || !ob)
        return;
    auto v = static_cast<View *>(ob);
    v->setWidth(CSSStrToPX(params[0]));
}

static void ViewSetHeight(Ref * ob, const CSSParams & params)
{
    if (params.empty() || !ob)
        return;
    auto v = static_cast<View *>(ob);
    v->setHeight(CSSStrToPX(params[0]));
}

static void ViewSetCursor(Ref * ob, const CSSParams & params)
{
    if (params.empty() || !ob)
        return;
    auto v = static_cast<View *>(ob);
    v->setCursor(CSSStrToCursor(params[0]));
}

static void ShirtSetOpacity(Ref * ob, const CSSParams & params)
{
    if (params.empty() || !ob || params.size() < 1)
        return;
    auto v = static_cast<Shirt *>(ob);
    v->setOpacity(CSSStrToFloat(params[0]));
}
/*
text
*/
static void TextSetColor(Ref * ob, const CSSParams & params)
{
    if (params.empty() || !ob)
        return;
    auto v = static_cast<Text *>(ob);
    v->setColor(CSSStrToColor(params[0]));
}

static void TextSetContent(Ref * ob, const CSSParams & params)
{
    if (params.empty() || !ob)
        return;
    auto v = static_cast<Text *>(ob);
    CSSText content(params[0]);
    v->set(Encoding::FromUTF8(content.begin, content.length).data());
}

/*
Shape
*/
static Shape::Style CSSStrToShapeStyle(const CSSString & str)
{
    return Shape::kSolid;
}
//static void ShapeSetMode(Ref * ob, const CSSParams & params)
//{
//    if (params.empty() || !ob)
//        return;
//    auto v = static_cast<Shape *>(ob);
//    Shape::Mode m = Shape::kFill;
//    LOG_VERBOSE << "shape unsupport mode\n";
//    v->setMode(m);
//}

//static void ShapeSetStyle(Ref * ob, const CSSParams & params)
//{
//    if (params.empty() || !ob)
//        return;
//    auto v = static_cast<Shape *>(ob);
//    v->setStyle(CSSStrToShapeStyle(params[0]));
//}

static void ShapeSetColor(Ref * ob, const CSSParams & params)
{
    if (params.empty() || !ob)
        return;
    auto v = static_cast<Shape *>(ob);
    v->setColor(CSSStrToColor(params[0]));
}

static void ShapeSetLinearGradient(Ref * ob, const CSSParams & params)
{
    if (params.empty() || !ob)
        return;

    ((Shape *)ob)->setShader(CSSParamsToLinearGradientShader(params));
}

static void ShapeSetRadialGradient(Ref * ob, const CSSParams & params)
{
    if (params.empty() || !ob)
        return;
    ((Shape *)ob)->setShader(CSSParamsToRadialGradientShader(params));
}

static void ShapeSetStrokeWidth(Ref * ob, const CSSParams & params)
{
    if (params.empty() || !ob)
        return;
    auto v = static_cast<Shape *>(ob);
    v->setStrokeWidth(CSSStrToPX(params[0]));
}
//
static void ShapeSetRect(Ref * ob, const CSSParams & params)
{
    if (params.empty() || !ob || params.size() < 2)
        return;
    auto v = static_cast<Shape *>(ob);
    Rect * pr = nullptr;
    Rect r;
    if (params.size() >= 6)
    {
        r.setLTRB(CSSStrToPX(params[2]),
                  CSSStrToPX(params[3]),
                  CSSStrToPX(params[4]),
                  CSSStrToPX(params[5]));
        pr = &r;
    }
    v->set(CSSStrToPX(params[0]), CSSStrToPX(params[1]), pr);
}
//
static void ShapeSetCircle(Ref * ob, const CSSParams & params)
{
    if (params.empty() || !ob || params.size() < 3)
        return;
    auto v = static_cast<Shape *>(ob);
    v->set(Point::Make(CSSStrToPX(params[0]), CSSStrToPX(params[1])), 
           CSSStrToPX(params[2]));
}

static void ShapeSetBorder(Ref * ob, const CSSParams & params)
{
    if (params.empty() || !ob)
        return;
    if (params.size() < 3)
        return;
    auto v = static_cast<Shape *>(ob);
    Scalar rx = 0;
    if (params.size() >= 4)
        rx = CSSStrToPX(params[3]);
    Scalar ry = 0;
    if (params.size() >= 5)
        rx = CSSStrToPX(params[4]);

    v->setBorder(CSSStrToPX(params[0]),
                 CSSStrToShapeStyle(params[1]),
                 CSSStrToColor(params[2]),
                 rx, ry);
}

/*
widget
*/
static void WidgetSetLeft(Ref * ob, const CSSParams & params)
{
    if (params.empty() || !ob)
        return;
    auto v = static_cast<Widget *>(ob);
    v->setLeft(CSSStrToPX(params[0]));
}
static void WidgetSetTop(Ref * ob, const CSSParams & params)
{
    if (params.empty() || !ob)
        return;
    auto v = static_cast<Widget *>(ob);
    v->setTop(CSSStrToPX(params[0]));
}
static void WidgetetWidth(Ref * ob, const CSSParams & params)
{
    if (params.empty() || !ob)
        return;
    auto v = static_cast<Widget *>(ob);
    v->setWidth(CSSStrToPX(params[0]));
}

static void WidgetSetHeight(Ref * ob, const CSSParams & params)
{
    if (params.empty() || !ob)
        return;
    auto v = static_cast<Widget *>(ob);
    v->setHeight(CSSStrToPX(params[0]));
}

static void WidgetSetContent(Ref * ob, const CSSParams & params)
{
    if (params.empty() || !ob)
        return;
    auto v = static_cast<Widget *>(ob);
    v->setWindowText(Encoding::FromUTF8(params[0].begin, params[0].length).data());
}
/*
panel
*/
Panel::NCArea CSSStrToPanelNCArea(const CSSString & str)
{
    if (str == "caption")
        return Panel::kCaption;
    if (str == "bottom")
        return Panel::kBottom;
    if (str == "bottom-left")
        return Panel::kBottomLeft;
    if (str == "bottom-right")
        return Panel::kBottomRight;
    if (str == "top")
        return Panel::kTop;
    if (str == "top-left")
        return Panel::kTopLeft;
    if (str == "top-right")
        return Panel::kTopRight;
    LOG_ERROR << "hit test param error";
    return Panel::kCaption;
}

static void PanelSetCursor(Ref * ob, const CSSParams & params)
{
    if (params.empty() || !ob)
        return;
    auto v = static_cast<Panel *>(ob);
    v->setCursor(CSSStrToCursor(params[0]));
}

static void PanelHitTest(Ref * ob, const CSSParams & params)
{
    if (params.empty() || !ob)
        return;
    if (params.size() != 5)
        return;
    auto v = static_cast<Panel *>(ob);
    Rect r;
    r.setLTRB(CSSStrToScalar(params[1]),
        CSSStrToPX(params[2]),
        CSSStrToPX(params[3]),
        CSSStrToPX(params[4]));
    Panel::NCArea nc = CSSStrToPanelNCArea(params[0]);
    v->setNCArea(nc, r);
}

static void PanelSetColor(Ref * ob, const CSSParams & params)
{
    if (params.empty() || !ob)
        return;
    auto v = static_cast<Panel *>(ob);
    v->setColor(CSSStrToColor(params[0]));
}

static void PanelSetOpacity(Ref * ob, const CSSParams & params)
{
    if (params.empty() || !ob || params.size() < 1)
        return;
    auto v = static_cast<Panel *>(ob);
    v->setOpacity(CSSStrToScalar(params[0]));
}

static void PanelLayered(Ref * ob, const CSSParams & params)
{
    if (params.empty() || !ob || params.size() < 1)
        return;
    bool b = CSSStrToBool(params[0]);
    if (b)
        static_cast<Panel *>(ob)->addEXStyle(Panel::kLayered);
    else
        static_cast<Panel *>(ob)->removeEXStyle(Panel::kLayered);
}



ClassDescriptor::ClassDescriptor()
{
    registerArea();
    registerImage();
    registerText();
    registerShape();
    registerPanel();
}

ClassDescriptor::~ClassDescriptor()
{
    ;
}

CSSClassTable * ClassDescriptor::getFunc(const char * class_name)
{
    if (!class_name)
        return nullptr;
    auto iter = multi_class_tables_.find(class_name);
    if (iter == multi_class_tables_.end())
        return nullptr;

    return &(iter->second);
}

void ClassDescriptor::registerArea()
{
    auto & table = multi_class_tables_[kClassArea];
    registerView(table);
}


void ClassDescriptor::registerImage()
{
    //view已经register上了
    auto & table = multi_class_tables_[kClassImage];
    registerShirt(table);
}

void ClassDescriptor::registerText()
{
    auto & table = multi_class_tables_[kClassText];   
    registerShirt(table);
    table[kDescColor] = &TextSetColor;
    table[kDescContent] = &TextSetContent;
}

void ClassDescriptor::registerShape()
{
    auto & table = multi_class_tables_[kClassShape];
    registerShirt(table);
    table[kDescBorder] = &ShapeSetBorder;
    table[kDescColor] = &ShapeSetColor;
    table[kDescLinearGradient] = &ShapeSetLinearGradient;
    table[kDescRadialGradient] = &ShapeSetRadialGradient;
    //table[kDescMode] =  &ShapeSetMode;
    //table[kDescStyle] = &ShapeSetStyle;
    table[kDescStrokeWidth] = &ShapeSetStrokeWidth;
    table[kDescRect] = &ShapeSetRect;
    table[kDescCircle] = &ShapeSetCircle;
}

void ClassDescriptor::registerPanel()
{
    auto & table = multi_class_tables_[kClassPanel];
    registerWidget(table);
    table[kDescCursor] = &PanelSetCursor;
    table[kDescHitTest] = &PanelHitTest;
    table[kDescColor] = &PanelSetColor;
    table[kDescOpacity] = &PanelSetOpacity;
    table[kDescLayered] = &PanelLayered;
}

void ClassDescriptor::registerView(CSSClassTable & table)
{
    table[kDescLeft] = &ViewSetLeft;
    table[kDescTop] = &ViewSetTop;
    table[kDescWidth] = &ViewSetWidth;
    table[kDescHeight] = &ViewSetHeight;
    table[kDescCursor] = &ViewSetCursor;
}

void ClassDescriptor::registerShirt(CSSClassTable & table)
{
    registerView(table);
    table[kDescOpacity] = &ShirtSetOpacity;
}

void ClassDescriptor::registerWidget(CSSClassTable & table)
{
    table[kDescLeft] = &WidgetSetLeft;
    table[kDescTop] = &WidgetSetTop;
    table[kDescWidth] = &WidgetetWidth;
    table[kDescHeight] = &WidgetSetHeight;
    table[kDescContent] = &WidgetSetContent;
}

}