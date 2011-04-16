
#ifndef __view_accelerator_h__
#define __view_accelerator_h__

#pragma once

#include "base/string16.h"

#include "../event/event.h"

// Accelerator���������̼��ټ�(���߼��̿�ݼ�).
// Keyboard��ݼ�ͨ��FocusManagerע��. �п����͸�ֵ���캯��, ֧�ֿ���.
// ������<������, ����std::map��ʹ��.

namespace view
{

    // �˵����ټ��Ŀ�ƽ̨����. ���뱻����ʵ��.
    class MenuAccelerator
    {
    public:
        MenuAccelerator() : key_code_(VKEY_UNKNOWN), modifiers_(0) {}

        MenuAccelerator(KeyboardCode keycode, int modifiers)
            : key_code_(keycode), modifiers_(modifiers) {}

        MenuAccelerator(const MenuAccelerator& accelerator)
        {
            key_code_ = accelerator.key_code_;
            modifiers_ = accelerator.modifiers_;
        }

        virtual ~MenuAccelerator() {}

        MenuAccelerator& operator=(const MenuAccelerator& accelerator)
        {
            if(this != &accelerator)
            {
                key_code_ = accelerator.key_code_;
                modifiers_ = accelerator.modifiers_;
            }
            return *this;
        }

        // ����<������, ����std::map��ʹ��.
        bool operator <(const MenuAccelerator& rhs) const
        {
            if(key_code_ != rhs.key_code_)
            {
                return key_code_ < rhs.key_code_;
            }
            return modifiers_ < rhs.modifiers_;
        }

        bool operator ==(const MenuAccelerator& rhs) const
        {
            return (key_code_==rhs.key_code_) && (modifiers_==rhs.modifiers_);
        }

        bool operator !=(const MenuAccelerator& rhs) const
        {
            return !(*this == rhs);
        }

        KeyboardCode GetKeyCode() const
        {
            return key_code_;
        }

        int modifiers() const
        {
            return modifiers_;
        }

    protected:
        // ����(VK_...).
        KeyboardCode key_code_;

        // Shift/Ctrl/Alt����״̬(ƽ̨��ص�).
        int modifiers_;
    };

    class Accelerator : public MenuAccelerator
    {
    public:
        Accelerator() : MenuAccelerator() {}

        Accelerator(KeyboardCode keycode, int modifiers)
            : MenuAccelerator(keycode, modifiers) {}

        Accelerator(KeyboardCode keycode, bool shift_pressed,
            bool ctrl_pressed, bool alt_pressed)
        {
            key_code_ = keycode;
            modifiers_ = 0;
            if(shift_pressed)
            {
                modifiers_ |= EF_SHIFT_DOWN;
            }
            if(ctrl_pressed)
            {
                modifiers_ |= EF_CONTROL_DOWN;
            }
            if(alt_pressed)
            {
                modifiers_ |= EF_ALT_DOWN;
            }
        }

        virtual ~Accelerator() {}

        bool IsShiftDown() const
        {
            return (modifiers_&EF_SHIFT_DOWN) == EF_SHIFT_DOWN;
        }

        bool IsCtrlDown() const
        {
            return (modifiers_&EF_CONTROL_DOWN) == EF_CONTROL_DOWN;
        }

        bool IsAltDown() const
        {
            return (modifiers_&EF_ALT_DOWN) == EF_ALT_DOWN;
        }

        // Returns a string with the localized shortcut if any.
        string16 GetShortcutText() const;
    };

    // ��Ҫע����̼��ټ�������Ҫʵ�ֱ��ӿ�.
    class AcceleratorTarget
    {
    public:
        // ������ټ�������, ����Ӧ�÷���true.
        virtual bool AcceleratorPressed(const Accelerator& accelerator) = 0;

    protected:
        virtual ~AcceleratorTarget() {}
    };

} //namespace view

#endif //__view_accelerator_h__