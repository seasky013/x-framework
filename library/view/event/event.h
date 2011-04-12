
#ifndef __view_framework_event_h__
#define __view_framework_event_h__

#pragma once

#include "base/basic_types.h"
#include "base/time.h"

#include "gfx/point.h"

#include "../keycodes/keyboard_codes_win.h"
#include "events.h"

class OSExchangeData;

namespace view
{

    class RootView;
    class View;

    ////////////////////////////////////////////////////////////////////////////////
    //
    // Event��
    //
    // �����¼��ķ�װ, ��view���д���. �¼��������͡�һЩ��Ǻ�ʱ���.
    // ÿ����Ҫ���¼����ж�Ӧ������.
    // �¼��ǲ��ɱ��, ����֧�ֿ���.
    //
    ////////////////////////////////////////////////////////////////////////////////
    class Event
    {
    public:
        const MSG& native_event() const { return native_event_; }
        EventType type() const { return type_; }
        const base::Time& time_stamp() const { return time_stamp_; }
        int flags() const { return flags_; }
        void set_flags(int flags) { flags_ = flags; }

        // ����shift���Ƿ���.
        bool IsShiftDown() const
        {
            return (flags_ & EF_SHIFT_DOWN) != 0;
        }

        // ����control���Ƿ���.
        bool IsControlDown() const
        {
            return (flags_ & EF_CONTROL_DOWN) != 0;
        }

        // ����caps���Ƿ�����.
        bool IsCapsLockDown() const
        {
            return (flags_ & EF_CAPS_LOCK_DOWN) != 0;
        }

        // ����alt���Ƿ���.
        bool IsAltDown() const
        {
            return (flags_ & EF_ALT_DOWN) != 0;
        }

        bool IsMouseEvent() const
        {
            return type_==ET_MOUSE_PRESSED ||
                type_==ET_MOUSE_DRAGGED ||
                type_==ET_MOUSE_RELEASED ||
                type_==ET_MOUSE_MOVED ||
                type_==ET_MOUSE_ENTERED ||
                type_==ET_MOUSE_EXITED ||
                type_==ET_MOUSEWHEEL;
        }

        // ��windows�ı�Ǹ�ʽ����EventFlags.
        int GetWindowsFlags() const;

    protected:
        Event(EventType type, int flags);
        Event(MSG native_event, EventType type, int flags);

        Event(const Event& model)
            : native_event_(model.native_event()),
            type_(model.type()),
            time_stamp_(model.time_stamp()),
            flags_(model.flags()) {}

    private:
        void operator=(const Event&);

        // ��ʼ�����Ա����.
        void Init();
        void InitWithNativeEvent(MSG native_event);

        MSG native_event_;
        EventType type_;
        base::Time time_stamp_;
        int flags_;
    };

    ////////////////////////////////////////////////////////////////////////////////
    //
    // LocatedEvent��
    //
    // �¼����ڶ�λ��Ļ�ϵ�ָ��λ��.
    //
    ////////////////////////////////////////////////////////////////////////////////
    class LocatedEvent : public Event
    {
    public:
        int x() const { return location_.x(); }
        int y() const { return location_.y(); }
        const gfx::Point& location() const { return location_; }

    protected:
        explicit LocatedEvent(MSG native_event);

        LocatedEvent(EventType type, const gfx::Point& location, int flags);

        // ����һ����model��ͬ��LocatedEvent. ����ṩ��source/target��ͼ, modelλ��
        // ��'source'����ϵͳת����'target'����ϵͳ.
        LocatedEvent(const LocatedEvent& model, View* source, View* target);

        // ������캯�����¼�λ�ô�widget����ת����RootView����.
        LocatedEvent(const LocatedEvent& model, RootView* root);

        gfx::Point location_;
    };

    ////////////////////////////////////////////////////////////////////////////////
    //
    // MouseEvent��
    //
    // �����ص������¼�.
    //
    ////////////////////////////////////////////////////////////////////////////////
    class MouseEvent : public LocatedEvent
    {
    public:
        explicit MouseEvent(MSG native_event);

        // ����һ����model��ͬ��MouseEvent. ����ṩ��from/to��ͼ, modelλ��
        // ��'from'����ϵͳת����'to'����ϵͳ.
        MouseEvent(const MouseEvent& model, View* from, View* to);

