
#ifndef __skia_ext_platform_device_win_h__
#define __skia_ext_platform_device_win_h__

#pragma once

#include <windows.h>

#include <vector>

#include "SkDevice.h"

class SkMatrix;
class SkPath;
class SkRegion;

namespace skia
{

    // PlatformDevice��SkBitmap�Ļ�����װ, ΪSkCanvas�ṩ��ͼ����. �豸ΪWindows�ṩ
    // ��һ����д�ı���, ���ṩ����GDI��ͼ������ϵĹ���. PlatformDevice�ǳ��������
    // ������ʵ��, Ҫôʹ�ú�̨λͼ, Ҫô����.
    class PlatformDevice : public SkDevice
    {
    public:
        typedef HDC PlatformSurface;

        // λͼ��Ӧ��DC, GDI����ͨ������λͼ�л���. ����һ���������Ľӿ�, ����ֻ��
        // ����Ⱦ���ε�ʱ�����ȡ.
        virtual HDC getBitmapDC() = 0;

        // �ڸ�������ĻDC��ͼ, ���λͼDC������, ����ʱ����. ������Ѿ�������λͼDC,
        // �Һ�������ǰû�б��ͷ�, ���ЧһЩ, ��Ϊ�����ٴ���һ��λͼDC. ���src_rect
        // Ϊ��, ����Դ�豸���ᱻ����.
        virtual void drawToHDC(HDC dc, int x, int y, const RECT* src_rect) = 0;

        // ���ø���������������ض��ǲ�͸����.
        virtual void makeOpaque(int x, int y, int width, int height) {}

        // ������ѡ����Ⱦ�����ǻ���ʸ���Ļ��ǻ���λͼ��.
        virtual bool IsVectorial() = 0;

        // �豸�Ƿ�����GDI��Ⱦ�ı�.
        virtual bool IsNativeFontRenderingAllowed() { return true; }

        // ��ʼ��DC��ȱʡ���ú���ɫ.
        static void InitializeDC(HDC context);

        // ����һ��SkPath��DC. ·�����غ�����ڲü����������.
        static void LoadPathToDC(HDC context, const SkPath& path);

        // ����һ��SkRegion��DC.
        static void LoadClippingRegionToDC(HDC context, const SkRegion& region,
            const SkMatrix& transformation);

    protected:
        struct CubicPoints
        {
            SkPoint p[4];
        };
        typedef std::vector<CubicPoints> CubicPath;
        typedef std::vector<CubicPath> CubicPaths;

        // ����|bitmap|��SkDevice�Ĺ��캯��.
        PlatformDevice(const SkBitmap& bitmap);

        // ����Skia�ı任��DC������任, �������ӽ�(GDI��֧��).
        static void LoadTransformToDC(HDC dc, const SkMatrix& matrix);

        // SkPath��·��ת����һϵ�е����α���������·��.
        static bool SkPathToCubicPaths(CubicPaths* paths, const SkPath& skpath);
    };

} //namespace skia

#endif //__skia_ext_platform_device_win_h__