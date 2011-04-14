
#ifndef __view_hwnd_util_h__
#define __view_hwnd_util_h__

#pragma once

#include <windows.h>

#include <string>

namespace gfx
{
    class Size;
}

namespace view
{

    // GetClassNameW API�ķ�װ, ���ش�������. ��ֵ��ʾ��ȡ����ʧ��.
    std::wstring GetClassName(HWND window);

    // ���໯����, ���ر��滻�Ĵ��ڹ���.
    WNDPROC SetWindowProc(HWND hwnd, WNDPROC wndproc);

    // Get/SetWindowLong(..., GWLP_USERDATA, ...)�ķ�װ.
    // ��������ǰ���û�����.
    void* SetWindowUserData(HWND hwnd, void* user_data);
    void* GetWindowUserData(HWND hwnd);

    // Returns true if the specified window is the current active top window or one
    // of its children.
    bool DoesWindowBelongToActiveWindow(HWND window);

    // Sizes the window to have a client or window size (depending on the value of
    // |pref_is_client|) of pref, then centers the window over parent, ensuring the
    // window fits on screen.
    void CenterAndSizeWindow(HWND parent, HWND window,
        const gfx::Size& pref, bool pref_is_client);

} //namespace view

#endif //__view_hwnd_util_h__