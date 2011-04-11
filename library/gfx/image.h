
#ifndef __gfx_image_h__
#define __gfx_image_h__

#pragma once

#include <map>

#include "base/basic_types.h"

class SkBitmap;

namespace gfx
{

    class ImageRep;

    // ����ͼ�����͵ķ�װ��, �����Ǳ���ƽ̨��GdkBitmap/NSImage����SkBitmap.
    // ͨ�����������ؿ���ת��������ͼ������. �ڲ��Ỻ��ת������ͼ������ظ�ת��.
    //
    // ��ʼ��ͼ������Լ�ת����ͼ������������ں�Image����һ��.
    class Image
    {
    public:
        enum RepresentationType
        {
            kGdkPixbufRep,
            kNSImageRep,
            kSkBitmapRep,
        };

        // ����ȱʡͼ�����͵�Image. ����ӹ�bitmap����Ȩ.
        explicit Image(const SkBitmap* bitmap);

        // ɾ��ͼ���Լ��ڲ������ת��ͼ��.
        ~Image();

        // ͼ������ת������.
        operator const SkBitmap*();
        operator const SkBitmap&();

        // �鿴�Ƿ���ָ�����͵�ͼ��.
        bool HasRepresentation(RepresentationType type);

        // ��|other|�����ڲ��洢��ͼ��.
        void SwapRepresentations(gfx::Image* other);

    private:
        // ����ȱʡ��ImageRep.
        ImageRep* DefaultRepresentation();

        // ����ָ�����͵�ImageRep, ��Ҫʱת��������.
        ImageRep* GetRepresentation(RepresentationType rep);

        // �洢��map.
        void AddRepresentation(ImageRep* rep);

        // ���캯�����ݵ�ͼ������. ��������Ǵ�����|representations_|��.
        RepresentationType default_representation_;

        typedef std::map<RepresentationType, ImageRep*> RepresentationMap;
        // ����ͼ������. ������һ��, ���ÿ��������һ��ת������.
        RepresentationMap representations_;

        DISALLOW_COPY_AND_ASSIGN(Image);
    };

} //namespace gfx

#endif //__gfx_image_h__