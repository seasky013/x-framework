
#ifndef __base_singleton_h__
#define __base_singleton_h__

#pragma once

#include "at_exit.h"
#include "atomicops.h"
#include "threading/thread_restrictions.h"

namespace base
{

    // Singleton<Type>ȱʡ��traits. ����new��delete�������ٶ���.
    // kRegisterAtExitע������˳�ʱ�Ƿ��Զ�����.
    // �����Ҫ�����������߲�ͬ���ڴ���䷽ʽ���Խ�������.
    template<typename Type>
    struct DefaultSingletonTraits
    {
        // �������.
        static Type* New()
        {
            // ()������ǳ���Ҫ, ǿ��POD���ͳ�ʼ��.
            return new Type();
        }

        // ���ٶ���.
        static void Delete(Type* x)
        {
            delete x;
        }

        // true: �����˳�ʱ�Զ�ɾ������.
        static const bool kRegisterAtExit = true;

        // false: ��ֹ����non-joinable�߳�. ��kRegisterAtExit��ͬ, ��Ϊ
        // StaticMemorySingletonTraits�������non-joinable�߳�, �Դ����˺ܺõĴ���.
        static const bool kAllowedToAccessOnNonjoinableThread = false;
    };


    // Singleton<Type>��һ����ѡ��traits, ָ�������˳�ʱ���������.
    template<typename Type>
    struct LeakySingletonTraits : public DefaultSingletonTraits<Type>
    {
        static const bool kRegisterAtExit = false;
        static const bool kAllowedToAccessOnNonjoinableThread = true;
    };


    // Singleton<Type>������һ��traits. ��һ�龲̬���ڴ���ʵ��������. ������
    // �����˳���ʱ������, ֻ���ڵ���Resurrect()�����������ٺ��������.
    //
    // ��ĳЩ�ط������õ�, ������logging��tracing��Щ����ʵ���������ڻ��ܷ���
    // �������.
    // ��logging��tracing��, ���ܻ�������ĵط�������, ���羲̬������, �̹߳ر�
    // ʱ��, ���ʱ����ϵĵ����������ٳ�ͻ. ���һ���̵߳���get(), ��ʱ��һ��
    // �߳�����ִ��AtExit����, ��һ���߳̿��ܻ����δ����ռ�Ķ���. ���ʵ��
    // �����ݶη���Ļ�, ������������һֱ����.
    //
    // ������������ϵͳ��Դ, �ᷴע��ϵͳ����־�ȼ��޸Ļص�.
    template<typename Type>
    struct StaticMemorySingletonTraits
    {
        static Type* New()
        {
            if(NoBarrier_AtomicExchange(&dead_, 1))
            {
                return NULL;
            }
            Type* ptr = reinterpret_cast<Type*>(buffer_);

            // ���ڴ����Ͻ��б���.
            new(ptr) Type();
            return ptr;
        }

        static void Delete(Type* p)
        {
            NoBarrier_Store(&dead_, 1);
            MemoryBarrier();
            if(p != NULL)
            {
                p->Type::~Type();
            }
        }

        static const bool kRegisterAtExit = true;
        static const bool kAllowedToAccessOnNonjoinableThread = true;

        // ���ڵ�Ԫ����.
        static void Resurrect()
        {
            NoBarrier_Store(&dead_, 0);
        }

    private:
        static const size_t kBufferSize = (sizeof(Type) +
            sizeof(intptr_t) - 1) / sizeof(intptr_t);
        static intptr_t buffer_[kBufferSize];

        // ��Ƕ����Ѿ���ɾ��, ���ٴ���.
        static Atomic32 dead_;
    };

    template<typename Type> intptr_t
        StaticMemorySingletonTraits<Type>::buffer_[kBufferSize];
    template<typename Type> Atomic32
        StaticMemorySingletonTraits<Type>::dead_ = 0;


