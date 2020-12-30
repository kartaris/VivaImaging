/****************************************************************************

		Project		: HNC Drawing
		Module 		: CurveTrace.cpp
		Description : Convert freehand-path to Polygon-Object
						by Curve Fitting Algorithm
		Date
			Coding	: 1993, 5, 1
			Update	:
		Written by	: Lees
****************************************************************************/
#include "stdafx.h"
#include <math.h>
#include "CurveTracer.h"
#include "mathmatics.h"

#ifdef _DEBUG
#undef THIS_FILE
static char	THIS_FILE[]=__FILE__;
#define	new	DEBUG_NEW
#endif 
 
#define MAXPOINTS		200
#define BASE_LONG_VAR   32768

KCurveTracer::KCurveTracer()
{
	pVector = NULL;
}

KCurveTracer::~KCurveTracer()
{
	if (pVector != NULL)
		delete[] pVector;
}

#define	LINE_THRESHOLD		40
#define	CUTOFF_THRESHOLD 	40
#define	TRACE_ERROR 25
#define	STRIGHTLINE_THRESHOLD 3

#ifdef	NEW_UNCOMPLETED

#define	MIN_LINED_LENGTH	16
#define	UNDEFINED_VALUE		0x7FFF
#define	MIN_ANGLE_CHANGE	10

LP_OBJECT GetPolygonCPoint(LPPOINT d, int npnts, short error, short Threshold)
{
	int		prv_angle, nxt_angle, tot_dista;
	int		processed_point;
	float	avr_angle_chage_ratio, cur_angle_change_ratio;

	// find lined segment
	avr_angle = prv_angle = GetLineAngle(&d[0], &d[1]);
	prv_dista = tot_dista = GetLineDistance(&d[0], &d[1]);
	avr_angle_chage_ratio = UNDEFINED_VALUE;
	processed_point = 0;

	for (i=1 ; i<npnts-1 ; i++)
		{
		nxt_angle = GetLineAngle(&d[i], &d[i+1]);
		nxt_dista = GetLineDistance(&d[i], &d[i+1]);

		if(nxt_dista >= MIN_LINED_LENGTH)
			{
			// current is lined

			// from processed_point to currnt 
			if(i - processed_point > 3)
				{
				// save to curve segment
				}
			else
				{
				// save to line segment
				}

			// save current line segment

			// reset variables
			processed_point = i + 1;
			tot_dista = nxt_dista;
			}
		else
			{
			angle_change = prv_angle - nxt_angle;
			cur_angle_change_ratio = (float) angle_change / (nxt_dista + prv_dista);

			if(avr_angle_chage_ratio == UNDEFINED_VALUE)
				avr_angle_chage_ratio = cur_angle_change_ratio;
			else
				{
				if( ABS_V( (long) ((avr_angle_chage_ratio -
					cur_angle_change_ratio) * 100)) < MIN_ANGLE_CHANGE)
					{
					// continuos segment

					}
				else
					{
					// discontinuous point

					}

				// calculate average change ratio
				avr_angle_chage_ratio = cur_angle_change_ratio;
				}
			}
		prv_angle = nxt_angle;
		prv_dista = nxt_dista;
		tot_dista += nxt_dista;
		}
	return(NULL);
}
#endif 
 
int KCurveTracer::FitCurvePoint(LP_CPOINT d, int npnts, short method, LP_CPOINT dest, int destSize)
{
	POINT			tHat1, tHat2;
	int				processPoint, i;

	SourcePoints = d;
	SourceNumber = npnts;
	recursiveCount = 0;
	processPoint = 0;
	ResultBufferSize = destSize;
	DestPoints = dest;
	if(DestPoints == NULL)
		return(0);

	// add line segment start point
	curvenum = 0;
	AddLineSegment(SourcePoints[0]);

	if(method == FIT_BY_DUAL)
	{
		getAngleVector();
		dumpAngleVector();

		for (i=1 ; i<npnts ; i++)
		{
			// current segment is processed LINE-SEG
			// get angleSpeed of curve
			if (isTraceBreakPoint(processPoint, i))
			{
				// add segment (processPoint ~ i)
				double err = 0;
				if ((i - processPoint) >= 2)
					err = getStrightAdoptError(processPoint, i);
				if (err > STRIGHTLINE_THRESHOLD) // TRACE_ERROR)
				{
					ComputeLeftTangent(processPoint, &tHat1);
					ComputeRightTangent(i, &tHat2);
					FitCubic(processPoint, i, tHat1, tHat2, TRACE_ERROR);
				}
				else
				{
					AddLineSegment(SourcePoints[i]);
				}
				processPoint = i;
			}

			/*
			if (LengthOfLine(SourcePoints[i], SourcePoints[i+1]) >= LINE_THRESHOLD)
			{
				// process previouse curve segment
				if( LengthOfPolyline(&(SourcePoints[processPoint]), i - processPoint) > CUTOFF_THRESHOLD)
				{
					ComputeLeftTangent(processPoint, &tHat1);
					ComputeRightTangent(i, &tHat2);
					FitCubic(processPoint, i, tHat1, tHat2, TRACE_ERROR);
				}

				DestPoints[curvenum].a = CPOINT_LINE;
				DestPoints[curvenum].x = SourcePoints[i + 1].x;
				DestPoints[curvenum].y = SourcePoints[i + 1].y;
				DestPoints[curvenum].p = SourcePoints[i + 1].p;
				curvenum++;
				processPoint = i + 1;
			}
			*/

			if(curvenum + 8 >= ResultBufferSize)
				break;
		}
	}

	if(processPoint+1 < npnts)
	{
		if(npnts - processPoint >= 3)
		{
			ComputeLeftTangent(processPoint, &tHat1);
			ComputeRightTangent(npnts-1, &tHat2);
			FitCubic(processPoint, npnts-1, tHat1, tHat2, TRACE_ERROR);
		}
		else
		{
			for (i=processPoint+1 ; i<npnts ; i++)
				AddLineSegment(SourcePoints[i]);
		}
	}
	return(curvenum);
}

