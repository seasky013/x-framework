
#include "vector_canvas.h"

namespace skia
{

    VectorCanvas::VectorCanvas(PlatformDevice* device)
        : PlatformCanvas(device->getDeviceFactory())
    {
        setDevice(device)->unref(); // ����������ü���Ϊ1, setDevice��������һ��.
    }

    VectorCanvas::~VectorCanvas() {}

    SkBounder* VectorCanvas::setBounder(SkBounder* bounder)
    {
        if(!IsTopDeviceVectorial())
        {
            return PlatformCanvas::setBounder(bounder);
        }

        // This function isn't used in the code. Verify this assumption.
        SkASSERT(false);
        return NULL;
    }

    SkDrawFilter* VectorCanvas::setDrawFilter(SkDrawFilter* filter)
    {
        // This function isn't used in the code. Verify this assumption.
        SkASSERT(false);
        return NULL;
    }

    bool VectorCanvas::IsTopDeviceVectorial() const
    {
        return getTopPlatformDevice().IsVectorial();
    }

} //namespace skia