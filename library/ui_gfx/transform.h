
#ifndef __ui_gfx_transform_h__
#define __ui_gfx_transform_h__

#pragma once

#include "SkMatrix.h"

namespace gfx
{

    class Point;
    class Rect;

    // 3x3 transformation matrix. Transform is cheap and explicitly allows
    // copy/assign.
    // TODO: make this a 4x4.
    class Transform
    {
    public:
        Transform();
        ~Transform();

        // NOTE: The 'Set' functions overwrite the previously set transformation
        // parameters. The 'Concat' functions apply a transformation (e.g. rotation,
        // scale, translate) on top of the existing transforms, instead of overwriting
        // them.

        // NOTE: The order of the 'Set' function calls do not matter. However, the
        // order of the 'Concat' function calls do matter, especially when combined
        // with the 'Set' functions.

        // Sets the rotation of the transformation.
        void SetRotate(float degree);

        // Sets the scaling parameters.
        void SetScaleX(float x);
        void SetScaleY(float y);
        void SetScale(float x, float y);

        // Sets the translation parameters.
        void SetTranslateX(float x);
        void SetTranslateY(float y);
        void SetTranslate(float x, float y);

        // Applies rotation on the current transformation.
        void ConcatRotate(float degree);

        // Applies scaling on current transform.
        void ConcatScale(float x, float y);

        // Applies translation on current transform.
        void ConcatTranslate(float x, float y);

        // Applies a transformation on the current transformation
        // (i.e. 'this = this * transform;'). Returns true if the result can be
        // represented.
        bool PreconcatTransform(const Transform& transform);

        // Applies a transformation on the current transformation
        // (i.e. 'this = transform * this;'). Returns true if the result can be
        // represented.
        bool ConcatTransform(const Transform& transform);

        // Does the transformation change anything?
        bool HasChange() const;

        // Applies the transformation on the point. Returns true if the point is
        // transformed successfully.
        bool TransformPoint(gfx::Point* point);

        // Applies the reverse transformation on the point. Returns true if the point
        // is transformed successfully.
        bool TransformPointReverse(gfx::Point* point);

        // Applies transformation on the rectangle. Returns true of the rectangle is
        // transformed successfully.
        bool TransformRect(gfx::Rect* rect) const;

        // Applies the reverse transformation on the rectangle. Returns true if the
        // rectangle is transformed successfully.
        bool TransformRectReverse(gfx::Rect* rect) const;

        // Returns the underlying matrix.
        const SkMatrix& matrix() const { return matrix_; }
        SkMatrix& matrix() { return matrix_; }

    private:
        SkMatrix matrix_;

        // copy/assign are allowed.
    };

} //namespace gfx

#endif //__ui_gfx_transform_h__