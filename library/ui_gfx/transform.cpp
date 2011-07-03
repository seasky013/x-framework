
#include "transform.h"

#include "point.h"
#include "rect.h"
#include "skia_util.h"

namespace gfx
{

    Transform::Transform()
    {
        matrix_.reset();
    }

    Transform::~Transform() {}

    void Transform::SetRotate(float degree)
    {
        matrix_.setRotate(SkFloatToScalar(degree));
    }

    void Transform::SetScaleX(float x)
    {
        matrix_.setScaleX(SkFloatToScalar(x));
    }

    void Transform::SetScaleY(float y)
    {
        matrix_.setScaleY(SkFloatToScalar(y));
    }

    void Transform::SetScale(float x, float y)
    {
        matrix_.setScale(SkFloatToScalar(x), SkFloatToScalar(y));
    }

    void Transform::SetTranslateX(float x)
    {
        matrix_.setTranslateX(SkFloatToScalar(x));
    }

    void Transform::SetTranslateY(float y)
    {
        matrix_.setTranslateY(SkFloatToScalar(y));
    }

    void Transform::SetTranslate(float x, float y)
    {
        matrix_.setTranslate(SkFloatToScalar(x), SkFloatToScalar(y));
    }

    void Transform::ConcatRotate(float degree)
    {
        matrix_.postRotate(SkFloatToScalar(degree));
    }

    void Transform::ConcatScale(float x, float y)
    {
        matrix_.postScale(SkFloatToScalar(x), SkFloatToScalar(y));
    }

    void Transform::ConcatTranslate(float x, float y)
    {
        matrix_.postTranslate(SkFloatToScalar(x), SkFloatToScalar(y));
    }

    bool Transform::PreconcatTransform(const Transform& transform)
    {
        return matrix_.setConcat(matrix_, transform.matrix_);
    }

    bool Transform::ConcatTransform(const Transform& transform)
    {
        return matrix_.setConcat(transform.matrix_, matrix_);
    }

    bool Transform::HasChange() const
    {
        return !matrix_.isIdentity();
    }

    bool Transform::TransformPoint(gfx::Point* point)
    {
        SkPoint skp;
        matrix_.mapXY(SkIntToScalar(point->x()), SkIntToScalar(point->y()), &skp);
        point->SetPoint(static_cast<int>(skp.fX), static_cast<int>(skp.fY));
        return true;
    }

    bool Transform::TransformPointReverse(gfx::Point* point)
    {
        SkMatrix inverse;
        // TODO(sad): Try to avoid trying to invert the matrix.
        if(matrix_.invert(&inverse))
        {
            SkPoint skp;
            inverse.mapXY(SkIntToScalar(point->x()), SkIntToScalar(point->y()), &skp);
            point->SetPoint(static_cast<int>(skp.fX), static_cast<int>(skp.fY));
            return true;
        }
        return false;
    }

    bool Transform::TransformRect(gfx::Rect* rect) const
    {
        SkRect src = gfx::RectToSkRect(*rect);
        if(!matrix_.mapRect(&src))
        {
            return false;
        }
        *rect = gfx::SkRectToRect(src);
        return true;
    }

    bool Transform::TransformRectReverse(gfx::Rect* rect) const
    {
        SkMatrix inverse;
        if(!matrix_.invert(&inverse))
        {
            return false;
        }

        SkRect src = gfx::RectToSkRect(*rect);
        if(!inverse.mapRect(&src))
        {
            return false;
        }
        *rect = gfx::SkRectToRect(src);
        return true;
    }

} //namespace gfx