        // �����µ�����¼�.
        MouseEvent(EventType type, int x, int y, int flags)
            : LocatedEvent(type, gfx::Point(x, y), flags) {}

        // �������ͺ͵����괴������¼�. ����ṩ��from/to��ͼ, �����꽫���'from'
        // ����ϵͳת����'to'����ϵͳ.
        MouseEvent(EventType type,
            View* from,
            View* to,
            const gfx::Point& l,
            int flags);

        // ���ڿ����ж��ĸ�������.
        bool IsOnlyLeftMouseButton() const
        {
            return (flags()&EF_LEFT_BUTTON_DOWN) &&
                !(flags()&(EF_MIDDLE_BUTTON_DOWN|EF_RIGHT_BUTTON_DOWN));
        }

        bool IsLeftMouseButton() const
        {
            return (flags() & EF_LEFT_BUTTON_DOWN) != 0;
        }

        bool IsOnlyMiddleMouseButton() const
        {
            return (flags()&EF_MIDDLE_BUTTON_DOWN) &&
                !(flags()&(EF_LEFT_BUTTON_DOWN|EF_RIGHT_BUTTON_DOWN));
        }

        bool IsMiddleMouseButton() const
        {
            return (flags()&EF_MIDDLE_BUTTON_DOWN) != 0;
        }

        bool IsOnlyRightMouseButton() const
        {
            return (flags()&EF_RIGHT_BUTTON_DOWN) &&
                !(flags()&(EF_LEFT_BUTTON_DOWN|EF_MIDDLE_BUTTON_DOWN));
        }

        bool IsRightMouseButton() const
        {
            return (flags()&EF_RIGHT_BUTTON_DOWN) != 0;
        }

    protected:
        MouseEvent(const MouseEvent& model, RootView* root)
            : LocatedEvent(model, root) {}

    private:
        friend class RootView;

        DISALLOW_COPY_AND_ASSIGN(MouseEvent);
    };

    ////////////////////////////////////////////////////////////////////////////////
    //
    // KeyEvent��
    //
    // ������ص������¼�.
    // ע��: �¼��ǰ�����ص�, �����������ַ�.
    //
    ////////////////////////////////////////////////////////////////////////////////
    class KeyEvent : public Event
    {
    public:
        explicit KeyEvent(MSG native_event);

        KeyEvent(EventType type, KeyboardCode key_code, int event_flags);

        KeyboardCode key_code() const { return key_code_; }

    private:
        KeyboardCode key_code_;

        DISALLOW_COPY_AND_ASSIGN(KeyEvent);
    };

    ////////////////////////////////////////////////////////////////////////////////
    //
    // MouseWheelEvent��
    //
    // �����������û��¼�.
    //
    ////////////////////////////////////////////////////////////////////////////////
    class MouseWheelEvent : public MouseEvent
    {
    public:
        // ��ϸ�μ�|offset|.
        static const int kWheelDelta;

        explicit MouseWheelEvent(MSG native_event);

        // ������. ��kWheelDelta�ı���.
        int offset() const { return offset_; }

    private:
        friend class RootView;

        MouseWheelEvent(const MouseWheelEvent& model, RootView* root)
            : MouseEvent(model, root), offset_(model.offset_) {}

        int offset_;

        DISALLOW_COPY_AND_ASSIGN(MouseWheelEvent);
    };

    ////////////////////////////////////////////////////////////////////////////////
    //
    // DropTargetEvent��
    //
    // ����ק����ʱ���͵������ͣ����ͼ.
    //
    ////////////////////////////////////////////////////////////////////////////////
    class DropTargetEvent : public LocatedEvent
    {
    public:
        DropTargetEvent(const OSExchangeData& data,
            int x,
            int y,
            int source_operations)
            : LocatedEvent(ET_DROP_TARGET_EVENT, gfx::Point(x, y), 0),
            data_(data),
            source_operations_(source_operations) {}

        const OSExchangeData& data() const { return data_; }
        int source_operations() const { return source_operations_; }

    private:
        // ��ק�����й���������.
        const OSExchangeData& data_;

        // ֧����קԴ��DragDropTypes::DragOperation������λ����.
        int source_operations_;

        DISALLOW_COPY_AND_ASSIGN(DropTargetEvent);
    };

} //namespace view

#endif //__view_framework_event_h__