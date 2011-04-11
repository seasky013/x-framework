
#ifndef __base_version_h__
#define __base_version_h__

#pragma once

#include <string>
#include <vector>

#include "basic_types.h"

class Version
{
public:
    // version�ַ�����������1���߶����'.'�ֿ���uint16���. �������ָ�ʽ��
    // ����NULL. �����߸����ͷ�Version����.
    static Version* GetVersionFromString(const std::string& version_str);

    // ��¶�ӿ�ֻ��Ϊ���ܴ洢��STL����, ����ķ������ö���DCHECK.
    Version();
    ~Version();

    // ����һ�ݿ���. �����߽ӹ�����Ȩ.
    Version* Clone() const;

    bool Equals(const Version& other) const;

    // ���� -1, 0, 1 ��ʾ <, ==, >.
    int CompareTo(const Version& other) const;

    // ����version�ַ���.
    const std::string GetString() const;

    const std::vector<uint16>& components() const { return components_; }

private:
    bool InitFromString(const std::string& version_str);

    bool is_valid_;
    std::vector<uint16> components_;
};

#endif //__base_version_h__