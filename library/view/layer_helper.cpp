
#include "layer_helper.h"

#include "ui_gfx/transform.h"

#include "ui_base/compositor/layer.h"

#include "layer_property_setter.h"

namespace view
{
    namespace internal
    {

        LayerHelper::LayerHelper()
            : bitmap_needs_updating_(true),
            fills_bounds_opaquely_(false),
            layer_updated_externally_(false),
            paint_to_layer_(false),
            property_setter_explicitly_set_(false),
            needs_paint_all_(true) {}

        LayerHelper::~LayerHelper()
        {
            if(property_setter_.get() && layer_.get())
            {
                property_setter_->Uninstalled(layer_.get());
            }
            property_setter_.reset();
            layer_.reset();
        }

        void LayerHelper::SetTransform(const gfx::Transform& transform)
        {
            transform_.reset(transform.HasChange() ?
                new gfx::Transform(transform) : NULL);
        }

        void LayerHelper::SetLayer(ui::Layer* layer)
        {
            if(property_setter_.get() && this->layer())
            {
                property_setter_->Uninstalled(this->layer());
            }
            layer_.reset(layer);
            if(layer)
            {
                if(!property_setter_.get())
                {
                    property_setter_.reset(LayerPropertySetter::CreateDefaultSetter());
                }
                property_setter_->Installed(this->layer());
            }
            else if(!property_setter_explicitly_set_)
            {
                property_setter_.reset(NULL);
            }
        }

        void LayerHelper::SetPropertySetter(LayerPropertySetter* setter)
        {
            if(property_setter_.get() && layer())
            {
                property_setter_->Uninstalled(layer());
            }
            property_setter_.reset(setter);
            if(layer())
            {
                if(!setter)
                {
                    property_setter_.reset(
                        LayerPropertySetter::CreateDefaultSetter());
                }
                property_setter_->Installed(layer());
            }
        }

        bool LayerHelper::ShouldPaintToLayer() const
        {
            return paint_to_layer_ || (transform_.get() && transform_->HasChange());
        }

    } //namespace internal
} //namespace view