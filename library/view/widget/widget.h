
#ifndef __view_widget_h__
#define __view_widget_h__

#pragma once

#include <vector>

#include "base/memory/ref_counted.h"
#include "base/memory/scoped_ptr.h"

#include "../accessibility/accessibility_types.h"
#include "../focus/focus_manager.h"
#include "native_widget_delegate.h"

namespace gfx
{
    class Canvas;
    class Compositor;
    class Path;
    class Point;
    class Rect;
}

class Accelerator;
class OSExchangeData;
class ThemeProvider;

namespace view
{

    class DefaultThemeProvider;
    class NativeWidget;
    class RootView;
    class TooltipManager;
    class View;
    class WidgetDelegate;
    class Window;

    ////////////////////////////////////////////////////////////////////////////////
    // Widget class
    //
    //  Encapsulates the platform-specific rendering, event receiving and widget
    //  management aspects of the UI framework.
    //
    //  Owns a RootView and thus a View hierarchy. Can contain child Widgets.
    //  Widget is a platform-independent type that communicates with a platform or
    //  context specific NativeWidget implementation.
    //
    //  TODO(beng): Note that this class being non-abstract means that we have a
    //              violation of Google style in that we are using multiple
    //              inheritance. The intention is to split this into a separate
    //              object associated with but not equal to a NativeWidget
    //              implementation. Multiple inheritance is required for this
    //              transitional step.
    class Widget : public NativeWidgetDelegate, public FocusTraversable
    {
    public:
        enum TransparencyParam
        {
            Transparent,
            NotTransparent
        };

        enum EventsParam
        {
            AcceptEvents,
            NotAcceptEvents
        };

        enum DeleteParam
        {
            DeleteOnDestroy,
            NotDeleteOnDestroy
        };

        enum MirroringParam
        {
            MirrorOriginInRTL,
            DontMirrorOriginInRTL
        };

        virtual ~Widget();

        // Creates a transient popup widget specific to the current platform.
        // If |mirror_in_rtl| is set to MirrorOriginInRTL, the contents of the
        // popup will be mirrored if the current locale is RTL.  You should use
        // DontMirrorOriginInRTL if you are aleady handling the RTL layout within
        // the widget.
        static Widget* CreatePopupWidget(TransparencyParam transparent,
            EventsParam accept_events,
            DeleteParam delete_on_destroy,
            MirroringParam mirror_in_rtl);

        // Enumerates all windows pertaining to us and notifies their
        // view hierarchies that the locale has changed.
        static void NotifyLocaleChanged();

        Widget();

        // Unconverted methods -------------------------------------------------------

        // TODO(beng):
        // Widget subclasses are still implementing these methods by overriding from
        // here rather than by implementing NativeWidget.

        // Initialize the Widget with a parent and an initial desired size.
        // |contents_view| is the view that will be the single child of RootView
        // within this Widget. As contents_view is inserted into RootView's tree,
        // RootView assumes ownership of this view and cleaning it up. If you remove
        // this view, you are responsible for its destruction. If this value is NULL,
        // the caller is responsible for populating the RootView, and sizing its
        // contents as the window is sized.
        virtual void Init(HWND parent, const gfx::Rect& bounds);
        virtual void InitWithWidget(Widget* parent, const gfx::Rect& bounds);

        // Returns the gfx::NativeView associated with this Widget.
        virtual HWND GetNativeView() const;

        // Starts a drag operation for the specified view. |point| is a position in
        // |view| coordinates that the drag was initiated from.
        virtual void GenerateMousePressedForView(View* view,
            const gfx::Point& point);

        // Returns the accelerator given a command id. Returns false if there is
        // no accelerator associated with a given id, which is a common condition.
        virtual bool GetAccelerator(int cmd_id, MenuAccelerator* accelerator);

        // Returns the Window containing this Widget, or NULL if not contained in a
        // window.
        virtual Window* GetWindow();
        virtual const Window* GetWindow() const;

