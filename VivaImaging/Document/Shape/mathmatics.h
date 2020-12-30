#pragma once

#define MIN_V(a, b) min(a, b)
#define MAX_V(a, b) max(a, b)
#define ABS_V(a) abs(a)

#define VALUE_PIE			((double) 3.141592)
#define VALUE_PIE_PER_180	((double) VALUE_PIE / 180)
#define VALUE_180_PER_PIE	((double) 180 / VALUE_PIE)
#define PIE_PER_180			(VALUE_PIE / 180)
#define ANGLE_TO_RADIAN(x)	((double) x * VALUE_PIE_PER_180 / 10)
#define RADIAN_TO_ANGLE(x)	((double) x * VALUE_180_PER_PIE * 10)

#define	CIRCLE_MAGICS		1.81066

#define		APC_ARC					0
#define		APC_PIE					1
#define		APC_CHORD				2

#include "CurveTracer.h"

extern int  IsTrianglePoint(LPPOINT path);
extern int PointIsonLine(LPPOINT p1, LPPOINT p2, LPPOINT np, long err);
extern int MidPointSkipLine(LPPOINT drag_path, int start, int end);

extern int ArrangeDragPoints(LPPOINT drag_path, int pnts);
extern int ArrangeDragPoints(LP_CPOINT drag_path, int pnts);

int  GetPieChordPolyline(LPPOINT center, LPPOINT size, int deg,
  int fst_angle, int snd_angle, int APC, LPPOINT pnt_buff);
int  	GetPieChordPolygon(LPPOINT center, LPPOINT size, int deg,
  int fst_angle, int snd_angle, int APC, LP_CPOINT pn);
void 	GetEllipsePoint(long xr, long yr, int Angle, LPPOINT pnts);
BOOL 	GetLineCrossPoint(LPPOINT p1, LPPOINT p2, LPPOINT s1, LPPOINT s2, LPPOINT dp);
BOOL 	GetLineCrossPoint2(LPPOINT p1, double a1, LPPOINT s1, double a2, LPPOINT dp);
long 	GetDistance(LPPOINT sp);
long	GetDistance(int x, int y);
long	GetDistance(LPPOINT p1, LPPOINT p2);
long	GetDistance(LP_CPOINT p1, LP_CPOINT p2);
long	GetDistance(LPPOINT points, int pnum);
long 	GetLinePointDistance(LPPOINT p1, LPPOINT p2, LPPOINT p3);
long	GetLinePointDistanceOn(POINT& sp, POINT& ep, POINT& pt);

double GetLinePointDistance(CPOINT& p1, CPOINT& p2, CPOINT& p3);

int  	AddAngle(int cur, int added);
int  	GetPolyPointAngle(LPPOINT pnts, int endp);
BOOL 	IsLeftAngle(int sa, int da);
long	ConvertDouble(double dbl);
double	SinusoidalRatio(double r);
void 	GetPointFromAngleDist(int angle, long dist, LPPOINT po);
void 	GetPointFromAngleDistD(double angle, long dist, LPPOINT po);

long 	LengthOfLine(LPPOINT p1, LPPOINT p2);
double LengthOfLine(CPOINT& p1, CPOINT& p2);

double GetLinePointAngle(int x, int y);
int GetLinePointAngle(LPPOINT ps, LPPOINT pe);
int GetLinePointAngle(CPOINT& ps, CPOINT& pe);

int GetAngleDiff(int a1, int a2);

#if 0
void 	ExpandRectangleArea(LP_RECTANGLE sr, long size);
void 	AddRectangle(LP_RECTANGLE dst, LP_RECTANGLE s1, LP_RECTANGLE s2);
BOOL 	IsRectangleEmpty(LP_RECTANGLE rect);
void 	SetRectangleEmpty(LP_RECTANGLE rect);
void 	AppendRectangle(LP_RECTANGLE adv, LP_RECTANGLE src);
void 	AddRectangleBlock(CRect& sr, CRect& add);
void	AddLPRECTBlock(LPRECT sr, LPRECT add);
void	SetValidRectangle(LP_RECTANGLE rect);
int  	CompareRectangleBlock(LPRECT s1, LPRECT s2);
BOOL 	RectHasCommonArea(CRect& s1, CRect& s2);
BOOL 	AndRectangleBlock(CRect& s1, CRect& s2);
void	CopyRectangleToRect(CRect& rt, LP_RECTANGLE rect);
// void 	CopyRectangleToRect(LPRECT rt, LP_RECTANGLE rect);
void	CopyCRectToRectangle(LP_RECTANGLE rect, CRect& rt);
void 	CopyPOINTToPoint(LPPOINT sp, LPPOINT lp);
void 	MoveRectangle(LP_RECTANGLE rect, long xo, long yo);
void 	RectangleToBlock(LPPOINT p1, LPPOINT p2, LPPOINT sp, LPPOINT ep);
int  	IsRectangleCrossed(LP_RECTANGLE rect, LP_RECTANGLE rect2);
int		IsRectangleMeet(LP_RECTANGLE rect, LP_RECTANGLE rect2);
int		IsRectangleCrossed(CRect& rect, CRect& rect2);
int		IsRectangleMeet(CRect& rect, CRect& rect2);
void 	CoordToRectArea(LP_RECTANGLE rect, LP_RECTANGLE cod);
void 	CoordToLogicRect(LP_RECTANGLE rect);

int  	PointIsonLine(LPPOINT p1, LPPOINT p2, LPPOINT np, long err);
int  	PointIsInsideBoundRect(LP_RECTANGLE rt, LPPOINT np, long err);
int  	PointIsonBoundRect(LP_RECTANGLE rt, LPPOINT np, long err);
int  	PointIsonRect(LP_RECTANGLE rt, LPPOINT np, long err);
int  	PointIsonPoint(LPPOINT p1, LPPOINT p2, long err);
int  	PointIsonLinePoly(LP_LPOPOLYPOLYGON lpoly, LPPOINT sp, long err);
int  	PointIsInsideLinePoly(LP_LPOPOLYPOLYGON lpoly, LPPOINT sp, long err, WORD selMode);
long 	GetVertyLineLen(int ang1, int ang2, long len);
RECTANGLE GetCurvePolygonRect(int pn, LP_CPOINT npoly);
RECTANGLE GetCurvePolygonRectPure(int pn, LP_CPOINT npoly);
RECTANGLE GetPOINTPolygonRect(int pn, LPPOINT npoly);
RECT 	  GetPointPolygonRect(int pn, LPPOINT npoly);
#endif

extern int SplitBezierSegmentByLine(LP_CPOINT bezseg, LPPOINT cutline, int seg);
extern int SplitBezierByLine(LPPOINT bezp, LPPOINT cutt, LP_CPOINT splitbez, int* first_seg);

extern void HFlopCPoint(int pnum, LP_CPOINT pn, long origin);
extern void VFlopCPoint(int pnum, LP_CPOINT pn, long origin);