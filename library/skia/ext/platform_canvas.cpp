
#include "platform_canvas.h"

#include "bitmap_platform_device_win.h"

namespace skia
{

    PlatformCanvas::PlatformCanvas()
        : SkCanvas(SkNEW(BitmapPlatformDeviceFactory)) {}

    PlatformCanvas::PlatformCanvas(SkDeviceFactory* factory)
        : SkCanvas(factory) {}

    SkDevice* PlatformCanvas::setBitmapDevice(const SkBitmap&)
    {
        SkASSERT(false); // ���ܵ���.
        return NULL;
    }

    PlatformDevice& PlatformCanvas::getTopPlatformDevice() const
    {
        // �������е��豸������ƽ̨��ص�.
        SkCanvas::LayerIter iter(const_cast<PlatformCanvas*>(this), false);
        return *static_cast<PlatformDevice*>(iter.device());
    }

    // static
    size_t PlatformCanvas::StrideForWidth(unsigned width)
    {
        return 4 * width;
    }

    bool PlatformCanvas::initializeWithDevice(SkDevice* device)
    {
        if(!device)
        {
            return false;
        }

        setDevice(device);
        device->unref(); // ����������ü���Ϊ1, setDevice��������һ��.
        return true;
    }

} //namespace skia