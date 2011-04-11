
#ifndef __skia_ext_platform_canvas_h__
#define __skia_ext_platform_canvas_h__

#pragma once

#include "platform_device_win.h"
#include "SkCanvas.h"

namespace skia
{

    // PlatformCanvas��һ�����⻯�Ĺ���SkCanvas, �������PlatformDevice������ƽ̨
    // ��صĻ�ͼ. ��ͬʱ����Skia������ƽ̨��ز���.
    class PlatformCanvas : public SkCanvas
    {
    public:
        // ���ʹ���޲����汾�Ĺ��캯��, �����ֶ�����initialize().
        PlatformCanvas();
        explicit PlatformCanvas(SkDeviceFactory* factory);
        // �����Ҫ����λͼ, �Ҳ���Ҫ͸����, ��������is_opaque: ��ô������һЩ
        // �Ż�.
        PlatformCanvas(int width, int height, bool is_opaque);
        virtual ~PlatformCanvas();

        // ����shared_section��BitmapPlatformDevice::create.
        PlatformCanvas(int width, int height, bool is_opaque, HANDLE shared_section);

        // �����ʹ�õ��������޲����Ĺ��캯��, ���ǵڶ�����ʼ������.
        bool initialize(int width, int height, bool is_opaque,
            HANDLE shared_section=NULL);

        // ����������Ӧ�ð�ƽ̨��ͼ������Χ����, ���صı�������ڱ���ƽ̨����.
        //
        // �ڵ���ƽ̨��ص�beginPlatformPaint֮��, ������ɻ�ͼ��Ҫʹ��Skia����ʱ,
        // ��Ҫ����endPlatformPaint; �����ڱ�Ҫʱͬ��λͼ��OS.
        PlatformDevice::PlatformSurface beginPlatformPaint();
        void endPlatformPaint();

        // ���������ε�ƽ̨�豸ָ��, ���зǿյĲü���. ʵ����, һ��Ҫô�Ƕ���
        // Ҫôʲô��û��, ��Ϊͨ�������²�ʱ�������òü���.
        //
        // ������еĲ㶼���ü���, �᷵��һ�����õ��豸, ���������߲��ؼ�鷵��ֵ.
        // �����ܹ�������, ���κλ�ͼ����ǰ���ü���.
        //
        // ��SkCanvas��getDevice������, ��ΪSkCanvas���ص�����ײ���豸.
        //
        // Σ��: ��Ҫ���淵�ص��豸, ��������һ�ε���save()����restore()ʱʧЧ.
        PlatformDevice& getTopPlatformDevice() const;

        // ���ظ�����ȵ�strideֵ(һ�����õ��ֽ���). ��Ϊÿ������ʹ��32-bits, ����
        // �����4*width. Ȼ��, ���ڶ����ԭ����ܻ�������.
        static size_t StrideForWidth(unsigned width);

    private:
        // initialize()�������õĸ�������.
        bool initializeWithDevice(SkDevice* device);

        // δʵ��. ������ֹ���˵���SkCanvas���������. SkCanvas�İ汾�����麯��, ��
        // �����ǲ�����100%����ֹ, ��ϣ�������������ǵ�ע��, ����ʹ���������. ����
        // SkCanvas�İ汾�ᴴ��һ���µĲ������豸, ���������ʹ��CoreGraphics������
        // ��ͼ, �������.
        virtual SkDevice* setBitmapDevice(const SkBitmap& bitmap);

        // ��ֹ�����͸�ֵ���캯��.
        PlatformCanvas(const PlatformCanvas&);
        PlatformCanvas& operator=(const PlatformCanvas&);
    };

} //namespace skia

#endif //__skia_ext_platform_canvas_h__