        // Forwarded from the RootView so that the widget can do any cleanup.
        virtual void ViewHierarchyChanged(bool is_add, View* parent, View* child);

        // Performs any necessary cleanup and forwards to RootView.
        virtual void NotifyNativeViewHierarchyChanged(bool attached,
            HWND native_view);

        // Converted methods ---------------------------------------------------------

        // TODO(beng):
        // Widget subclasses are implementing these methods by implementing
        // NativeWidget. Remove this comment once complete.

        // Returns the topmost Widget in a hierarchy. Will return NULL if called
        // before the underlying Native Widget has been initialized.
        Widget* GetTopLevelWidget();
        const Widget* GetTopLevelWidget() const;

        // Gets/Sets the WidgetDelegate.
        WidgetDelegate* widget_delegate() const { return widget_delegate_; }
        void set_widget_delegate(WidgetDelegate* widget_delegate)
        {
            widget_delegate_ = widget_delegate;
        }

        // Sets the specified view as the contents of this Widget. There can only
        // be one contents view child of this Widget's RootView. This view is sized to
        // fit the entire size of the RootView. The RootView takes ownership of this
        // View, unless it is set as not being parent-owned.
        void SetContentsView(View* view);

        // Returns the bounds of the Widget in screen coordinates.
        gfx::Rect GetWindowScreenBounds() const;

        // Returns the bounds of the Widget's client area in screen coordinates.
        gfx::Rect GetClientAreaScreenBounds() const;

        // Sizes and/or places the widget to the specified bounds, size or position.
        void SetBounds(const gfx::Rect& bounds);

        // Places the widget in front of the specified widget in z-order.
        void MoveAbove(Widget* widget);

        // Sets a shape on the widget. This takes ownership of shape.
        void SetShape(HRGN shape);

        // Hides the widget then closes it after a return to the message loop.
        void Close();

        // TODO(beng): Move off public API.
        // Closes the widget immediately. Compare to |Close|. This will destroy the
        // window handle associated with this Widget, so should not be called from
        // any code that expects it to be valid beyond this call.
        void CloseNow();

        // Shows or hides the widget, without changing activation state.
        void Show();
        void Hide();

        // Sets the opacity of the widget. This may allow widgets behind the widget
        // in the Z-order to become visible, depending on the capabilities of the
        // underlying windowing system. Note that the caller must then schedule a
        // repaint to allow this change to take effect.
        void SetOpacity(unsigned char opacity);

        // Sets the widget to be on top of all other widgets in the windowing system.
        void SetAlwaysOnTop(bool on_top);

        // Returns the RootView contained by this Widget.
        RootView* GetRootView();

        // Returns whether the Widget is visible to the user.
        bool IsVisible() const;

        // Returns whether the Widget is the currently active window.
        bool IsActive() const;

        // Returns whether the Widget is customized for accessibility.
        bool IsAccessibleWidget() const;

        // Returns the ThemeProvider that provides theme resources for this Widget.
        virtual ThemeProvider* GetThemeProvider() const;

        // Returns the FocusManager for this widget.
        // Note that all widgets in a widget hierarchy share the same focus manager.
        // TODO(beng): remove virtual.
        virtual FocusManager* GetFocusManager();

        // Returns true if the native view |native_view| is contained in the
        // views::View hierarchy rooted at this widget.
        // TODO(beng): const.
        bool ContainsNativeView(HWND native_view);

        // Starts a drag operation for the specified view. This blocks until the drag
        // operation completes. |view| can be NULL.
        // If the view is non-NULL it can be accessed during the drag by calling
        // dragged_view(). If the view has not been deleted during the drag,
        // OnDragDone() is called on it.
        void RunShellDrag(View* view, const OSExchangeData& data, int operation);

        // Returns the view that requested the current drag operation via
        // RunShellDrag(), or NULL if there is no such view or drag operation.
        View* dragged_view() { return dragged_view_; }

