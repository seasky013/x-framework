
#ifndef __base_threading_thread_checker_h__
#define __base_threading_thread_checker_h__

#pragma once

#include "../synchronization/lock.h"

// ������֤�෽�������Ƿ�����ͬ���߳���. �Ӹ���̳�, ����CalledOnValidThread()
// ������֤.
//
// ���������ڱ�֤��Щ�����̰߳�ȫ�����ܱ���ȫʹ��. ����, һ���������һ������
// ���õ���.
//
// ʾ��:
//     class MyClass : public NonThreadSafe {
//     public:
//       void Foo() {
//         DCHECK(CalledOnValidThread());
//         ... (do stuff) ...
//       }
//     }
//
// ��Releaseģʽ��, CalledOnValidThreadʼ�շ���true.

#ifndef NDEBUG
class ThreadChecker
{
public:
    ThreadChecker();
    ~ThreadChecker();

    bool CalledOnValidThread() const;

    // �޸�CalledOnValidThread����֤�߳�, ��һ�ε���CalledOnValidThread����฽��
    // ��һ���µ��߳�, �Ƿ��뱩¶�˹���ȡ����NonThreadSafe��������. ��һ��������
    // һ���߳��д�����ֻ����һ���߳��б�ʹ�õ�ʱ��, �����õĴ˷���.
    void DetachFromThread();

private:
    void EnsureThreadIdAssigned() const;

    mutable base::Lock lock_;
    // ��ΪCalledOnValidThread�����ø�ֵ, ����ʹ��mutable.
    mutable DWORD valid_thread_id_;
};
#else
// releaseģʽ��ʲô������.
class ThreadChecker
{
public:
    bool CalledOnValidThread() const
    {
        return true;
    }

    void DetachFromThread() {}
};
#endif //NDEBUG

#endif //__base_threading_thread_checker_h__