
#ifndef __base__ref_counted_memory_h__
#define __base__ref_counted_memory_h__

#pragma once

#include <vector>

#include "ref_counted.h"


// �ڴ��ͨ�ýӿ�. ����������ü���, ��Ϊ����������������һ��ӵ������,
// ������Ҫ��̬��ʵ���������ڴ�����.
class RefCountedMemory : public base::RefCountedThreadSafe<RefCountedMemory>
{
public:
    // �������ݵ���ʼ��ַָ��. �������Ϊ��, ����NULL.
    virtual const unsigned char* front() const = 0;

    // �ڴ��С.
    virtual size_t size() const = 0;

protected:
    friend class base::RefCountedThreadSafe<RefCountedMemory>;
    RefCountedMemory();
    virtual ~RefCountedMemory();
};

// ���ü����޹ص�RefCountedMemoryʵ��.
class RefCountedStaticMemory : public RefCountedMemory
{
public:
    RefCountedStaticMemory() : data_(NULL), length_(0) {}
    RefCountedStaticMemory(const unsigned char* data, size_t length)
        : data_(data), length_(length) {}

    // ������RefCountedMemory:
    virtual const unsigned char* front() const;
    virtual size_t size() const;

private:
    const unsigned char* data_;
    size_t length_;

    DISALLOW_COPY_AND_ASSIGN(RefCountedStaticMemory);
};

// ��vector�������ݵ�RefCountedMemoryʵ��.
class RefCountedBytes : public RefCountedMemory
{
public:
    RefCountedBytes();

    // ͨ������|initializer|����һ��RefCountedBytes����.
    RefCountedBytes(const std::vector<unsigned char>& initializer);

    // ͨ��swap����һ��RefCountedBytes����.
    static RefCountedBytes* TakeVector(std::vector<unsigned char>* to_destroy);

    // ������RefCountedMemory:
    virtual const unsigned char* front() const;
    virtual size_t size() const;

    std::vector<unsigned char> data;

protected:
    friend class base::RefCountedThreadSafe<RefCountedBytes>;
    virtual ~RefCountedBytes();

private:
    DISALLOW_COPY_AND_ASSIGN(RefCountedBytes);
};

#endif //__base__ref_counted_memory_h__