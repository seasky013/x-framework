
#include "image.h"

#include "base/logging.h"

#include "SkBitmap.h"

namespace gfx
{

    class SkBitmapRep;

    // ImageRep�洢ͼ����ڴ��. ÿ��RepresentationType��һ��ImageRep������, ����
    // �ͷ��ڴ�. ����һ��ImageRep��, ��ӹ�ͼ�������Ȩ, ����ά�������������ü���.
    class ImageRep
    {
    public:
        explicit ImageRep(Image::RepresentationType rep) : type_(rep) {}

        // ɾ��ImageRep����������.
        virtual ~ImageRep() {}

        // ת������("�ٵ�RTTI").
        SkBitmapRep* AsSkBitmapRep()
        {
            CHECK_EQ(type_, Image::kSkBitmapRep);
            return reinterpret_cast<SkBitmapRep*>(this);
        }

        Image::RepresentationType type() const { return type_; }

    private:
        Image::RepresentationType type_;
    };

    class SkBitmapRep : public ImageRep
    {
    public:
        explicit SkBitmapRep(const SkBitmap* bitmap)
            : ImageRep(Image::kSkBitmapRep),
            bitmap_(bitmap)
        {
            CHECK(bitmap);
        }

        virtual ~SkBitmapRep()
        {
            delete bitmap_;
            bitmap_ = NULL;
        }

        const SkBitmap* bitmap() const { return bitmap_; }

    private:
        const SkBitmap* bitmap_;

        DISALLOW_COPY_AND_ASSIGN(SkBitmapRep);
    };


    Image::Image(const SkBitmap* bitmap)
        : default_representation_(Image::kSkBitmapRep)
    {
        SkBitmapRep* rep = new SkBitmapRep(bitmap);
        AddRepresentation(rep);
    }

    Image::~Image()
    {
        for(RepresentationMap::iterator it=representations_.begin();
            it!=representations_.end(); ++it)
        {
            delete it->second;
        }
        representations_.clear();
    }

    Image::operator const SkBitmap*()
    {
        ImageRep* rep = GetRepresentation(Image::kSkBitmapRep);
        return rep->AsSkBitmapRep()->bitmap();
    }

    Image::operator const SkBitmap&()
    {
        return *(this->operator const SkBitmap*());
    }

    bool Image::HasRepresentation(RepresentationType type)
    {
        return representations_.count(type) != 0;
    }

    void Image::SwapRepresentations(gfx::Image* other)
    {
        representations_.swap(other->representations_);
        std::swap(default_representation_, other->default_representation_);
    }

    ImageRep* Image::DefaultRepresentation()
    {
        RepresentationMap::iterator it =
            representations_.find(default_representation_);
        DCHECK(it != representations_.end());
        return it->second;
    }

    ImageRep* Image::GetRepresentation(RepresentationType rep_type)
    {
        // �������ȱʡ��, ֱ�ӷ���.
        ImageRep* default_rep = DefaultRepresentation();
        if(rep_type == default_representation_)
        {
            return default_rep;
        }

        // ����Ƿ��Ѿ�����.
        RepresentationMap::iterator it = representations_.find(rep_type);
        if(it != representations_.end())
        {
            return it->second;
        }

        // ��ʱ, ��Ҫת�������Ͳ�����, �����ȱʡ������ת��.

        // �����ص�Skia��ת��.
        if(rep_type == Image::kSkBitmapRep)
        {
            SkBitmapRep* rep = NULL;
            if(rep)
            {
                AddRepresentation(rep);
                return rep;
            }
            NOTREACHED();
        }

        // ����Skia�����ص�ת��.
        if(default_rep->type() == Image::kSkBitmapRep)
        {
            SkBitmapRep* skia_rep = default_rep->AsSkBitmapRep();
            ImageRep* native_rep = NULL;
            if(native_rep)
            {
                AddRepresentation(native_rep);
                return native_rep;
            }
            NOTREACHED();
        }

        // ���������صĴ���...
        return NULL;
    }

    void Image::AddRepresentation(ImageRep* rep)
    {
        representations_.insert(std::make_pair(rep->type(), rep));
    }

} //namespace gfx