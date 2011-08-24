
#include "platform_canvas.h"

#include "bitmap_platform_device_win.h"

namespace skia
{

    PlatformCanvas::PlatformCanvas() {}

    SkDevice* PlatformCanvas::setBitmapDevice(const SkBitmap&)
    {
        SkASSERT(false); // Should not be called.
        return NULL;
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
        device->unref(); // Was created with refcount 1, and setDevice also refs.
        return true;
    }

    SkCanvas* CreateBitmapCanvas(int width, int height, bool is_opaque)
    {
        return new PlatformCanvas(width, height, is_opaque);
    }

    SkCanvas* TryCreateBitmapCanvas(int width, int height, bool is_opaque)
    {
        PlatformCanvas* canvas = new PlatformCanvas();
        if(!canvas->initialize(width, height, is_opaque))
        {
            delete canvas;
            canvas = NULL;
        }
        return canvas;
    }

    SkDevice* GetTopDevice(const SkCanvas& canvas)
    {
        SkCanvas::LayerIter iter(const_cast<SkCanvas*>(&canvas), false);
        return iter.device();
    }

    bool SupportsPlatformPaint(const SkCanvas* canvas)
    {
        // TODO(alokp): Rename IsNativeFontRenderingAllowed after removing these
        // calls from WebKit.
        PlatformDevice* platform_device = GetPlatformDevice(GetTopDevice(*canvas));
        return platform_device && platform_device->IsNativeFontRenderingAllowed();
    }

    HDC BeginPlatformPaint(SkCanvas* canvas)
    {
        PlatformDevice* platform_device = GetPlatformDevice(GetTopDevice(*canvas));
        if(platform_device)
        {
            return platform_device->BeginPlatformPaint();
        }

        return NULL;
    }

    void EndPlatformPaint(SkCanvas* canvas)
    {
        PlatformDevice* platform_device = GetPlatformDevice(GetTopDevice(*canvas));
        if(platform_device)
        {
            platform_device->EndPlatformPaint();
        }
    }

    void DrawToNativeContext(SkCanvas* canvas, HDC context, int x,
        int y, const RECT* src_rect)
    {
        PlatformDevice* platform_device = GetPlatformDevice(GetTopDevice(*canvas));
        if(platform_device)
        {
            platform_device->DrawToNativeContext(context, x, y, src_rect);
        }
    }

    void MakeOpaque(SkCanvas* canvas, int x, int y, int width, int height)
    {
        PlatformDevice* platform_device = GetPlatformDevice(GetTopDevice(*canvas));
        if(platform_device)
        {
            platform_device->MakeOpaque(x, y, width, height);
        }
    }

} //namespace skia