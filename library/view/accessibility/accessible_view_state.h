
#ifndef __view_accessible_view_state_h__
#define __view_accessible_view_state_h__

#pragma once

#include "base/string16.h"

#include "accessibility_types.h"

////////////////////////////////////////////////////////////////////////////////
//
// AccessibleViewState
//
//   �洢�ɷ�����Ϣ�Ŀ�ƽ̨�ṹ��, ΪUI��ͼ�ṩ��������(AT).
//
////////////////////////////////////////////////////////////////////////////////
struct AccessibleViewState
{
public:
    AccessibleViewState();
    ~AccessibleViewState();

    // ��ͼ�Ľ�ɫ, ��button����list box.
    AccessibilityTypes::Role role;

    // ��ͼ��״̬, ��ѡ��(��ѡ��)�ͱ���(�����)�����ֶε�λ��.
    AccessibilityTypes::State state;

    // ��ͼ������/��ǩ.
    string16 name;

    // ��ͼ��ֵ, �����ı�����.
    string16 value;

    // �û������ͼ��ȱʡ������.
    string16 default_action;

    // ������ͼ�ļ��̿�ݼ�.
    string16 keyboard_shortcut;

    // ѡ�е���ʼ����λ��. ֻ�Դ����ı����ݵ���ͼ��Ч, �����ı��������Ͽ�;
    // ��������ʼ�ͽ�������-1.
    int selection_start;
    int selection_end;

    // The selected item's index and the count of the number of items.
    // Only applies to views with multiple choices like a listbox; both
    // index and count should be -1 otherwise.
    // ѡ����Ŀ������������Ŀ��. ֻ�Զ�ѡ��ͼ��Ч, �����б�; ��������ͼ
    // ��Ϊ-1.
    int index;
    int count;
};

#endif //__view_accessible_view_state_h__