        // Adds the specified |rect| in client area coordinates to the rectangle to be
        // redrawn.
        void SchedulePaintInRect(const gfx::Rect& rect);

        // Sets the currently visible cursor. If |cursor| is NULL, the cursor used
        // before the current is restored.
        void SetCursor(HCURSOR cursor);

        // Retrieves the focus traversable for this widget.
        FocusTraversable* GetFocusTraversable();

        // Notifies the view hierarchy contained in this widget that theme resources
        // changed.
        void ThemeChanged();

        // Notifies the view hierarchy contained in this widget that locale resources
        // changed.
        void LocaleChanged();

        void SetFocusTraversableParent(FocusTraversable* parent);
        void SetFocusTraversableParentView(View* parent_view);

        // Notifies assistive technology that an accessibility event has
        // occurred on |view|, such as when the view is focused or when its
        // value changes. Pass true for |send_native_event| except for rare
        // cases where the view is a native control that's already sending a
        // native accessibility event and the duplicate event would cause
        // problems.
        virtual void NotifyAccessibilityEvent(
            View* view,
            AccessibilityTypes::Event event_type,
            bool send_native_event) = 0;

        NativeWidget* native_widget() { return native_widget_; }

        // Overridden from NativeWidgetDelegate:
        virtual void OnNativeFocus(HWND focused_view);
        virtual void OnNativeBlur(HWND focused_view);
        virtual void OnNativeWidgetCreated();
        virtual void OnSizeChanged(const gfx::Size& new_size);
        virtual bool HasFocusManager() const;
        virtual void OnNativeWidgetPaint(gfx::Canvas* canvas);

        // Overridden from FocusTraversable:
        virtual FocusSearch* GetFocusSearch();
        virtual FocusTraversable* GetFocusTraversableParent();
        virtual View* GetFocusTraversableParentView();

    protected:
        // Creates the RootView to be used within this Widget. Subclasses may override
        // to create custom RootViews that do specialized event processing.
        // TODO(beng): Investigate whether or not this is needed.
        virtual RootView* CreateRootView();

        // Provided to allow the WidgetWin/Gtk implementations to destroy the RootView
        // _before_ the focus manager/tooltip manager.
        // TODO(beng): remove once we fold those objects onto this one.
        void DestroyRootView();

        // TODO(beng): Temporarily provided as a way to associate the subclass'
        //             implementation of NativeWidget with this.
        void set_native_widget(NativeWidget* native_widget)
        {
            native_widget_ = native_widget;
        }

        // Used for testing.
        void ReplaceFocusManager(FocusManager* focus_manager);

    private:
        // Refresh the compositor tree. This is called by a View whenever its texture
        // is updated.
        void RefreshCompositeTree();

        // Try to create a compositor if one hasn't been created yet. Returns false if
        // a compositor couldn't be created.
        bool EnsureCompositor();

        NativeWidget* native_widget_;

        // Non-owned pointer to the Widget's delegate.  May be NULL if no delegate is
        // being used.
        WidgetDelegate* widget_delegate_;

        // The root of the View hierarchy attached to this window.
        // WARNING: see warning in tooltip_manager_ for ordering dependencies with
        // this and tooltip_manager_.
        scoped_ptr<RootView> root_view_;

        // The focus manager keeping track of focus for this Widget and any of its
        // children.  NULL for non top-level widgets.
        // WARNING: RootView's destructor calls into the FocusManager. As such, this
        // must be destroyed AFTER root_view_. This is enforced in DestroyRootView().
        scoped_ptr<FocusManager> focus_manager_;

        // A theme provider to use when no other theme provider is specified.
        scoped_ptr<DefaultThemeProvider> default_theme_provider_;

        // Valid for the lifetime of RunShellDrag(), indicates the view the drag
        // started from.
        View* dragged_view_;

        // The compositor for accelerated drawing.
        scoped_refptr<gfx::Compositor> compositor_;

        DISALLOW_COPY_AND_ASSIGN(Widget);
    };

} //namespace view

#endif //__view_widget_h__