
#ifndef __animation_animation_delegate_h__
#define __animation_animation_delegate_h__

#pragma once

class Animation;

// AnimationDelegate
//
// ��Ҫ����animation��״̬֪ͨ, ��Ҫʵ������ӿ�.
class AnimationDelegate
{
public:
    // animation���ʱ����.
    virtual void AnimationEnded(const Animation* animation) {}

    // animation����ʱ����.
    virtual void AnimationProgressed(const Animation* animation) {}

    // animationȡ��ʱ����.
    virtual void AnimationCanceled(const Animation* animation) {}

protected:
    virtual ~AnimationDelegate() {}
};

#endif //__animation_animation_delegate_h__