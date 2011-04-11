
#ifndef __gfx_transform_h__
#define __gfx_transform_h__

#pragma once

namespace gfx
{

    class Point;
    class Rect;

    // �任�ӿ�.
    // ʵ�ֽӿڵ������ڽ�������ı任(������ת�����ŵ�).
    class Transform
    {
    public:
        virtual ~Transform() {}

        // ����ʵ�ֽӿڵĶ���(����ʹ��skia�ı任����).
        static Transform* Create();

        // ������ת�Ƕ�.
        virtual void SetRotate(float degree) = 0;

        // �������Ų���.
        virtual void SetScaleX(float x) = 0;
        virtual void SetScaleY(float y) = 0;
        virtual void SetScale(float x, float y) = 0;

        // ����ƽ�Ʋ���.
        virtual void SetTranslateX(float x) = 0;
        virtual void SetTranslateY(float y) = 0;
        virtual void SetTranslate(float x, float y) = 0;

        // �ڵ�ǰ�任�����Ͻ�����ת.
        virtual void ConcatRotate(float degree) = 0;

        // �ڵ�ǰ�任�����Ͻ�������.
        virtual void ConcatScale(float x, float y) = 0;

        // �ڵ�ǰ�任�����Ͻ���ƽ��.
        virtual void ConcatTranslate(float x, float y) = 0;

        // �ڵ�ǰ�任�����Ͻ��б任(���� 'this = this * transform;').
        virtual bool ConcatTransform(const Transform& transform) = 0;

        // �Ƿ�Ϊ��Ч�任?
        virtual bool HasChange() const = 0;

        // �Ե�Ӧ�õ�ǰ�任.
        virtual bool TransformPoint(Point* point) = 0;

        // �Ե�Ӧ�õ�ǰ��任.
        virtual bool TransformPointReverse(Point* point) = 0;

        // �Ծ���Ӧ�õ�ǰ�任.
        virtual bool TransformRect(Rect* rect) = 0;
    };

} //namespace gfx

#endif //__gfx_transform_h__