    // Singleton<Type, Traits>���ڵ�һ��ʹ�õ�ʱ�򴴽�
    // һ��ʵ��, ���������˳���ʱ������. �������˳��������Trait::Delete.
    //
    // Singleton<>û�зǾ�̬��ԱҲ����Ҫʵ����. ��Ȼʵ����Ϊ���Ա����ȫ�ֶ���
    // ��û�������, ��Ϊ����POD���͵�.
    //
    // ��������̰߳�ȫ��, ����벢��ʹ����ôType��Ҳ�������̰߳�ȫ��,
    // �û����������Ҫ��֤���������.
    //
    // ����:
    //   RAE = kRegisterAtExit
    //
    // ���Traits::RAE��true, �����ڽ����˳���ʱ�������, ׼ȷ��˵����AtExitManager
    // ��Ҫʵ����һ���������͵Ķ���. AtExitManagerģ����atexit()��LIFO������嵫��
    // �����ϸ���ȫһЩ. �������ݲμ�at_exit.h.
    //
    // ���Traits::RAE��false, �����ڽ����˳���ʱ�򲻻��ͷ�, ��˻���й©(�����Ѵ���).
    // Traits::RAE�Ǳ�Ҫһ�㲻Ҫ����Ϊfalse, ��Ϊ�����Ķ�����ܻᱻCRT����.
    //
    // ����������һ������, ���๹�캯��private, ������DefaultSingletonTraits<>��Ԫ.
    //
    //   #include "singleton.h"
    //   class FooClass {
    //    public:
    //     static FooClass* GetInstance(); <--�μ������ע��.
    //     void Bar() { ... }
    //    private:
    //     FooClass() { ... }
    //     friend struct DefaultSingletonTraits<FooClass>;
    //
    //     DISALLOW_COPY_AND_ASSIGN(FooClass);
    //   };
    //
    // ��Դ�ļ���:
    //   FooClass* FooClass::GetInstance() {
    //    return Singleton<FooClass>::get();
    //   }
    //
    // ����FooClass�ķ���:
    //   FooClass::GetInstance()->Bar();
    //
    // ע��:
    // (a) ÿ�ε���get(),operator->()��operator*()�������(16ns on my P4/2.8GHz)����
    //     �ж϶����Ƿ��Ѿ���ʼ��. ���Ի���get()���, ��Ϊָ�벻��仯.
    //
    // (b) ����ʵ����������Ҫ�׳��쳣, ��Ϊ�಻���쳣��ȫ(exception-safe)��.
    template<typename Type, typename Traits=DefaultSingletonTraits<Type>,
        typename DifferentiatingType=Type>
    class Singleton
    {
    public:
        // ʹ��Singleton<T>������Ҫ����һ��GetInstance()��������Singleton::get().
        friend Type* Type::GetInstance();

        // ��������͸�ֵ�����ǰ�ȫ��, ��Ϊû���κγ�Ա.

        // ����Type���ʵ��ָ��
        static Type* get()
        {
            if(!Traits::kAllowedToAccessOnNonjoinableThread)
            {
                ThreadRestrictions::AssertSingletonAllowed();
            }

            // AtomicWordҲ��������������, kBeingCreatedMarker��ʾ�������ȴ�����.
            static const AtomicWord kBeingCreatedMarker = 1;

            AtomicWord value = NoBarrier_Load(&instance_);
            if(value!=0 && value!=kBeingCreatedMarker)
            {
                return reinterpret_cast<Type*>(value);
            }

            // ����û������, ���Դ���.
            if(Acquire_CompareAndSwap(&instance_, 0, kBeingCreatedMarker) == 0)
            {
                // instance_ΪNULL��������kBeingCreatedMarker. ֻ������һ���߳�������,
                // �����߳�ֻ��ѭ���ȴ�(spinning).
                Type* newval = Traits::New();

                Release_Store(&instance_, reinterpret_cast<AtomicWord>(newval));

                if(newval!=NULL && Traits::kRegisterAtExit)
                {
                    AtExitManager::RegisterCallback(OnExit, NULL);
                }

                return newval;
            }

            // ��ͻ, �����߳��Ѿ�:
            // - ���ڴ���
            // - �Ѿ��������
            // value != NULLʱ������kBeingCreatedMarker���ߺϷ�ָ��.
            // һ�㲻���ͻ, ����ʵ��������ǳ���ʱ, ��ʱ����ѭ���л��������߳�ֱ��
            // ���󴴽��ɹ�.
            while(true)
            {
                value = NoBarrier_Load(&instance_);
                if(value != kBeingCreatedMarker)
                {
                    break;
                }
                Sleep(0);
            }

            return reinterpret_cast<Type*>(value);
        }

    private:
        // AtExit()�����亯��. Ӧ���ڵ��߳��е���, ���ǰ����ܲ��Ǳ����.
        static void OnExit(void* unused)
        {
            // ����ʵ������֮��Ż���AtExit��ע��. instance_ָ��Ƿ��򲻻����.
            Traits::Delete(reinterpret_cast<Type*>(
                NoBarrier_AtomicExchange(&instance_, 0)));
        }
        static AtomicWord instance_;
    };

    template<typename Type, typename Traits, typename DifferentiatingType>
    AtomicWord Singleton<Type, Traits, DifferentiatingType>::instance_ = 0;

} //namespace base

#endif //__base_singleton_h__