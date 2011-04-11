
#ifndef __base_win_win_util_h__
#define __base_win_win_util_h__

#pragma once

#include <windows.h>

#include <string>

namespace base
{

    void GetNonClientMetrics(NONCLIENTMETRICS* metrics);

    // ���ص�ǰ�û���sid�ַ���.
    bool GetUserSidString(std::wstring* user_sid);

    // ����shift����ǰ�Ƿ񱻰���.
    bool IsShiftPressed();

    // ����ctrl����ǰ�Ƿ񱻰���.
    bool IsCtrlPressed();

    // ����alt����ǰ�Ƿ񱻰���.
    bool IsAltPressed();

    // ����û��ʻ�����(UAC)�Ѿ�ͨ��ע���EnableLUA��ֵ�����÷���false. �������
    // ����true.
    // ע��: EnableLUA��ֵ��Windows XP���Ǳ����Ե�, �����ܴ��ڲ������ó�0(����UAC),
    // ��ʱ�᷵��false. ����ǰ��Ҫ������ϵͳΪVista�����Ժ�İ汾.
    bool UserAccountControlIsEnabled();

    // ʹ��FormatMessage() API�����ַ���, ʹ��Windowsȱʡ����Ϣ������Դ;
    // ����%ռλ��.
    std::wstring FormatMessage(unsigned messageid);

    // ʹ��GetLastError()���ɿɶ�����Ϣ�ַ���.
    std::wstring FormatLastWin32Error();

} //namespace base

#endif //__base_win_win_util_h__