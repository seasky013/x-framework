
#ifndef __animation_animation_container_observer_h__
#define __animation_animation_container_observer_h__

#pragma once

class AnimationContainer;

// ���������animationsÿ�θ��¶���֪ͨ�۲���.
class AnimationContainerObserver
{
public:
    // ���������ʱ�Ӵ���ʱ, ������animations������ɺ���ñ�����.
    virtual void AnimationContainerProgressed(
        AnimationContainer* container) = 0;

    // �����������animationsΪ��ʱ����.
    virtual void AnimationContainerEmpty(AnimationContainer* container) = 0;

protected:
    virtual ~AnimationContainerObserver() {}
};

#endif //__animation_animation_container_observer_h__