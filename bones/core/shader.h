#ifndef BONE_CORE_SHADER_H_
#define BONE_CORE_SHADER_H_

#include "core.h"
#include "point.h"
#include "color.h"
#include <vector>

class SkShader;
namespace bones
{

class Shader
{
public:
    enum TileMode
    {
        kClamp,
        kRepeat,
        kMirror,
    };
private:
    enum Type
    {
        kNone,
        kLinearGradient,
        kRadialGradient,
    };

    typedef struct 
    {
        Type type;
        std::vector<Color> colors;
        std::vector<float> pos;
        TileMode tile;
        union
        {
            struct LinearGradient
            {
                Scalar pts[4];
            } linear;

            struct RadialGradient
            {
                Scalar center[2];
                Scalar radius;
            } radial;
        };
    } Params;
public:
    Shader();

    ~Shader();

    void setGradient(const Point & begin, const Point & end,
        Color * color, float * pos, size_t count, TileMode tile);

    void setGradient(const Point & center, Scalar radius,
        Color * color, float * pos, size_t count, TileMode tile);

    Shader & operator=(const Shader & right);
private:
    void free();

    SkShader * getShader();
private:
    Params params_;

    SkShader * shader_;
    friend class Helper;
};
}


#endif