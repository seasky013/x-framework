
#ifndef __base_win_windows_version_h__
#define __base_win_windows_version_h__

#pragma once

typedef void* HANDLE;

namespace base
{

    // ע��: ����ö��˳���Ա�����߿���"if(GetWinVersion() > WINVERSION_2000) ...".
    // ö��ֵ�ǿ����޸ĵ�.
    enum WinVersion
    {
        WINVERSION_PRE_2000 = 0, // ��֧��.
        WINVERSION_2000 = 1, // ��֧��.
        WINVERSION_XP = 2,
        WINVERSION_SERVER_2003 = 3,
        WINVERSION_VISTA = 4,
        WINVERSION_2008 = 5,
        WINVERSION_WIN7 = 6,
    };

    // ���ص�ǰ���е�Windows�汾.
    WinVersion GetWinVersion();

    // ���ذ�װ��SP������Ҫ�ʹ�Ҫ�汾��.
    void GetServicePackLevel(int* major, int* minor);

    enum WindowsArchitecture
    {
        X86_ARCHITECTURE,
        X64_ARCHITECTURE,
        IA64_ARCHITECTURE,
        OTHER_ARCHITECTURE,
    };

    // ����Windows����ʹ�õĴ������ܹ�.
    // �������x64���ݵĴ�����, ���������ֿ���:
    //   32����������32λ��Windows:                 X86_ARCHITECTURE
    //   32����ͨ��WOW64������64λ��Windows:        X64_ARCHITECTURE
    //   64����������64λ��Windows:                 X64_ARCHITECTURE
    WindowsArchitecture GetWindowsArchitecture();

    enum WOW64Status
    {
        WOW64_DISABLED,
        WOW64_ENABLED,
        WOW64_UNKNOWN,
    };

    // ���ش������Ƿ�������WOW64(64λ�汾��Windowsģ��32λ������)��. ����"32λ����
    // ��32λ��Windows��ִ��"��"64λ������64λ��Windows��ִ��"������WOW64_DISABLED.
    // WOW64_UNKNOWN��ʾ"�д�����", ������̵�Ȩ�޲���.
    WOW64Status GetWOW64Status();

    // ��GetWOW64Status()����, �ж�ָ������״̬�����ǵ�ǰ����.
    WOW64Status GetWOW64StatusForProcess(HANDLE process_handle);

} //namespace base

#endif //__base_win_windows_version_h__