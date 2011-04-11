
#ifndef __skia_ext_vector_canvas_h__
#define __skia_ext_vector_canvas_h__

#pragma once

#include "platform_canvas.h"

namespace skia
{

    // PlatformCanvas��һ�����⻯�Ĺ���SkCanvas, �������VectorDevice������ƽ̨
    // ��صĻ�ͼ. ��ͬʱ����Skia������ƽ̨��ز���. ����֧��λͼ˫������Ϊû��
    // ʹ��λͼ.
    class VectorCanvas : public PlatformCanvas
    {
    public:
        VectorCanvas();
        explicit VectorCanvas(SkDeviceFactory* factory);
        VectorCanvas(HDC dc, int width, int height);
        virtual ~VectorCanvas();

        // �����ʹ�õ��������޲����Ĺ��캯��, ���ǵڶ�����ʼ������.
        bool initialize(HDC context, int width, int height);

        virtual SkBounder* setBounder(SkBounder* bounder);
        virtual SkDrawFilter* setDrawFilter(SkDrawFilter* filter);

    private:
        // ��������豸�ǻ�������������λͼ��, ����true.
        bool IsTopDeviceVectorial() const;

        // ��֧�ֿ����͸�ֵ���캯��.
        VectorCanvas(const VectorCanvas&);
        const VectorCanvas& operator=(const VectorCanvas&);
    };

} //namespace skia

#endif //__skia_ext_vector_canvas_h__