double KCurveTracer::getStrightAdoptError(int sp, int ep)
{
	double er = 0.0;

	for (int i = sp + 1; i < ep; i++)
	{
		er += GetLinePointDistance(SourcePoints[sp], SourcePoints[ep], SourcePoints[i]);
	}
	er = er / (ep - sp - 1);
	return er;
}

BOOL KCurveTracer::getAverageAngle(int startFrom, int p, OUT int& prev_avr_angle)
{
	double sum_va = 0;
	double sum_len = 0;

	for (int i = startFrom; i < p; i++)
	{
		sum_va += pVector[i].angle * pVector[i].len;
		sum_len += pVector[i].len;
	}

	if (sum_len < 5) // 길이가 얼마 안되면 무시.
		return FALSE;

	prev_avr_angle = (int)(sum_va / sum_len);
	return TRUE;
}

BOOL KCurveTracer::getAverageAngleAfter(int p, int end, OUT int& next_avr_angle)
{
	double sum_va = 0;
	double sum_len = 0;

	for (int i = p; i < end; i++)
	{
		if ((i > p) &&
			!IsStrightLinePartition(p, i + 1))
			break;

		sum_va += pVector[i].angle * pVector[i].len;
		sum_len += pVector[i].len;

		if (sum_len >= 20.0)
			break;
	}

	if (sum_len < 5) // 길이가 얼마 안되면 무시.
		return FALSE;

	next_avr_angle = (int)(sum_va / sum_len);
	return TRUE;
}

// check if (startPoint ~ endPoint) is almost stright line
BOOL KCurveTracer::IsStrightLinePartition(int startPoint, int endPoint)
{
	if (startPoint + 1 < endPoint)
	{
		for (int i = startPoint + 1; i < endPoint - 1; i++)
		{
			double d = GetLinePointDistance(SourcePoints[startPoint], SourcePoints[endPoint], SourcePoints[i]);
			if (d >= 2.0)
				return FALSE;
		}
	}
	else
	{
		TCHAR msg[128];
		StringCchPrintf(msg, 128, _T("neighboring point %d,%d\n"), startPoint, endPoint);
		OutputDebugString(msg);
	}
	return TRUE;
}

BOOL KCurveTracer::isTraceBreakPoint(int startFrom, int p)
{
	if (p >= SourceNumber - 1)
		return TRUE;
	// p 점을 기준으로
	// 1. 다음 직선 길이가 LINE_THRESHOLD 보다 긴 경우
	// 2. angle이 90이상 바뀌는 경우
	if (pVector[p].len >= LINE_THRESHOLD)
		return FALSE;

	int prev_avr_angle;
	int next_avr_angle;

	// check pressure
	if (startFrom < p)
	{
		int w_sum = 0;
		for (int i = startFrom; i < p; i++)
			w_sum += SourcePoints[i].p;
		w_sum = w_sum / (p - startFrom);
		if ((w_sum / 100) != (SourcePoints[p].p / 100))
			return TRUE;
	}

	if (getAverageAngle(startFrom, p, prev_avr_angle) &&
		getAverageAngleAfter(p, SourceNumber, next_avr_angle))
	{
		int diff = GetAngleDiff(prev_avr_angle, next_avr_angle);
		if (abs(diff) >= 900)
			return TRUE;

		TCHAR msg[256];
		StringCchPrintf(msg, 256, _T("AngleDiff on p(%d), diff=%d\n"), p, diff);
		OutputDebugString(msg);
	}
	return FALSE;

#ifdef _OLD_METHOD
	// angle vector of (processPoint ~ i)
	int a = getAngleVector();

	TCHAR msg[256];
	StringCchPrintf(msg, 256, _T("p=%d, angleVector=%d\n"), p, a);
	OutputDebugString(msg);

	return FALSE;

	// find end
	int len = 0;
	int e = curvenum;

	for (int i = p; i < curvenum; i++)
	{
		len += (int)LengthOfLine(SourcePoints[i], SourcePoints[i + 1]);
		if (len > 100)
		{
			e = i + 1;
			break;
		}
	}

	int b = getAngleVector();

	return (abs(a - b) > 90);
#endif
}

