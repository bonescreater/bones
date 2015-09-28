#ifndef BONES_CORE_COLOR_H_
#define BONES_CORE_COLOR_H_

#include "core.h"

namespace bones
{

inline Color ColorSetARGB(uint8_t a, uint8_t r, uint8_t g, uint8_t b)
{
    return (a << 24) | (r << 16) | (g << 8) | (b << 0);
}

inline uint8_t ClampAlpha(float opacity, uint8_t max_alpha = 255)
{
    if (opacity < 0)
        return 0;
    if (opacity > 1)
        return max_alpha;

    return static_cast<uint8_t>(opacity * max_alpha);
}

inline uint8_t ColorGetA(Color color)
{
    return (((color) >> 24) & 0xFF);
}

inline uint8_t ColorGetR(Color color)
{
    return (((color) >> 16) & 0xFF);
}

inline uint8_t ColorGetG(Color color)
{
    return (((color) >> 8) & 0xFF);
}

inline uint8_t ColorGetB(Color color)
{
    return (((color) >> 0) & 0xFF);
}

inline PMColor PreMultiplyARGB(uint8_t a, uint8_t r, uint8_t g, uint8_t b)
{
    if (a != 255)
    {
        r = (r * a) >> 8;
        g = (g * a) >> 8;
        b = (b * a) >> 8;
    }

    return ColorSetARGB(a, r, g, b);
}

inline PMColor PreMultiplyColor(Color color)
{
    return PreMultiplyARGB(
        ColorGetA(color), ColorGetR(color),
        ColorGetG(color), ColorGetB(color));
}

inline ColorF ColorToColorF(Color color)
{
    ColorF cf;
    cf.a = ColorGetA(color) / 255.f;
    cf.r = ColorGetR(color) / 255.f;
    cf.g = ColorGetG(color) / 255.f;
    cf.b = ColorGetB(color) / 255.f;
    return cf;
}

inline Color ColorFToColor(ColorF cf)
{
    return ColorSetARGB(
        static_cast<uint8_t>(cf.a * 255.f),
        static_cast<uint8_t>(cf.r * 255.f),
        static_cast<uint8_t>(cf.g * 255.f),
        static_cast<uint8_t>(cf.b * 255.f)
        );
}


typedef struct
{
    Scalar mat[20];
}ColorMatrix;


}

#endif