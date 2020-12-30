// AmandShp.cpp: implementation of the CAmandaClipart class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <math.h>
#include "KShapeSmart.h"
#include "mathmatics.h"
#include "..\KDragAction.h"

#include "../../Platform/KColor.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif 
 
#define CP_STAT         0x0001
#define CP_LINE         0x0002
#define CP_CURV         0x0004
#define CP_ENDS         0x0008
#define CP_ENDG         0x0010
#define CP_CLOSED       0x2000

#define MAX_ADJUSTVALUE		21600
#define dPI		3.14159265358979323846


#define ConvertDouble2Int(d) ((int)((d < 0) ? (d + 0.5) : (d - 0.5)))

#define CALLOUT_X_MARGIN	10 //1800
#define CALLOUT_Y_MARGIN	10 //1800

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


#ifdef _MSC_VER
#pragma warning( push ) 
 #pragma warning( disable : 4244 )
#endif // _MSC_VER

#define	ELLIPSE_BEZIER_VAR		1.81066

static int MakeRoundCorner(CPOINT* cp, int x1, int y1, int x2, int y2, int width, int height, BOOL bNorthWest, BOOL addLast)
{
	cp[0].x = x1 * width / 10000;
	cp[0].y = y1 * height / 10000;
	cp[0].a = CP_LINE;
	if(bNorthWest)
		{
		cp[1].x = (int) ((x1 + (double) ((x2 - x1) / ELLIPSE_BEZIER_VAR)) * width / 10000 + 0.5);
		cp[1].y = (double)y1 * height / 10000;
		cp[1].a = CP_CURV;
		cp[2].x = (double)x2 * width / 10000;
		cp[2].y = (int) ((y2 - (double) ((y2 - y1) / ELLIPSE_BEZIER_VAR)) * height / 10000 + 0.5);
		cp[2].a = CP_CURV;
		}
	else
		{
		cp[1].x = (double)x1 * width / 10000;
		cp[1].y = (int) ((y1 + (double) ((y2 - y1) / ELLIPSE_BEZIER_VAR)) * height / 10000 + 0.5);
		cp[1].a = CP_CURV;
		cp[2].x = (int) ((x2 + (double) ((x1 - x2) / ELLIPSE_BEZIER_VAR)) * width / 10000 + 0.5);
		cp[2].y = (double)y2 * height / 10000;
		cp[2].a = CP_CURV;
		}
	if(addLast)
		{
		cp[3].x = (double)x2 * width / 10000;
		cp[3].y = (double)y2 * height / 10000;
		cp[3].a = CP_LINE;
		return(4);
		}
	return(3);
}


extern int GetPieFragment(long cx, long cy, long sw, int a1, int a2, int aincr, LP_CPOINT pn, BOOL add_stt);

static int GetArcPolyline(LP_CPOINT pt, int cx, int cy, int hw, short start, int end)
{
	int		p = 0;
	int		e_segment, s_segment;

	if (abs(end - start) <= 900)
	{
		p += GetPieFragment(cx, cy, hw, start, end, 30, &(pt[p]), TRUE);
	}
	else
	{
		if (start < end)	// counter-clockwize
		{
			s_segment = (int)((double)(start + 899) / 900);
			s_segment = s_segment * 900;
			e_segment = (int)((double)end / 900);
			e_segment = e_segment * 900;
		}
		else
		{
			s_segment = (int)((double)start / 900);
			s_segment = s_segment * 900;
			e_segment = (int)((double)(end + 899) / 900);
			e_segment = e_segment * 900;
		}

		if (start != s_segment)
		{
			// qDebug("pieseg : %d - %d", start, s_segment);
			p += GetPieFragment(cx, cy, hw, start, s_segment, 30, &(pt[p]), TRUE);
		}

		int nw = (start < end) ? hw : -hw;
		BOOL dir = FALSE;	// (start < end) ? FALSE : TRUE;
		while (((start < end) && (e_segment > s_segment)) || ((start > end) && (e_segment < s_segment)))
		{
			if (p > 0)
				p--;

			if ((s_segment % 3600) == 0)
				MakeRoundCorner(&(pt[p]), cx + hw, cy, cx, cy - nw, 10000, 10000, dir, TRUE);
			else if ((s_segment % 3600) == 900)
				MakeRoundCorner(&(pt[p]), cx, cy - hw, cx - nw, cy, 10000, 10000, !dir, TRUE);
			else if ((s_segment % 3600) == 1800)
				MakeRoundCorner(&(pt[p]), cx - hw, cy, cx, cy + nw, 10000, 10000, dir, TRUE);
			else if ((s_segment % 3600) == 2700)
				MakeRoundCorner(&(pt[p]), cx, cy + hw, cx + nw, cy, 10000, 10000, !dir, TRUE);
			p += 4;

			s_segment += (start < end) ? 900 : -900;
		}

		if (e_segment != end)
		{
			if (p > 0)
				p--;
			// qDebug("pieseg : %d - %d", e_segment, end);
			p += GetPieFragment(cx, cy, hw, e_segment, end, 30, &(pt[p]), TRUE);
		}
	}
	return(p);
}

static void modifyWaveBanner(int count, LP_CPOINT lpData, int a1, int a2, int index)
{
#define	MAX_WAVE_A1		4459
	int		dx;

	if (a2 > 10800)
		dx = 21600 - (a2 - 10800) * 2;
	else
		dx = 21600 - (10800 - a2) * 2;

	if (a1 > MAX_WAVE_A1)
		a1 = MAX_WAVE_A1;

	for (int i = 0; i < count; i++)
	{
		if (index == 0)
			lpData[i].y = (int)((double)lpData[i].y * a1 / MAX_WAVE_A1 + 0.5);
		else
			lpData[i].y = 10000 - (int)((double)(10000 - lpData[i].y) * a1 / MAX_WAVE_A1 + 0.5);

		if (a2 != 10800)
		{
			if ((a2 > 10800) == (index == 0))
				lpData[i].x = (int)((double)lpData[i].x * dx / 21600 + 0.5);
			else
				lpData[i].x = 10000 - (int)((double)(10000 - lpData[i].x) * dx / 21600 + 0.5);
		}
	}
}


#define QUADRATIC_POINT_NUM	8

//#define QUADRATIC_VAR (ELLIPSE_BEZIER_VAR * 1.414)
#define QUADRATIC_VAR 3

int Make8CurveSegment(LP_CPOINT lpData, POINT& p1, POINT& p2, POINT& pc, POINT& ps)
{
	int a1 = GetLinePointAngle(&p1, &p2);
	int a2 = GetLinePointAngle(&p1, &pc);
	int a3 = GetLinePointAngle(&ps, &p2);

	lpData[0].x = p1.x;
	lpData[0].y = p1.y;
	lpData[0].a = CP_STAT | CP_LINE;
	if ((a1 == a2) && (a1 == a3))
	{
		lpData[1].x = p2.x;
		lpData[1].y = p2.y;
		lpData[1].a = CP_LINE | CP_ENDS | CP_ENDG;
		return 2;
	}
	else
	{
		POINT po;
		int len, a, p;

		p = 1;
		if (a1 != a2)
		{
			a = a2 - (a1 - a2);
			len = GetDistance(pc.x - p1.x, pc.y - p1.y);
			GetPointFromAngleDist(a, len / QUADRATIC_VAR, &po);
			lpData[p].x = p1.x + po.x;
			lpData[p].y = p1.y + po.y;
			lpData[p++].a = CP_CURV;
			GetPointFromAngleDist(a1, len / QUADRATIC_VAR, &po);
			lpData[p].x = pc.x - po.x;
			lpData[p].y = pc.y - po.y;
			lpData[p++].a = CP_CURV;
		}
		lpData[p].x = pc.x;
		lpData[p].y = pc.y;
		lpData[p++].a = CP_LINE;
		if (a2 != a3)
		{
			len = GetDistance(pc.x - ps.x, pc.y - ps.y);
			GetPointFromAngleDist(a1, len / QUADRATIC_VAR, &po);
			lpData[p].x = pc.x + po.x;
			lpData[p].y = pc.y + po.y;
			lpData[p++].a = CP_CURV;
			lpData[p].x = ps.x - po.x;
			lpData[p].y = ps.y - po.y;
			lpData[p++].a = CP_CURV;
		}
		lpData[p].x = ps.x;
		lpData[p].y = ps.y;
		lpData[p++].a = CP_LINE;

		if (a1 != a3)
		{
			len = GetDistance(p2.x - ps.x, p2.y - ps.y);
			GetPointFromAngleDist(a1, len / QUADRATIC_VAR, &po);
			lpData[p].x = ps.x + po.x;
			lpData[p].y = ps.y + po.y;
			lpData[p++].a = CP_CURV;

			a = (a3 + 1800) - (a1 - a3);
			GetPointFromAngleDist(a, len / QUADRATIC_VAR, &po);
			lpData[p].x = p2.x + po.x;
			lpData[p].y = p2.y + po.y;
			lpData[p++].a = CP_CURV;
		}
		lpData[p].x = p2.x;
		lpData[p].y = p2.y;
		lpData[p++].a = CP_LINE | CP_ENDS | CP_ENDG;
		return p;
	}
}

int MakeQuadraticCurveSegment(LP_CPOINT lpData, POINT& p1, POINT& p2, POINT& pc)
{
	int a1 = GetLinePointAngle(&p1, &p2);
	int a2 = GetLinePointAngle(&p1, &pc);

	lpData[0].x = p1.x;
	lpData[0].y = p1.y;
	lpData[0].a = CP_STAT | CP_LINE;
	if (a1 == a2)
	{
		lpData[1].x = p2.x;
		lpData[1].y = p2.y;
		lpData[1].a = CP_LINE | CP_ENDS | CP_ENDG;
		return 2;
	}
	else
	{
		POINT po;
		int len;
		int a = a2 - (a1 - a2);
		int a3 = GetLinePointAngle(&p2, &pc);

		len = GetDistance(pc.x - p1.x, pc.y - p1.y);
		GetPointFromAngleDist(a, len / QUADRATIC_VAR, &po);
		lpData[1].x = p1.x + po.x;
		lpData[1].y = p1.y + po.y;
		lpData[1].a = CP_CURV;
		GetPointFromAngleDist(a1, len / QUADRATIC_VAR, &po);
		lpData[2].x = pc.x - po.x;
		lpData[2].y = pc.y - po.y;
		lpData[2].a = CP_CURV;
		lpData[3].x = pc.x;
		lpData[3].y = pc.y;
		lpData[3].a = CP_LINE;

		len = GetDistance(pc.x - p2.x, pc.y - p2.y);
		GetPointFromAngleDist(a1, len / QUADRATIC_VAR, &po);
		lpData[4].x = pc.x + po.x;
		lpData[4].y = pc.y + po.y;
		lpData[4].a = CP_CURV;
		a = a3 - (a1 + 1800 - a3);
		GetPointFromAngleDist(a, len / QUADRATIC_VAR, &po);
		lpData[5].x = p2.x + po.x;
		lpData[5].y = p2.y + po.y;
		lpData[5].a = CP_CURV;

		lpData[6].x = p2.x;
		lpData[6].y = p2.y;
		lpData[6].a = CP_LINE | CP_ENDS | CP_ENDG;
		return 7;
	}
}

int MakeCurveSegment(LP_CPOINT lpData, POINT& p1, POINT& p2, int len)
{
	POINT po;
	int angle = GetLinePointAngle(&p1, &p2);
	int a1, a2;

	if (len > 0)
	{
		a1 = angle - 450;
		a2 = angle - 1350;
	}
	else if (len < 0)
	{
		a1 = angle + 450;
		a2 = angle + 1350;
		len = -len;
	}

	lpData[0].x = p1.x;
	lpData[0].y = p1.y;
	lpData[0].a = CP_STAT | CP_LINE;
	if (len == 0)
	{
		lpData[1].x = p2.x;
		lpData[1].y = p2.y;
		lpData[1].a = CP_LINE | CP_ENDS | CP_ENDG;
		return 2;
	}
	else
	{
		GetPointFromAngleDist(a1, len, &po);
		lpData[1].x = p1.x + po.x;
		lpData[1].y = p1.y + po.y;
		lpData[1].a = CP_CURV;
		GetPointFromAngleDist(a2, len, &po);
		lpData[2].x = p2.x + po.x;
		lpData[2].y = p2.y + po.y;
		lpData[2].a = CP_CURV;
		lpData[3].x = p2.x;
		lpData[3].y = p2.y;
		lpData[3].a = CP_LINE | CP_ENDS | CP_ENDG;
		return 4;
		}
}


#ifdef _unused

KShapeSmart::KShapeSmart()
{
	m_nActiveShape = 0;
	m_nNumberOf = 0;
}

KShapeSmart::~KShapeSmart()
{
}

int KShapeSmart::GetCategoryCount()
{
	return(5);
}

KString KShapeSmart::GetCategoryName(int idx)
{
	KString		szName;

	switch(idx)
		{
		case 0:
			szName.Load(THIS_MODULE_RES_HANDLE, IDS_BASIC_SHAPE);
			break;
		case 1:
			szName.Load(THIS_MODULE_RES_HANDLE, IDS_BLOCK_ARROWS);
			break;
		case 2:
			szName.Load(THIS_MODULE_RES_HANDLE, IDS_FLOW_CHART);
			break;
		case 3:
			szName.Load(THIS_MODULE_RES_HANDLE, IDS_STARS_AND_BANNERS);
			break;
		case 4:
			szName.Load(THIS_MODULE_RES_HANDLE, IDS_CALLOUTS);
			break;
		}
	return(szName);
}

int KShapeSmart::SelectCategory(int idx)
{
	m_nNumberOf = 0;
	if( (idx >= 0) && (idx < 5) )
		{
		m_nActiveShape = idx;
		m_nNumberOf = GetItemCount(m_nActiveShape);
		}
	return(0);
}

int KShapeSmart::GetItemCount(int category)
{
	if(category < 0)
		return m_nNumberOf;
	else if(category == 0)
		return( sizeof(pBasicShapes) / sizeof(WORD));
	else if(category == 1)
		return( sizeof(pBlockArrows) / sizeof(WORD));
	else if(category == 2)
		return( sizeof(pFlowChart) / sizeof(WORD));
	else if(category == 3)
		return( sizeof(pStarsandBanners) / sizeof(WORD));
	else if(category == 4)
		return( sizeof(pCallouts) / sizeof(WORD));
	return(0);
}

KString KShapeSmart::GetItemName(int category, int index)
{
	KString name;
	return name;
}

void ReadyShapeRobusView()
{
	pShapeRobusView = new CAmandaView(FALSE);

	pShapeRobusView->ReadyDocument();

	pShapeObjLink = (LP_OBJLINK) AllocateObjectLink(0);
	if(pShapeObjLink != NULL)
		{
		FILLBRUSH		brush;
		LINEPEN			pen;

		brush.Type = SOLID_FILL;
		brush.Solid.Fcolor.Red = 192;
		brush.Solid.Fcolor.Grn = 192;
		brush.Solid.Fcolor.Blu = 192;
		brush.Solid.Fcolor.Tps = 192;
		brush.Solid.Fcolor.Model = COLOR_RGB;
		brush.Solid.Fcolor.byPanton = 0;
		brush.Solid.Fcolor.Index = 0;
		brush.Solid.ColorIndex = 0;
		brush.Solid.Mode = 0;		// button-shape
		brush.Solid.nAlpha=0;//2004.09.08. 정원석. 반투명.
		pen.Type = SOLID;
		pen.Width = 0;
		pen.SttArrow = LINE_EDGE_NONE;
		pen.EndArrow = LINE_EDGE_NONE;
		pen.CapJoinType = (BUTT_CAP | BEVEL_JOIN);
		pen.Color.Red = 0;
		pen.Color.Grn = 0;
		pen.Color.Blu = 0;
		pen.Color.Tps = 0;
		pen.Color.Model = COLOR_RGB;
		pen.Color.byPanton = 0;
		pen.Color.Index = 0;

		pShapeObjLink->Obj.ObjType = SHAPE_OBJC;
		pShapeObjLink->Obj.Shape.Type = 0;
		pShapeObjLink->Obj.Shape.Category = 0;
		pShapeObjLink->Obj.Shape.ps.x = MM2LOG(1);
		pShapeObjLink->Obj.Shape.ps.y = MM2LOG(1);
		pShapeObjLink->Obj.Shape.pe.x = MM2LOG(99);
		pShapeObjLink->Obj.Shape.pe.y = MM2LOG(99);
		pShapeObjLink->Obj.Shape.length = MM2LOG(98);

		ChangeObjectBrushData(&(pShapeObjLink->Obj), &brush, CHANGE_BRUSH_NOTEXT,TRUE);//2004.09.08.정원석. Alpha값도 복사할지를 세팅한다.
		ChangeObjectPenData(&(pShapeObjLink->Obj), &pen);

		pShapeRobusView->m_pDoc->RecalculateObjectsRectBox( &(pShapeObjLink->Obj) );
		}
}


void KShapeSmart::AdjustIconObjectCoord(LP_OBJECT obj)
{
	switch(mType)
		{
		case KSSTCallout1:					// = 41,
		case KSSTAccentCallout1:				// = 44,
		case KSSTBorderCallout1:				// = 47,
		case KSSTAccentBorderCallout1:		// = 50,
		case KSSTCallout2:					// = 42,
		case KSSTAccentCallout2:				// = 45,
		case KSSTBorderCallout2:				// = 48,
		case KSSTAccentBorderCallout2:		// = 51,
		case KSSTCallout90:					// = 178,
		case KSSTAccentCallout90:				// = 179,
		case KSSTBorderCallout90:				// = 180,
		case KSSTAccentBorderCallout90:		// = 181,
			obj->Shape.Coord.ys = MM2LOG(21);
			obj->Shape.Coord.ye = MM2LOG(79);
			obj->Shape.Coord.xs = MM2LOG(31);
			break;

		case KSSTCallout3:					// = 43,
		case KSSTAccentCallout3:				// = 46,
		case KSSTBorderCallout3:				// = 49,
		case KSSTAccentBorderCallout3:		// = 52,
			obj->Shape.Coord.ys = MM2LOG(21);
			obj->Shape.Coord.ye = MM2LOG(79);
			obj->Shape.Coord.xe = MM2LOG(69);
			break;

		case KSSTWedgeRectCallout:			// = 61,
		case KSSTWedgeRRectCallout:			// = 62,
		case KSSTWedgeEllipseCallout:			// = 63,
		case KSSTCloudCallout:				// = 106,
			obj->Shape.Coord.ye = MM2LOG(79);
			break;
		case KSSTLeftBracket:
		case KSSTRightBracket:
			obj->Shape.Coord.xs = MM2LOG(41);
			obj->Shape.Coord.xe = MM2LOG(59);
			break;
		case KSSTLeftBrace:
		case KSSTRightBrace:
			obj->Shape.Coord.xs = MM2LOG(31);
			obj->Shape.Coord.xe = MM2LOG(69);
			break;
		case KSSTMoon:
			obj->Shape.Coord.xs = MM2LOG(21);
			obj->Shape.Coord.xe = MM2LOG(79);
			break;

		default:
			break;
		}
	pShapeRobusView->m_pDoc->RecalculateObjectsRectBox( obj );
}


WORD KShapeSmart::GetShapeID(int indx, int category)
{
	WORD id = 0;
	int maxItem = 0;

	if(category < 0)
		{
		category = m_nActiveShape;
		maxItem = m_nNumberOf;
		}
	else
		{
		maxItem = GetItemCount(category);
		}

	if((maxItem > 0) && (indx < maxItem))
		{
		if(category == 0)
			id = pBasicShapes[indx];
		else if(category == 1)
			id = pBlockArrows[indx];
		else if(category == 2)
			id = pFlowChart[indx];
		else if(category == 3)
			id = pStarsandBanners[indx];
		else if(category == 4)
			id = pCallouts[indx];
		}
	return(id);
}

int KShapeSmart::GetValidAdjustValue(LP_OBJECT obj, LONG* pAdjustValue)
{
	LONG defAdjustValue[8];
	int ncount = 0;

	memset(pAdjustValue, 0, sizeof(LONG) * 8);
	memset(defAdjustValue, 0, sizeof(LONG) * 8);
	ncount = GetDefaultAdjustValue(mType, defAdjustValue);

	// 기본값과 동일한 것은 큰 값으로 하여 저장이 안되도록..
	for (int i = 0; i < ncount; i++)
	{
		pAdjustValue[i] = mAdjustValue[i];
		if (pAdjustValue[i] == defAdjustValue[i])
			pAdjustValue[i] = 0x7FFFFFFF;
	}
	return ncount;
}

void KShapeSmart::CheckDefaultCoord(ShapeType index, LP_OBJECT obj, BOOL bDefaultSize)
{
	if (obj->ObjType != SHAPE_OBJC)
		return;
}

BOOL KShapeSmart::IsDefaultColor(ShapeType index)
{
	return(FALSE);
}

BOOL KShapeSmart::IsFixedColor(ShapeType index)
{
	return(FALSE);
}

BOOL KShapeSmart::GetDefaultFillBrush(ShapeType index, int nChild, LP_FILLBRUSH brush, BOOL bInit)
{

	return(FALSE);
}

BOOL KShapeSmart::GetDefaultLinePen(ShapeType index, int nChild, LP_LINEPEN pen, BOOL bInit)
{

	return(FALSE);
}

BOOL KShapeSmart::IsAngleLengthBase(int shapeType)
{
	return FALSE;
}

// 화면갱신영역 늘림. object 속성에는 영향을 주지 않음
void KShapeSmart::AddHandleArea(LP_OBJECT obj, LP_RECTANGLE rt)
{
#ifdef _DEBUG
	if ((obj == NULL) || (obj->ObjType != SHAPE_OBJC))
	{
		HncMessageBox(NULL, L"None shape object has no handle");
		return;
	}
#endif

	if (obj->Base.Reshape != 0) {
		LONG lTempValue = 0;
		if (obj->Base.Reshape->Rotate.flip & FLOP_HORIZON) {
			if (KShapeSmart::GetCalloutType(mType) == -1) {
				lTempValue = Rect.left;
				Rect.left = Rect.right;
				Rect.right = lTempValue;
			}
			obj->Base.Reshape->Rotate.flip ^= FLOP_HORIZON;
		}
		if (obj->Base.Reshape->Rotate.flip & FLOP_VERTICAL) {
			if (KShapeSmart::GetCalloutType(mType) == -1) {
				lTempValue = Rect.top;
				Rect.top = Rect.bottom;
				Rect.bottom = lTempValue;
			}
			obj->Base.Reshape->Rotate.flip ^= FLOP_VERTICAL;
		}
		if (!IsValidRotateSlant(&(obj->Base.Reshape->Rotate)) && obj->Base.Reshape->lpExtrude == NULL) {
			HCMemFree(obj->Base.Reshape);
			obj->Base.Reshape = NULL;
		}
	}

	int width = Rect.right - Rect.left;
	int height = Rect.bottom - Rect.top;

	int nCalloutType = GetCalloutType(mType);
	if (nCalloutType != -1) {
		POINT ptAdjust;
		ptAdjust.x = (int)((double)mAdjustValue[0] * abs(width) / 21600 + 0.5) + Rect.left;
		ptAdjust.y = (int)((double)mAdjustValue[1] * abs(height) / 21600 + 0.5) + Rect.top;
		if (ptAdjust.x < rt->xs || ptAdjust.x > rt->xe) {
			if (mAdjustValue[0] > 0) {
				rt->xe += abs(ptAdjust.x - rt->xe);
			}
			else {
				rt->xs -= abs(rt->xs - ptAdjust.x);
			}
		}
		if (ptAdjust.y < rt->ys || ptAdjust.y > rt->ye) {
			if (mAdjustValue[1] > 0) {
				rt->ye += abs(ptAdjust.y - rt->ye);
			}
			else {
				rt->ys -= abs(rt->ys - ptAdjust.y);
			}
		}
		if (nCalloutType == 4) {
			ptAdjust.x = (int)((double)mAdjustValue[2] * abs(width) / 21600 + 0.5) + Rect.left;
			if (ptAdjust.x < rt->xs || ptAdjust.x > rt->xe) {
				if (mAdjustValue[2] > 0) {
					rt->xe += abs(ptAdjust.x - rt->xe);
				}
				else {
					rt->xs -= abs(rt->xs - ptAdjust.x);
				}
			}
		}
		return;
	}

	rt->xs -= SHAPE_HANDLE_OUT_MARGIN;
	rt->xe += SHAPE_HANDLE_OUT_MARGIN;
	rt->ys -= SHAPE_HANDLE_OUT_MARGIN;
	rt->ye += SHAPE_HANDLE_OUT_MARGIN;

	return;
}

BOOL SetShapeStartpoint(LP_OBJECT obj, BOOL bSet, LPPOINT pnts, BOOL bMoveAll)
{
	if ((Rect.left != pnts->x) && (Rect.top != pnts->y))
	{
		if (bSet)
		{
			POINT	diff;
			if (bMoveAll)
			{
				diff.x = pnts->x - Rect.left;
				diff.y = pnts->y - Rect.top;
			}
			obj->Shape.ps = *pnts;
			if (bMoveAll)
			{
				Rect.right += diff.x;
				Rect.bottom += diff.y;
			}
		}
		return TRUE;
	}
	return FALSE;
}

BOOL SetShapeEndpoint(LP_OBJECT obj, BOOL bSet, LPPOINT pnts)
{
	if ((Rect.right != pnts->x) && (Rect.bottom != pnts->y))
	{
		if (bSet)
			obj->Shape.pe = *pnts;
		return TRUE;
	}
	return FALSE;
}

#define CHANGE_LENGTH	0x01
#define CHANGE_ANGLE	0x02

BOOL SetShapeAngleLength(LP_OBJECT obj, BOOL bSet, LPPOINT pnts, DWORD mask)
{
	int length = GetDistance(Rect.left - pnts->x, Rect.top - pnts->y);
	int angle = GetLinePointAngle(&obj->Shape.ps, pnts);

	if (((mask & CHANGE_LENGTH) && (obj->Shape.length != length)) ||
		((mask & CHANGE_ANGLE) && (obj->Shape.angle != angle)))
	{
		if (bSet)
		{
			if (mask & CHANGE_LENGTH)
				obj->Shape.length = length;
			if (mask & CHANGE_ANGLE)
				obj->Shape.angle = angle;
		}
		return TRUE;
	}
	return FALSE;
}

BOOL SetAdjustCurveRadius(BOOL bSet, short& a1, short& a2, int a, int len, POINT& p1)
{
	int	na1, na2;

	RotatePOINTAbs(1, &p1, -a);

	na1 = p1.x * 10000 / len;
	na2 = p1.y * 10000 / len;

	//TRACE("change adjust0 %d -> %d", a1, na1);
	//TRACE("change adjust1 %d -> %d", a2, na2);

	if (na1 < 0)
		na1 = 0;
	if (na1 > 10000)
		na1 = 10000;
	if (na2 < -5000)
		na2 = -5000;
	if (na2 > 5000)
		na2 = 5000;
	if ((na1 != a1) ||
		(na2 != a2))
	{
		if (bSet)
		{
			a1 = na1;
			a2 = na2;
		}
		return TRUE;
	}
	return FALSE;
}
#endif

static void ClipMinMaxValue(int& adjust, int min, int max)
{
	if (adjust < min)
		adjust = min;
	if (adjust > max)
		adjust = max;
}

static int LogicalAdjust(int dx, int log, int base)
{
	ClipMinMaxValue(dx, 0, base);
	dx = (int)((double)dx * log / base + 0.5);
	return dx;
}

static int GetAngleFromOrigin(int x, int y)
{
	int     angle;

	if (x > 0)
		angle = (int)RADIAN_TO_ANGLE(atan((double)y / x));
	else if (x == 0)
	{
		if (y == 0)
			return(-1);
		angle = ((y > 0) ? 900 : 2700);
	}
	else
		angle = 1800 + (int)RADIAN_TO_ANGLE(atan((double)y / x));

	if (angle < 0)
		angle += 3600;
	return(angle);
}


static WORD pBasicShapes[] = // 32 
{
	(WORD)ShapeType::Rectangle,
	(WORD)ShapeType::Parallelogram,
	(WORD)ShapeType::Trapezoid,
	(WORD)ShapeType::Diamond,
	(WORD)ShapeType::RoundRectangle,
	(WORD)ShapeType::Arc,
	(WORD)ShapeType::IsocelesTriangle,
	(WORD)ShapeType::RightTriangle,
	(WORD)ShapeType::Ellipse,
	(WORD)ShapeType::Hexagon,
	(WORD)ShapeType::Plus,
	(WORD)ShapeType::Pentagon,

	(WORD)ShapeType::Can,
	(WORD)ShapeType::Cube,
	(WORD)ShapeType::Bevel,
	(WORD)ShapeType::FoldedCorner,//Chevron,
	(WORD)ShapeType::SmileyFace,//Plaque,
	(WORD)ShapeType::Donut,
	(WORD)ShapeType::NoSmoking,
	(WORD)ShapeType::Arc,
	(WORD)ShapeType::Heart,
	(WORD)ShapeType::LightningBolt,
	(WORD)ShapeType::Sun,
	(WORD)ShapeType::Moon,

	(WORD)ShapeType::Arc,
	(WORD)ShapeType::BracketPair,
	(WORD)ShapeType::BracePair,
	(WORD)ShapeType::Octagon,
	(WORD)ShapeType::LeftBrace,
	(WORD)ShapeType::RightBrace,
	(WORD)ShapeType::LeftBracket,
	(WORD)ShapeType::RightBracket,
};

static WORD pBlockArrows[] = // 27
{
	(WORD)ShapeType::Arrow,
	(WORD)ShapeType::LeftArrow,
	(WORD)ShapeType::UpArrow,
	(WORD)ShapeType::DownArrow,
	(WORD)ShapeType::LeftRightArrow,
	(WORD)ShapeType::UpDownArrow,
	(WORD)ShapeType::QuadArrow,
	(WORD)ShapeType::LeftRightUpArrow,
	(WORD)ShapeType::BentArrow,
	(WORD)ShapeType::UturnArrow,
	(WORD)ShapeType::LeftUpArrow,
	(WORD)ShapeType::BentUpArrow,
	(WORD)ShapeType::CurvedRightArrow,
	(WORD)ShapeType::CurvedLeftArrow,
	(WORD)ShapeType::CurvedUpArrow,
	(WORD)ShapeType::CurvedDownArrow,
	(WORD)ShapeType::StripedRightArrow,
	(WORD)ShapeType::NotchedRightArrow,
	(WORD)ShapeType::HomePlate,
	(WORD)ShapeType::Chevron,
	(WORD)ShapeType::RightArrowCallout,
	(WORD)ShapeType::DownArrowCallout,
	(WORD)ShapeType::LeftArrowCallout,
	(WORD)ShapeType::UpArrowCallout,
	(WORD)ShapeType::LeftRightArrowCallout,
	// (WORD)ShapeType::UpDownArrowCallout,
	(WORD)ShapeType::QuadArrowCallout,
	(WORD)ShapeType::CircularArrow,
};

static WORD pFlowChart[] = //28
{
	(WORD)ShapeType::FlowChartProcess,
	(WORD)ShapeType::FlowChartAlternateProcess,
	(WORD)ShapeType::FlowChartDecision,
	(WORD)ShapeType::FlowChartInputOutput,
	(WORD)ShapeType::FlowChartPredefinedProcess,
	(WORD)ShapeType::FlowChartInternalStorage,
	(WORD)ShapeType::FlowChartDocument,
	(WORD)ShapeType::FlowChartMultidocument,
	(WORD)ShapeType::FlowChartTerminator,
	(WORD)ShapeType::FlowChartPreparation,
	(WORD)ShapeType::FlowChartManualInput,
	(WORD)ShapeType::FlowChartManualOperation,
	(WORD)ShapeType::FlowChartConnector,
	(WORD)ShapeType::FlowChartOffpageConnector,
	(WORD)ShapeType::FlowChartPunchedCard,
	(WORD)ShapeType::FlowChartPunchedTape,
	(WORD)ShapeType::FlowChartSummingJunction,
	(WORD)ShapeType::FlowChartOr,
	(WORD)ShapeType::FlowChartCollate,
	(WORD)ShapeType::FlowChartSort,
	(WORD)ShapeType::FlowChartExtract,
	(WORD)ShapeType::FlowChartMerge,
	(WORD)ShapeType::FlowChartOnlineStorage,
	(WORD)ShapeType::FlowChartDelay,
	(WORD)ShapeType::FlowChartMagneticTape,
	(WORD)ShapeType::FlowChartMagneticDisk,
	(WORD)ShapeType::FlowChartMagneticDrum,
	(WORD)ShapeType::FlowChartDisplay,
	(WORD)ShapeType::FlowChartAlternateProcess,
	// (WORD) ShapeType::FlowChartOfflineStorage,
};

static WORD pStarsandBanners[] = // 16
{
	(WORD)ShapeType::IrregularSeal1,
	(WORD)ShapeType::IrregularSeal2,
	(WORD)ShapeType::Seal4,
	(WORD)ShapeType::Star,
	(WORD)ShapeType::Seal8,
	(WORD)ShapeType::Seal16,
	(WORD)ShapeType::Seal24,
	(WORD)ShapeType::Seal32,
	(WORD)ShapeType::Ribbon2,
	(WORD)ShapeType::Ribbon,
	(WORD)ShapeType::EllipseRibbon2,
	(WORD)ShapeType::EllipseRibbon,
	(WORD)ShapeType::VerticalScroll,
	(WORD)ShapeType::HorizontalScroll,
	(WORD)ShapeType::Wave,
	(WORD)ShapeType::DoubleWave,
};

static WORD pCallouts[] = // 20 -> 27
{
	(WORD)ShapeType::WedgeRectCallout,
	(WORD)ShapeType::WedgeRRectCallout,
	(WORD)ShapeType::WedgeEllipseCallout,
	(WORD)ShapeType::CloudCallout,
	(WORD)ShapeType::Callout1,
	(WORD)ShapeType::Callout2,
	(WORD)ShapeType::Callout3,
	(WORD)ShapeType::AccentCallout1,
	(WORD)ShapeType::AccentCallout2,
	(WORD)ShapeType::AccentCallout3,
	(WORD)ShapeType::BorderCallout1,
	(WORD)ShapeType::BorderCallout2,
	(WORD)ShapeType::BorderCallout3,
	(WORD)ShapeType::AccentBorderCallout1,
	(WORD)ShapeType::AccentBorderCallout2,
	(WORD)ShapeType::AccentBorderCallout3,
	(WORD)ShapeType::Callout90,
	(WORD)ShapeType::AccentCallout90,
	(WORD)ShapeType::BorderCallout90,
	(WORD)ShapeType::AccentBorderCallout90,
	(WORD)ShapeType::LeftArrowCallout,
	(WORD)ShapeType::RightArrowCallout,
	(WORD)ShapeType::UpArrowCallout,
	(WORD)ShapeType::DownArrowCallout,
	(WORD)ShapeType::LeftRightArrowCallout,
	(WORD)ShapeType::UpDownArrowCallout,
	(WORD)ShapeType::QuadArrowCallout,
};

int KShapeSmartBaseObject::GetItemCount(int category)
{
	if (category == 0)
		return(sizeof(pBasicShapes) / sizeof(WORD));
	else if (category == 1)
		return(sizeof(pBlockArrows) / sizeof(WORD));
	else if (category == 2)
		return(sizeof(pFlowChart) / sizeof(WORD));
	else if (category == 3)
		return(sizeof(pStarsandBanners) / sizeof(WORD));
	else if (category == 4)
		return(sizeof(pCallouts) / sizeof(WORD));
	return(0);
}

ShapeType KShapeSmartBaseObject::GetShapeID(int indx, int category)
{
	ShapeType id = ShapeType::ShapeNone;
	int maxItem = 0;

	maxItem = GetItemCount(category);

	if ((maxItem > 0) && (indx < maxItem))
	{
		if (category == 0)
			id = (ShapeType)pBasicShapes[indx];
		else if (category == 1)
			id = (ShapeType)pBlockArrows[indx];
		else if (category == 2)
			id = (ShapeType)pFlowChart[indx];
		else if (category == 3)
			id = (ShapeType)pStarsandBanners[indx];
		else if (category == 4)
			id = (ShapeType)pCallouts[indx];
	}
	return(id);
}

BOOL KShapeSmartBaseObject::IsValidShapeID(ShapeType index)
{
	switch (index)
	{
	case ShapeType::RoundRectangle:
	case ShapeType::Diamond:
	case ShapeType::IsocelesTriangle:
	case ShapeType::RightTriangle:
	case ShapeType::Parallelogram:
	case ShapeType::Trapezoid:
	case ShapeType::Hexagon:
	case ShapeType::Octagon:
	case ShapeType::Plus:
	case ShapeType::Arrow:
	case ShapeType::HomePlate:
	case ShapeType::Cube:
	case ShapeType::Arc:
	case ShapeType::Plaque:
	case ShapeType::Can:
	case ShapeType::Donut:
	case ShapeType::Chevron:
	case ShapeType::Pentagon:
	case ShapeType::NoSmoking:
	case ShapeType::FoldedCorner:
	case ShapeType::LeftArrow:
	case ShapeType::DownArrow:
	case ShapeType::UpArrow:
	case ShapeType::LeftRightArrow:
	case ShapeType::UpDownArrow:
	case ShapeType::LightningBolt:
	case ShapeType::Heart:
	case ShapeType::QuadArrow:
	case ShapeType::LeftArrowCallout:
	case ShapeType::RightArrowCallout:
	case ShapeType::UpArrowCallout:
	case ShapeType::DownArrowCallout:
	case ShapeType::LeftRightArrowCallout:
	case ShapeType::UpDownArrowCallout:
	case ShapeType::QuadArrowCallout:
	case ShapeType::Bevel:
	case ShapeType::LeftBracket:
	case ShapeType::RightBracket:
	case ShapeType::LeftBrace:
	case ShapeType::RightBrace:
	case ShapeType::LeftUpArrow:
	case ShapeType::BentUpArrow:
	case ShapeType::BentArrow:
	case ShapeType::StripedRightArrow:
	case ShapeType::NotchedRightArrow:
	case ShapeType::BlockArc:
	case ShapeType::SmileyFace:
	case ShapeType::CircularArrow:
	case ShapeType::UturnArrow:
	case ShapeType::CurvedRightArrow:
	case ShapeType::CurvedLeftArrow:
	case ShapeType::CurvedUpArrow:
	case ShapeType::CurvedDownArrow:
	case ShapeType::Sun:
	case ShapeType::Moon:
	case ShapeType::BracketPair:
	case ShapeType::BracePair:
	case ShapeType::StraightConnector1:
	case ShapeType::Star:
	case ShapeType::TextOctagon:
	case ShapeType::FlowChartProcess:
	case ShapeType::Callout1:
	case ShapeType::Callout2:
	case ShapeType::Callout3:
	case ShapeType::AccentCallout1:
	case ShapeType::AccentCallout2:
	case ShapeType::AccentCallout3:
	case ShapeType::BorderCallout1:
	case ShapeType::BorderCallout2:
	case ShapeType::BorderCallout3:
	case ShapeType::AccentBorderCallout1:
	case ShapeType::AccentBorderCallout2:
	case ShapeType::AccentBorderCallout3:
	case ShapeType::Ribbon:
	case ShapeType::Ribbon2:
	case ShapeType::Seal8:
	case ShapeType::Seal16:
	case ShapeType::Seal32:
	case ShapeType::WedgeRectCallout:
	case ShapeType::WedgeRRectCallout:
	case ShapeType::WedgeEllipseCallout:
	case ShapeType::Wave:
	case ShapeType::IrregularSeal1:
	case ShapeType::IrregularSeal2:
	case ShapeType::Seal24:
	case ShapeType::VerticalScroll:
	case ShapeType::HorizontalScroll:
	case ShapeType::CloudCallout:
	case ShapeType::EllipseRibbon:
	case ShapeType::EllipseRibbon2:
	case ShapeType::FlowChartDecision:
	case ShapeType::FlowChartInputOutput:
	case ShapeType::FlowChartPredefinedProcess:
	case ShapeType::FlowChartInternalStorage:
	case ShapeType::FlowChartDocument:
	case ShapeType::FlowChartMultidocument:
	case ShapeType::FlowChartTerminator:
	case ShapeType::FlowChartPreparation:
	case ShapeType::FlowChartManualInput:
	case ShapeType::FlowChartManualOperation:
	case ShapeType::FlowChartConnector:
	case ShapeType::FlowChartPunchedCard:
	case ShapeType::FlowChartPunchedTape:
	case ShapeType::FlowChartSummingJunction:
	case ShapeType::FlowChartOr:
	case ShapeType::FlowChartCollate:
	case ShapeType::FlowChartSort:
	case ShapeType::FlowChartExtract:
	case ShapeType::FlowChartMerge:
	case ShapeType::FlowChartOnlineStorage:
	case ShapeType::FlowChartMagneticDisk:
	case ShapeType::FlowChartMagneticDrum:
	case ShapeType::FlowChartDisplay:
	case ShapeType::FlowChartDelay:
	case ShapeType::FlowChartAlternateProcess:
	case ShapeType::FlowChartOffpageConnector:
	case ShapeType::Callout90:
	case ShapeType::AccentCallout90:
	case ShapeType::BorderCallout90:
	case ShapeType::AccentBorderCallout90:
	case ShapeType::LeftRightUpArrow:
	case ShapeType::Seal4:
	case ShapeType::DoubleWave:
	case ShapeType::FlowChartMagneticTape:
		return TRUE;

	default:
		return FALSE;
	}
}

int KShapeSmartBaseObject::GetDefaultAdjustValue(ShapeType objType, LONG* pAdjustValue)
{
	if (pAdjustValue != NULL)
		memset(pAdjustValue, 0, sizeof(LONG) * 8);

	switch(objType)
		{
		case ShapeType::RoundRectangle:
		case ShapeType::FlowChartAlternateProcess:
			if (pAdjustValue != NULL)
				pAdjustValue[0] = 4000;
			return 1;
		case ShapeType::Parallelogram:				// = 7,
		case ShapeType::Trapezoid:					// = 8,
		case ShapeType::Hexagon:						// = 9,
		case ShapeType::Plus:						// = 11,
			if (pAdjustValue != NULL)
				pAdjustValue[0] = 5000;
			return 1;
		case ShapeType::Octagon:						// = 10,
			if (pAdjustValue != NULL)
				pAdjustValue[0] = 6000;
			return 1;
		case ShapeType::IsocelesTriangle:			// = 5,
		case ShapeType::FlowChartExtract:			// = 127,
		case ShapeType::FlowChartMerge:				// = 128,
			if (pAdjustValue != NULL)
				pAdjustValue[0] = 10800;
			return 1;
		case ShapeType::Cube:						// = 16,
			if (pAdjustValue != NULL)
				pAdjustValue[0] = 5500;
			return 1;
		case ShapeType::Bevel:						// = 84,
			if (pAdjustValue != NULL)
				pAdjustValue[0] = 2700;
			return 1;
		case ShapeType::FlowChartInputOutput:		// = 111,
		case ShapeType::FlowChartPreparation:		// = 117,
		case ShapeType::FlowChartManualInput:		// = 118,
		case ShapeType::FlowChartManualOperation:	// = 119,
		case ShapeType::FlowChartPunchedCard:		// = 121,
			if (pAdjustValue != NULL)
				pAdjustValue[0] = 4320;
			return 1;
		case ShapeType::FlowChartPredefinedProcess:	// = 112,
			if (pAdjustValue != NULL)
				pAdjustValue[0] = 2590;
			return 1;
		case ShapeType::FlowChartInternalStorage:	// = 113,
			if (pAdjustValue != NULL)
				pAdjustValue[0] = 4270;
			return 1;
		case ShapeType::FlowChartOffpageConnector:	// = 177,
			if (pAdjustValue != NULL)
				pAdjustValue[0] = 17280;
			return 1;
		case ShapeType::Arrow:						// = 13,
		case ShapeType::DownArrow:					// = 67,
			if (pAdjustValue != NULL)
			{
				pAdjustValue[0] = 16600;
				pAdjustValue[1] = 5500;
			}
			return 2;
		case ShapeType::LeftArrow:					// = 66,
		case ShapeType::UpArrow:						// = 68,
			if (pAdjustValue != NULL)
			{
				pAdjustValue[0] = 5000;
				pAdjustValue[1] = 5500;
			}
			return 2;

		case ShapeType::LeftRightArrow:				// = 69,
		case ShapeType::UpDownArrow:					// = 70,
			if (pAdjustValue != NULL)
			{
				pAdjustValue[0] = 5000;
				pAdjustValue[1] = 5000;
			}
			return 2;

		case ShapeType::QuadArrow:					// = 76,
			if (pAdjustValue != NULL)
			{
				pAdjustValue[0] = 6500;
				pAdjustValue[1] = 8500;
				pAdjustValue[2] = 4300;
			}
			return 3;
		case ShapeType::LeftRightUpArrow:			// = 182,
			if (pAdjustValue != NULL)
			{
				pAdjustValue[0] = 6500;
				pAdjustValue[1] = 8500;
				pAdjustValue[2] = 6000;
			}
			return 3;
		case ShapeType::LeftUpArrow:					// = 89,
		case ShapeType::BentUpArrow:					// = 90,
			if (pAdjustValue != NULL)
			{
				pAdjustValue[0] = 9300;
				pAdjustValue[1] = 18000;
				pAdjustValue[2] = 6000;
			}
			return 3;
		case ShapeType::NotchedRightArrow:			// = 94,
			if (pAdjustValue != NULL)
			{
				pAdjustValue[0] = 16000;
				pAdjustValue[1] = 5000;
			}
			return 2;
		case ShapeType::HomePlate:					// = 15,
		case ShapeType::Chevron:						// = 55,
			if (pAdjustValue != NULL)
				pAdjustValue[0] = 16000;
			return 1;
		case ShapeType::RightArrowCallout:			// = 78,
		case ShapeType::DownArrowCallout:			// = 80,
			if (pAdjustValue != NULL)
			{
				pAdjustValue[0] = 14000;
				pAdjustValue[1] = 5000;
				pAdjustValue[2] = 18000;
				pAdjustValue[3] = 8000;
			}
			return 4;
		case ShapeType::LeftArrowCallout:			// = 77,
		case ShapeType::UpArrowCallout:				// = 79,
		case ShapeType::LeftRightArrowCallout:		// = 81,
		case ShapeType::UpDownArrowCallout:			// = 82,
			if (pAdjustValue != NULL)
			{
				pAdjustValue[0] = 5400;//7600;
				pAdjustValue[1] = 5000;
				pAdjustValue[2] = 3600;
				pAdjustValue[3] = 8000;
			}
			return 4;
		case ShapeType::QuadArrowCallout:			// = 83,
			if (pAdjustValue != NULL)
			{
				pAdjustValue[0] = 5000;
				pAdjustValue[1] = 8000;
				pAdjustValue[2] = 2500;
				pAdjustValue[3] = 9000;
			}
			return 4;
		case ShapeType::Seal4:						// = 187,
			if (pAdjustValue != NULL)
				pAdjustValue[0] = 8000;
			return 1;
		case ShapeType::Seal8:						// = 58,
		case ShapeType::Seal16:						// = 59,
		case ShapeType::Seal24:						// = 92,
		case ShapeType::Seal32:						// = 60,
			if (pAdjustValue != NULL)
				pAdjustValue[0] = 2600;
			return 1;
		case ShapeType::Can:							// = 22,
			if (pAdjustValue != NULL)
				pAdjustValue[0] = 5400;
			return 1;
		case ShapeType::FoldedCorner:				// = 65,
			if (pAdjustValue != NULL)
				pAdjustValue[0] = 18900;
			return 1;
		case ShapeType::SmileyFace:					// = 96,
			if (pAdjustValue != NULL)
				pAdjustValue[0] = 17600;
			return 1;
		case ShapeType::Donut:						// = 23,
		case ShapeType::Sun:							// = 183,
			if (pAdjustValue != NULL)
				pAdjustValue[0] = 5400;
			return 1;
		case ShapeType::BlockArc:					// = 95,
			if (pAdjustValue != NULL)
			{
				pAdjustValue[0] = 180 << 16;
				pAdjustValue[1] = 5400;
			}
			return 2;
		case ShapeType::Arc:
			if (pAdjustValue != NULL)
			{
				pAdjustValue[0] = -(90 << 16);
				pAdjustValue[1] = 0;
			}
			return 2;
		case ShapeType::Moon:						// = 184,
			if (pAdjustValue != NULL)
				pAdjustValue[0] = 10800;
			return 1;
		case ShapeType::BracketPair:					// = 185,
		case ShapeType::BracePair:					// = 186,
		case ShapeType::Plaque:						// = 21,
			if (pAdjustValue != NULL)
				pAdjustValue[0] = 3600;
			return 1;
		case ShapeType::LeftBracket:					// = 85,
		case ShapeType::RightBracket:				// = 86,
			if (pAdjustValue != NULL)
				pAdjustValue[0] = 1900;
			return 1;
		case ShapeType::LeftBrace:					// = 87,
		case ShapeType::RightBrace:					// = 88,
			if (pAdjustValue != NULL)
			{
				pAdjustValue[0] = 1900;				// 0 - 5400
				pAdjustValue[1] = 10800;			// 0 - 21600
			}
			return 2;
		case ShapeType::BentArrow:					// = 91,
			if (pAdjustValue != NULL)
			{
				pAdjustValue[0] = 15078;				// 12427 - 21600
				pAdjustValue[1] = 2900;				// 0 - 6079
			}
			return 2;
		case ShapeType::CurvedLeftArrow:				// = 103,
		case ShapeType::CurvedUpArrow:				// = 104,
			if (pAdjustValue != NULL)
			{
				pAdjustValue[0] = 13000;
				pAdjustValue[1] = 19400;
				pAdjustValue[2] = 7200;
			}
			return 3;

		case ShapeType::CurvedRightArrow:			// = 102,
		case ShapeType::CurvedDownArrow:				// = 105,
			if (pAdjustValue != NULL)
			{
				pAdjustValue[0] = 13000;
				pAdjustValue[1] = 19400;
				pAdjustValue[2] = 14400;
			}
			return 3;
		case ShapeType::StripedRightArrow:			// = 93,
			if (pAdjustValue != NULL)
			{
				pAdjustValue[0] = 16200;				// 12427 - 21600
				pAdjustValue[1] = 5400;				// 0 - 6079
			}
			return 2;
		case ShapeType::Ribbon2:						// = 54,
			if (pAdjustValue != NULL)
			{
				pAdjustValue[0] = 5400;
				pAdjustValue[1] = 19000;
			}
			return 2;
		case ShapeType::Ribbon:						// = 53,
			if (pAdjustValue != NULL)
			{
				pAdjustValue[0] = 5400;
				pAdjustValue[1] = 21600 - 19000;
			}
			return 2;
		case ShapeType::EllipseRibbon2:				// = 108,
			if (pAdjustValue != NULL)
			{
				pAdjustValue[0] = 5400;
				pAdjustValue[1] = 16200;
				pAdjustValue[2] = 2700;
			}
			return 3;
		case ShapeType::EllipseRibbon:				// = 107,
			if (pAdjustValue != NULL)
			{
				pAdjustValue[0] = 5400;
				pAdjustValue[1] = 21600 - 16200;
				pAdjustValue[2] = 21600 - 2700;
			}
			return 3;
		case ShapeType::VerticalScroll:				// = 97,
		case ShapeType::HorizontalScroll:			// = 98,
			if (pAdjustValue != NULL)
				pAdjustValue[0] = 2700;
			return 1;
		case ShapeType::Wave:						// = 64,
		case ShapeType::DoubleWave:					// = 188,
			if (pAdjustValue != NULL)
			{
				pAdjustValue[0] = 2800;
				pAdjustValue[1] = 10800;
				pAdjustValue[2] = 0;
			}
			return 3;
		case ShapeType::Callout1:					// = 41,
		case ShapeType::AccentCallout1:				// = 44,
		case ShapeType::BorderCallout1:				// = 47,
		case ShapeType::AccentBorderCallout1:		// = 50,
			if (pAdjustValue != NULL)
			{
				pAdjustValue[0] = -8288;
				pAdjustValue[1] = 24200;
				pAdjustValue[2] = -3600;
			}
			return 3;

		case ShapeType::Callout2:					// = 42,
		case ShapeType::AccentCallout2:				// = 45,
		case ShapeType::BorderCallout2:				// = 48,
		case ShapeType::AccentBorderCallout2:		// = 51,
			if (pAdjustValue != NULL)
			{
				pAdjustValue[0] = -10088;
				pAdjustValue[1] = 24200;
				pAdjustValue[2] = -3600;
			}
			return 3;
		case ShapeType::Callout3:					// = 43,
		case ShapeType::AccentCallout3:				// = 46,
		case ShapeType::BorderCallout3:				// = 49,
		case ShapeType::AccentBorderCallout3:		// = 52,
			if (pAdjustValue != NULL)
			{
				pAdjustValue[0] = 0;
				pAdjustValue[1] = 24200;
				pAdjustValue[2] = -7200;
				pAdjustValue[3] = 21800;
				pAdjustValue[4] = 25000;
			}
			return 5;
		case ShapeType::WedgeRectCallout:			// = 61,
		case ShapeType::WedgeRRectCallout:			// = 62,
		case ShapeType::WedgeEllipseCallout:			// = 63,
		case ShapeType::CloudCallout:				// = 106,
			if (pAdjustValue != NULL)
			{
				pAdjustValue[0] = 700;
				pAdjustValue[1] = 27000;
				pAdjustValue[2] = 14400;
			}
			return 3;
		case ShapeType::Callout90:					// = 178,
		case ShapeType::AccentCallout90:				// = 179,
		case ShapeType::BorderCallout90:				// = 180,
		case ShapeType::AccentBorderCallout90:		// = 181,
			if (pAdjustValue != NULL)
			{
				pAdjustValue[0] = -3600;
				pAdjustValue[1] = 25000;
				pAdjustValue[2] = 19000;
				pAdjustValue[3] = 25000;
			}
			return 4;
		case ShapeType::NoSmoking:
			if (pAdjustValue != NULL)
				pAdjustValue[0] = 2700;
			return 1;
		case ShapeType::CircularArrow:				// 원형 화살표
			if (pAdjustValue != NULL)
			{
				pAdjustValue[0] = 180 << 16;
				pAdjustValue[1] = 0;
				pAdjustValue[2] = 5000;
			}
			return 3;
		case ShapeType::Diamond:						// = 4,
		case ShapeType::RightTriangle:				// = 6,
		case ShapeType::Pentagon:					// = 56,
		case ShapeType::FlowChartDecision:			// = 110,
		case ShapeType::FlowChartCollate:			// = 125,
		case ShapeType::FlowChartSort:				// = 126,
		case ShapeType::Star:						// = 12,
		case ShapeType::IrregularSeal1:				// = 71,
		case ShapeType::IrregularSeal2:				// = 72,		
		case ShapeType::Heart:						// = 74,
		case ShapeType::LightningBolt:				// = 73,
		case ShapeType::UturnArrow:					// = 101,		
		case ShapeType::FlowChartDocument:			// = 114,
		case ShapeType::FlowChartMultidocument:		// = 115,
		case ShapeType::FlowChartTerminator:			// = 116,
		case ShapeType::FlowChartPunchedTape:		// = 122,
		case ShapeType::FlowChartSummingJunction:	// = 123,
		case ShapeType::FlowChartOr:					// = 124,
		case ShapeType::FlowChartOfflineStorage:		// = 129,
		case ShapeType::FlowChartOnlineStorage:		// = 130,
		case ShapeType::FlowChartMagneticTape:		// = 131,
		case ShapeType::FlowChartMagneticDisk:		// = 132,
		case ShapeType::FlowChartMagneticDrum:		// = 133,
		case ShapeType::FlowChartDisplay:			// = 134,
		case ShapeType::FlowChartDelay:				// = 135,
		default:
			return 0;
		}
}

// 0: 1행(사각모양4개), 1: 2행(조절점2개의 수직/수평 가이드), 2: 3행(조절점 2개의 꺾인 가이드), 
// 3: 4행(조절점 3개의 한번꺾인 가이드), 4: 5행(조절점 4개의 두번꺾인 가이드)
int KShapeSmartBaseObject::GetCalloutType(ShapeType nDefNum)
{
	int nCalloutType = -1;
	ShapeType nCalloutArr[5][4] = { {ShapeType::WedgeRectCallout, ShapeType::WedgeRRectCallout, ShapeType::WedgeEllipseCallout, ShapeType::CloudCallout},
		{ShapeType::BorderCallout90, ShapeType::Callout90, ShapeType::AccentCallout90, ShapeType::AccentBorderCallout90},
		{ShapeType::BorderCallout1, ShapeType::Callout1, ShapeType::AccentCallout1, ShapeType::AccentBorderCallout1},
		{ShapeType::BorderCallout2, ShapeType::Callout2, ShapeType::AccentCallout2, ShapeType::AccentBorderCallout2},
		{ShapeType::BorderCallout3, ShapeType::Callout3, ShapeType::AccentCallout3, ShapeType::AccentBorderCallout3} };
	int nMIndex = 0, nEIndex = 0;

	for (nMIndex = 0; nMIndex < 5; nMIndex++) {
		for (nEIndex = 0; nEIndex < 4; nEIndex++) {
			if (nDefNum == nCalloutArr[nMIndex][nEIndex]) {
				nCalloutType = nMIndex;
				break;
			}
		}
	}

	return nCalloutType;
}

KShapeSmartBaseObject* KShapeSmartBaseObject::CreateShapeObject(ShapeType objType)
{
	KShapeSmartBaseObject* object = NULL;

	switch (objType)
	{
	case ShapeType::Rectangle:
	case ShapeType::RoundRectangle:
	case ShapeType::Ellipse:
	case ShapeType::Trapezoid:					// = 8,
	case ShapeType::Parallelogram:				// = 7,
	case ShapeType::Diamond:						// = 4,
	case ShapeType::Octagon:						// = 10,
	case ShapeType::IsocelesTriangle:			// = 5,
	case ShapeType::RightTriangle:				// = 6,
	case ShapeType::Hexagon:						// = 9,
	case ShapeType::Plus:						// = 11,
	case ShapeType::Pentagon:					// = 56,		
	case ShapeType::FlowChartProcess:			// = 109,
	case ShapeType::FlowChartDecision:			// = 110,
	case ShapeType::FlowChartInputOutput:		// = 111,
	case ShapeType::FlowChartPredefinedProcess:	// = 112,
	case ShapeType::FlowChartInternalStorage:	// = 113,
	case ShapeType::FlowChartAlternateProcess:	// = 176,
	case ShapeType::FlowChartPreparation:		// = 117,
	case ShapeType::FlowChartManualInput:		// = 118,
	case ShapeType::FlowChartManualOperation:	// = 119,
	case ShapeType::FlowChartOffpageConnector:	// = 177,
	case ShapeType::FlowChartConnector:		// = 120,
	case ShapeType::FlowChartPunchedCard:		// = 121,
	case ShapeType::FlowChartCollate:			// = 125,
	case ShapeType::FlowChartSort:				// = 126,
	case ShapeType::FlowChartExtract:			// = 127,
	case ShapeType::FlowChartMerge:				// = 128,
	case ShapeType::Arrow:						// = 13,
	case ShapeType::LeftArrow:					// = 66,
	case ShapeType::UpArrow:						// = 68,
	case ShapeType::DownArrow:					// = 67,
	case ShapeType::LeftRightArrow:				// = 69,
	case ShapeType::UpDownArrow:					// = 70,
	case ShapeType::QuadArrow:					// = 76,
	case ShapeType::LeftRightUpArrow:			// = 182,
	case ShapeType::LeftUpArrow:					// = 89,
	case ShapeType::BentUpArrow:					// = 90,
	case ShapeType::NotchedRightArrow:			// = 94,
	case ShapeType::HomePlate:					// = 15,
	case ShapeType::Chevron:						// = 55,
	case ShapeType::RightArrowCallout:			// = 78,
	case ShapeType::LeftArrowCallout:			// = 77,
	case ShapeType::UpArrowCallout:				// = 79,
	case ShapeType::DownArrowCallout:			// = 80,
	case ShapeType::LeftRightArrowCallout:		// = 81,
	case ShapeType::UpDownArrowCallout:			// = 82,
	case ShapeType::QuadArrowCallout:			// = 83,
	case ShapeType::Seal4:						// = 187,
	case ShapeType::Star:						// = 12,
	case ShapeType::Seal8:						// = 58,
	case ShapeType::Seal16:						// = 59,
	case ShapeType::Seal24:						// = 92,
	case ShapeType::Seal32:						// = 60,
	case ShapeType::IrregularSeal1:				// = 71,
	case ShapeType::IrregularSeal2:				// = 72,
	case ShapeType::CubeShadow:					// = 300, 큐브 그림자
	case ShapeType::CanShadow:						// = 301, 
	case ShapeType::BevelShadow:					// = 302, 
	case ShapeType::CurvedRightArrowShadow:		// = 303, 
	case ShapeType::CurvedLeftArrowShadow:			// = 304, 
	case ShapeType::CurvedUpArrowShadow:			// = 305, 
	case ShapeType::CurvedDownArrowShadow:			// = 306, 
	case ShapeType::FlowChartMultidocumentShadow:	// = 307, 
	case ShapeType::FlowChartMagneticDiskShadow:	// = 308, 
	case ShapeType::FlowChartMagneticDrumShadow:	// = 309, 
	case ShapeType::RibbonShadow:					// = 310, 
	case ShapeType::Ribbon2Shadow:					// = 311, 
	case ShapeType::VerticalScrollShadow:			// = 312, 
	case ShapeType::HorizontalScrollShadow:		// = 313, 
	case ShapeType::EllipseRibbonShadow:			// = 314, 
	case ShapeType::EllipseRibbon2Shadow:			// = 315, 
	case ShapeType::SmileyFaceShadow:				// = 316,:
	case ShapeType::FoldedCornerShadow:			// = 317,
	case ShapeType::StripedRightArrowShadow:		// = 318
		object = new KShapeSmartRectObject(objType);
		break;
	case ShapeType::Cube:
	case ShapeType::Bevel:
	case ShapeType::Arc:
	case ShapeType::Can:							// = 22,
	case ShapeType::FoldedCorner:				// = 65,
	case ShapeType::SmileyFace:					// = 96,
	case ShapeType::Donut:						// = 23,
	case ShapeType::NoSmoking:					// = 57,
	case ShapeType::BlockArc:					// = 95,
	case ShapeType::Heart:						// = 74,
	case ShapeType::LightningBolt:				// = 73,
	case ShapeType::Sun:							// = 183,
	case ShapeType::Moon:						// = 184,
	case ShapeType::BracketPair:					// = 185,
	case ShapeType::BracePair:					// = 186,
	case ShapeType::Plaque:						// = 21,
	case ShapeType::LeftBracket:					// = 85,
	case ShapeType::RightBracket:				// = 86,
	case ShapeType::LeftBrace:					// = 87,
	case ShapeType::RightBrace:					// = 88,
	case ShapeType::BentArrow:					// = 91,
	case ShapeType::UturnArrow:					// = 101,
	case ShapeType::CurvedRightArrow:			// = 102,
	case ShapeType::CurvedLeftArrow:				// = 103,
	case ShapeType::CurvedUpArrow:				// = 104,
	case ShapeType::CurvedDownArrow:				// = 105,
	case ShapeType::StripedRightArrow:			// = 93,
	case ShapeType::CircularArrow:				// = 99,
	case ShapeType::Ribbon2:						// = 54,
	case ShapeType::Ribbon:						// = 53,
	case ShapeType::EllipseRibbon2:				// = 108,
	case ShapeType::EllipseRibbon:				// = 107,
	case ShapeType::VerticalScroll:				// = 97,
	case ShapeType::HorizontalScroll:			// = 98,
	case ShapeType::Wave:						// = 64,
	case ShapeType::DoubleWave:					// = 188,
	case ShapeType::FlowChartDocument:			// = 114,
	case ShapeType::FlowChartMultidocument:		// = 115,
	case ShapeType::FlowChartTerminator:			// = 116,
	case ShapeType::FlowChartPunchedTape:		// = 122,
	case ShapeType::FlowChartSummingJunction:	// = 123,
	case ShapeType::FlowChartOr:					// = 124,
	case ShapeType::FlowChartOfflineStorage:		// = 129,
	case ShapeType::FlowChartOnlineStorage:		// = 130,
	case ShapeType::FlowChartMagneticTape:		// = 131,
	case ShapeType::FlowChartMagneticDisk:		// = 132,
	case ShapeType::FlowChartMagneticDrum:		// = 133,
	case ShapeType::FlowChartDisplay:			// = 134,
	case ShapeType::FlowChartDelay:				// = 135,
		object = new KShapeSmartGroupObject(objType);
		break;
	case ShapeType::Callout1:					// = 41,
	case ShapeType::Callout2:					// = 42,
	case ShapeType::Callout3:					// = 43,
	case ShapeType::AccentCallout1:				// = 44,
	case ShapeType::AccentCallout2:				// = 45,
	case ShapeType::AccentCallout3:				// = 46,
	case ShapeType::BorderCallout1:				// = 47,
	case ShapeType::BorderCallout2:				// = 48,
	case ShapeType::BorderCallout3:				// = 49,
	case ShapeType::AccentBorderCallout1:		// = 50,
	case ShapeType::AccentBorderCallout2:		// = 51,
	case ShapeType::AccentBorderCallout3:		// = 52,
	case ShapeType::WedgeRectCallout:			// = 61,
	case ShapeType::WedgeRRectCallout:			// = 62,
	case ShapeType::WedgeEllipseCallout:			// = 63,
	case ShapeType::CloudCallout:				// = 106,
	case ShapeType::Callout90:					// = 178,
	case ShapeType::AccentCallout90:				// = 179,
	case ShapeType::BorderCallout90:				// = 180,
	case ShapeType::AccentBorderCallout90:		// = 181,
		object = new KShapeSmartCalloutObject(objType);
		break;
	default:
		object = NULL;
		TCHAR msg[128];
		StringCchPrintf(msg, 128, _T("Unknown ShapeType =%d"), objType);
		StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG | SYNC_MSG_DEBUG);
		break;
	}

	return(object);
}

KShapeSmartBaseObject* KShapeSmartBaseObject::CreateShapeObject(ShapeType objType, RECT& area, DWORD fillcolor, OutlineStyle linestyle, LONG* adjust)
{
	KShapeSmartBaseObject* object = NULL;

	switch(objType)
		{
		case ShapeType::Rectangle:
		case ShapeType::RoundRectangle:
		case ShapeType::Ellipse:
		case ShapeType::Trapezoid:					// = 8,
		case ShapeType::Parallelogram:				// = 7,
		case ShapeType::Diamond:						// = 4,
		case ShapeType::Octagon:						// = 10,
		case ShapeType::IsocelesTriangle:			// = 5,
		case ShapeType::RightTriangle:				// = 6,
		case ShapeType::Hexagon:						// = 9,
		case ShapeType::Plus:						// = 11,
		case ShapeType::Pentagon:					// = 56,		
		case ShapeType::FlowChartProcess:			// = 109,
		case ShapeType::FlowChartDecision:			// = 110,
		case ShapeType::FlowChartInputOutput:		// = 111,
		case ShapeType::FlowChartPredefinedProcess:	// = 112,
		case ShapeType::FlowChartInternalStorage:	// = 113,
		case ShapeType::FlowChartAlternateProcess:	// = 176,
		case ShapeType::FlowChartPreparation:		// = 117,
		case ShapeType::FlowChartManualInput:		// = 118,
		case ShapeType::FlowChartManualOperation:	// = 119,
		case ShapeType::FlowChartOffpageConnector:	// = 177,
		case ShapeType::FlowChartConnector:		// = 120,
		case ShapeType::FlowChartPunchedCard:		// = 121,
		case ShapeType::FlowChartCollate:			// = 125,
		case ShapeType::FlowChartSort:				// = 126,
		case ShapeType::FlowChartExtract:			// = 127,
		case ShapeType::FlowChartMerge:				// = 128,
		case ShapeType::Arrow:						// = 13,
		case ShapeType::LeftArrow:					// = 66,
		case ShapeType::UpArrow:						// = 68,
		case ShapeType::DownArrow:					// = 67,
		case ShapeType::LeftRightArrow:				// = 69,
		case ShapeType::UpDownArrow:					// = 70,
		case ShapeType::QuadArrow:					// = 76,
		case ShapeType::LeftRightUpArrow:			// = 182,
		case ShapeType::LeftUpArrow:					// = 89,
		case ShapeType::BentUpArrow:					// = 90,
		case ShapeType::NotchedRightArrow:			// = 94,
		case ShapeType::HomePlate:					// = 15,
		case ShapeType::Chevron:						// = 55,
		case ShapeType::RightArrowCallout:			// = 78,
		case ShapeType::LeftArrowCallout:			// = 77,
		case ShapeType::UpArrowCallout:				// = 79,
		case ShapeType::DownArrowCallout:			// = 80,
		case ShapeType::LeftRightArrowCallout:		// = 81,
		case ShapeType::UpDownArrowCallout:			// = 82,
		case ShapeType::QuadArrowCallout:			// = 83,
		case ShapeType::Seal4:						// = 187,
		case ShapeType::Star:						// = 12,
		case ShapeType::Seal8:						// = 58,
		case ShapeType::Seal16:						// = 59,
		case ShapeType::Seal24:						// = 92,
		case ShapeType::Seal32:						// = 60,
		case ShapeType::IrregularSeal1:				// = 71,
		case ShapeType::IrregularSeal2:				// = 72,
		case ShapeType::CubeShadow:					// = 300, 큐브 그림자
		case ShapeType::CanShadow:						// = 301, 
		case ShapeType::BevelShadow:					// = 302, 
		case ShapeType::CurvedRightArrowShadow:		// = 303, 
		case ShapeType::CurvedLeftArrowShadow:			// = 304, 
		case ShapeType::CurvedUpArrowShadow:			// = 305, 
		case ShapeType::CurvedDownArrowShadow:			// = 306, 
		case ShapeType::FlowChartMultidocumentShadow:	// = 307, 
		case ShapeType::FlowChartMagneticDiskShadow:	// = 308, 
		case ShapeType::FlowChartMagneticDrumShadow:	// = 309, 
		case ShapeType::RibbonShadow:					// = 310, 
		case ShapeType::Ribbon2Shadow:					// = 311, 
		case ShapeType::VerticalScrollShadow:			// = 312, 
		case ShapeType::HorizontalScrollShadow:		// = 313, 
		case ShapeType::EllipseRibbonShadow:			// = 314, 
		case ShapeType::EllipseRibbon2Shadow:			// = 315, 
		case ShapeType::SmileyFaceShadow:				// = 316,:
		case ShapeType::FoldedCornerShadow:			// = 317,
		case ShapeType::StripedRightArrowShadow:		// = 318
			object = new KShapeSmartRectObject(objType, area, fillcolor, linestyle, adjust);
			break;
		case ShapeType::Cube:
		case ShapeType::Bevel:
		case ShapeType::Arc:
		case ShapeType::Can:							// = 22,
		case ShapeType::FoldedCorner:				// = 65,
		case ShapeType::SmileyFace:					// = 96,
		case ShapeType::Donut:						// = 23,
		case ShapeType::NoSmoking:					// = 57,
		case ShapeType::BlockArc:					// = 95,
		case ShapeType::Heart:						// = 74,
		case ShapeType::LightningBolt:				// = 73,
		case ShapeType::Sun:							// = 183,
		case ShapeType::Moon:						// = 184,
		case ShapeType::BracketPair:					// = 185,
		case ShapeType::BracePair:					// = 186,
		case ShapeType::Plaque:						// = 21,
		case ShapeType::LeftBracket:					// = 85,
		case ShapeType::RightBracket:				// = 86,
		case ShapeType::LeftBrace:					// = 87,
		case ShapeType::RightBrace:					// = 88,
		case ShapeType::BentArrow:					// = 91,
		case ShapeType::UturnArrow:					// = 101,
		case ShapeType::CurvedRightArrow:			// = 102,
		case ShapeType::CurvedLeftArrow:				// = 103,
		case ShapeType::CurvedUpArrow:				// = 104,
		case ShapeType::CurvedDownArrow:				// = 105,
		case ShapeType::StripedRightArrow:			// = 93,
		case ShapeType::CircularArrow:				// = 99,
		case ShapeType::Ribbon2:						// = 54,
		case ShapeType::Ribbon:						// = 53,
		case ShapeType::EllipseRibbon2:				// = 108,
		case ShapeType::EllipseRibbon:				// = 107,
		case ShapeType::VerticalScroll:				// = 97,
		case ShapeType::HorizontalScroll:			// = 98,
		case ShapeType::Wave:						// = 64,
		case ShapeType::DoubleWave:					// = 188,
		case ShapeType::FlowChartDocument:			// = 114,
		case ShapeType::FlowChartMultidocument:		// = 115,
		case ShapeType::FlowChartTerminator:			// = 116,
		case ShapeType::FlowChartPunchedTape:		// = 122,
		case ShapeType::FlowChartSummingJunction:	// = 123,
		case ShapeType::FlowChartOr:					// = 124,
		case ShapeType::FlowChartOfflineStorage:		// = 129,
		case ShapeType::FlowChartOnlineStorage:		// = 130,
		case ShapeType::FlowChartMagneticTape:		// = 131,
		case ShapeType::FlowChartMagneticDisk:		// = 132,
		case ShapeType::FlowChartMagneticDrum:		// = 133,
		case ShapeType::FlowChartDisplay:			// = 134,
		case ShapeType::FlowChartDelay:				// = 135,
			object = new KShapeSmartGroupObject(objType, area, fillcolor, linestyle, adjust);
			break;
		case ShapeType::Callout1:					// = 41,
		case ShapeType::Callout2:					// = 42,
		case ShapeType::Callout3:					// = 43,
		case ShapeType::AccentCallout1:				// = 44,
		case ShapeType::AccentCallout2:				// = 45,
		case ShapeType::AccentCallout3:				// = 46,
		case ShapeType::BorderCallout1:				// = 47,
		case ShapeType::BorderCallout2:				// = 48,
		case ShapeType::BorderCallout3:				// = 49,
		case ShapeType::AccentBorderCallout1:		// = 50,
		case ShapeType::AccentBorderCallout2:		// = 51,
		case ShapeType::AccentBorderCallout3:		// = 52,
		case ShapeType::WedgeRectCallout:			// = 61,
		case ShapeType::WedgeRRectCallout:			// = 62,
		case ShapeType::WedgeEllipseCallout:			// = 63,
		case ShapeType::CloudCallout:				// = 106,
		case ShapeType::Callout90:					// = 178,
		case ShapeType::AccentCallout90:				// = 179,
		case ShapeType::BorderCallout90:				// = 180,
		case ShapeType::AccentBorderCallout90:		// = 181,
			object = new KShapeSmartCalloutObject(objType, area, fillcolor, linestyle, adjust);
			break;
		default:
			object = NULL;
			break;
		}

	return(object);
}

// 그룹속성안에 dominant(int)라는 변수는 여러 child중 한개만 가리키고 있다.
// 여기서는 dominant는 아니지만 dominant의 채우기 속성을 반영해야 하는 면인지를 리턴해준다. dominant도 포함
BOOL KShapeSmartBaseObject::IsDominantOfGrpObject(ShapeType nObjType, int nIndex)
{
	BOOL bRet = FALSE;

	switch (nObjType) {
		case ShapeType::Cube:
		case ShapeType::Can:
		case ShapeType::FoldedCorner:
		case ShapeType::CurvedLeftArrow:
		case ShapeType::CurvedUpArrow:
		case ShapeType::SmileyFace:
			if(nIndex == 0)
				bRet = TRUE;
			break;
		case ShapeType::Bevel:
			if(nIndex == 4)
				bRet = TRUE;
			break;
		case ShapeType::CurvedRightArrow:
		case ShapeType::CurvedDownArrow:
			if(nIndex == 1)
				bRet = TRUE;
			break;
		case ShapeType::Ribbon2:
		case ShapeType::Ribbon:
			if((nIndex == 0) || (nIndex == 1) || (nIndex == 2))
				bRet = TRUE;
			break;
		case ShapeType::EllipseRibbon2:
		case ShapeType::EllipseRibbon:
			if((nIndex == 0) || (nIndex == 1) || (nIndex == 4))
				bRet = TRUE;
			break;
		case ShapeType::VerticalScroll:
		case ShapeType::HorizontalScroll:
			if((nIndex == 0) || (nIndex == 2) || (nIndex == 4))
				bRet = TRUE;
			break;
	}
	return bRet;
}

BOOL KShapeSmartBaseObject::GetShadowShapeType(ShapeType type, ShapeType& shadowType)
{
	BOOL bRet = TRUE;
	switch (type) {
		case ShapeType::Cube:
			shadowType = ShapeType::CubeShadow;
			break;
		case ShapeType::Can:
			shadowType = ShapeType::CanShadow;
			break;
		case ShapeType::Bevel:
			shadowType = ShapeType::BevelShadow;
			break;
		case ShapeType::CurvedRightArrow:
			shadowType = ShapeType::CurvedRightArrowShadow;
			break;
		case ShapeType::CurvedLeftArrow:
			shadowType = ShapeType::CurvedLeftArrowShadow;
			break;
		case ShapeType::CurvedUpArrow:
			shadowType = ShapeType::CurvedUpArrowShadow;
			break;
		case ShapeType::CurvedDownArrow:
			shadowType = ShapeType::CurvedDownArrowShadow;
			break;
		case ShapeType::FlowChartMultidocument:
			shadowType = ShapeType::FlowChartMultidocumentShadow;
			break;
		case ShapeType::FlowChartMagneticDisk:
			shadowType = ShapeType::FlowChartMagneticDiskShadow;
			break;
		case ShapeType::FlowChartMagneticDrum:
			shadowType = ShapeType::FlowChartMagneticDrumShadow;
			break;
		case ShapeType::Ribbon:
			shadowType = ShapeType::RibbonShadow;
			break;
		case ShapeType::Ribbon2:
			shadowType = ShapeType::Ribbon2Shadow;
			break;
		case ShapeType::VerticalScroll:
			shadowType = ShapeType::VerticalScrollShadow;
			break;
		case ShapeType::HorizontalScroll:
			shadowType = ShapeType::HorizontalScrollShadow;
			break;
		case ShapeType::EllipseRibbon:
			shadowType = ShapeType::EllipseRibbonShadow;
			break;
		case ShapeType::EllipseRibbon2:
			shadowType = ShapeType::EllipseRibbon2Shadow;
			break;
		case ShapeType::SmileyFace:
			shadowType = ShapeType::SmileyFaceShadow;
			break;
		case ShapeType::FoldedCorner:
			shadowType = ShapeType::FoldedCornerShadow;
			break;
		case ShapeType::StripedRightArrow:
			shadowType = ShapeType::StripedRightArrowShadow;
			break;
		default:
			bRet = FALSE;
			break;
	}
	return bRet;
}

BOOL KShapeSmartBaseObject::isExtraFillShape(ShapeType type)
{
	if( (type == ShapeType::Arc) || 
		(type == ShapeType::BracketPair) ||
		(type == ShapeType::BracePair) )
		return TRUE;
	return FALSE;
}

#if 0
LP_OBJECT KShapeSmartBaseObject::MakeGroupPolygon(LP_OBJECT obj, BOOL noLabel, LP_OBJDRAW_INFO info)
{
	if((obj == NULL) || (obj->ObjType != SHAPE_OBJC))
		return(NULL);

	CRect area(Rect.left, Rect.top, Rect.right, Rect.bottom);

	// Shape 개체중에 도넛이나 금지는 Alternate FillMode로 그림자를 그린다.
	if (mType == KSSTDonut || mType == KSSTNoSmoking) {
		if (obj->Base.Reshape != NULL && obj->Base.Reshape->lpExtrude != NULL)
			obj->Base.Reshape->lpExtrude->PersFlag |= SHADOW_ALTERNATEMODE;
	}

	// 그룹으로 나누어질 쉐입의 그림자 그리기
	KShapeSmartBaseObject*	object = NULL;
	BOOL bShadowShape = FALSE;
	ShapeType shapeType = KSSTMin;
	if (info && (info->drawMode & DRAW_SHADOW)) {
		bShadowShape = GetShadowShapeType(mType, shapeType);
		if (obj->Base.Reshape && obj->Base.Reshape->lpExtrude && 
			(obj->Base.Reshape->lpExtrude->Style == EXTRUDESTYLE_NEARHALFREFLECTION ||
			obj->Base.Reshape->lpExtrude->Style == EXTRUDESTYLE_FARHALFREFLECTION||
			obj->Base.Reshape->lpExtrude->Style == EXTRUDESTYLE_NEARALMOSTREFLECTION|| 
			obj->Base.Reshape->lpExtrude->Style == EXTRUDESTYLE_FARALMOSTREFLECTION||
			obj->Base.Reshape->lpExtrude->Style == EXTRUDESTYLE_NEARALLREFLECTION||
			obj->Base.Reshape->lpExtrude->Style == EXTRUDESTYLE_FARALLREFLECTION))
			bShadowShape = FALSE;
	}

	if (bShadowShape) {
		object = CreateShapeObj(shapeType, area, mAdjustValue);
	} else {
		object = CreateShapeObj((ShapeType) mType, area, mAdjustValue);
	}

	LP_OBJECT	grp_obj = NULL;

	if(object != NULL) {
		int		nChildNumber = 0;
		int		ntype;
		CRect	cr;
		int			pnum;
		CPOINT*		ppoint = NULL;
		LP_OBJECT	child;

		if (info && (info->drawMode & DRAW_SHADOW) && bShadowShape) {
			nChildNumber = 1;
		} else {
			nChildNumber = object->GetChildNumber();
		}
		object->m_angle = obj->Shape.angle;
		object->m_length = obj->Shape.length;
		object->pBrush = GetObjectsBrushDataPtr(obj);
		object->pPen = GetObjectsPenDataPtr(obj);
		object->drawFlags = (info != NULL) ? info->drawFlags : 0;

		if(nChildNumber > 1) {
			int		n = 0;
			BOOL	bColorChange;
			BOOL	bSetDefaultColor;
			BOOL	initDominant = FALSE;

			grp_obj = (LP_OBJECT) MakeGroupObject();
			bColorChange = object->IsColorChange();
			bSetDefaultColor = KShapeSmart::IsFixedColor(object->mType);
			if( bColorChange )
				grp_obj->Group.flag = GROUP_COLOR_PROPAGATE;

			int nPenTypeBck = object->pPen->Type; // 펜 스타일 임시저장

			while( (ntype = object->GetChild(n, &cr, &ppoint, &pnum)) > 0) {
				// 테두리 없는 설명선의 경우 스타일을 HOLLOW로 바꾸어주고, 다음 child일때 복원시켜 준다.
				if(object->mType == KSSTAccentCallout90 || object->mType == KSSTAccentCallout1 || 
					object->mType == KSSTAccentCallout2 || object->mType == KSSTAccentCallout3 || object->mType == KSSTCallout90 ||
					object->mType == KSSTCallout1 || object->mType == KSSTCallout2 || object->mType == KSSTCallout3) {
					if(n == 0) {
						object->pPen->Type = HOLLOW;
					} else {
						object->pPen->Type = nPenTypeBck;
					}
				}

				child = MakeSimpleChildObject(object, area, ntype, cr, ppoint, pnum, n);

				if (child != NULL) {
					if (bColorChange ) {
						int colorBrighten = object->GetColorBright(n);

						initDominant = FALSE;
						if (mType == KSSTCurvedRightArrow || mType == KSSTCurvedDownArrow) {
							if (n == 1)
								initDominant = TRUE;
						} else if (mType == KSSTBevel) {
							if (n == 4)
								initDominant = TRUE;
						} else {
							if (n == 0)
								initDominant = TRUE;
						}

						LP_FILLBRUSH lpBrush = GetObjectsBrushDataPtr(child);
						if (lpBrush != NULL) {
							if (initDominant) {
								grp_obj->Group.dominant = n;
								if (lpBrush->Type == PTRN_FILL) {
									if (!(lpBrush->Ptrn.flag & PTRN_FILL_TILE) || lpBrush->Ptrn.flag == 6)
										lpBrush->Solid.Fcolor.Red = lpBrush->Solid.Fcolor.Grn = lpBrush->Solid.Fcolor.Blu = 0;
								}
							} else {
								if (IsDominantOfGrpObject(object->mType, n)) {
									lpBrush->Solid.ColorIndex = 10000;
								} else {
									lpBrush->Solid.ColorIndex = colorBrighten;
								}
							}
						}
					} else if (bSetDefaultColor) {
						KShapeSmart::GetDefaultFillBrush(object->mType, n, GetObjectsBrushDataPtr(child), FALSE);
						KShapeSmart::GetDefaultLinePen(object->mType, n, GetObjectsPenDataPtr(child), FALSE);
					}

					if (obj->Base.Reshape != NULL && obj->Base.Reshape->lpExtrude != NULL) {
						if (GetCalloutType(mType) != -1 && n == 0) // 지시선을 제외한 설명선에 그림자 정보 전달
							ChangeObjectExtrude(child, obj->Base.Reshape->lpExtrude);
					}
					AddGroupChild(grp_obj, (LP_OBJLINK) child);
				}
				else {
					break;
				}
				n++;
			}
		} else if (nChildNumber > 0) {
			ntype = object->GetChild(0, &cr, &ppoint, &pnum, NULL);
			child = MakeSimpleChildObject(object, area, ntype, cr, ppoint, pnum, TRUE);

			if(child)
				grp_obj = child;
		}
		delete object;
	}
	if (grp_obj != NULL) {
		DWORD field = (BASEDATA_ALL & ~BASEDATA_RESHAPE);

		if(noLabel)
			field &= ~BASEDATA_LABEL;
		CopyBaseData(&(grp_obj->Base), &(obj->Base), field);
	}
	return(grp_obj);

return NULL;
}

extern void ConvertRectangle(LPRECT rt, LPRECT rect);

LP_OBJECT KShapeSmart::MakeSimpleChildObject(KShapeSmartBaseObject* object, CRect& rect, int ntype, CRect& cr, CPOINT* ppoint, int pnum, int childIndex)
{
	LP_OBJECT	child;
	RECTANGLE	rt;

	if((ntype == BasicGraphElement::PolygonElement) && (pnum > 1))
		{
		child = (LP_OBJECT) MakePolygonObject(rect, ppoint, pnum, object->IsEvenOddFill() ? EVEN_ODD_FILL : ADDITIVE_FILL);

		// CopyFillBrushData( GetObjectsBrushDataPtr(child), object->pBrush,TRUE);//2004.09.08.정원석. Alpha값도 복사할지를 세팅한다.
		// CopyLinePenData( GetObjectsPenDataPtr(child), object->pPen);
		}
	else if(ntype == BasicGraphElement::EllipseElement)
		{
		rt.xs = cr.left + rect.left;
		rt.xe = cr.right + rect.left;
		rt.ys = cr.top + rect.top;
		rt.ye = cr.bottom + rect.top;
		child = (LP_OBJECT) MakeEllipseObject(&rt);
		// CopyFillBrushData( GetObjectsBrushDataPtr(child), object->pBrush,TRUE);//2004.09.08.정원석. Alpha값도 복사할지를 세팅한다.
		// CopyLinePenData( GetObjectsPenDataPtr(child), object->pPen);
		}
	else if(ntype == OBJ_ELLIPSE_CENTER)
		{
		rt.xs = (cr.left - cr.right) + rect.left;
		rt.xe = (cr.left + cr.right) + rect.left;
		rt.ys = (cr.top - cr.bottom) + rect.top;
		rt.ye = (cr.top + cr.bottom) + rect.top;
		child = (LP_OBJECT) MakeEllipseObject(&rt);
		// CopyFillBrushData( GetObjectsBrushDataPtr(child), object->pBrush,TRUE);//2004.09.08.정원석. Alpha값도 복사할지를 세팅한다.
		// CopyLinePenData( GetObjectsPenDataPtr(child), object->pPen);
		}
	else if((ntype == BasicGraphElement::RectElement) || (ntype == OBJ_ROUNDRECTANGLE))
		{
		rt.xs = cr.left + rect.left;
		rt.xe = cr.right + rect.left;
		rt.ys = cr.top + rect.top;
		rt.ye = cr.bottom + rect.top;
		child = (LP_OBJECT) MakeRectangleObject(&rt);

		if(ntype == OBJ_ROUNDRECTANGLE)
			{
			double round = (double)1000 * object->mAdjustValue[0] / 10800;
			if(abs(rect.right - rect.left) > abs(rect.bottom - rect.top))
				{
				double dx = (double) abs(rect.bottom - rect.top) / abs(rect.right - rect.left);
				child->Rect.RoundW = (int)(round * dx + 0.5);
				child->Rect.RoundH = (int)(round + 0.5);
				}
			else
				{
				double dx = (double) abs(rect.right - rect.left) / abs(rect.bottom - rect.top);
				child->Rect.RoundW = (int)(round + 0.5);
				child->Rect.RoundH = (int)(round * dx + 0.5);
				}
			}
		// CopyFillBrushData( GetObjectsBrushDataPtr(child), object->pBrush,TRUE);//2004.09.08.정원석. Alpha값도 복사할지를 세팅한다.
		}
	else
		child = NULL;

	if(child != NULL)
	{
		if(	object->HasPen(childIndex) )
			CopyLinePenData( GetObjectsPenDataPtr(child), object->pPen);
		else
			child->Rect.Pen.Type = HOLLOW;

		if(	object->HasBrush(childIndex) )
			CopyFillBrushData( GetObjectsBrushDataPtr(child), object->pBrush,TRUE);//2004.09.08.정원석. Alpha값도 복사할지를 세팅한다.
		else
			SetBrushHollow(GetObjectsBrushDataPtr(child));
	}
	return(child);
}

LP_OBJLINK KShapeSmart::MakeGroupObject()
{
	LP_OBJLINK newLink = AllocateObjectLink(0);
	if(newLink != NULL)
		{
		newLink->Obj.ObjType = GROUP_OBJC;
		newLink->Obj.Group.Childs =	NULL;
		newLink->Obj.Group.ChildNumber = 0;
		newLink->Obj.Group.ChildPath.Method	= NO_PATH;
		newLink->Obj.Group.ChildPath.PointNumber = 0;
		newLink->Obj.Group.ChildPath.Points	= NULL;
		newLink->Obj.Group.flag = 0;
		newLink->Obj.Group.dominant = 0;
		}
	return(newLink);
}

LP_OBJLINK KShapeSmart::MakeRectangleObject(LP_RECTANGLE rt)
{
	LP_OBJLINK newLink = AllocateObjectLink(0);
	if (newLink != NULL) {
        newLink->Obj.ObjType = RECT_OBJC;
        // gpGlobal->GetCurrentFillBrush((LP_FILLBRUSH) &(newLink->Obj.Rect.Brush));
        // gpGlobal->GetCurrentLinePen((LP_LINEPEN) &(newLink->Obj.Rect.Pen));

        newLink->Obj.Rect.RoundH = 0;
        newLink->Obj.Rect.RoundW = 0;
        newLink->Obj.Rect.Coord = *rt;
		newLink->Obj.Rect.lpWordArt = NULL; // ws*
    }
    return(newLink);
}

LP_OBJLINK KShapeSmart::MakeEllipseObject(LP_RECTANGLE rt)
{
	LP_OBJLINK newLink = AllocateObjectLink(0);
	if(newLink != NULL)
		{
		newLink->Obj.ObjType = ELLIPSE_OBJC;
		// gpGlobal->GetCurrentFillBrush((LP_FILLBRUSH) &(newLink->Obj.Ellipse.Brush));
		// gpGlobal->GetCurrentLinePen((LP_LINEPEN) &(newLink->Obj.Ellipse.Pen));
		newLink->Obj.Ellipse.Coord = *rt;
		newLink->Obj.Ellipse.SttAngle = 0;
		newLink->Obj.Ellipse.EndAngle = 3600;
		newLink->Obj.Ellipse.APC = 0;
		}
	return(newLink);
}

LP_OBJLINK KShapeSmart::MakePolygonObject(CRect& rect, LP_CPOINT pp, int nPoint, short fillType)
{
	LP_OBJLINK newLink = AllocateObjectLink(0);

	if(newLink != NULL)
		{
		newLink->Obj.ObjType = POLYGON_OBJC;
		// gpGlobal->GetCurrentFillBrush((LP_FILLBRUSH) &(newLink->Obj.Polygon.Brush));
		// gpGlobal->GetCurrentLinePen((LP_LINEPEN) &(newLink->Obj.Polygon.Pen));
		// UpdatePaletteColor(&(newLink->Obj), NULL);

		for(int i=0 ; i<nPoint ; i++)
			{
			pp[i].x += rect.left;
			pp[i].y += rect.top;
			}
		newLink->Obj.Polygon.PointNumber = nPoint;
		newLink->Obj.Polygon.FillMethod = fillType;
		newLink->Obj.Polygon.Points = (LP_CPOINT) pp;
		}
	return( newLink );
}
#endif

/*************
LP_OBJLINK KShapeSmart::MakePolygonObject(CRect& rect, POINT* ppoint, int pcnt, int* pArray, short fillType)
{
	int			i;
	POINT		pt;
	LP_CPOINT	pp = (LP_CPOINT) HCMemAlloc(sizeof(CPOINT) * pcnt);

	for (i = 0; i < pcnt; i++)
		{
		// TRACE("point %d:%d,%d", i, ppoint[i].x, ppoint[i].y);
		pt.x = ppoint[i].x;
		pt.y = ppoint[i].y;
		// ConvertPoint( &pt );

		pp[i].x = pt.x;
		pp[i].y = pt.y;
		pp[i].a = CP_LINE;
		}
	if((pArray != NULL) && (pArray[0] > 1))
		{
		// polypolygon
		i = 0;
		for(int p=0 ; p<pArray[0] ; p++)
			{
			pp[i].a |= CP_STAT;
			pp[i + pArray[p+1] - 1].a |= CP_ENDS | CP_CLOSED;
			i += pArray[p+1];
			}
		pp[pcnt - 1].a |= CP_ENDG;
		}
	else
		{
		pp[0].a |= CP_STAT;
		pp[pcnt-1].a |= CP_ENDS | CP_ENDG | CP_CLOSED;
		}
	return( MakePolygonObject(rect, pp, pcnt, fillType));
}
******************/

/*************************************************
KShapeSmartBaseObject
*************************************************/


const POINT KShapeSmartBaseObject::_rectangle[] = 
{
	{ 0, 0 }, { 10000, 0 }, { 10000, 10000 }, { 0, 10000 }
};
const POINT KShapeSmartBaseObject::_diamond[] = 
{
	{ 5000, 0 }, { 10000, 5000 }, { 5000, 10000 }, { 0, 5000 }
};
const POINT KShapeSmartBaseObject::_octagon[] = 
{
	{ 0, 0 }, { 0, 0 }, { 10000, 0 }, { 10000, 0 }, { 0, 10000 }, { 0, 10000 }, { 0, 0 }, { 0, 0 }
};
const POINT KShapeSmartBaseObject::_triangle[] = 
{
	{ 0, 0 }, { 10000, 10000 }, { 0, 10000 }
};
const POINT KShapeSmartBaseObject::_hexagon[] = 
{
	{ 0, 0 }, { 0, 0 }, { 10000, 5000 }, { 0, 10000 }, { 0, 10000 }, { 0, 5000 }
};
const POINT KShapeSmartBaseObject::_pluspoly[] = 
{
	{ 0, 0 }, { 0, 0 }, { 0, 0 }, { 10000, 0 }, { 10000, 0 }, { 0, 0 }, 
	{ 0, 10000 }, { 0, 10000 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }
};
const POINT KShapeSmartBaseObject::_pentagon[] = 
{
	{ 5000, 0 }, { 10000, 3800 }, { 8100, 10000 }, { 1900, 10000 }, { 0, 3800 }
};
const POINT KShapeSmartBaseObject::_arrow[] = 
{
	{ 0, 0 }, { 0, 0 }, { 0, 0 }, { 10000, 5000 }, { 0, 10000 }, { 0, 0 }, { 0, 0 } 
};
const POINT KShapeSmartBaseObject::_leftarrow[] = 
{
	{ 0, 5000 }, { 0, 0 }, { 0, 0 }, { 10000, 0 }, { 10000, 0 }, { 0, 0 }, { 0, 10000 } 
};
const POINT KShapeSmartBaseObject::_uparrow[] = 
{
	{ 5000, 0 }, { 10000, 0 }, { 0, 0 }, { 0, 10000 }, { 0, 10000 }, { 0, 0 }, { 0, 0 } 
};
const POINT KShapeSmartBaseObject::_downarrow[] = 
{
	{ 5000, 10000 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 10000, 0 } 
};
const POINT KShapeSmartBaseObject::_leftrightarrow[] = 
{
	{ 0, 5000 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 10000, 5000 }, { 0, 10000 }, { 0, 0 }, { 0, 0 }, { 0, 10000 } 
};
const POINT KShapeSmartBaseObject::_updownarrow[] = 
{
	{ 5000, 0 }, { 10000, 0 }, { 0, 0 }, { 0, 0 }, { 10000, 0 }, { 5000, 10000 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } 
};
const POINT KShapeSmartBaseObject::_quadarrow[] = 
{
	{ 5000, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, 
	{ 10000, 5000 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, 
	{ 5000, 10000 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, 
	{ 0, 5000 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }
};
const POINT KShapeSmartBaseObject::_leftrightuparrow[] = 
{
	{ 5000, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, 
	{ 10000, 0 }, { 0, 10000 }, { 0, 0 }, { 0, 0 }, { 0, 10000 }, 
	{ 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }
};
const POINT KShapeSmartBaseObject::_leftuparrow[] = 
{
	{ 0, 0 }, { 10000, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 10000 }, 
	{ 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }
};
const POINT KShapeSmartBaseObject::_bentuparrow[] = 
{
	{ 0, 0 }, { 10000, 0 }, { 0, 0 }, { 0, 10000 }, { 0, 10000 }, 
	{ 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } 
};
const POINT KShapeSmartBaseObject::_notchedrightarrow[] = 
{
	{ 0, 0 }, { 0, 0 }, { 0, 0 }, { 10000, 5000 }, { 0, 10000 }, { 0, 0 }, { 0, 0 }, { 0, 5000 }
};
const POINT KShapeSmartBaseObject::_chevron[] = 
{
	{ 0, 0 }, { 0, 0 }, { 10000, 5000 }, { 0, 10000 }, { 0, 10000 }, { 0, 5000 }
};
const POINT KShapeSmartBaseObject::_RightArrowCallout[] = 
{
	{ 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 10000, 5000 },
	{ 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 10000 }, { 0, 10000 }
};
const POINT KShapeSmartBaseObject::_LeftArrowCallout[] = 
{
	{ 10000, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 5000 },
	{ 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 10000 }, { 10000, 10000 }
};
const POINT KShapeSmartBaseObject::_UpArrowCallout[] = 
{
	{ 10000, 10000 }, { 10000, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 5000, 0 },
	{ 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 10000 }
};
const POINT KShapeSmartBaseObject::_DownArrowCallout[] = 
{
	{ 10000, 0 }, { 10000, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 5000, 10000 },
	{ 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }
};
const POINT KShapeSmartBaseObject::_LeftRightArrowCallout[] = 
{
	{ 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 5000 },
	{ 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 10000 }, { 0, 10000 },
	{ 0, 0 }, { 0, 0 }, { 0, 0 }, { 10000, 5000 }, { 0, 0 }, { 0, 0 }, { 0, 0 }
};
const POINT KShapeSmartBaseObject::_UpDownArrowCallout[] = 
{
	{ 10000, 0 }, { 10000, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 5000, 0 },
	{ 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 },
	{ 0, 0 }, { 0, 0 }, { 0, 0 }, { 5000, 10000 }, { 0, 0 }, { 0, 0 }, { 0, 0 }
};
const POINT KShapeSmartBaseObject::_QuadArrowCallout[] = 
{
	{ 5000, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 },
	{ 10000, 5000 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 },
	{ 5000, 10000 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 },
	{ 0, 5000 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }
};
const POINT KShapeSmartBaseObject::_FlowChartPredefinedProcess[] = 
{
	{ 0, 0 }, { 0, 10000 }, { 0, 10000 }, { 0, 0 }, { 0, 10000 }, { 0, 10000 }, { 0, 0 }, { 0, 10000 }, { 10000, 10000 }, { 10000, 0 }
};
const POINT KShapeSmartBaseObject::_FlowChartInternalStorage[] = 
{
	{ 0, 0 }, { 0, 0 }, { 10000, 0 }, { 0, 0 }, { 0, 10000 }, { 0, 10000 }, { 0, 0 }, { 0, 10000 }, { 10000, 10000 }, { 10000, 0 }
};
const POINT KShapeSmartBaseObject::_FlowChartOffpageConnector[] = 
{
	{ 0, 0 }, { 0, 0 }, { 5000, 10000 }, { 10000, 0 }, { 10000, 0 }
};
const POINT KShapeSmartBaseObject::_FlowChartPunchedCard[] = 
{
	{ 0, 0 }, { 0, 10000 }, { 10000, 10000 }, { 10000, 0 }, { 0, 0 }
};
const POINT KShapeSmartBaseObject::_FlowChartCollate[] = 
{
	{ 0, 0 }, { 10000, 10000 }, { 0, 10000 }, { 10000, 0 }
};
const POINT KShapeSmartBaseObject::_FlowChartSort[] = 
{
	{ 10000, 5000 }, { 0, 5000 }, { 5000, 10000 }, { 10000, 5000 }, { 5000, 0 }
};
const POINT KShapeSmartBaseObject::_FlowChartMerge[] = 
{
	{ 0, 0 }, { 10000, 0 }, { 5000, 10000 }
};
const POINT KShapeSmartBaseObject::_Seal4[] = 
{
	{ 5000, 0 }, { 0, 0 }, { 10000, 5000 }, { 0, 0 }, { 5000, 10000 }, { 0, 0 }, { 0, 5000 }, { 0, 0 }
};
const POINT KShapeSmartBaseObject::_Star[] = 
{
	{ 5000, 0 }, { 6200, 3800 }, { 10000, 3800 }, { 6800, 6250 }, { 8050, 10000 }, 
	{ 5000, 7700 }, { 1950, 10000 }, { 3200, 6250 }, { 0, 3800 }, { 3800, 3800 }
};
const POINT KShapeSmartBaseObject::_IrregularSeal1[] = 
{
	{ 200, 1100 }, { 3450, 2850 }, { 3850, 1100 }, { 5000, 2650 }, { 6700, 0 }, 
	{ 6560, 2400 }, { 8500, 2050 }, { 7800, 3400 }, { 9800, 3750 }, { 8200, 4800 },
	{ 10000, 6150 }, { 7800, 6000 }, { 8400, 8400 }, { 6500, 6700 }, { 6150, 9150 }, 
	{ 4850, 6950 }, { 3970, 10000 }, { 3600, 7250 }, { 2300, 8200 }, { 2650, 6450 }, 
	{ 120, 6800 }, { 1700, 5400 }, { 0, 4000 }, { 2100, 3600 }
};
const POINT KShapeSmartBaseObject::_IrregularSeal2[] = 
{
	{ 0, 6000 }, { 1800, 5300 }, { 650, 3850 }, { 2500, 3700 }, { 2100, 1700 }, 
	{ 4000, 3000 }, { 4500, 900 }, { 5350, 2000 }, { 6850, 0 }, { 6800, 2650 }, 
	{ 8400, 1500 }, { 7600, 3000 }, { 10000, 3050 }, { 7800, 4300 }, { 8450, 5200 }, 
	{ 7600, 5700 }, { 8700, 7250 }, { 6800, 6750 }, { 6900, 8020 }, { 5600, 7400 }, 
	{ 5400, 8700 }, { 4650, 8020 }, { 4070, 9090 }, { 3500, 8460 }, { 2250, 10000 }, 
	{ 2180, 8420 }, { 700, 8200 }, { 1560, 7100 }
};
const POINT KShapeSmartBaseObject::_Cube[] = 
{
	{ 0, 0 }, { 0, 10000 }, { 0, 10000 }, { 0, 0 }, { 0, 0 }, 
	{ 0, 0 }, { 10000, 0 }, { 0, 0 }, { 10000, 0 }, { 10000, 0 }
};
const POINT KShapeSmartBaseObject::_Bevel[] = 
{
	{ 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, 
	{ 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 10000 }, { 0, 0 }, 
	{ 0, 0 }, { 10000, 10000 }, { 0, 0 }, { 0, 0 }, { 10000, 0 },
	{ 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 10000 }, { 10000, 10000 }, 
	{ 10000, 0 }, { 0, 0 } 
};
const CPOINT KShapeSmartBaseObject::_FlowChartMultidocumentShadow[] = 
{
	{ 10000, 6640, 0x3 },
	{ 10000, 6640, 0x2 },
	{ 10000, 6640, 0x2 },
	{ 10000, 6640, 0x4 },
	{ 8370, 6590, 0x4 },
	{ 6500, 7470, 0x2 },
	{ 4520, 8280, 0x4 },
	{ 4250, 8300, 0x4 },
	{ 3590, 8300, 0x2 },
	{ 2910, 8300, 0x4 },
	{ 1250, 7800, 0x4 },
	{ 1250, 7800, 0x2 },
	{ 1250, 0, 0x201a },
};

KShapeSmartBaseObject::KShapeSmartBaseObject(ShapeType type, LONG* adjust)
{
	mType = type;
	drawFlags = 0;
	memset(mAdjustValue, 0, sizeof(LONG) * SHAPE_ADJUST_NUMBER);

	if (adjust != NULL)
		memcpy(mAdjustValue, adjust, sizeof(LONG) * SHAPE_ADJUST_NUMBER);
	else
		GetDefaultAdjustValue(mType, mAdjustValue);
}

KShapeSmartBaseObject::~KShapeSmartBaseObject() 
{
}

void KShapeSmartBaseObject::convertPoints(CPOINT* lpdest, const POINT* lpsource, int cnt, int width, int height)
{
	for (int i = 0; i < cnt; i++)
		{
		lpdest[i].x = (LONG)( (double)lpsource[i].x * width / 10000 );
		lpdest[i].y = (LONG)( (double)lpsource[i].y * height / 10000 );
		lpdest[i].a = CP_LINE;
		}
}

void KShapeSmartBaseObject::convertPoints(CPOINT* lpdest, const POINT* lpsource, int cnt, int width, int height, int offx, int offy)
{
	for (int i = 0; i < cnt; i++)
		{
		lpdest[i].x = (LONG)( (double)(lpsource[i].x + offx) * width / 10000 );
		lpdest[i].y = (LONG)( (double)(lpsource[i].y + offy) * height / 10000 );
		lpdest[i].a = CP_LINE;
		}
}

void KShapeSmartBaseObject::convertPoints(CPOINT* lpdest, const CPOINT* lpsource, int cnt, int width, int height)
{
	for (int i = 0; i < cnt; i++)
		{
		lpdest[i].x = (LONG)( (double)lpsource[i].x * width / 10000 );
		lpdest[i].y = (LONG)( (double)lpsource[i].y * height / 10000 );
		lpdest[i].a = lpsource[i].a;
		}
}

void KShapeSmartBaseObject::convertPoints(CPOINT* lpdest, const CPOINT* lpsource, int cnt, int width, int height, int offx, int offy)
{
	for (int i = 0; i < cnt; i++)
		{
		lpdest[i].x = (LONG)( (double)(lpsource[i].x + offx) * width / 10000 );
		lpdest[i].y = (LONG)( (double)(lpsource[i].y + offy) * height / 10000 );
		lpdest[i].a = lpsource[i].a;
		}
}

void KShapeSmartBaseObject::ReadyPolygonFlag(CPOINT* lpPoint, int nCount)
{
	lpPoint[0].a = CP_LINE | CP_STAT;
	for(int i=1 ; i<nCount-1 ; i++)
		lpPoint[i].a = CP_LINE;
	lpPoint[nCount-1].a = CP_ENDS | CP_ENDG | CP_CLOSED;
}

void KShapeSmartBaseObject::RotatePt(int cx, int cy, int angle, int *x, int *y, int divide)
{
	int		bx, by;
	static double	_sin = 0., _cos = 1.;
	static int		old_angle = 0, old_divide = 0;
	double radian;

	bx = *x - cx;
	by = *y - cy;

	if (angle == 0)
		return;

	radian = atan(1.0) / 45.0;	

	if (angle != old_angle || divide != old_divide) 
	{
		old_angle	= angle;
		old_divide	= divide;

		_sin = sin(angle * radian / divide);
		_cos = cos(angle * radian / divide);
	}

	*x = ConvertDouble2Int(_cos * bx + _sin * by + cx);
	*y = ConvertDouble2Int(-_sin * bx + _cos * by + cy);
}

void KShapeSmartBaseObject::RotatePt(int cx, int cy, int angle, CPOINT* lppt)
{
	RotatePt(cx, cy, angle, &(lppt->x), &(lppt->y), 1);
}

CPOINT* KShapeSmartBaseObject::GetSealPoint(int& pnum, int num, long adjustValue, int width, int height)
{
	int		cx, cy, i;
	double	angle, angle_;
	int		xs;
	CPOINT*	lpPoint;

	lpPoint = new CPOINT[num * 2 + 3];
	if (lpPoint == NULL)
		return NULL;
	cx = width / 2;
	cy = height / 2;
	angle = (360. / (num * 2));
	xs = (int)((double)(width * adjustValue) / MAX_ADJUSTVALUE + 0.5);

	for (i = 0, angle_ = 0; i < num; i++, angle_ += angle)
	{
		lpPoint[i * 2].x = 0;
		lpPoint[i * 2].y = cx;
		RotatePt(cx, cx, (int)angle_, &lpPoint[i * 2]);
		angle_ += angle;
		lpPoint[i * 2 + 1].x = xs;
		lpPoint[i * 2 + 1].y = cx;
		RotatePt(cx, cx, (int)angle_, &lpPoint[i * 2 + 1]);
	}
	lpPoint[num * 2] = lpPoint[0];
	pnum = num * 2 + 1;

	if( (width > 0) && (height != width) )
		{
		double dy = (double) height / width;
		for(int i=0 ; i<pnum ; i++)
			{
			lpPoint[i].y = (int) (lpPoint[i].y * dy);
			}
		}
	return(lpPoint);
}

BOOL KShapeSmartBaseObject::IsShadowType(ShapeType shadowType)
{
	switch (shadowType) {
		case ShapeType::CubeShadow:
		case ShapeType::CanShadow:
		case ShapeType::BevelShadow:
		case ShapeType::CurvedRightArrowShadow:
		case ShapeType::CurvedLeftArrowShadow:
		case ShapeType::CurvedUpArrowShadow:
		case ShapeType::CurvedDownArrowShadow:
		case ShapeType::FlowChartMultidocumentShadow:
		case ShapeType::FlowChartMagneticDiskShadow:
		case ShapeType::FlowChartMagneticDrumShadow:
		case ShapeType::RibbonShadow:
		case ShapeType::Ribbon2Shadow:
		case ShapeType::VerticalScrollShadow:
		case ShapeType::HorizontalScrollShadow:
		case ShapeType::EllipseRibbonShadow:
		case ShapeType::EllipseRibbon2Shadow:
		case ShapeType::SmileyFaceShadow:
		case ShapeType::FoldedCornerShadow:
		case ShapeType::StripedRightArrowShadow:
			return TRUE;
	}
	return FALSE;
}


COLORREF GetLightColor(COLORREF cr, int nLightValue, int nMaxValue)
{
	if (nMaxValue == 0)
		return cr;

	int nMiddle;
	BYTE a = GetAlphaValue(cr);

	//최대값이 200이 되도록 보정한다.
	double fRatio = (double)nMaxValue / 200.;
	nMaxValue = (int)(nMaxValue / fRatio);

	nLightValue = (int)(nLightValue / fRatio);
	if (nLightValue > nMaxValue)
		nLightValue = nMaxValue;

	nMiddle = nMaxValue / 2;

	RGB_C rgb_c;
	HSV_C hsv_c;

	rgb_c.Red = GetRValue(cr);
	rgb_c.Grn = GetGValue(cr);
	rgb_c.Blu = GetBValue(cr);
	hsv_c = ConvertRGB2HSV(rgb_c);

	if (nLightValue <= nMiddle)
	{
		if (nLightValue > 100)
			nLightValue = 100;
		hsv_c.Val = (int)(hsv_c.Val / 100. * nLightValue);
		rgb_c = ConvertHSV2RGB(hsv_c);
	}
	else
	{
		nLightValue -= nMiddle;
		if (nLightValue > nMiddle)
			nLightValue = nMiddle;
		hsv_c.Val = (int)(hsv_c.Val + (100. - hsv_c.Val) / 100. * nLightValue);
		hsv_c.Sat = (int)(hsv_c.Sat - ((int)hsv_c.Sat * nLightValue / 100.));
		rgb_c = ConvertHSV2RGB(hsv_c);
	}

	return MAKE_ARGB(a, rgb_c.Red, rgb_c.Grn, rgb_c.Blu);
}

DWORD MakeShapeElementColor(BasicElementColorType colorBright, DWORD color)
{
	if (colorBright == BasicElementColorType::HOLLOW)
		return 0;

	switch (colorBright) {
	case BasicElementColorType::LOW_MEDIUM_BRIGHT:
		color = GetLightColor(color, 80, 200);
		break;
	case BasicElementColorType::LOW_BRIGHT:
		color = GetLightColor(color, 60, 200);
		break;
	case BasicElementColorType::HIGH_BRIGHT:
		color = GetLightColor(color, 140, 200);
		break;
	case BasicElementColorType::HIGH_MEDIUM_BRIGHT:
		color = GetLightColor(color, 120, 200);
		break;
	}

	return color;
}

void KShapeSmartBaseObject::drawRectangleElement(Graphics& g, ShapeDrawerHelper& helper, RECT& r, COLORREF color, OutlineStyle& outline)
{
	color = MakeShapeElementColor(helper.colorBright, color);
	BYTE a = GetAlphaValue(color);

	if ((a > 0) && (helper.penBrushFlag & DRAW_WITH_BRUSH))
	{
		SolidBrush brush(color);
		g.FillRectangle(&brush, r.left, r.top, r.right - r.left, r.bottom - r.top);
	}

	if ((outline.Style > (BYTE)LineStyle::LineHollow) && (helper.penBrushFlag & DRAW_WITH_PEN))
	{
		float width = outline.Width * helper.pInfo->currentZoom;
		Pen pen(Color(outline.Color), width);

		if (outline.Style == (BYTE)LineStyle::LineDot)
			pen.SetDashStyle(DashStyleDot);
		else if (outline.Style == (BYTE)LineStyle::LineDash)
			pen.SetDashStyle(DashStyleDash);
		else if (outline.Style == (BYTE)LineStyle::LineDashDot)
			pen.SetDashStyle(DashStyleDashDot);

		g.DrawRectangle(&pen, r.left, r.top, r.right - r.left, r.bottom - r.top);
	}
}

void KShapeSmartBaseObject::drawPathElement(Graphics& g, ShapeDrawerHelper& helper, GraphicsPath& path, COLORREF color, OutlineStyle& outline)
{
	color = MakeShapeElementColor(helper.colorBright, color);
	BYTE a = GetAlphaValue(color);

	if ((a > 0) && (helper.penBrushFlag & DRAW_WITH_BRUSH))
	{
		SolidBrush brush(color);
		g.FillPath(&brush, &path);
	}

	if ((outline.Style > (BYTE)LineStyle::LineHollow) && (helper.penBrushFlag & DRAW_WITH_PEN))
	{
		float width = outline.Width * helper.pInfo->currentZoom;
		Pen pen(Color(outline.Color), width);

		if (outline.Style == (BYTE)LineStyle::LineDot)
			pen.SetDashStyle(DashStyleDot);
		else if (outline.Style == (BYTE)LineStyle::LineDash)
			pen.SetDashStyle(DashStyleDash);
		else if (outline.Style == (BYTE)LineStyle::LineDashDot)
			pen.SetDashStyle(DashStyleDashDot);

		g.DrawPath(&pen, &path);
	}
}

void KShapeSmartBaseObject::drawRoundRectangleElement(Graphics& g, ShapeDrawerHelper& helper, RECT& rect, int rx, int ry, COLORREF color, OutlineStyle& outline)
{
	GraphicsPath path;

	path.AddLine(rect.left + rx, rect.top, rect.right - rx, rect.top);
	path.AddArc(rect.right - rx * 2, rect.top, rx * 2, ry * 2, 270, 90);
	path.AddLine(rect.right, rect.top + ry, rect.right, rect.bottom - ry);
	path.AddArc(rect.right - rx * 2, rect.bottom - ry * 2, rx * 2, ry * 2, 0, 90);
	path.AddLine(rect.right - rx, rect.bottom, rect.left + rx, rect.bottom);
	path.AddArc(rect.left, rect.bottom - ry * 2, rx * 2, ry * 2, 90, 90);
	path.AddLine(rect.left, rect.bottom - ry, rect.left, rect.top + ry);
	path.AddArc(rect.left, rect.top, rx * 2, ry * 2, 180, 90);
	path.CloseFigure();

	drawPathElement(g, helper, path, color, outline);
}

void KShapeSmartBaseObject::drawEllipseElement(Graphics& g, ShapeDrawerHelper& helper, RECT& r, COLORREF color, OutlineStyle& outline)
{
	color = MakeShapeElementColor(helper.colorBright, color);
	BYTE a = GetAlphaValue(color);

	if ((a > 0) && (helper.penBrushFlag & DRAW_WITH_BRUSH))
	{
		SolidBrush brush(color);
		g.FillEllipse(&brush, r.left, r.top, r.right - r.left, r.bottom - r.top);
	}

	if ((outline.Style > (BYTE)LineStyle::LineHollow) && (helper.penBrushFlag & DRAW_WITH_PEN))
	{
		float width = outline.Width * helper.pInfo->currentZoom;
		Pen pen(Color(outline.Color), width);

		if (outline.Style == (BYTE)LineStyle::LineDot)
			pen.SetDashStyle(DashStyleDot);
		else if (outline.Style == (BYTE)LineStyle::LineDash)
			pen.SetDashStyle(DashStyleDash);
		else if (outline.Style == (BYTE)LineStyle::LineDashDot)
			pen.SetDashStyle(DashStyleDashDot);

		g.DrawEllipse(&pen, r.left, r.top, r.right - r.left, r.bottom - r.top);
	}
}

void KShapeSmartBaseObject::drawPolygonElement(Graphics& g, ShapeDrawerHelper& helper, LP_CPOINT points, int pointNumber, COLORREF color, OutlineStyle& outline, BOOL isEvenOdd)
{
	GraphicsPath path;
	int ox = helper.renderRect.left;
	int oy = helper.renderRect.top;

	for (int i = 0; i < pointNumber; i++)
	{
		if (points[i + 1].a & CPOINT_CURV)
		{
			path.AddBezier(ox + points[i].x, oy + points[i].y, ox + points[i + 1].x, oy + points[i + 1].y,
				ox + points[i + 2].x, oy + points[i + 2].y, ox + points[i + 3].x, oy + points[i + 3].y);
			i += 2;
		}
		else
		{
			path.AddLine(ox + points[i].x, oy + points[i].y, ox + points[i + 1].x, oy + points[i + 1].y);
		}
		if (points[i + 1].a & CP_CLOSED) //CPOINT_ENDS)
		{
			path.CloseFigure();
		}
		if (points[i + 1].a & CPOINT_ENDG)
			break;
		if (points[i + 1].a & CPOINT_ENDS)
		{
			path.StartFigure();
			i++;
		}
	}
	//path.CloseFigure();

	path.SetFillMode(isEvenOdd ? FillMode::FillModeAlternate : FillMode::FillModeWinding);
	drawPathElement(g, helper, path, color, outline);
}

#ifdef USE_XML_STORAGE
HRESULT KShapeSmartBaseObject::Store(CComPtr<IXmlWriter> pWriter)
{
	HRESULT hr;

	if (FAILED(hr = pWriter->WriteStartElement(NULL, tagSmartShape, NULL)))
	{
		wprintf(L"Error, Method: WriteStartElement, error is %08.8lx", hr);
		return -1;
	}

	TCHAR val[16];
	StringCchPrintf(val, 16, _T("%d"), (int)mType);

	if (FAILED(hr = pWriter->WriteAttributeString(NULL, L"Type", NULL, val)))
	{
		wprintf(L"Error, Method: WriteAttributeString, error is %08.8lx", hr);
		return -1;
	}

	TCHAR attr_name[16];
	POINT handles[6];
	int num = GetAdjustHandlePoint(handles);
	for (int i = 0; i < num; i++)
	{
		StringCchPrintf(attr_name, 16, _T("adjx%d"), i + 1);
		StringCchPrintf(val, 16, _T("%d"), handles[i].x);
		pWriter->WriteAttributeString(NULL, attr_name, NULL, val);

		StringCchPrintf(attr_name, 16, _T("adjy%d"), i + 1);
		StringCchPrintf(val, 16, _T("%d"), handles[i].y);
		pWriter->WriteAttributeString(NULL, attr_name, NULL, val);
	}

	StoreShapeData(pWriter);

	if (FAILED(hr = pWriter->WriteEndElement()))
	{
		wprintf(L"Error, Method: WriteEndElement, error is %08.8lx", hr);
		return -1;
	}

	return 1;
}
#endif // USE_XML_STORAGE

BOOL KShapeSmartBaseObject::LoadFromBufferExt(KMemoryStream& mf)
{
	// mf.Read(&mType, sizeof(int));

	// AdjustValues
	int num;
	mf.Read(&num, sizeof(int));
	if (num > 0)
	{
		mf.Read(mAdjustValue, sizeof(LONG) * num);
	}

	LoadShapeDataFromBufferExt(mf);
	return TRUE;
}

BOOL KShapeSmartBaseObject::StoreToBufferExt(KMemoryStream& mf)
{
	// mf.Write(&mType, sizeof(int));

	int num = GetDefaultAdjustValue(mType, NULL);

	mf.Write(&num, sizeof(int));
	if (num > 0)
		mf.Write(mAdjustValue, sizeof(LONG) * num);

	StoreShapeDataToBufferExt(mf);
	return TRUE;
}


/*************************************************
KShapeSmartLineObject
*************************************************/
KShapeSmartLineObject::KShapeSmartLineObject(ShapeType type)
	:KShapeSmartBaseObject(type)
{
	memset(&StartPoint, 0, sizeof(POINT));
	memset(&EndPoint, 0, sizeof(POINT));
	StartArrow = 0;
	EndArrow = 0;
	memset(&Outline, 0, sizeof(OutlineStyle));
}

KShapeSmartLineObject::KShapeSmartLineObject(ShapeType type, POINT& sp, POINT& ep, OutlineStyle linestyle, LONG* adjust, BYTE start, BYTE end)
	:KShapeSmartBaseObject(type, adjust)
{
	StartPoint = sp;
	EndPoint = ep;
	memcpy(&Outline, &linestyle, sizeof(OutlineStyle));
	StartArrow = start;
	EndArrow = end;
}

KShapeSmartLineObject::~KShapeSmartLineObject()
{

}

BOOL KShapeSmartLineObject::GetOutlineStyle(OutlineStyle& linestyle)
{
	memcpy(&linestyle, &Outline, sizeof(OutlineStyle));
	return TRUE;
}

BOOL KShapeSmartLineObject::SetOutlineStyle(OutlineStyle& linestyle)
{
	memcpy(&Outline, &linestyle, sizeof(OutlineStyle));
	return TRUE;
}

#ifdef USE_XML_STORAGE
HRESULT KShapeSmartLineObject::StoreShapeData(CComPtr<IXmlWriter> pWriter)
{
	StoreStartEndPoint(StartPoint, EndPoint, pWriter);
	StoreOutline(Outline, pWriter);
	return S_OK;
}
#endif // USE_XML_STORAGE

BOOL KShapeSmartLineObject::LoadShapeDataFromBufferExt(KMemoryStream& mf)
{
	mf.Read(&StartPoint, sizeof(POINT));
	mf.Read(&EndPoint, sizeof(POINT));
	mf.Read(&Outline, sizeof(OutlineStyle));
	return TRUE;
}

BOOL KShapeSmartLineObject::StoreShapeDataToBufferExt(KMemoryStream& mf)
{
	mf.Write(&StartPoint, sizeof(POINT));
	mf.Write(&EndPoint, sizeof(POINT));
	mf.Write(&Outline, sizeof(OutlineStyle));
	return TRUE;
}

/*************************************************
KShapeSmartRectObject
*************************************************/
KShapeSmartRectObject::KShapeSmartRectObject(ShapeType type)
	:KShapeSmartBaseObject(type)
{
	memset(&Rect, 0, sizeof(RECT));
	memset(&Outline, 0, sizeof(OutlineStyle));
	memset(&Fill, 0, sizeof(DWORD));
}

KShapeSmartRectObject::KShapeSmartRectObject(ShapeType type, RECT& rect, DWORD fillcolor, OutlineStyle linestyle, LONG* adjust)
	:KShapeSmartBaseObject(type, adjust)
{
	Rect = rect;
	Outline = linestyle;
	Fill = fillcolor;
}

KShapeSmartRectObject::~KShapeSmartRectObject()
{

}

BOOL KShapeSmartRectObject::GetFillColor(DWORD& color)
{
	color = Fill;
	return TRUE;
}
BOOL KShapeSmartRectObject::GetOutlineStyle(OutlineStyle& linestyle)
{
	memcpy(&linestyle, &Outline, sizeof(OutlineStyle));
	return TRUE;
}

BOOL KShapeSmartRectObject::SetFillColor(DWORD& color)
{
	Fill = color;
	return TRUE;
}
BOOL KShapeSmartRectObject::SetOutlineStyle(OutlineStyle& linestyle)
{
	memcpy(&Outline, &linestyle, sizeof(OutlineStyle));
	return TRUE;
}

BasicGraphElement KShapeSmartRectObject::GetChild(int child, ShapeDrawerHelper& helper, LPRECT rect, CPOINT** lppPts, int* pnum, LONG* pAdjustValue)
{
	int		width = (helper.renderRect.right - helper.renderRect.left) - 1;
	int		height = (helper.renderRect.bottom - helper.renderRect.top) - 1;
	CPOINT* lpPoint = NULL;

	*pnum = 0;
	helper.colorBright = BasicElementColorType::NORMAL;

	switch (mType)
	{
	case ShapeType::BevelShadow:
	case ShapeType::Rectangle:
	case ShapeType::FlowChartProcess:			// = 109,
		rect->left = 0;
		rect->top = 0;
		rect->right = width;
		rect->bottom = height;
		return BasicGraphElement::RectElement;
	case ShapeType::RoundRectangle:
	case ShapeType::FlowChartAlternateProcess:
		rect->left = 0;
		rect->top = 0;
		rect->right = width;
		rect->bottom = height;
		return BasicGraphElement::RoundRectangle;
	case ShapeType::SmileyFaceShadow:
	case ShapeType::Ellipse:
	case ShapeType::FlowChartConnector:		// = 120,
		rect->left = 0;
		rect->top = 0;
		rect->right = width;
		rect->bottom = height;
		return BasicGraphElement::EllipseElement;
	case ShapeType::CubeShadow:			// 큐브 그림자
	{
		lpPoint = new CPOINT[7];
		int AdjValueH = (LONG)((double)height * pAdjustValue[0] / MAX_ADJUSTVALUE);
		int AdjValueW = (LONG)((double)width * pAdjustValue[0] / MAX_ADJUSTVALUE);
		int aV = 0;
		int aW = abs(width);
		int aH = abs(height);
		int i = 0;

		if (abs(AdjValueW) > abs(AdjValueH)) { // 조절점이 위/아래에 위치, 값은 Height에 비례
			aV = abs(AdjValueH);
		}
		else {
			aV = abs(AdjValueW);
		}
		lpPoint[0].x = 0;
		lpPoint[0].y = aV;
		lpPoint[1].x = 0;
		lpPoint[1].y = aH;
		lpPoint[1].a = CP_LINE;
		lpPoint[2].x = aW - aV;
		lpPoint[2].y = aH;
		lpPoint[2].a = CP_LINE;
		lpPoint[3].x = aW;
		lpPoint[3].y = aH - aV;
		lpPoint[3].a = CP_LINE;
		lpPoint[4].x = aW;
		lpPoint[4].y = 0;
		lpPoint[4].a = CP_LINE;
		lpPoint[5].x = aV;
		lpPoint[5].y = 0;
		lpPoint[5].a = CP_LINE;
		lpPoint[6] = lpPoint[0];
		lpPoint[0].a = CP_STAT | CP_LINE;
		lpPoint[6].a = CP_LINE | CP_ENDS | CP_ENDG | CP_CLOSED;

		if (width < 0) { // y축 대칭
			for (i = 0; i < 7; i++)
				lpPoint[i].x *= -1;
		}
		if (height < 0) { // x축 대칭
			for (i = 0; i < 7; i++)
				lpPoint[i].y *= -1;
		}
		*pnum = 7;
	}
	break;
	case ShapeType::CanShadow:	// 원기둥 그림자
	{
		int halfHeight = (int)((double)(10000 / 2) * pAdjustValue[0] / 21600 + 0.5); // 뚜껑 원의 세로 절반

		lpPoint = new CPOINT[15];

		MakeRoundCorner(&(lpPoint[0]), 0, halfHeight, 5000, 0, width, height, FALSE, FALSE);
		MakeRoundCorner(&(lpPoint[3]), 5000, 0, 10000, halfHeight, width, height, TRUE, FALSE);
		lpPoint[5].a = CP_LINE;
		lpPoint[6].x = width;
		lpPoint[6].y = (LONG)((double)height * halfHeight / 10000);
		lpPoint[6].a = CP_LINE;

		MakeRoundCorner(&(lpPoint[7]), 10000, 10000 - halfHeight, 5000, 10000, width, height, FALSE, FALSE);
		MakeRoundCorner(&(lpPoint[10]), 5000, 10000, 0, 10000 - halfHeight, width, height, TRUE, TRUE);

		lpPoint[0].a = CP_STAT;
		lpPoint[13].a = CP_LINE;
		lpPoint[14] = lpPoint[0];
		lpPoint[14].a = CP_LINE | CP_ENDS | CP_CLOSED;
		*pnum = 15;
	}
	break;
	case ShapeType::CurvedLeftArrowShadow:
	case ShapeType::CurvedRightArrowShadow:
	{
		int arrowback_ConnectTop_y = 0;		// 삼각형 꼬리접합부분의 위쪽 y좌표
		int arrow_top_y = 0;				// 삼각형 위쪽 y좌표
		int arrow_width = 0;				// 삼각형 두께
		int arrowback_Connect_x = 0;		// 삼각형 꼬리접합부분의 x좌표
		int arrowback_ConnectBottom_y = 0;	// 삼각형 꼬리접합부분의 아래쪽 y좌표
		int arrowfront_y = 0;				// 삼각형 앞부분의 y좌표
		int arrow_margin = 0;				// 삼각형 뒤쪽 열린부분을 제외한 부분의 절반
		int strip_height = 0;				// 삼각형 뒤쪽 열린부분의 길이
		int	p = 0, dy = 0;
		POINT cutline[2];

		arrowback_ConnectTop_y = arrow_top_y = (int)((double)10000 * pAdjustValue[0] / 21600);
		arrowback_ConnectBottom_y = (int)((double)10000 * pAdjustValue[1] / 21600);
		arrow_margin = 10000 - arrowback_ConnectBottom_y;
		strip_height = 10000 - arrow_top_y - arrow_margin * 2;
		arrow_width = arrowback_Connect_x = (int)((double)10000 * pAdjustValue[2] / 21600);
		arrowfront_y = (arrowback_ConnectTop_y + arrowback_ConnectBottom_y) / 2;

		if (mType == ShapeType::CurvedRightArrowShadow)
			arrow_width = 10000 - arrowback_Connect_x;
		arrowback_ConnectTop_y += arrow_margin;

		lpPoint = new CPOINT[22];

		cutline[0].x = (10000 - arrow_width);
		cutline[0].y = 0;
		cutline[1].x = (10000 - arrow_width);
		cutline[1].y = 10000;

		MakeRoundCorner(&(lpPoint[0]), 0, arrowback_ConnectTop_y / 2, 10000, arrowback_ConnectTop_y, 10000, 10000, FALSE, TRUE);

		if (arrow_width > 0) {
			p = SplitBezierSegmentByLine(&(lpPoint[0]), cutline, 0);
		}
		else {
			p = 4;
		}

		if (arrow_margin > 0) { // move arrow position vertically					
			dy = arrowback_ConnectTop_y - lpPoint[p - 1].y;
			lpPoint[p].x = (10000 - arrow_width);
			lpPoint[p].y = (arrowback_ConnectTop_y - arrow_margin) - dy;
			lpPoint[p].a = CP_LINE;
			p++;
			if (arrow_width > 0) {
				lpPoint[p].x = 10000;
				lpPoint[p].y = (arrowback_ConnectTop_y + strip_height / 2);
				lpPoint[p].a = CP_LINE;
				p++;
			}
			lpPoint[p].x = (10000 - arrow_width);
			lpPoint[p].y = 10000 - dy;
			lpPoint[p].a = CP_LINE;
			p++;
		}
		else {
			lpPoint[p].x = 10000;
			lpPoint[p].y = (arrowback_ConnectTop_y + strip_height / 2);
			lpPoint[p].a = CP_LINE;
			p++;
		}
		MakeRoundCorner(&(lpPoint[p]), 10000, arrowback_ConnectTop_y + strip_height, 0, arrowback_ConnectTop_y / 2 + strip_height, 10000, 10000, TRUE, TRUE);
		if (arrow_width > 0) {
			p += SplitBezierSegmentByLine(&(lpPoint[p]), cutline, 1);
		}
		else {
			p += 4;
		}
		lpPoint[p - 1].a = CP_LINE;
		lpPoint[p] = lpPoint[0];
		lpPoint[p].a = CP_LINE;
		MakeRoundCorner(&(lpPoint[p + 1]), 0, arrowback_ConnectTop_y / 2, 10000, 0, 10000, 10000, FALSE, TRUE);
		lpPoint[p + 4].a = CP_LINE;
		lpPoint[p + 5].x = 10000;
		lpPoint[p + 5].y = strip_height;
		lpPoint[p + 5].a = CP_LINE;
		MakeRoundCorner(&(lpPoint[p + 6]), 10000, strip_height, 0, arrowback_ConnectTop_y / 2 + strip_height / 2, 10000, 10000, TRUE, TRUE);

		lpPoint[0].a = CP_STAT;
		lpPoint[p + 9].a = CP_LINE | CP_ENDS | CP_ENDG | CP_CLOSED;
		*pnum = p + 10;
		if ((mType == ShapeType::CurvedLeftArrowShadow) && (lpPoint != NULL))
			HFlopCPoint(*pnum, lpPoint, 5000);
		convertPoints(lpPoint, lpPoint, p + 10, width, height);
	}
	break;
	case ShapeType::CurvedDownArrowShadow:
	case ShapeType::CurvedUpArrowShadow:
	{
		int arrowback_ConnectTop_y = 0;		// 삼각형 꼬리접합부분의 위쪽 y좌표
		int arrow_top_y = 0;				// 삼각형 위쪽 y좌표
		int arrow_width = 0;				// 삼각형 두께
		int arrowback_Connect_x = 0;		// 삼각형 꼬리접합부분의 x좌표
		int arrowback_ConnectBottom_y = 0;	// 삼각형 꼬리접합부분의 아래쪽 y좌표
		int arrowfront_y = 0;				// 삼각형 앞부분의 y좌표
		int arrow_margin = 0;				// 삼각형 뒤쪽 열린부분을 제외한 부분의 절반
		int strip_height = 0;				// 삼각형 뒤쪽 열린부분의 길이
		int	p = 0, dy = 0;
		POINT cutline[2];

		arrowback_ConnectTop_y = arrow_top_y = (int)((double)10000 * pAdjustValue[0] / 21600);
		arrowback_ConnectBottom_y = (int)((double)10000 * pAdjustValue[1] / 21600);
		arrow_margin = 10000 - arrowback_ConnectBottom_y;
		strip_height = 10000 - arrow_top_y - arrow_margin * 2;
		arrow_width = arrowback_Connect_x = (int)((double)10000 * pAdjustValue[2] / 21600);
		arrowfront_y = (arrowback_ConnectTop_y + arrowback_ConnectBottom_y) / 2;

		if (mType == ShapeType::CurvedDownArrowShadow)
			arrow_width = 10000 - arrowback_Connect_x;
		arrowback_ConnectTop_y += arrow_margin;

		lpPoint = new CPOINT[22];

		cutline[0].x = 0;
		cutline[0].y = arrow_width;
		cutline[1].x = 10000;
		cutline[1].y = arrow_width;

		MakeRoundCorner(&(lpPoint[0]), arrowback_ConnectTop_y / 2, 10000, arrowback_ConnectTop_y, 0, 10000, 10000, TRUE, TRUE);

		if (arrow_width > 0) {
			p = SplitBezierSegmentByLine(&(lpPoint[0]), cutline, 0);
		}
		else {
			p = 4;
		}

		if (arrow_margin > 0) { // move arrow position vertically					
			dy = arrowback_ConnectTop_y - lpPoint[p - 1].x;
			lpPoint[p].x = (arrowback_ConnectTop_y - arrow_margin) - dy;
			lpPoint[p].y = arrow_width;
			lpPoint[p].a = CP_LINE;
			p++;
			if (arrow_width > 0) {
				lpPoint[p].x = (arrowback_ConnectTop_y + strip_height / 2);
				lpPoint[p].y = 0;
				lpPoint[p].a = CP_LINE;
				p++;
			}
			lpPoint[p].x = 10000 - dy;
			lpPoint[p].y = arrow_width;
			lpPoint[p].a = CP_LINE;
			p++;
		}
		else {
			if (arrow_width > 0) {
				lpPoint[p].x = arrowback_ConnectTop_y + strip_height / 2;
				lpPoint[p].y = 0;
				lpPoint[p].a = CP_LINE;
				p++;
			}
		}
		MakeRoundCorner(&(lpPoint[p]), arrowback_ConnectTop_y + strip_height, 0, arrowback_ConnectTop_y / 2 + strip_height, 10000, 10000, 10000, FALSE, TRUE);
		if (arrow_width > 0) {
			p += SplitBezierSegmentByLine(&(lpPoint[p]), cutline, 1);
		}
		else {
			p += 4;
		}
		lpPoint[p - 1].a = CP_LINE;
		lpPoint[p] = lpPoint[0];
		lpPoint[p].a = CP_LINE;
		MakeRoundCorner(&(lpPoint[p + 1]), arrowback_ConnectTop_y / 2, 10000, 0, 0, 10000, 10000, TRUE, TRUE);
		lpPoint[p + 4].a = CP_LINE;
		lpPoint[p + 5].x = strip_height;
		lpPoint[p + 5].y = 0;
		lpPoint[p + 5].a = CP_LINE;
		MakeRoundCorner(&(lpPoint[p + 6]), strip_height, 0, arrowback_ConnectTop_y / 2 + strip_height / 2, 10000, 10000, 10000, FALSE, TRUE);

		lpPoint[0].a = CP_STAT;
		lpPoint[p + 9].a = CP_LINE | CP_ENDS | CP_ENDG | CP_CLOSED;
		*pnum = p + 10;
		if ((mType == ShapeType::CurvedDownArrowShadow) && (lpPoint != NULL))
			VFlopCPoint(*pnum, lpPoint, 5000);
		convertPoints(lpPoint, lpPoint, p + 10, width, height);
	}
	break;
	case ShapeType::FlowChartMagneticDiskShadow:
	{
		lpPoint = new CPOINT[15];

		MakeRoundCorner(&(lpPoint[0]), 0, 1500, 5000, 0, width, height, FALSE, FALSE);
		MakeRoundCorner(&(lpPoint[3]), 5000, 0, 10000, 1500, width, height, TRUE, FALSE);
		lpPoint[5].a = CP_LINE;
		lpPoint[6].x = width;
		lpPoint[6].y = (LONG)((double)height * 1500 / 10000);
		lpPoint[6].a = CP_LINE;

		MakeRoundCorner(&(lpPoint[7]), 10000, 10000 - 1500, 5000, 10000, width, height, FALSE, FALSE);
		MakeRoundCorner(&(lpPoint[10]), 5000, 10000, 0, 10000 - 1500, width, height, TRUE, TRUE);

		lpPoint[0].a = CP_STAT;
		lpPoint[13].a = CP_LINE;
		lpPoint[14] = lpPoint[0];
		lpPoint[14].a = CP_LINE | CP_ENDS | CP_CLOSED;
		*pnum = 15;
	}
	break;
	case ShapeType::FlowChartMagneticDrumShadow:
	{
		int depth = 1700;
		lpPoint = new CPOINT[27];

		MakeRoundCorner(&(lpPoint[0]), 10000 - depth, 0, 10000 - depth * 2, 5000, width, height, TRUE, FALSE);
		MakeRoundCorner(&(lpPoint[3]), 10000 - depth * 2, 5000, 10000 - depth, 10000, width, height, FALSE, TRUE);

		lpPoint[0].a = CP_STAT | CP_LINE;
		lpPoint[5].a = CP_LINE;
		lpPoint[6].x = (LONG)((double)width * (10000 - depth) / 10000);
		lpPoint[6].y = height;
		lpPoint[6].a = CP_LINE;
		MakeRoundCorner(&(lpPoint[7]), depth, 10000, 0, 5000, width, height, TRUE, TRUE);
		MakeRoundCorner(&(lpPoint[10]), 0, 5000, depth, 0, width, height, FALSE, TRUE);
		lpPoint[13].a = CP_LINE;
		lpPoint[14] = lpPoint[0];
		lpPoint[14].a = CP_LINE | CP_ENDS | CP_ENDG | CP_CLOSED;

		MakeRoundCorner(&(lpPoint[14]), 10000 - depth, 0, 10000, 5000, width, height, TRUE, FALSE);
		MakeRoundCorner(&(lpPoint[17]), 10000, 5000, 10000 - depth, 10000, width, height, FALSE, FALSE);
		MakeRoundCorner(&(lpPoint[20]), 10000 - depth, 10000, 10000 - depth * 2, 5000, width, height, TRUE, FALSE);
		MakeRoundCorner(&(lpPoint[23]), 10000 - depth * 2, 5000, 10000 - depth, 0, width, height, FALSE, TRUE);
		lpPoint[0].a = CP_STAT | CP_LINE;
		lpPoint[26].a = CP_LINE | CP_ENDS | CP_ENDG | CP_CLOSED;
		*pnum = 27;
	}
	break;
	case ShapeType::FlowChartMultidocumentShadow:
	{
		lpPoint = new CPOINT[25];
		lpPoint[0].x = 1250;
		lpPoint[0].y = 0;
		lpPoint[0].a = 0x3;
		lpPoint[1].x = 10000;
		lpPoint[1].y = 0;
		lpPoint[1].a = 0x2;
		lpPoint[2].x = 10000;
		lpPoint[2].y = 6640;
		lpPoint[2].a = 0x2;
		lpPoint[3].x = 10000 - 680;
		lpPoint[3].y = 6640;
		lpPoint[3].a = 0x2;
		lpPoint[4].x = 10000 - 680;
		lpPoint[4].y = 6640 + 850;
		lpPoint[4].a = 0x2;
		lpPoint[5].x = 10000 - 1250;
		lpPoint[5].y = 6640 + 850;
		lpPoint[5].a = 0x2;
		lpPoint[6].x = 10000 - 1250;
		lpPoint[6].y = 6640 + 1700;
		lpPoint[6].a = 0x2;
		lpPoint[7].x = 10000 - 1250;
		lpPoint[7].y = 6640 + 1700;
		lpPoint[7].a = 0x4;
		/*
		*pnum = 8;
		lpPoint[7].a = CP_LINE | CP_ENDS | CP_ENDG | CP_CLOSED;
		*/

		//int pt = sizeof(_FlowChartMultidocumentShadow) / sizeof(CPOINT);
		convertPoints(&(lpPoint[8]), _FlowChartMultidocumentShadow, 13, 10000, 10000, -1250, 1700);
		lpPoint[20].a = CP_LINE;
		lpPoint[21].x = 680;
		lpPoint[21].y = 1700;
		lpPoint[21].a = CP_LINE;
		lpPoint[22].x = 680;
		lpPoint[22].y = 850;
		lpPoint[22].a = CP_LINE;
		lpPoint[23].x = 1250;
		lpPoint[23].y = 850;
		lpPoint[23].a = CP_LINE;
		lpPoint[24].x = 1250;
		lpPoint[24].y = 0;
		lpPoint[24].a = CP_LINE | CP_ENDS | CP_ENDG | CP_CLOSED;
		*pnum = 25;

		convertPoints(lpPoint, lpPoint, *pnum, width, height);
	}
	break;
	case ShapeType::RibbonShadow:
	case ShapeType::Ribbon2Shadow:
	{
		int yMargin = (int)((double)height * (21600 - pAdjustValue[1]) / 21600 + 0.5);
		int yMarginLog = (int)((double)10000 * (21600 - pAdjustValue[1]) / 21600 + 0.5);
		int	p = 0;
		int	x1 = (int)((double)pAdjustValue[0] * 10000 / 21600 + 0.5);
		int x2 = (int)((double)(pAdjustValue[0] + 2700) * 10000 / 21600 + 0.5);

		if (mType == ShapeType::RibbonShadow) {
			yMargin = (int)((double)height * pAdjustValue[1] / 21600 + 0.5);
			yMarginLog = (int)((double)10000 * pAdjustValue[1] / 21600 + 0.5);
		}

		lpPoint = new CPOINT[36];

		lpPoint[p].x = 0;
		lpPoint[p].y = height;
		lpPoint[p++].a = CP_STAT | CP_LINE;
		lpPoint[p].x = 1270 * width / 10000;
		lpPoint[p].y = height / 2 + yMargin / 2;
		lpPoint[p++].a = CP_LINE;
		lpPoint[p].x = 0;
		lpPoint[p].y = yMargin;
		lpPoint[p++].a = CP_LINE;
		lpPoint[p].x = (int)((double)pAdjustValue[0] * width / 21600 + 0.5);
		lpPoint[p].y = yMargin;
		lpPoint[p++].a = CP_LINE;

		MakeRoundCorner(&(lpPoint[p]), x1, yMarginLog / 4, x1 + 310, 0, width, height, FALSE, TRUE);
		p += 4;
		MakeRoundCorner(&(lpPoint[p]), (10000 - x1 - 310), 0, (10000 - x1), yMarginLog / 4, width, height, TRUE, TRUE);
		p += 4;

		lpPoint[p].x = width - (int)((double)pAdjustValue[0] * width / 21600 + 0.5);
		lpPoint[p].y = yMargin;
		lpPoint[p++].a = CP_LINE;
		lpPoint[p].x = width;
		lpPoint[p].y = yMargin;
		lpPoint[p++].a = CP_LINE;
		lpPoint[p].x = (10000 - 1270) * width / 10000;
		lpPoint[p].y = height / 2 + yMargin / 2;
		lpPoint[p++].a = CP_LINE;
		lpPoint[p].x = width;
		lpPoint[p].y = height;
		lpPoint[p++].a = CP_LINE;
		lpPoint[p].x = (10000 - x2 + x1) * width / 10000;
		lpPoint[p].y = height;
		lpPoint[p++].a = CP_LINE;
		x1 = 310;
		MakeRoundCorner(&(lpPoint[p]), 10000 - x2 + x1, 10000, 10000 - x2, 10000 - yMarginLog / 4, width, height, TRUE, TRUE);
		p += 4;

		lpPoint[p].x = width - (int)((double)(pAdjustValue[0] + 2700) * width / 21600 + 0.5);
		lpPoint[p].y = height - yMargin;
		lpPoint[p++].a = CP_LINE;
		lpPoint[p].x = (int)((double)(pAdjustValue[0] + 2700) * width / 21600 + 0.5);
		lpPoint[p].y = height - yMargin;
		lpPoint[p++].a = CP_LINE;

		x1 = -310;
		MakeRoundCorner(&(lpPoint[p]), x2, 10000 - yMarginLog / 4, x2 + x1, 10000, width, height, FALSE, TRUE);
		p += 4;

		lpPoint[p] = lpPoint[0];
		lpPoint[p].a = CP_LINE | CP_ENDS | CP_ENDG | CP_CLOSED;
		*pnum = p + 1;

		if ((mType == ShapeType::RibbonShadow) && (lpPoint != NULL))
			VFlopCPoint(*pnum, lpPoint, height / 2);
	}
	break;
	case ShapeType::EllipseRibbonShadow: // 314
	case ShapeType::EllipseRibbon2Shadow: // 315
	{
		int		p = 0;
		int		d;
		int		x1 = (int)((double)pAdjustValue[0] * 10000 / 21600 + 0.5);			 // 적게 구부러진 정도(중심점, 시작점 y거리)
		int		x2 = (int)((double)(pAdjustValue[0] + 2700) * 10000 / 21600 + 0.5); // 비교적 크게 구부러진 정도(중심점, 시작점 y거리)
		POINT	cutline[2];
		CPOINT* lpTmp = new CPOINT[15];
		int		nH, dH, a3;
		int		i = 0;

		if (mType == ShapeType::EllipseRibbonShadow) {
			a3 = (int)((double)10000 * (21600 - pAdjustValue[2]) / 21600 + 0.5);
			nH = (int)((double)(21600 - pAdjustValue[1]) * 10000 / 21600 + 0.5);
			dH = (int)((double)10000 * (pAdjustValue[1] + pAdjustValue[2] - 21600) / 21600 + 0.5);
		}
		else {
			a3 = (int)((double)10000 * pAdjustValue[2] / 21600 + 0.5);
			nH = (int)((double)pAdjustValue[1] * 10000 / 21600 + 0.5);
			dH = (int)((double)10000 * (21600 - pAdjustValue[1] - pAdjustValue[2]) / 21600 + 0.5);
		}


		lpPoint = new CPOINT[50];
		MakeRoundCorner(&(lpPoint[p]), 0, 10000, 5000, 10000 - a3, 10000, 10000, FALSE, TRUE);
		lpPoint[1].x = lpPoint[0].x;
		lpPoint[1].y = lpPoint[0].y;

		if (x2 < 5000) {
			cutline[0].x = x2;
			cutline[0].y = 0;
			cutline[1].x = x2;
			cutline[1].y = 10000;
			p = SplitBezierSegmentByLine(lpPoint, cutline, 0);
		}
		else {
			p = 4;
		}

		lpPoint[p - 1].a = CP_LINE;
		lpPoint[p].x = lpPoint[p - 1].x;
		lpPoint[p].y = nH;
		lpPoint[p].a = CP_LINE;

		CPOINT* tmpPoint = new CPOINT[p + 2];
		for (i = 0; i < p + 1; i++)
			tmpPoint[i] = lpPoint[i];

		HFlopCPoint(p + 1, tmpPoint, 5000);

		for (i = p; i >= 0; i--)
			lpPoint[++p] = tmpPoint[i];

		cutline[0].x = 1270;
		cutline[0].y = 0;
		cutline[1].x = 1270;
		cutline[1].y = 10000;
		MakeRoundCorner(lpTmp, 0, 10000, 5000, 10000 - a3, 10000, 10000, FALSE, TRUE);
		lpTmp[1].x = lpTmp[0].x;
		lpTmp[1].y = lpTmp[0].y;
		d = SplitBezierSegmentByLine(lpTmp, cutline, 0);
		d = 10000 - lpTmp[d - 1].y;

		p++;
		int flopPos = p;
		lpPoint[p].x = 10000 - 1270;
		lpPoint[p].y = 10000 - (nH / 2 + d);
		lpPoint[p++].a = CP_LINE;

		cutline[0].x = nH;
		cutline[0].y = 0;
		cutline[1].x = nH;
		cutline[1].y = 10000;
		MakeRoundCorner(&(lpPoint[p]), 10000, lpPoint[p - 2].y - nH, 5000, a3, 10000, 10000, FALSE, TRUE);
		lpPoint[p + 1].x = lpPoint[p].x;
		lpPoint[p + 1].y = lpPoint[p].y;
		d = SplitBezierSegmentByLine(&(lpPoint[p]), cutline, 0);

		p += d;

		MakeRoundCorner(lpTmp, 5000, 0, 10000, dH, 10000, 10000, TRUE, TRUE);
		lpTmp[1].x = lpTmp[0].x;
		lpTmp[1].y = lpTmp[0].y;
		cutline[0].x = lpPoint[p - 1].x;
		cutline[0].y = 0;
		cutline[1].x = lpPoint[p - 1].x;
		cutline[1].y = 10000;
		d = SplitBezierSegmentByLine(lpTmp, cutline, 0);

		lpPoint[p].a = CP_LINE;

		for (i = d - 1; i >= 0; i--)
			lpPoint[p++] = lpTmp[i];

		delete[] tmpPoint;
		int index = 0;
		int flopcnt = p - flopPos;
		tmpPoint = new CPOINT[flopcnt];

		for (i = p - 1; i >= flopPos; i--)
			tmpPoint[index++] = lpPoint[i];

		HFlopCPoint(flopcnt, tmpPoint, 5000);

		for (i = 0; i < flopcnt; i++)
			lpPoint[p++] = tmpPoint[i];

		lpPoint[p] = lpPoint[0];

		lpPoint[0].a = CP_STAT;
		lpPoint[p].a = CP_LINE | CP_ENDS | CP_ENDG | CP_CLOSED;
		*pnum = p + 1;
		*lppPts = lpPoint;
		convertPoints(lpPoint, lpPoint, p + 1, width, height);
		delete[] tmpPoint;

		if ((mType == ShapeType::EllipseRibbonShadow) && (lpPoint != NULL))
			VFlopCPoint(*pnum, lpPoint, height / 2);
		delete[] lpTmp;
	}
	break;
	case ShapeType::VerticalScrollShadow:
	case ShapeType::HorizontalScrollShadow: // 313
		if (pAdjustValue[0] == 0) {
			rect->left = 0;
			rect->top = 0;
			rect->right = width;
			rect->bottom = height;
			return BasicGraphElement::RectElement;
		}
		else {
			double dx = (double)pAdjustValue[0] / 5400;
			int i = 0, p = 0;

			lpPoint = new CPOINT[50];

			lpPoint[0].x = (int)(2600 * dx);
			lpPoint[0].y = 10000 - (int)(2500 * dx);
			lpPoint[0].a = CP_LINE | CP_STAT;

			MakeRoundCorner(&(lpPoint[1]), 1250 * dx, 10000 - 2500 * dx, 0, 10000 - 1150 * dx, 10000, 10000, TRUE, FALSE);
			MakeRoundCorner(&(lpPoint[4]), 0, 10000 - 1150 * dx, 1250 * dx, 10000, 10000, 10000, FALSE, TRUE);
			lpPoint[7].a = CP_LINE;
			MakeRoundCorner(&(lpPoint[8]), 10000 - 3950 * dx, 10000, 10000 - 2600 * dx, 10000 - 1150 * dx, 10000, 10000, TRUE, TRUE);
			lpPoint[11].a = CP_LINE;
			lpPoint[12].x = 10000 - 2600 * dx;
			lpPoint[12].y = 2500 * dx;
			lpPoint[12].a = CP_LINE;
			MakeRoundCorner(&(lpPoint[13]), 10000 - 1250 * dx, 2500 * dx, 10000, 1150 * dx, 10000, 10000, TRUE, FALSE);
			MakeRoundCorner(&(lpPoint[16]), 10000, 1150 * dx, 10000 - 1250 * dx, 0, 10000, 10000, FALSE, TRUE);
			lpPoint[19].a = CP_LINE;
			MakeRoundCorner(&(lpPoint[20]), 4050 * dx, 0, 2600 * dx, 1150 * dx, 10000, 10000, TRUE, TRUE);
			lpPoint[24] = lpPoint[0];
			lpPoint[24].a = CP_LINE | CP_ENDS | CP_ENDG | CP_CLOSED;
			*pnum = 25;
			*lppPts = lpPoint;

			if (mType == ShapeType::HorizontalScrollShadow) {
				if (lpPoint != NULL) {
					for (i = 0; i < *pnum; i++) {
						p = lpPoint[i].x;
						lpPoint[i].x = lpPoint[i].y;
						lpPoint[i].y = p;
					}
				}
				else {
					p = rect->top;
					rect->top = rect->left;
					rect->left = p;
					p = rect->right;
					rect->right = rect->bottom;
					rect->bottom = p;
				}
			}
			convertPoints(lpPoint, lpPoint, 25, width, height);
		}
		break;// 312
	case ShapeType::FoldedCornerShadow:
	{
		int halfHeight = (int)((double)(21600 - pAdjustValue[0]) * 5000 / 10800 + 0.5);
		if (halfHeight < 0)
			halfHeight = 0;
		if (halfHeight > 5000)
			halfHeight = 5000;

		lpPoint = new CPOINT[6];
		lpPoint[0].x = 0;
		lpPoint[0].y = 0;
		lpPoint[0].a = CP_LINE | CP_STAT;
		lpPoint[1].x = 0;
		lpPoint[1].y = 10000;
		lpPoint[1].a = CP_LINE;
		lpPoint[2].x = 10000 - halfHeight;
		lpPoint[2].y = 10000;
		lpPoint[2].a = CP_LINE;
		lpPoint[3].x = 10000;
		lpPoint[3].y = 10000 - halfHeight;
		lpPoint[3].a = CP_LINE;
		lpPoint[4].x = 10000;
		lpPoint[4].y = 0;
		lpPoint[4].a = CP_LINE;
		lpPoint[5] = lpPoint[0];
		lpPoint[5].a = CP_LINE | CP_ENDS | CP_ENDG | CP_CLOSED;

		convertPoints(lpPoint, lpPoint, 6, width, height);
		*pnum = 6;
		*lppPts = lpPoint;
	}
	break;
	case ShapeType::StripedRightArrowShadow:
	{
		int		cx, cy, p;
		lpPoint = new CPOINT[18];

		cy = (int)((double)5000 * pAdjustValue[1] / 10800 + 0.5);
		if (cy > 5000)
			cy = 5000;
		cx = 1540 + (int)((double)8460 * (pAdjustValue[0] - 3375) / (21600 - 3375) + 0.5);
		if (cx > 10000)
			cx = 10000;
		if (cx < 1540)
			cx = 1540;

		*pnum = p = 0;

		if (cy == 5000) {
			lpPoint[0].x = 0;
			lpPoint[0].y = (int)((double)height / 2 + 0.5);
			lpPoint[0].a = CP_STAT | CP_LINE;
			lpPoint[1].x = (int)((double)300 * width / 10000 + 0.5);
			lpPoint[1].y = (int)((double)height / 2 + 0.5);
			lpPoint[1].a = CP_LINE | CP_ENDS;

			lpPoint[2].x = (int)((double)600 * width / 10000 + 0.5);
			lpPoint[2].y = (int)((double)height / 2 + 0.5);
			lpPoint[2].a = CP_STAT | CP_LINE;
			lpPoint[3].x = (int)((double)1230 * width / 10000 + 0.5);
			lpPoint[3].y = (int)((double)height / 2 + 0.5);
			lpPoint[3].a = CP_LINE | CP_ENDS;

			p = 4;
		}
		else {
			lpPoint[0].x = 0;
			lpPoint[0].y = (int)((double)cy * height / 10000 + 0.5);
			lpPoint[0].a = CP_STAT | CP_LINE;
			lpPoint[1].x = (int)((double)300 * width / 10000 + 0.5);
			lpPoint[1].y = (int)((double)cy * height / 10000 + 0.5);
			lpPoint[1].a = CP_LINE;
			lpPoint[2].x = (int)((double)300 * width / 10000 + 0.5);
			lpPoint[2].y = (int)((double)(10000 - cy) * height / 10000 + 0.5);
			lpPoint[2].a = CP_LINE;
			lpPoint[3].x = 0;
			lpPoint[3].y = (int)((double)(10000 - cy) * height / 10000 + 0.5);
			lpPoint[3].a = CP_LINE;
			lpPoint[4].x = 0;
			lpPoint[4].y = (int)((double)cy * height / 10000 + 0.5);
			lpPoint[4].a = CP_ENDS | CP_CLOSED;

			lpPoint[5].x = (int)((double)600 * width / 10000 + 0.5);
			lpPoint[5].y = (int)((double)cy * height / 10000 + 0.5);
			lpPoint[5].a = CP_STAT | CP_LINE;
			lpPoint[6].x = (int)((double)1230 * width / 10000 + 0.5);
			lpPoint[6].y = (int)((double)cy * height / 10000 + 0.5);
			lpPoint[6].a = CP_LINE;
			lpPoint[7].x = (int)((double)1230 * width / 10000 + 0.5);
			lpPoint[7].y = (int)((double)(10000 - cy) * height / 10000 + 0.5);
			lpPoint[7].a = CP_LINE;
			lpPoint[8].x = (int)((double)600 * width / 10000 + 0.5);
			lpPoint[8].y = (int)((double)(10000 - cy) * height / 10000 + 0.5);
			lpPoint[8].a = CP_LINE;
			lpPoint[9].x = (int)((double)600 * width / 10000 + 0.5);
			lpPoint[9].y = (int)((double)cy * height / 10000 + 0.5);
			lpPoint[9].a = CP_ENDS | CP_CLOSED;

			p = 10;
		}

		lpPoint[p].x = (int)((double)1540 * width / 10000 + 0.5);
		lpPoint[p].y = (int)((double)cy * height / 10000 + 0.5);
		lpPoint[p].a = CP_STAT | CP_LINE;
		p++;
		lpPoint[p].x = (int)((double)cx * width / 10000 + 0.5);
		lpPoint[p].y = (int)((double)cy * height / 10000 + 0.5);
		lpPoint[p].a = CP_LINE;
		p++;
		lpPoint[p].x = (int)((double)cx * width / 10000 + 0.5);
		lpPoint[p].y = 0;
		lpPoint[p].a = CP_LINE;
		p++;
		lpPoint[p].x = width;
		lpPoint[p].y = (int)((double)5000 * height / 10000 + 0.5);
		lpPoint[p].a = CP_LINE;
		p++;
		lpPoint[p].x = (int)((double)cx * width / 10000 + 0.5);
		lpPoint[p].y = height;
		lpPoint[p].a = CP_LINE;
		p++;
		lpPoint[p].x = (int)((double)cx * width / 10000 + 0.5);
		lpPoint[p].y = (int)((double)(10000 - cy) * height / 10000 + 0.5);
		lpPoint[p].a = CP_LINE;
		p++;
		lpPoint[p].x = (int)((double)1540 * width / 10000 + 0.5);
		lpPoint[p].y = (int)((double)(10000 - cy) * height / 10000 + 0.5);
		lpPoint[p].a = CP_LINE;
		p++;
		lpPoint[p].x = (int)((double)1540 * width / 10000 + 0.5);
		lpPoint[p].y = (int)((double)cy * height / 10000 + 0.5);
		lpPoint[p].a = CP_ENDS | CP_ENDG | CP_CLOSED;
		p++;
		*pnum = p;
		*lppPts = lpPoint;
	}
	break;
	case ShapeType::Parallelogram:				// = 7,
	case ShapeType::FlowChartInputOutput:		// = 111,
		lpPoint = new CPOINT[5];
		if (lpPoint == NULL)
			break;
		convertPoints(lpPoint, _rectangle, 4, width, height);
		lpPoint[0].x = (LONG)((double)width * pAdjustValue[0] / MAX_ADJUSTVALUE);
		lpPoint[2].x = width - lpPoint[0].x;
		lpPoint[4] = lpPoint[0];
		*pnum = 5;
		*lppPts = lpPoint;
		break;
	case ShapeType::Diamond:						// = 4,
	case ShapeType::FlowChartDecision:			// = 110,
		lpPoint = new CPOINT[5];
		if (lpPoint == NULL)
			break;
		convertPoints(lpPoint, _diamond, 4, width, height);
		lpPoint[4] = lpPoint[0];
		*pnum = 5;
		break;
	case ShapeType::Octagon:						// = 10,
		lpPoint = new CPOINT[9];
		if (lpPoint == NULL)
			break;
		convertPoints(lpPoint, _octagon, 8, width, height);
		lpPoint[5].x = lpPoint[0].x = (LONG)((double)width * pAdjustValue[0] / MAX_ADJUSTVALUE);
		lpPoint[4].x = lpPoint[1].x = width - lpPoint[0].x;
		lpPoint[7].y = lpPoint[2].y = (LONG)((double)height * pAdjustValue[0] / MAX_ADJUSTVALUE);
		lpPoint[6].y = lpPoint[3].y = height - lpPoint[2].y;
		lpPoint[8] = lpPoint[0];
		*pnum = 9;
		break;
	case ShapeType::IsocelesTriangle:			// = 5,
	case ShapeType::FlowChartExtract:			// = 127,
		lpPoint = new CPOINT[4];
		if (lpPoint == NULL)
			break;
		convertPoints(lpPoint, _triangle, 3, width, height);
		lpPoint[0].x = (LONG)((double)width * pAdjustValue[0] / MAX_ADJUSTVALUE);
		lpPoint[3] = lpPoint[0];
		*pnum = 4;
		break;
	case ShapeType::RightTriangle:				// = 6,
		lpPoint = new CPOINT[4];
		if (lpPoint == NULL)
			break;
		convertPoints(lpPoint, _triangle, 3, width, height);
		lpPoint[3] = lpPoint[0];
		*pnum = 4;
		break;
	case ShapeType::Hexagon:						// = 9,
	case ShapeType::FlowChartPreparation:		// = 117,
		lpPoint = new CPOINT[7];
		if (lpPoint == NULL)
			break;
		convertPoints(lpPoint, _hexagon, 6, width, height);
		lpPoint[4].x = lpPoint[0].x = (LONG)((double)width * pAdjustValue[0] / MAX_ADJUSTVALUE);
		lpPoint[3].x = lpPoint[1].x = width - lpPoint[0].x;
		lpPoint[6] = lpPoint[0];
		*pnum = 7;
		break;
	case ShapeType::Plus:						// = 11,
		lpPoint = new CPOINT[13];
		if (lpPoint == NULL)
			break;
		convertPoints(lpPoint, _pluspoly, 12, width, height);
		lpPoint[0].x = lpPoint[11].x = lpPoint[8].x = lpPoint[7].x = (LONG)((double)width * pAdjustValue[0] / MAX_ADJUSTVALUE);
		lpPoint[1].x = lpPoint[2].x = lpPoint[5].x = lpPoint[6].x = width - lpPoint[0].x;
		lpPoint[3].y = lpPoint[2].y = lpPoint[11].y = lpPoint[10].y = (LONG)((double)height * pAdjustValue[0] / MAX_ADJUSTVALUE);
		lpPoint[4].y = lpPoint[5].y = lpPoint[8].y = lpPoint[9].y = height - lpPoint[3].y;
		lpPoint[12] = lpPoint[0];
		*pnum = 13;
		break;
	case ShapeType::Pentagon:					// = 56,
		lpPoint = new CPOINT[6];
		if (lpPoint == NULL)
			break;
		convertPoints(lpPoint, _pentagon, 5, width, height);
		lpPoint[5] = lpPoint[0];
		*pnum = 6;
		break;
	case ShapeType::FlowChartPredefinedProcess:	// = 112,
		lpPoint = new CPOINT[11];
		if (lpPoint == NULL)
			break;
		convertPoints(lpPoint, _FlowChartPredefinedProcess, 10, width, height);
		lpPoint[3].x = lpPoint[2].x = lpPoint[4].x = (LONG)((double)width * pAdjustValue[0] / MAX_ADJUSTVALUE);
		lpPoint[6].x = lpPoint[5].x = lpPoint[7].x = width - lpPoint[3].x;
		lpPoint[10] = lpPoint[0];
		*pnum = 11;
		break;
	case ShapeType::FlowChartInternalStorage:	// = 113,
		lpPoint = new CPOINT[11];
		if (lpPoint == NULL)
			break;
		convertPoints(lpPoint, _FlowChartInternalStorage, 10, width, height);
		lpPoint[1].y = lpPoint[3].y = lpPoint[2].y = (LONG)((double)height * pAdjustValue[0] / MAX_ADJUSTVALUE);
		lpPoint[6].x = lpPoint[5].x = lpPoint[7].x = (LONG)((double)width * pAdjustValue[0] / MAX_ADJUSTVALUE);
		lpPoint[10] = lpPoint[0];
		*pnum = 11;
		break;
	case ShapeType::FlowChartManualInput:		// = 118,
		lpPoint = new CPOINT[5];
		if (lpPoint == NULL)
			break;
		convertPoints(lpPoint, _rectangle, 4, width, height);
		lpPoint[0].y = (LONG)((double)height * pAdjustValue[0] / MAX_ADJUSTVALUE);
		lpPoint[4] = lpPoint[0];
		*pnum = 5;
		break;
	case ShapeType::Trapezoid:					// = 8,
	case ShapeType::FlowChartManualOperation:	// = 119,
		lpPoint = new CPOINT[5];
		if (lpPoint == NULL)
			break;
		convertPoints(lpPoint, _rectangle, 4, width, height);
		lpPoint[3].x = (LONG)((double)width * pAdjustValue[0] / MAX_ADJUSTVALUE);
		lpPoint[2].x = width - lpPoint[3].x;
		lpPoint[4] = lpPoint[0];
		*pnum = 5;
		break;
	case ShapeType::FlowChartOffpageConnector:	// = 177,
		lpPoint = new CPOINT[6];
		if (lpPoint == NULL)
			break;
		convertPoints(lpPoint, _FlowChartOffpageConnector, 5, width, height);
		lpPoint[1].y = lpPoint[3].y = (LONG)((double)height * pAdjustValue[0] / MAX_ADJUSTVALUE);
		lpPoint[5] = lpPoint[0];
		*pnum = 6;
		break;
	case ShapeType::FlowChartPunchedCard:		// = 121,
		lpPoint = new CPOINT[6];
		if (lpPoint == NULL)
			break;
		convertPoints(lpPoint, _FlowChartPunchedCard, 5, width, height);
		lpPoint[0].y = (LONG)((double)height * pAdjustValue[0] / MAX_ADJUSTVALUE);
		lpPoint[4].x = (LONG)((double)width * pAdjustValue[0] / MAX_ADJUSTVALUE);
		lpPoint[5] = lpPoint[0];
		*pnum = 6;
		break;
	case ShapeType::FlowChartCollate:			// = 125,
		lpPoint = new CPOINT[5];
		if (lpPoint == NULL)
			break;
		convertPoints(lpPoint, _FlowChartCollate, 4, width, height);
		lpPoint[4] = lpPoint[0];
		*pnum = 5;
		break;
	case ShapeType::FlowChartSort:				// = 126,
		lpPoint = new CPOINT[6];
		if (lpPoint == NULL)
			break;
		convertPoints(lpPoint, _FlowChartSort, 5, width, height);
		lpPoint[5] = lpPoint[1];
		*pnum = 6;
		break;
	case ShapeType::FlowChartMerge:				// = 128,
		lpPoint = new CPOINT[4];
		if (lpPoint == NULL)
			break;
		convertPoints(lpPoint, _FlowChartMerge, 3, width, height);
		lpPoint[3] = lpPoint[0];
		*pnum = 4;
		break;
	case ShapeType::Arrow:						// = 13,
		lpPoint = new CPOINT[8];
		if (lpPoint == NULL)
			break;
		convertPoints(lpPoint, _arrow, 7, width, height);
		lpPoint[2].x = lpPoint[1].x = lpPoint[5].x = lpPoint[4].x = (LONG)((double)width * pAdjustValue[0] / MAX_ADJUSTVALUE);
		lpPoint[1].y = lpPoint[0].y = (LONG)((double)height * pAdjustValue[1] / MAX_ADJUSTVALUE);
		lpPoint[5].y = lpPoint[6].y = height - lpPoint[1].y;
		lpPoint[7] = lpPoint[0];
		*pnum = 8;
		break;
	case ShapeType::LeftArrow:					// = 66,
		lpPoint = new CPOINT[8];
		if (lpPoint == NULL)
			break;
		convertPoints(lpPoint, _leftarrow, 7, width, height);
		lpPoint[1].x = lpPoint[2].x = lpPoint[5].x = lpPoint[6].x = (LONG)((double)width * pAdjustValue[0] / MAX_ADJUSTVALUE);
		lpPoint[2].y = lpPoint[3].y = (LONG)((double)height * pAdjustValue[1] / MAX_ADJUSTVALUE);
		lpPoint[5].y = lpPoint[4].y = height - lpPoint[2].y;
		lpPoint[7] = lpPoint[0];
		*pnum = 8;
		break;
	case ShapeType::UpArrow:						// = 68,
		lpPoint = new CPOINT[8];
		if (lpPoint == NULL)
			break;
		convertPoints(lpPoint, _uparrow, 7, width, height);
		lpPoint[1].y = lpPoint[2].y = lpPoint[5].y = lpPoint[6].y = (LONG)((double)height * pAdjustValue[0] / MAX_ADJUSTVALUE);
		lpPoint[5].x = lpPoint[4].x = (LONG)((double)width * pAdjustValue[1] / MAX_ADJUSTVALUE);
		lpPoint[2].x = lpPoint[3].x = width - lpPoint[5].x;
		lpPoint[7] = lpPoint[0];
		*pnum = 8;
		break;
	case ShapeType::DownArrow:					// = 67,
		lpPoint = new CPOINT[8];
		if (lpPoint == NULL)
			break;
		convertPoints(lpPoint, _downarrow, 7, width, height);
		lpPoint[1].y = lpPoint[2].y = lpPoint[5].y = lpPoint[6].y = (LONG)((double)height * pAdjustValue[0] / MAX_ADJUSTVALUE);
		lpPoint[2].x = lpPoint[3].x = (LONG)((double)width * pAdjustValue[1] / MAX_ADJUSTVALUE);
		lpPoint[4].x = lpPoint[5].x = width - lpPoint[2].x;
		lpPoint[7] = lpPoint[0];
		*pnum = 8;
		break;
	case ShapeType::LeftRightArrow:				// = 69,
		lpPoint = new CPOINT[11];
		if (lpPoint == NULL)
			break;
		convertPoints(lpPoint, _leftrightarrow, 10, width, height);
		lpPoint[1].x = lpPoint[2].x = lpPoint[8].x = lpPoint[9].x = (LONG)((double)width * pAdjustValue[0] / MAX_ADJUSTVALUE);
		lpPoint[3].x = lpPoint[4].x = lpPoint[7].x = lpPoint[6].x = width - lpPoint[1].x;
		lpPoint[2].y = lpPoint[3].y = (LONG)((double)height * pAdjustValue[1] / MAX_ADJUSTVALUE);
		lpPoint[8].y = lpPoint[7].y = height - lpPoint[2].y;
		lpPoint[10] = lpPoint[0];
		*pnum = 11;
		break;
	case ShapeType::UpDownArrow:					// = 70,
		lpPoint = new CPOINT[11];
		if (lpPoint == NULL)
			break;
		convertPoints(lpPoint, _updownarrow, 10, width, height);
		lpPoint[1].y = lpPoint[2].y = lpPoint[8].y = lpPoint[9].y = (LONG)((double)height * pAdjustValue[1] / MAX_ADJUSTVALUE);
		lpPoint[3].y = lpPoint[4].y = lpPoint[7].y = lpPoint[6].y = height - lpPoint[1].y;
		lpPoint[8].x = lpPoint[7].x = (LONG)((double)width * pAdjustValue[0] / MAX_ADJUSTVALUE);
		lpPoint[2].x = lpPoint[3].x = width - lpPoint[8].x;
		lpPoint[10] = lpPoint[0];
		*pnum = 11;
		break;
	case ShapeType::QuadArrow:					// = 76,
		lpPoint = new CPOINT[25];
		if (lpPoint == NULL)
			break;
		convertPoints(lpPoint, _quadarrow, 24, width, height);
		lpPoint[23].x = lpPoint[13].x = (LONG)((double)width * pAdjustValue[0] / MAX_ADJUSTVALUE);
		lpPoint[1].x = lpPoint[11].x = width - lpPoint[23].x;
		lpPoint[5].y = lpPoint[19].y = (LONG)((double)height * pAdjustValue[0] / MAX_ADJUSTVALUE);
		lpPoint[7].y = lpPoint[17].y = height - lpPoint[5].y;
		lpPoint[22].x = lpPoint[21].x = lpPoint[15].x = lpPoint[14].x = (LONG)((double)width * pAdjustValue[1] / MAX_ADJUSTVALUE);
		lpPoint[2].x = lpPoint[3].x = lpPoint[9].x = lpPoint[10].x = width - lpPoint[22].x;
		lpPoint[4].y = lpPoint[3].y = lpPoint[21].y = lpPoint[20].y = (LONG)((double)height * pAdjustValue[1] / MAX_ADJUSTVALUE);
		lpPoint[8].y = lpPoint[9].y = lpPoint[15].y = lpPoint[16].y = height - lpPoint[4].y;
		lpPoint[23].y = lpPoint[22].y = lpPoint[2].y = lpPoint[1].y = (LONG)((double)height * pAdjustValue[2] / MAX_ADJUSTVALUE);
		lpPoint[13].y = lpPoint[14].y = lpPoint[10].y = lpPoint[11].y = height - lpPoint[23].y;
		lpPoint[19].x = lpPoint[20].x = lpPoint[16].x = lpPoint[17].x = (LONG)((double)width * pAdjustValue[2] / MAX_ADJUSTVALUE);
		lpPoint[5].x = lpPoint[4].x = lpPoint[8].x = lpPoint[7].x = width - lpPoint[19].x;
		lpPoint[24] = lpPoint[0];
		*pnum = 25;
		break;
	case ShapeType::LeftRightUpArrow:			// = 182,
	{
		LONG	y_value;
		lpPoint = new CPOINT[18];
		if (lpPoint == NULL)
			break;
		convertPoints(lpPoint, _leftrightuparrow, 17, width, height);
		lpPoint[16].x = (LONG)((double)width * pAdjustValue[0] / MAX_ADJUSTVALUE);
		lpPoint[1].x = width - lpPoint[16].x;
		y_value = (LONG)((double)height * (MAX_ADJUSTVALUE / 2 - pAdjustValue[0]) * 1.4 / MAX_ADJUSTVALUE);
		lpPoint[6].y = lpPoint[11].y = height - y_value;
		lpPoint[5].y = lpPoint[12].y = height - (y_value + y_value);
		lpPoint[15].x = lpPoint[14].x = (LONG)((double)width * pAdjustValue[1] / MAX_ADJUSTVALUE);
		lpPoint[2].x = lpPoint[3].x = width - lpPoint[15].x;
		y_value = (LONG)((double)height * (MAX_ADJUSTVALUE / 2 - pAdjustValue[1]) / MAX_ADJUSTVALUE);
		lpPoint[4].y = lpPoint[3].y = lpPoint[14].y = lpPoint[13].y = lpPoint[6].y - y_value;
		lpPoint[8].y = lpPoint[9].y = lpPoint[6].y + y_value;
		lpPoint[16].y = lpPoint[15].y = lpPoint[2].y = lpPoint[1].y = (LONG)((double)height * pAdjustValue[2] / MAX_ADJUSTVALUE);
		lpPoint[12].x = lpPoint[13].x = lpPoint[9].x = lpPoint[10].x = (LONG)((double)width * pAdjustValue[2] * 0.75 / MAX_ADJUSTVALUE);
		lpPoint[5].x = lpPoint[4].x = lpPoint[8].x = lpPoint[7].x = width - lpPoint[12].x;
		lpPoint[17] = lpPoint[0];
		*pnum = 18;
		break;
	}
	case ShapeType::LeftUpArrow:					// = 89,
	{
		LONG	_value;
		lpPoint = new CPOINT[13];
		if (lpPoint == NULL)
			break;
		convertPoints(lpPoint, _leftuparrow, 12, width, height);
		_value = (LONG)(double)(((double)width * (MAX_ADJUSTVALUE - pAdjustValue[0]) / 2) / MAX_ADJUSTVALUE);
		lpPoint[0].x = width - _value;
		lpPoint[11].x = lpPoint[0].x - _value;
		_value = (LONG)(double)(((double)height * (MAX_ADJUSTVALUE - pAdjustValue[0]) / 2) / MAX_ADJUSTVALUE);
		lpPoint[6].y = height - _value;
		lpPoint[7].y = lpPoint[6].y - _value;
		lpPoint[2].x = lpPoint[3].x = (LONG)((double)width * pAdjustValue[1] / MAX_ADJUSTVALUE);
		lpPoint[10].x = lpPoint[9].x = lpPoint[0].x - (lpPoint[2].x - lpPoint[0].x);
		lpPoint[4].y = lpPoint[3].y = (LONG)((double)height * pAdjustValue[1] / MAX_ADJUSTVALUE);
		lpPoint[8].y = lpPoint[9].y = lpPoint[6].y - (lpPoint[4].y - lpPoint[6].y);
		lpPoint[1].y = lpPoint[2].y = lpPoint[10].y = lpPoint[11].y = (LONG)((double)height * pAdjustValue[2] / MAX_ADJUSTVALUE);
		lpPoint[7].x = lpPoint[8].x = lpPoint[4].x = lpPoint[5].x = (LONG)((double)width * pAdjustValue[2] / MAX_ADJUSTVALUE);
		lpPoint[12] = lpPoint[0];
		*pnum = 13;
		break;
	}
	case ShapeType::BentUpArrow:					// = 90,
	{
		LONG	_value;
		lpPoint = new CPOINT[10];
		if (lpPoint == NULL)
			break;
		convertPoints(lpPoint, _bentuparrow, 9, width, height);
		_value = (LONG)(double)(((double)width * (MAX_ADJUSTVALUE - pAdjustValue[0]) / 2) / MAX_ADJUSTVALUE);
		lpPoint[0].x = width - _value;
		lpPoint[8].x = lpPoint[0].x - _value;
		lpPoint[2].x = lpPoint[3].x = (LONG)(((double)width * pAdjustValue[1]) / MAX_ADJUSTVALUE);
		lpPoint[7].x = lpPoint[6].x = lpPoint[0].x - (lpPoint[2].x - lpPoint[0].x);
		_value = pAdjustValue[0] + (MAX_ADJUSTVALUE - pAdjustValue[0]) / 2;

		LONG _value2 = (LONG)((double)height * (MAX_ADJUSTVALUE - pAdjustValue[0]) / MAX_ADJUSTVALUE);
		lpPoint[5].y = lpPoint[6].y = height - (LONG)((double)_value2 * (pAdjustValue[1] - _value) / (MAX_ADJUSTVALUE - _value));

		lpPoint[1].y = lpPoint[2].y = lpPoint[7].y = lpPoint[8].y = (LONG)((double)height * pAdjustValue[2] / MAX_ADJUSTVALUE);
		lpPoint[9] = lpPoint[0];
		*pnum = 10;
		break;
	}
	case ShapeType::NotchedRightArrow:			// = 94,
	{
		LONG	_value;
		lpPoint = new CPOINT[9];
		if (lpPoint == NULL)
			break;
		convertPoints(lpPoint, _notchedrightarrow, 8, width, height);
		lpPoint[2].x = lpPoint[1].x = lpPoint[5].x = lpPoint[4].x = (LONG)((double)width * pAdjustValue[0] / MAX_ADJUSTVALUE);
		lpPoint[1].y = lpPoint[0].y = (LONG)((double)height * pAdjustValue[1] / MAX_ADJUSTVALUE);
		lpPoint[5].y = lpPoint[6].y = height - lpPoint[1].y;
		if (pAdjustValue[0] == 0)
		{
			_value = pAdjustValue[1] * 2;
		}
		else if (pAdjustValue[1] == 0)
		{
			_value = pAdjustValue[0];
		}
		else if (pAdjustValue[0] == MAX_ADJUSTVALUE)
		{
			_value = MAX_ADJUSTVALUE;
		}
		else if (pAdjustValue[1] == (MAX_ADJUSTVALUE / 2))
		{
			_value = MAX_ADJUSTVALUE;
		}
		else
		{
			_value = (LONG)(double)(pAdjustValue[0] * 0.65 + pAdjustValue[1] * 1.4);
		}
		_value = (_value > MAX_ADJUSTVALUE) ? MAX_ADJUSTVALUE : _value;
		_value = (_value < 0) ? 0 : _value;
		lpPoint[7].x = width - (LONG)((double)width * _value / MAX_ADJUSTVALUE);
		lpPoint[8] = lpPoint[0];
		*pnum = 9;
		break;
	}
	case ShapeType::HomePlate:					// = 15,
		lpPoint = new CPOINT[6];
		if (lpPoint == NULL)
			break;

		convertPoints(lpPoint, _chevron, 5, width, height);
		lpPoint[1].x = lpPoint[3].x = (int)((double)width * pAdjustValue[0] / MAX_ADJUSTVALUE);
		if (lpPoint[1].x < 0)
			int a1 = 0;
		lpPoint[5] = lpPoint[0];
		*pnum = 6;
		break;
	case ShapeType::Chevron:						// = 55,
		lpPoint = new CPOINT[7];
		if (lpPoint == NULL)
			break;
		convertPoints(lpPoint, _chevron, 6, width, height);
		lpPoint[1].x = lpPoint[3].x = (LONG)((double)width * pAdjustValue[0] / MAX_ADJUSTVALUE);
		lpPoint[5].x = width - lpPoint[1].x;
		lpPoint[6] = lpPoint[0];
		*pnum = 7;
		break;
	case ShapeType::RightArrowCallout:			// = 78,
		lpPoint = new CPOINT[12];
		if (lpPoint == NULL)
			break;
		convertPoints(lpPoint, _RightArrowCallout, 11, width, height);
		lpPoint[1].x = lpPoint[2].x = lpPoint[8].x = lpPoint[9].x = (LONG)((double)width * pAdjustValue[0] / MAX_ADJUSTVALUE);
		lpPoint[4].y = (LONG)((double)height * pAdjustValue[1] / MAX_ADJUSTVALUE);
		lpPoint[6].y = height - lpPoint[4].y;
		lpPoint[3].x = lpPoint[7].x = lpPoint[4].x = lpPoint[6].x = (LONG)((double)width * pAdjustValue[2] / MAX_ADJUSTVALUE);
		lpPoint[2].y = lpPoint[3].y = (LONG)((double)height * pAdjustValue[3] / MAX_ADJUSTVALUE);
		lpPoint[8].y = lpPoint[7].y = height - lpPoint[2].y;
		lpPoint[11] = lpPoint[0];
		*pnum = 12;
		break;
	case ShapeType::LeftArrowCallout:			// = 77,
		lpPoint = new CPOINT[12];
		if (lpPoint == NULL)
			break;
		convertPoints(lpPoint, _LeftArrowCallout, 11, width, height);
		lpPoint[1].x = lpPoint[2].x = lpPoint[8].x = lpPoint[9].x = (LONG)((double)width * pAdjustValue[0] / MAX_ADJUSTVALUE);
		lpPoint[4].y = (LONG)(((double)height * pAdjustValue[1]) / MAX_ADJUSTVALUE);
		lpPoint[6].y = height - lpPoint[4].y;
		lpPoint[3].x = lpPoint[7].x = lpPoint[4].x = lpPoint[6].x = (LONG)((double)width * pAdjustValue[2] / MAX_ADJUSTVALUE);
		lpPoint[2].y = lpPoint[3].y = (LONG)((double)height * pAdjustValue[3] / MAX_ADJUSTVALUE);
		lpPoint[8].y = lpPoint[7].y = height - lpPoint[2].y;
		lpPoint[11] = lpPoint[0];
		*pnum = 12;
		break;
	case ShapeType::UpArrowCallout:				// = 79,
		lpPoint = new CPOINT[12];
		if (lpPoint == NULL)
			break;
		convertPoints(lpPoint, _UpArrowCallout, 11, width, height);
		lpPoint[1].y = lpPoint[2].y = lpPoint[8].y = lpPoint[9].y = (LONG)((double)height * pAdjustValue[0] / MAX_ADJUSTVALUE);
		lpPoint[6].x = (LONG)((double)width * pAdjustValue[1] / MAX_ADJUSTVALUE);
		lpPoint[4].x = width - lpPoint[6].x;
		lpPoint[6].y = lpPoint[7].y = lpPoint[3].y = lpPoint[4].y = (LONG)((double)height * pAdjustValue[2] / MAX_ADJUSTVALUE);
		lpPoint[7].x = lpPoint[8].x = (LONG)((double)width * pAdjustValue[3] / MAX_ADJUSTVALUE);
		lpPoint[3].x = lpPoint[2].x = width - lpPoint[7].x;
		lpPoint[11] = lpPoint[0];
		*pnum = 12;
		break;
	case ShapeType::DownArrowCallout:			// = 80,
		lpPoint = new CPOINT[12];
		if (lpPoint == NULL)
			break;
		convertPoints(lpPoint, _DownArrowCallout, 11, width, height);
		lpPoint[1].y = lpPoint[2].y = lpPoint[8].y = lpPoint[9].y = (LONG)((double)height * pAdjustValue[0] / MAX_ADJUSTVALUE);
		lpPoint[6].x = (LONG)(((double)width * pAdjustValue[1]) / MAX_ADJUSTVALUE);
		lpPoint[4].x = width - lpPoint[6].x;
		lpPoint[6].y = lpPoint[7].y = lpPoint[3].y = lpPoint[4].y = (LONG)((double)height * pAdjustValue[2] / MAX_ADJUSTVALUE);
		lpPoint[7].x = lpPoint[8].x = (LONG)((double)width * pAdjustValue[3] / MAX_ADJUSTVALUE);
		lpPoint[3].x = lpPoint[2].x = width - lpPoint[7].x;
		lpPoint[11] = lpPoint[0];
		*pnum = 12;
		break;
	case ShapeType::LeftRightArrowCallout:		// = 81,
		lpPoint = new CPOINT[19];
		if (lpPoint == NULL)
			break;
		convertPoints(lpPoint, _LeftRightArrowCallout, 18, width, height);
		lpPoint[1].x = lpPoint[2].x = lpPoint[8].x = lpPoint[9].x = (LONG)((double)width * pAdjustValue[0] / MAX_ADJUSTVALUE);
		lpPoint[0].x = lpPoint[17].x = lpPoint[11].x = lpPoint[10].x = width - lpPoint[1].x;
		lpPoint[4].y = lpPoint[15].y = (LONG)((double)height * pAdjustValue[1] / MAX_ADJUSTVALUE);
		lpPoint[6].y = lpPoint[13].y = height - lpPoint[4].y;
		lpPoint[3].x = lpPoint[7].x = lpPoint[4].x = lpPoint[6].x = (LONG)((double)width * pAdjustValue[2] / MAX_ADJUSTVALUE);
		lpPoint[15].x = lpPoint[16].x = lpPoint[12].x = lpPoint[13].x = width - lpPoint[3].x;
		lpPoint[2].y = lpPoint[3].y = lpPoint[17].y = lpPoint[16].y = (LONG)((double)height * pAdjustValue[3] / MAX_ADJUSTVALUE);
		lpPoint[8].y = lpPoint[7].y = lpPoint[11].y = lpPoint[12].y = height - lpPoint[2].y;
		lpPoint[18] = lpPoint[0];
		*pnum = 19;
		break;
	case ShapeType::UpDownArrowCallout:			// = 82,
		lpPoint = new CPOINT[19];
		if (lpPoint == NULL)
			break;
		convertPoints(lpPoint, _UpDownArrowCallout, 18, width, height);
		lpPoint[1].y = lpPoint[2].y = lpPoint[8].y = lpPoint[9].y = (LONG)((double)height * pAdjustValue[0] / MAX_ADJUSTVALUE);
		lpPoint[10].y = lpPoint[11].y = lpPoint[17].y = lpPoint[0].y = height - lpPoint[1].y;
		lpPoint[6].x = lpPoint[13].x = (LONG)((double)width * pAdjustValue[1] / MAX_ADJUSTVALUE);
		lpPoint[4].x = lpPoint[15].x = width - lpPoint[6].x;
		lpPoint[6].y = lpPoint[7].y = lpPoint[3].y = lpPoint[4].y = (LONG)((double)height * pAdjustValue[2] / MAX_ADJUSTVALUE);
		lpPoint[13].y = lpPoint[12].y = lpPoint[16].y = lpPoint[15].y = height - lpPoint[6].y;
		lpPoint[7].x = lpPoint[8].x = lpPoint[11].x = lpPoint[12].x = (LONG)((double)width * pAdjustValue[3] / MAX_ADJUSTVALUE);
		lpPoint[3].x = lpPoint[2].x = lpPoint[17].x = lpPoint[16].x = width - lpPoint[7].x;
		lpPoint[18] = lpPoint[0];
		*pnum = 19;
		break;
	case ShapeType::QuadArrowCallout:			// = 83,
		lpPoint = new CPOINT[33];
		if (lpPoint == NULL)
			break;
		convertPoints(lpPoint, _QuadArrowCallout, 32, width, height);
		lpPoint[28].y = lpPoint[29].y = lpPoint[3].y = lpPoint[4].y = (LONG)((double)height * pAdjustValue[0] / MAX_ADJUSTVALUE);
		lpPoint[20].y = lpPoint[19].y = lpPoint[13].y = lpPoint[12].y = height - lpPoint[28].y;
		lpPoint[28].x = lpPoint[27].x = lpPoint[21].x = lpPoint[20].x = (LONG)((double)width * pAdjustValue[0] / MAX_ADJUSTVALUE);
		lpPoint[4].x = lpPoint[5].x = lpPoint[11].x = lpPoint[12].x = width - lpPoint[28].x;
		lpPoint[31].x = lpPoint[17].x = (LONG)((double)width * pAdjustValue[1] / MAX_ADJUSTVALUE);
		lpPoint[1].x = lpPoint[15].x = width - lpPoint[31].x;
		lpPoint[25].y = lpPoint[7].y = (LONG)((double)height * pAdjustValue[1] / MAX_ADJUSTVALUE);
		lpPoint[23].y = lpPoint[9].y = height - lpPoint[25].y;
		lpPoint[31].y = lpPoint[30].y = lpPoint[2].y = lpPoint[1].y = (LONG)((double)height * pAdjustValue[2] / MAX_ADJUSTVALUE);
		lpPoint[17].y = lpPoint[18].y = lpPoint[14].y = lpPoint[15].y = height - lpPoint[31].y;
		lpPoint[25].x = lpPoint[26].x = lpPoint[22].x = lpPoint[23].x = (LONG)((double)width * pAdjustValue[2] / MAX_ADJUSTVALUE);
		lpPoint[7].x = lpPoint[6].x = lpPoint[10].x = lpPoint[9].x = width - lpPoint[25].x;
		lpPoint[30].x = lpPoint[29].x = lpPoint[19].x = lpPoint[18].x = (LONG)((double)width * pAdjustValue[3] / MAX_ADJUSTVALUE);
		lpPoint[2].x = lpPoint[3].x = lpPoint[13].x = lpPoint[14].x = width - lpPoint[30].x;
		lpPoint[26].y = lpPoint[27].y = lpPoint[5].y = lpPoint[6].y = (LONG)((double)height * pAdjustValue[3] / MAX_ADJUSTVALUE);
		lpPoint[22].y = lpPoint[21].y = lpPoint[11].y = lpPoint[10].y = height - lpPoint[26].y;
		lpPoint[32] = lpPoint[0];
		*pnum = 33;
		break;
	case ShapeType::Seal4:						// = 187,
	{
		lpPoint = new CPOINT[9];
		if (lpPoint == NULL)
			break;
		convertPoints(lpPoint, _Seal4, 8, width, height);
		LONG _adjustValue = (LONG)(double)((MAX_ADJUSTVALUE * 0.15) + (pAdjustValue[0] * 0.7));
		lpPoint[7].x = lpPoint[5].x = (LONG)((double)width * _adjustValue / MAX_ADJUSTVALUE);
		lpPoint[1].x = lpPoint[3].x = width - lpPoint[7].x;
		lpPoint[7].y = lpPoint[1].y = (LONG)((double)height * _adjustValue / MAX_ADJUSTVALUE);
		lpPoint[5].y = lpPoint[3].y = height - lpPoint[7].y;
		lpPoint[8] = lpPoint[0];
		*pnum = 9;
		break;
	}
	case ShapeType::Star:						// = 12,
		lpPoint = new CPOINT[11];
		if (lpPoint == NULL)
			break;
		convertPoints(lpPoint, _Star, 10, width, height);
		lpPoint[10] = lpPoint[0];
		*pnum = 11;
		break;
	case ShapeType::Seal8:						// = 58,
	{
		int nPointCnt;
		lpPoint = GetSealPoint(nPointCnt, 8, pAdjustValue[0], width, height);
		*pnum = nPointCnt;
	}
	break;
	case ShapeType::Seal16:						// = 59,
	{
		int nPointCnt;
		lpPoint = GetSealPoint(nPointCnt, 16, pAdjustValue[0], width, height);
		*pnum = nPointCnt;
	}
	break;
	case ShapeType::Seal24:						// = 92,
	{
		int nPointCnt;
		lpPoint = GetSealPoint(nPointCnt, 24, pAdjustValue[0], width, height);
		*pnum = nPointCnt;
	}
	break;
	case ShapeType::Seal32:						// = 60,
	{
		int nPointCnt;
		lpPoint = GetSealPoint(nPointCnt, 32, pAdjustValue[0], width, height);
		*pnum = nPointCnt;
	}
	break;
	case ShapeType::IrregularSeal1:				// = 71,
		lpPoint = new CPOINT[25];
		if (lpPoint == NULL)
			break;
		convertPoints(lpPoint, _IrregularSeal1, 24, width, height);
		lpPoint[24] = lpPoint[0];
		*pnum = 25;
		break;
	case ShapeType::IrregularSeal2:				// = 72,
		lpPoint = new CPOINT[28 + 3];
		if (lpPoint == NULL)
			break;
		convertPoints(lpPoint, _IrregularSeal2, 28, width, height);
		lpPoint[28] = lpPoint[0];
		*pnum = 29;
		break;
	}

	if (lpPoint != NULL && !IsShadowType(mType)) // Shadow type은 곡선이 들어간다. 자체적으로 플래그를 세팅
		ReadyPolygonFlag(lpPoint, *pnum);	// Flag를 직선으로 강제 세팅한다.

	*lppPts = lpPoint;
	return((*pnum > 0) ? BasicGraphElement::PolygonElement : BasicGraphElement::None);
}

int KShapeSmartRectObject::GetAdjustHandlePoint(LPPOINT pnts)
{
	return GetAdjustHandlePoint(pnts, Rect, mAdjustValue);
}

int KShapeSmartRectObject::GetAdjustHandlePoint(LPPOINT pnts, RECT& rect, LONG* pAdjustValue)
{
	CPoint	po;
	int		height, width;

	width = rect.right - rect.left;
	height = rect.bottom - rect.top;

	int nHandleXMargin = 0, nHandleYMargin = 0;
	if (height < 0) {
		nHandleYMargin -= SHAPE_HANDLE_OUT_MARGIN;
	}
	else {
		nHandleYMargin += SHAPE_HANDLE_OUT_MARGIN;
	}
	if (width < 0) {
		nHandleXMargin -= SHAPE_HANDLE_OUT_MARGIN;
	}
	else {
		nHandleXMargin += SHAPE_HANDLE_OUT_MARGIN;
	}

	switch (mType)
	{
	case ShapeType::Can:							// = 22,
		pnts[0].x = (rect.left + rect.right) / 2;
		pnts[0].y = rect.top + (int)((double)height * pAdjustValue[0] / 21600);
		return 1;
	case ShapeType::Trapezoid: // 사다리꼴
	case ShapeType::FoldedCorner:				// = 65,
		pnts[0].x = rect.left + (int)((double)width * pAdjustValue[0] / 21600);
		pnts[0].y = rect.bottom + nHandleYMargin;
		return 1;
	case ShapeType::SmileyFace:					// = 96,
		pnts[0].x = (int)((double)(rect.left + rect.right) / 2);
		pnts[0].y = rect.top + (int)((double)height * pAdjustValue[0] / 21600);
		return 1;
	case ShapeType::NoSmoking:					// = 57,
	case ShapeType::Donut:						// = 23,
		pnts[0].x = rect.left + (int)((double)width * pAdjustValue[0] / 21600);
		pnts[0].y = (int)((double)(rect.top + rect.bottom) / 2);
		return 1;
	case ShapeType::Arc: // 원호
	{
		int nSAngle = pAdjustValue[0] >> 16;
		int nEAngle = pAdjustValue[1] >> 16;
		POINT ptCenter;

		if (pAdjustValue[0] == 0 && pAdjustValue[3] == 0)
			nSAngle = -90;

		if (nSAngle > 0) {
			nSAngle = 360 - nSAngle;
		}
		else {
			nSAngle *= -1;
		}
		if (nEAngle > 0) {
			nEAngle = 360 - nEAngle;
		}
		else {
			nEAngle *= -1;
		}

		ptCenter.x = (rect.left + rect.right) / 2;
		ptCenter.y = (rect.top + rect.bottom) / 2;
		if (nEAngle > nSAngle)
			nSAngle += 360;

		CPOINT arc[50];
		int nArcSize = GetArcPolyline(arc, (int)((double)ptCenter.x * 10000 / width), 
			(int)((double)ptCenter.y * 10000 / height), 5000, nSAngle * 10, nEAngle * 10);
		CPOINT* lpData = new CPOINT[nArcSize + 5];
		for (int i = 0; i < nArcSize; i++) {
			lpData[i].x = (LONG)((double)arc[i].x * width / 10000);
			lpData[i].y = (LONG)((double)arc[i].y * height / 10000);
			lpData[i].a = arc[i].a;
		}
		pnts[0].x = lpData[nArcSize - 1].x;
		pnts[0].y = lpData[nArcSize - 1].y;
		pnts[1].x = lpData[0].x;
		pnts[1].y = lpData[0].y;
		delete[] lpData;
	}
	return 2;
	case ShapeType::CircularArrow:				// 원형 화살표
	{
		int nSAngle = pAdjustValue[0] >> 16;
		int nEAngle = pAdjustValue[1] >> 16;
		int cx = (int)((double)pAdjustValue[2] * 5000 / 10800 + 0.5);
		POINT ptCenter;

		if (pAdjustValue[0] == 0 && pAdjustValue[2] == 0)
			nSAngle = -90;

		if (nSAngle > 0) {
			nSAngle = 360 - nSAngle;
		}
		else {
			nSAngle *= -1;
		}
		if (nEAngle > 0) {
			nEAngle = 360 - nEAngle;
		}
		else {
			nEAngle *= -1;
		}

		ptCenter.x = (rect.left + rect.right) / 2;
		ptCenter.y = (rect.top + rect.bottom) / 2;

		if (nEAngle > nSAngle)
			nSAngle += 360;

		CPOINT arc[50];
		int i;
		int nArcSize = GetArcPolyline(arc, (int)((double)ptCenter.x * 10000 / width), 
			(int)((double)ptCenter.y * 10000 / height), 5000, nSAngle * 10, nEAngle * 10);
		CPOINT* lpData = new CPOINT[nArcSize + 5];
		for (i = 0; i < nArcSize; i++) {
			lpData[i].x = (LONG)((double)arc[i].x * width / 10000);
			lpData[i].y = (LONG)((double)arc[i].y * height / 10000);
			lpData[i].a = arc[i].a;
		}
		pnts[0].x = lpData[0].x;
		pnts[0].y = lpData[0].y;

		delete[] lpData;
		lpData = NULL;

		nArcSize = GetArcPolyline(arc, (int)((double)ptCenter.x * 10000 / width), 
			(int)((double)ptCenter.y * 10000 / height), cx, nSAngle * 10, nEAngle * 10);
		lpData = new CPOINT[nArcSize + 5];
		for (i = 0; i < nArcSize; i++) {
			lpData[i].x = (LONG)((double)arc[i].x * width / 10000);
			lpData[i].y = (LONG)((double)arc[i].y * height / 10000);
			lpData[i].a = arc[i].a;
		}
		pnts[1].x = lpData[nArcSize - 1].x;
		pnts[1].y = lpData[nArcSize - 1].y;

		delete[] lpData;
		lpData = NULL;
	}
	return 2;
	case ShapeType::BlockArc:					// = 95,
	{
		POINT pe;
		int angle = (int)(short)((pAdjustValue[0] >> 16) & 0x00FFFF);
		int len = (int)(short)(pAdjustValue[1] & 0x00FFFF);
		double yr = (double)abs(rect.right - rect.left) / abs(rect.bottom - rect.top);

		int a = (3600 - angle * 10);
		len = (int)((double)abs(width) * len / 21600 + 0.5);
		GetPointFromAngleDist(a, len, &pe);

		if (width < 0)
			pe.x *= -1;
		if (height < 0)
			pe.y *= -1;

		pnts[0].x = (int)((double)(rect.left + rect.right) / 2 + 0.5) + pe.x;
		pnts[0].y = (int)((double)(rect.top + rect.bottom) / 2 + 0.5) + (int)((double)pe.y / yr + 0.5);
		return 1;
	}
	case ShapeType::Heart:						// = 74,
	case ShapeType::LightningBolt:				// = 73,
		return 0; // 핸들이 없음.
	case ShapeType::Sun:						// = 183,
	case ShapeType::Moon:						// = 184,
		pnts[0].x = rect.left + (int)((double)width * pAdjustValue[0] / 21600 + 0.5);
		pnts[0].y = (int)((double)(rect.top + rect.bottom) / 2 + 0.5);
		return 1;
	case ShapeType::HomePlate: // 오각형(블럭화살표)
	case ShapeType::Chevron: // 갈매기형 수장
	case ShapeType::Hexagon: // 육각형
	case ShapeType::IsocelesTriangle: // 이등변 삼각형
	case ShapeType::Parallelogram: // 평행사변형
		pnts[0].x = rect.left + (int)((double)width * pAdjustValue[0] / 21600 + 0.5);
		pnts[0].y = rect.top - nHandleYMargin;
		return 1;
	case ShapeType::LeftBracket:					// = 85,
		pnts[0].x = rect.left - nHandleXMargin;
		pnts[0].y = rect.top + (int)((double)height * pAdjustValue[0] / 21600 + 0.5);
		return 1;
	case ShapeType::RightBracket:				// = 86,
		pnts[0].x = rect.right + nHandleXMargin;
		pnts[0].y = rect.top + (int)((double)height * pAdjustValue[0] / 21600 + 0.5);
		return 1;

	case ShapeType::LeftBrace:					// = 87,
		pnts[0].x = (int)((double)(rect.left + rect.right) / 2 + 0.5);
		pnts[0].y = rect.top + (int)((double)height * pAdjustValue[0] / 21600 + 0.5);
		pnts[1].x = rect.left - nHandleXMargin;
		pnts[1].y = rect.top + (int)((double)height * pAdjustValue[1] / 21600 + 0.5);
		return 2;
	case ShapeType::RightBrace:					// = 88,
		pnts[0].x = (int)((double)(rect.left + rect.right) / 2 + 0.5);
		pnts[0].y = rect.top + (int)((double)height * pAdjustValue[0] / 21600 + 0.5);
		pnts[1].x = rect.right + nHandleXMargin;
		pnts[1].y = rect.top + (int)((double)height * pAdjustValue[1] / 21600 + 0.5);
		return 2;
	case ShapeType::CloudCallout: // 구름 모양 설명선
	case ShapeType::WedgeEllipseCallout: // 타원형 설명선
	case ShapeType::WedgeRRectCallout: // 모서리 둥근 사각형 설명선
	case ShapeType::WedgeRectCallout: // 사각형 설명선
	case ShapeType::NotchedRightArrow: // 톱니 모양의 오른쪽 화살표
	case ShapeType::LeftArrow: // 왼쪽 화살표
	case ShapeType::LeftRightArrow: // 왼쪽/오른쪽 화살표
	case ShapeType::UpDownArrow: // 위쪽/아래쪽 화살표
	case ShapeType::Arrow: // 오른쪽 화살표
	case ShapeType::BentArrow:					// = 91,
		pnts[0].x = (LONG)(rect.left + (double)width * pAdjustValue[0] / 21600 + 0.5);
		pnts[0].y = (LONG)(rect.top + (double)height * pAdjustValue[1] / 21600 + 0.5);
		return 1;
	case ShapeType::UpArrow: // 위쪽 화살표
	case ShapeType::DownArrow: // 아래쪽 화살표
		pnts[0].x = rect.left + (int)((double)width * pAdjustValue[1] / 21600 + 0.5);
		pnts[0].y = rect.top + (int)((double)height * pAdjustValue[0] / 21600 + 0.5);
		return 1;
	case ShapeType::StripedRightArrow:			// = 93,
		pnts[0].x = rect.left + (int)((double)width * pAdjustValue[0] / 21600 + 0.5);
		pnts[0].y = rect.top + (int)((double)height * pAdjustValue[1] / 21600 + 0.5);
		return 1;
	case ShapeType::CurvedRightArrow:			// = 102,
	case ShapeType::CurvedLeftArrow:			// = 103,
		pnts[0].x = (mType == ShapeType::CurvedLeftArrow) ? (rect.left - nHandleXMargin) : (rect.right + nHandleXMargin);
		pnts[0].y = rect.top + (int)((double)height * pAdjustValue[0] / 21600 + 0.5);
		pnts[1].x = pnts[0].x;
		pnts[1].y = rect.top + (int)((double)height * pAdjustValue[1] / 21600 + 0.5);
		pnts[2].x = rect.left + (int)((double)width * pAdjustValue[2] / 21600 + 0.5);
		pnts[2].y = rect.bottom + nHandleYMargin;
		return 3;
	case ShapeType::CurvedUpArrow:				// = 104,
	case ShapeType::CurvedDownArrow:			// = 105,
		pnts[0].x = rect.left + (int)((double)width * pAdjustValue[0] / 21600 + 0.5);
		pnts[0].y = (mType == ShapeType::CurvedUpArrow) ? (rect.top - nHandleYMargin) : (rect.bottom + nHandleYMargin);
		pnts[1].x = rect.left + (int)((double)width * pAdjustValue[1] / 21600 + 0.5);
		pnts[1].y = pnts[0].y;
		pnts[2].x = rect.right + nHandleXMargin;
		pnts[2].y = rect.top + (int)((double)height * pAdjustValue[2] / 21600 + 0.5);
		return 3;
	case ShapeType::LeftRightArrowCallout: // 좌우 화살표 설명선
	case ShapeType::LeftArrowCallout: // 왼쪽 화살표 설명선
	case ShapeType::RightArrowCallout: // 오른쪽 화살표 설명선
		pnts[0].x = rect.left + (int)((double)width * pAdjustValue[0] / 21600 + 0.5);
		pnts[0].y = rect.top - nHandleYMargin;
		pnts[1].x = (mType == ShapeType::RightArrowCallout) ? (rect.right + nHandleXMargin) : (rect.left - nHandleXMargin);
		pnts[1].y = rect.top + (int)((double)height * pAdjustValue[1] / 21600 + 0.5);
		pnts[2].x = rect.left + (int)((double)width * pAdjustValue[2] / 21600 + 0.5);
		pnts[2].y = rect.top + (int)((double)height * pAdjustValue[3] / 21600 + 0.5);
		return 3;
	case ShapeType::QuadArrowCallout: // 상하좌우 화살표 설명선
	case ShapeType::UpDownArrowCallout: // 상하 화살표 설명선
	case ShapeType::UpArrowCallout: // 위쪽 화살표 설명선
		pnts[0].x = rect.left - nHandleXMargin;
		pnts[0].y = rect.top + (int)((double)height * pAdjustValue[0] / 21600 + 0.5);
		pnts[1].x = rect.left + (int)((double)width * pAdjustValue[1] / 21600 + 0.5);
		pnts[1].y = rect.top - nHandleYMargin;
		pnts[2].x = rect.left + (int)((double)width * pAdjustValue[3] / 21600 + 0.5);
		pnts[2].y = rect.top + (int)((double)height * pAdjustValue[2] / 21600 + 0.5);
		return 3;
	case ShapeType::DownArrowCallout: // 아래쪽 화살표 설명선
		pnts[0].x = rect.left - nHandleXMargin;
		pnts[0].y = rect.top + (int)((double)height * pAdjustValue[0] / 21600 + 0.5);
		pnts[1].x = rect.left + (int)((double)width * pAdjustValue[1] / 21600 + 0.5);
		pnts[1].y = rect.bottom + nHandleYMargin;
		pnts[2].x = rect.left + (int)((double)width * pAdjustValue[3] / 21600 + 0.5);
		pnts[2].y = rect.top + (int)((double)height * pAdjustValue[2] / 21600 + 0.5);
		return 3;
	case ShapeType::BentUpArrow: // 위로 굽은 화살표
	case ShapeType::LeftUpArrow: // 좌상 화살표
	case ShapeType::QuadArrow: // 상하좌우 화살표
		pnts[0].x = rect.left + (int)((double)width * pAdjustValue[0] / 21600 + 0.5);
		pnts[0].y = rect.top - nHandleYMargin;
		pnts[1].x = rect.left + (int)((double)width * pAdjustValue[1] / 21600 + 0.5);
		pnts[1].y = rect.top + (int)((double)height * pAdjustValue[2] / 21600 + 0.5);
		return 2;
	case ShapeType::LeftRightUpArrow: // 상좌우 화살표
		pnts[0].x = rect.left + (int)((double)width * pAdjustValue[0] / 21600 + 0.5);
		pnts[0].y = rect.top - nHandleYMargin;
		pnts[1].x = rect.left + (int)((double)width * pAdjustValue[1] / 21600 + 0.5);
		pnts[1].y = rect.top + (int)((double)height * pAdjustValue[2] / 21600 + 0.5);
		return 2;
	case ShapeType::Ribbon2:					// = 54, 위쪽 리본
	case ShapeType::Ribbon:					// = 53,
		pnts[0].x = rect.left + (int)((double)width * pAdjustValue[0] / 21600 + 0.5);
		pnts[0].y = (mType == ShapeType::Ribbon2) ? (rect.top - nHandleYMargin) : (rect.bottom + nHandleYMargin);
		pnts[1].x = (int)((double)(rect.left + rect.right) / 2 + 0.5);
		pnts[1].y = rect.top + (int)((double)height * pAdjustValue[1] / 21600 + 0.5);
		return 2;
	case ShapeType::EllipseRibbon2:			// = 108, 위쪽 리본
	case ShapeType::EllipseRibbon:				// = 107,
		pnts[0].x = rect.left + (int)((double)width * pAdjustValue[0] / 21600 + 0.5);
		pnts[0].y = (mType == ShapeType::EllipseRibbon2) ? (rect.top - nHandleYMargin) : (rect.bottom + nHandleYMargin);
		pnts[1].x = (int)((double)(rect.left + rect.right) / 2 + 0.5);
		pnts[1].y = rect.top + (int)((double)height * pAdjustValue[1] / 21600 + 0.5);
		pnts[2].x = rect.left - nHandleXMargin;
		pnts[2].y = rect.top + (int)((double)height * pAdjustValue[2] / 21600 + 0.5);
		return 3;
	case ShapeType::Wave:
	case ShapeType::DoubleWave:
		pnts[0].x = rect.left - nHandleXMargin;
		pnts[0].y = rect.top + (int)((double)height * pAdjustValue[0] / 21600 + 0.5);
		pnts[1].x = rect.left + (int)((double)width * pAdjustValue[1] / 21600 + 0.5);
		pnts[1].y = rect.bottom + nHandleYMargin;
		return 2;

	case ShapeType::Seal4:
	case ShapeType::Seal8:
	case ShapeType::Seal16:
	case ShapeType::Seal24:
	case ShapeType::Seal32:
		pnts[0].x = rect.left + (int)((double)width * pAdjustValue[0] / 21600 + 0.5);
		pnts[0].y = (int)((double)(rect.top + rect.bottom) / 2 + 0.5);
		return 1;
	case ShapeType::VerticalScroll:			// = 97,
		pnts[0].x = rect.left - nHandleXMargin;
		pnts[0].y = rect.top + (int)((double)height * pAdjustValue[0] / 43200 + 0.5);
		return 1;
	case ShapeType::HorizontalScroll:			// = 98,
		pnts[0].x = rect.left + (int)((double)width * pAdjustValue[0] / 43200 + 0.5);
		pnts[0].y = rect.top - nHandleYMargin;
		return 1;
	case ShapeType::BracePair: // 양쪽 중괄호
	case ShapeType::Cube: // 정육면체
		if (abs(width) < abs(height)) {
			if ((width > 0 && height > 0) || (width < 0 && height > 0)) {
				pnts[0].x = rect.left + (int)((double)width * pAdjustValue[0] / 21600 + 0.5);
				pnts[0].y = rect.top - SHAPE_HANDLE_OUT_MARGIN;
			}
			else if ((width > 0 && height < 0) || (width < 0 && height < 0)) {
				pnts[0].x = rect.left + (int)((double)width * pAdjustValue[0] / 21600 + 0.5);
				pnts[0].y = rect.top + SHAPE_HANDLE_OUT_MARGIN;
			}
		}
		else {
			if ((width > 0 && height > 0) || (width > 0 && height < 0)) {
				pnts[0].x = rect.left - SHAPE_HANDLE_OUT_MARGIN;
				pnts[0].y = rect.top + (int)((double)height * pAdjustValue[0] / 21600 + 0.5);
			}
			else if ((width < 0 && height > 0) || (width < 0 && height < 0)) {
				pnts[0].x = rect.left + SHAPE_HANDLE_OUT_MARGIN;
				pnts[0].y = rect.top + (int)((double)height * pAdjustValue[0] / 21600 + 0.5);
			}
		}
		return 1;
	case ShapeType::Bevel:				// 빗면
	case ShapeType::BracketPair:		// 양쪽 대괄호
	case ShapeType::Plaque:			// 배지
	case ShapeType::Plus:				// 십자가
	case ShapeType::Octagon:			// 팔각형
	case ShapeType::RoundRectangle:	// 모서리가 둥근 사각형
		if (abs(width) < abs(height)) {
			if (width > 0 && height > 0) {
				pnts[0].x = rect.left - SHAPE_HANDLE_OUT_MARGIN;
				pnts[0].y = rect.top + (int)((double)abs(width) * pAdjustValue[0] / MAX_ADJUSTVALUE + 0.5);
			}
			else if (width > 0 && height < 0) {
				pnts[0].x = rect.left - SHAPE_HANDLE_OUT_MARGIN;
				pnts[0].y = rect.top - (int)((double)abs(width) * pAdjustValue[0] / MAX_ADJUSTVALUE + 0.5);
			}
			else if (width < 0 && height > 0) {
				pnts[0].x = rect.left + SHAPE_HANDLE_OUT_MARGIN;
				pnts[0].y = rect.top + (int)((double)abs(width) * pAdjustValue[0] / MAX_ADJUSTVALUE + 0.5);
			}
			else if (width < 0 && height < 0) {
				pnts[0].x = rect.left + SHAPE_HANDLE_OUT_MARGIN;
				pnts[0].y = rect.top - (int)((double)abs(width) * pAdjustValue[0] / MAX_ADJUSTVALUE + 0.5);
			}
		}
		else {
			if (width > 0 && height > 0) {
				pnts[0].x = rect.left + (int)((double)abs(height) * pAdjustValue[0] / MAX_ADJUSTVALUE + 0.5);
				pnts[0].y = rect.top - SHAPE_HANDLE_OUT_MARGIN;
			}
			else if (width > 0 && height < 0) {
				pnts[0].x = rect.left + (int)((double)abs(height) * pAdjustValue[0] / MAX_ADJUSTVALUE + 0.5);
				pnts[0].y = rect.top + SHAPE_HANDLE_OUT_MARGIN;
			}
			else if (width < 0 && height > 0) {
				pnts[0].x = rect.left - (int)((double)abs(height) * pAdjustValue[0] / MAX_ADJUSTVALUE + 0.5);
				pnts[0].y = rect.top - SHAPE_HANDLE_OUT_MARGIN;
			}
			else if (width < 0 && height < 0) {
				pnts[0].x = rect.left - (int)((double)abs(height) * pAdjustValue[0] / MAX_ADJUSTVALUE + 0.5);
				pnts[0].y = rect.top + SHAPE_HANDLE_OUT_MARGIN;
			}
		}
		return 1;
	case ShapeType::Callout90:					// = 178,
	case ShapeType::AccentCallout90:				// = 179,
	case ShapeType::AccentBorderCallout90:		// = 181,
	case ShapeType::BorderCallout90:				// = 180,
	case ShapeType::Callout1:					// = 41,
	case ShapeType::Callout2:					// = 42,
	case ShapeType::Callout3:					// = 43,
	case ShapeType::AccentCallout1:				// = 44,
	case ShapeType::AccentCallout2:				// = 45,
	case ShapeType::AccentCallout3:				// = 46,
	case ShapeType::BorderCallout1:				// = 47,
	case ShapeType::BorderCallout2:				// = 48,
	case ShapeType::BorderCallout3:				// = 49,
	case ShapeType::AccentBorderCallout1:		// = 50,
	case ShapeType::AccentBorderCallout2:		// = 51,
	case ShapeType::AccentBorderCallout3:		// = 52,
		pnts[0].x = rect.left + (int)((double)width * pAdjustValue[0] / 21600 + 0.5);
		pnts[0].y = rect.top + (int)((double)height * pAdjustValue[1] / 21600 + 0.5);
		return 1;
	default:
		break;
	}
	return 0;
}


BOOL KShapeSmartRectObject::SetAdjustHandlePoint(int idx, LPPOINT pnts, BOOL bSet, LONG* pAdjustValue)
{
	BOOL	rtnX = FALSE, rtnY = FALSE;
	int		adjust;
	int		width, height;
	int		min, max;
		
	height = abs(Rect.top - Rect.bottom);
	width = abs(Rect.right - Rect.left);

	switch (mType)
	{
	case ShapeType::FoldedCorner: // 모서리 접힌 사각형 = 65,
		rtnX = SetAdjustHandlePointHelper(pnts, bSet, pAdjustValue, 0, 21600, 0, 10800, 21600);
		break;
	case ShapeType::SmileyFace: // 웃는 얼굴 = 96,
		rtnY = SetAdjustHandlePointHelper(pnts, bSet, pAdjustValue, 0, 0, 21600, 15510, 17600);
		// halfHeight = (pAdjustValue[0] - 15510) * 900 / (17600 - 15510);
		break;
	case ShapeType::Donut:	// 도넛 = 23
	case ShapeType::Hexagon: // 육각형
	case ShapeType::Trapezoid: // 사다리꼴
		rtnX = SetAdjustHandlePointHelper(pnts, bSet, pAdjustValue, 0, 21600, 0, 0, 10800);
		break;
	case ShapeType::NoSmoking:	// 없음 = 57,
		rtnX = SetAdjustHandlePointHelper(pnts, bSet, pAdjustValue, 0, 21600, 0, 0, 7200);
		break;
	case ShapeType::Arc: // 원호
	{
		BOOL bXFlip = FALSE, bYFlip = FALSE;

		if (Rect.left > Rect.right)
			bXFlip = TRUE;
		if (Rect.top > Rect.bottom)
			bYFlip = TRUE;

		int nIndex = 0;
		if (idx == 0) {
			nIndex = 1;
		}
		else if (idx == 1) {
			nIndex = 0;
		}

		int nAdjust = pAdjustValue[nIndex];
		POINT ptCenter;

		ptCenter.x = (Rect.left + Rect.right) / 2;
		ptCenter.y = (Rect.top + Rect.bottom) / 2;

		POINT c, d;
		double yr = (double)abs(width) / abs(height);
		c = ptCenter;
		d.x = pnts->x;

		if (bYFlip) {
			d.y = c.y + (int)((c.y - pnts->y) * yr + 0.5);
		}
		else {
			d.y = c.y + (int)((pnts->y - c.y) * yr + 0.5);
		}
		int a;
		a = (int)((double)((double)GetLinePointAngle(&c, &d) / 10 + 0.5));

		if (bXFlip)
			a = -a + 540;

		if (0 <= a && a < 180) {
			nAdjust = -(a << 16);
		}
		else {
			nAdjust = (360 - a) << 16;
		}

		if (pAdjustValue[nIndex] != nAdjust) {
			if (bSet)
				pAdjustValue[nIndex] = nAdjust;
			rtnX = TRUE;
		}
	}
	break;
	case ShapeType::CircularArrow: // 원형 화살표
	{
		BOOL bXFlip = FALSE, bYFlip = FALSE;

		if (Rect.left > Rect.right)
			bXFlip = TRUE;
		if (Rect.top > Rect.bottom)
			bYFlip = TRUE;

		int nAdjust = pAdjustValue[idx];
		POINT ptCenter;

		ptCenter.x = (Rect.left + Rect.right) / 2;
		ptCenter.y = (Rect.top + Rect.bottom) / 2;

		POINT c, d;
		double yr = (double)abs(width) / abs(height);
		c = ptCenter;
		d.x = pnts->x;

		if (bYFlip) {
			d.y = c.y + (int)((c.y - pnts->y) * yr + 0.5);
		}
		else {
			d.y = c.y + (int)((pnts->y - c.y) * yr + 0.5);
		}
		int a;
		a = (int)((double)((double)GetLinePointAngle(&c, &d) / 10 + 0.5));

		if (bXFlip)
			a = -a + 540;

		if (0 <= a && a < 180) {
			nAdjust = -(a << 16);
		}
		else {
			nAdjust = (360 - a) << 16;
		}

		if (pAdjustValue[idx] != nAdjust) {
			if (bSet)
				pAdjustValue[idx] = nAdjust;
			rtnX = TRUE;
		}

		if (idx == 1) {
			int len = GetDistance((c.x - d.x), (c.y - d.y)) * 21600 / abs(width);
			if (len >= 10800)
				len = 10799;
			if (len < 0)
				len = 0;
			if (pAdjustValue[2] != len) {
				if (bSet)
					pAdjustValue[2] = len;
				rtnX = TRUE;
			}
		}
	}
	break;
	case ShapeType::BlockArc: // 막힌 원호 = 95,
	{
		POINT c, d;
		int a, len;
		BOOL bXFlip = FALSE, bYFlip = FALSE;

		if (Rect.left > Rect.right)
			bXFlip = TRUE;
		if (Rect.top > Rect.bottom)
			bYFlip = TRUE;

		double yr = (double)abs(Rect.right - Rect.left) / abs(Rect.bottom - Rect.top);

		c.x = (Rect.left + Rect.right) / 2;
		c.y = (Rect.top + Rect.bottom) / 2;
		d.x = pnts->x;

		if (bYFlip) {
			d.y = c.y + (int)((c.y - pnts->y) * yr + 0.5);
		}
		else {
			d.y = c.y + (int)((pnts->y - c.y) * yr + 0.5);
		}

		if (bXFlip) {
			a = 180 + (int)(GetLinePointAngle(&c, &d) / 10 + 0.5);
		}
		else {
			a = 360 - (int)(GetLinePointAngle(&c, &d) / 10 + 0.5);
		}

		len = GetDistance((c.x - d.x), (c.y - d.y)) * 21600 / abs(width);
		if (len >= 10800)
			len = 10799;
		if (len < 0)
			len = 0;
		adjust = (pAdjustValue[0] & 0x0000FFFF) | (a << 16);

		if ((pAdjustValue[0] != adjust) || (pAdjustValue[1] != len))
		{
			if (bSet)
			{
				pAdjustValue[0] = adjust;
				pAdjustValue[1] = len;
			}
			// TRACE("new angle=%d, new width = %d\n", a, pAdjustValue[1]);
			rtnX = rtnY = TRUE;
		}
	}
	break;
	case ShapeType::Heart:	// 하트 = 74,
	case ShapeType::LightningBolt:	// 번개 = 73,
		break; // 핸들이 없음.
	case ShapeType::Sun: // 태양 = 183,
		rtnX = SetAdjustHandlePointHelper(pnts, bSet, pAdjustValue, 0, 21600, 0, 2700, 10125);
		break;
	case ShapeType::Moon: // 달 = 184,			
		rtnX = SetAdjustHandlePointHelper(pnts, bSet, pAdjustValue, 0, 21600, 0, 0, 18900);
		break;
	case ShapeType::Can: // 원통 = 22
		rtnY = SetAdjustHandlePointHelper(pnts, bSet, pAdjustValue, 0, 0, 21600, 0, 10800);
		break;
	case ShapeType::BracePair:	// 양쪽 중괄호 = 186
		if (width < height) {
			rtnX = SetAdjustHandlePointHelper(pnts, bSet, pAdjustValue, 0, 21600, 0, 0, 5400);
		}
		else {
			rtnY = SetAdjustHandlePointHelper(pnts, bSet, pAdjustValue, 0, 0, 21600, 0, 5400);
		}
		break;
	case ShapeType::Cube: // 정육면체
		if (width < height) {
			rtnX = SetAdjustHandlePointHelper(pnts, bSet, pAdjustValue, 0, 21600, 0, 0, 21600);
		}
		else {
			rtnY = SetAdjustHandlePointHelper(pnts, bSet, pAdjustValue, 0, 0, 21600, 0, 21600);
		}
		break;
	case ShapeType::Bevel:				// 빗면
	case ShapeType::BracketPair:		// 양쪽 대괄호 = 185
	case ShapeType::Plus:				// 십자가
	case ShapeType::Octagon:			// 팔각형
	case ShapeType::RoundRectangle:	// 모서리가 둥근 사각형
	case ShapeType::Plaque:			// 배지 = 21,
	{
		int nAdjust = 0;
		if (width < height) {
			if (Rect.top < Rect.bottom) {
				if (pnts->y < Rect.top)
					pnts->y = Rect.top;
			}
			else {
				if (pnts->y > Rect.top)
					pnts->y = Rect.top;
			}
			nAdjust = LogicalAdjust(abs(pnts->y - Rect.top), 21600, width);
		}
		else {
			if (Rect.left < Rect.right) {
				if (pnts->x < Rect.left)
					pnts->x = Rect.left;
			}
			else {
				if (pnts->x > Rect.left)
					pnts->x = Rect.left;
			}
			nAdjust = LogicalAdjust(abs(pnts->x - Rect.left), 21600, height);
		}
		ClipMinMaxValue(nAdjust, 0, 10800);
		if (pAdjustValue[0] != nAdjust) {
			if (bSet)
				pAdjustValue[0] = nAdjust;
			rtnX = TRUE;
		}
	}
	break;
	case ShapeType::LeftBracket: // 왼쪽 대괄호 = 85,
	case ShapeType::RightBracket: // 오른쪽 대괄호 = 86,
		rtnY = SetAdjustHandlePointHelper(pnts, bSet, pAdjustValue, 0, 0, 21600, 0, 10800);
		break;
	case ShapeType::LeftBrace:	// 왼쪽 중괄호 = 87,
	case ShapeType::RightBrace: // 오른쪽 중괄호 = 88,
		if (idx == 0) {
			min = pAdjustValue[1] / 2;
			max = (21600 - pAdjustValue[1]) / 2;
			if (max > min)
				max = min;
			if (max > 5395)
				max = 5395;
			rtnY = SetAdjustHandlePointHelper(pnts, bSet, pAdjustValue, 0, 0, 21600, 0, max);
		}
		else if (idx == 1) {
			rtnY = SetAdjustHandlePointHelper(pnts, bSet, pAdjustValue, 1, 0, 21600, pAdjustValue[0] * 2, 21600 - pAdjustValue[0] * 2);
		}
		break;
	case ShapeType::BentArrow:	// 굽은 화살표 = 91,
		rtnX = SetAdjustHandlePointHelper(pnts, bSet, pAdjustValue, 0, 21600, 0, 12427, 21600);
		rtnY = SetAdjustHandlePointHelper(pnts, bSet, pAdjustValue, 1, 0, 21600, 0, 6079);
		break;
	case ShapeType::StripedRightArrow: // 줄무늬 오른쪽 화살표 = 93,
		rtnX = SetAdjustHandlePointHelper(pnts, bSet, pAdjustValue, 0, 21600, 0, 3375, 21600);
		rtnY = SetAdjustHandlePointHelper(pnts, bSet, pAdjustValue, 1, 0, 21600, 0, 10800);
		break;
	case ShapeType::CurvedRightArrow: // 오른쪽으로 구부러진 화살표 = 102,
	case ShapeType::CurvedLeftArrow: // 왼쪽으로 구부러진 화살표 = 103,
		if (idx == 0) {
			rtnY = SetAdjustHandlePointHelper(pnts, bSet, pAdjustValue, 0, 0, 21600, 10800, 21600 - (21600 - pAdjustValue[1]) * 2);
		}
		else if (idx == 1) {
			rtnY = SetAdjustHandlePointHelper(pnts, bSet, pAdjustValue, 1, 0, 21600, 21600 - (21600 - pAdjustValue[0]) / 2, 21600);
		}
		else if (idx == 2) {
			if (mType == ShapeType::CurvedRightArrow) {
				rtnX = SetAdjustHandlePointHelper(pnts, bSet, pAdjustValue, 2, 21600, 0, 10800 - (pAdjustValue[0] - 10800), 21600);
			}
			else {
				rtnX = SetAdjustHandlePointHelper(pnts, bSet, pAdjustValue, 2, 21600, 0, 0, 10800 + (pAdjustValue[0] - 10800));
			}
		}
		break;
	case ShapeType::CurvedUpArrow:	// 위로 굽은 화살표 = 104,
	case ShapeType::CurvedDownArrow: // 아래로 굽은 화살표 = 105,
		if (idx == 0) {
			rtnX = SetAdjustHandlePointHelper(pnts, bSet, pAdjustValue, 0, 21600, 0, 10800, 21600 - (21600 - pAdjustValue[1]) * 2);
		}
		else if (idx == 1) {
			rtnX = SetAdjustHandlePointHelper(pnts, bSet, pAdjustValue, 1, 21600, 0, 21600 - ((21600 - pAdjustValue[0]) / 2), 21600);
		}
		else if (idx == 2) {
			if (mType == ShapeType::CurvedDownArrow) {
				rtnY = SetAdjustHandlePointHelper(pnts, bSet, pAdjustValue, 2, 0, 21600, 10800 - (pAdjustValue[0] - 10800), 21600);
			}
			else {
				rtnY = SetAdjustHandlePointHelper(pnts, bSet, pAdjustValue, 2, 0, 21600, 0, 10800 + (pAdjustValue[0] - 10800));
			}
		}
		break;
	case ShapeType::Ribbon2: // 위쪽 리본 = 54
	case ShapeType::Ribbon: // 아래쪽 리본 = 53,
		if (idx == 0) {
			rtnX = SetAdjustHandlePointHelper(pnts, bSet, pAdjustValue, 0, 21600, 0, 2700, 8100);
		}
		else if (idx == 1) {
			if (mType == ShapeType::Ribbon2) {
				rtnY = SetAdjustHandlePointHelper(pnts, bSet, pAdjustValue, 1, 0, 21600, 14400, 21600);
			}
			else {
				rtnY = SetAdjustHandlePointHelper(pnts, bSet, pAdjustValue, 1, 0, 21600, 0, 7200);
			}
		}
		break;
	case ShapeType::EllipseRibbon2: // 위로 구부러진 리본 = 108
	case ShapeType::EllipseRibbon:	// 아래로 구부러진 리본 = 107,
		// min, max값 정확하지 않음.
		if (idx == 0) {
			rtnX = SetAdjustHandlePointHelper(pnts, bSet, pAdjustValue, 0, 21600, 0, 2700, 8100);
		}
		else if (idx == 1) {
			if (mType == ShapeType::EllipseRibbon2) {
				rtnY = SetAdjustHandlePointHelper(pnts, bSet, pAdjustValue, 1, 0, 21600, 21600 - pAdjustValue[2] * 2, 21600 - pAdjustValue[2]);
			}
			else {
				rtnY = SetAdjustHandlePointHelper(pnts, bSet, pAdjustValue, 1, 0, 21600, 21600 - pAdjustValue[2], (21600 - pAdjustValue[2]) * 2);
			}
		}
		else if (idx == 2) {
			if (mType == ShapeType::EllipseRibbon2) {
				rtnY = SetAdjustHandlePointHelper(pnts, bSet, pAdjustValue, 2, 0, 21600, 675, 21600 - pAdjustValue[1]);
			}
			else {
				rtnY = SetAdjustHandlePointHelper(pnts, bSet, pAdjustValue, 2, 0, 21600, 21600 - pAdjustValue[1], 21600 - 675);
			}
		}
		break;
	case ShapeType::Wave: // 물결
	case ShapeType::DoubleWave: // 이중물결
		if (idx == 0) {
			rtnY = SetAdjustHandlePointHelper(pnts, bSet, pAdjustValue, 0, 0, 21600, 0, 4459);
		}
		else if (idx == 1) {
			rtnX = SetAdjustHandlePointHelper(pnts, bSet, pAdjustValue, 1, 21600, 0, 8640, 12960);
		}
		break;
	case ShapeType::Seal4: // 포인터 4인 별
	case ShapeType::Seal8: // 포인터 8인 별
	case ShapeType::Seal16: // 포인터 16인 별
	case ShapeType::Seal24: // 포인터 24인 별
	case ShapeType::Seal32: // 포인터 32인 별
		rtnX = SetAdjustHandlePointHelper(pnts, bSet, pAdjustValue, 0, 21600, 0, 0, 10800);
		break;
	case ShapeType::VerticalScroll: // 세로 두루마리 = 97,
		rtnY = SetAdjustHandlePointHelper(pnts, bSet, pAdjustValue, 0, 0, 43200, 0, 5400);
		break;
	case ShapeType::HorizontalScroll: // 가로 두루마리 = 98,
		rtnX = SetAdjustHandlePointHelper(pnts, bSet, pAdjustValue, 0, 43200, 0, 0, 5400);
		break;
	case ShapeType::HomePlate: // 오각형(블럭화살표)
	case ShapeType::Chevron: // 갈매기형 수장
	case ShapeType::IsocelesTriangle: // 이등변삼각형
	case ShapeType::Parallelogram: // 평행사변형
		rtnX = SetAdjustHandlePointHelper(pnts, bSet, pAdjustValue, 0, 21600, 0, 0, 21600);
		break;
	case ShapeType::NotchedRightArrow: // 톱니 모양의 오른쪽 화살표
	case ShapeType::LeftArrow: // 왼쪽 화살표
	case ShapeType::LeftRightArrow: // 왼쪽/오른쪽 화살표
	case ShapeType::Arrow: // 오른쪽 화살표
	case ShapeType::UpArrow: // 위쪽 화살표
	case ShapeType::DownArrow: // 아래쪽 화살표
	case ShapeType::UpDownArrow: // 위쪽/아래쪽 화살표
		if (mType == ShapeType::Arrow || mType == ShapeType::LeftArrow || mType == ShapeType::NotchedRightArrow) {
			rtnX = SetAdjustHandlePointHelper(pnts, bSet, pAdjustValue, 0, 21600, 0, 0, 21600);
			rtnY = SetAdjustHandlePointHelper(pnts, bSet, pAdjustValue, 1, 0, 21600, 0, 10800);
		}
		else if (mType == ShapeType::LeftRightArrow || mType == ShapeType::UpDownArrow) {
			rtnX = SetAdjustHandlePointHelper(pnts, bSet, pAdjustValue, 0, 21600, 0, 0, 10800);
			rtnY = SetAdjustHandlePointHelper(pnts, bSet, pAdjustValue, 1, 0, 21600, 0, 10800);
		}
		else if (mType == ShapeType::UpArrow || mType == ShapeType::DownArrow) {
			rtnX = SetAdjustHandlePointHelper(pnts, bSet, pAdjustValue, 1, 21600, 0, 0, 10800);
			rtnY = SetAdjustHandlePointHelper(pnts, bSet, pAdjustValue, 0, 0, 21600, 0, 21600);
		}
		break;
	case ShapeType::LeftRightUpArrow: // 상좌우 화살표
		if (idx == 0) {
			rtnX = SetAdjustHandlePointHelper(pnts, bSet, pAdjustValue, 0, 21600, 0, pAdjustValue[2], pAdjustValue[1]);
		}
		else if (idx == 1) {
			rtnX = SetAdjustHandlePointHelper(pnts, bSet, pAdjustValue, 1, 21600, 0, pAdjustValue[0], 10800);
			rtnY = SetAdjustHandlePointHelper(pnts, bSet, pAdjustValue, 2, 0, 21600, 0, pAdjustValue[1]);
		}
		break;
	case ShapeType::QuadArrow: // 상하좌우 화살표
		if (idx == 0) {
			rtnX = SetAdjustHandlePointHelper(pnts, bSet, pAdjustValue, 0, 21600, 0, pAdjustValue[2], pAdjustValue[1]);
		}
		else if (idx == 1) {
			rtnX = SetAdjustHandlePointHelper(pnts, bSet, pAdjustValue, 1, 21600, 0, pAdjustValue[0], 10800);
			rtnY = SetAdjustHandlePointHelper(pnts, bSet, pAdjustValue, 2, 0, 21600, 0, pAdjustValue[0]);
		}
		break;
	case ShapeType::BentUpArrow: // 위로 굽은 화살표
	case ShapeType::LeftUpArrow: // 좌상 화살표
		if (idx == 0) {
			//				rtnX = SetAdjustHandlePointHelper(pnts, bSet, idx - 1, 21600, 0, 7200, pAdjustValue[1] - (21600 - pAdjustValue[1]));
			rtnX = SetAdjustHandlePointHelper(pnts, bSet, pAdjustValue, 0, 21600, 0, pAdjustValue[2], pAdjustValue[1] - (21600 - pAdjustValue[1]));
		}
		else if (idx == 1) {
			rtnX = SetAdjustHandlePointHelper(pnts, bSet, pAdjustValue, 1, 21600, 0, (21600 + pAdjustValue[0]) / 2, 21600);
			rtnY = SetAdjustHandlePointHelper(pnts, bSet, pAdjustValue, 2, 0, 21600, 0, pAdjustValue[0]);
		}
		break;
	case ShapeType::RightArrowCallout: // 오른쪽 화살표 설명선
		if (idx == 0) {
			rtnX = SetAdjustHandlePointHelper(pnts, bSet, pAdjustValue, 0, 21600, 0, 0, pAdjustValue[2]);
		}
		else if (idx == 1) {
			rtnY = SetAdjustHandlePointHelper(pnts, bSet, pAdjustValue, 1, 0, 21600, 0, pAdjustValue[3]);
		}
		else if (idx == 2) {
			rtnX = SetAdjustHandlePointHelper(pnts, bSet, pAdjustValue, 2, 21600, 0, pAdjustValue[0], 21600);
			rtnY = SetAdjustHandlePointHelper(pnts, bSet, pAdjustValue, 3, 0, 21600, pAdjustValue[1], 10800);
		}
		break;
	case ShapeType::LeftArrowCallout: // 왼쪽 화살표 설명선
		if (idx == 0) {
			rtnX = SetAdjustHandlePointHelper(pnts, bSet, pAdjustValue, 0, 21600, 0, pAdjustValue[2], 21600);
		}
		else if (idx == 1) {
			rtnY = SetAdjustHandlePointHelper(pnts, bSet, pAdjustValue, 1, 0, 21600, 0, pAdjustValue[3]);
		}
		else if (idx == 2) {
			rtnX = SetAdjustHandlePointHelper(pnts, bSet, pAdjustValue, 2, 21600, 0, 0, pAdjustValue[0]);
			rtnY = SetAdjustHandlePointHelper(pnts, bSet, pAdjustValue, 3, 0, 21600, pAdjustValue[1], 10800);
		}
		break;
	case ShapeType::UpArrowCallout: // 위쪽 화살표 설명선
		if (idx == 0) {
			rtnY = SetAdjustHandlePointHelper(pnts, bSet, pAdjustValue, 0, 0, 21600, pAdjustValue[2], 21600);
		}
		else if (idx == 1) {
			rtnX = SetAdjustHandlePointHelper(pnts, bSet, pAdjustValue, 1, 21600, 0, 0, pAdjustValue[3]);
		}
		else if (idx == 2) {
			rtnY = SetAdjustHandlePointHelper(pnts, bSet, pAdjustValue, 2, 0, 21600, 0, pAdjustValue[0]);
			rtnX = SetAdjustHandlePointHelper(pnts, bSet, pAdjustValue, 3, 21600, 0, pAdjustValue[1], 10800);
		}
		break;
	case ShapeType::DownArrowCallout: // 아래쪽 화살표 설명선
		if (idx == 0) {
			rtnY = SetAdjustHandlePointHelper(pnts, bSet, pAdjustValue, 0, 0, 21600, 0, pAdjustValue[2]);
		}
		else if (idx == 1) {
			rtnX = SetAdjustHandlePointHelper(pnts, bSet, pAdjustValue, 1, 21600, 0, 0, pAdjustValue[3]);
		}
		else if (idx == 2) {
			rtnY = SetAdjustHandlePointHelper(pnts, bSet, pAdjustValue, 2, 0, 21600, pAdjustValue[0], 21600);
			rtnX = SetAdjustHandlePointHelper(pnts, bSet, pAdjustValue, 3, 21600, 0, pAdjustValue[1], 10800);
		}
		break;
	case ShapeType::LeftRightArrowCallout: // 좌우 화살표 설명선
		if (idx == 0) {
			rtnX = SetAdjustHandlePointHelper(pnts, bSet, pAdjustValue, 0, 21600, 0, pAdjustValue[2], 10800);
		}
		else if (idx == 1) {
			rtnY = SetAdjustHandlePointHelper(pnts, bSet, pAdjustValue, 1, 0, 21600, 0, pAdjustValue[3]);
		}
		else if (idx == 2) {
			rtnX = SetAdjustHandlePointHelper(pnts, bSet, pAdjustValue, 2, 21600, 0, 0, pAdjustValue[0]);
			rtnY = SetAdjustHandlePointHelper(pnts, bSet, pAdjustValue, 3, 0, 21600, pAdjustValue[1], 10800);
		}
		break;
	case ShapeType::UpDownArrowCallout: // 상하 화살표 설명선
		if (idx == 0) {
			rtnY = SetAdjustHandlePointHelper(pnts, bSet, pAdjustValue, 0, 0, 21600, pAdjustValue[2], 10800);
		}
		else if (idx == 1) {
			rtnX = SetAdjustHandlePointHelper(pnts, bSet, pAdjustValue, 1, 21600, 0, 0, pAdjustValue[3]);
		}
		else if (idx == 2) {
			rtnY = SetAdjustHandlePointHelper(pnts, bSet, pAdjustValue, 2, 0, 21600, 0, pAdjustValue[0]);
			rtnX = SetAdjustHandlePointHelper(pnts, bSet, pAdjustValue, 3, 21600, 0, pAdjustValue[1], 10800);
		}
		break;
	case ShapeType::QuadArrowCallout: // 상하좌우 화살표 설명선
		if (idx == 0) {
			rtnY = SetAdjustHandlePointHelper(pnts, bSet, pAdjustValue, 0, 0, 21600, pAdjustValue[2], pAdjustValue[1]);
		}
		else if (idx == 1) {
			rtnX = SetAdjustHandlePointHelper(pnts, bSet, pAdjustValue, 1, 21600, 0, pAdjustValue[0], pAdjustValue[3]);
		}
		else if (idx == 2) {
			rtnY = SetAdjustHandlePointHelper(pnts, bSet, pAdjustValue, 2, 0, 21600, 0, pAdjustValue[0]);
			rtnX = SetAdjustHandlePointHelper(pnts, bSet, pAdjustValue, 3, 21600, 0, pAdjustValue[1], 10800);
		}
		break;
	default:
		break;
	}

	if (rtnX || rtnY) {
		rtnX = TRUE;
	}
	else {
		rtnX = FALSE;
	}
	return rtnX;
}


// 수평이동 조절점 : nXLog에 0이 아닌값, nYLog에 0
// 수직이동 조절점 : nYLog에 0이 아닌값, nXLog에 0
// 조절점 이동범위 : nMin 최소값, nMax 최대값
BOOL KShapeSmartRectObject::SetAdjustHandlePointHelper(LPPOINT pnts, BOOL bSet, LONG* pAdjustValue, int nAdjustIndex, int nXLog, int nYLog, int nMin, int nMax)
{
	BOOL bRet = FALSE;
	int nAdjust = 0;
	int nXDistancePnt = 0, nYDistancePnt = 0;

	if (Rect.right < Rect.left) {
		nXDistancePnt = Rect.left - pnts->x;
	}
	else {
		nXDistancePnt = pnts->x - Rect.left;
	}
	if (Rect.bottom < Rect.top) {
		nYDistancePnt = Rect.top - pnts->y;
	}
	else {
		nYDistancePnt = pnts->y - Rect.top;
	}

	if (nXLog != 0 && nYLog == 0) {
		nAdjust = LogicalAdjust(nXDistancePnt, nXLog, abs(Rect.right - Rect.left));
	}
	else if (nYLog != 0 && nXLog == 0) {
		nAdjust = LogicalAdjust(nYDistancePnt, nYLog, abs(Rect.bottom - Rect.top));
	}
	else {
		return FALSE;
	}
	ClipMinMaxValue(nAdjust, nMin, nMax);
	if (pAdjustValue[nAdjustIndex] != nAdjust)
	{
		if (bSet)
			pAdjustValue[nAdjustIndex] = nAdjust;
		bRet = TRUE;
	}
	return bRet;
}

void KShapeSmartRectObject::SetValidAdjust(LONG* pAdjustValue, int nAdjIndex, int nValue, BOOL bSet, BOOL& rtn)
{
	if (pAdjustValue[nAdjIndex] != nValue) {
		if (bSet)
			pAdjustValue[nAdjIndex] = nValue;
		rtn = TRUE;
	}
}


BOOL KShapeSmartRectObject::Move(POINT& offset)
{
	Rect.left += offset.x;
	Rect.top += offset.y;
	Rect.right += offset.x;
	Rect.bottom += offset.y;
	return TRUE;
}

BOOL KShapeSmartRectObject::HandleEdit(EditHandleType handleType, POINT& handle, int keyState)
{
	return FALSE;
}

EditHandleType KShapeSmartRectObject::HitObjectsHandle(KImageDrawer& drawer, POINT& pt)
{
  // check adjust handle
  POINT handles[6];
  int num = GetAdjustHandlePoint(handles, Rect, mAdjustValue);
  if (num > 0)
  {
    EditHandleType t = drawer.HitAdjustHandle(handles, num, pt);
    if (t != EditHandleType::HandleNone)
      return t;
  }

	return drawer.HitRectangleHandle(Rect, pt);
}

EditHandleType KShapeSmartRectObject::HitOver(KImageDrawer& drawer, POINT& pt)
{
	if (drawer.HitOver(Rect, ShapeType::ShapeRectangle, pt) > EditHandleType::HandleNone)
		//if (IsOver(Rect, pt))
		return EditHandleType::MoveObjectHandle;

	return EditHandleType::HandleNone;
}

BOOL KShapeSmartRectObject::HitOver(POINT& pt)
{
	int pen_half_width = Outline.Width / 2 + 1;
	if (((Rect.left - pen_half_width) <= pt.x) && (pt.x <= (Rect.right + pen_half_width)) &&
		((Rect.top - pen_half_width) <= pt.y) && (pt.y <= (Rect.bottom + pen_half_width)))
	{
		return TRUE;
	}
	return FALSE;
}

void KShapeSmartRectObject::GetRectangle(RECT& rect)
{
	rect = Rect;
}
void KShapeSmartRectObject::GetNormalRectangle(RECT& rect)
{ 
	rect = Rect;
	NormalizeRect(rect);
}

RECT KShapeSmartRectObject::GetBounds(KImageDrawer* drawer)
{
	RECT rect = Rect;
	NormalizeRect(rect);

	AdjustShapeBound(rect, mAdjustValue);

	int pen_hw = Outline.Width / 2 + 1;
	AddToBounds(rect, drawer, pen_hw);
	return rect;
}

RECT KShapeSmartRectObject::GetBounds(RECT& bound, KImageDrawer* drawer)
{
	RECT rect = bound;
	NormalizeRect(rect);
	AdjustShapeBound(rect, mAdjustValue);

	int pen_hw = Outline.Width / 2 + 1;
	AddToBounds(rect, drawer, pen_hw);
	return rect;
}

BOOL KShapeSmartRectObject::SetRectangle(RECT& rect)
{
	NormalizeRect(rect);
	if ((Rect.right != rect.right) || (Rect.bottom != rect.bottom) || (Rect.top != rect.top) || (Rect.left != rect.left))
	{
		memcpy(&Rect, &rect, sizeof(RECT));
		return TRUE;
	}
	return FALSE;
}


void KShapeSmartRectObject::AdjustShapeBound(RECT& rect, long* pAdjustValue)
{
	switch (mType)
	{
	case ShapeType::Callout1:					// = 41,
	case ShapeType::AccentCallout1:				// = 44,
	case ShapeType::BorderCallout1:				// = 47,
	case ShapeType::AccentBorderCallout1:		// = 50,
	case ShapeType::Callout2:					// = 42,
	case ShapeType::AccentCallout2:				// = 45,
	case ShapeType::BorderCallout2:				// = 48,
	case ShapeType::AccentBorderCallout2:		// = 51,
	case ShapeType::Callout90:					// = 178,
	case ShapeType::AccentCallout90:				// = 179,
	case ShapeType::BorderCallout90:				// = 180,
	case ShapeType::AccentBorderCallout90:		// = 181,
		if (pAdjustValue[1] < 0)
		{
			int height = rect.bottom - rect.top;
			rect.top += (int)((double)height * pAdjustValue[1] / 21600 + 0.5);
		}
		else if (pAdjustValue[1] > 21600)
		{
			int height = rect.bottom - rect.top;
			rect.bottom += (int)((double)height * (pAdjustValue[1] - 21600) / 21600 + 0.5);
		}
		if (pAdjustValue[0] < 0)
		{
			int width = rect.right - rect.left;
			rect.left += (int)((double)width * pAdjustValue[0] / 21600 + 0.5);
		}
		break;

	case ShapeType::Callout3:					// = 43,
	case ShapeType::AccentCallout3:				// = 46,
	case ShapeType::BorderCallout3:				// = 49,
	case ShapeType::AccentBorderCallout3:		// = 52,
		if (pAdjustValue[1] < 0)
		{
			int height = rect.bottom - rect.top;
			rect.top += (int)((double)height * pAdjustValue[1] / 21600 + 0.5);
		}
		else if (pAdjustValue[1] > 21600)
		{
			int height = rect.bottom - rect.top;
			rect.bottom += (int)((double)height * (pAdjustValue[1] - 21600) / 21600 + 0.5);
		}
		if (pAdjustValue[0] > 21600)
		{
			int width = rect.right - rect.left;
			rect.right += (int)((double)width * (pAdjustValue[0] - 21600) / 21600 + 0.5);
		}
		break;

	case ShapeType::WedgeRectCallout:			// = 61,
	case ShapeType::WedgeRRectCallout:			// = 62,
	case ShapeType::WedgeEllipseCallout:			// = 63,
	case ShapeType::CloudCallout:				// = 106,
		if (pAdjustValue[1] > 21600)
		{
			int height = rect.bottom - rect.top;
			rect.bottom += (LONG)((double)height * (pAdjustValue[1] - 21600) / 21600 + 0.5);
		}
		break;
	/*
	case ShapeType::LeftBracket:
	case ShapeType::RightBracket:
		obj->Shape.Coord.xs = MM2LOG(41);
		obj->Shape.Coord.xe = MM2LOG(59);
		break;
	case ShapeType::LeftBrace:
	case ShapeType::RightBrace:
		obj->Shape.Coord.xs = MM2LOG(31);
		obj->Shape.Coord.xe = MM2LOG(69);
		break;
	*/
	case ShapeType::Moon:
	{
		int width = rect.right - rect.left;
		if (pAdjustValue[0] < 0)
			rect.left += (int)((double)width * pAdjustValue[0] / 21600 + 0.5);
		else if (pAdjustValue[0] > 21600)
			rect.right += (int)((double)width * (pAdjustValue[0] - 21600) / 21600 + 0.5);
		break;
	}

	default:
		break;
	}
}


void KShapeSmartRectObject::drawShape(Graphics& g, KImageDrawer& info, RECT& r, int flag, LONG* pAdjustValue)
{
	ShapeDrawerHelper helper;

	helper.renderRect = r;
	helper.pInfo = &info;

	int childNumber = GetChildNumber();
	BOOL isEvenOdd = IsEvenOddFill();
	if (pAdjustValue == NULL)
		pAdjustValue = mAdjustValue;

	for (int i = 0; i < childNumber; i++)
	{
		LP_CPOINT lpPts = NULL;
		int pnum = 0;
		RECT r;

		BasicGraphElement e = GetChild(i, helper, &r, &lpPts, &pnum, pAdjustValue);
		helper.penBrushFlag = 0;

		if (HasPen(i))
			helper.penBrushFlag = DRAW_WITH_PEN;
		if (HasBrush(i))
			helper.penBrushFlag |= DRAW_WITH_BRUSH;

		if (e == BasicGraphElement::RectElement)
		{
			r.left += helper.renderRect.left;
			r.right += helper.renderRect.left;
			r.top += helper.renderRect.top;
			r.bottom += helper.renderRect.top;

			drawRectangleElement(g, helper, r, Fill, Outline);
		}
		else if (e == BasicGraphElement::RoundRectangle)
		{
			int roundW, roundH;

			r.left += helper.renderRect.left;
			r.right += helper.renderRect.left;
			r.top += helper.renderRect.top;
			r.bottom += helper.renderRect.top;

			double round = (double)pAdjustValue[0] / 10800;
			if (abs(r.right - r.left) > abs(r.bottom - r.top))
			{
				roundW = (int)(round * abs(r.bottom - r.top) + 0.5);
				roundH = roundW;
			}
			else
			{
				roundW = (int)(round * abs(r.right - r.left) + 0.5);
				roundH = roundW;
			}
			drawRoundRectangleElement(g, helper, r, roundW, roundH, Fill, Outline);
		}
		else if (e == BasicGraphElement::EllipseElement)
		{
			r.left += helper.renderRect.left;
			r.right += helper.renderRect.left;
			r.top += helper.renderRect.top;
			r.bottom += helper.renderRect.top;

			drawEllipseElement(g, helper, r, Fill, Outline);
		}
		else if (e == BasicGraphElement::PolygonElement)
		{
			if (lpPts != NULL)
			{
				drawPolygonElement(g, helper, lpPts, pnum, Fill, Outline, isEvenOdd);
				delete[] lpPts;
			}
			else
				OutputDebugString(_T("Empty Polygon\n"));
		}
	}

	if (IsSelected() && !(flag & DRAW_NO_HANDLE))
	{
		info.DrawBoundingHandle(g, helper.renderRect, 0);

		// draw smart handle
		POINT handles[6];
		int num = GetAdjustHandlePoint(handles, helper.renderRect, pAdjustValue);
		for (int i = 0; i < num; i++)
		{
			//info.ImageToScreen(handles[i]);

			info.DrawAdjustHandle(g, handles[i], flag);
		}
	}
}

void KShapeSmartRectObject::Draw(Graphics& g, KImageDrawer& info, int flag)
{
	RECT r = Rect;
	info.ImageToScreen(r);

	if (info.IsPaintArea(r))
		drawShape(g, info, r, flag);
}

void KShapeSmartRectObject::OnRenderRubber(Graphics& g, KDragAction* drag, KImageDrawer& info, int flag)
{
	RECT r;
	LONG adjustValue[SHAPE_ADJUST_NUMBER];

	memcpy(adjustValue, mAdjustValue, sizeof(LONG) * SHAPE_ADJUST_NUMBER);
	if (drag->mouseDragMode == MouseDragMode::HandleObject)
  {
		// TODO:: handle 이동에 따라 변경된 rect 계산
    // MoveAdjustHandle(drag->handleType, drag->dragAmount)
		// r = MoveAdjustHandle(drag->handleType, drag->dragAmount, adjustValue);
		int idx = (int)drag->handleType - (int)EditHandleType::ObjectHandle1;
		POINT dp[6];

 		GetAdjustHandlePoint(dp);
		dp[idx].x += drag->dragAmount.x;
		dp[idx].y += drag->dragAmount.y;

		SetAdjustHandlePoint(idx, &dp[idx], TRUE, adjustValue);

#ifdef _DEBUG
		TCHAR msg[120];
		StringCchPrintf(msg, 120, _T("AdjustHandle point=(%d, %d), dragAmount=(%d,%d), value=%d\n"), 
			dp[idx].x, dp[idx].y, drag->dragAmount.x, drag->dragAmount.y, adjustValue[idx]);
		OutputDebugString(msg);
#endif

		r = Rect;
  }
	else
	{
		r = drag->MakeResizedRect(Rect);
	}

	info.ImageToScreen(r);
	drawShape(g, info, r, flag, adjustValue);
}

void KShapeSmartRectObject::GetDragBounds(KDragAction* drag, int flag, RECT& rect)
{
	LONG adjustValue[SHAPE_ADJUST_NUMBER];

	memcpy(adjustValue, mAdjustValue, sizeof(LONG) * SHAPE_ADJUST_NUMBER);
	if (drag->mouseDragMode == MouseDragMode::HandleObject)
	{
		// TODO:: handle 이동에 따라 변경된 rect 계산
		int idx = (int)drag->handleType - (int)EditHandleType::ObjectHandle1;
		POINT dp[6];

		GetAdjustHandlePoint(dp);
		dp[idx].x += drag->dragAmount.x;
		dp[idx].y += drag->dragAmount.y;

		SetAdjustHandlePoint(idx, &dp[idx], TRUE, adjustValue);

#ifdef _DEBUG
		TCHAR msg[120];
		StringCchPrintf(msg, 120, _T("AdjustHandle point=(%d, %d), dragAmount=(%d,%d), value=%d\n"),
			dp[idx].x, dp[idx].y, drag->dragAmount.x, drag->dragAmount.y, adjustValue[idx]);
		OutputDebugString(msg);
#endif
	}

	rect = Rect;
	NormalizeRect(rect);

	AdjustShapeBound(rect, adjustValue);
}

void KShapeSmartRectObject::OnDragEnd(KDragAction* drag, POINT& amount)
{
	if (drag->mouseDragMode == MouseDragMode::HandleObject)
	{
		int idx = (int)drag->handleType - (int)EditHandleType::ObjectHandle1;
		POINT dp[8];

		GetAdjustHandlePoint(dp);
		dp[idx].x += drag->dragAmount.x;
		dp[idx].y += drag->dragAmount.y;

		SetAdjustHandlePoint(idx, &dp[idx], TRUE, mAdjustValue);
	}
	else
	{
		Rect = drag->MakeResizedRect(Rect, amount);
		NormalizeRect(Rect);
	}
}

void KShapeSmartRectObject::GetDragBounds(RECT& bound, KDragAction* drag)
{
	bound = drag->MakeResizedRect(Rect);
	NormalizeRect(bound);

	// add pen half width
	int pen_hw = Outline.Width / 2 + 1;
	if (pen_hw > 0)
	{
		bound.left -= pen_hw;
		bound.top -= pen_hw;
		bound.right += pen_hw;
		bound.bottom += pen_hw;
	}
}

#ifdef USE_XML_STORAGE
HRESULT KShapeSmartRectObject::StoreShapeData(CComPtr<IXmlWriter> pWriter)
{
	StoreRect(Rect, pWriter);
	StoreFill(Fill, pWriter);
	StoreOutline(Outline, pWriter);
	return S_OK;
}
#endif // USE_XML_STORAGE

BOOL KShapeSmartRectObject::LoadShapeDataFromBufferExt(KMemoryStream& mf)
{
	mf.Read(&Rect, sizeof(RECT));
	mf.Read(&Fill, sizeof(DWORD));
	mf.Read(&Outline, sizeof(OutlineStyle));
	return TRUE;
}

BOOL KShapeSmartRectObject::StoreShapeDataToBufferExt(KMemoryStream& mf)
{
	mf.Write(&Rect, sizeof(RECT));
	mf.Write(&Fill, sizeof(DWORD));
	mf.Write(&Outline, sizeof(OutlineStyle));
	return TRUE;
}


/*************************************************
KShapeSmartGroupObject
*************************************************/

CPOINT KShapeSmartGroupObject::_Can[] = 
{
	{ 0, 8800, CP_STAT | CP_LINE }, { 0, 9200, CP_CURV }, { 2239, 10000, CP_CURV },
		{ 5000, 10000, CP_CURV }, { 7761, 10000, CP_CURV }, { 10000, 9200, CP_CURV },
		{ 10000, 8800, CP_LINE }, { 10000, 1200, CP_LINE }, { 0, 1200, CP_LINE },
		{0, 8800, CP_LINE | CP_ENDS | CP_ENDG | CP_CLOSED },
	{ 0, 1200, CP_STAT | CP_CURV }, { 0, 800, CP_CURV }, { 2239, 0, CP_CURV }, { 5000, 0, CP_CURV },
		{ 7761, 0, CP_CURV }, { 10000, 800, CP_CURV }, { 10000, 1200, CP_CURV },
		{ 10000, 1600, CP_CURV }, { 7761, 2400, CP_CURV }, { 5000, 2400, CP_CURV },
		{ 2239, 2400, CP_CURV }, { 0, 1600, CP_CURV }, { 0, 1200, CP_CURV | CP_ENDS | CP_ENDG | CP_CLOSED } 
};

CPOINT KShapeSmartGroupObject::_FoldedCorner[] = 
{
	{ 0, 0, CP_STAT | CP_LINE }, { 10000, 0, CP_LINE }, { 10000, 8700, CP_LINE }, 
		{ 8700, 10000, CP_LINE }, { 0, 10000, CP_LINE }, { 0, 0, CP_LINE | CP_ENDS | CP_ENDG | CP_CLOSED },
	{ 10000, 5000, CP_STAT | CP_LINE },
	{ 5000, 10000, 0x2 },
	{ 6300, 5200, 0x2 },
	{ 6300, 5200, 0x4 },
	{ 6887, 6492, 0x4 },
	{ 10000, 5000, CP_LINE | CP_ENDS | CP_ENDG | CP_CLOSED },
};


CPOINT KShapeSmartGroupObject::_SmileMouse[] = 
{
	{ 2300, 7200, CP_STAT | CP_LINE }, { 4000, 8376, CP_CURV }, { 6000, 8376, CP_CURV }, { 7700, 7200, CP_LINE | CP_ENDS | CP_ENDG }
};

CPOINT KShapeSmartGroupObject::_NoSmoking[] = 
{
	{ 5000, 0, 0x03 }, { 7761,0,0x4 }, { 10000,2238,0x4 }, { 10000,5000,0x2 },
	{ 10000,7761,0x4 }, { 7761,10000,0x4 }, { 5000,10000,0x2 }, { 2238,10000,0x4 },
	{ 0,7761,0x4 }, { 0,5000,0x2 }, { 0,2238,0x4 }, { 2238,0,0x4 }, { 5000,0,0x200a },
	{ 2715,2114,0x3 }, { 3379,1581,0x4 }, { 4202,1312,0x4 }, { 5049,1300,0x2 },
	{ 7120,1300,0x4 }, { 8799,2978,0x4 }, { 8799,5049,0x2 }, { 8783,5897,0x4 },
	{ 8517,6717,0x4 }, { 7983,7383,0x2 }, { 2715,2114,0x200a },
	{ 7383,7983,0x3 }, { 6718,8518,0x4 }, { 5898,8784,0x4 }, { 5049,8800,0x2 },
	{ 2978,8800,0x4 }, { 1300,7120,0x4 }, { 1300,5049,0x2 }, { 1313,4203,0x4 },
	{ 1581,3379,0x4 }, { 2115,2715,0x2 }, { 7383,7983,0x201a },
};

CPOINT KShapeSmartGroupObject::_Heart[] =
{
	{ 5000, 1000, 0x1 },
	{ 5000, 1000, 0x4 },
	{ 6246, 0, 0x4 },
	{ 7500, 0, 0x4 },
	{ 9943, 0, 0x4 },
	{ 10030, 1799, 0x4 },
	{ 10000, 2400, 0x4 },
	{ 9992, 2830, 0x4 },
	{ 9858, 3193, 0x4 },
	{ 9600, 3600, 0x4 },
	{ 9591, 3623, 0x4 },
	{ 5000, 10000, 0x4 },
	{ 5000, 10000, 0x2 },
	{ 400, 3600, 0x2 },
	{ 100, 3170, 0x4 },
	{ 0, 2817, 0x4 },
	{ 0, 2400, 0x4 },
	{ 0, 1832, 0x4 },
	{ 0, 0, 0x4 },
	{ 2400, 0, 0x4 },
	{ 3857, 0, 0x4 },
	{ 5000, 1000, 0x4 },
	{ 5000, 1000, 0x201a },
};

CPOINT KShapeSmartGroupObject::_LightningBolt[] =
{
	{ 0, 1800, 0x1 },
	{ 3900, 0, 0x2 },
	{ 6000, 2800, 0x2 },
	{ 5200, 3200, 0x2 },
	{ 7700, 5600, 0x2 },
	{ 6900, 5900, 0x2 },
	{ 9999, 9999, 0x2 },
	{ 4600, 6900, 0x2 },
	{ 5700, 6400, 0x2 },
	{ 2300, 4500, 0x2 },
	{ 3500, 3900, 0x2 },
	{ 0, 1800, 0x201a },
};

CPOINT KShapeSmartGroupObject::_Sun[] = 
{
	{ 24, 4991, 0x1 },
	{ 2037, 4276, 0x2 },
	{ 2056, 5724, 0x2 },
	{ 24, 4991, 0x201a },
	{ 1472, 8509, 0x1 },
	{ 2413, 6609, 0x2 },
	{ 3410, 7587, 0x2 },
	{ 1472, 8509, 0x201a },
	{ 4294, 7963, 0x1 },
	{ 5724, 7963, 0x2 },
	{ 5009, 9995, 0x2 },
	{ 4294, 7963, 0x201a },
	{ 6590, 7587, 0x1 },
	{ 7587, 6609, 0x2 },
	{ 8528, 8509, 0x2 },
	{ 6590, 7587, 0x201a },
	{ 7963, 5724, 0x1 },
	{ 7945, 4257, 0x2 },
	{ 9995, 4972, 0x2 },
	{ 7963, 5724, 0x201a },
	{ 7606, 3373, 0x1 },
	{ 6609, 2413, 0x2 },
	{ 8547, 1472, 0x2 },
	{ 7606, 3373, 0x201a },
	{ 4991, 5, 0x1 },
	{ 5724, 2056, 0x2 },
	{ 4276, 2056, 0x2 },
	{ 4991, 5, 0x201a },
	{ 1472, 1491, 0x1 },
	{ 3429, 2394, 0x2 },
	{ 2432, 3410, 0x2 },
	{ 1472, 1491, 0x201a },
};

CPOINT KShapeSmartGroupObject::_UturnArrow[] =
{
	{ 9999, 3900, 0x3 },
	{ 8599, 3900, 0x2 },
	{ 8599, 1819, 0x4 },
	{ 7035, 0, 0x4 },
	{ 4300, 0, 0x2 },
	{ 2040, 12, 0x4 },
	{ 0, 1802, 0x4 },
	{ 0, 3900, 0x2 },
	{ 0, 9999, 0x2 },
	{ 2800, 9999, 0x2 },
	{ 2800, 3900, 0x2 },
	{ 2800, 3900, 0x4 },
	{ 2808, 2700, 0x4 },
	{ 4300, 2700, 0x2 },
	{ 5825, 2689, 0x4 },
	{ 5800, 3900, 0x4 },
	{ 5800, 3900, 0x2 },
	{ 5366, 3900, 0x4 },
	{ 4933, 3900, 0x4 },
	{ 4500, 3900, 0x2 },
	{ 7300, 6600, 0x2 },
	{ 9999, 3900, 0x201a },
};

CPOINT KShapeSmartGroupObject::_FlowChartDocument[] = 
{
	{ 0, 0, 0x3 },
	{ 10000, 0, 0x2 },
	{ 10000, 8000, 0x2 },
	{ 10000, 8000, 0x4 },
	{ 8138, 7744, 0x4 },
	{ 6000, 9000, 0x2 },
	{ 3737, 9977, 0x4 },
	{ 3452, 10000, 0x4 },
	{ 2700, 10000, 0x2 },
	{ 1919, 10000, 0x4 },
	{ 0, 9400, 0x4 },
	{ 0, 9400, 0x2 },
	{ 0, 0, 0x201a },
};

CPOINT KShapeSmartGroupObject::_FlowChartMultidocument[] = 
{
	{ 1250, 0, 0x3 },
	{ 10000, 0, 0x2 },
	{ 10000, 6640, 0x2 },
	{ 10000, 6640, 0x4 },
	{ 8370, 6590, 0x4 },
	{ 6500, 7470, 0x2 },
	{ 4520, 8280, 0x4 },
	{ 4250, 8300, 0x4 },
	{ 3590, 8300, 0x2 },
	{ 2910, 8300, 0x4 },
	{ 1250, 7800, 0x4 },
	{ 1250, 7800, 0x2 },
	{ 1250, 0, 0x201a },
};

CPOINT KShapeSmartGroupObject::_FlowChartSummingJunction[] = 
{

	{ 1460, 8530, 0x3 },
	{ -460, 6600, 0x4 },
	{ -430, 3370, 0x4 },
	{ 1460, 1460, 0x2 },
	{ 5000, 5000, 0x2 },
	{ 1460, 8530, CP_ENDS | CP_CLOSED },	
	{ 8530, 1460, 0x3 },
	{ 10470, 3400, 0x4 },
	{ 10440, 6630, 0x4 },
	{ 8530, 8530, 0x2 },
	{ 5000, 5000, 0x2 },
	{ 8530, 1460, CP_ENDS | CP_CLOSED },
	{ 1460, 1460, 0x3 },
	{ 3400, -460, 0x4 },
	{ 6630, -430, 0x4 },
	{ 8540, 1460, 0x2 },
	{ 5000, 5000, 0x2 },
	{ 1460, 1460, CP_ENDS | CP_CLOSED },
	{ 1460, 8530, 0x3 },
	{ 3400, 10470, 0x4 },
	{ 6630, 10440, 0x4 },
	{ 8540, 8530, 0x2 },
	{ 5000, 5000, 0x2 },
	{ 1460, 8530, CP_ENDS | CP_ENDG | CP_CLOSED },

};

CPOINT KShapeSmartGroupObject::_FlowChartOr[] = 
{
	{ 5000, 0, 0x3 },
	{ 7740, 0, 0x4 },
	{ 10000, 2300, 0x4 },
	{ 10000, 5000, 0x2 },
	{ 5000, 5000, 0x2 },
	{ 5000, 0, CP_ENDS | CP_CLOSED },
	{ 10000, 5000, 0x3 },
	{ 10000, 7730, 0x4 },
	{ 7690, 10000, 0x4 },
	{ 5000, 10000, 0x2 },
	{ 5000, 5000, 0x2 },
	{ 10000, 5000, CP_ENDS | CP_CLOSED },
	{ 0, 5000, 0x3 },
	{ 0, 7730, 0x4 },
	{ 2300, 10000, 0x4 },
	{ 5000, 10000, 0x2 },
	{ 5000, 5000, 0x2 },
	{ 0, 5000, CP_ENDS | CP_CLOSED },
	{ 0, 5000, 0x3 },
	{ 0, 2260, 0x4 },
	{ 2300, 0, 0x4 },
	{ 5000, 0, 0x2 },
	{ 5000, 5000, 0x2 },
	{ 0, 5000, CP_ENDS | CP_ENDG | CP_CLOSED },
};

CPOINT KShapeSmartGroupObject::_FlowChartMagneticTape[] = 
{
	{ 5000, 10000, 0x3 },
	{ 2240, 10000, 0x4 },
	{ 0, 7760, 0x4 },
	{ 0, 5000, 0x2 },
	{ 0, 2240, 0x4 },
	{ 2240, 0, 0x4 },
	{ 5000, 0, 0x2 },
	{ 7760, 0, 0x4 },
	{ 10000, 2240, 0x4 },
	{ 10000, 5000, 0x2 },
	{ 9980, 6270, 0x4 },
	{ 9520, 7460, 0x4 },
	{ 8660, 8400, 0x2 },
	{ 9700, 8400, 0x2 },
	{ 9700, 10000, 0x2 },
	{ 5000, 10000, 0x201a },
};

CPOINT KShapeSmartGroupObject::_BannerWave[] = 
{
	{ 0, 2100, 0x3 },
	{ 0, 2100, 0x4 },
	{ 750, 0, 0x4 },
	{ 2100, 0, 0x2 },
	{ 4180, 0, 0x4 },
	{ 5870, 4200, 0x4 },
	{ 7900, 4200, 0x2 },
	{ 9200, 4200, 0x4 },
	{ 10000, 2110, 0x4 },
	{ 10000, 2100, 0x2 },
	{ 10000, 8000, 0x2 },
	{ 10000, 8000, 0x4 },
	{ 9200, 10000, 0x4 },
	{ 7900, 10000, 0x2 },
	{ 5810, 10000, 0x4 },
	{ 4130, 5840, 0x4 },
	{ 2090, 5840, 0x2 },
	{ 880, 5840, 0x4 },
	{ 0, 7900, 0x4 },
	{ 0, 7900, 0x2 },
	{ 0, 2100, 0x201a },
};

CPOINT KShapeSmartGroupObject::_BannerDoubleWave[] = 
{
	{ 0, 1100, 0x3 },
	{ 0, 1100, 0x4 },
	{ 370, 0, 0x4 },
	{ 1100, 0, 0x2 },
	{ 1990, 0, 0x4 },
	{ 3070, 2110, 0x4 },
	{ 3900, 2100, 0x2 },
	{ 4990, 2110, 0x4 },
	{ 5020, 0, 0x4 },
	{ 6100, 0, 0x2 },
	{ 7030, 0, 0x4 },
	{ 8060, 2110, 0x4 },
	{ 8900, 2100, 0x2 },
	{ 9600, 2110, 0x4 },
	{ 10000, 1080, 0x4 },
	{ 10000, 1100, 0x2 },
	{ 10000, 9000, 0x2 },
	{ 10000, 9000, 0x4 },
	{ 9710, 10000, 0x4 },
	{ 8900, 10000, 0x2 },
	{ 8040, 10000, 0x4 },
	{ 6970, 7930, 0x4 },
	{ 6100, 7930, 0x2 },
	{ 4980, 7930, 0x4 },
	{ 5110, 10000, 0x4 },
	{ 3900, 10000, 0x2 },
	{ 2950, 10000, 0x4 },
	{ 2030, 7930, 0x4 },
	{ 1100, 7930, 0x2 },
	{ 380, 7930, 0x4 },
	{ 0, 9000, 0x4 },
	{ 0, 9000, 0x2 },
	{ 0, 1100, 0x201a },
};

CPOINT KShapeSmartGroupObject::_HorizontalScrollBanner2[] = 
{
	{ 1250, 7500, 0x3 },
	{ 1540, 7500, 0x4 },
	{ 1880, 7730, 0x4 },
	{ 1880, 8140, 0x2 },
	{ 1890, 8520, 0x4 },
	{ 1540, 8750, 0x4 },
	{ 1250, 8750, 0x2 },
	{ 2500, 8750, 0x2 },
	{ 2500, 10000, 0x2 },
	{ 1250, 10000, 0x2 },
	{ 560, 10000, 0x4 },
	{ 0, 9440, 0x4 },
	{ 0, 8750, 0x2 },
	{ 0, 8060, 0x4 },
	{ 570, 7500, 0x4 },
	{ 1250, 7500, 0x201a },
};

CPOINT KShapeSmartGroupObject::_HorizontalScrollBanner4[] = 
{
	{ 3760, 1240, 0x3 },
	{ 5000, 1240, 0x2 },
	{ 5000, 2520, 0x2 },
	{ 3760, 2520, 0x2 },
	{ 3410, 2520, 0x4 },
	{ 3120, 2240, 0x4 },
	{ 3120, 1880, 0x2 },
	{ 3130, 1490, 0x4 },
	{ 3410, 1240, 0x4 },
	{ 3760, 1240, 0x201a },
};

KShapeSmartGroupObject::KShapeSmartGroupObject(ShapeType type)
	:KShapeSmartRectObject(type)
{
}

KShapeSmartGroupObject::KShapeSmartGroupObject(ShapeType type, RECT& rect, DWORD fillcolor, OutlineStyle linestyle, LONG* adjust)
: KShapeSmartRectObject(type, rect, fillcolor, linestyle, adjust)
{ 
}

KShapeSmartGroupObject::~KShapeSmartGroupObject() 
{
}

BOOL KShapeSmartGroupObject::IsEvenOddFill()
{
	switch (mType)
		{
		case ShapeType::FlowChartMultidocument:		// = 115,
		case ShapeType::FlowChartMagneticDisk:
		case ShapeType::FlowChartMagneticDrum:
			return(FALSE);
		default:
			break;
		}
	return(TRUE);
}

BOOL KShapeSmartGroupObject::HasBrush(int child)
{
	if ((mType == ShapeType::SmileyFace) && (child == 3))
		return FALSE;
	if ((mType == ShapeType::BracketPair) || (mType == ShapeType::BracePair) ||
		(mType == ShapeType::LeftBracket) || (mType == ShapeType::RightBracket) ||
		(mType == ShapeType::LeftBrace) || (mType == ShapeType::RightBrace) ||
		(mType == ShapeType::Arc))
		return FALSE;

	return TRUE;
}

int KShapeSmartGroupObject::GetChildNumber()
{
	switch (mType)
		{
		case ShapeType::Cube:
			return(3);
		case ShapeType::Bevel:
			return(5);
		case ShapeType::Arc:
			return(1);
		case ShapeType::Can:							// = 22,
			//TRACE("Can mAdjustValue[0]=%d", mAdjustValue[0]);
			return(2);
		case ShapeType::FoldedCorner:				// = 65,
			//TRACE("FoldedCorner mAdjustValue[0]=%d", mAdjustValue[0]);
			return(2);
		case ShapeType::SmileyFace:					// = 96,
			//TRACE("SmileyFace mAdjustValue[0]=%d", mAdjustValue[0]);
			return(4);
		case ShapeType::Donut:						// = 23,
			//TRACE("Donut mAdjustValue[0]=%d", mAdjustValue[0]);
			return(1);
		case ShapeType::NoSmoking:					// = 57,
			//TRACE("NoSmoking mAdjustValue[0]=%d", mAdjustValue[0]);
			return(1);
		case ShapeType::BlockArc:					// = 95,
			{
			int angle = (int) ((mAdjustValue[0] >> 16) & 0x00FFFF);
			//TRACE("BlockArc mAdjustValue[0]=%x,mAdjustValue[1]=%d,angle=%d", mAdjustValue[0], mAdjustValue[1],angle);
			return(1);
			}
		case ShapeType::Heart:						// = 74,
			//TRACE("Heart mAdjustValue[0]=%d", mAdjustValue[0]);
			return(1);
		case ShapeType::LightningBolt:				// = 73,
			return(1);
		case ShapeType::Sun:							// = 183,
			//TRACE("Sun mAdjustValue[0]=%d", mAdjustValue[0]);
			return(1);
		case ShapeType::Moon:						// = 184,
			//TRACE("Moon mAdjustValue[0]=%d", mAdjustValue[0]);
			return(1);
		case ShapeType::BracketPair:					// = 185,
			//TRACE("BracketPair mAdjustValue[0]=%d", mAdjustValue[0]);
			return(1);
		case ShapeType::BracePair:					// = 186,
			//TRACE("BracePair mAdjustValue[0]=%d", mAdjustValue[0]);
			return(1);
		case ShapeType::Plaque:						// = 21,
			//TRACE("Plaque mAdjustValue[0]=%d", mAdjustValue[0]);
			return(1);
		case ShapeType::LeftBracket:					// = 85,
			//TRACE("LeftBracket mAdjustValue[0]=%d", mAdjustValue[0]);
			return(1);
		case ShapeType::RightBracket:				// = 86,
			//TRACE("RightBracket mAdjustValue[0]=%d", mAdjustValue[0]);
			return(1);
		case ShapeType::LeftBrace:					// = 87,
			//TRACE("LeftBrace mAdjustValue[0]=%d,mAdjustValue[1]=%d", mAdjustValue[0],mAdjustValue[1]);
			return(1);
		case ShapeType::RightBrace:					// = 88,
			//TRACE("RightBrace mAdjustValue[0]=%d,mAdjustValue[1]=%d", mAdjustValue[0],mAdjustValue[1]);
			return(1);

		case ShapeType::BentArrow:					// = 91,
			//TRACE("BentArrow mAdjustValue[0]=%d,mAdjustValue[1]=%d", mAdjustValue[0],mAdjustValue[1]);
			return(1);
		case ShapeType::UturnArrow:					// = 101,
			//TRACE("UturnArrow mAdjustValue[0]=%d", mAdjustValue[0]);
			return(1);
		case ShapeType::CurvedRightArrow:			// = 102,
			//TRACE("CurvedRightArrow mAdjustValue[0]=%d,mAdjustValue[1]=%d,mAdjustValue[2]=%d", mAdjustValue[0],mAdjustValue[1],mAdjustValue[2]);
			return(2);
		case ShapeType::CurvedLeftArrow:				// = 103,
			//TRACE("CurvedLeftArrow mAdjustValue[0]=%d,mAdjustValue[1]=%d,mAdjustValue[2]=%d", mAdjustValue[0],mAdjustValue[1],mAdjustValue[2]);
			return(2);
		case ShapeType::CurvedUpArrow:				// = 104,
			//TRACE("CurvedUpArrow mAdjustValue[0]=%d,mAdjustValue[1]=%d,mAdjustValue[2]=%d", mAdjustValue[0],mAdjustValue[1],mAdjustValue[2]);
			return(2);
		case ShapeType::CurvedDownArrow:				// = 105,
			//TRACE("CurvedDownArrow mAdjustValue[0]=%d,mAdjustValue[1]=%d,mAdjustValue[2]=%d", mAdjustValue[0],mAdjustValue[1],mAdjustValue[2]);
			return(2);
		case ShapeType::StripedRightArrow:			// = 93,
			//TRACE("StripedRightArrow mAdjustValue[0]=%d,mAdjustValue[1]=%d,mAdjustValue[2]=%d", mAdjustValue[0],mAdjustValue[1],mAdjustValue[2]);
			return(1);
		case ShapeType::CircularArrow:				// = 99,
			//TRACE("CircularArrow mAdjustValue[0]=%d,mAdjustValue[1]=%d,mAdjustValue[2]=%d", mAdjustValue[0],mAdjustValue[1],mAdjustValue[2]);
			return(1);

		case ShapeType::Ribbon2:
			//TRACE("ShapeType::Ribbon2 mAdjustValue[0]=%d,mAdjustValue[1]=%d,mAdjustValue[2]=%d", mAdjustValue[0],mAdjustValue[1],mAdjustValue[2]);
			return(5);
		case ShapeType::Ribbon:
			//TRACE("ShapeType::Ribbon mAdjustValue[0]=%d,mAdjustValue[1]=%d,mAdjustValue[2]=%d", mAdjustValue[0],mAdjustValue[1],mAdjustValue[2]);
			return(5);
		case ShapeType::EllipseRibbon2:
			//TRACE("ShapeType::EllipseRibbon2 mAdjustValue[0]=%d,mAdjustValue[1]=%d,mAdjustValue[2]=%d", mAdjustValue[0],mAdjustValue[1],mAdjustValue[2]);
			return(5);
		case ShapeType::EllipseRibbon:
			//TRACE("ShapeType::EllipseRibbon mAdjustValue[0]=%d,mAdjustValue[1]=%d,mAdjustValue[2]=%d", mAdjustValue[0],mAdjustValue[1],mAdjustValue[2]);
			return(5);
		case ShapeType::VerticalScroll:
			//TRACE("ShapeType::VerticalScroll mAdjustValue[0]=%d", mAdjustValue[0]);
			return(5);
		case ShapeType::HorizontalScroll:
			//TRACE("ShapeType::HorizontalScroll mAdjustValue[0]=%d", mAdjustValue[0]);
			return(5);
		case ShapeType::Wave:
			//TRACE("ShapeType::Wave mAdjustValue[0]=%d,mAdjustValue[1]=%d", mAdjustValue[0],mAdjustValue[1]);
			return(1);
		case ShapeType::DoubleWave:
			//TRACE("ShapeType::DoubleWave mAdjustValue[0]=%d,mAdjustValue[1]=%d", mAdjustValue[0],mAdjustValue[1]);
			return(1);
		case ShapeType::FlowChartDocument:			// = 114,
		case ShapeType::FlowChartMultidocument:		// = 115,
		case ShapeType::FlowChartTerminator:			// = 116,
		case ShapeType::FlowChartPunchedTape:		// = 122,
		case ShapeType::FlowChartSummingJunction:	// = 123,
		case ShapeType::FlowChartOr:					// = 124,
		case ShapeType::FlowChartOfflineStorage:		// = 129,
		case ShapeType::FlowChartOnlineStorage:		// = 130,
		case ShapeType::FlowChartMagneticTape:		// = 131,				
		case ShapeType::FlowChartDisplay:			// = 134,
		case ShapeType::FlowChartDelay:				// = 135,
			return(1);
		case ShapeType::FlowChartMagneticDrum:		// = 133
		case ShapeType::FlowChartMagneticDisk:		// = 132
			return(2);

		default:
			return(0);
		}

	return(0);
}

BOOL KShapeSmartGroupObject::IsColorChange()
{
	switch (mType)
		{
		case ShapeType::Cube:
		case ShapeType::Bevel:
		case ShapeType::Can:
		case ShapeType::FoldedCorner:
		case ShapeType::SmileyFace:
		case ShapeType::CurvedRightArrow:
		case ShapeType::CurvedLeftArrow:
		case ShapeType::CurvedUpArrow:
		case ShapeType::CurvedDownArrow:
		case ShapeType::Ribbon2:
		case ShapeType::Ribbon:
		case ShapeType::EllipseRibbon2:
		case ShapeType::EllipseRibbon:
		case ShapeType::VerticalScroll:
		case ShapeType::HorizontalScroll:
			return(TRUE);
		default:
			return(FALSE);
		}
	return(FALSE);
}

static int MakeCanSegment(int seg, LP_CPOINT lpData, int depth, int width, int height)
{
	if(seg == 0) { // 윗부분의 가로선이 생성되지 않게 수정
		MakeRoundCorner(&(lpData[0]), 0, depth, 5000, depth*2, width, height, FALSE, FALSE);
		MakeRoundCorner(&(lpData[3]), 5000, depth*2, 10000, depth, width, height, TRUE, FALSE);
		lpData[0].a = CP_STAT | CP_LINE;
		lpData[5].a = CP_LINE;
		lpData[6].x = width;
		lpData[6].y = (LONG)( (double)height * depth / 10000 );
		lpData[6].a = CP_LINE;

		MakeRoundCorner(&(lpData[7]), 10000, 10000-depth, 5000, 10000, width, height, FALSE, FALSE);
		MakeRoundCorner(&(lpData[10]), 5000, 10000, 0, 10000-depth, width, height, TRUE, TRUE);
		
		lpData[13].a = CP_LINE;
		lpData[14] = lpData[0];
		lpData[14].a = CP_LINE | CP_ENDS | CP_CLOSED;
		return(15);
	} else if(seg == 1) {
		MakeRoundCorner(&(lpData[0]), 0, depth, 5000, 0, width, height, FALSE, FALSE);
		MakeRoundCorner(&(lpData[3]), 5000, 0, 10000, depth, width, height, TRUE, FALSE);
		MakeRoundCorner(&(lpData[6]), 10000, depth, 5000, depth*2, width, height, FALSE, FALSE);
		MakeRoundCorner(&(lpData[9]), 5000, depth*2, 0, depth, width, height, TRUE, TRUE);
		lpData[0].a = CP_STAT | CP_LINE;
		lpData[12].a = CP_LINE | CP_ENDS | CP_CLOSED; // MakeRoundCorner로 생성되는 마지막 점 12번
		return(13);
	}
	return 0;
}

/*
BasicElementColorType KShapeSmartGroupObject::GetColorBright(int child)
{
	switch (child)
		{
		case ShapeType::Cube:  
			if(child == 1) {
				return BasicElementColorType::HIGH_BRIGHT;
			} else if(child == 2) {
				return BasicElementColorType::LOW_MEDIUM_BRIGHT;
			}
			break;
		case ShapeType::Bevel:
			if(child == 0) {
				return BasicElementColorType::HIGH_MEDIUM_BRIGHT;
			} else if(child == 1) {
				return BasicElementColorType::HIGH_BRIGHT;
			} else if(child == 2) {
				return BasicElementColorType::LOW_MEDIUM_BRIGHT;
			} else if(child == 3) {
				return BasicElementColorType::LOW_BRIGHT;
			}
			break;
		case ShapeType::Can:							// = 22,
			if(child == 1)
				return BasicElementColorType::HIGH_BRIGHT;
			break;
		case ShapeType::FoldedCorner:				// = 65,
			if(child == 1)
				return BasicElementColorType::LOW_MEDIUM_BRIGHT;
			break;
		case ShapeType::SmileyFace:					// = 96,
			if((child == 1) || (child == 2))
				return BasicElementColorType::LOW_MEDIUM_BRIGHT;
			break;
		case ShapeType::CurvedRightArrow:
		case ShapeType::CurvedDownArrow:
			if(child == 0)
				return BasicElementColorType::LOW_MEDIUM_BRIGHT;
			break;
		case ShapeType::CurvedLeftArrow:
		case ShapeType::CurvedUpArrow:
			if(child == 1)
				return BasicElementColorType::LOW_MEDIUM_BRIGHT;
			break;
		case ShapeType::Ribbon2:
		case ShapeType::Ribbon:
			if((child == 3) || (child == 4))
				return BasicElementColorType::LOW_MEDIUM_BRIGHT;
			break;
		case ShapeType::EllipseRibbon2:
		case ShapeType::EllipseRibbon:
			if((child == 2) || (child == 3))
				return BasicElementColorType::LOW_MEDIUM_BRIGHT;
			break;
		case ShapeType::VerticalScroll:
		case ShapeType::HorizontalScroll:
			if((child == 1) || (child == 3))
				return BasicElementColorType::LOW_MEDIUM_BRIGHT;
		default:
			break;
		}
	return BasicElementColorType::NORMAL;
}
*/

static void RotateCurvePoint(int pnum, LP_CPOINT pn, short Value, LPPOINT origin)
{
	double			CosTh, SinTh;
	int				i;
	POINT			pos, npt;

	if(Value != 0)
		{
		CosTh = cos(ANGLE_TO_RADIAN(Value));
		SinTh = sin(ANGLE_TO_RADIAN(Value));

		for (i=0 ; i<pnum ; i++)
			{
			pos.x = pn[i].x - origin->x;
			pos.y = pn[i].y - origin->y;
			npt.x = (LONG) (pos.x * CosTh + pos.y * SinTh + 0.5);
			npt.y = (LONG) (-pos.x * SinTh + pos.y * CosTh + 0.5);
			pn[i].x = origin->x + npt.x;
			pn[i].y = origin->y + npt.y;
			}
		}
}
// 원호가 cood영역에 가득차도록 fullrect를 계산함.

#if 0
BOOL KShapeSmartGroupObject::CalculateFullEllipseByLogicRect(LP_OBJECT obj, LP_RECTANGLE cood, LP_RECTANGLE fullrect)
{
	int diff, baseline;
	double ratio;

	if(obj->Shape.Coord.xe == obj->Shape.Coord.xs)
	{
		fullrect->xs = cood->xs;
		fullrect->xe = cood->xe;
	}
	else
	{
		if( (cood->xs != obj->Shape.Coord.xs) && (cood->xe != obj->Shape.Coord.xe) )
		{
			diff = ABS_V((cood->xs - obj->Shape.Coord.xs)) + ABS_V((cood->xe - obj->Shape.Coord.xe));
			baseline = obj->Shape.Coord.xs + (obj->Shape.Coord.xe - obj->Shape.Coord.xs) * ABS_V((cood->xs - obj->Shape.Coord.xs)) / diff;
			if(baseline != obj->Shape.Coord.xs)
			{
				ratio = (double) (cood->xs - obj->Shape.Coord.xs) / (baseline - obj->Shape.Coord.xs);
				fullrect->xs = Rect.left + (int) (ratio * (baseline - Rect.left));
			}
			else
				fullrect->xs = Rect.left;
			if(obj->Shape.Coord.xe != baseline)
			{
				ratio = (double) (cood->xe - obj->Shape.Coord.xe) / (obj->Shape.Coord.xe - baseline);
				fullrect->xe = Rect.right + (int) (ratio * (Rect.right - baseline));
			}
			else
				fullrect->xe = Rect.right;
		}
		else if(cood->xs != obj->Shape.Coord.xs)
		{
			ratio = (double) (cood->xs - obj->Shape.Coord.xs) / (obj->Shape.Coord.xe - obj->Shape.Coord.xs);
			fullrect->xs = Rect.left + (int) (ratio * (obj->Shape.Coord.xe - Rect.left));
			fullrect->xe = Rect.right - (int) (ratio * (Rect.right - obj->Shape.Coord.xe));
		}
		else if(cood->xe != obj->Shape.Coord.xe)
		{
			ratio = (double) (cood->xe - obj->Shape.Coord.xe) / (obj->Shape.Coord.xe - obj->Shape.Coord.xs);
			fullrect->xe = Rect.right + (int) (ratio * (Rect.right - obj->Shape.Coord.xs));
			fullrect->xs = Rect.left - (int) (ratio * (obj->Shape.Coord.xs - Rect.left));
		}
		else
		{
			fullrect->xs = Rect.left;
			fullrect->xe = Rect.right;
		}
	}

	if(obj->Shape.Coord.ye == obj->Shape.Coord.ys)
	{
		fullrect->ys = cood->ys;
		fullrect->ye = cood->ye;
	}
	else
	{
		if( (cood->ys != obj->Shape.Coord.ys) && (cood->ye != obj->Shape.Coord.ye) )
		{
			diff = ABS((cood->ys - obj->Shape.Coord.ys)) + ABS((cood->ye - obj->Shape.Coord.ye));
			baseline = obj->Shape.Coord.ys + (obj->Shape.Coord.ye - obj->Shape.Coord.ys) * ABS((cood->ys - obj->Shape.Coord.ys)) / diff;
			if(baseline != obj->Shape.Coord.ys)
			{
				ratio = (double) (cood->ys - obj->Shape.Coord.ys) / (baseline - obj->Shape.Coord.ys);
				fullrect->ys = Rect.top + (int) (ratio * (baseline - Rect.top));
			}
			else
				fullrect->ys = Rect.top;
			if(obj->Shape.Coord.ye != baseline)
			{
				ratio = (double) (cood->ye - obj->Shape.Coord.ye) / (obj->Shape.Coord.ye - baseline);
				fullrect->ye = Rect.bottom + (int) (ratio * (Rect.bottom - baseline));
			}
			else
				fullrect->ye = Rect.bottom;
		}
		else if(cood->ys != obj->Shape.Coord.ys)
		{
			ratio = (double) (cood->ys - obj->Shape.Coord.ys) / (obj->Shape.Coord.ye - obj->Shape.Coord.ys);
			fullrect->ys = Rect.top + (int) (ratio * (obj->Shape.Coord.ye - Rect.top));
			fullrect->ye = Rect.bottom - (int) (ratio * (Rect.bottom - obj->Shape.Coord.ye));
		}
		else if(cood->ye != obj->Shape.Coord.ye)
		{
			ratio = (double) (cood->ye - obj->Shape.Coord.ye) / (obj->Shape.Coord.ye - obj->Shape.Coord.ys);
			fullrect->ys = Rect.top - (int) (ratio * (obj->Shape.Coord.ys - Rect.top));
			fullrect->ye = Rect.bottom + (int) (ratio * (Rect.bottom - obj->Shape.Coord.ys));
		}
		else
		{
			fullrect->ys = Rect.top;
			fullrect->ye = Rect.bottom;
		}
	}
	return TRUE;
}
#endif

BasicGraphElement KShapeSmartGroupObject::GetChild(int child, ShapeDrawerHelper& helper, LPRECT rect, CPOINT** lppPts, int* pnum, LONG* pAdjustValue)
{
	int		width = (helper.renderRect.right - helper.renderRect.left) - 1;
	int		height = (helper.renderRect.bottom - helper.renderRect.top) - 1;
	BasicGraphElement	objtype = BasicGraphElement::None;
	CPOINT	*lpData = NULL;

	*pnum = 0;
	helper.colorBright = BasicElementColorType::NORMAL;

	switch (mType)
		{
		int i, halfHeight;
		
		case ShapeType::Cube:						// = 16,
			{
				if(child < 0 || child > 2)
					break;
				lpData = new CPOINT[5];
				int AdjValueH = (LONG)( (double)height * pAdjustValue[0] / MAX_ADJUSTVALUE );
				int AdjValueW = (LONG)( (double)width * pAdjustValue[0] / MAX_ADJUSTVALUE );
				int aV = 0;
				int aW = abs(width);
				int aH = abs(height);
				
				if(abs(AdjValueW) > abs(AdjValueH)) { // 조절점이 위/아래에 위치, 값은 Height에 비례
					aV = abs(AdjValueH);
				} else {
					aV = abs(AdjValueW);
				}

				if(child == 0) {
					lpData[0].x = lpData[1].x = 0;				
					lpData[2].x = lpData[3].x = aW - aV;
					lpData[0].y = lpData[3].y = aV;
					lpData[1].y = lpData[2].y = aH;
				} else if(child == 1) {
					lpData[0].x = aV;
					lpData[1].x = 0;
					lpData[2].x = aW - aV;
					lpData[3].x = aW;
					lpData[0].y = lpData[3].y = 0;
					lpData[1].y = lpData[2].y = aV;

					helper.colorBright = BasicElementColorType::HIGH_BRIGHT;
				} else if(child == 2) {
					lpData[0].x = lpData[1].x = aW - aV;				
					lpData[2].x = lpData[3].x = aW;				
					lpData[0].y = aV;
					lpData[1].y = aH;
					lpData[2].y = aH - aV;
					lpData[3].y = 0;

					helper.colorBright = BasicElementColorType::LOW_MEDIUM_BRIGHT;
				}
				lpData[4] = lpData[0];
				if(width < 0) { // y축 대칭
					for(i = 0; i < 5; i++)
						lpData[i].x *= -1;
				}
				if(height < 0) { // x축 대칭
					for(i = 0; i < 5; i++)
						lpData[i].y *= -1;
				}
				*pnum = 5;
				for(i = 0; i < 4; i++)
					lpData[i].a = CP_LINE;
				lpData[4].a = CP_LINE | CP_ENDS | CP_ENDG | CP_CLOSED;
				objtype = BasicGraphElement::PolygonElement;
			}
			break; 
		case ShapeType::Bevel:
			{
				if(child < 0 || child > 4)
					break;
				
				lpData = new CPOINT[5];
				int AdjValueH = (LONG)( (double)height * pAdjustValue[0] / MAX_ADJUSTVALUE );
				int AdjValueW = (LONG)( (double)width * pAdjustValue[0] / MAX_ADJUSTVALUE );
				int aV = 0;
				int aW = abs(width);
				int aH = abs(height);
				
				if(abs(AdjValueW) > abs(AdjValueH)) { // 조절점이 위/아래에 위치, 값은 Height에 비례
					aV = abs(AdjValueH);
				} else {
					aV = abs(AdjValueW);
				}

				if(child == 0) {
					lpData[0].x = 0;
					lpData[1].x = aV;
					lpData[2].x = aW - aV;
					lpData[3].x = aW;
					lpData[0].y = lpData[3].y = 0;
					lpData[1].y = lpData[2].y = aV;
					helper.colorBright = BasicElementColorType::HIGH_MEDIUM_BRIGHT;
				} else if(child == 1) {
					lpData[0].x = lpData[3].x = 0;
					lpData[1].x = lpData[2].x = aV;
					lpData[0].y = 0;
					lpData[1].y = aV;
					lpData[2].y = aH - aV;
					lpData[3].y = aH;
					helper.colorBright = BasicElementColorType::HIGH_BRIGHT;
				} else if(child == 2) {
					lpData[0].x = 0;
					lpData[1].x = aV;
					lpData[2].x = aW - aV;
					lpData[3].x = aW;
					lpData[0].y = lpData[3].y = aH;
					lpData[1].y = lpData[2].y = aH - aV;					
					helper.colorBright = BasicElementColorType::LOW_MEDIUM_BRIGHT;
				} else if(child == 3) {
					lpData[0].x = lpData[3].x = aW;
					lpData[1].x = lpData[2].x = aW - aV;
					lpData[0].y = 0;
					lpData[1].y = aV;
					lpData[2].y = aH - aV;
					lpData[3].y = aH;
					helper.colorBright = BasicElementColorType::LOW_BRIGHT;
				} else if(child == 4) {
					lpData[0].x = lpData[1].x = aV;					
					lpData[2].x = lpData[3].x = aW - aV;					
					lpData[0].y = lpData[3].y = aV;
					lpData[1].y = lpData[2].y = aH - aV;
				}
				lpData[4] = lpData[0];
				if(width < 0) { // y축 대칭
					for(i = 0; i < 5; i++)
						lpData[i].x *= -1;
				}
				if(height < 0) { // x축 대칭
					for(i = 0; i < 5; i++)
						lpData[i].y *= -1;
				}
				*pnum = 5;
				for(i = 0; i < 4; i++)
					lpData[i].a = CP_LINE;
				lpData[4].a = CP_LINE | CP_ENDS | CP_ENDG | CP_CLOSED;
				objtype = BasicGraphElement::PolygonElement;
			}
			break;

		case ShapeType::Can:							// = 22,
			halfHeight = (int)((double)(10000 / 2) * pAdjustValue[0] / 21600 + 0.5); // 뚜껑 원의 세로 절반
			if(child == 0) {
				lpData = new CPOINT[15];

				*pnum = MakeCanSegment(0, lpData, halfHeight, width, height);
				lpData[*pnum-1].a = CP_LINE | CP_ENDS | CP_ENDG | CP_CLOSED;
				objtype = BasicGraphElement::PolygonElement;
			} else if(child == 1) {
				lpData = new CPOINT[13];
				*pnum = MakeCanSegment(1, lpData, halfHeight, width, height);
				lpData[*pnum-1].a = CP_LINE | CP_ENDS | CP_ENDG | CP_CLOSED;
				objtype = BasicGraphElement::PolygonElement;
				helper.colorBright = BasicElementColorType::HIGH_BRIGHT;
			}
			break;
		case ShapeType::FoldedCorner:				// = 65,
			halfHeight = (int)((double)(21600 - pAdjustValue[0]) * 5000 / 10800 + 0.5);
			if(halfHeight < 0)
				halfHeight = 0;
			if(halfHeight > 5000)
				halfHeight = 5000;
			if(child == 0) // 외곽 사각형
				{ 
				lpData = new CPOINT[6];
				// 조절점에 영향을 받는 원소만 수정
				_FoldedCorner[2].y = 10000 - halfHeight;
				_FoldedCorner[3].x = 10000 - halfHeight;

				convertPoints(lpData, _FoldedCorner, 6, width, height);
				*pnum = 6;
				objtype = BasicGraphElement::PolygonElement;
				}
			else if(child == 1) // 접히는 부분
				{
				_FoldedCorner[6].y = 10000 - halfHeight;
				_FoldedCorner[7].x = 10000 - halfHeight;
				_FoldedCorner[8].x = 10000 - (int)((double)halfHeight * 3700 / 5000 + 0.5);
				_FoldedCorner[8].y = 10000 - (int)((double)halfHeight * 4800 / 5000);
				_FoldedCorner[9].x = 10000 - (int)((double)halfHeight * 3700 / 5000);
				_FoldedCorner[9].y = 10000 - (int)((double)halfHeight * 4800 / 5000);
				_FoldedCorner[10].x = 10000 - (int)((double)halfHeight * (10000 - 6887) / 5000);
				_FoldedCorner[10].y = 10000 - (int)((double)halfHeight * (10000 - 6492) / 5000);
				_FoldedCorner[11].y = 10000 - halfHeight;

				lpData = new CPOINT[6];
				// 6번 지점 이후 변환
				convertPoints(lpData, &(_FoldedCorner[6]), 6, width, height);
				*pnum = 6;
				objtype = BasicGraphElement::PolygonElement;
				helper.colorBright = BasicElementColorType::LOW_MEDIUM_BRIGHT;
				}
			break;
		case ShapeType::SmileyFace:					// = 96,
			if(child == 0)
			{
				*pnum = 0;
				objtype = BasicGraphElement::EllipseElement;
				rect->left = rect->top = 0;
				rect->right = width;
				rect->bottom = height;
			}
			else if(child == 1)
			{
				*pnum = 0;
				objtype = BasicGraphElement::EllipseElement;
				rect->left = (int) (width * 0.3);
				rect->top = (int) (height * 0.3);
				rect->right = rect->left + (int) (width * 0.1);
				rect->bottom = rect->top + (int) (height * 0.1);
				helper.colorBright = BasicElementColorType::LOW_MEDIUM_BRIGHT;
			}
			else if(child == 2)
			{
				*pnum = 0;
				objtype = BasicGraphElement::EllipseElement;
				rect->left = (int)(width * 0.6);
				rect->top = (int)(height * 0.3);
				rect->right = rect->left + (int)(width * 0.1);
				rect->bottom = rect->top + (int)(height * 0.1);
				helper.colorBright = BasicElementColorType::LOW_MEDIUM_BRIGHT;
			}
			else if(child == 3)
			{
				halfHeight = (int)((double)(pAdjustValue[0] - 15510) * 900 / (17600 - 15510) + 0.5);
				if(halfHeight < 0)
					halfHeight = 0;
				if(halfHeight > 900)
					halfHeight = 900;
				// TRACE("Smile Height =%d", halfHeight);
				_SmileMouse[0].y = 8100 - halfHeight;
				_SmileMouse[3].y = 8100 - halfHeight;

				halfHeight = (int)((double)(pAdjustValue[0] - 15510) * 1500 / (17600 - 15510) + 0.5);
				_SmileMouse[1].y = 6900 + halfHeight;
				_SmileMouse[2].y = 6900 + halfHeight;
				lpData = new CPOINT[4];
				convertPoints(lpData, _SmileMouse, 4, width, height);
				*pnum = 4;
				objtype = BasicGraphElement::PolygonElement;
			}
			break;
		case ShapeType::Arc: // 원호
			{
				if(child == 0) {
					// Start Angle(nSAngle)에서 오른쪽으로 그려진다.
					int nSAngle = pAdjustValue[0] >> 16;
					int nEAngle = pAdjustValue[1] >> 16;
					int p = 0;
					CPOINT arc[100];

					if(nSAngle > 0) {
						nSAngle = 360 - nSAngle;
					} else {
						nSAngle *= -1;
					}
					if(nEAngle > 0) {
						nEAngle = 360 - nEAngle;
					} else {
						nEAngle *= -1;
					}

					if(pAdjustValue[0] == 0 && pAdjustValue[3] == 0)
						nSAngle = 90;

					if(nEAngle > nSAngle)
						nSAngle += 360;

					p += GetArcPolyline(arc, 5000, 5000, 5000, nSAngle * 10, nEAngle * 10);
					if(drawFlags & DRAW_FILL_AREA) // filled as pie, by walker
					{
						arc[p].x = 5000;
						arc[p].y = 5000;
						arc[p].a = CP_LINE;
						p++;
					}

					lpData = new CPOINT[p + 10];
					convertPoints(lpData, arc, p, width, height);
					lpData[p-1].a = CP_LINE | CP_ENDS | CP_ENDG;

					*pnum = p;
					objtype = BasicGraphElement::PolygonElement;
				}
			}
			break;
		case ShapeType::Donut:						// = 23,
			if(child == 0)
				{
				int cx = pAdjustValue[0] * 5000 / 10800;

				lpData = new CPOINT[26];
				MakeRoundCorner(&(lpData[0]), 5000, 0, 10000, 5000, width, height, TRUE, FALSE);
				MakeRoundCorner(&(lpData[3]), 10000, 5000, 5000, 10000, width, height, FALSE, FALSE);
				MakeRoundCorner(&(lpData[6]), 5000, 10000, 0, 5000, width, height, TRUE, FALSE);
				MakeRoundCorner(&(lpData[9]), 0, 5000, 5000, 0, width, height, FALSE, TRUE);
				lpData[0].a = CP_STAT | CP_LINE;
				if((cx == 0) || (cx == 5000))
					{
					lpData[12].a = CP_LINE | CP_ENDS | CP_ENDG;
					if(cx == 5000)
						lpData[12].a |= CP_CLOSED;
					*pnum = 13;
					}
				else
					{
					lpData[12].a = CP_LINE | CP_ENDS | CP_CLOSED;

					MakeRoundCorner(&(lpData[13]), 5000, cx, 10000 - cx, 5000, width, height, TRUE, FALSE);
					MakeRoundCorner(&(lpData[16]), 10000 - cx, 5000, 5000, 10000 - cx, width, height, FALSE, FALSE);
					MakeRoundCorner(&(lpData[19]), 5000, 10000 - cx, cx, 5000, width, height, TRUE, FALSE);
					MakeRoundCorner(&(lpData[22]), cx, 5000, 5000, cx, width, height, FALSE, TRUE);
					lpData[13].a = CP_STAT | CP_LINE;
					lpData[25].a = CP_LINE | CP_ENDS | CP_ENDG | CP_CLOSED;
					*pnum = 26;
					}
				objtype = BasicGraphElement::PolygonElement;
				}
			break;
		case ShapeType::NoSmoking:					// 없음 기호
			if(child == 0) {
				int cx = (int)((double)(10800 - pAdjustValue[0]) * 5000 / 10800);
				int cx2 = (int)((double)pAdjustValue[0] * 5000 / 10800);
				lpData = new CPOINT[40];
				CPOINT arc[40];
				MakeRoundCorner(&(lpData[0]), 5000, 0, 10000, 5000, width, height, TRUE, FALSE);
				MakeRoundCorner(&(lpData[3]), 10000, 5000, 5000, 10000, width, height, FALSE, FALSE);
				MakeRoundCorner(&(lpData[6]), 5000, 10000, 0, 5000, width, height, TRUE, FALSE);
				MakeRoundCorner(&(lpData[9]), 0, 5000, 5000, 0, width, height, FALSE, TRUE);
				lpData[0].a = CP_STAT | CP_LINE;
				int p = 12;
				int nBckIndex = 0;
				if(pAdjustValue[0] == 0) {
					lpData[12].a = CP_ENDS;
					double bias = 2 * (sqrt(2.f) - 1) / 3;
					lpData[13].x = (width / 4) * (2 - sqrt(2.f));
					lpData[13].y = (height / 4) * (2 - sqrt(2.f));
					lpData[13].a = CP_LINE;
					lpData[14].x = (width / 4) * (sqrt(2.f) + 2);
					lpData[14].y = (height / 4) * (sqrt(2.f) + 2);
					lpData[14].a = CP_ENDS | CP_ENDG;
					*pnum = 15;
				} else if(pAdjustValue[0] == 7200) {
					lpData[12].a = CP_LINE | CP_ENDS | CP_CLOSED;
					MakeRoundCorner(&(lpData[13]), 5000, cx2, 10000 - cx2, 5000, width, height, TRUE, FALSE);
					MakeRoundCorner(&(lpData[16]), 10000 - cx2, 5000, 5000, 10000 - cx2, width, height, FALSE, FALSE);
					MakeRoundCorner(&(lpData[19]), 5000, 10000 - cx2, cx2, 5000, width, height, TRUE, FALSE);
					MakeRoundCorner(&(lpData[22]), cx2, 5000, 5000, cx2, width, height, FALSE, TRUE);
					lpData[13].a = CP_STAT | CP_LINE;
					lpData[25].a = CP_LINE | CP_ENDS | CP_ENDG;
					*pnum = 26;
				} else {
					lpData[12].a = CP_LINE | CP_ENDS | CP_CLOSED;

					// double dPI = 3.14159265358979323846;
					double varAngle = (double)asin((double)((double)pAdjustValue[0] * 5000 / 21600) / (5000 - ((double)pAdjustValue[0] * 5000 / 10800))) * 1800 / dPI;

					p += GetArcPolyline(arc, 5000, 5000, cx, short(3600 + 1350 - varAngle), int(3150 + varAngle));
					convertPoints(&(lpData[13]), arc, p + 1 - 13, width, height);
					lpData[p++].a = CP_LINE;
					lpData[p] = lpData[13];
					lpData[p].a = CP_ENDS | CP_CLOSED;
					nBckIndex = p;

					p += GetArcPolyline(arc, 5000, 5000, cx, short(3150 - varAngle), int(1350 + varAngle));
					convertPoints(&(lpData[nBckIndex + 1]), arc, p + 1 - (nBckIndex + 1), width, height);
					lpData[p++].a = CP_LINE;
					lpData[p] = lpData[nBckIndex + 1];
					lpData[p].a = CP_ENDS | CP_ENDG | CP_CLOSED;

					*pnum = p + 1;
				}
				objtype = BasicGraphElement::PolygonElement;
			}
			break;
		case ShapeType::BlockArc:					// = 95,
			if(child == 0) {
				CPOINT		arc[40];
				int angle = (int) (short) ((pAdjustValue[0] >> 16) & 0x00FFFF);
				int hx = pAdjustValue[1];
				int s, e;
				int p = 0;
				int cx = (int)((double)pAdjustValue[1] * 5000 / 10800 + 0.5);
				int cy = (int)((double)(pAdjustValue[0] & 0x00FFFF) * 5000 / 10800 + 0.5);

				int close = 0;

				if(angle < 0)
					angle += 360;
				if(angle >= 360)
					angle -= 360;
				*pnum = 0;

				// TRACE("ShapeType::BlockArc angle=%d, cx=%d, cy=%d\n", angle, cx, cy);
				if(angle == 270) {
					return(BasicGraphElement::None);
				}
				else if(angle == 90) {
					lpData = new CPOINT[26];
					MakeRoundCorner(&(lpData[0]), 5000, 0, 10000, 5000, width, height, TRUE, FALSE);
					MakeRoundCorner(&(lpData[3]), 10000, 5000, 5000, 10000, width, height, FALSE, FALSE);
					MakeRoundCorner(&(lpData[6]), 5000, 10000, 0, 5000, width, height, TRUE, FALSE);
					MakeRoundCorner(&(lpData[9]), 0, 5000, 5000, 0, width, height, FALSE, TRUE);
					lpData[12].a = CP_LINE | CP_ENDS;
					if(cx < 5000)
						lpData[12].a |= CP_CLOSED;
					*pnum = 13;

					if((cx < 5000) && (cx > 0)) {
						MakeRoundCorner(&(lpData[13]), 5000, 5000-cx, 5000 + cx, 5000, width, height, TRUE, FALSE);
						MakeRoundCorner(&(lpData[16]), 5000 + cx, 5000, 5000, 5000 + cx, width, height, FALSE, FALSE);
						MakeRoundCorner(&(lpData[19]), 5000, 5000 + cx, 5000-cx, 5000, width, height, TRUE, FALSE);
						MakeRoundCorner(&(lpData[22]), 5000-cx, 5000, 5000, 5000-cx, width, height, FALSE, TRUE);
						lpData[25].a = CP_LINE | CP_ENDS | CP_CLOSED;
						*pnum = 26;
					}
					lpData[0].a = CP_STAT | CP_LINE;
					lpData[*pnum-1].a |= CP_ENDG;
				} else {
					if((angle >= 0) && (angle < 90)) {
						s = 1800 + angle * 10;
						e = s + (1800 - angle * 20);
					} else if((angle > 90) && (angle < 180)) {
						angle = angle - 90;
						s = 2700 + angle * 10;
						e = s + 3600 - angle * 20;
					} else if((angle >= 180) && (angle < 270)) {
						s = angle * 10 - 1800;
						e = s + (270 - angle) * 20;
					} else if((angle > 270) && (angle < 360)) {
						s = 900 + (angle - 270) * 10;
						e = s + 3600 - (angle - 270) * 20;
					} else {
						return(BasicGraphElement::None);
					}

					p = GetArcPolyline(arc, 5000, 5000, 5000, s, e);
					if((cx < 5000) && (cx > 0)) {
						p += GetArcPolyline(&(arc[p]), 5000, 5000, cx, e, s);
						close = 1;
					}
					if(cx == 0) {
						// add center point
						arc[p].x = arc[p].y = 5000;
						p++;
					}
					lpData = new CPOINT[40];
					convertPoints(lpData, arc, p, width, height);
					lpData[p] = lpData[0];
					p++;

					// add end-of-polygon point
					lpData[0].a = CP_STAT | CP_LINE;
					lpData[p - 1].a = CP_LINE | CP_ENDS | CP_ENDG;
					if(close)
						lpData[p - 1].a |= CP_CLOSED;
					*pnum = p;
				}
				objtype = BasicGraphElement::PolygonElement;
			}
			break;
		case ShapeType::CircularArrow:					// 원형 화살표
			{
				if(child == 0) {
					CPOINT		arc[40];
					int angle0 = pAdjustValue[0] >> 16;
					int angle = pAdjustValue[1] >> 16;
					int hx = pAdjustValue[2];
					int s, e;
					int p = 0;
					int cx;
					if(pAdjustValue[2] >= 10800) {
						cx = (int)((double)10799 * 5000 / 10800 + 0.5);
					} else {
						cx = (int)((double)pAdjustValue[2] * 5000 / 10800 + 0.5);
					}
					int cy = (int)((double)(pAdjustValue[1] & 0x00FFFF) * 5000 / 10800 + 0.5);

					int close = 0;

					if(angle0 > 0) {
						angle0 = 360 - angle0;
					} else {
						angle0 *= -1;
					}

					if(angle > 0) {
						angle = 360 - angle;
					} else {
						angle *= -1;
					}

					if(angle0 < angle)
						angle0 += 360;
					*pnum = 0;

					s = angle0;
					e = angle;
					p = GetArcPolyline(arc, 5000, 5000, 5000, s * 10, e * 10);

					arc[p].a = CP_LINE;
					arc[p].x = (int)((double)(5 * arc[p - 1].x - 5000) / 4 + 0.5);
					arc[p].y = (int)((double)(5 * arc[p - 1].y - 5000) / 4 + 0.5);
					arc[p].a = CP_LINE;
					p++;
					arc[p].x = arc[p - 1].x + int(-((double)(7500 - cx) / 2.0) * cos((double)(-angle) * dPI / 180) - ( ((double)(7500 - cx) / 2.0) * sin((double)(-angle) * dPI / 180) ));
					arc[p].y = arc[p - 1].y + int(-((double)(7500 - cx) / 2.0) * sin((double)(-angle) * dPI / 180) + ( ((double)(7500 - cx) / 2.0) / 2.0 * cos((double)(-angle) * dPI / 180) ));
					arc[p].a = CP_LINE;
					p++;
					arc[p].x = (int)((double)(arc[p - 2].x * (cx - 1250) + 5000 * (7500 - cx)) / 6250 + 0.5);
					arc[p].y = (int)((double)(arc[p - 2].y * (cx - 1250) + 5000 * (7500 - cx)) / 6250 + 0.5);
					arc[p].a = CP_LINE;
					p++;

					if((cx < 5000) && (cx > 0)) {
						p += GetArcPolyline(&(arc[p]), 5000, 5000, cx, e * 10, s * 10);
						close = 1;
					}
					lpData = new CPOINT[40];
					convertPoints(lpData, arc, p, width, height);
					if((cx < 5000) && (cx > 0)) {
						lpData[p] = lpData[0];
						p++;
						// add end-of-polygon point
						lpData[0].a = CP_STAT | CP_LINE;
						lpData[p - 1].a = CP_LINE | CP_ENDS | CP_ENDG;
					}
					if(close)
						lpData[p - 1].a |= CP_CLOSED;
					*pnum = p;					
					objtype = BasicGraphElement::PolygonElement;
				}
			}
			break;
		case ShapeType::Heart:						// = 74,
			if(child == 0)
				{
				lpData = new CPOINT[23];
				convertPoints(lpData, _Heart, 23, width, height);
				*pnum = 23;
				objtype = BasicGraphElement::PolygonElement;
				}
			break;
		case ShapeType::LightningBolt:				// = 73,
			if(child == 0)
				{
				lpData = new CPOINT[12];
				convertPoints(lpData, _LightningBolt, 12, width, height);
				*pnum = 12;
				objtype = BasicGraphElement::PolygonElement;
				}
			break;
		case ShapeType::Sun:							// = 183,
			if(child == 0)
				{
				CPOINT	triangle[4];
				POINT		origin;
				int			cx, cy;

				lpData = new CPOINT[4*8+13];
				cx = 1100 - (int)((double)1000 * (pAdjustValue[0] - 2700) / (10125 - 2700) + 0.5);
				cy = 1200 + (int)((double)2400 * (pAdjustValue[0] - 2700) / (10125 - 2700) + 0.5);
				origin.x = 5000;
				origin.y = 5000;
				for(i=0 ; i<8 ; i++)
					{
					triangle[0].x = triangle[3].x = 5000;
					triangle[0].y = triangle[3].y = 0;
					triangle[0].a = CP_LINE;
					triangle[1].x = 5000 + cx;
					triangle[1].y = cy;
					triangle[1].a = CP_LINE;
					triangle[2].x = 5000 - cx;
					triangle[2].y = cy;
					triangle[2].a = CP_LINE;
					triangle[3].a = CP_LINE | CP_ENDS | CP_CLOSED;

					if(i > 0)
						RotateCurvePoint(4, triangle, i*450, &origin);
					convertPoints(&(lpData[i*4]), triangle, 4, width, height);
					}

				cx = 1300 + (int)((double)3400 * (pAdjustValue[0] - 2700) / (10125 - 2700) + 0.5);
				MakeRoundCorner(&(lpData[32]), 5000, cx, cx, 5000, width, height, TRUE, FALSE);
				MakeRoundCorner(&(lpData[35]), cx, 5000, 5000, 10000-cx, width, height, FALSE, FALSE);
				MakeRoundCorner(&(lpData[38]), 5000, 10000-cx, 10000-cx, 5000, width, height, TRUE, FALSE);
				MakeRoundCorner(&(lpData[41]), 10000-cx, 5000, 5000, cx, width, height, FALSE, TRUE);
				lpData[0].a = CP_STAT | CP_LINE;
				lpData[44].a = CP_LINE | CP_ENDS | CP_ENDG | CP_CLOSED;
				*pnum = 4*8+13;
				objtype = BasicGraphElement::PolygonElement;
				}
			break;
		case ShapeType::Moon:						// = 184,
			if(child == 0)
				{
				int cx = (int)((double)8760 * pAdjustValue[0] / 18900 + 0.5);
				lpData = new CPOINT[13];
				MakeRoundCorner(&(lpData[0]), 10000, 0, 0, 5000, width, height, TRUE, FALSE);
				MakeRoundCorner(&(lpData[3]), 0, 5000, 10000, 10000, width, height, FALSE, FALSE);
				MakeRoundCorner(&(lpData[6]), 10000, 10000, cx, 5000, width, height, TRUE, FALSE);
				MakeRoundCorner(&(lpData[9]), cx, 5000, 10000, 0, width, height, FALSE, TRUE);
				lpData[0].a = CP_STAT | CP_LINE;
				lpData[12].a = CP_LINE | CP_ENDS | CP_ENDG | CP_CLOSED;
				*pnum = 13;
				objtype = BasicGraphElement::PolygonElement;
				}
			break;

		case ShapeType::BracketPair:					// = 185,
			if(child == 0)
				{
				if(pAdjustValue[0] == 0)
					{
					lpData = new CPOINT[4];

					lpData[0].x = 0;
					lpData[0].y = 0;
					lpData[0].a = CP_STAT | CP_LINE;
					lpData[1].x = 0;
					lpData[1].y = height;
					lpData[1].a = (drawFlags & DRAW_FILL_AREA) ? CP_LINE : (CP_LINE | CP_ENDS);
					lpData[2].x = width;
					lpData[2].y = height;
					lpData[2].a = (drawFlags & DRAW_FILL_AREA) ? CP_LINE : (CP_LINE | CP_STAT);
					lpData[3].x = width;
					lpData[3].y = 0;
					lpData[3].a = (drawFlags & DRAW_FILL_AREA) ? (CP_ENDS | CP_ENDG | CP_CLOSED) : (CP_ENDS | CP_ENDG);
					*pnum = 4;
					}
				else
					{
					int cx, cy;

					cy = cx = (int)((double)5000 * pAdjustValue[0] / 10800 + 0.5);
					if(abs(width) > abs(height)) {
						cx = cx * height / width;
						cx = abs(cx);
					} else {
						if (height != 0)
							cy = cy * width / height;
						else
							cy = 0;
						cy = abs(cy);
					}

					lpData = new CPOINT[16];
					MakeRoundCorner(&(lpData[0]), cx, 0, 0, cy, width, height, TRUE, TRUE);
					MakeRoundCorner(&(lpData[4]), 0, 10000-cy, cx, 10000, width, height, FALSE, TRUE);
					lpData[0].a = CP_STAT | CP_LINE;
					// lpData[7].a = CP_LINE;
					// filled as one-polygon, by walker
					lpData[7].a = (drawFlags & DRAW_FILL_AREA) ? CP_LINE : (CP_LINE | CP_ENDS);

					MakeRoundCorner(&(lpData[8]), 10000-cx, 10000, 10000, 10000-cy, width, height, TRUE, TRUE);
					MakeRoundCorner(&(lpData[12]), 10000, cy, 10000-cx, 0, width, height, FALSE, TRUE);
					lpData[8].a = (drawFlags & DRAW_FILL_AREA) ? CP_LINE : (CP_STAT | CP_LINE);
					lpData[15].a = CP_LINE | CP_ENDS | CP_ENDG;
					if(drawFlags & DRAW_FILL_AREA)
						lpData[15].a |= CP_CLOSED;
					*pnum = 16;
					}
				objtype = BasicGraphElement::PolygonElement;
				}
			break;
		case ShapeType::BracePair:					// = 186,
			if(child == 0)
				{
				if(pAdjustValue[0] == 0)
					{
					lpData = new CPOINT[4];

					lpData[0].x = 0;
					lpData[0].y = 0;
					lpData[0].a = CP_STAT | CP_LINE;
					lpData[1].x = 0;
					lpData[1].y = height;
					lpData[1].a = (drawFlags & DRAW_FILL_AREA) ? CP_LINE : (CP_LINE | CP_ENDS);
					lpData[2].x = width;
					lpData[2].y = height;
					lpData[2].a = (drawFlags & DRAW_FILL_AREA) ? CP_LINE : (CP_LINE | CP_STAT);
					lpData[3].x = width;
					lpData[3].y = 0;
					lpData[3].a = (drawFlags & DRAW_FILL_AREA) ? (CP_ENDS | CP_ENDG | CP_CLOSED) : (CP_ENDS | CP_ENDG);
					*pnum = 4;
					}
				else
					{
					int cx = 0, cy = 0;

					// width, height의 절대값으로 비교해야 함
					if(abs(width) > abs(height)) {
						cx = (int)((double)((double)2500 * pAdjustValue[0] / 5400) * height / width + 0.5);
						cx = abs((int)((double)2500 * pAdjustValue[0] / 5400 + 0.5));
						cy = (int)((double)2500 * pAdjustValue[0] / 5400 + 0.5);
					} else {
						cx = (int)((double)2500 * pAdjustValue[0] / 5400 + 0.5);
						cy = (int)((double)((double)2500 * pAdjustValue[0] / 5400) * width / height + 0.5);
						cy = abs((int)((double)2500 * pAdjustValue[0] / 5400 + 0.5));
					}

					lpData = new CPOINT[32];
					MakeRoundCorner(&(lpData[0]), cx*2, 0, cx, cy, width, height, TRUE, TRUE);
					MakeRoundCorner(&(lpData[4]), cx, 5000-cy, 0, 5000, width, height, FALSE, TRUE);
					MakeRoundCorner(&(lpData[8]), 0, 5000, cx, 5000+cy, width, height, TRUE, TRUE);
					MakeRoundCorner(&(lpData[12]), cx, 10000-cy, cx*2, 10000, width, height, FALSE, TRUE);
					lpData[0].a = CP_STAT | CP_LINE;
					// filled as one-polygon, by walker
					lpData[15].a = (drawFlags & DRAW_FILL_AREA) ? CP_LINE : (CP_LINE | CP_ENDS);

					MakeRoundCorner(&(lpData[16]), 10000-cx*2, 10000, 10000-cx, 10000-cy, width, height, TRUE, TRUE);
					MakeRoundCorner(&(lpData[20]), 10000-cx, 5000+cy, 10000, 5000, width, height, FALSE, TRUE);
					MakeRoundCorner(&(lpData[24]), 10000, 5000, 10000-cx, 5000-cy, width, height, TRUE, TRUE);
					MakeRoundCorner(&(lpData[28]), 10000-cx, cy, 10000-cx*2, 0, width, height, FALSE, TRUE);
					// filled as one-polygon, by walker
					lpData[16].a = (drawFlags & DRAW_FILL_AREA) ? CP_LINE : (CP_STAT | CP_LINE);
					lpData[31].a = CP_LINE | CP_ENDS | CP_ENDG;
					if(drawFlags & DRAW_FILL_AREA)
						lpData[31].a = CP_CLOSED;
					*pnum = 32;
					}
				objtype = BasicGraphElement::PolygonElement;
				}
			break;
		case ShapeType::Plaque:						// = 21,
			if(child == 0)
				{
				if(pAdjustValue[0] == 0)
					{
					lpData = new CPOINT[5];

					lpData[0].x = 0;
					lpData[0].y = 0;
					lpData[0].a = CP_STAT | CP_LINE;
					lpData[1].x = width;
					lpData[1].y = 0;
					lpData[1].a = CP_LINE;
					lpData[2].x = width;
					lpData[2].y = height;
					lpData[2].a = CP_LINE;
					lpData[3].x = 0;
					lpData[3].y = height;
					lpData[3].a = CP_LINE;
					lpData[4].x = 0;
					lpData[4].y = 0;
					lpData[4].a = CP_LINE | CP_ENDS | CP_ENDG | CP_CLOSED;
					*pnum = 5;
					}
				else
					{
					int cx = 0, cy = 0;

					// width, height의 절대값으로 비교해야 함
					if(abs(width) > abs(height)) {
						cx = (int)((double)((double)5000 * pAdjustValue[0] / 10800) * height / width + 0.5);
						cx = abs((int)((double)5000 * pAdjustValue[0] / 10800 + 0.5));
						cy = (int)((double)5000 * pAdjustValue[0] / 10800 + 0.5);
					} else {
						cx = (int)((double)5000 * pAdjustValue[0] / 10800 + 0.5);
						cy = (int)((double)((double)5000 * pAdjustValue[0] / 10800) * width / height + 0.5);
						cy = abs((int)((double)5000 * pAdjustValue[0] / 10800 + 0.5));
					}
					lpData = new CPOINT[17];
					MakeRoundCorner(&(lpData[0]), 0, cy, cx, 0, width, height, TRUE, TRUE);
					MakeRoundCorner(&(lpData[4]), 10000-cx, 0, 10000, cy, width, height, FALSE, TRUE);
					MakeRoundCorner(&(lpData[8]), 10000, 10000-cy, 10000-cx, 10000, width, height, TRUE, TRUE);
					MakeRoundCorner(&(lpData[12]), cx, 10000, 0, 10000-cy, width, height, FALSE, TRUE);
					lpData[0].a = CP_STAT | CP_LINE;
					lpData[16].x = 0;
					lpData[16].y = (int)((double)cy * height / 10000 + 0.5);
					lpData[16].a = CP_LINE | CP_ENDS | CP_ENDG | CP_CLOSED;
					*pnum = 17;
					}
				objtype = BasicGraphElement::PolygonElement;
				}
			break;
		case ShapeType::LeftBracket:					// = 85,
			if(child == 0)
				{
				if(pAdjustValue[0] == 0)
					{
					lpData = new CPOINT[4];

					lpData[0].x = width;
					lpData[0].y = 0;
					lpData[0].a = CP_STAT | CP_LINE;
					lpData[1].x = 0;
					lpData[1].y = 0;
					lpData[1].a = CP_LINE;
					lpData[2].x = 0;
					lpData[2].y = height;
					lpData[2].a = CP_LINE;
					lpData[3].x = width;
					lpData[3].y = height;
					lpData[3].a = CP_LINE | CP_ENDS | CP_ENDG;
					*pnum = 4;
					}
				else
					{
					int cx = (int)((double)5000 * pAdjustValue[0] / 10800 + 0.5);

					lpData = new CPOINT[8];
					MakeRoundCorner(&(lpData[0]), 10000, 0, 0, cx, width, height, TRUE, TRUE);
					MakeRoundCorner(&(lpData[4]), 0, 10000-cx, 10000, 10000, width, height, FALSE, TRUE);
					lpData[0].a = CP_STAT | CP_LINE;
					lpData[7].a = CP_LINE | CP_ENDS | CP_ENDG;
					*pnum = 8;
					}
				objtype = BasicGraphElement::PolygonElement;
				}
			break;
		case ShapeType::RightBracket:				// = 86,
			if(child == 0)
				{
				if(pAdjustValue[0] == 0)
					{
					lpData = new CPOINT[4];

					lpData[0].x = 0;
					lpData[0].y = 0;
					lpData[0].a = CP_STAT | CP_LINE;
					lpData[1].x = width;
					lpData[1].y = 0;
					lpData[1].a = CP_LINE;
					lpData[2].x = width;
					lpData[2].y = height;
					lpData[2].a = CP_LINE;
					lpData[3].x = 0;
					lpData[3].y = height;
					lpData[3].a = CP_LINE | CP_ENDS | CP_ENDG;
					*pnum = 4;
					}
				else
					{
					int cx = (int)((double)5000 * pAdjustValue[0] / 10800 + 0.5);

					lpData = new CPOINT[8];
					MakeRoundCorner(&(lpData[0]), 0, 0, 10000, cx, width, height, TRUE, TRUE);
					MakeRoundCorner(&(lpData[4]), 10000, 10000-cx, 0, 10000, width, height, FALSE, TRUE);
					lpData[0].a = CP_STAT | CP_LINE;
					lpData[7].a = CP_LINE | CP_ENDS | CP_ENDG;
					*pnum = 8;
					}
				objtype = BasicGraphElement::PolygonElement;
				}
			break;
		case ShapeType::LeftBrace:					// = 87,
			if(child == 0)
				{
				int cy = (int)((double)height * pAdjustValue[1] / 21600 + 0.5);
				if(pAdjustValue[0] == 0)
					{
					lpData = new CPOINT[7];

					lpData[0].x = width;
					lpData[0].y = 0;
					lpData[0].a = CP_STAT | CP_LINE;
					lpData[1].x = (int)((double)width / 2 + 0.5);
					lpData[1].y = 0;
					lpData[1].a = CP_LINE;
					lpData[2].x = (int)((double)width / 2 + 0.5);
					lpData[2].y = cy;
					lpData[2].a = CP_LINE;
					lpData[3].x = 0;
					lpData[3].y = cy;
					lpData[3].a = CP_LINE;
					lpData[4].x = (int)((double)width / 2 + 0.5);
					lpData[4].y = cy;
					lpData[4].a = CP_LINE;
					lpData[5].x = (int)((double)width / 2 + 0.5);
					lpData[5].y = height;
					lpData[5].a = CP_LINE;
					lpData[6].x = width;
					lpData[6].y = height;
					lpData[6].a = CP_LINE | CP_ENDS | CP_ENDG;
					*pnum = 7;
					}
				else
					{
					int cx = (int)((double)2500 * pAdjustValue[0] / 5400 + 0.5);

					cy = (int)((double)10000 * pAdjustValue[1] / 21600 + 0.5);
					if(cy < cx * 2)
						cy = cx * 2;
					if(cy > 10000 - cx * 2)
						cy = 10000 - cx * 2;
					lpData = new CPOINT[16];
					MakeRoundCorner(&(lpData[0]), 10000, 0, 5000, cx, width, height, TRUE, TRUE);
					MakeRoundCorner(&(lpData[4]), 5000, cy-cx, 0, cy, width, height, FALSE, TRUE);
					MakeRoundCorner(&(lpData[8]), 0, cy, 5000, cy+cx, width, height, TRUE, TRUE);
					MakeRoundCorner(&(lpData[12]), 5000, 10000-cx, 10000, 10000, width, height, FALSE, TRUE);
					lpData[0].a = CP_STAT | CP_LINE;
					lpData[15].a = CP_LINE | CP_ENDS | CP_ENDG;
					*pnum = 16;
					}
				objtype = BasicGraphElement::PolygonElement;
				}
			break;
		case ShapeType::RightBrace:					// = 88,
			if(child == 0)
				{
				int cy = (int)((double)height * pAdjustValue[1] / 21600 + 0.5);
				if(pAdjustValue[0] == 0)
					{
					lpData = new CPOINT[7];

					lpData[0].x = 0;
					lpData[0].y = 0;
					lpData[0].a = CP_STAT | CP_LINE;
					lpData[1].x = width / 2;
					lpData[1].y = 0;
					lpData[1].a = CP_LINE;
					lpData[2].x = width / 2;
					lpData[2].y = cy;
					lpData[2].a = CP_LINE;
					lpData[3].x = width;
					lpData[3].y = cy;
					lpData[3].a = CP_LINE;
					lpData[4].x = width / 2;
					lpData[4].y = cy;
					lpData[4].a = CP_LINE;
					lpData[5].x = width / 2;
					lpData[5].y = height;
					lpData[5].a = CP_LINE;
					lpData[6].x = 0;
					lpData[6].y = height;
					lpData[6].a = CP_LINE | CP_ENDS | CP_ENDG;
					*pnum = 7;
					}
				else
					{
					int cx = (int)((double)2500 * pAdjustValue[0] / 5400 + 0.5);

					cy = (int)((double)10000 * pAdjustValue[1] / 21600 + 0.5);
					if(cy < cx * 2)
						cy = cx * 2;
					if(cy > 10000 - cx * 2)
						cy = 10000 - cx * 2;
					lpData = new CPOINT[16];
					MakeRoundCorner(&(lpData[0]), 0, 0, 5000, cx, width, height, TRUE, TRUE);
					MakeRoundCorner(&(lpData[4]), 5000, cy-cx, 10000, cy, width, height, FALSE, TRUE);
					MakeRoundCorner(&(lpData[8]), 10000, cy, 5000, cy+cx, width, height, TRUE, TRUE);
					MakeRoundCorner(&(lpData[12]), 5000, 10000-cx, 0, 10000, width, height, FALSE, TRUE);
					lpData[0].a = CP_STAT | CP_LINE;
					lpData[15].a = CP_LINE | CP_ENDS | CP_ENDG;
					*pnum = 16;
					}
				objtype = BasicGraphElement::PolygonElement;
				}
			break;
		case ShapeType::BentArrow:					// = 91,
			if(child == 0)
				{
				int		cx, cy;
				lpData = new CPOINT[17];

				cy = (int)((double)2900 * pAdjustValue[1] / 6079 + 0.5);
				if(cy > 2900)
					cy = 2900;
				cx = (int)((double)4200 * (pAdjustValue[0] - 12427) / (21600 - 12427) + 0.5);

				lpData[0].x = 0;
				lpData[0].y = height;
				lpData[0].a = CP_STAT | CP_LINE;
				lpData[1].x = 0;
				lpData[1].y = (int)((double)5800 * height / 10000 + 0.5);
				lpData[1].a = CP_LINE;
				MakeRoundCorner(&(lpData[2]), 0, 5800, 5800, cy, width, height, FALSE, TRUE);
				lpData[6].x = (int)((double)(5800 + cx) * width / 10000 + 0.5);
				lpData[6].y = (int)((double)cy * height / 10000 + 0.5);
				lpData[6].a = CP_LINE;
				lpData[7].x = (int)((double)(5800 + cx) * width / 10000 + 0.5);
				lpData[7].y = 0;
				lpData[7].a = CP_LINE;
				lpData[8].x = width;
				lpData[8].y = (int)((double)2900 * height / 10000 + 0.5);
				lpData[8].a = CP_LINE;
				lpData[9].x = (int)((double)(5800 + cx) * width / 10000 + 0.5);
				lpData[9].y = (int)((double)5800 * height / 10000 + 0.5);
				lpData[9].a = CP_LINE;
				lpData[10].x = (int)((double)(5800 + cx) * width / 10000 + 0.5);
				lpData[10].y = (int)((double)(5800 - cy) * height / 10000 + 0.5);
				lpData[10].a = CP_LINE;
				MakeRoundCorner(&(lpData[11]), 5800, 5800 - cy, 5800 - cy * 2, 5800, width, height, TRUE, TRUE);
				lpData[15].x = (int)((double)(5800 - cy * 2) * width / 10000 + 0.5);
				lpData[15].y = height;
				lpData[15].a = CP_LINE;
				lpData[16].x = 0;
				lpData[16].y = height;
				lpData[16].a = CP_LINE | CP_ENDS | CP_ENDG | CP_CLOSED;
				*pnum = 17;
				objtype = BasicGraphElement::PolygonElement;
				}
			break;
		case ShapeType::UturnArrow:					// = 101,
			if(child == 0)
				{
				lpData = new CPOINT[22];
				convertPoints(lpData, _UturnArrow, 22, width, height);
				*pnum = 22;
				objtype = BasicGraphElement::PolygonElement;
				}
			break;
		case ShapeType::StripedRightArrow:			// = 93,
			{
			int		cx, cy, p;
			lpData = new CPOINT[18];

			cy = (int)((double)5000 * pAdjustValue[1] / 10800 + 0.5);
			if(cy > 5000)
				cy = 5000;
			cx = 1540 + (int)((double)8460 * (pAdjustValue[0] - 3375) / (21600 - 3375) + 0.5);
			if(cx > 10000)
				cx = 10000;
			if(cx < 1540)
				cx = 1540;

			if(child == 0)
				{
				*pnum = p = 0;

				if(cy == 5000)
					{
					lpData[0].x = 0;
					lpData[0].y = (int)((double)height / 2 + 0.5);
					lpData[0].a = CP_STAT | CP_LINE;
					lpData[1].x = (int)((double)300 * width / 10000 + 0.5);
					lpData[1].y = (int)((double)height / 2 + 0.5);
					lpData[1].a = CP_LINE | CP_ENDS;

					lpData[2].x = (int)((double)600 * width / 10000 + 0.5);
					lpData[2].y = (int)((double)height / 2 + 0.5);
					lpData[2].a = CP_STAT | CP_LINE;
					lpData[3].x = (int)((double)1230 * width / 10000 + 0.5);
					lpData[3].y = (int)((double)height / 2 + 0.5);
					lpData[3].a = CP_LINE | CP_ENDS;

					p = 4;
					}
				else
					{
					lpData[0].x = 0;
					lpData[0].y = (int)((double)cy * height / 10000 + 0.5);
					lpData[0].a = CP_STAT | CP_LINE;
					lpData[1].x = (int)((double)300 * width / 10000 + 0.5);
					lpData[1].y = (int)((double)cy * height / 10000 + 0.5);
					lpData[1].a = CP_LINE;
					lpData[2].x = (int)((double)300 * width / 10000 + 0.5);
					lpData[2].y = (int)((double)(10000 - cy) * height / 10000 + 0.5);
					lpData[2].a = CP_LINE;
					lpData[3].x = 0;
					lpData[3].y = (int)((double)(10000 - cy) * height / 10000 + 0.5);
					lpData[3].a = CP_LINE;
					lpData[4].x = 0;
					lpData[4].y = (int)((double)cy * height / 10000 + 0.5);
					lpData[4].a = CP_ENDS | CP_CLOSED;

					lpData[5].x = (int)((double)600 * width / 10000 + 0.5);
					lpData[5].y = (int)((double)cy * height / 10000 + 0.5);
					lpData[5].a = CP_STAT | CP_LINE;
					lpData[6].x = (int)((double)1230 * width / 10000 + 0.5);
					lpData[6].y = (int)((double)cy * height / 10000 + 0.5);
					lpData[6].a = CP_LINE;
					lpData[7].x = (int)((double)1230 * width / 10000 + 0.5);
					lpData[7].y = (int)((double)(10000 - cy) * height / 10000 + 0.5);
					lpData[7].a = CP_LINE;
					lpData[8].x = (int)((double)600 * width / 10000 + 0.5);
					lpData[8].y = (int)((double)(10000 - cy) * height / 10000 + 0.5);
					lpData[8].a = CP_LINE;
					lpData[9].x = (int)((double)600 * width / 10000 + 0.5);
					lpData[9].y = (int)((double)cy * height / 10000 + 0.5);
					lpData[9].a = CP_ENDS | CP_CLOSED;

					p = 10;
					}

				lpData[p].x = (int)((double)1540 * width / 10000 + 0.5);
				lpData[p].y = (int)((double)cy * height / 10000 + 0.5);
				lpData[p].a = CP_STAT | CP_LINE;
				p++;
				lpData[p].x = (int)((double)cx * width / 10000 + 0.5);
				lpData[p].y = (int)((double)cy * height / 10000 + 0.5);
				lpData[p].a = CP_LINE;
				p++;
				lpData[p].x = (int)((double)cx * width / 10000 + 0.5);
				lpData[p].y = 0;
				lpData[p].a = CP_LINE;
				p++;
				lpData[p].x = width;
				lpData[p].y = (int)((double)5000 * height / 10000 + 0.5);
				lpData[p].a = CP_LINE;
				p++;
				lpData[p].x = (int)((double)cx * width / 10000 + 0.5);
				lpData[p].y = height;
				lpData[p].a = CP_LINE;
				p++;
				lpData[p].x = (int)((double)cx * width / 10000 + 0.5);
				lpData[p].y = (int)((double)(10000 - cy) * height / 10000 + 0.5);
				lpData[p].a = CP_LINE;
				p++;
				lpData[p].x = (int)((double)1540 * width / 10000 + 0.5);
				lpData[p].y = (int)((double)(10000 - cy) * height / 10000 + 0.5);
				lpData[p].a = CP_LINE;
				p++;
				lpData[p].x = (int)((double)1540 * width / 10000 + 0.5);
				lpData[p].y = (int)((double)cy * height / 10000 + 0.5);
				lpData[p].a = CP_ENDS | CP_ENDG | CP_CLOSED;
				p++;
				*pnum = p;
				objtype = BasicGraphElement::PolygonElement;
				}
			break;
			}

		case ShapeType::CurvedRightArrow:			// = 102,
		case ShapeType::CurvedLeftArrow:				// = 103,
			{
			int cy = (int)((double)10000 * pAdjustValue[0] / 21600 + 0.5);						// 삼각형 윗 모서리의 y좌표(10000기준 상대좌표)
			int arrow_margin = 10000 - (int)((double)10000 * pAdjustValue[1] / 21600 + 0.5);   // 삼각형 뒤쪽 열린부분을 제외한 부분의 절반
			int strip_height = 10000 - cy - arrow_margin * 2;									// 삼각형 꼬리 접합부분의 길이
			int arrow_width = (int)((double)10000 * pAdjustValue[2] / 21600 + 0.5);			// 삼각형 두께
			int	p, dy;

			if(mType == ShapeType::CurvedRightArrow)
				arrow_width = 10000 - arrow_width; // ShapeType::CurvedRightArrow 경우 pAdjustValue[2]가 두께이외 길이를 나타낸다.
			cy += arrow_margin; // 위쪽 타원의 세로축 길이
			if(mType == ShapeType::CurvedLeftArrow)
				child = 1 - child;

			if(child == 0)
				{
				lpData = new CPOINT[9];
				MakeRoundCorner(&(lpData[0]), 0, cy / 2, 10000, 0, width, height, FALSE, TRUE);
				MakeRoundCorner(&(lpData[4]), 10000, strip_height, 0, cy / 2+strip_height, width, height, TRUE, TRUE);
				lpData[8] = lpData[0];
				lpData[0].a = CP_STAT | CP_LINE;
				lpData[8].a = CP_LINE | CP_ENDS | CP_ENDG | CP_CLOSED;
				*pnum = 9;
				objtype = BasicGraphElement::PolygonElement;

				if (mType == ShapeType::CurvedRightArrow)
					helper.colorBright = BasicElementColorType::LOW_MEDIUM_BRIGHT;
				}
			else if(child == 1)
				{
				POINT	cutline[2];

				cutline[0].x = (10000 - arrow_width);
				cutline[0].y = 0;
				cutline[1].x = (10000 - arrow_width);
				cutline[1].y = 10000;

				lpData = new CPOINT[12];
				MakeRoundCorner(&(lpData[0]), 0, cy / 2, 10000, cy, 10000, 10000, FALSE, TRUE);

				if(arrow_width > 0)
					p = SplitBezierSegmentByLine(lpData, cutline, 0);
				else
					p = 4;

				if(arrow_margin > 0)
					{
					// move arrow position vertically
					dy = cy - lpData[p-1].y;
					lpData[p].x = (10000 - arrow_width);
					lpData[p].y = (cy - arrow_margin) - dy;
					lpData[p].a = CP_LINE;
					p++;
					if(arrow_width > 0)
						{
						lpData[p].x = 10000;
						lpData[p].y = (cy + strip_height / 2);
						lpData[p].a = CP_LINE;
						p++;
						}
					lpData[p].x = (10000 - arrow_width);
					lpData[p].y = 10000 - dy;
					lpData[p].a = CP_LINE;
					p++;
					}
				else
					{
					lpData[p].x = 10000;
					lpData[p].y = (cy + strip_height / 2);
					lpData[p].a = CP_LINE;
					p++;
					}
				MakeRoundCorner(&(lpData[p]), 10000, cy+strip_height, 0, cy / 2+strip_height, 10000, 10000, TRUE, TRUE);
				if(arrow_width > 0)
					p += SplitBezierSegmentByLine(&(lpData[p]), cutline, 1);
				else
					p += 4;

				lpData[p] = lpData[0];
				lpData[0].a = CP_STAT | CP_LINE;
				lpData[p].a = CP_LINE | CP_ENDS | CP_ENDG | CP_CLOSED;
				*pnum = p + 1;

				convertPoints(lpData, lpData, p+1, width, height);
				objtype = BasicGraphElement::PolygonElement;
				if (mType == ShapeType::CurvedLeftArrow)
					helper.colorBright = BasicElementColorType::LOW_MEDIUM_BRIGHT;
			}

			if((mType == ShapeType::CurvedLeftArrow) && (lpData != NULL))
				HFlopCPoint(*pnum, lpData, width / 2);
			break;
			}
		case ShapeType::CurvedUpArrow:				// = 104,
		case ShapeType::CurvedDownArrow:				// = 105,
			{
			int cy = (int)((double)10000 * pAdjustValue[0] / 21600 + 0.5);
			int arrow_margin = 10000 - (int)((double)10000 * pAdjustValue[1] / 21600 + 0.5);
			int strip_height = 10000 - cy - arrow_margin * 2;
			int arrow_width = (int)((double)10000 * pAdjustValue[2] / 21600 + 0.5);
			int	p, dy;

			if(mType == ShapeType::CurvedDownArrow)
				arrow_width = 10000 - arrow_width;
			cy += arrow_margin;
			if(mType == ShapeType::CurvedUpArrow)
				child = 1 - child;
			if(child == 0)
				{
				lpData = new CPOINT[9];
				MakeRoundCorner(&(lpData[0]), cy / 2, 10000, 0, 0, width, height, TRUE, TRUE);
				MakeRoundCorner(&(lpData[4]), strip_height, 0, cy / 2+strip_height, 10000, width, height, FALSE, TRUE);
				lpData[8] = lpData[0];
				lpData[0].a = CP_STAT | CP_LINE;
				lpData[8].a = CP_LINE | CP_ENDS | CP_ENDG | CP_CLOSED;
				*pnum = 9;
				objtype = BasicGraphElement::PolygonElement;
				if (mType == ShapeType::CurvedDownArrow)
					helper.colorBright = BasicElementColorType::LOW_MEDIUM_BRIGHT;
			}
			else if(child == 1)
				{
				POINT	cutline[2];

				cutline[0].x = 0;
				cutline[0].y = arrow_width;
				cutline[1].x = 10000;
				cutline[1].y = arrow_width;

				lpData = new CPOINT[12];
				MakeRoundCorner(&(lpData[0]), cy / 2, 10000, cy, 0, 10000, 10000, TRUE, TRUE);

				if(arrow_width > 0)
					p = SplitBezierSegmentByLine(lpData, cutline, 0);
				else
					p = 4;

				if(arrow_margin > 0)
					{
					// move arrow position vertically
					dy = cy - lpData[p-1].x;
					lpData[p].x = (cy - arrow_margin) - dy;
					lpData[p].y = arrow_width;
					lpData[p].a = CP_LINE;
					p++;
					if(arrow_width > 0)
						{
						lpData[p].x = (cy + strip_height / 2);
						lpData[p].y = 0;
						lpData[p].a = CP_LINE;
						p++;
						}
					lpData[p].x = 10000 - dy;
					lpData[p].y = arrow_width;
					lpData[p].a = CP_LINE;
					p++;
					}
				else
					{
					if(arrow_width > 0)
						{
						lpData[p].x = (cy + strip_height / 2);
						lpData[p].y = 0;
						lpData[p].a = CP_LINE;
						p++;
						}
					}
				MakeRoundCorner(&(lpData[p]), cy+strip_height, 0, cy / 2+strip_height, 10000, 10000, 10000, FALSE, TRUE);
				if(arrow_width > 0)
					p += SplitBezierSegmentByLine(&(lpData[p]), cutline, 1);
				else
					p += 4;

				lpData[p] = lpData[0];
				lpData[0].a = CP_STAT | CP_LINE;
				lpData[p].a = CP_LINE | CP_ENDS | CP_ENDG | CP_CLOSED;
				*pnum = p + 1;

				convertPoints(lpData, lpData, p+1, width, height);
				objtype = BasicGraphElement::PolygonElement;
				if (mType == ShapeType::CurvedUpArrow)
					helper.colorBright = BasicElementColorType::LOW_MEDIUM_BRIGHT;
			}

			if((mType == ShapeType::CurvedDownArrow) && (lpData != NULL))
				VFlopCPoint(*pnum, lpData, height / 2);
			break;
			}
		case ShapeType::Ribbon2:						// = 54,
		case ShapeType::Ribbon:						// = 53,
			{
			int sideMargin = (int)((double)width * pAdjustValue[0] / 21600 + 0.5);
			int yMargin = (int)((double)height * (21600 - pAdjustValue[1]) / 21600 + 0.5);
			int yMarginLog = (int)((double)10000 * (21600 - pAdjustValue[1]) / 21600 + 0.5);
			int	p = 0;
			int		x1 = (int)((double)pAdjustValue[0] * 10000 / 21600 + 0.5);
			int		x2 = (int)((double)(pAdjustValue[0] + 2700) * 10000 / 21600 + 0.5);

			if(mType == ShapeType::Ribbon)
				{
				yMargin = (int)((double)height * pAdjustValue[1] / 21600 + 0.5);
				yMarginLog = (int)((double)10000 * pAdjustValue[1] / 21600 + 0.5);
				}

			if((child == 0) || (child == 1))
				{
				if(child == 1)
					x2 = 10000 - x2;
				lpData = new CPOINT[9];
				lpData[p].x = (child == 0) ? 0 : width;
				lpData[p].y = height;
				lpData[p++].a = CP_STAT | CP_LINE;
				lpData[p].x = ((child == 0) ? 1270 : (10000 - 1270)) * width / 10000;
				lpData[p].y = height / 2 + yMargin / 2;
				lpData[p++].a = CP_LINE;
				lpData[p].x = (child == 0) ? 0 : width;
				lpData[p].y = yMargin;
				lpData[p++].a = CP_LINE;
				lpData[p].x = (int)((double)x2 * width / 10000 + 0.5);
				lpData[p].y = yMargin;
				lpData[p++].a = CP_LINE;
				x1 = (child == 0) ? -310 : 310;
				MakeRoundCorner(&(lpData[p]), x2, 10000-yMarginLog/4, x2+x1, 10000, width, height, FALSE, TRUE);
				p += 4;
				lpData[p] = lpData[0];
				lpData[p].a = CP_LINE | CP_ENDS | CP_ENDG | CP_CLOSED;
				*pnum = p+1;
				objtype = BasicGraphElement::PolygonElement;
				}
			else if(child == 2)
				{
				// dx = pAdjustValue[0] * 10000 / 21600;
				lpData = new CPOINT[15];
				lpData[p].x = (int)((double)(10000 - x1) * width / 10000 + 0.5);
				lpData[p].y = height - yMargin * 3 / 4;
				lpData[p++].a = CP_STAT | CP_LINE;
				lpData[p].x = (int)((double)(10000 - x2) * width / 10000 + 0.5);
				lpData[p].y = height - yMargin * 3 / 4;
				lpData[p++].a = CP_LINE;
				lpData[p].x = (int)((double)(10000 - x2) * width / 10000 + 0.5);
				lpData[p].y = height - yMargin;
				lpData[p++].a = CP_LINE;
				lpData[p].x = (int)((double)x2 * width / 10000 + 0.5);
				lpData[p].y = height - yMargin;
				lpData[p++].a = CP_LINE;
				lpData[p].x = (int)((double)x2 * width / 10000 + 0.5);
				lpData[p].y = height - yMargin * 3 / 4;
				lpData[p++].a = CP_LINE;
				lpData[p].x = (int)((double)x1 * width / 10000 + 0.5);
				lpData[p].y = height - yMargin * 3 / 4;
				lpData[p++].a = CP_LINE;

				MakeRoundCorner(&(lpData[p]), x1, yMarginLog/4, x1+310, 0, width, height, FALSE, TRUE);
				p += 4;
				MakeRoundCorner(&(lpData[p]), (10000-x1-310), 0, (10000-x1), yMarginLog/4, width, height, TRUE, TRUE);
				p += 4;

				lpData[p] = lpData[0];
				lpData[p].a = CP_LINE | CP_ENDS | CP_ENDG | CP_CLOSED;
				*pnum = p+1;
				objtype = BasicGraphElement::PolygonElement;
				}
			else if((child == 3) || (child == 4))
				{
				int dp = 310;
				if(child == 4)
					{
					x2 = 10000 - x2;
					x1 = 10000 - x1;
					dp = -310;
					}
				lpData = new CPOINT[14];
				int dy = (int)((double)(10000 - yMarginLog + yMarginLog / 4) * height / 10000 + 0.5);
				lpData[p].x = (int)((double)x2 * width / 10000 + 0.5);
				lpData[p].y = dy;
				lpData[p++].a = CP_STAT | CP_LINE;
				dy = (int)((double)(10000 - yMarginLog) * height / 10000 + 0.5);
				lpData[p].x = (int)((double)x2 * width / 10000 + 0.5);
				lpData[p].y = dy;
				lpData[p++].a = CP_STAT | CP_LINE;
				lpData[p].x = (int)((double)(x1+dp) * width / 10000 + 0.5);
				lpData[p].y = dy;
				lpData[p++].a = CP_STAT | CP_LINE;
				MakeRoundCorner(&(lpData[p]), (x1+dp), 10000 - yMarginLog, x1, 10000 - yMarginLog + yMarginLog / 4, width, height, TRUE, FALSE);
				p += 3;
				MakeRoundCorner(&(lpData[p]), x1, 10000 - yMarginLog + yMarginLog / 4, x1+dp, 10000 - yMarginLog / 2, width, height, FALSE, TRUE);
				p += 4;
				MakeRoundCorner(&(lpData[p]), x2-dp, 10000 - yMarginLog / 2, x2, 10000 - yMarginLog / 4, width, height, TRUE, TRUE);
				p += 4;
				lpData[p-1].a = CP_LINE | CP_ENDS | CP_ENDG | CP_CLOSED;
				*pnum = p;
				objtype = BasicGraphElement::PolygonElement;
				helper.colorBright = BasicElementColorType::LOW_MEDIUM_BRIGHT;
			}
			if((mType == ShapeType::Ribbon) && (lpData != NULL))
				VFlopCPoint(*pnum, lpData, height / 2);
			break;
			}
		case ShapeType::EllipseRibbon2:				// = 108,
		case ShapeType::EllipseRibbon:				// = 107,
			{
			int		p = 0;
			int		d;
			int		x1 = (int)((double)pAdjustValue[0] * 10000 / 21600 + 0.5);
			int		x2 = (int)((double)(pAdjustValue[0] + 2700) * 10000 / 21600 + 0.5);
			POINT	cutline[2];
			CPOINT* lpTmp = new CPOINT[12];
			int		nH, dH, a3;

			if(mType == ShapeType::EllipseRibbon)
				{
				a3 = (int)((double)10000 * (21600 - pAdjustValue[2]) / 21600 + 0.5);
				nH = (int)((double)(21600 - pAdjustValue[1]) * 10000 / 21600 + 0.5);
				dH = (int)((double)10000 * (pAdjustValue[1] + pAdjustValue[2] - 21600) / 21600 + 0.5);
				}
			else
				{
				a3 = (int)((double)10000 * pAdjustValue[2] / 21600 + 0.5);
				nH = (int)((double)pAdjustValue[1] * 10000 / 21600 + 0.5);
				dH = (int)((double)10000 * (21600 - pAdjustValue[1] - pAdjustValue[2]) / 21600 + 0.5);
				}

			if((child == 0) || (child == 1))
				{
				lpData = new CPOINT[14];
				MakeRoundCorner(&(lpData[p]), 0, 10000, 5000, 10000 - a3, 10000, 10000, FALSE, TRUE);
				lpData[1].x = lpData[0].x;
				lpData[1].y = lpData[0].y;

				if(x2 < 5000)
					{
					cutline[0].x = x2;
					cutline[0].y = 0;
					cutline[1].x = x2;
					cutline[1].y = 10000;
					p = SplitBezierSegmentByLine(lpData, cutline, 0);
					}
				else
					p = 4;
				convertPoints(lpData, lpData, p, width, height);

				cutline[0].x = 1270;
				cutline[0].y = 0;
				cutline[1].x = 1270;
				cutline[1].y = 10000;
				MakeRoundCorner(lpTmp, 0, 10000, 5000, 10000 - a3, 10000, 10000, FALSE, TRUE);
				lpTmp[1].x = lpTmp[0].x;
				lpTmp[1].y = lpTmp[0].y;
				d = SplitBezierSegmentByLine(lpTmp, cutline, 0);
				d = 10000 - lpTmp[d-1].y;

				for(i=0 ; i<p ; i++)
					{
					lpData[p+i] = lpData[p-i-1];
					lpData[p+i].y -= height * nH / 10000;
					}
				p = p * 2 + 2;
				lpData[0].a = CP_STAT | CP_LINE;
				lpData[p-2].x = 1270 * width / 10000;
				lpData[p-2].y = height - (nH / 2 + d) * height / 10000;
				lpData[p-2].a = CP_LINE;
				lpData[p-1] = lpData[0];
				lpData[p-1].a = CP_LINE | CP_ENDS | CP_ENDG | CP_CLOSED;
				if(child == 1)
					HFlopCPoint(p, lpData, width / 2);
				*pnum = p;
				objtype = BasicGraphElement::PolygonElement;
				}
			else if((child == 2) || (child == 3))
				{
				MakeRoundCorner(lpTmp, 0, 10000, 5000, 10000 - a3, 10000, 10000, FALSE, TRUE);
				lpTmp[1].x = lpTmp[0].x;
				lpTmp[1].y = lpTmp[0].y;
				cutline[0].x = x2;
				cutline[0].y = 0;
				cutline[1].x = x2;
				cutline[1].y = 10000;
				d = SplitBezierSegmentByLine(lpTmp, cutline, 0);
				cutline[0].x = x1;
				cutline[0].y = 0;
				cutline[1].x = x1;
				cutline[1].y = 10000;
				d = SplitBezierSegmentByLine(lpTmp, cutline, 1);

				lpData = new CPOINT[5];
				lpData[p] = lpTmp[0];
				lpData[p++].a = CP_STAT | CP_LINE;
				lpData[p] = lpTmp[d-1];
				lpData[p++].a = CP_LINE;
				lpData[p] = lpTmp[d-1];
				lpData[p].y -= nH;
				lpData[p++].a = CP_LINE;
				lpData[p] = lpTmp[0];
				lpData[p].y -= nH;
				lpData[p++].a = CP_LINE;
				lpData[0].y -= dH;
				lpData[3].y -= dH;
				lpData[p++] = lpData[0];
				lpData[p-1].a = CP_LINE | CP_ENDS | CP_ENDG | CP_CLOSED;
				convertPoints(lpData, lpData, p, width, height);
				if(child == 3)
					HFlopCPoint(p, lpData, width / 2);
				*pnum = p;
				objtype = BasicGraphElement::PolygonElement;
				helper.colorBright = BasicElementColorType::LOW_MEDIUM_BRIGHT;
			}
			else if(child == 4)
				{
				MakeRoundCorner(lpTmp, 0, 10000-dH, 5000, 10000 - a3 - dH, 10000, 10000, FALSE, TRUE);
				lpTmp[1].x = lpTmp[0].x;
				lpTmp[1].y = lpTmp[0].y;
				cutline[0].x = x1;
				cutline[0].y = 0;
				cutline[1].x = x1;
				cutline[1].y = 10000;
				d = SplitBezierSegmentByLine(lpTmp, cutline, 1);
				lpData = new CPOINT[16];
				memcpy(lpData, lpTmp, sizeof(CPOINT) * d);
				p = d;
				for(i=d-2 ; i>=0 ; i--)
					{
					lpData[p] = lpTmp[i];
					lpData[p].x = 10000 - lpData[p].x;
					p++;
					}
				d = p;
				// nH = (double)pAdjustValue[1] * 10000 / 21600;
				for(int i=d-1 ; i>=0 ; i--)
					{
					lpData[p] = lpData[i];
					lpData[p].y -= nH;
					p++;
					}
				lpData[p++] = lpData[0];
				lpData[0].a = CP_STAT | CP_LINE;
				lpData[p-1].a = CP_LINE | CP_ENDS | CP_ENDG | CP_CLOSED;
				convertPoints(lpData, lpData, p, width, height);
				*pnum = p;
				objtype = BasicGraphElement::PolygonElement;
				}
			if((mType == ShapeType::EllipseRibbon) && (lpData != NULL))
				VFlopCPoint(*pnum, lpData, height / 2);
			delete[] lpTmp;
			break;
			}
		case ShapeType::VerticalScroll:				// = 97,
		case ShapeType::HorizontalScroll:			// = 98,
			if (pAdjustValue[0] == 0) {
				if (child == 0) {
					rect->left = 0;
					rect->top = 0;
					rect->right = width;
					rect->bottom = height;
					objtype = BasicGraphElement::RectElement;
				}
			} else {
				int		p = 0;

				BOOL	bFlipOnly = FALSE; // 어느 한쪽만 플립이 되어 있는 경우
				int		nHeight = (abs(width) < abs(height)) ? width : height;

				if ((width < 0 && height > 0) || (height < 0 && width > 0))
					bFlipOnly = TRUE;				

				if (mType == ShapeType::HorizontalScroll) {
					if (abs(width) < abs(height)) {
						if (bFlipOnly)
							nHeight *= -1;
					}
					p = width;
					width = height;
					height = p;
					p = 0;
				} else {
					if (abs(width) > abs(height)) {
						if (bFlipOnly)
							nHeight *= -1;
					}
				}

				double dx = (double)nHeight * pAdjustValue[0] / (10000 * 5400);				
					
				int a2500 = (int)((double)2500 * pAdjustValue[0] / 5400 + 0.5);
				int a1200 = (int)((double)1250 * pAdjustValue[0] / 5400 + 0.5);
				int a5000 = (int)((double)5000 * pAdjustValue[0] / 5400 + 0.5);
				CPOINT	pTmp[4];

				if (child == 0) {
					rect->left = (int) (1250 * dx);
					if (bFlipOnly) {
						rect->top = height + (int) (1150 * dx);
					} else {
						rect->top = height - (int) (1150 * dx);
					}
					rect->right = (int) (2600 * dx);

					if (bFlipOnly) {
						rect->bottom = height + (int) (2500 * dx);
					} else {
						rect->bottom = height - (int) (2500 * dx);
					}
					objtype = BasicGraphElement::RectElement;
				} else if (child == 1) {
					lpData = new CPOINT[16];
					for (i = 0; i < 16; i++) {
						lpData[i].a = _HorizontalScrollBanner2[i].a;
						lpData[i].x = (int) (dx * _HorizontalScrollBanner2[i].x);
						if (bFlipOnly) {
							lpData[i].y = height + (int) (dx * (10000 - _HorizontalScrollBanner2[i].y));
						} else {
							lpData[i].y = height - (int) (dx * (10000 - _HorizontalScrollBanner2[i].y));
						}
					}
					*pnum = 16;
					objtype = BasicGraphElement::PolygonElement;
					helper.colorBright = BasicElementColorType::LOW_MEDIUM_BRIGHT;
				} else if (child == 2) {
					lpData = new CPOINT[14];
					lpData[p].x = width - (int)((double) (nHeight * a2500) / 10000 + 0.5);
					lpData[p].y = 0;
					lpData[p++].a = CP_STAT | CP_LINE;
					MakeRoundCorner(pTmp, -a2500, -a1200, -a2500- a1200, 0, 10000, 10000, FALSE, TRUE);

					for (i = 0; i < 4; i++) {
						lpData[p].a = pTmp[i].a;
						lpData[p].x = width + (int)((double)nHeight * pTmp[i].x / 10000 + 0.5);

						if (bFlipOnly) {
							lpData[p++].y = height - (int)((double)nHeight * pTmp[i].y / 10000 + 0.5);
						} else {
							lpData[p++].y = height + (int)((double)nHeight * pTmp[i].y / 10000 + 0.5);
						}
					}
					for (i = 0; i < 4; i++) {
						lpData[p].a = pTmp[3-i].a;
						lpData[p].x = (int)((double)nHeight * (pTmp[3-i].x + a2500*2) / 10000 + 0.5);

						if (bFlipOnly) {
							lpData[p++].y = height - (int)((double)nHeight * pTmp[3-i].y / 10000 + 0.5);
						} else {
							lpData[p++].y = height + (int)((double)nHeight * pTmp[3-i].y / 10000 + 0.5);
						}
					}
					MakeRoundCorner(pTmp, a2500, a1200, a2500+a1200, 0, 10000, 10000, FALSE, TRUE);
					for (i = 0; i < 4; i++) {
						lpData[p].a = pTmp[i].a;
						lpData[p].x = (int)((double)nHeight * pTmp[i].x / 10000 + 0.5);
						if (bFlipOnly) {
							lpData[p++].y = -(int)((double)nHeight * pTmp[i].y / 10000 + 0.5);
						} else {
							lpData[p++].y = (int)((double)nHeight * pTmp[i].y / 10000 + 0.5);
						}
					}
					lpData[p] = lpData[0];
					lpData[p].a = CP_LINE | CP_ENDS | CP_ENDG | CP_CLOSED;
					*pnum = p + 1;
					objtype = BasicGraphElement::PolygonElement;
				} else if (child == 3) {
					lpData = new CPOINT[10];
					lpData[p].x = a5000 * nHeight / 10000;
					lpData[p].y = a1200 * nHeight / 10000;
					lpData[p++].a = CP_STAT | CP_LINE;
					lpData[p].x = a5000 * nHeight / 10000;
					lpData[p].y = a2500 * nHeight / 10000;
					lpData[p++].a = CP_STAT | CP_LINE;
					p += MakeRoundCorner(&(lpData[p]), (a5000 - a1200), a2500, (a5000 - a1200 - a1200 / 2), a2500 - a1200/2, nHeight, nHeight, TRUE, FALSE);
					p += MakeRoundCorner(&(lpData[p]), (a5000 - a1200 - a1200 / 2), a2500 - a1200/2, (a5000 - a1200), a1200, nHeight, nHeight, FALSE, TRUE);
					lpData[p] = lpData[0];
					lpData[p].a = CP_LINE | CP_ENDS | CP_ENDG | CP_CLOSED;
					*pnum = p + 1;

					for (i = 0; i < p + 1; i++) {
						if (bFlipOnly)
							lpData[i].y *= -1;
					}

					objtype = BasicGraphElement::PolygonElement;
					helper.colorBright = BasicElementColorType::LOW_MEDIUM_BRIGHT;
				} else if (child == 4) {
					lpData = new CPOINT[15];
					MakeRoundCorner(pTmp, -a1200, 0, 0, a1200, nHeight, nHeight, TRUE, FALSE);
					for (i = 0; i < 3; i++) {
						lpData[p].a = pTmp[i].a;
						lpData[p].x = width + pTmp[i].x;
						lpData[p++].y = pTmp[i].y;
					}
					MakeRoundCorner(pTmp, 0, a1200, -a1200, a2500, nHeight, nHeight, FALSE, TRUE);
					for (i = 0; i < 4; i++) {
						lpData[p].a = pTmp[i].a;
						lpData[p].x = width + pTmp[i].x;
						lpData[p++].y = pTmp[i].y;
					}
					p += MakeRoundCorner(&(lpData[p]), (a5000 - a1200), a2500, a5000, a1200, nHeight, nHeight, TRUE, FALSE);
					p += MakeRoundCorner(&(lpData[p]), a5000, a1200, (a5000 - a1200), 0, nHeight, nHeight, FALSE, TRUE);
					lpData[p] = lpData[0];
					lpData[p].a = CP_LINE | CP_ENDS | CP_ENDG | CP_CLOSED;
					*pnum = p + 1;
					
					for (i = 0; i < p + 1; i++) {
						if (bFlipOnly)
							lpData[i].y *= -1;
					}

					objtype = BasicGraphElement::PolygonElement;
				}

				if (mType == ShapeType::HorizontalScroll) {
					if (lpData != NULL) {
						for (i = 0; i < *pnum; i++) {
							p = lpData[i].x;
							lpData[i].x = lpData[i].y;
							lpData[i].y = p;
						}
					} else {
						p = rect->top;
						rect->top = rect->left;
						rect->left = p;
						p = rect->right;
						rect->right = rect->bottom;
						rect->bottom = p;
					}
				}
			}
			break;
		case ShapeType::Wave:						// = 64,
			if(child == 0)
				{
				lpData = new CPOINT[21];
				memcpy(lpData, _BannerWave, sizeof(CPOINT) * 21);
				modifyWaveBanner(10, lpData, pAdjustValue[0], pAdjustValue[1], 0);
				modifyWaveBanner(10, &(lpData[10]), pAdjustValue[0], pAdjustValue[1], 1);
				lpData[20].x = lpData[0].x;
				lpData[20].y = lpData[0].y;
				convertPoints(lpData, lpData, 21, width, height);
				lpData[0].a = CP_STAT | CP_LINE;
				lpData[20].a = CP_LINE | CP_ENDS | CP_ENDG | CP_CLOSED;
				*pnum = 21;
				objtype = BasicGraphElement::PolygonElement;
				}
			break;
		case ShapeType::DoubleWave:					// = 188,
			if(child == 0)
				{
				lpData = new CPOINT[33];
				memcpy(lpData, _BannerDoubleWave, sizeof(CPOINT) * 33);
				modifyWaveBanner(16, lpData, pAdjustValue[0], pAdjustValue[1], 0);
				modifyWaveBanner(16, &(lpData[16]), pAdjustValue[0], pAdjustValue[1], 1);
				lpData[32].x = lpData[0].x;
				lpData[32].y = lpData[0].y;
				convertPoints(lpData, lpData, 33, width, height);
				*pnum = 33;
				objtype = BasicGraphElement::PolygonElement;
				}
			break;
		case ShapeType::FlowChartDocument:			// = 114,
			if(child == 0)
				{
				lpData = new CPOINT[13];
				convertPoints(lpData, _FlowChartDocument, 13, width, height);
				*pnum = 13;
				objtype = BasicGraphElement::PolygonElement;
				}
			break;
		case ShapeType::FlowChartMultidocument:		// = 115,
			if(child == 0)
				{
				lpData = new CPOINT[13 * 3];
				convertPoints(lpData, _FlowChartMultidocument, 13, width, height);
				convertPoints(&(lpData[13]), _FlowChartMultidocument, 13, width, height, -680, 850);
				convertPoints(&(lpData[26]), _FlowChartMultidocument, 13, width, height, -1250, 1700);

				lpData[13 - 1].a = CP_LINE | CP_ENDS | CP_CLOSED;
				lpData[13*2 - 1].a = CP_LINE | CP_ENDS | CP_CLOSED;
				lpData[13*3 - 1].a = CP_LINE | CP_ENDS | CP_ENDG | CP_CLOSED;
				*pnum = 13 * 3;
				objtype = BasicGraphElement::PolygonElement;
				}
			break;
		case ShapeType::FlowChartTerminator:			// = 116,
			if(child == 0)
				{
				lpData = new CPOINT[15];
				MakeRoundCorner(&(lpData[0]), 1500, 0, 0, 5000, width, height, TRUE, FALSE);
				MakeRoundCorner(&(lpData[3]), 0, 5000, 1500, 10000, width, height, FALSE, TRUE);
				MakeRoundCorner(&(lpData[7]), 8500, 10000, 10000, 5000, width, height, TRUE, FALSE);
				MakeRoundCorner(&(lpData[10]), 10000, 5000, 8500, 0, width, height, FALSE, TRUE);
				lpData[0].a = CP_STAT | CP_LINE;
				lpData[14] = lpData[0];
				lpData[14].a = CP_LINE | CP_ENDS | CP_ENDG | CP_CLOSED;
				*pnum = 15;
				objtype = BasicGraphElement::PolygonElement;
				}
			break;

		case ShapeType::FlowChartPunchedTape:		// = 122,
			if(child == 0)
				{
				lpData = new CPOINT[27];

				MakeRoundCorner(&(lpData[0]), 0, 1000, 2500, 2000, width, height, FALSE, FALSE);
				MakeRoundCorner(&(lpData[3]), 2500, 2000, 5000, 1000, width, height, TRUE, FALSE);
				MakeRoundCorner(&(lpData[6]), 5000, 1000, 7500, 0, width, height, FALSE, FALSE);
				MakeRoundCorner(&(lpData[9]), 7500, 0, 10000, 1000, width, height, TRUE, TRUE);
				MakeRoundCorner(&(lpData[13]), 10000, 9000, 7500, 8000, width, height, FALSE, FALSE);
				MakeRoundCorner(&(lpData[16]), 7500, 8000, 5000, 9000, width, height, TRUE, FALSE);
				MakeRoundCorner(&(lpData[19]), 5000, 9000, 2500, 10000, width, height, FALSE, FALSE);
				MakeRoundCorner(&(lpData[22]), 2500, 10000, 0, 9000, width, height, TRUE, TRUE);
				lpData[0].a = CP_STAT | CP_LINE;
				lpData[26] = lpData[0];
				lpData[26].a = CP_LINE | CP_ENDS | CP_ENDG | CP_CLOSED;
				*pnum = 27;
				objtype = BasicGraphElement::PolygonElement;
				}
			break;
		case ShapeType::FlowChartSummingJunction:	// = 123,
			if(child == 0)
				{
//				lpData = new CPOINT[17];
				lpData = new CPOINT[24];
				convertPoints(lpData, _FlowChartSummingJunction, 24, width, height);
/*				MakeRoundCorner(&(lpData[0]), 5000, 0, 10000, 5000, width, height, TRUE, FALSE);
				MakeRoundCorner(&(lpData[3]), 10000, 5000, 5000, 10000, width, height, FALSE, FALSE);
				MakeRoundCorner(&(lpData[6]), 5000, 10000, 0, 5000, width, height, TRUE, FALSE);
				MakeRoundCorner(&(lpData[9]), 0, 5000, 5000, 0, width, height, FALSE, TRUE);
				lpData[0].a = CP_STAT | CP_LINE;
				lpData[12].a = CP_ENDS | CP_CLOSED;
				float bias = 2.f * (float)(sqrt((float)2) - 1.f) / 3.f;
				lpData[13].x = (int)((width / 4) * (2.f - sqrt((float)2)));
				lpData[13].y = (int)((height / 4) * (2.f - sqrt((float)2)));
				lpData[13].a = CP_LINE;
				lpData[14].x = (int)((width / 4) * (sqrt((float)2) + 2.f));
				lpData[14].y = (int)((height / 4) * (sqrt((float)2) + 2.f));
				lpData[14].a = CP_ENDS | CP_CLOSED;
								
				bias = 2 * (float)(sqrt((float)2) - 1.f) / 3.f;
				lpData[15].x = (int)((width / 4) * (sqrt((float)2) + 2.f));
				lpData[15].y = (int)((height / 4) * (2.f - sqrt((float)2)));
				lpData[15].a = CP_LINE;
				lpData[16].x = (int)((width / 4) * (2.f - sqrt((float)2)));
				lpData[16].y = (int)((height / 4) * (sqrt((float)2) + 2.f));
				lpData[16].a = CP_ENDS | CP_ENDG | CP_CLOSED;
*/
				*pnum = 24;
				objtype = BasicGraphElement::PolygonElement;
				}
			break;
		case ShapeType::FlowChartOr:					// = 124,
			if(child == 0)
				{
				lpData = new CPOINT[17];
//				convertPoints(lpData, _FlowChartOr, 24, width, height);
				MakeRoundCorner(&(lpData[0]), 5000, 0, 10000, 5000, width, height, TRUE, FALSE);
				MakeRoundCorner(&(lpData[3]), 10000, 5000, 5000, 10000, width, height, FALSE, FALSE);
				MakeRoundCorner(&(lpData[6]), 5000, 10000, 0, 5000, width, height, TRUE, FALSE);
				MakeRoundCorner(&(lpData[9]), 0, 5000, 5000, 0, width, height, FALSE, TRUE);
				lpData[0].a = CP_STAT | CP_LINE;
				lpData[12].a = CP_ENDS | CP_CLOSED;
				lpData[13].x = width / 2;
				lpData[13].y = 0;
				lpData[13].a = CP_LINE;
				lpData[14].x = width / 2;
				lpData[14].y = height;
				lpData[14].a = CP_ENDS | CP_CLOSED;

				lpData[15].x = 0;
				lpData[15].y = height / 2;
				lpData[15].a = CP_LINE;
				lpData[16].x = width;
				lpData[16].y = height / 2;
				lpData[16].a = CP_ENDS | CP_ENDG | CP_CLOSED;

				*pnum = 17;
				objtype = BasicGraphElement::PolygonElement;
				}
			break;
		case ShapeType::FlowChartOnlineStorage:		// = 130,
			if(child == 0)
				{
				lpData = new CPOINT[15];
				MakeRoundCorner(&(lpData[0]), 10000, 0, 8300, 5000, width, height, TRUE, FALSE);
				MakeRoundCorner(&(lpData[3]), 8300, 5000, 10000, 10000, width, height, FALSE, TRUE);
				MakeRoundCorner(&(lpData[7]), 1700, 10000, 0, 5000, width, height, TRUE, FALSE);
				MakeRoundCorner(&(lpData[10]), 0, 5000, 1700, 0, width, height, FALSE, TRUE);
				lpData[0].a = CP_STAT | CP_LINE;
				lpData[14] = lpData[0];
				lpData[14].a = CP_LINE | CP_ENDS | CP_ENDG | CP_CLOSED;
				*pnum = 15;
				objtype = BasicGraphElement::PolygonElement;
				}
			break;
		case ShapeType::FlowChartMagneticTape:		// = 131,
			if(child == 0)
				{
				lpData = new CPOINT[16];
				convertPoints(lpData, _FlowChartMagneticTape, 16, width, height);
				*pnum = 16;
				objtype = BasicGraphElement::PolygonElement;
				}
			break;

		case ShapeType::FlowChartMagneticDisk:		// = 132,
			if(child == 0) {
				lpData = new CPOINT[15];
				*pnum = MakeCanSegment(0, lpData, 1500, width, height);				
				lpData[*pnum-1].a = CP_LINE | CP_ENDS | CP_ENDG | CP_CLOSED;
				objtype = BasicGraphElement::PolygonElement;
			} else if(child == 1) {
				lpData = new CPOINT[13];
				*pnum = MakeCanSegment(1, lpData, 1500, width, height);
				lpData[*pnum-1].a = CP_LINE | CP_ENDS | CP_ENDG | CP_CLOSED;
				objtype = BasicGraphElement::PolygonElement;
			}
			break;
		case ShapeType::FlowChartMagneticDrum:		// = 133,
			if(child == 0) {
				int depth = 1700;
				lpData = new CPOINT[15];

				MakeRoundCorner(&(lpData[0]), 10000-depth, 0, 10000 - depth * 2, 5000, width, height, TRUE, FALSE);
				MakeRoundCorner(&(lpData[3]), 10000 - depth * 2, 5000, 10000 - depth, 10000, width, height, FALSE, TRUE);

				lpData[0].a = CP_STAT | CP_LINE;
				lpData[5].a = CP_LINE;
				lpData[6].x = (LONG)( (double)width * (10000 - depth) / 10000 );
				lpData[6].y = height;
				lpData[6].a = CP_LINE;
				MakeRoundCorner(&(lpData[7]), depth, 10000, 0, 5000, width, height, TRUE, TRUE);
				MakeRoundCorner(&(lpData[10]), 0, 5000, depth, 0, width, height, FALSE, TRUE);				
				lpData[13].a = CP_LINE;
				lpData[14] = lpData[0];
				lpData[14].a = CP_LINE | CP_ENDS | CP_ENDG | CP_CLOSED;
				*pnum = 15;
				objtype = BasicGraphElement::PolygonElement;
			} else if(child == 1) {
				int depth = 1700;
				lpData = new CPOINT[13];

				MakeRoundCorner(&(lpData[0]), 10000-depth, 0, 10000, 5000, width, height, TRUE, FALSE);
				MakeRoundCorner(&(lpData[3]), 10000, 5000, 10000-depth, 10000, width, height, FALSE, FALSE);
				MakeRoundCorner(&(lpData[6]), 10000-depth, 10000, 10000-depth*2, 5000, width, height, TRUE, FALSE);
				MakeRoundCorner(&(lpData[9]), 10000-depth*2, 5000, 10000-depth, 0, width, height, FALSE, TRUE);
				lpData[0].a = CP_STAT | CP_LINE;
				lpData[12].a = CP_LINE | CP_ENDS | CP_ENDG | CP_CLOSED;
				*pnum = 13;
				objtype = BasicGraphElement::PolygonElement;
			}
			break;
		case ShapeType::FlowChartDisplay:			// = 134,
			if(child == 0)
				{
				lpData = new CPOINT[11];
				MakeRoundCorner(&(lpData[0]), 8300, 0, 10000, 5000, width, height, TRUE, FALSE);
				MakeRoundCorner(&(lpData[3]), 10000, 5000, 8300, 10000, width, height, FALSE, TRUE);
				lpData[0].a = CP_STAT | CP_LINE;
				lpData[7].x = (int)((double)1700 * width / 10000 + 0.5);
				lpData[7].y = height;
				lpData[7].a = CP_LINE;
				lpData[8].x = 0;
				lpData[8].y = (int)((double)height / 2 + 0.5);
				lpData[8].a = CP_LINE;
				lpData[9].x = (int)((double)1700 * width / 10000 + 0.5);
				lpData[9].y = 0;
				lpData[9].a = CP_LINE;
				lpData[10] = lpData[0];
				lpData[10].a = CP_LINE | CP_ENDS | CP_ENDG | CP_CLOSED;
				*pnum = 11;
				objtype = BasicGraphElement::PolygonElement;
				}
			break;
		case ShapeType::FlowChartDelay:		// = 135,
			if(child == 0)
				{
				lpData = new CPOINT[10];
				MakeRoundCorner(&(lpData[0]), 5000, 0, 10000, 5000, width, height, TRUE, FALSE);
				MakeRoundCorner(&(lpData[3]), 10000, 5000, 5000, 10000, width, height, FALSE, TRUE);
				lpData[0].a = CP_STAT | CP_LINE;
				lpData[7].x = 0;
				lpData[7].y = height;
				lpData[7].a = CP_LINE;
				lpData[8].x = 0;
				lpData[8].y = 0;
				lpData[8].a = CP_LINE;
				lpData[9] = lpData[0];
				lpData[9].a = CP_LINE | CP_ENDS | CP_ENDG | CP_CLOSED;
				*pnum = 10;
				objtype = BasicGraphElement::PolygonElement;
				}
			break;
		}
	*lppPts = lpData;
	return objtype;
}

/*************************************************
KShapeSmartCalloutObject
*************************************************/
KShapeSmartCalloutObject::KShapeSmartCalloutObject(ShapeType type)
	:KShapeSmartRectObject(type)
{

}

KShapeSmartCalloutObject::KShapeSmartCalloutObject(ShapeType type, RECT& rect, DWORD fillcolor, OutlineStyle linestyle, LONG* adjust)
: KShapeSmartRectObject(type, rect, fillcolor, linestyle, adjust)
{

}

KShapeSmartCalloutObject::~KShapeSmartCalloutObject()
{
}

int KShapeSmartCalloutObject::GetChildNumber()
{
	// TRACE("Callout:%d mAdjustValue[0]=%d,mAdjustValue[1]=%d,mAdjustValue[2]=%d,mAdjustValue[3]=%d,mAdjustValue[4]=%d,mAdjustValue[5]=%d,mAdjustValue[6]=%d", mType, mAdjustValue[0],mAdjustValue[1],mAdjustValue[2],mAdjustValue[3],mAdjustValue[4],mAdjustValue[5],mAdjustValue[6]);

	switch (mType)
		{
		case ShapeType::Callout1:						// = 41,
		case ShapeType::Callout2:						// = 42,
		case ShapeType::Callout3:						// = 43,
		case ShapeType::AccentCallout1:				// = 44,
		case ShapeType::AccentCallout2:				// = 45,
		case ShapeType::AccentCallout3:				// = 46,
		case ShapeType::BorderCallout1:				// = 47,
		case ShapeType::BorderCallout2:				// = 48,
		case ShapeType::BorderCallout3:				// = 49,
		case ShapeType::AccentBorderCallout1:			// = 50,
		case ShapeType::AccentBorderCallout2:			// = 51,
		case ShapeType::AccentBorderCallout3:			// = 52,
		case ShapeType::Callout90:						// = 178,
		case ShapeType::AccentCallout90:				// = 179,
		case ShapeType::AccentBorderCallout90:			// = 181,
		case ShapeType::BorderCallout90:				// = 180,
			return(2);
		case ShapeType::CloudCallout:					// = 106,
			return(4);
		case ShapeType::WedgeRectCallout:				// = 61,
		case ShapeType::WedgeRRectCallout:				// = 62,
		case ShapeType::WedgeEllipseCallout:			// = 63,
			return(1);
		default:
			break;
		}
	return(0);
}

BOOL KShapeSmartCalloutObject::HasPen(int child)
{
	if(child == 0)
		{
		if( (mType == ShapeType::AccentCallout90) ||
			(mType == ShapeType::AccentCallout1) ||
			(mType == ShapeType::AccentCallout2) ||
			(mType == ShapeType::AccentCallout3) ||
			(mType == ShapeType::Callout90) ||
			(mType == ShapeType::Callout1) ||
			(mType == ShapeType::Callout2) ||
			(mType == ShapeType::Callout3) )
			return(FALSE);
		}
	return(TRUE);
}

BOOL KShapeSmartCalloutObject::HasBrush(int child)
{
	switch (mType)
	{
		case ShapeType::Callout1:					// = 41,
		case ShapeType::Callout2:					// = 42,
		case ShapeType::Callout3:					// = 43,
		case ShapeType::AccentCallout1:				// = 44,
		case ShapeType::AccentCallout2:				// = 45,
		case ShapeType::AccentCallout3:				// = 46,
		case ShapeType::BorderCallout1:				// = 47,
		case ShapeType::BorderCallout2:				// = 48,
		case ShapeType::BorderCallout3:				// = 49,
		case ShapeType::AccentBorderCallout1:		// = 50,
		case ShapeType::AccentBorderCallout2:		// = 51,
		case ShapeType::AccentBorderCallout3:		// = 52,
		case ShapeType::Callout90:					// = 178,
		case ShapeType::AccentCallout90:				// = 179,
		case ShapeType::AccentBorderCallout90:		// = 181,
		case ShapeType::BorderCallout90:				// = 180,
			if(child != 0)
				return FALSE;
			break;
		default:
			break;
	}
	return TRUE; 
}

// Polygon 86
CPOINT KShapeSmartCalloutObject::_CloudCallout[] = 
{
	{ 6910, 560, 0x3 },
	{ 7100, 290, 0x4 },
	{ 7380, 0, 0x4 },
	{ 7800, 0, 0x2 },
	{ 8490, 40, 0x4 },
	{ 8810, 710, 0x4 },
	{ 8900, 1300, 0x2 },
	{ 9730, 1550, 0x4 },
	{ 9920, 2850, 0x4 },
	{ 9700, 3600, 0x2 },
	{ 9900, 3980, 0x4 },
	{ 10020, 4460, 0x4 },
	{ 10000, 4900, 0x2 },
	{ 9990, 5640, 0x4 },
	{ 9660, 6740, 0x4 },
	{ 8680, 6940, 0x2 },
	{ 8630, 8250, 0x4 },
	{ 7710, 9320, 0x4 },
	{ 6600, 8500, 0x2 },
	{ 6480, 9250, 0x4 },
	{ 5890, 10000, 0x4 },
	{ 5100, 10000, 0x2 },
	{ 4430, 10010, 0x4 },
	{ 4070, 9510, 0x4 },
	{ 3820, 9060, 0x2 },
	{ 2980, 9760, 0x4 },
	{ 1850, 9340, 0x4 },
	{ 1360, 8170, 0x2 },
	{ 810, 8240, 0x4 },
	{ 420, 7830, 0x4 },
	{ 290, 7210, 0x2 },
	{ 190, 6580, 0x4 },
	{ 310, 6190, 0x4 },
	{ 510, 5900, 0x2 },
	{ 280, 5730, 0x4 },
	{ 0, 5230, 0x4 },
	{ 0, 4700, 0x2 },
	{ 0, 4210, 0x4 },
	{ 230, 3500, 0x4 },
	{ 910, 3340, 0x2 },
	{ 710, 2260, 0x4 },
	{ 1700, 180, 0x4 },
	{ 3280, 1220, 0x2 },
	{ 3630, 170, 0x4 },
	{ 4690, 40, 0x4 },
	{ 5200, 800, 0x2 },
	{ 5310, 440, 0x4 },
	{ 5690, 0, 0x4 },
	{ 6100, 0, 0x2 },
	{ 6560, 0, 0x4 },
	{ 6770, 280, 0x4 },
	{ 6910, 560, CP_ENDS | CP_CLOSED },
	{ 9330, 4190, 0x1 },
	{ 9440, 4080, 0x4 },
	{ 9600, 3820, 0x4 },
	{ 9690, 3610, CP_ENDS },
	{ 8900, 1300, 0x1 },
	{ 8890, 1580, CP_ENDS },
	{ 6910, 560, 0x1 },
	{ 6910, 560, 0x4 },
	{ 6770, 820, 0x4 },
	{ 6740, 920, CP_ENDS },
	{ 3270, 1220, 0x1 },
	{ 3570, 1540, CP_ENDS },
	{ 5200, 790, 0x1 },
	{ 5110, 1100, CP_ENDS },
	{ 900, 3340, 0x1 },
	{ 970, 3660, CP_ENDS },
	{ 510, 5900, 0x1 },
	{ 510, 5900, 0x4 },
	{ 790, 6120, 0x4 },
	{ 1090, 6090, CP_ENDS },
	{ 1360, 8170, 0x1 },
	{ 1360, 8170, 0x4 },
	{ 1480, 8170, 0x4 },
	{ 1620, 8090, CP_ENDS },
	{ 3820, 9060, 0x1 },
	{ 3820, 9060, 0x4 },
	{ 3660, 8710, 0x4 },
	{ 3670, 8660, CP_ENDS },
	{ 6610, 8510, 0x1 },
	{ 6700, 8060, CP_ENDS },
	{ 8690, 6950, 0x1 },
	{ 8690, 6950, 0x4 },
	{ 8630, 5780, 0x4 },
	{ 7920, 5340, CP_ENDS | CP_ENDG },
};

BasicGraphElement KShapeSmartCalloutObject::GetChild(int child, ShapeDrawerHelper& helper, LPRECT rect, CPOINT** lppPts, int* pnum, LONG* pAdjustValue)
{
	int		width = (helper.renderRect.right - helper.renderRect.left) - 1;
	int		height = (helper.renderRect.bottom - helper.renderRect.top) - 1;
	BasicGraphElement		objtype = BasicGraphElement::None;
	CPOINT	*lpData = NULL;
	int		p = 0;

	*pnum = 0;
	helper.colorBright = BasicElementColorType::NORMAL;

	switch (mType)
		{
		case ShapeType::WedgeRectCallout:			// = 61,
		case ShapeType::WedgeRRectCallout:			// = 62,
			if(child == 0)
				{
				if( (pAdjustValue[0] >= 0) && (pAdjustValue[0] <= 21600) &&
					(pAdjustValue[1] >= 0) && (pAdjustValue[1] <= 21600) )
					{
					rect->left = 0;
					rect->top = 0;
					rect->right = width;
					rect->bottom = height;
					objtype = BasicGraphElement::RectElement;
					}
				else
					{
					int angle = GetAngleFromOrigin(pAdjustValue[0] - 10800, 10800 - pAdjustValue[1]);

					// TRACE("CallOut hangle angle=%d", angle);
					if(mType == ShapeType::WedgeRRectCallout)
						lpData = new CPOINT[20];
					else
						lpData = new CPOINT[8];
					if((angle >= 450) && (angle < 1350))
						{
						if(mType == ShapeType::WedgeRRectCallout)
							p = MakeRoundCorner(&(lpData[0]), 0, 2000, 2000, 0, width, height, FALSE, TRUE);
						else
							{
							lpData[0].x = 0;
							lpData[0].y = 0;
							lpData[0].a = CP_STAT | CP_LINE;
							p = 1;
							}
						lpData[p].x = (LONG)( (double)((pAdjustValue[0] > 10800) ? 6 : 2) * width / 10 );
						lpData[p].y = 0;
						lpData[p++].a = CP_LINE;
						lpData[p].x = (LONG)( (double)pAdjustValue[0] * width / 21600 );
						lpData[p].y = (LONG)( (double)pAdjustValue[1] * height / 21600 );
						lpData[p++].a = CP_LINE;
						lpData[p].x = (LONG)( (double)((pAdjustValue[0] > 10800) ? 8 : 4) * width / 10 );
						lpData[p].y = 0;
						lpData[p++].a = CP_LINE;
						if(mType == ShapeType::WedgeRRectCallout)
							{
							p += MakeRoundCorner(&(lpData[p]), 8000, 0, 10000, 2000, width, height, TRUE, TRUE);
							p += MakeRoundCorner(&(lpData[p]), 10000, 8000, 8000, 10000, width, height, FALSE, TRUE);
							p += MakeRoundCorner(&(lpData[p]), 2000, 10000, 0, 8000, width, height, TRUE, TRUE);
							}
						else
							{
							lpData[p].x = width;
							lpData[p].y = 0;
							lpData[p++].a = CP_LINE;
							lpData[p].x = width;
							lpData[p].y = height;
							lpData[p++].a = CP_LINE;
							lpData[p].x = 0;
							lpData[p].y = height;
							lpData[p++].a = CP_LINE;
							}
						lpData[p] = lpData[0];
						lpData[p++].a = CP_LINE | CP_ENDS | CP_ENDG | CP_CLOSED;
						}
					else if((angle >= 1350) && (angle < 2250))
						{
						if(mType == ShapeType::WedgeRRectCallout)
							{
							p = MakeRoundCorner(&(lpData[0]), 0, 2000, 2000, 0, width, height, FALSE, TRUE);
							p += MakeRoundCorner(&(lpData[p]), 8000, 0, 10000, 2000, width, height, TRUE, TRUE);
							p += MakeRoundCorner(&(lpData[p]), 10000, 8000, 8000, 10000, width, height, FALSE, TRUE);
							p += MakeRoundCorner(&(lpData[p]), 2000, 10000, 0, 8000, width, height, TRUE, TRUE);
							}
						else
							{
							lpData[0].x = 0;
							lpData[0].y = 0;
							lpData[0].a = CP_STAT | CP_LINE;
							lpData[1].x = width;
							lpData[1].y = 0;
							lpData[1].a = CP_LINE;
							lpData[2].x = width;
							lpData[2].y = height;
							lpData[2].a = CP_LINE;
							lpData[3].x = 0;
							lpData[3].y = height;
							lpData[3].a = CP_LINE;
							p = 4;
							}
						lpData[p].x = 0;
						lpData[p].y = (LONG)( (double)((pAdjustValue[1] > 10800) ? 8 : 4) * height / 10 );
						lpData[p++].a = CP_LINE;
						lpData[p].x = (LONG)( (double)pAdjustValue[0] * width / 21600);
						lpData[p].y = (LONG)( (double)pAdjustValue[1] * height / 21600);
						lpData[p++].a = CP_LINE;
						lpData[p].x = 0;
						lpData[p].y = (LONG)( (double)((pAdjustValue[1] > 10800) ? 6 : 2) * height / 10 );
						lpData[p++].a = CP_LINE;
						lpData[p] = lpData[0];
						lpData[p++].a = CP_LINE | CP_ENDS | CP_ENDG | CP_CLOSED;
						}
					else if((angle >= 2250) && (angle < 3150))
						{
						if(mType == ShapeType::WedgeRRectCallout)
							{
							p = MakeRoundCorner(&(lpData[0]), 0, 2000, 2000, 0, width, height, FALSE, TRUE);
							p += MakeRoundCorner(&(lpData[p]), 8000, 0, 10000, 2000, width, height, TRUE, TRUE);
							p += MakeRoundCorner(&(lpData[p]), 10000, 8000, 8000, 10000, width, height, FALSE, TRUE);
							}
						else
							{
							lpData[0].x = 0;
							lpData[0].y = 0;
							lpData[0].a = CP_STAT | CP_LINE;
							lpData[1].x = width;
							lpData[1].y = 0;
							lpData[1].a = CP_LINE;
							lpData[2].x = width;
							lpData[2].y = height;
							lpData[2].a = CP_LINE;
							p = 3;
							}
						lpData[p].x = (LONG)( (double)((pAdjustValue[0] > 10800) ? 8 : 4) * width / 10 );
						lpData[p].y = height;
						lpData[p++].a = CP_LINE;
						lpData[p].x = (LONG)( (double)pAdjustValue[0] * width / 21600 );
						lpData[p].y = (LONG)( (double)pAdjustValue[1] * height / 21600 );
						lpData[p++].a = CP_LINE;
						lpData[p].x = (LONG)( (double)((pAdjustValue[0] > 10800) ? 6 : 2) * width / 10 );
						lpData[p].y = height;
						lpData[p++].a = CP_LINE;

						if(mType == ShapeType::WedgeRRectCallout)
							p += MakeRoundCorner(&(lpData[p]), 2000, 10000, 0, 8000, width, height, TRUE, TRUE);
						else
							{
							lpData[p].x = 0;
							lpData[p].y = height;
							lpData[p++].a = CP_LINE;
							}
						lpData[p] = lpData[0];
						lpData[p++].a = CP_LINE | CP_ENDS | CP_ENDG | CP_CLOSED;
						}
					else
						{
						if(mType == ShapeType::WedgeRRectCallout)
							{
							p = MakeRoundCorner(&(lpData[0]), 0, 2000, 2000, 0, width, height, FALSE, TRUE);
							p += MakeRoundCorner(&(lpData[p]), 8000, 0, 10000, 2000, width, height, TRUE, TRUE);
							}
						else
							{
							lpData[0].x = 0;
							lpData[0].y = 0;
							lpData[0].a = CP_STAT | CP_LINE;
							lpData[1].x = width;
							lpData[1].y = 0;
							lpData[1].a = CP_LINE;
							p = 2;
							}
						lpData[p].x = width;
						lpData[p].y = (LONG)((double)((pAdjustValue[1] > 10800) ? 6 : 2) * height / 10);
						lpData[p++].a = CP_LINE;
						lpData[p].x = (LONG)((double)pAdjustValue[0] * width / 21600);
						lpData[p].y = (LONG)((double)pAdjustValue[1] * height / 21600);
						lpData[p++].a = CP_LINE;
						lpData[p].x = width;
						lpData[p].y = (LONG)((double)((pAdjustValue[1] > 10800) ? 8 : 4) * height / 10);
						lpData[p++].a = CP_LINE;
						if(mType == ShapeType::WedgeRRectCallout)
							{
							p += MakeRoundCorner(&(lpData[p]), 10000, 8000, 8000, 10000, width, height, FALSE, TRUE);
							p += MakeRoundCorner(&(lpData[p]), 2000, 10000, 0, 8000, width, height, TRUE, TRUE);
							}
						else
							{
							lpData[p].x = width;
							lpData[p].y = height;
							lpData[p++].a = CP_LINE;
							lpData[p].x = 0;
							lpData[p].y = height;
							lpData[p++].a = CP_LINE;
							}
						lpData[p] = lpData[0];
						lpData[p++].a = CP_LINE | CP_ENDS | CP_ENDG | CP_CLOSED;
						}
					*pnum = p;
					objtype = BasicGraphElement::PolygonElement;
					}
				}
			break;
		case ShapeType::WedgeEllipseCallout:			// = 63,
			if(child == 0)
				{
				int angle = GetAngleFromOrigin(pAdjustValue[0] - 10800, 10800 - pAdjustValue[1]);
				int radius = GetDistance(pAdjustValue[0] - 10800, 10800 - pAdjustValue[1]);
				if(radius <= 10800)
					{
					rect->left = 0;
					rect->top = 0;
					rect->right = width;
					rect->bottom = height;
					objtype = BasicGraphElement::EllipseElement;
					}
				else
					{
					int a1 = angle - 100 + 3600;
					int a2 = angle + 100;

					lpData = new CPOINT[20];
					p = GetArcPolyline(lpData, 5000, 5000, 5000, a2, a1);
					lpData[p].x = (LONG)( (double)pAdjustValue[0] * 10000 / 21600 );
					lpData[p].y = (LONG)( (double)pAdjustValue[1] * 10000 / 21600 );
					lpData[p++].a = CP_LINE;
					lpData[p] = lpData[0];
					lpData[0].a = CP_STAT | CP_LINE;
					lpData[p++].a = CP_LINE | CP_ENDS | CP_ENDG | CP_CLOSED;

					convertPoints(lpData, lpData, p, width, height);
					*pnum = p;
					objtype = BasicGraphElement::PolygonElement;
					}
				}
			break;

		case ShapeType::Callout1:					// = 41,
		case ShapeType::Callout2:					// = 42,
		case ShapeType::Callout3:					// = 43,
		case ShapeType::AccentCallout1:				// = 44,
		case ShapeType::AccentCallout2:				// = 45,
		case ShapeType::AccentCallout3:				// = 46,
		case ShapeType::BorderCallout1:				// = 47,
		case ShapeType::BorderCallout2:				// = 48,
		case ShapeType::BorderCallout3:				// = 49,
		case ShapeType::AccentBorderCallout1:		// = 50,
		case ShapeType::AccentBorderCallout2:		// = 51,
		case ShapeType::AccentBorderCallout3:		// = 52,
		case ShapeType::Callout90:					// = 178,
		case ShapeType::AccentCallout90:				// = 179,
		case ShapeType::AccentBorderCallout90:		// = 181,
		case ShapeType::BorderCallout90:				// = 180,
			if(child == 0) 
				{
				rect->left = 0;
				rect->top = 0;
				rect->right = width;
				rect->bottom = height;

				objtype = BasicGraphElement::RectElement;
				}
			else if(child == 1)
				{
				lpData = new CPOINT[6];
				p = 0;
				if( (mType == ShapeType::Callout1) ||
					(mType == ShapeType::AccentCallout1) ||
					(mType == ShapeType::BorderCallout1) ||
					(mType == ShapeType::AccentBorderCallout1) )
					{
					lpData[0].x = (int)((double)pAdjustValue[0] * width / 21600 + 0.5);
					lpData[0].y = (int)((double)pAdjustValue[1] * height / 21600 + 0.5);
					lpData[0].a = CP_STAT | CP_LINE;
					lpData[1].x = (pAdjustValue[0] > 0) ? (width + CALLOUT_X_MARGIN) : -CALLOUT_X_MARGIN;
					lpData[1].y = CALLOUT_Y_MARGIN;
					lpData[1].a = CP_LINE | CP_ENDS;
	
					p = 2;
					}
				else if( (mType == ShapeType::Callout2) ||
					(mType == ShapeType::AccentCallout2) ||
					(mType == ShapeType::BorderCallout2) ||
					(mType == ShapeType::AccentBorderCallout2) )
					{
					lpData[0].x = (int)((double)pAdjustValue[0] * width / 21600 + 0.5);
					lpData[0].y = (int)((double)pAdjustValue[1] * height / 21600 + 0.5);
					lpData[0].a = CP_STAT | CP_LINE;

					if(pAdjustValue[0] < 0) {
						lpData[1].x = (LONG)( (double)lpData[0].x / 2  + 0.5);
					} else {
						lpData[1].x = (LONG)( (double)(lpData[0].x + abs(width)) / 2 + 0.5);
					}
					lpData[1].y = CALLOUT_Y_MARGIN;
					lpData[1].a = CP_LINE;
					lpData[2].x = (pAdjustValue[0] > 0) ? (width + CALLOUT_X_MARGIN) : -CALLOUT_X_MARGIN;//(pAdjustValue[2] > 0) ? (width + CALLOUT_X_MARGIN) : -CALLOUT_X_MARGIN;
					lpData[2].y = CALLOUT_Y_MARGIN;
					lpData[2].a = CP_LINE | CP_ENDS;
					p = 3;
					}
				else if( (mType == ShapeType::Callout3) ||
					(mType == ShapeType::BorderCallout3) ||
					(mType == ShapeType::AccentCallout3) ||
					(mType == ShapeType::AccentBorderCallout3) )
					{
					lpData[0].x = (int)((double)pAdjustValue[0] * width / 21600 + 0.5);
					lpData[0].y = (int)((double)pAdjustValue[1] * height / 21600 + 0.5);
					lpData[0].a = CP_STAT | CP_LINE;
					lpData[1].x = (int)((double)pAdjustValue[2] * width / 21600 + 0.5);
					lpData[1].y = (LONG)( (double)lpData[0].y / 2 + 0.5);
					lpData[1].a = CP_LINE;					
					lpData[2].x = (int)((double)pAdjustValue[2] * width / 21600 + 0.5);
					lpData[2].y = CALLOUT_Y_MARGIN;
					lpData[2].a = CP_LINE;
					lpData[3].x = (pAdjustValue[2] < 0) ? -CALLOUT_X_MARGIN : (width + CALLOUT_X_MARGIN);
					lpData[3].y = CALLOUT_Y_MARGIN;
					lpData[3].a = CP_LINE | CP_ENDS;
					p = 4;
					}

				if( (mType >= ShapeType::Callout90) && (mType <= ShapeType::AccentBorderCallout90) )
					{
					if(pAdjustValue[1] != pAdjustValue[3]) // 두조절점이 수직인 경우
						pAdjustValue[2] = pAdjustValue[0];
						
					lpData[p].x = (int)((double)pAdjustValue[0] * width / 21600 + 0.5);	// -CALLOUT_X_MARGIN;
					lpData[p].y = (int)((double)pAdjustValue[1] * height / 21600 + 0.5);
					lpData[p++].a = CP_STAT | CP_LINE;
					lpData[p].x = (int)((double)pAdjustValue[2] * width / 21600 + 0.5);	// -CALLOUT_X_MARGIN;
					lpData[p].y = (int)((double)pAdjustValue[3] * height / 21600 + 0.5);
					lpData[p++].a = CP_LINE | CP_ENDS;
					}
				else if( ((mType >= ShapeType::AccentCallout1) && (mType <= ShapeType::AccentCallout3)) ||
					((mType >= ShapeType::AccentBorderCallout1) && (mType <= ShapeType::AccentBorderCallout3)) )
					{
					lpData[p].x = (lpData[p-1].x > 0) ? (width + CALLOUT_X_MARGIN) : -CALLOUT_X_MARGIN;
					lpData[p].y = 0;
					lpData[p++].a = CP_STAT | CP_LINE;
					lpData[p].x = lpData[p-1].x;
					lpData[p].y = height;
					lpData[p++].a = CP_LINE | CP_ENDS;
					}
				lpData[p-1].a |= CP_ENDG;
				*pnum = p;
				objtype = BasicGraphElement::PolygonElement;
				}
			break;
		case ShapeType::CloudCallout:				// = 106,
			{
			int		ox = 0, oy = 0;			

			int angle = GetAngleFromOrigin(pAdjustValue[0] - 10800, 10800 - pAdjustValue[1]);
			int radius = GetDistance(pAdjustValue[0] - 10800, 10800 - pAdjustValue[1]);

			if(radius > 10800)
				{
				POINT offset;
				int nr = child * (radius - 10800) / 3 + 10800;
				GetPointFromAngleDist(angle, nr, &offset);

				ox = (int)((double)pAdjustValue[0] * width / 21600 + 0.5);
				oy = (int)((double)pAdjustValue[1] * height / 21600 + 0.5);
				ox = (int)((double)(offset.x + 10800) * width / 21600 + 0.5);
				oy = (int)((double)(offset.y + 10800) * height / 21600 + 0.5);
				}

			if(child == 0)
				{
				lpData = new CPOINT[86];
				convertPoints(lpData, _CloudCallout, 86, width, height);
				*pnum = 86;
				objtype = BasicGraphElement::PolygonElement;
				}
			else if(child == 1)
				{
				rect->left = ox - (int)((double)820 * width / 10000 + 0.5);
				rect->top = oy - (int)((double)820 * height / 10000 + 0.5);
				rect->right = ox + (int)((double)820 * width / 10000 + 0.5);
				rect->bottom = oy + (int)((double)820 * height / 10000 + 0.5);
				objtype = BasicGraphElement::EllipseElement;
				}
			else if(child == 2)
				{
				rect->left = ox - (int)((double)550 * width / 10000 + 0.5);
				rect->top = oy - (int)((double)550 * height / 10000 + 0.5);
				rect->right = ox + (int)((double)550 * width / 10000 + 0.5);
				rect->bottom = oy + (int)((double)550 * height / 10000 + 0.5);
				objtype = BasicGraphElement::EllipseElement;
				}
			else if(child == 3)
				{
				rect->left = ox - (int)((double)280 * width / 10000 + 0.5);
				rect->top = oy - (int)((double)280 * height / 10000 + 0.5);
				rect->right = ox + (int)((double)280 * width / 10000 + 0.5);
				rect->bottom = oy + (int)((double)280 * height / 10000 + 0.5);
				objtype = BasicGraphElement::EllipseElement;
				}
			}
		default:
			break;
		}
	*lppPts = lpData;
	return objtype;
}

BOOL KShapeSmartCalloutObject::SetAdjustHandlePoint(int idx, LPPOINT pnts, BOOL bSet, LONG* pAdjustValue)
{
	BOOL	rtnX = FALSE, rtnY = FALSE;
	/*
	int		adjust;
	int		width, height;
	int		min, max;
	*/

	int nCalloutType = GetCalloutType(mType);

	// 설명선 처리부분
	if (nCalloutType != -1)
	{
		int nAdjust = 0;
		if (nCalloutType == 0) { // 사각, 둥근사각, 타원, 구름모양 설명선
			nAdjust = (int)((double)(pnts->x - Rect.left) * 21600 / abs(Rect.right - Rect.left) + 0.5);
			SetValidAdjust(pAdjustValue, 0, nAdjust, bSet, rtnX);
			nAdjust = (int)((double)(pnts->y - Rect.top) * 21600 / abs(Rect.bottom - Rect.top) + 0.5);
			SetValidAdjust(pAdjustValue, 1, nAdjust, bSet, rtnY);
		}
		else if (nCalloutType == 1) { // 조절점 2개, 수직/수평 가이드라인
			SetCallout90Adjust(pnts, pAdjustValue, idx, bSet, rtnX, rtnY);
		}
		else if (nCalloutType == 2 || nCalloutType == 3) { // 조절점 2개, 한번 꺾인 가이드라인 || 조절점 3개, 두번 꺾인 가이드라인
			SetCallout12Adjust(pnts, pAdjustValue, idx, bSet, rtnX, rtnY);
		}
		else if (nCalloutType == 4) { // 조절점 4개, 세번 꺾인 가이드라인
			SetCallout3Adjust(pnts, pAdjustValue, idx, bSet, rtnX, rtnY);
		}

		if (rtnX || rtnY) {
			rtnX = TRUE;
		}
		else {
			rtnX = FALSE;
		}
	}
	return rtnX;
}

void KShapeSmartCalloutObject::SetCallout90Adjust(LPPOINT pnts, LONG* pAdjustValue, int idx, BOOL bSet, BOOL &rtnX, BOOL &rtnY)
{
	int nAdjustX = 0, nAdjustY = 0;
	int height = abs(Rect.top - Rect.bottom);
	int width = abs(Rect.right - Rect.left);
	int nAdjust = 0;

	if(idx == 1) {
		if(pAdjustValue[1] == pAdjustValue[3]) { // 두 조절점이 수평의 위치에 있을 경우
			// 수평 이동 관련 변경
			nAdjust = (int)( (double)(pnts->x - Rect.left) * 21600 / abs(Rect.right - Rect.left) + 0.5 );
			if(pAdjustValue[0] != nAdjust) {
				if(bSet) {
					if(pAdjustValue[0] < pAdjustValue[2]) { // 1번 조절점의 x좌표가 2번 조절점 보다 적을 경우
						if(nAdjust > pAdjustValue[2]) {
							Rect.left = Rect.right - CALLOUT_X_MARGIN * 2;
							Rect.right = Rect.left + width;
							nAdjust = (int)( (double)(pnts->x - Rect.left) * 21600 / abs(Rect.right - Rect.left) + 0.5 );
							pAdjustValue[0] = nAdjust;
							nAdjust = (int)( (double)((double)(21600 - pAdjustValue[2]) * width / 21600) * 21600 / abs(Rect.right - Rect.left) + 0.5 );
							pAdjustValue[2] = nAdjust;
						} else {
							pAdjustValue[0] = nAdjust;
						}
					} else {
						if(nAdjust < pAdjustValue[2]) {
							Rect.left -= width - CALLOUT_X_MARGIN * 2;
							Rect.right = Rect.left + width;
							nAdjust = (int)( (double)(pnts->x - Rect.left) * 21600 / abs(Rect.right - Rect.left) + 0.5 );
							pAdjustValue[0] = nAdjust;
							nAdjust = (int)( (double)((double)(21600 - pAdjustValue[2]) * width / 21600) * 21600 / abs(Rect.right - Rect.left) + 0.5 );
							pAdjustValue[2] = nAdjust;
						} else {
							pAdjustValue[0] = nAdjust;
						}
					}
				}
				rtnX = TRUE;
			}
			// 수직 이동 관련 변경
			nAdjust = (int)( (double)(pnts->y - Rect.top) * 21600 / abs(Rect.bottom - Rect.top) + 0.5 );
			if(pAdjustValue[1] != nAdjust) {
				if(bSet) {
					if(pAdjustValue[1] > 0 && pAdjustValue[3] > 0) {
						if(nAdjust < pAdjustValue[3]) {
							Rect.top = pnts->y + CALLOUT_Y_MARGIN;
							Rect.bottom = Rect.top + height;
							nAdjust = (int)( (double)(pnts->y - Rect.top) * 21600 / abs(Rect.bottom - Rect.top) + 0.5 );
							pAdjustValue[1] = pAdjustValue[3] = nAdjust;
						} else {
							Rect.top = pnts->y - (height + CALLOUT_Y_MARGIN);
							Rect.bottom = pnts->y - CALLOUT_Y_MARGIN;
						}
					} else if(pAdjustValue[1] < 0 && pAdjustValue[3] < 0) {
						if(nAdjust > pAdjustValue[3]) {
							Rect.top = pnts->y - (height + CALLOUT_Y_MARGIN);
							Rect.bottom = pnts->y - CALLOUT_Y_MARGIN;
							nAdjust = (int)( (double)(pnts->y - Rect.top) * 21600 / abs(Rect.bottom - Rect.top) + 0.5 );
							pAdjustValue[1] = pAdjustValue[3] = nAdjust;
						} else {
							Rect.top = pnts->y + CALLOUT_Y_MARGIN;
							Rect.bottom = Rect.top + height;
						}
					}
				}
				rtnY = TRUE;
			}
		} else {
			// 수직 이동 관련 변경
			nAdjust = (int)( (double)(pnts->y - Rect.top) * 21600 / abs(Rect.bottom - Rect.top) + 0.5 );
			if(pAdjustValue[1] != nAdjust) {
				if(bSet) {
					if(pAdjustValue[1] > pAdjustValue[3]) { // 1번 조절점의 y좌표가 2번 조절점보다 클 경우
						if(nAdjust < pAdjustValue[3]) {
							Rect.top -= height - CALLOUT_Y_MARGIN * 2;
							Rect.bottom = Rect.top + height;
							nAdjust = (int)( (double)(pnts->y - Rect.top) * 21600 / abs(Rect.bottom - Rect.top) + 0.5 );
							pAdjustValue[1] = nAdjust;
							nAdjust = (int)( (double)((double)(21600 - pAdjustValue[3]) * height / 21600) * 21600 / abs(Rect.bottom - Rect.top) + 0.5 );
							pAdjustValue[3] = nAdjust;
						} else {
							pAdjustValue[1] = nAdjust;
						}
					} else {
						if(nAdjust > pAdjustValue[3]) {
							Rect.top -= height - CALLOUT_Y_MARGIN * 2;
							Rect.bottom = Rect.top + height;
							nAdjust = (int)( (double)(pnts->y - Rect.top) * 21600 / abs(Rect.bottom - Rect.top) + 0.5 );
							pAdjustValue[1] = nAdjust;
							nAdjust = (int)( (double)((double)(21600 - pAdjustValue[3]) * height / 21600) * 21600 / abs(Rect.bottom - Rect.top) + 0.5 );
							pAdjustValue[3] = nAdjust;
						} else {
							pAdjustValue[1] = nAdjust;
						}
					}
				}
				rtnY = TRUE;
			}
			// 수평 이동 관련 변경
			nAdjust = (int)( (double)(pnts->x - Rect.left) * 21600 / abs(Rect.right - Rect.left) + 0.5 );
			if(pAdjustValue[0] != nAdjust) {
				if(bSet) {
					if(pAdjustValue[0] < 0 && pAdjustValue[2] < 0) { // 두 조절점의 x좌표가 모두 사각형의 왼쪽에 있는 경우
						if(nAdjust > pAdjustValue[2]) {
							Rect.left = pnts->x - (width + CALLOUT_X_MARGIN);
							Rect.right = Rect.left + width;
							nAdjust = (int)( (double)(pnts->x - Rect.left) * 21600 / abs(Rect.right - Rect.left) + 0.5 );
							pAdjustValue[0] = pAdjustValue[2] = nAdjust;
						} else {
							Rect.left = pnts->x + CALLOUT_X_MARGIN;
							Rect.right = Rect.left + width;
						}
					} else {
						if(nAdjust < pAdjustValue[2]) {
							Rect.left = pnts->x + CALLOUT_X_MARGIN;
							Rect.right = Rect.left + width;
							nAdjust = (int)( (double)(pnts->x - Rect.left) * 21600 / abs(Rect.right - Rect.left) + 0.5 );
							pAdjustValue[0] = pAdjustValue[2] = nAdjust;
						} else {
							Rect.left = pnts->x - (width + CALLOUT_X_MARGIN);
							Rect.right = Rect.left + width;
						}
					}
				}
				rtnX = TRUE;
			}
		}
	}
}

void KShapeSmartCalloutObject::SetCallout12Adjust(LPPOINT pnts, LONG* pAdjustValue, int idx, BOOL bSet, BOOL &rtnX, BOOL &rtnY)
{
	if(idx == 1) {
		int nAdjust = 0;
		int height = abs(Rect.top - Rect.bottom);
		int width = abs(Rect.right - Rect.left);
		BOOL _rtnX = FALSE, _rtnY = FALSE;

		_rtnX = rtnX;
		_rtnY = rtnY;

		nAdjust = (int)( (double)(pnts->y - Rect.top) * 21600 / abs(Rect.bottom - Rect.top) + 0.5 );
		SetValidAdjust(pAdjustValue, 1, nAdjust, bSet, _rtnY);
		
		nAdjust = (int)( (double)(pnts->x - Rect.left) * 21600 / abs(Rect.right - Rect.left) + 0.5 );
		if(pAdjustValue[0] != nAdjust) {
			if(bSet) {
				if(pAdjustValue[0] < 0) {
					if(nAdjust > (double)-CALLOUT_X_MARGIN * 21600 / abs(Rect.right - Rect.left)) {
						Rect.left -= width + CALLOUT_X_MARGIN * 2;
						Rect.right -= width + CALLOUT_X_MARGIN * 2;
						nAdjust = (int)( (double)(pnts->x - Rect.left) * 21600 / abs(Rect.right - Rect.left) + 0.5 );
					}
				} else {
					if(nAdjust < (double)(abs(Rect.right - Rect.left) + CALLOUT_X_MARGIN) * 21600 / abs(Rect.right - Rect.left)) {
						Rect.left += width + CALLOUT_X_MARGIN * 2;
						Rect.right += width + CALLOUT_X_MARGIN * 2;
						nAdjust = (int)( (double)(pnts->x - Rect.left) * 21600 / abs(Rect.right - Rect.left) + 0.5 );
					}
				}

				pAdjustValue[0] = nAdjust;
			}
			_rtnX = TRUE;
		}
		rtnX = _rtnX;
		rtnY = _rtnY;
	}
}

void KShapeSmartCalloutObject::SetCallout3Adjust(LPPOINT pnts, LONG* pAdjustValue, int idx, BOOL bSet, BOOL &rtnX, BOOL &rtnY)
{
	if(idx == 1) {
		int nAdjust = 0;
		int height = abs(Rect.top - Rect.bottom);
		int width = abs(Rect.right - Rect.left);
		BOOL _rtnX = FALSE, _rtnY = FALSE;

		_rtnX = rtnX;
		_rtnY = rtnY;

		nAdjust = (int)( (double)(pnts->y - Rect.top) * 21600 / abs(Rect.bottom - Rect.top) + 0.5 );
		SetValidAdjust(pAdjustValue, 1, nAdjust, bSet, _rtnY);
		
		nAdjust = (int)( (double)(pnts->x - Rect.left) * 21600 / abs(Rect.right - Rect.left) + 0.5 );
		if(pAdjustValue[0] != nAdjust) {
			if(bSet) {
				if(pAdjustValue[2] < 0) {
					if(nAdjust < -7200) {
						Rect.left -= width + (int)((double)7200 * width * 2 / 21600 + 0.5);
						Rect.right -= width + (int)((double)7200 * width * 2 / 21600 + 0.5);
						nAdjust = (int)( (double)(pnts->x - Rect.left) * 21600 / abs(Rect.right - Rect.left) + 0.5 );
						pAdjustValue[2] = 21600 + 7200;
					}
				} else {
					if(nAdjust > 21600 + 7200) {
						Rect.left += width + (int)((double)7200 * width * 2 / 21600 + 0.5);
						Rect.right += width + (int)((double)7200 * width * 2 / 21600 + 0.5);
						nAdjust = (int)( (double)(pnts->x - Rect.left) * 21600 / abs(Rect.right - Rect.left) + 0.5 );
						pAdjustValue[2] = -7200;
					}
				}

				pAdjustValue[0] = nAdjust;
			}
			_rtnX = TRUE;
		}
		rtnX = _rtnX;
		rtnY = _rtnY;
	}
}
#ifdef _MSC_VER
#pragma warning( pop )
#endif // _MSC_VER