int KCurveTracer::getMeanAngleAround(int p)
{
	int sum_be = 0;
	int sum_af = 0;
	int sum_bl = 0;
	int sum_al = 0;

	if (p > 0)
	{
		for (int i = p - 1; i >= 0; i--)
		{
			sum_be += pVector[i].angle * pVector[i].len;
			sum_bl += pVector[i].len;
			if (sum_bl > 20)
				break;
		}
	}

	if (p < SourceNumber - 1)
	{
		for (int i = p + 1; i < SourceNumber; i++)
		{
			sum_af += pVector[i].angle * pVector[i].len;
			sum_al += pVector[i].len;
			if (sum_al > 20)
				break;
		}
	}

	// 앞뒤 각도 차이가 1800이상이면 앞만 사용.
	if ((sum_bl > 20) && (sum_al > 20))
	{
		int diff = abs(sum_be / sum_bl - sum_af / sum_al);
		if (diff >= 1800)
			return (sum_be / sum_bl);
	}

	return ((sum_be + sum_af) / (sum_bl + sum_al));
}

int KCurveTracer::getAngleVector()
{
	int s = 0;

	if (pVector != NULL)
		delete[] pVector;
	pVector = new CurveVectorInfo[SourceNumber];

	for (int i = 0; i < SourceNumber - 1; i++)
	{
		double a = GetLinePointAngle(SourcePoints[i + 1].x - SourcePoints[i].x, SourcePoints[i + 1].y - SourcePoints[i].y);
		double d = LengthOfLine(SourcePoints[i], SourcePoints[i + 1]);
		pVector[i].angle = (int)a;
		pVector[i].len = (int)d;
	}
	pVector[SourceNumber - 1].angle = (int)pVector[SourceNumber - 2].angle;
	pVector[SourceNumber - 1].len = (int)0;

	if (SourceNumber >= 3)
	{
		for (int i = 0; i < SourceNumber - 1; i++)
		{
			if (pVector[i].angle < 900)
			{
				int mean_a = getMeanAngleAround(i);
				if (mean_a > 2700)
					pVector[i].angle += 3600;
			}
		}
	}

	// calculate angle vector
	pVector[0].av = 0;
	for (int i = 1; i < SourceNumber - 1; i++)
	{
		int angle_diff = GetAngleDiff(pVector[i].angle, pVector[i-1].angle);
		double dif = angle_diff * pVector[i].len / (pVector[i].len + pVector[i-1].len);
		pVector[i].av = (int)dif;
	}
	return 1;
}

void KCurveTracer::dumpAngleVector()
{
	TCHAR msg[256];
	for (int i = 0; i < SourceNumber; i++)
	{
		StringCchPrintf(msg, 256, _T("p=%d(%d,%d), angle=%d, av=%d, len=%d, avr=%d\n"), i, 
			SourcePoints[i].x, SourcePoints[i].y, pVector[i].angle, pVector[i].av, pVector[i].len, pVector[i].avr);
		OutputDebugString(msg);
	}
}

void KCurveTracer::AddLineSegment(CPOINT& sp)
{
	if (curvenum < ResultBufferSize)
	{
		DestPoints[curvenum].a = CPOINT_LINE;
		DestPoints[curvenum].x = sp.x;
		DestPoints[curvenum].y = sp.y;
		DestPoints[curvenum].p = sp.p;
		curvenum++;

		TCHAR msg[128];
		StringCchPrintf(msg, 128, _T("AddLineSegment (%d,%d), w=%d\n"), sp.x, sp.y, sp.p);
		OutputDebugString(msg);
	}
}

void KCurveTracer::AddBezierSegment(int num, LPPOINT Buff, int pr)
{
	if(curvenum + 4 < ResultBufferSize)
	{
		DestPoints[curvenum].x = Buff[1].x;
		DestPoints[curvenum].y = Buff[1].y;
		DestPoints[curvenum].p = pr;
		DestPoints[curvenum].a = CPOINT_CURV;
		curvenum++;

		DestPoints[curvenum].x = Buff[2].x;
		DestPoints[curvenum].y = Buff[2].y;
		DestPoints[curvenum].p = pr;
		DestPoints[curvenum].a = CPOINT_CURV;
		curvenum++;

		DestPoints[curvenum].x = Buff[3].x;
		DestPoints[curvenum].y = Buff[3].y;
		DestPoints[curvenum].p = pr;
		DestPoints[curvenum].a = CPOINT_LINE;
		curvenum++;

		TCHAR msg[128];
		StringCchPrintf(msg, 128, _T("AddBezierSegment (%d,%d) (%d,%d) (%d,%d), w=%d\n"), Buff[1].x, Buff[1].y, Buff[2].x, Buff[2].y, Buff[3].x, Buff[3].y, pr);
		OutputDebugString(msg);
	}
}

