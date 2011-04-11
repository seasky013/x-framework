
#include "bitmap_platform_device_win.h"

#include "SkUtils.h"

#include "bitmap_platform_device_data.h"

namespace
{

    // Լ��position��size, ʹ֮�޶���[0, available_size]��.
    // ���|size|��-1, ����available_size. ���position����available_size,
    // ��������false.
    bool Constrain(int available_size, int* position, int* size)
    {
        if(*size < -2)
        {
            return false;
        }

        // ������ԭ��.
        if(*position < 0)
        {
            if(*size != -1)
            {
                *size += *position;
            }
            *position = 0;
        }
        if(*size==0 || *position>=available_size)
        {
            return false;
        }

        if(*size > 0)
        {
            int overflow = (*position + *size) - available_size;
            if(overflow > 0)
            {
                *size -= overflow;
            }
        }
        else
        {
            // ����available_size.
            *size = available_size - *position;
        }
        return true;
    }

}

namespace skia
{

    void BitmapPlatformDevice::makeOpaque(int x, int y, int width, int height)
    {
        const SkBitmap& bitmap = accessBitmap(true);
        SkASSERT(bitmap.config() == SkBitmap::kARGB_8888_Config);

        // �޸�: ����������̫��, ��Ӧ����������洦��任. PlatformCanvasӦ���ṩ
        // ����任�ĺ���(ʹ�ñ任�����Ǳ�ͨ), ���ݸ����ǵ����Ѿ��任���ľ���.
        const SkMatrix& matrix = data_->transform();
        int bitmap_start_x = SkScalarRound(matrix.getTranslateX()) + x;
        int bitmap_start_y = SkScalarRound(matrix.getTranslateY()) + y;

        if(Constrain(bitmap.width(), &bitmap_start_x, &width) &&
            Constrain(bitmap.height(), &bitmap_start_y, &height))
        {
            SkAutoLockPixels lock(bitmap);
            SkASSERT(bitmap.rowBytes()%sizeof(uint32_t) == 0u);
            size_t row_words = bitmap.rowBytes() / sizeof(uint32_t);
            // ָ��ָ���һ���޸ĵ�����.
            uint32_t* data = bitmap.getAddr32(0, 0) + (bitmap_start_y * row_words) +
                bitmap_start_x;
            for(int i=0; i<height; i++)
            {
                for(int j=0; j<width; j++)
                {
                    data[j] |= (0xFF << SK_A32_SHIFT);
                }
                data += row_words;
            }
        }
    }

} //namespace skia