#pragma once

#ifndef CURVE_TREACE_HEADER_
#define CURVE_TREACE_HEADER_

#include <math.h>

typedef struct tagCPOINT
{
  int x;
  int y;
  WORD a;
  WORD p;
} CPOINT, *LP_CPOINT;

#define CPOINT_LINE         0
#define CPOINT_STAT         0x0001
#define CPOINT_CURV         0x0004
#define CPOINT_ENDS         0x0008
#define CPOINT_ENDG         0x0010
#define CPOINT_CLOSED       0x2000
#define CPOINT_TYPE         (CPOINT_CURV | CPOINT_LINE)

#define FIT_BY_LINE 1
#define FIT_BY_CURVE 2
#define FIT_BY_DUAL 3

typedef POINT* LPBezierCurve;

typedef struct tagCurveVectorInfo
{
  int angle;
  int av;
  int len;
  int avr;
  int sum;
} CurveVectorInfo;

class KCurveTracer
{
public:
  KCurveTracer();
  ~KCurveTracer();

  int LinePolygonToCurveCP(LPPOINT pnts, int npnts, LP_CPOINT curv);
  int LinePolygonToCurve(LPPOINT pnts, int npnts, LPPOINT bez);

  int FitCurvePoint(LP_CPOINT d, int npnts, short method, LP_CPOINT dst, int destSize);

private:
  void FitCubic(int first, int last, POINT tHat1, POINT tHat2, short error);
  int getMeanPressure(int first, int last);

  double getStrightAdoptError(int sp, int ep);
  BOOL getAverageAngle(int startFrom, int p, OUT int& prev_avr_angle);
  BOOL getAverageAngleAfter(int p, int end, OUT int& next_avr_angle);
  BOOL IsStrightLinePartition(int startPoint, int endPoint);

  BOOL isTraceBreakPoint(int s, int p);
  int getMeanAngleAround(int p);
  int getAngleVector();
  void dumpAngleVector();

  void AddLineSegment(CPOINT& sp);
  void AddBezierSegment(int num, LPPOINT Buff, int pr);
  void   GenerateBezier(int first, int last, double* uPrime, POINT tHat1, POINT that2);
  double* Reparameterize(int first, int last, double* u, LPBezierCurve bezCurve);
  double NR_RootFind(LPBezierCurve Q, CPOINT& P, double u);
  POINT Bezier(int degree, POINT* V, double t);
  double B0(double u);
  double B1(double u);
  double B2(double u);
  double B3(double u);
  void   ComputeLeftTangent(int end, LPPOINT ds);
  void   ComputeRightTangent(int end, LPPOINT ds);
  void   ComputeCenterTangent(int center, LPPOINT ds);
  double* ChordLengthParameterize(int first, int last);
  double ComputeMaxError(int first, int last, LPBezierCurve bezCurve, double* u, int* splitPoint);
  POINT V2AddII(POINT a, POINT b);
  POINT V2ScaleII(POINT a, double u);
  POINT V2SubII(CPOINT& a, CPOINT& b);
  POINT V2SubII(POINT& a, POINT& b);

  double V2Length(POINT* a);
  double V2SquaredLength(POINT* a);
  void   V2Negative(POINT* a);
  void   V2Add(POINT* a, POINT* b, POINT* c);
  double V2Dot(POINT a, POINT b);
  void   V2Scale(POINT* a, double newlen);
  void   V2Normalize(LPPOINT a);

  long LengthOfPolyline(LP_CPOINT pnts, int num);

  POINT CurveSegment[10];
  LP_CPOINT SourcePoints;
  int SourceNumber;
  LP_CPOINT DestPoints;
  int recursiveCount = 0;
  int curvenum = 0;
  int ResultBufferSize;

  CurveVectorInfo* pVector;
};


#endif