void KCurveTracer::FitCubic(int first, int last, POINT tHat1, POINT tHat2, short error)
{
	double  *u;
	double  *uPrime;
	int     splitPoint;
	int     npnts;
	int     maxIterations = 4;
	POINT  tHatCenter;
	int     i;
	long    maxError;
	long    iterationError;
#ifdef	ISBETA    
	char	fitmsg[60];
#endif 
 
	iterationError = (long) error * error;
	npnts = last - first + 1;

	int meanp = getMeanPressure(first, last);
	// by walker, 2003-11-29 포트가 3또는 그 이하이면 계산 안되니 근사해서 끝낸다.
	if(npnts <= 3)
		{
		double dist;

		dist = LengthOfLine(SourcePoints[last], SourcePoints[first]) / 2;

		CurveSegment[0] = { SourcePoints[first].x, SourcePoints[first].y };
		CurveSegment[3] = { SourcePoints[last].x, SourcePoints[last].y };
		V2Scale(&tHat1, dist);
		V2Scale(&tHat2, dist);
		V2Add(&CurveSegment[0], &tHat1, &CurveSegment[1]);
		V2Add(&CurveSegment[3], &tHat2, &CurveSegment[2]);

		// DrawLPBezierCurve(hDC, 3, CurveSegment, RGB(0,0,0));
		AddBezierSegment(3, CurveSegment, meanp);
		return;
		}

	u = ChordLengthParameterize(first, last);
	GenerateBezier(first, last, u, tHat1, tHat2);
	maxError = (long) ComputeMaxError(first, last, CurveSegment, u, &splitPoint);
#ifdef	ISBETA    
    wsprintf(fitmsg, "Curve Fitting Err=%d\n", (int) maxError);
    qDebug(fitmsg);
#endif 
 
	recursiveCount++;
	if(recursiveCount > 50)
	{
		AddBezierSegment(3, (LPPOINT) CurveSegment, meanp);
		delete[] u;
		return;
	}
    
	if(maxError < (long) error)
	{
		// DrawLPBezierCurve(hDC, 3, CurveSegment, RGB(0, 0, 0));
		AddBezierSegment(3, (LPPOINT) CurveSegment, meanp);
		delete[] u;
		return;
	}
	if(maxError == iterationError)
	{
		for (i=0 ; i<maxIterations ; i++)
		{
			uPrime = Reparameterize(first, last, u, CurveSegment);
			GenerateBezier(first, last, uPrime, tHat1, tHat2);
			maxError = (long) ComputeMaxError(first, last, CurveSegment, uPrime, &splitPoint);
			delete[] u;
			if(maxError < (long) error)
			{
				// DrawLPBezierCurve(hDC, 3, CurveSegment, RGB(0,0,0));
				AddBezierSegment(3, (LPPOINT) CurveSegment, meanp);
				return;
			}
			u = uPrime;
		}
	}
	delete[] u;

	// fitting failure. split it two segment
	ComputeCenterTangent(splitPoint, &tHatCenter);
	FitCubic(first, splitPoint, tHat1, tHatCenter, error);
	V2Negative((LPPOINT) &tHatCenter);
	FitCubic(splitPoint, last, tHatCenter, tHat2, error);
}

int KCurveTracer::getMeanPressure(int first, int last)
{
	int s = 0;
	for (int i = first; i <= last; i++)
	{
		s += SourcePoints[i].p;
	}
	return s / (last - first + 1);
}

int KCurveTracer::LinePolygonToCurveCP(LPPOINT pnts, int npnts, LP_CPOINT curv)
{
	POINT		tmp[4];
	int			add, i;
	
	add = LinePolygonToCurve(pnts, npnts, tmp);
				
	for(i=0 ; i<add ; i++)
		{
		curv[i].x = tmp[i].x;
		curv[i].y = tmp[i].y;
		}
	if(add == 2)
		{
		curv[0].a = CPOINT_LINE;
		curv[1].a = CPOINT_LINE;
		return(2);
		}
	else
		{
		curv[0].a = CPOINT_LINE;
		curv[1].a = CPOINT_CURV;
		curv[2].a = CPOINT_CURV;
		curv[3].a = CPOINT_LINE;
		return(4);
		}
}

