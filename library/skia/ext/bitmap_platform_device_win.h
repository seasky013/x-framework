
#ifndef __skia_ext_bitmap_platform_device_win_h__
#define __skia_ext_bitmap_platform_device_win_h__

#pragma once

#include "platform_device_win.h"

namespace skia
{

    class BitmapPlatformDeviceFactory : public SkDeviceFactory
    {
    public:
        virtual SkDevice* newDevice(SkCanvas* ignored,
            SkBitmap::Config config, 
            int width, int height,
            bool isOpaque, bool isForLayer);
    };

    // BitmapPlatformDevice��SkBitmap�Ļ�����װ, ΪSkCanvas�ṩ��ͼ����. �豸Ϊ
    // Windows�ṩ��һ����д�ı���. BitmapPlatformDeviceʹ��CreateDIBSection()��
    // ��һ��Skia֧�ֵ�λͼ, �����Ϳ��Ի���ClearType����. �����������豸��λͼ��,
    // �������Ա�����.
    //
    // �豸ӵ����������, ���豸����ʱ, ��������Ҳ�����Ч��. ��һ�㲻ͬ��SKIA,
    // ������������ʹ�������ü���. ��Skia��, �����������һ��λͼ����豸��λͼ
    // ��ֵ, ���������. ��������, �滻��λͼ�����豸�Ƿ���ʱ��Ҳ��ɲ��Ϸ���,
    // �ⳣ������һЩ���޵�����. ����, ��Ҫ��������λͼ������豸���������ݸ�ֵ,
    // ȷ��ʹ�õ��ǿ���.
    class BitmapPlatformDevice : public PlatformDevice
    {
    public:
        // ���̺���. screen_dc���ڴ���λͼ, �����ں����д洢. �����������ȷ֪��λͼ
        // ��ȫ��͸���������������is_opaqueΪtrue, ������������һЩ�Ż�.
        //
        // shared_section�����ǿ�ѡ��(����NULLʹ��ȱʡ��Ϊ). ���shared_section�ǿ�,
        // ��������һ��CreateFileMapping���ص��ļ�ӳ�����. ϸ�ڲμ�CreateDIBSection.
        static BitmapPlatformDevice* create(HDC screen_dc,
            int width, int height, bool is_opaque, HANDLE shared_section);

        // ������һ��, ֻ�Ǻ��������ȡscreen_dc.
        static BitmapPlatformDevice* create(int width, int height,
            bool is_opaque, HANDLE shared_section);

        // �������캯��. ����ʱ, �����豸���ڲ�����ָ��, ���Զ���乲������. ����
        // ��Ϊ�ڲ���ʵ������������(�������ڴ��һЩGDI����). ���һ���豸������,
        // �ü���������״̬���ᱣ��ͬ��.
        //
        // ����ζ�Ÿ���һ���豸����ֵ��һ�������ǲ��е�, ��Ϊ�������������������
        // �Լ��Ĳü���, ���յ�GDI�ü�����Ҳ�ǲ�һ����.
        //
        // �����͸�ֵ���챻���������ʡ�豸���Ǵ��ݵ�������Ҫֱ�Ӵ���λͼ���ݵĺ�
        // ��.
        BitmapPlatformDevice(const BitmapPlatformDevice& other);
        virtual ~BitmapPlatformDevice();

        // �μ�������ڿ������캯���ľ���.
        BitmapPlatformDevice& operator=(const BitmapPlatformDevice& other);

        virtual SkDeviceFactory* getDeviceFactory()
        {
            return SkNEW(BitmapPlatformDeviceFactory);
        }

        // ����λͼ���ݵ��ڴ�DC. λͼDC����������.
        virtual HDC getBitmapDC();

        // ���ظ����ı仯�Ͳü�����HDC. ����SkDevice��.
        virtual void setMatrixClip(const SkMatrix& transform,
            const SkRegion& region,
            const SkClipStack&);

        virtual void drawToHDC(HDC dc, int x, int y, const RECT* src_rect);
        virtual void makeOpaque(int x, int y, int width, int height);
        virtual bool IsVectorial() { return false; }

        // ���ظ���λ�õ���ɫֵ. �������豸�����õ��κα仯.
        SkColor getColorAt(int x, int y);

    protected:
        // ˢ��Windows DC, �Ա�Skia�ܹ�ֱ�ӷ�����������. ����SkDevice��, ��Skia��ʼ
        // ������������ʱ����.
        virtual void onAccessBitmap(SkBitmap* bitmap);

    private:
        // ���ü�������, �ܱ�����豸�乲��, ��֤��������͸�ֵ������������. �����豸
        // ʹ�õ�λͼ�Ѿ������ü�����, ֧�ֿ���.
        class BitmapPlatformDeviceData;

        // ˽�й��캯��. ����Ӧ�������ù���.
        BitmapPlatformDevice(BitmapPlatformDeviceData* data,
            const SkBitmap& bitmap);

        // �豸����������, ��֤�ǿ�. ���Ǵ洢���������.
        BitmapPlatformDeviceData* data_;
    };

} //namespace skia

#endif //__skia_ext_bitmap_platform_device_win_h__