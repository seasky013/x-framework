
#include "windows_version.h"

namespace base
{

    // static
    OSInfo* OSInfo::GetInstance()
    {
        return Singleton<OSInfo>::get();
    }

    OSInfo::OSInfo() : version_(WINVERSION_PRE_XP),
        architecture_(OTHER_ARCHITECTURE),
        wow64_status_(GetWOW64StatusForProcess(GetCurrentProcess()))
    {
        OSVERSIONINFOEX version_info = { sizeof(version_info) };
        GetVersionEx(reinterpret_cast<OSVERSIONINFO*>(&version_info));
        version_number_.major = version_info.dwMajorVersion;
        version_number_.minor = version_info.dwMinorVersion;
        version_number_.build = version_info.dwBuildNumber;
        if((version_number_.major==5) && (version_number_.minor>0))
        {
            version_ = (version_number_.minor==1) ? WINVERSION_XP :
                WINVERSION_SERVER_2003;
        }
        else if(version_number_.major == 6)
        {
            if(version_info.wProductType == VER_NT_WORKSTATION)
            {
                version_ = (version_number_.minor==0) ? WINVERSION_VISTA :
                    WINVERSION_WIN7;
            }
            else
            {
                version_ = WINVERSION_SERVER_2008;
            }
        }
        else if(version_number_.major > 6)
        {
            version_ = WINVERSION_WIN7;
        }
        service_pack_.major = version_info.wServicePackMajor;
        service_pack_.minor = version_info.wServicePackMinor;

        SYSTEM_INFO system_info = { 0 };
        GetNativeSystemInfo(&system_info);
        switch(system_info.wProcessorArchitecture)
        {
        case PROCESSOR_ARCHITECTURE_INTEL: architecture_ = X86_ARCHITECTURE; break;
        case PROCESSOR_ARCHITECTURE_AMD64: architecture_ = X64_ARCHITECTURE; break;
        case PROCESSOR_ARCHITECTURE_IA64:  architecture_ = IA64_ARCHITECTURE; break;
        }
        processors_ = system_info.dwNumberOfProcessors;
        allocation_granularity_ = system_info.dwAllocationGranularity;
    }

    OSInfo::~OSInfo() {}

    // static
    OSInfo::WOW64Status OSInfo::GetWOW64StatusForProcess(HANDLE process_handle)
    {
        typedef BOOL (WINAPI* IsWow64ProcessFunc)(HANDLE, PBOOL);
        IsWow64ProcessFunc is_wow64_process = reinterpret_cast<IsWow64ProcessFunc>(
            GetProcAddress(GetModuleHandle(L"kernel32.dll"), "IsWow64Process"));
        if(!is_wow64_process)
        {
            return WOW64_DISABLED;
        }
        BOOL is_wow64 = FALSE;
        if(!(*is_wow64_process)(process_handle, &is_wow64))
        {
            return WOW64_UNKNOWN;
        }
        return is_wow64 ? WOW64_ENABLED : WOW64_DISABLED;
    }

    WinVersion GetWinVersion()
    {
        return OSInfo::GetInstance()->version();
    }

} //namespace base