int KCurveTracer::LinePolygonToCurve(LPPOINT pnts, int npnts, LPPOINT bez)
{
	double  *u;
	int     	splitPoint;
	long    	maxError;
	POINT		tHat1, tHat2;

	bez[0].x = pnts[0].x;
	bez[0].y = pnts[0].y;
	if(npnts <= 2)
		{
		bez[1].x = pnts[npnts-1].x;
		bez[1].y = pnts[npnts-1].y;
		return(2);
		}
	if(npnts == 3)
		{
		bez[1].x = bez[2].x = pnts[1].x;
		bez[1].y = bez[2].y = pnts[1].y;

		bez[3].x = pnts[2].x;
		bez[3].y = pnts[2].y;
		return(4);
		}
	if(npnts == 3)
		{
		bez[1].x = pnts[1].x;
		bez[1].y = pnts[1].y;
		bez[2].x = pnts[2].x;
		bez[2].y = pnts[2].y;
		bez[3].x = pnts[3].x;
		bez[3].y = pnts[3].y;
		return(4);
		}
		
	// SourcePoints = pnts;
	SourcePoints = new CPOINT[npnts];
	for (int i = 0; i < npnts; i++)
	{
		SourcePoints[i].x = pnts[i].x;
		SourcePoints[i].y = pnts[i].y;
	}

	ComputeLeftTangent(0, &tHat1);
	ComputeRightTangent(npnts-1, &tHat2);

	u = ChordLengthParameterize(0, npnts-1);
	GenerateBezier(0, npnts-1, u, tHat1, tHat2);
	maxError = (long) ComputeMaxError(0, npnts-1, CurveSegment, u, &splitPoint);

	bez[1].x = CurveSegment[1].x;
	bez[1].y = CurveSegment[1].y;
	bez[2].x = CurveSegment[2].x;
	bez[2].y = CurveSegment[2].y;
	bez[3].x = CurveSegment[3].x;
	bez[3].y = CurveSegment[3].y;
	delete[] u;

	delete[] SourcePoints;
	return(4);
}

void KCurveTracer::GenerateBezier(int first, int last, double *uPrime, POINT tHat1, POINT tHat2)
{
	int     i;
	int     npnts;
	double  C[2][2];
	double  X[2];
	double  det_C0_C1, det_C0_X, det_X_C1;
	double  alpha_1, alpha_r;
	POINT  tmp, v1, v2, mf, ml;
	POINT ms;
	// POINT  A[MAXPOINTS][2];
	LPPOINT  pA;

	npnts = last - first + 1;
	pA = new POINT[(npnts + 2) * 2];
	for (i=0 ; i<npnts ; i++)
		{
		v1 = tHat1;
		v2 = tHat2;
		V2Scale(&v1, B1(uPrime[i]) * BASE_LONG_VAR);
		V2Scale(&v2, B2(uPrime[i]) * BASE_LONG_VAR);
		pA[i*2] = v1;
		pA[i*2+1] = v2;
		// qDebug("bzinit:%d:(%d,%d) (%d,%d)", i, v1.x, v1.y, v2.x, v2.y);
		}
	// create the C and X matrix
	C[0][0] = 0.0;
	C[0][1] = 0.0;
	C[1][0] = 0.0;
	C[1][1] = 0.0;
	X[0] = X[1] = 0.0;

	mf.x = SourcePoints[first].x;
	mf.y = SourcePoints[first].y;
	ml.x = SourcePoints[last].x;
	ml.y = SourcePoints[last].y;

	for (i=0 ; i<npnts ; i++)
		{
		C[0][0] += V2Dot(pA[i*2], pA[i*2]);
		C[0][1] += V2Dot(pA[i*2], pA[i*2+1]);
		C[1][0] = C[0][1];
		C[1][1] += V2Dot(pA[i*2+1], pA[i*2+1]);

		ms.x = SourcePoints[first+i].x;
		ms.y = SourcePoints[first+i].y;

		v1 = V2AddII( V2ScaleII(ml, B2(uPrime[i])),
    		V2ScaleII(ml, B3(uPrime[i])) );
		v2 = V2AddII( V2ScaleII(mf, B1(uPrime[i])), v1);
		POINT pt = V2AddII(V2ScaleII(mf, B0(uPrime[i])), v2);
		tmp = V2SubII(ms, pt);

		X[0] += V2Dot(pA[i*2], tmp);
		X[1] += V2Dot(pA[i*2+1], tmp);
		}
	// compute  determinent of C and X
	det_C0_C1 = C[0][0] * C[1][1] - C[0][1] * C[1][0];
	det_C0_X = C[0][0] * X[1] - C[0][1] * X[0];
	det_X_C1 = C[1][1] * X[0] - C[0][1] * X[1];

	if(det_C0_C1 == 0.0)
		det_C0_C1 = (C[0][0] * C[1][1]) * (10e-12) / BASE_LONG_VAR;

	if(det_C0_C1 == 0.0)
		{
		alpha_1 = -1.0;
		alpha_r = -1.0;
		// qDebug("GenerateBezier Divider is Zero\n");
		}
	else
		{
		alpha_1 = det_X_C1 / det_C0_C1;
		alpha_r = det_C0_X / det_C0_C1;
		}

	if( (alpha_1 < 0.0) || (alpha_r < 0.0) || (alpha_1 > 1.0) || (alpha_r > 1.0))
		{
		double dist;

		dist = LengthOfLine(SourcePoints[last], SourcePoints[first]) / 3;
		CurveSegment[0] = { SourcePoints[first].x, SourcePoints[first].y };
		CurveSegment[3] = { SourcePoints[last].x, SourcePoints[last].y };
		V2Scale(&tHat1, (double) dist);
		V2Scale(&tHat2, (double) dist);
		V2Add(&CurveSegment[0], &tHat1, &CurveSegment[1]);
		V2Add(&CurveSegment[3], &tHat2, &CurveSegment[2]);
		}
	else
		{
		/**
		if( (alpha_1 > 1.0) || (alpha_r > 1.0) )
			{
			qDebug("alpha_1 = %f, alpha_r=%f", alpha_1, alpha_r);
			qDebug("det_C0_C1 = %f", det_C0_C1);
			qDebug("det_C0_X = %f", det_C0_X);
			qDebug("det_X_C1 = %f", det_X_C1);
			qDebug("C[0][0]=%f, C[1][1]=%f, C[0][1]=%f, C[1][0]=%f", C[0][0], C[1][1], C[0][1], C[1][0]);
			qDebug("X[0]=%f, X[1]=%f", X[0], X[1]);
			}
		****/
		CurveSegment[0] = { SourcePoints[first].x, SourcePoints[first].y };
		CurveSegment[3] = { SourcePoints[last].x, SourcePoints[last].y };
		V2Scale(&tHat1, alpha_1 * BASE_LONG_VAR);
		V2Scale(&tHat2, alpha_r * BASE_LONG_VAR);
		V2Add(&CurveSegment[0], &tHat1, &CurveSegment[1]);
		V2Add(&CurveSegment[3], &tHat2, &CurveSegment[2]);
		}
	delete[] pA;
	return;
}

