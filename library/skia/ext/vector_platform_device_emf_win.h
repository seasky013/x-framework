
#ifndef __skia_ext_vector_platform_device_win_h__
#define __skia_ext_vector_platform_device_win_h__

#pragma once

#include "SkMatrix.h"
#include "SkRegion.h"

#include "platform_device_win.h"

namespace skia
{

    class VectorPlatformDeviceFactory : public SkDeviceFactory
    {
    public:
        virtual SkDevice* newDevice(SkCanvas* ignored,
            SkBitmap::Config config,
            int width, int height,
            bool isOpaque, bool isForLayer);
        static SkDevice* CreateDevice(int width, int height,
            bool isOpaque, HANDLE shared_section);
    };

    // VectorPlatformDevice��SkBitmap�Ļ�����װ, ΪSkCanvas�ṩ��ͼ����. ��û��
    // �ڴ滺��, ����ǲ��ɶ���. ��Ϊ��̨��������ȫʸ������, VectorPlatformDevice
    // ֻ�Ƕ�Windows DC����ķ�װ.
    class VectorPlatformDevice : public PlatformDevice
    {
    public:
        // �೧����. DC��������Ϊ�������.
        static VectorPlatformDevice* create(HDC dc, int width, int height);

        VectorPlatformDevice(HDC dc, const SkBitmap& bitmap);
        virtual ~VectorPlatformDevice();

        virtual SkDeviceFactory* getDeviceFactory()
        {
            return SkNEW(VectorPlatformDeviceFactory);
        }

        virtual HDC getBitmapDC()
        {
            return hdc_;
        }

        virtual void drawPaint(const SkDraw& draw, const SkPaint& paint);
        virtual void drawPoints(const SkDraw& draw, SkCanvas::PointMode mode,
            size_t count, const SkPoint[], const SkPaint& paint);
        virtual void drawRect(const SkDraw& draw, const SkRect& r,
            const SkPaint& paint);
        virtual void drawPath(const SkDraw& draw, const SkPath& path,
            const SkPaint& paint);
        virtual void drawBitmap(const SkDraw& draw, const SkBitmap& bitmap,
            const SkMatrix& matrix, const SkPaint& paint);
        virtual void drawSprite(const SkDraw& draw, const SkBitmap& bitmap,
            int x, int y, const SkPaint& paint);
        virtual void drawText(const SkDraw& draw, const void* text, size_t len,
            SkScalar x, SkScalar y, const SkPaint& paint);
        virtual void drawPosText(const SkDraw& draw, const void* text, size_t len,
            const SkScalar pos[], SkScalar constY,
            int scalarsPerPos, const SkPaint& paint);
        virtual void drawTextOnPath(const SkDraw& draw, const void* text, size_t len,
            const SkPath& path, const SkMatrix* matrix,
            const SkPaint& paint);
        virtual void drawVertices(const SkDraw& draw, SkCanvas::VertexMode,
            int vertexCount,
            const SkPoint verts[], const SkPoint texs[],
            const SkColor colors[], SkXfermode* xmode,
            const uint16_t indices[], int indexCount,
            const SkPaint& paint);
        virtual void drawDevice(const SkDraw& draw, SkDevice*, int x, int y,
            const SkPaint&);

        virtual void setMatrixClip(const SkMatrix& transform, const SkRegion& region,
            const SkClipStack&);
        virtual void drawToHDC(HDC dc, int x, int y, const RECT* src_rect);
        virtual bool IsVectorial() { return true; }

        void LoadClipRegion();
        bool alpha_blend_used() const { return alpha_blend_used_; }

    private:
        // Ӧ��SkPaint�Ļ�ͼ���Ե���ǰ��GDI�豸����. ���GDI��֧�����еĻ�ͼ����,
        // ��������false. ��ִ��SkPaint�е�"����".
        bool ApplyPaint(const SkPaint& paint);

        // ѡ��һ���¶����豸����. �����ǻ��ʡ���ˢ���ü�����λͼ��������.
        // ����ѡ���Ķ���.
        HGDIOBJ SelectObject(HGDIOBJ object);

        // ����SkPaint�����Դ���һ����ˢ.
        bool CreateBrush(bool use_brush, const SkPaint& paint);

        // ����SkPaint�����Դ���һ������.
        bool CreatePen(bool use_pen, const SkPaint& paint);

        // ��ִ�л�ͼ�����ָ�����ǰ�Ķ���(���ʡ���ˢ��).
        void Cleanup();

        // ����һ������.
        bool CreateBrush(bool use_brush, COLORREF color);

        // ����SkPaint�����Դ���һ������.
        bool CreatePen(bool use_pen, COLORREF color, int stroke_width,
            float stroke_miter, DWORD pen_style);

        // ���豸�л���λͼ, ʹ�õ�ǰ���صı任����.
        void InternalDrawBitmap(const SkBitmap& bitmap, int x, int y,
            const SkPaint& paint);

        // Windows DC���. GDI��ͼ���豸����, ��ֻд����ʸ����.
        HDC hdc_;

        // DC�ı任����: ��Ҫ����ά���������, ��Ϊ�豸������û������ʱ��
        // �����Ѿ���Ҫ�������ֵ.
        SkMatrix transform_;

        // ��ǰ�ü���.
        SkRegion clip_region_;

        // ��ǰ���Ƶľɻ�ˢ.
        HGDIOBJ previous_brush_;

        // ��ǰ���Ƶľɻ���.
        HGDIOBJ previous_pen_;

        // ��ӡʱ���ù�AlphaBlend()��Ϊtrue.
        bool alpha_blend_used_;

        // ��֧�ֿ����͸�ֵ���캯��.
        VectorPlatformDevice(const VectorPlatformDevice&);
        const VectorPlatformDevice& operator=(const VectorPlatformDevice&);
    };

} //namespace skia

#endif //__skia_ext_vector_platform_device_win_h__