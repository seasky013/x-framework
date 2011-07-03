
#include "native_widget_view.h"

#include "ui_gfx/canvas.h"

namespace view
{
    namespace internal
    {

        ////////////////////////////////////////////////////////////////////////////////
        // NativeWidgetView, public:

        // static
        const char NativeWidgetView::kViewClassName[] = "view/NativeWidgetView";

        NativeWidgetView::NativeWidgetView(NativeWidgetViews* native_widget)
            : native_widget_(native_widget), sent_create_(false) {}

        NativeWidgetView::~NativeWidgetView() {}

        Widget* NativeWidgetView::GetAssociatedWidget()
        {
            return native_widget_->delegate()->AsWidget();
        }

        ////////////////////////////////////////////////////////////////////////////////
        // NativeWidgetView, View overrides:

        void NativeWidgetView::SchedulePaintInternal(const gfx::Rect& r)
        {
            View::SchedulePaintInternal(r);
        }

        void NativeWidgetView::MarkLayerDirty() {
            View::MarkLayerDirty();
        }

        void NativeWidgetView::CalculateOffsetToAncestorWithLayer(gfx::Point* offset,
            View** ancestor)
        {
            View::CalculateOffsetToAncestorWithLayer(offset, ancestor);
        }

        void NativeWidgetView::ViewHierarchyChanged(bool is_add,
            View* parent, View* child)
        {
            if(is_add && child==this && !sent_create_)
            {
                sent_create_ = true;
                delegate()->OnNativeWidgetCreated();
            }
        }

        void NativeWidgetView::OnBoundsChanged(const gfx::Rect& previous_bounds)
        {
            delegate()->OnNativeWidgetSizeChanged(size());
        }

        void NativeWidgetView::OnPaint(gfx::Canvas* canvas)
        {
            canvas->FillRectInt(SK_ColorRED, 0, 0, width(), height());
            delegate()->OnNativeWidgetPaint(canvas);
        }

        bool NativeWidgetView::OnMousePressed(const MouseEvent& event)
        {
            MouseEvent e(event, this);
            return delegate()->OnMouseEvent(event);
        }

        bool NativeWidgetView::OnMouseDragged(const MouseEvent& event)
        {
            MouseEvent e(event, this);
            return delegate()->OnMouseEvent(event);
        }

        void NativeWidgetView::OnMouseReleased(const MouseEvent& event)
        {
            MouseEvent e(event, this);
            delegate()->OnMouseEvent(event);
        }

        void NativeWidgetView::OnMouseCaptureLost()
        {
            delegate()->OnMouseCaptureLost();
        }

        void NativeWidgetView::OnMouseMoved(const MouseEvent& event)
        {
            MouseEvent e(event, this);
            delegate()->OnMouseEvent(event);
        }

        void NativeWidgetView::OnMouseEntered(const MouseEvent& event)
        {
            MouseEvent e(event, this);
            delegate()->OnMouseEvent(event);
        }

        void NativeWidgetView::OnMouseExited(const MouseEvent& event)
        {
            MouseEvent e(event, this);
            delegate()->OnMouseEvent(event);
        }

        bool NativeWidgetView::OnKeyPressed(const KeyEvent& event)
        {
            return delegate()->OnKeyEvent(event);
        }

        bool NativeWidgetView::OnKeyReleased(const KeyEvent& event)
        {
            return delegate()->OnKeyEvent(event);
        }

        bool NativeWidgetView::OnMouseWheel(const MouseWheelEvent& event)
        {
            MouseWheelEvent e(event, this);
            return delegate()->OnMouseEvent(event);
        }

        void NativeWidgetView::OnFocus()
        {
            // TODO(beng): check if we have to do this.
            //delegate()->OnNativeFocus(NULL);
        }

        void NativeWidgetView::OnBlur()
        {
            // TODO(beng): check if we have to do this.
            //delegate()->OnNativeBlur(NULL);
        }

        std::string NativeWidgetView::GetClassName() const
        {
            return kViewClassName;
        }

        void NativeWidgetView::MoveLayerToParent(ui::Layer* parent_layer,
            const gfx::Point& point)
        {
            View::MoveLayerToParent(parent_layer, point);
            if(!layer() || parent_layer==layer())
            {
                gfx::Point new_offset(point);
                if(layer() != parent_layer)
                {
                    new_offset.Offset(x(), y());
                }
                GetAssociatedWidget()->GetRootView()->MoveLayerToParent(
                    parent_layer, new_offset);
            }
        }

        void NativeWidgetView::DestroyLayerRecurse()
        {
            GetAssociatedWidget()->GetRootView()->DestroyLayerRecurse();
            View::DestroyLayerRecurse();
        }

        void NativeWidgetView::UpdateLayerBounds(const gfx::Point& offset)
        {
            View::UpdateLayerBounds(offset);
            if(!layer())
            {
                gfx::Point new_offset(offset.x()+x(), offset.y()+y());
                GetAssociatedWidget()->GetRootView()->UpdateLayerBounds(new_offset);
            }
        }

        void NativeWidgetView::PaintToLayer(const gfx::Rect& dirty_rect)
        {
            View::PaintToLayer(dirty_rect);

            View* root = GetAssociatedWidget()->GetRootView();
            gfx::Rect root_dirty_rect = dirty_rect;
            root->GetTransform().TransformRectReverse(&root_dirty_rect);
            root_dirty_rect =
                gfx::Rect(gfx::Point(), root->size()).Intersect(root_dirty_rect);

            if(!root_dirty_rect.IsEmpty())
            {
                root->PaintToLayer(root_dirty_rect);
            }
        }

        void NativeWidgetView::PaintComposite()
        {
            View::PaintComposite();
            GetAssociatedWidget()->GetRootView()->PaintComposite();
        }

    } //namespace internal
} //namespace view