#ifdef  COMP_DOUBLE

void GenerateBezier2(LPPOINT d, int first, int last, double *uPrime, POINT lHat1, POINT lHat2);
POINT CurveSegment2[10];

void 	V2DScale(DPOINT *a, double newlen);
double 	V2DDot(DPOINT *a, DPOINT *b);
void 	V2DAdd(DPOINT *a, DPOINT *b, DPOINT *c);
	DPOINT V2DAddII(DPOINT a, DPOINT b);
	DPOINT V2DScaleII(DPOINT a, double u);
	DPOINT V2DSubII(DPOINT a, DPOINT b);

void GenerateBezier2(LPPOINT d, int first, int last, double *uPrime, POINT lHat1, POINT lHat2)
{
	int     i;
	 DPOINT A[MAXPOINTS][2];
	int     npnts;
	double  C[2][2];
	double  X[2];
	double  det_C0_C1, det_C0_X, det_X_C1;
	double  alpha_1, alpha_r;
	DPOINT  tmp, v1, v2, m1;
	DPOINT  bezCurve[5];
	DPOINT  tHat1, tHat2;
	DPOINT  pf, ps, pl;
	DPOINT  tb1, tb2, tb3, tb4; 

	tHat1.x = (double) lHat1.x / BASE_LONG_VAR;
	tHat1.y = (double) lHat1.y / BASE_LONG_VAR;
	tHat2.x = (double) lHat2.x / BASE_LONG_VAR;
	tHat2.y = (double) lHat2.y / BASE_LONG_VAR;

	npnts = last - first + 1;

	for (i=0 ; i<npnts ; i++)
		{
		A[i][0] = tHat1;
		A[i][1] = tHat2;
		V2DScale(&A[i][0], B1(uPrime[i]));
		V2DScale(&A[i][1], B2(uPrime[i]));
		}

	// create the C and X matrix
	C[0][0] = 0.0;
	C[0][1] = 0.0;
	C[1][0] = 0.0;
	C[1][1] = 0.0;
	X[0] = X[1] = 0.0;

	pf.x = (double) d[first].x;
	pf.y = (double) d[first].y;
	pl.x = (double) d[last].x;
	pl.y = (double) d[last].y;

	for (i=0 ; i<npnts ; i++)
		{
		C[0][0] += V2DDot(&A[i][0], &A[i][0]);
		C[0][1] += V2DDot(&A[i][0], &A[i][1]);
		C[1][0] = C[0][1];
		C[1][1] += V2DDot(&A[i][1], &A[i][1]);

		ps.x = (double) d[first+i].x;
		ps.y = (double) d[first+i].y;

		tb1 = V2DAddII( V2DScaleII(pl, B2(uPrime[i])),
                    		V2DScaleII(pl, B3(uPrime[i])) );
		tb2 = V2DAddII(V2DScaleII(pf, B1(uPrime[i])), tb1);
		tmp = V2DSubII(ps, V2DAddII( V2DScaleII(pf, B0(uPrime[i])), tb2));

		X[0] += V2DDot(&A[i][0], &tmp);
		X[1] += V2DDot(&A[i][1], &tmp);
		}
	// compute  determinent of C and X
	det_C0_C1 = C[0][0] * C[1][1] - C[0][1] * C[1][0];
	det_C0_X = C[0][0] * X[1] - C[0][1] * X[0];
	det_X_C1 = C[1][1] * X[0] - C[0][1] * X[1];

	if(det_C0_C1 == 0.0)
		det_C0_C1 = (C[0][0] * C[1][1]) * (10e-12);

	alpha_1 = det_X_C1 / det_C0_C1;
	alpha_r = det_C0_X / det_C0_C1;
	if( (alpha_1 < 0.0) || (alpha_r < 0.0) )
		{
		long dist;

		dist = V2DistanceBetween2Points(d[last], d[first]) / 3;
		bezCurve[0].x = (double) d[first].x;
		bezCurve[0].y = (double) d[first].y;
		bezCurve[3].x = (double) d[last].x;
		bezCurve[3].y = (double) d[last].y;

		V2DScale(&tHat1, (double) dist);
		V2DScale(&tHat2, (double) dist);
		V2DAdd(&bezCurve[0], &tHat1, &bezCurve[1]);
		V2DAdd(&bezCurve[3], &tHat2, &bezCurve[2]);

		// return(bezCurve);
		}
	else
		{
		bezCurve[0].x = (double) d[first].x;
		bezCurve[0].y = (double) d[first].y;
		bezCurve[3].x = (double) d[last].x;
		bezCurve[3].y = (double) d[last].y;

		V2DScale(&tHat1, alpha_1);
		V2DScale(&tHat2, alpha_r);
		V2DAdd(&bezCurve[0], &tHat1, &bezCurve[1]);
		V2DAdd(&bezCurve[3], &tHat2, &bezCurve[2]);
		}
	for (i=0 ; i<4 ; i++)
		{
		CurveSegment2[i].x = (long) bezCurve[i].x;
		CurveSegment2[i].y = (long) bezCurve[i].y;
		}
	return;
}

