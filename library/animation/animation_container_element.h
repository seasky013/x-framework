
#ifndef __animation_animation_container_element_h__
#define __animation_animation_container_element_h__

#include "base/time.h"

#pragma once

// AnimationContainer������Ԫ�ؽӿ�, ��Animationʵ��.
class AnimationContainerElement
{
public:
    // ����animation������ʱ��. ��AnimationContainer::Start����.
    virtual void SetStartTime(base::TimeTicks start_time) = 0;

    // ��animation����ʱ����.
    virtual void Step(base::TimeTicks time_now) = 0;

    // ����animation��ʱ����. ���һ��Ԫ����Ҫ�ı����ֵ, ��Ҫ��
    // ����Stop, Ȼ��Start.
    virtual base::TimeDelta GetTimerInterval() const = 0;

protected:
    virtual ~AnimationContainerElement() {}
};

#endif //__animation_animation_container_element_h__