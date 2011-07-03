
#include "layer.h"

#include <algorithm>

#include "base/logging.h"

#include "compositor.h"

namespace ui
{

    Layer::Layer(Compositor* compositor)
        : compositor_(compositor),
        texture_(compositor->CreateTexture()),
        parent_(NULL) {}

    Layer::~Layer()
    {
        if(parent_)
        {
            parent_->Remove(this);
        }
        for(size_t i=0; i<children_.size(); ++i)
        {
            children_[i]->parent_ = NULL;
        }
    }

    void Layer::Add(Layer* child)
    {
        if(child->parent_)
        {
            child->parent_->Remove(child);
        }
        child->parent_ = this;
        children_.push_back(child);
    }

    void Layer::Remove(Layer* child)
    {
        std::vector<Layer*>::iterator i = std::find(children_.begin(),
            children_.end(), child);
        DCHECK(i != children_.end());
        children_.erase(i);
        child->parent_ = NULL;
    }

    void Layer::SetTexture(Texture* texture)
    {
        if(texture == NULL)
        {
            texture_ = compositor_->CreateTexture();
        }
        else
        {
            texture_ = texture;
        }
    }

    void Layer::SetBitmap(const SkBitmap& bitmap, const gfx::Point& origin)
    {
        texture_->SetBitmap(bitmap, origin, bounds_.size());
    }

    void Layer::Draw()
    {
        gfx::Transform transform;
        for(Layer* layer=this; layer; layer=layer->parent_)
        {
            transform.ConcatTransform(layer->transform_);
            transform.ConcatTranslate(static_cast<float>(layer->bounds_.x()),
                static_cast<float>(layer->bounds_.y()));
        }
        texture_->Draw(transform);
    }

} //namespace ui