void V2DScale(DPOINT *a, double newlen)
{
	double len;

	len = (a->x * a->x) + (a->y * a->y);
	if(len > 0.0)
		len = sqrt(len);
	else
		len = 0.0;
	if(len != 0.0)
		{
		a->x = (a->x * newlen / len);
		a->y = (a->y * newlen / len);
		}
}

double V2DDot(DPOINT *a, DPOINT *b)
{
	return( (a->x * b->x) + (a->y * b->y) );
}

void V2DAdd(DPOINT *a, DPOINT *b, DPOINT *c)
{
	c->x = a->x + b->x;
	c->y = a->y + b->y;
}

DPOINT V2DAddII(DPOINT a, DPOINT b)
{
	DPOINT c;

	c.x = a.x + b.x;
	c.y = a.y + b.y;
	return(c);
}

DPOINT V2DScaleII(DPOINT a, double u)
{
	DPOINT c;

	c.x = a.x * u;
	c.y = a.y * u;
	return(c);
}

DPOINT V2DSubII(DPOINT a, DPOINT b)
{
	DPOINT c;

	c.x = a.x - b.x;
	c.y = a.y - b.y;
	return(c);
}

#endif 
 

double* KCurveTracer::Reparameterize(int first, int last, double *u, LPBezierCurve bezCurve)
{
	int		npnts, i;
	double	*uPrime;

	npnts = last - first + 1;
	uPrime = new double[npnts];

	for (i=first ; i<=last ; i++)
		uPrime[i-first] = NR_RootFind(bezCurve, SourcePoints[i], u[i-first]);
	return(uPrime);
}

double KCurveTracer::NR_RootFind(LPBezierCurve Q, CPOINT& P, double u)
{
	double numerator, denominator;
	POINT  Q1[3], Q2[3];
	POINT  Q_u, Q1_u, Q2_u;
	double  uPrime;
	int     i;

	Q_u = Bezier(3, (LPPOINT ) Q, u);

	// generate control vertices for Q1
	for (i=0 ; i<=2 ; i++)
		{
		Q1[i].x = (Q[i+1].x - Q[i].x) * 3;
		Q1[i].y = (Q[i+1].y - Q[i].y) * 3;
		}

	// generate control vertices for Q2
	for (i=0 ; i<=1 ; i++)
		{
		Q2[i].x = (Q1[i+1].x - Q1[i].x) * 2;
		Q2[i].y = (Q1[i+1].y - Q1[i].y) * 2;
		}

	Q1_u = Bezier(2, (LPPOINT ) Q1, u);
	Q2_u = Bezier(1, (LPPOINT ) Q2, u);

	numerator = (Q_u.x - P.x) * (Q1_u.x) + (Q_u.y - P.y) * (Q1_u.y);
	denominator = (Q1_u.x) * (Q1_u.x) + (Q1_u.y) * (Q1_u.y)
		+ (Q_u.x - P.x) * (Q2_u.x) + (Q_u.y - P.y) * (Q2_u.y);
	uPrime = u - (numerator / denominator);
	return(uPrime);
}

POINT KCurveTracer::Bezier(int degree, LPPOINT V, double t)
{
	int     	i, j;
	POINT  	Q;
	LPPOINT  Vtemp;

	Vtemp = new POINT[degree + 1];

	for (i=0 ; i<=degree ; i++)
		Vtemp[i] = V[i];

	for (i=1 ; i<=degree ; i++)
		{
		for (j=0 ; j<=degree-i ; j++)
			{
			Vtemp[j].x = (long) ((1.0 - t) * Vtemp[j].x + t * Vtemp[j+1].x);
			Vtemp[j].y = (long) ((1.0 - t) * Vtemp[j].y + t * Vtemp[j+1].y);
			}
		}

	Q = Vtemp[0];
	delete[] Vtemp;
	return( Q );
}

