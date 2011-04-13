
#include "lazy_instance.h"

#include "at_exit.h"
#include "threading/platform_thread.h"

namespace base
{

    bool LazyInstanceHelper::NeedsInstance()
    {
        // ���Դ���ʵ��, ����ǵ�һ�ε���, state��_��EMPTY��ΪCREATING.
        // �����ʾ��������.
        if(Acquire_CompareAndSwap(&state_, STATE_EMPTY,
            STATE_CREATING) == STATE_EMPTY)
        {
            // �����߱��봴��ʵ��.
            return true;
        }
        else
        {
            // Ҫô���ڴ���Ҫô�Ѿ��������. ѭ���ȴ�.
            while(NoBarrier_Load(&state_) != STATE_CREATED)
            {
                PlatformThread::YieldCurrentThread();
            }
        }

        // �����߳��Ѿ�������ʵ��.
        return false;
    }

    void LazyInstanceHelper::CompleteInstance(void* instance, void (*dtor)(void*))
    {
        // ʵ��������, ��CREATING��ΪCREATED.
        Release_Store(&state_, STATE_CREATED);

        // ȷ��ʵ�����Ķ����ڳ����˳�ʱ������.
        if(dtor)
        {
            AtExitManager::RegisterCallback(dtor, instance);
        }
    }

} //namespace base