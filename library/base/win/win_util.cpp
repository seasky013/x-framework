
#include "win_util.h"

#include <sddl.h>

#include "../scoped_ptr.h"
#include "../stringprintf.h"
#include "../threading/thread_restrictions.h"
#include "registry.h"
#include "scoped_handle.h"
#include "windows_version.h"

namespace base
{

#define SIZEOF_STRUCT_WITH_SPECIFIED_LAST_MEMBER(struct_name, member) \
    offsetof(struct_name, member) + \
    (sizeof static_cast<struct_name*>(NULL)->member)
#define NONCLIENTMETRICS_SIZE_PRE_VISTA \
    SIZEOF_STRUCT_WITH_SPECIFIED_LAST_MEMBER(NONCLIENTMETRICS, lfMessageFont)

    void GetNonClientMetrics(NONCLIENTMETRICS* metrics)
    {
        DCHECK(metrics);

        static const UINT SIZEOF_NONCLIENTMETRICS =
            (GetWinVersion()>=WINVERSION_VISTA) ?
            sizeof(NONCLIENTMETRICS) : NONCLIENTMETRICS_SIZE_PRE_VISTA;
        metrics->cbSize = SIZEOF_NONCLIENTMETRICS;
        const bool success = !!SystemParametersInfo(SPI_GETNONCLIENTMETRICS,
            SIZEOF_NONCLIENTMETRICS, metrics, 0);
        DCHECK(success);
    }

    bool GetUserSidString(std::wstring* user_sid)
    {
        // ��ȡ��ǰ����.
        HANDLE token = NULL;
        if(!::OpenProcessToken(::GetCurrentProcess(), TOKEN_QUERY, &token))
        {
            return false;
        }
        ScopedHandle token_scoped(token);

        DWORD size = sizeof(TOKEN_USER) + SECURITY_MAX_SID_SIZE;
        scoped_array<BYTE> user_bytes(new BYTE[size]);
        TOKEN_USER* user = reinterpret_cast<TOKEN_USER*>(user_bytes.get());

        if(!::GetTokenInformation(token, TokenUser, user, size, &size))
        {
            return false;
        }

        if(!user->User.Sid)
        {
            return false;
        }

        // ����ת����һ���ַ���.
        wchar_t* sid_string;
        if(!::ConvertSidToStringSid(user->User.Sid, &sid_string))
        {
            return false;
        }

        *user_sid = sid_string;

        ::LocalFree(sid_string);

        return true;
    }

    bool IsShiftPressed()
    {
        return (::GetKeyState(VK_SHIFT)&0x8000) == 0x8000;
    }

    bool IsCtrlPressed()
    {
        return (::GetKeyState(VK_CONTROL)&0x8000) == 0x8000;
    }

    bool IsAltPressed()
    {
        return (::GetKeyState(VK_MENU)&0x8000) == 0x8000;
    }

    bool UserAccountControlIsEnabled()
    {
        // ���Windows�˳�д���̵�ʱ���Ƚ���. Ӧ��ֻ��ȡһ��, Ȼ����Ӽ��ı仯,
        // ��������ļ��߳�.
        //   http://code.google.com/p/chromium/issues/detail?id=61644
        base::ThreadRestrictions::ScopedAllowIO allow_io;

        base::RegKey key(HKEY_LOCAL_MACHINE,
            L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Policies\\System",
            KEY_READ);
        DWORD uac_enabled;
        if(key.ReadValueDW(L"EnableLUA", &uac_enabled) != ERROR_SUCCESS)
        {
            return true;
        }
        // �û���������EnableLUA��Ϊ����ֵ, ����2, Vista��Ϊ�������������UAC,
        // ��������ֻ��Ҫȷ���Ƿ�Ϊ��0.
        return (uac_enabled != 0);
    }

    std::wstring FormatMessage(unsigned messageid)
    {
        wchar_t* string_buffer = NULL;
        unsigned string_length = ::FormatMessage(
            FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM|
            FORMAT_MESSAGE_IGNORE_INSERTS, NULL, messageid, 0,
            reinterpret_cast<wchar_t*>(&string_buffer), 0, NULL);

        std::wstring formatted_string;
        if(string_buffer)
        {
            formatted_string = string_buffer;
            LocalFree(reinterpret_cast<HLOCAL>(string_buffer));
        }
        else
        {
            // ��ʽ������. �򵥵ĸ�ʽ����Ϣֵ���ַ�����.
            SStringPrintf(&formatted_string, L"message number %d", messageid);
        }
        return formatted_string;
    }

    std::wstring FormatLastWin32Error()
    {
        return FormatMessage(GetLastError());
    }

} //namespace base