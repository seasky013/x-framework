
#ifndef __gfx_brush_h__
#define __gfx_brush_h__

#pragma once

#include "base/basic_types.h"

namespace gfx
{

    // Brush��װƽ̨���ػ�ˢ. ���ദ��ײ�ı��ػ�ˢ�ڴ����.
    class Brush
    {
    public:
        Brush() {}
        virtual ~Brush() {}

    private:
        DISALLOW_COPY_AND_ASSIGN(Brush);
    };

} //namespace gfx

#endif //__gfx_brush_h__