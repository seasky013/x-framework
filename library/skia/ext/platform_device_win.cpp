
#include "platform_device_win.h"

#include "skia_utils_win.h"
#include "SkMatrix.h"
#include "SkPath.h"
#include "SkRegion.h"
#include "SkUtils.h"

namespace skia
{

    PlatformDevice::PlatformDevice(const SkBitmap& bitmap)
        : SkDevice(NULL, bitmap, /*isForLayer=*/false) {}

    // static
    void PlatformDevice::InitializeDC(HDC context)
    {
        // ��������仯.
        // ���������GM_ADVANCEDͼ��ģʽ, GDI���߼��ռ���������ʱ�ӷ�����ƻ�. ��
        // �ȼ�����GM_ADVANCEDͼ��ģʽ��, ���Ŀ��Ƶ�ͻ�������ȫ����DC�����絽�豸
        // �任.
        BOOL res = SetGraphicsMode(context, GM_ADVANCED);
        SkASSERT(res != 0);

        // ���ö���.
        res = SetStretchBltMode(context, HALFTONE);
        SkASSERT(res != 0);
        // ����SetStretchBltMode()�ĵ�, ���������SetBrushOrgEx().
        res = SetBrushOrgEx(context, 0, 0, NULL);
        SkASSERT(res != 0);

        // ����ȱʡ�ķ���.
        res = SetArcDirection(context, AD_CLOCKWISE);
        SkASSERT(res != 0);

        // ����ȱʡ����ɫ.
        res = SetBkColor(context, RGB(255, 255, 255));
        SkASSERT(res != CLR_INVALID);
        res = SetTextColor(context, RGB(0, 0, 0));
        SkASSERT(res != CLR_INVALID);
        res = SetDCBrushColor(context, RGB(255, 255, 255));
        SkASSERT(res != CLR_INVALID);
        res = SetDCPenColor(context, RGB(0, 0, 0));
        SkASSERT(res != CLR_INVALID);

        // ����ȱʡ��͸����.
        res = SetBkMode(context, OPAQUE);
        SkASSERT(res != 0);
        res = SetROP2(context, R2_COPYPEN);
        SkASSERT(res != 0);
    }

    // static
    void PlatformDevice::LoadPathToDC(HDC context, const SkPath& path)
    {
        switch(path.getFillType())
        {
        case SkPath::kWinding_FillType:
            {
                int res = SetPolyFillMode(context, WINDING);
                SkASSERT(res != 0);
                break;
            }
        case SkPath::kEvenOdd_FillType:
            {
                int res = SetPolyFillMode(context, ALTERNATE);
                SkASSERT(res != 0);
                break;
            }
        default:
            {
                SkASSERT(false);
                break;
            }
        }
        BOOL res = BeginPath(context);
        SkASSERT(res != 0);

        CubicPaths paths;
        if(!SkPathToCubicPaths(&paths, path))
        {
            return;
        }

        std::vector<POINT> points;
        for(CubicPaths::const_iterator path(paths.begin()); path!=paths.end();
            ++path)
        {
            if(!path->size())
            {
                continue;
            }
            points.resize(0);
            points.reserve(path->size() * 3 / 4 + 1);
            points.push_back(SkPointToPOINT(path->front().p[0]));
            for(CubicPath::const_iterator point(path->begin()); point!=path->end();
                ++point)
            {
                // �Ӳ����point->p[0].
                points.push_back(SkPointToPOINT(point->p[1]));
                points.push_back(SkPointToPOINT(point->p[2]));
                points.push_back(SkPointToPOINT(point->p[3]));
            }
            SkASSERT((points.size()-1)%3 == 0);
            // ���Ч����һ��, ��Ϊһ�κͶ������߶�"����"Ϊ��������.
            // TODO: http://b/1147346 Ӧ�þ�����ʹ��PolyDraw/PolyBezier/Polyline.
            res = PolyBezier(context, &points.front(),
                static_cast<DWORD>(points.size()));
            SkASSERT(res != 0);
            if(res == 0)
            {
                break;
            }
        }
        if(res == 0)
        {
            // ȷ��·��������.
            AbortPath(context);
        }
        else
        {
            res = EndPath(context);
            SkASSERT(res != 0);
        }
    }