double KCurveTracer::B0(double u)
{
	double temp;

	temp = 1.0 - u;
	return(temp * temp * temp);
}

double KCurveTracer::B1(double u)
{
	double temp;

	temp = 1.0 - u;
	return(3 * u * temp * temp);
}

double KCurveTracer::B2(double u)
{
	double temp;

	temp = 1.0 - u;
	return(3 * u * u * temp);
}

double KCurveTracer::B3(double u)
{
	return(u * u * u);
}

void KCurveTracer::ComputeLeftTangent(int end, LPPOINT ds)
{
	*ds = V2SubII(SourcePoints[end+1], SourcePoints[end]);
	V2Normalize(ds);
}

void KCurveTracer::ComputeRightTangent(int end, LPPOINT ds)
{
	*ds = V2SubII(SourcePoints[end-1], SourcePoints[end]);
	V2Normalize(ds);
}

void KCurveTracer::ComputeCenterTangent(int center, LPPOINT ds)
{
	POINT  V1, V2;

	V1 = V2SubII(SourcePoints[center-1], SourcePoints[center]);
	V2 = V2SubII(SourcePoints[center], SourcePoints[center+1]);

	ds->x = (V1.x + V2.x) / 2;
	ds->y = (V1.y + V2.y) / 2;
	V2Normalize(ds);
}

double* KCurveTracer::ChordLengthParameterize(int first, int last)
{
	int     i;
	double *u;

	u = new double[last - first + 1];

	u[0] = 0.0;
	for (i=first+1 ; i<=last ; i++)
		{
		u[i-first] = u[i-first-1] +
			LengthOfLine(SourcePoints[i], SourcePoints[i-1]);
		}
	for (i=first+1 ; i<=last ; i++)
		u[i-first] = u[i-first] / u[last-first];
	return( u );
}

double KCurveTracer::ComputeMaxError(int first, int last, LPBezierCurve bezCurve, double *u, int *splitPoint)
{
	int     i;
	double  maxDist;
	double  dist;
	POINT  P;
	POINT  V;

	*splitPoint = (last - first + 1) / 2;
	maxDist = 0.0;

	for (i=first+1 ; i<last ; i++)
		{
		P = Bezier(3, (LPPOINT ) bezCurve, u[i-first]);
		POINT sp = { SourcePoints[i].x, SourcePoints[i].y };
		V = V2SubII(P, sp);
		dist = V2SquaredLength(&V);
		if(dist >= maxDist)
			{
			maxDist = dist;
			*splitPoint = i;
			}
		}
	return( maxDist);
}

POINT KCurveTracer::V2AddII(POINT a, POINT b)
{
	POINT c;

	c.x = a.x + b.x;
	c.y = a.y + b.y;
	return(c);
}

POINT KCurveTracer::V2ScaleII(POINT a, double u)
{
	POINT c;

	c.x = (long) (a.x * u);
	c.y = (long) (a.y * u);
	return(c);
}

POINT KCurveTracer::V2SubII(CPOINT& a, CPOINT& b)
{
	POINT c;

	c.x = a.x - b.x;
	c.y = a.y - b.y;
	return(c);
}

POINT KCurveTracer::V2SubII(POINT& a, POINT& b)
{
	POINT c;

	c.x = a.x - b.x;
	c.y = a.y - b.y;
	return(c);
}

double KCurveTracer::V2SquaredLength(POINT *a)
{
	return( (a->x * a->x) + (a->y * a->y) );
}

double KCurveTracer::V2Length(POINT *a)
{
	double	len;
	len = (a->x * a->x) + (a->y * a->y);
	if(len > 0.0)
		return( sqrt(len) );
	return(0.0);
}

void KCurveTracer::V2Negative(POINT *a)
{
	a->x = -(a->x);
	a->y = -(a->y);
}

void KCurveTracer::V2Add(POINT *a, POINT *b, POINT *c)
{
	c->x = a->x + b->x;
	c->y = a->y + b->y;
}

double KCurveTracer::V2Dot(POINT a, POINT b)
{
	return( (a.x * b.x) + (a.y * b.y) );
}

void KCurveTracer::V2Scale(POINT *a, double newlen)
{
	double len;

	len = V2Length(a);
	if(len != 0.0)
		{
		a->x = (long) ((double) a->x * newlen / len);
		a->y = (long) ((double) a->y * newlen / len);
		}
}

void KCurveTracer::V2Normalize(LPPOINT a)
{
	double len;

	len = V2Length(a);
	if(len != 0.0)
		{
		a->x = (long) (a->x * BASE_LONG_VAR / len);
		a->y = (long) (a->y * BASE_LONG_VAR / len);
		}
}

long KCurveTracer::LengthOfPolyline(LP_CPOINT pnts, int num)
{
	double len = 0;
	int		i;

	for (i=0 ; i<num ; i++)
		len += LengthOfLine(pnts[i], pnts[i+1]);
	return (long)len;
}

