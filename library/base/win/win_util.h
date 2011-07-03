
#ifndef __base_win_util_h__
#define __base_win_util_h__

#pragma once

#include <windows.h>

#include "base/string16.h"

namespace base
{
    namespace win
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

        // Adds the specified |command| using the specified |name| to the AutoRun key.
        // |root_key| could be HKCU or HKLM or the root of any user hive.
        bool AddCommandToAutoRun(HKEY root_key, const string16& name,
            const string16& command);
        // Removes the command specified by |name| from the AutoRun key. |root_key|
        // could be HKCU or HKLM or the root of any user hive.
        bool RemoveCommandFromAutoRun(HKEY root_key, const string16& name);

        // Reads the command specified by |name| from the AutoRun key. |root_key|
        // could be HKCU or HKLM or the root of any user hive.
        bool ReadCommandFromAutoRun(HKEY root_key, const string16& name,
            string16* command);

    } //namespace win
} //namespace base

#endif //__base_win_util_h__