    // static
    void PlatformDevice::LoadTransformToDC(HDC dc, const SkMatrix& matrix)
    {
        XFORM xf;
        xf.eM11 = matrix[SkMatrix::kMScaleX];
        xf.eM21 = matrix[SkMatrix::kMSkewX];
        xf.eDx = matrix[SkMatrix::kMTransX];
        xf.eM12 = matrix[SkMatrix::kMSkewY];
        xf.eM22 = matrix[SkMatrix::kMScaleY];
        xf.eDy = matrix[SkMatrix::kMTransY];
        SetWorldTransform(dc, &xf);
    }

    // static
    bool PlatformDevice::SkPathToCubicPaths(CubicPaths* paths,
        const SkPath& skpath)
    {
        paths->clear();
        CubicPath* current_path = NULL;
        SkPoint current_points[4];
        CubicPoints points_to_add;
        SkPath::Iter iter(skpath, false);
        for(SkPath::Verb verb=iter.next(current_points);
            verb!=SkPath::kDone_Verb;
            verb=iter.next(current_points))
        {
            switch(verb)
            {
            case SkPath::kMove_Verb: // iter.next����1����.
                {
                    // �����������Ϊ���ᱻ��������һ��������. �μ�SkPath::Iter::next().
                    paths->push_back(CubicPath());
                    current_path = &paths->back();
                    // ����������.
                    continue;
                }
            case SkPath::kLine_Verb: // iter.next����2����.
                {
                    points_to_add.p[0] = current_points[0];
                    points_to_add.p[1] = current_points[0];
                    points_to_add.p[2] = current_points[1];
                    points_to_add.p[3] = current_points[1];
                    break;
                }
            case SkPath::kQuad_Verb: // iter.next����3����.
                {
                    points_to_add.p[0] = current_points[0];
                    points_to_add.p[1] = current_points[1];
                    points_to_add.p[2] = current_points[2];
                    points_to_add.p[3] = current_points[2];
                    break;
                }
            case SkPath::kCubic_Verb: // iter.next����4����.
                {
                    points_to_add.p[0] = current_points[0];
                    points_to_add.p[1] = current_points[1];
                    points_to_add.p[2] = current_points[2];
                    points_to_add.p[3] = current_points[3];
                    break;
                }
            case SkPath::kClose_Verb: // iter.next����1����(���һ��).
                {
                    paths->push_back(CubicPath());
                    current_path = &paths->back();
                    continue;
                }
            case SkPath::kDone_Verb: // iter.next����0����.
            default:
                {
                    current_path = NULL;
                    // ������false.
                    break;
                }
            }
            SkASSERT(current_path);
            if(!current_path)
            {
                paths->clear();
                return false;
            }
            current_path->push_back(points_to_add);
        }
        return true;
    }

    // static
    void PlatformDevice::LoadClippingRegionToDC(HDC context,
        const SkRegion& region,
        const SkMatrix& transformation)
    {
        HRGN hrgn;
        if(region.isEmpty())
        {
            // �������Ϊ��, ��ʱ���ж������ᱻ�ü���.
            hrgn = CreateRectRgn(0, 0, 0, 0);
        }
        else if(region.isRect())
        {
            // ��Ӧ�ñ任, ��Ϊ�任�Ѿ�Ӧ�õ�����.
            hrgn = CreateRectRgnIndirect(&SkIRectToRECT(region.getBounds()));
        }
        else
        {
            // �������.
            SkPath path;
            region.getBoundaryPath(&path);
            // �ü�. ע��windows�ü������ܱ任Ӱ��, ���������Լ�����.
            // ��Ϊ�任�ǻ��ڻ�����ԭ��ƽ�Ƶ�, ����Ӧ������ƽ��.
            SkMatrix t(transformation);
            t.setTranslateX(-t.getTranslateX());
            t.setTranslateY(-t.getTranslateY());
            path.transform(t);
            LoadPathToDC(context, path);
            hrgn = PathToRegion(context);
        }
        int result = SelectClipRgn(context, hrgn);
        SkASSERT(result != ERROR);
        result = DeleteObject(hrgn);
        SkASSERT(result != 0);
    }

} //namespace skia