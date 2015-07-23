#include "shader.h"
#include "SkGradientShader.h"

namespace bones
{

Shader::Shader()
:shader_(nullptr)
{
    params_.type = kNone;
}

Shader::~Shader()
{
    free();
}

void Shader::setGradient(const Point & begin, const Point & end,
    Color * color, float * pos, size_t count, TileMode tile)
{
    free();
    params_.type = kLinearGradient;
    params_.colors.resize(count);
    params_.pos.resize(count);
    params_.tile = tile;

    for (size_t i = 0; i < count; ++i)
    {
        params_.colors[i] = color[i];
        params_.pos[i] = pos[i];
    }
    params_.linear.pts[0] = begin.x();
    params_.linear.pts[1] = begin.y();
    params_.linear.pts[2] = end.x();
    params_.linear.pts[3] = end.y();
}

void Shader::setGradient(const Point & center, Scalar radius,
    Color * color, float * pos, size_t count, TileMode tile)
{
    free();
    params_.type = kRadialGradient;
    params_.colors.resize(count);
    params_.pos.resize(count);
    params_.tile = tile;
    for (size_t i = 0; i < count; ++i)
    {
        params_.colors[i] = color[i];
        params_.pos[i] = pos[i];
    }
    params_.radial.radius = radius;
    params_.radial.center[0] = center.x();
    params_.radial.center[1] = center.y();
}

void Shader::free()
{
    if (shader_)
        shader_->unref();
    shader_ = nullptr;
}

SkShader * Shader::getShader()
{
    if (!shader_)
    {
        SkShader::TileMode mode = SkShader::kClamp_TileMode;
        if (kRepeat == params_.tile)
            mode = SkShader::kRepeat_TileMode;
        else if (kMirror == params_.tile)
            mode = SkShader::kMirror_TileMode;
        if (kLinearGradient == params_.type)
        {
            SkPoint pt[2] = { { params_.linear.pts[0], params_.linear.pts[1] },
                              { params_.linear.pts[2], params_.linear.pts[3] } };
            shader_ = SkGradientShader::CreateLinear(
                pt, &params_.colors[0], &params_.pos[0], 
                params_.colors.size(), mode);
        }
        else if (kRadialGradient == params_.type)
        {
            SkPoint center = { params_.radial.center[0], params_.radial.center[1] };
            shader_ = SkGradientShader::CreateRadial(
                center, params_.radial.radius, &params_.colors[0], &params_.pos[0],
                params_.colors.size(), mode);
        }

    }

    return shader_;
}

Shader & Shader::operator=(const Shader & right)
{
    free();
    params_ = right.params_;
    shader_ = right.shader_;
    if (shader_)
        shader_->ref();
    return *this;
}

}