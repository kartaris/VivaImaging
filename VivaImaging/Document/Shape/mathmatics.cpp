/**
* @file mathmatics.cpp
* @date 2018.07
* @brief mathmatic functions
*/
#include "stdafx.h"
#include "mathmatics.h"
#include "CurveTracer.h"

int  IsTrianglePoint(LPPOINT path)
{
  if ((ABS_V(path[0].x - path[2].x) == 1) &&
    (ABS_V(path[0].y - path[2].y) == 1) &&
    (ABS_V(path[1].x - path[2].x) + ABS_V(path[1].y - path[2].y) == 1))
  {
    return(1);
  }
  return(0);
}

int CompareCPoint(CPOINT& s, CPOINT& d)
{
  if ((s.x == d.x) && (s.y == d.y))
    return 0;
  int a = s.x - d.x + s.y - d.y;
  return ((a < 0) ? -1 : 1);
}

int PointIsonLine(LPPOINT p1, LPPOINT p2, LPPOINT np, long err)
{
  double src_deg;
  long ox, oy, xerr, yerr;
  POINT	sp, ep;

  sp.x = ((p1->x < p2->x) ? p1->x : p2->x);
  ep.x = ((p1->x < p2->x) ? p2->x : p1->x);
  sp.y = ((p1->y < p2->y) ? p1->y : p2->y);
  ep.y = ((p1->y < p2->y) ? p2->y : p1->y);

  if ((sp.x - err <= np->x) && (ep.x + err >= np->x) &&
    (sp.y - err <= np->y) && (ep.y + err >= np->y))
  {
    // p1, p2 is in screen point
    if ((p2->y == p1->y) || (p2->x == p1->x))
      return(1);
    src_deg = (double)(p2->y - p1->y) / (p2->x - p1->x);
    ox = np->x - p1->x;
    oy = np->y - p1->y;
    yerr = ABS_V((int)(oy - (ox * src_deg)));
    xerr = ABS_V((int)(ox - (oy / src_deg)));
    double ferr = sqrt(xerr * xerr + yerr * yerr);
    if (ferr < err)
      return(1);
  }
  return(0);
}

int MidPointSkipLine(LPPOINT drag_path, int start, int end)
{
  int		i, j;

  for (i = start + 2; i<end; i++)
  {
    // start에서 i 사이의 점들의 거리가 1 이하인지.
    for (j = start + 1; j<i; j++)
    {
      if (!PointIsonLine(&drag_path[start], &drag_path[i], &drag_path[j], 1))
        return(i - 1);
    }
  }
  return(end - 1);
}

int MidPointSkipLine(LP_CPOINT drag_path, int start, int end)
{
  int		i, j;

  for (i = start + 2; i < end; i++)
  {
    // start에서 i 사이의 점들의 거리가 1 이하인지.
    for (j = start + 1; j < i; j++)
    {
      if (!PointIsonLine((LPPOINT)&drag_path[start], (LPPOINT)&drag_path[i], (LPPOINT)&drag_path[j], 1))
        return(i - 1);
    }
  }
  return(end - 1);
}

int ArrangeDragPoints(LPPOINT drag_path, int pnts)
{
  int         i, new_num, start;
  LPPOINT		tp;

  if ((drag_path == (LPPOINT)NULL) || (pnts < 2))
    return(0);
  new_num = pnts;
  tp = (LPPOINT) new POINT[pnts + 2];
  if (tp != (LPPOINT)NULL)
  {
    // first path. delete triangle points
    new_num = 1;
    tp[0] = drag_path[0];
    for (i = 1; i<pnts - 1; i++)
    {
      if (!IsTrianglePoint((POINT  *) &(drag_path[i - 1])))
      {
        tp[new_num] = drag_path[i];
        new_num++;
      }
      else
      {
        drag_path[i].x = drag_path[i + 1].x + 4;
      }
    }
    tp[new_num] = drag_path[pnts - 1];
    new_num++;
    if (new_num != pnts)
    {
      for (i = 0; i<new_num; i++)
        drag_path[i] = tp[i];
    }
    // second path. connect two lines which is equal slops
    pnts = new_num;
    tp[0] = drag_path[0];
    new_num = 1;
    start = 0;

    while (start + 1 < pnts)
    {
      i = MidPointSkipLine(drag_path, start, pnts);
      if (i > 0)
      {
        tp[new_num] = drag_path[i];
        new_num++;
        start = i;
      }
      else
        break;
    }
    tp[new_num] = drag_path[pnts - 1];
    new_num++;
    if (new_num != pnts)
      memcpy((LPSTR)drag_path, (LPSTR)tp, sizeof(POINT) * new_num);
    delete[] tp;
  }
  return(new_num);
}

int ArrangeDragPoints(LP_CPOINT drag_path, int pnts)
{
  int         i, new_num, start;
  LP_CPOINT		tp;

  if ((drag_path == (LP_CPOINT)NULL) || (pnts < 2))
    return(0);
  new_num = pnts;
  tp = (LP_CPOINT) new CPOINT[pnts + 2];
  if (tp != (LP_CPOINT)NULL)
  {
    // first path. delete triangle points
    new_num = 1;
    tp[0] = drag_path[0];
    for (i = 1; i < pnts - 1; i++)
    {
      if (!IsTrianglePoint((LPPOINT)&(drag_path[i - 1])))
      {
        tp[new_num] = drag_path[i];
        new_num++;
      }
      else
      {
        drag_path[i].x = drag_path[i + 1].x + 4;
      }
    }
    tp[new_num] = drag_path[pnts - 1];
    new_num++;
    if (new_num != pnts)
    {
      for (i = 0; i < new_num; i++)
        drag_path[i] = tp[i];
    }
    // second path. connect two lines which is equal slops
    pnts = new_num;
    tp[0] = drag_path[0];
    new_num = 1;
    start = 0;

    while (start + 1 < pnts)
    {
      i = MidPointSkipLine(drag_path, start, pnts);
      if (i <= 0)
        break;

      tp[new_num] = drag_path[i];
      new_num++;
      start = i;
    }
    if (CompareCPoint(tp[new_num - 1], drag_path[pnts - 1]) != 0)
    {
      tp[new_num] = drag_path[pnts - 1];
      new_num++;
    }
    if (new_num != pnts)
      memcpy(drag_path, tp, sizeof(CPOINT) * new_num);
    delete[] tp;
  }
  return(new_num);
}

static int Get900Degree(int degr);
static int GetFragedPieCPoint(long cx, long cy, long sx, long sy, int a1, int a2, int aincr, LP_CPOINT pn, BOOL add_stt);
static int getHCrossingPoint(LPPOINT pnts, int npnum, long basey, long* cpbuff);
static int get_direction_flag(long differ);
static int SortPosition(long *dp, int pnum);

void GetEllipsePoint(long sx, long sy, int Angle, LPPOINT pnts)
{
  double	t_the, tt_sq, xrot_sq;
  int		degr_90, degr;
  double 	xs_sq, ys_sq;

  xs_sq = (double)sx * sx;
  ys_sq = (double)sy * sy;
  degr = Angle;

  if (degr >= 3600)
    degr = (degr % 3600);

  if ((degr == 0) || (degr == 1800))
  {
    pnts->x = sx;
    pnts->y = 0;
  }
  else if ((degr == 900) || (degr == 2700))
  {
    pnts->x = 0;
    pnts->y = sy;
  }
  else
  {
    degr_90 = Get900Degree(degr);
    t_the = (double)tan((double)degr_90 * VALUE_PIE_PER_180 / 10);
    tt_sq = t_the * t_the;
    xrot_sq = ((double)xs_sq * ys_sq) / ((double)ys_sq + (double)xs_sq * tt_sq);

    if (xrot_sq > 0)
    {
      tt_sq = (double)sqrt(xrot_sq);
      pnts->x = (long)tt_sq;
      pnts->y = (long)(tt_sq * t_the);
    }
    else
    {
      pnts->x = sx;
      pnts->y = 0;
    }
  }

  if (degr < 1800)
    pnts->y = -pnts->y;
  if ((degr > 900) && (degr < 2700))
    pnts->x = -pnts->x;
}

static int Get900Degree(int degr)
{
  if ((degr >= 900) && (degr < 1800))
    return(1800 - degr);
  else if ((degr >= 1800) && (degr < 2700))
    return(degr - 1800);
  else if ((degr >= 2700) && (degr < 3600))
    return(3600 - degr);
  return(degr);
}

int  GetPieChordPolyline(LPPOINT center, LPPOINT size, int deg,
  int fst_angle, int snd_angle, int APC, LPPOINT pnt_buff)
{
  int 	id, pnum;
  int 	angle_range;
  int  	angle_incr;
  POINT 	np;

  if (deg != 0)
    angle_incr = deg;
  else
    angle_incr = 50;

  angle_range = snd_angle - fst_angle;
  if (angle_range <= 0)
    angle_range += 3600;

  GetEllipsePoint(size->x, size->y, fst_angle, &np);

  pnt_buff[0].x = (int)(center->x + np.x);
  pnt_buff[0].y = (int)(center->y + np.y);
  pnum = 1;
  id = angle_incr;

  while (id < angle_range + angle_incr)
  {
    if (id > angle_range)
      id = angle_range;
    GetEllipsePoint(size->x, size->y, fst_angle + id, &np);

    pnt_buff[pnum].x = (int)(center->x + np.x);
    pnt_buff[pnum].y = (int)(center->y + np.y);
    id += angle_incr;
    pnum++;
  }

  if (APC == APC_PIE)			// closed-pie
  {
    pnt_buff[pnum].x = (int)(center->x);
    pnt_buff[pnum].y = (int)(center->y);
    pnum++;
    pnt_buff[pnum].x = pnt_buff[0].x;
    pnt_buff[pnum].y = pnt_buff[0].y;
    pnum++;
  }
  else if (APC == APC_CHORD)		// chord
  {
    pnt_buff[pnum].x = pnt_buff[0].x;
    pnt_buff[pnum].y = pnt_buff[0].y;
    pnum++;
  }
  // else arc
  return(pnum);
}

int GetPieChordPolygon(LPPOINT center, LPPOINT size, int deg,
  int fst_angle, int snd_angle, int APC, LP_CPOINT pn)
{
  int 		id, nid, pnum;
  int 		angle_range, first_pt = 0, last_pt = 0;
  int  		angle_incr;

  if (deg != 0)
    angle_incr = deg;
  else
    angle_incr = 50;

  if (snd_angle < fst_angle)
    snd_angle += 3600;

  if ((snd_angle == fst_angle) || (snd_angle == fst_angle + 3600))
  {
    fst_angle = 0;
    snd_angle = 3600;
  }

  angle_range = snd_angle - fst_angle;

  if (angle_range >= 900)
  {
    first_pt = (snd_angle / 900) * 900;
    if ((fst_angle % 900) == 0)
      last_pt = (fst_angle / 900) * 900;
    else
      last_pt = (fst_angle / 900) * 900 + 900;
  }
  else
  {
    last_pt = first_pt = fst_angle;
  }

  pnum = 0;
  if (first_pt != snd_angle)
    pnum = GetFragedPieCPoint(center->x, center->y, size->x, size->y, snd_angle, first_pt, -angle_incr, pn, 1);

  if (first_pt > last_pt)
  {
    id = first_pt;
    if (pnum == 0)
    {
      nid = (id % 3600);
      if ((nid == 0) || (nid == 3600))
      {
        pn[pnum].x = center->x + size->x;
        pn[pnum].y = center->y;
        pn[pnum].a = CPOINT_LINE;
        pnum++;
      }
      else if (nid == 900)
      {
        pn[pnum].x = center->x;
        pn[pnum].y = center->y - size->y;
        pn[pnum].a = CPOINT_LINE;
        pnum++;
      }
      else if (nid == 1800)
      {
        pn[pnum].x = center->x - size->x;
        pn[pnum].y = center->y;
        pn[pnum].a = CPOINT_LINE;
        pnum++;
      }
      else if (nid == 2700)
      {
        pn[pnum].x = center->x;
        pn[pnum].y = center->y + size->y;
        pn[pnum].a = CPOINT_LINE;
        pnum++;
      }
    }

    do
    {
      nid = (id % 3600);
      if ((nid == 0) || (nid == 3600))
      {
        pn[pnum].x = center->x + size->x;
        pn[pnum].y = center->y + (long)((double)size->y / CIRCLE_MAGICS);
        pn[pnum].a = CPOINT_CURV;
        pnum++;
        pn[pnum].x = center->x + (long)((double)size->x / CIRCLE_MAGICS);
        pn[pnum].y = center->y + size->y;
        pn[pnum].a = CPOINT_CURV;
        pnum++;
        pn[pnum].x = center->x;
        pn[pnum].y = center->y + size->y;
        pn[pnum].a = CPOINT_LINE;
        pnum++;
      }
      else if (nid == 900)
      {
        pn[pnum].x = center->x + (long)((double)size->x / CIRCLE_MAGICS);
        pn[pnum].y = center->y - size->y;
        pn[pnum].a = CPOINT_CURV;
        pnum++;
        pn[pnum].x = center->x + size->x;
        pn[pnum].y = center->y - (long)((double)size->y / CIRCLE_MAGICS);
        pn[pnum].a = CPOINT_CURV;
        pnum++;
        pn[pnum].x = center->x + size->x;
        pn[pnum].y = center->y;
        pn[pnum].a = CPOINT_LINE;
        pnum++;
      }
      else if (nid == 1800)
      {
        pn[pnum].x = center->x - size->x;
        pn[pnum].y = center->y - (long)((double)size->y / CIRCLE_MAGICS);
        pn[pnum].a = CPOINT_CURV;
        pnum++;
        pn[pnum].x = center->x - (long)((double)size->x / CIRCLE_MAGICS);
        pn[pnum].y = center->y - size->y;
        pn[pnum].a = CPOINT_CURV;
        pnum++;
        pn[pnum].x = center->x;
        pn[pnum].y = center->y - size->y;
        pn[pnum].a = CPOINT_LINE;
        pnum++;
      }
      else if (nid == 2700)
      {
        pn[pnum].x = center->x - (long)((double)size->x / CIRCLE_MAGICS);
        pn[pnum].y = center->y + size->y;
        pn[pnum].a = CPOINT_CURV;
        pnum++;
        pn[pnum].x = center->x - size->x;
        pn[pnum].y = center->y + (long)((double)size->y / CIRCLE_MAGICS);
        pn[pnum].a = CPOINT_CURV;
        pnum++;
        pn[pnum].x = center->x - size->x;
        pn[pnum].y = center->y;
        pn[pnum].a = CPOINT_LINE;
        pnum++;
      }
      id -= 900;
    } while (id > last_pt);
  }

  if (last_pt != fst_angle)
    pnum += GetFragedPieCPoint(center->x, center->y, size->x, size->y, last_pt, fst_angle, -angle_incr, &(pn[pnum]), (pnum == 0));

  if (snd_angle == fst_angle + 3600)
  {
    pn[pnum - 1].a |= (CPOINT_ENDG | CPOINT_ENDS | CPOINT_CLOSED);
  }
  else
  {
    if (APC == APC_PIE)
    {
      pn[pnum].x = center->x;
      pn[pnum].y = center->y;
      pn[pnum].a = CPOINT_LINE;
      pnum++;
      pn[pnum].x = pn[0].x;
      pn[pnum].y = pn[0].y;
      pn[pnum].a = (CPOINT_LINE | CPOINT_ENDG | CPOINT_ENDS | CPOINT_CLOSED);
      pnum++;
    }
    else if (APC == APC_CHORD)		// chord
    {
      pn[pnum].x = pn[0].x;
      pn[pnum].y = pn[0].y;
      pn[pnum].a = (CPOINT_LINE | CPOINT_ENDG | CPOINT_ENDS | CPOINT_CLOSED);
      pnum++;
    }
  }
  pn[0].a |= CPOINT_STAT;
  return(pnum);
}
/******************************
int GetPieChordPolygon(LPPOINT center, LPPOINT size, int deg,
int fst_angle, int snd_angle, int APC, LP_CPOINT pn)
{
int 		id, nid, pnum;
int 		angle_range, first_pt=0, last_pt=0;
int  		angle_incr;

if(deg != 0)
angle_incr = deg;
else
angle_incr = 50;

if(snd_angle < fst_angle)
snd_angle += 3600;

if((snd_angle == fst_angle) || (snd_angle == fst_angle + 3600))
{
fst_angle = 0;
snd_angle = 3600;
}

angle_range = snd_angle - fst_angle;

if(angle_range >= 900)
{
if((fst_angle % 900) == 0)
first_pt = (fst_angle / 900) * 900;
else
first_pt = (fst_angle / 900) * 900 + 900;
last_pt = (snd_angle / 900) * 900;
}
else
{
last_pt = first_pt = fst_angle;
}

pnum = 0;
if(first_pt != fst_angle)
pnum = GetFragedPieCPoint(center->x, center->y, size->x, size->y, fst_angle, first_pt, angle_incr, pn, 1);

if(first_pt < last_pt)
{
id = first_pt;
if(pnum == 0)
{
nid = (id % 3600);
if((nid == 0) || (nid == 3600))
{
pn[pnum].x = center->x + size->x;
pn[pnum].y = center->y;
pn[pnum].a = CPOINT_LINE;
pnum++;
}
else if(nid == 900)
{
pn[pnum].x = center->x;
pn[pnum].y = center->y - size->y;
pn[pnum].a = CPOINT_LINE;
pnum++;
}
else if(nid == 1800)
{
pn[pnum].x = center->x - size->x;
pn[pnum].y = center->y;
pn[pnum].a = CPOINT_LINE;
pnum++;
}
else if(nid == 2700)
{
pn[pnum].x = center->x;
pn[pnum].y = center->y + size->y;
pn[pnum].a = CPOINT_LINE;
pnum++;
}
}

do
{
nid = (id % 3600);
if((nid == 0) || (nid == 3600))
{
pn[pnum].x = center->x + size->x;
pn[pnum].y = center->y - (long) ((double) size->y / CIRCLE_MAGICS);
pn[pnum].a = CPOINT_CURV;
pnum++;
pn[pnum].x = center->x + (long) ((double) size->x / CIRCLE_MAGICS);
pn[pnum].y = center->y - size->y;
pn[pnum].a = CPOINT_CURV;
pnum++;
pn[pnum].x = center->x;
pn[pnum].y = center->y - size->y;
pn[pnum].a = CPOINT_LINE;
pnum++;
}
else if(nid == 900)
{
pn[pnum].x = center->x - (long) ((double) size->x / CIRCLE_MAGICS);
pn[pnum].y = center->y - size->y;
pn[pnum].a = CPOINT_CURV;
pnum++;
pn[pnum].x = center->x - size->x;
pn[pnum].y = center->y - (long) ((double) size->y / CIRCLE_MAGICS);
pn[pnum].a = CPOINT_CURV;
pnum++;
pn[pnum].x = center->x - size->x;
pn[pnum].y = center->y;
pn[pnum].a = CPOINT_LINE;
pnum++;
}
else if(nid == 1800)
{
pn[pnum].x = center->x - size->x;
pn[pnum].y = center->y + (long) ((double) size->y / CIRCLE_MAGICS);
pn[pnum].a = CPOINT_CURV;
pnum++;
pn[pnum].x = center->x - (long) ((double) size->x / CIRCLE_MAGICS);
pn[pnum].y = center->y + size->y;
pn[pnum].a = CPOINT_CURV;
pnum++;
pn[pnum].x = center->x;
pn[pnum].y = center->y + size->y;
pn[pnum].a = CPOINT_LINE;
pnum++;
}
else if(nid == 2700)
{
pn[pnum].x = center->x + (long) ((double) size->x / CIRCLE_MAGICS);
pn[pnum].y = center->y + size->y;
pn[pnum].a = CPOINT_CURV;
pnum++;
pn[pnum].x = center->x + size->x;
pn[pnum].y = center->y + (long) ((double) size->y / CIRCLE_MAGICS);
pn[pnum].a = CPOINT_CURV;
pnum++;
pn[pnum].x = center->x + size->x;
pn[pnum].y = center->y;
pn[pnum].a = CPOINT_LINE;
pnum++;
}
id += 900;
}
while (id < last_pt);
}

if(last_pt != snd_angle)
pnum += GetFragedPieCPoint(center->x, center->y, size->x, size->y, last_pt, snd_angle, angle_incr, &(pn[pnum]), (pnum == 0));

if(snd_angle == fst_angle + 3600)
{
pn[pnum-1].a |= (CPOINT_ENDG | CPOINT_ENDS | CPOINT_CLOSED);
}
else
{
if(APC == APC_PIE)
{
pn[pnum].x = center->x;
pn[pnum].y = center->y;
pn[pnum].a = CPOINT_LINE;
pnum++;
pn[pnum].x = pn[0].x;
pn[pnum].y = pn[0].y;
pn[pnum].a = (CPOINT_LINE | CPOINT_ENDG | CPOINT_ENDS | CPOINT_CLOSED);
pnum++;
}
else if(APC == APC_CHORD)		// chord
{
pn[pnum].x = pn[0].x;
pn[pnum].y = pn[0].y;
pn[pnum].a = (CPOINT_LINE | CPOINT_ENDG | CPOINT_ENDS | CPOINT_CLOSED);
pnum++;
}
}
pn[0].a |= CPOINT_STAT;
return(pnum);
}
****************/

static int GetFragedPieCPoint(long cx, long cy, long sx, long sy, int a1, int a2, int aincr, LP_CPOINT pn, BOOL add_stt)
{
  int				lpnum, id, pnum;
  POINT 			np, bezp[4];
  LPPOINT		linep;

  GetEllipsePoint(sx, sy, a1, &np);

  // Get Bezier curve from ARC(first_angle -> first_pt)
  linep = new POINT[100];
  linep[0].x = cx + np.x;
  linep[0].y = cy + np.y;
  lpnum = 1;
  id = a1 + aincr;
  while (id > a2 + aincr)
  {
    if (id < a2)
      id = a2;
    GetEllipsePoint(sx, sy, id, &np);
    linep[lpnum].x = cx + np.x;
    linep[lpnum].y = cy + np.y;
    id += aincr;
    lpnum++;
  }

  pnum = 0;
  KCurveTracer ct;

  if ((lpnum > 2) && (ct.LinePolygonToCurve(linep, lpnum, bezp) == 4))
  {
    if (add_stt)
    {
      pn[0].x = bezp[0].x;
      pn[0].y = bezp[0].y;
      pn[0].a = CPOINT_LINE;
      pnum++;
    }
    pn[pnum].x = bezp[1].x;
    pn[pnum].y = bezp[1].y;
    pn[pnum].a = CPOINT_CURV;
    pnum++;
    pn[pnum].x = bezp[2].x;
    pn[pnum].y = bezp[2].y;
    pn[pnum].a = CPOINT_CURV;
    pnum++;
    pn[pnum].x = bezp[3].x;
    pn[pnum].y = bezp[3].y;
    pn[pnum].a = CPOINT_LINE;
    pnum++;
  }
  else
  {
    if (add_stt)
    {
      pn[pnum].x = linep[0].x;
      pn[pnum].y = linep[0].y;
      pn[pnum].a = CPOINT_LINE;
      pnum++;
    }
    if (lpnum > 1)
    {
      pn[pnum].x = linep[lpnum - 1].x;
      pn[pnum].y = linep[lpnum - 1].y;
      pn[pnum].a = CPOINT_LINE;
      pnum++;
    }
  }
  delete[] linep;
  return(pnum);
}

int GetPieFragment(long cx, long cy, long sw, int a1, int a2, int aincr, LP_CPOINT pn, BOOL add_last)
{
  POINT		st1, ed1, st2, ed2, dp, base;
  long		l1, l2;
  int			pnum;
  double		ta1, ta2;
  //double		arad, slop;

  base.x = 0;
  base.y = 0;
  GetEllipsePoint(sw, sw, a1, &st1);
  GetEllipsePoint(sw, sw, a2, &ed1);
  pnum = (a1 < a2) ? 900 : -900;
  ta1 = (double)AddAngle(a1, pnum);
  ta2 = (double)AddAngle(a2, -pnum);

  GetPointFromAngleDistD(ta1, sw, &st2);
  st2.x += st1.x;
  st2.y += st1.y;
  GetPointFromAngleDistD(ta2, sw, &ed2);
  ed2.x += ed1.x;
  ed2.y += ed1.y;
  pnum = 0;

  if (GetLineCrossPoint(&st1, &st2, &ed1, &ed2, &dp))
  {
    l1 = LengthOfLine(&st1, &dp);
    l1 = (long)((double)l1 / CIRCLE_MAGICS);
    GetPointFromAngleDistD(ta1, l1, &st2);
    st2.x += st1.x;
    st2.y += st1.y;
    l2 = LengthOfLine(&ed1, &dp);
    l2 = (long)((double)l2 / CIRCLE_MAGICS);
    GetPointFromAngleDistD(ta2, l2, &ed2);
    ed2.x += ed1.x;
    ed2.y += ed1.y;

    pn[pnum].x = cx + st1.x;
    pn[pnum].y = cy + st1.y;
    pn[pnum].a = CPOINT_LINE;
    pnum++;
    pn[pnum].x = cx + st2.x;
    pn[pnum].y = cy + st2.y;
    pn[pnum].a = CPOINT_CURV;
    pnum++;
    pn[pnum].x = cx + ed2.x;
    pn[pnum].y = cy + ed2.y;
    pn[pnum].a = CPOINT_CURV;
    pnum++;
    if (add_last)
    {
      pn[pnum].x = cx + ed1.x;
      pn[pnum].y = cy + ed1.y;
      pn[pnum].a = CPOINT_LINE;
      pnum++;
    }
  }
  else
  {
    pn[pnum].x = cx + st1.x;
    pn[pnum].y = cy + st1.y;
    pn[pnum].a = CPOINT_LINE;
    pnum++;
    pn[pnum].x = cx + (st1.x + ed1.x) / 2;
    pn[pnum].y = cy + (st1.y + ed1.y) / 2;
    pn[pnum].a = CPOINT_LINE;
    pnum++;
    if (add_last)
    {
      pn[pnum].x = cx + ed1.x;
      pn[pnum].y = cy + ed1.y;
      pn[pnum].a = CPOINT_LINE;
      pnum++;
    }
  }
  return(pnum);
}

#define	BY_MYRULE

BOOL GetLineCrossPoint(LPPOINT p1, LPPOINT p2, LPPOINT s1, LPPOINT s2, LPPOINT dp)
#ifdef	BY_MYRULE
{
  double		a1, b1;
  double		a2, b2;
  double		dx, dy;

  if ((p2->x == p1->x) || (s2->x == s1->x))
  {
    if ((p2->x == p1->x) && (s2->x == s1->x))	// vertical line
      return(FALSE);
    if (p2->x == p1->x)
    {
      a2 = (double)(s2->y - s1->y) / (s2->x - s1->x);
      b2 = (s1->y - a2 * s1->x);
      dy = (double)a2 * p2->x + b2;
      dp->x = p2->x;
      dp->y = (long)dy;
      return(TRUE);
    }
    if (s2->x == s1->x)
    {
      a1 = (double)(p2->y - p1->y) / (p2->x - p1->x);
      b1 = (p1->y - a1 * p1->x);
      dy = (double)a1 * s2->x + b1;
      dp->x = s2->x;
      dp->y = (long)dy;
      return(TRUE);
    }
  }

  a1 = (double)(p2->y - p1->y) / (p2->x - p1->x);
  b1 = (p1->y - a1 * p1->x);
  a2 = (double)(s2->y - s1->y) / (s2->x - s1->x);
  b2 = (s1->y - a2 * s1->x);

  if (a1 == a2)
    return(FALSE);
  dx = (double)(b1 - b2) / (a2 - a1);
  dy = (double)a1 * dx + b1;
  dp->x = (long)(dx + 0.5);
  dp->y = (long)(dy + 0.5);

  // is there horizontal line ?
  if (p2->y == p1->y)
    dp->y = p2->y;
  else if (s2->y == s1->y)
    dp->y = s2->y;
  return(TRUE);
}
#else
{
  long	a1, b1, c1, r1, r2, r3, r4;
  long	a2, b2, c2;
  double 	dn, offset, num;

  a1 = p2->y - p1->y;
  b1 = p1->x - p2->x;
  c1 = p2->x * p1->y - p1->x * p2->y;

  r3 = a1 * s1->x + b1 * s1->y + c1;
  r4 = a1 * s2->x + b1 * s2->y + c1;
  if ((r3 != 0) && (r4 != 0) && IsSameSign(r3, r4))
    return(FALSE);	// does not intersect

  a2 = s2->y - s1->y;
  b2 = s1->x - s2->x;
  c2 = s2->x * s1->y - s1->x * s2->y;
  r1 = a2 * p1->x + b2 * p1->y + c2;
  r2 = a2 * p2->x + b2 * p2->y + c2;
  if ((r1 != 0) && (r2 != 0) && IsSameSign(r1, r2))
    return(FALSE);	// does not intersect

  dn = (double)a1 * b2 - a2 * b1;
  if (dn == (double)0)
    return(FALSE);	// parallel
  offset = dn / 2;
  if (dn < (double)0)
    offset = -offset;

  num = (double)b1 * c2 - b2 * c1;
  dp->x = (long)(((num < 0) ? num - offset : num + offset) / dn);
  num = (double)a2 * c1 - (double)a1 * c2;
  dp->y = (long)(((num < 0) ? num - offset : num + offset) / dn);
  return(TRUE);
}
#endif 


BOOL GetLineCrossPoint2(LPPOINT p1, double a1, LPPOINT s1, double a2, LPPOINT dp)
{
  double		b1, b2, sp1, sp2;
  double		dx, dy, r;

  if (a1 == a2)	// equal angle
    return(FALSE);
  if ((a1 == 900) || (a1 == 2700))		// vertical
  {
    if ((a2 == 900) || (a2 == 2700))	// vertical
      return(FALSE);
    r = (double)a2 * VALUE_PIE_PER_180 / 10;
    sp2 = (double)tan(r);
    b2 = (s1->y - sp2 * (double)s1->x);
    dy = (double)p1->x * sp2 + b2;
    dp->x = p1->x;
    dp->y = (long)(dy + 0.5);
    return(TRUE);
  }
  if ((a2 == 900) || (a2 == 2700))		// vertical
  {
    r = (double)a1 * VALUE_PIE_PER_180 / 10;
    sp1 = (double)tan(r);
    b1 = (p1->y - sp1 * (double)p1->x);
    dy = (double)s1->x * sp1 + b1;
    dp->x = s1->x;
    dp->y = (long)(dy + 0.5);
    return(TRUE);
  }

  if ((a1 == 0) || (a1 == 1800))		// horizontal
    sp1 = (double) 0.0;
  else
  {
    r = (double)a1 * VALUE_PIE_PER_180 / 10;
    sp1 = (double)tan(r);
  }

  if ((a2 == 0) || (a2 == 1800))		// horizontal
    sp2 = (double) 0.0;
  else
  {
    r = (double)a2 * VALUE_PIE_PER_180 / 10;
    sp2 = (double)tan(r);
  }

  b1 = ((double)p1->y + sp1 * (double)p1->x);
  b2 = ((double)s1->y + sp2 * (double)s1->x);

  dx = (b1 - b2) / (sp2 - sp1);
  dy = sp1 * dx + b1;
  dp->x = (long)(dx + 0.5);
  dp->y = (long)(dy + 0.5);

  // is there horizontal line ?
  if ((a1 == 0) || (a1 == 1800))
    dp->y = p1->y;
  if ((a2 == 0) || (a2 == 1800))
    dp->y = s1->y;
  return(TRUE);
}

long GetDistance(LPPOINT sp)
{
  return GetDistance(sp->x, sp->y);
}

long GetDistance(int x, int y)
{
  double val;
  val = (double)x * x + (double)y * y;

  if (val > (double) 0.0)
    return((long)(sqrt(val)));
  return(0);
}

long GetDistance(LPPOINT p1, LPPOINT p2)
{
  return GetDistance(p1->x - p2->x, p1->y - p2->y);
}

long GetDistance(LP_CPOINT p1, LP_CPOINT p2)
{
  return GetDistance(p1->x - p2->x, p1->y - p2->y);
}

long GetDistance(LPPOINT points, int pnum)
{
  long dist = 0;
  for (int i = 0; i<pnum - 1; i++)
    dist += GetDistance(&points[i], &points[i + 1]);
  return dist;
}

long GetLinePointDistance(LPPOINT p1, LPPOINT p2, LPPOINT p3)
{
  double a, b, c, len;

  a = (p2->y - p1->y);
  b = (p1->x - p2->x);
  c = (double)p2->x * p1->y - (double)p1->x * p2->y;

  len = fabs(a * p3->x + b * p3->y + c);
  if (len > 0)
    len = sqrt(len);
  return((int)len);
}

long GetLinePointDistanceOn(POINT& sp, POINT& ep, POINT& pt)
{
  CRect rect;

  rect.left = (sp.x < ep.x) ? sp.x : ep.x;
  rect.right = (sp.x < ep.x) ? ep.x : sp.x;
  rect.top = (sp.y < ep.y) ? sp.y : ep.y;
  rect.bottom = (sp.y < ep.y) ? ep.y : sp.y;

  // out of bound
  if ((rect.left > pt.x) || (rect.right < pt.x) ||
    (rect.top > pt.y) || (rect.bottom < pt.y))
  {
    long l1, l2;

    l1 = GetDistance(sp.x - pt.x, sp.y - pt.y);
    l2 = GetDistance(ep.x - pt.x, ep.y - pt.y);
    if (l1 > l2)
      l1 = l2;
    return l1;
  }
  return GetLinePointDistance(&sp, &ep, &pt);
}

double GetLinePointDistance(CPOINT& p1, CPOINT& p2, CPOINT& p3)
{
  double a, b, c, len;

  a = (p2.y - p1.y);
  b = (p1.x - p2.x);
  c = (double)p2.x * p1.y - (double)p1.x * p2.y;

  len = fabs(a * p3.x + b * p3.y + c);
  if (len > 0)
    len = sqrt(len);
  return len;
}

int AddAngle(int cur, int added)
{
  int ang;

  ang = cur + added;
  if (ang < 0)
    ang += 3600;
  else if (ang >= 3600)
    ang -= 3600;
  return(ang);
}

int GetPolyPointAngle(LPPOINT pnts, int endp)
{
  LPPOINT	ps, pe;
  long    	x, y;
  int     	angle;

  // calculate lines angle from +x' axis.
  ps = pnts;
  pnts++;
  pe = pnts;

  while ((ps->x == pe->x) && (ps->y == pe->y))
  {
    if (endp <= 0)
      return(0);
    pnts++;
    pe = pnts;
    endp--;
  };

  x = pe->x - ps->x;
  y = ps->y - pe->y;

  if (x > 0)
    angle = (int)RADIAN_TO_ANGLE(atan((double)y / x));
  else if (x == 0)
    angle = ((y > 0) ? 900 : 2700);
  else
    angle = 1800 + (int)RADIAN_TO_ANGLE(atan((double)y / x));

  if (angle < 0)
    angle += 3600;
  return(angle);
}

BOOL IsLeftAngle(int sa, int da)
{
  int		min;

  if (sa >= 1800)
  {
    min = sa - 1800;
    if ((min <= da) && (sa >= da))
      return(TRUE);
    else
      return(FALSE);
  }
  min = sa + 1800;
  if ((sa < da) && (da < min))
    return(FALSE);
  return(TRUE);
}

long ConvertDouble(double dbl)
{
  if (dbl >= 0.5)
    return((long)(dbl + 0.5));
  else if (dbl <= -0.5)
    return((long)(dbl - 0.5));
  return((long)dbl);
}

double SinusoidalRatio(double r)
{
  double pa = (double)VALUE_PIE * r - VALUE_PIE / 2;
  r = (sin(pa) + 1.0) / 2;
  return r;
}

void GetPointFromAngleDist(int angle, long dist, LPPOINT po)
{
  if (angle < 0)
    angle += 3600;
  if (angle > 3600)
    angle -= 3600;
  po->x = ConvertDouble(cos(ANGLE_TO_RADIAN(angle)) * dist);
  po->y = ConvertDouble(-sin(ANGLE_TO_RADIAN(angle)) * dist);
}

void GetPointFromAngleDistD(double angle, long dist, LPPOINT po)
{
  if (angle < 0)
    angle += 3600;
  if (angle > 3600)
    angle -= 3600;
  po->x = ConvertDouble(cos(ANGLE_TO_RADIAN(angle)) * dist);
  po->y = ConvertDouble(-sin(ANGLE_TO_RADIAN(angle)) * dist);
}

long LengthOfLine(LPPOINT p1, LPPOINT p2)
{
  double	len;

  len = (double)(p2->x - p1->x) * (p2->x - p1->x) +
    (double)(p2->y - p1->y) * (p2->y - p1->y);
  if (len == 0.0)
    return(0);
  return((long)sqrt(len));
}

double LengthOfLine(CPOINT& p1, CPOINT& p2)
{
  double	len = (double)(p2.x - p1.x) * (p2.x - p1.x) +
    (double)(p2.y - p1.y) * (p2.y - p1.y);
  if (len == 0.0)
    return(0);
  return sqrt(len);
}

double GetLinePointAngle(int x, int y)
{
  double	angle;
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

int GetLinePointAngle(LPPOINT ps, LPPOINT pe)
{
  // calculate lines angle from +x' axis.
  return (int)GetLinePointAngle(pe->x - ps->x, ps->y - pe->y);
}

int GetLinePointAngle(CPOINT& ps, CPOINT& pe)
{
  // calculate lines angle from +x' axis.
  return (int)GetLinePointAngle(pe.x - ps.x, ps.y - pe.y);
}

int GetAngleDiff(int a1, int a2)
{
  int d1 = abs(a1 - a2);
  int d2 = abs(a1 + 3600 - a2);
  if (d1 > d2)
    d1 = d2;
  return d1;
}

static BOOL IsLineLineCross(LPPOINT p, LPPOINT s, LPPOINT cross)
{
  POINT	sp1, ep1, sp2, ep2;

  sp1.x = (p[0].x < p[1].x) ? p[0].x : p[1].x;
  sp1.y = (p[0].y < p[1].y) ? p[0].y : p[1].y;
  ep1.x = (p[0].x < p[1].x) ? p[1].x : p[0].x;
  ep1.y = (p[0].y < p[1].y) ? p[1].y : p[0].y;

  sp2.x = (s[0].x < s[1].x) ? s[0].x : s[1].x;
  sp2.y = (s[0].y < s[1].y) ? s[0].y : s[1].y;
  ep2.x = (s[0].x < s[1].x) ? s[1].x : s[0].x;
  ep2.y = (s[0].y < s[1].y) ? s[1].y : s[0].y;

  if ((sp2.x > ep1.x) || (sp2.y > ep1.y))
    return(FALSE);
  if ((ep2.x < sp1.x) || (ep2.y < sp1.y))
    return(FALSE);
  if (GetLineCrossPoint(&(p[0]), &(p[1]), &(s[0]), &(s[1]), cross))
  {
    if (sp1.x < sp2.x)
      sp1.x = sp2.x;
    if (sp1.y < sp2.y)
      sp1.y = sp2.y;
    if (ep1.x > ep2.x)
      ep1.x = ep2.x;
    if (ep1.y > ep2.y)
      ep1.y = ep2.y;

    if ((cross->x >= sp1.x) && (cross->x <= ep1.x) &&
      (cross->y >= sp1.y) && (cross->y <= ep1.y))
      return(TRUE);
  }
  return(FALSE);
}

#if 0

void ExpandRectangleArea(LP_RECTANGLE sr, long size)
{
  sr->xs -= size;
  sr->ys -= size;
  sr->xe += size;
  sr->ye += size;
}

void AddRectangle(LP_RECTANGLE dst, LP_RECTANGLE s1, LP_RECTANGLE s2)
{
  dst->xs = ((s1->xs < s2->xs) ? s1->xs : s2->xs);
  dst->ys = ((s1->ys < s2->ys) ? s1->ys : s2->ys);
  dst->xe = ((s1->xe > s2->xe) ? s1->xe : s2->xe);
  dst->ye = ((s1->ye > s2->ye) ? s1->ye : s2->ye);
}

void SetRectangleEmpty(LP_RECTANGLE rect)
{
  rect->xs = 1;
  rect->xe = 0;
}

BOOL IsRectangleEmpty(LP_RECTANGLE rect)
{
  return((rect->xs >= rect->xe));
}

void AppendRectangle(LP_RECTANGLE adv, LP_RECTANGLE src)
{
  if ((adv->xs >= adv->xe) || (adv->ys >= adv->ye))
    *adv = *src;
  else
    AddRectangle(adv, adv, src);
}

void AddLPRECTBlock(LPRECT sr, LPRECT add)
{
  if (sr->left > add->left)
    sr->left = add->left;
  if (sr->top > add->top)
    sr->top = add->top;
  if (sr->right < add->right)
    sr->right = add->right;
  if (sr->bottom < add->bottom)
    sr->bottom = add->bottom;
}

void AddRectangleBlock(CRect& sr, CRect& add)
{
  if (sr.IsRectEmpty())
    sr = add;
  else
    sr.UnionRect(&sr, &add);
}

void SetValidRectangle(LP_RECTANGLE rect)
{
  long		tc;

  if (rect->xs > rect->xe)
  {
    tc = rect->xs;
    rect->xs = rect->xe;
    rect->xe = tc;
  }
  if (rect->ys > rect->ye)
  {
    tc = rect->ys;
    rect->ys = rect->ye;
    rect->ye = tc;
  }
}

int CompareRectangleBlock(LPRECT s1, LPRECT s2)
{
  if ((s1->left < s2->left) || (s1->top < s2->top) ||
    (s1->right > s2->right) || (s1->bottom > s2->bottom))
    return(1);
  if ((s1->left == s2->left) && (s1->top == s2->top) &&
    (s1->right == s2->right) && (s1->bottom == s2->bottom))
    return(0);
  return(-1);
}

BOOL RectHasCommonArea(CRect& s1, CRect& s2)
{
  if ((s1.left > s2.right) || (s2.left > s1.right) ||
    (s1.top > s2.bottom) || (s2.top > s1.bottom))
    return(FALSE);
  return(TRUE);
}

BOOL AndRectangleBlock(CRect& s1, CRect& s2)
{
  if (RectHasCommonArea(s1, s2))
  {
    if (s1.left < s2.left)
      s1.left = s2.left;
    if (s1.top < s2.top)
      s1.top = s2.top;
    if (s1.right > s2.right)
      s1.right = s2.right;
    if (s1.bottom > s2.bottom)
      s1.bottom = s2.bottom;
    return((BOOL)(s1.left < s1.right) && (s1.top < s1.bottom));
  }
  return(FALSE);
}

void CopyRectangleToRect(CRect& rt, LP_RECTANGLE rect)
{
  rt.left = rect->xs;
  rt.right = rect->xe;
  rt.top = rect->ys;
  rt.bottom = rect->ye;
}

void CopyCRectToRectangle(LP_RECTANGLE rect, CRect& rt)
{
  rect->xs = rt.left;
  rect->xe = rt.right;
  rect->ys = rt.top;
  rect->ye = rt.bottom;
}

void CopyRectangleToRect(LPRECT rt, LP_RECTANGLE rect)
{
#ifdef WIN32
  rt->left = rect->xs;
  rt->right = rect->xe;
  rt->top = rect->ys;
  rt->bottom = rect->ye;
#else
  if (rect->xs < -32760L)
    rt->left = -32760;
  else if (rect->xs > 32760)
    rt->left = (int)32760;
  else
    rt->left = (int)rect->xs;

  if (rect->ys < -32760L)
    rt->top = -32760;
  else if (rect->ys > 32760)
    rt->top = (int)32760;
  else
    rt->top = (int)rect->ys;

  if (rect->xe < -32760L)
    rt->right = -32760;
  else if (rect->xe > 32760)
    rt->right = (int)32760;
  else
    rt->right = (int)rect->xe;

  if (rect->ye < -32760L)
    rt->bottom = -32760;
  else if (rect->ye > 32760)
    rt->bottom = (int)32760;
  else
    rt->bottom = (int)rect->ye;
#endif 

}


void CopyPOINTToPoint(LPPOINT sp, LPPOINT lp)
{
#ifdef WIN32
  sp->x = lp->x;
  sp->y = lp->y;
#else

  if (lp->x < -32760)
    sp->x = (int)-32760;
  else if (lp->x > 32760)
    sp->x = (int)32760;
  else
    sp->x = (int)lp->x;

  if (lp->y < -32760)
    sp->y = (int)-32760;
  else if (lp->y > 32760)
    sp->y = (int)32760;
  else
    sp->y = (int)lp->y;
#endif 

}

void MoveRectangle(LP_RECTANGLE rect, long xo, long yo)
{
  rect->xs += xo;
  rect->ys += yo;
  rect->xe += xo;
  rect->ye += yo;
}

void RectangleToBlock(LPPOINT p1, LPPOINT p2, LPPOINT sp, LPPOINT ep)
{
  sp->x = ((p1->x < p2->x) ? p1->x : p2->x);
  ep->x = ((p1->x < p2->x) ? p2->x : p1->x);
  sp->y = ((p1->y < p2->y) ? p1->y : p2->y);
  ep->y = ((p1->y < p2->y) ? p2->y : p1->y);
}

int IsRectangleCrossed(LP_RECTANGLE rect, LP_RECTANGLE rect2)
{
  if (rect->xs >= rect2->xe)
    return(0);
  if (rect->ys >= rect2->ye)
    return(0);
  if (rect->xe <= rect2->xs)
    return(0);
  if (rect->ye <= rect2->ys)
    return(0);
  return(1);
}

int IsRectangleMeet(LP_RECTANGLE rect, LP_RECTANGLE rect2)
{
  if ((rect->xs >= rect2->xe) && (rect2->xs != rect2->xe))
    return(0);
  if ((rect->ys >= rect2->ye) && (rect2->ys != rect2->ye))
    return(0);
  if ((rect->xe != rect->xs) && (rect->xe <= rect2->xs))
    return(0);
  if ((rect->ye != rect->ys) && (rect->ye <= rect2->ys))
    return(0);
  return(1);
}

int IsRectangleCrossed(CRect& rect, CRect& rect2)
{
  if (rect.left >= rect2.right)
    return(0);
  if (rect.top >= rect2.bottom)
    return(0);
  if (rect.right <= rect2.left)
    return(0);
  if (rect.bottom <= rect2.top)
    return(0);
  return(1);
}

int IsRectangleMeet(CRect& rect, CRect& rect2)
{
  if ((rect.left >= rect2.right) && (rect2.left != rect2.right))
    return(0);
  if ((rect.top >= rect2.bottom) && (rect2.top != rect2.bottom))
    return(0);
  if ((rect.right != rect.left) && (rect.right <= rect2.left))
    return(0);
  if ((rect.bottom != rect.top) && (rect.bottom <= rect2.top))
    return(0);
  return(1);
}

void CoordToRectArea(LP_RECTANGLE rect, LP_RECTANGLE cod)
{
  if (cod->xs <= cod->xe)
  {
    rect->xs = cod->xs;
    rect->xe = cod->xe;
  }
  else
  {
    rect->xs = cod->xe;
    rect->xe = cod->xs;
  }
  if (cod->ys <= cod->ye)
  {
    rect->ys = cod->ys;
    rect->ye = cod->ye;
  }
  else
  {
    rect->ys = cod->ye;
    rect->ye = cod->ys;
  }
}

void CoordToLogicRect(LP_RECTANGLE rect)
{
  long		tval;

  if (rect->xs > rect->xe)
  {
    tval = rect->xs;
    rect->xs = rect->xe;
    rect->xe = tval;
  }
  if (rect->ys > rect->ye)
  {
    tval = rect->ys;
    rect->ys = rect->ye;
    rect->ye = tval;
  }

}

int PointIsonLine(LPPOINT p1, LPPOINT p2, LPPOINT np, long err)
{
  double src_deg;
  long ox, oy, xerr, yerr;
  POINT	sp, ep;

  sp.x = ((p1->x < p2->x) ? p1->x : p2->x);
  ep.x = ((p1->x < p2->x) ? p2->x : p1->x);
  sp.y = ((p1->y < p2->y) ? p1->y : p2->y);
  ep.y = ((p1->y < p2->y) ? p2->y : p1->y);

  if ((sp.x - err <= np->x) && (ep.x + err >= np->x) &&
    (sp.y - err <= np->y) && (ep.y + err >= np->y))
  {
    // p1, p2 is in screen point
    if ((p2->y == p1->y) || (p2->x == p1->x))
      return(1);
    src_deg = (double)(p2->y - p1->y) / (p2->x - p1->x);
    ox = np->x - p1->x;
    oy = np->y - p1->y;
    yerr = ABS_V((int)(oy - (ox * src_deg)));
    xerr = ABS_V((int)(ox - (oy / src_deg)));
    if (xerr > yerr)
      xerr = yerr;
    if (xerr <= err)
      return(1);
  }
  return(0);
}

int PointIsInsideBoundRect(LP_RECTANGLE rt, LPPOINT np, long err)
{
  if ((rt->xs <= np->x) && (rt->xe >= np->x) &&
    (rt->ys <= np->y) && (rt->ye >= np->y))
    return(1);
  return(0);
}

int PointIsonBoundRect(LP_RECTANGLE rt, LPPOINT np, long err)
{
  if ((((rt->xs - err <= np->x) && (rt->xs + err >= np->x)) ||
    ((rt->xe - err <= np->x) && (rt->xe + err >= np->x))) &&
    ((rt->ys - err <= np->y) && (rt->ye + err >= np->y)))
    return(1);
  return(0);
}

int PointIsonRect(LP_RECTANGLE rt, LPPOINT np, long err)
{
  if ((rt->xs - err <= np->x) && (rt->xe + err >= np->x) &&
    (rt->ys - err <= np->y) && (rt->ye + err >= np->y))
    return(1);
  return(0);
}

int PointIsonPoint(LPPOINT p1, LPPOINT p2, long err)
{
  if ((ABS_V(p1->x - p2->x) <= err) && (ABS_V(p1->y - p2->y) <= err))
    return(1);
  return(0);
}

int PointIsonLinePoly(LP_LPOPOLYPOLYGON lpoly, LPPOINT sp, long err)
{
  LP_LPOPOLYGON	linep;
  int				i, p;
#ifdef		TEST_VER
  HDC				hDC;
  POINT			ps;
#endif 


  linep = (LP_LPOPOLYGON)lpoly->lPoly;
  for (i = 0; i<lpoly->polygonNumber; i++)
  {
    for (p = 0; p<linep->pointNumber - 1; p++)
    {
      if (PointIsonLine(&(linep->lpnts[p]), &(linep->lpnts[p + 1]), sp, err))
        return(1);
    }
    if (linep->polyClose)
    {
      p = linep->pointNumber - 1;

      if (PointIsonLine(&(linep->lpnts[p]), &(linep->lpnts[0]), sp, err))
        return(1);
    }
    linep++;
  }

#ifdef		TEST_VER
  // for testing
  hDC = GetDC(wsheet);
  linep = (LP_LPOPOLYGON)lpoly->lPoly;
  for (i = 0; i<lpoly->polygonNumber; i++)
  {
    ps = linep->lpnts[0];
    DataPointToScreen(&ps);
    MoveTo(hDC, (int)ps.x, (int)ps.y);

    for (p = 1; p<linep->pointNumber; p++)
    {
      ps = linep->lpnts[p];
      DataPointToScreen(&ps);
      LineTo(hDC, (int)ps.x, (int)ps.y);
    }
    if (linep->polyClose)
    {
      ps = linep->lpnts[0];
      DataPointToScreen(&ps);
      LineTo(hDC, (int)ps.x, (int)ps.y);
    }
    linep++;
  }
  ReleaseDC(wsheet, hDC);
#endif 

  return(0);
}

#define	MAX_CROSSPNT_BUFFLEN	300

int PointIsInsideLinePoly(LP_LPOPOLYPOLYGON lpoly, LPPOINT sp, long err, WORD selMode)
{
  LP_LPOPOLYGON	linep;
  int				i, p, match_point, pnum;
  long			*cpnt;
  BOOL			rtn = FALSE;
#ifdef TEST_V
  char			buf1[256], buf2[30];
#endif 

  linep = (LP_LPOPOLYGON)lpoly->lPoly;
  if (selMode & SEL_INNERAREA)
    cpnt = (long *)HCMemAlloc(sizeof(long) * MAX_CROSSPNT_BUFFLEN);
  else
    cpnt = NULL;
  match_point = 0;
  if (selMode & SEL_ONOUTLINE)
  {
    for (i = 0; i<lpoly->polygonNumber; i++)
    {
      pnum = linep->pointNumber - 1;
      for (p = 0; p<pnum; p++)
      {
        if (PointIsonLine(&(linep->lpnts[p]), &(linep->lpnts[p + 1]), sp, err))
        {
          rtn = TRUE;
          goto pilp_complete;
        }
      }
      if (linep->polyClose)
      {
        if (PointIsonLine(&(linep->lpnts[pnum - 1]), &(linep->lpnts[0]), sp, err))
        {
          rtn = TRUE;
          goto pilp_complete;
        }
      }
      linep++;
    }
  }

  if (selMode & SEL_INNERAREA)
  {
    linep = (LP_LPOPOLYGON)lpoly->lPoly;
    for (i = 0; i<lpoly->polygonNumber; i++)
    {
      if (linep->polyClose)
      {
        pnum = getHCrossingPoint(linep->lpnts, linep->pointNumber, sp->y, &(cpnt[match_point]));
        if (pnum & 0x0001)
        {
          qDebug("Odd Number Crossing occured\n");
          pnum--;
        }
        match_point += pnum;
        if (match_point >= MAX_CROSSPNT_BUFFLEN)
        {
          match_point = MAX_CROSSPNT_BUFFLEN;
          qDebug("Cross point buffer overflow\n");
          break;
        }
      }
      linep++;
    }
  }

  if (match_point >= 2)
  {
    match_point = SortPosition(cpnt, match_point);

    if (match_point >= 2)
    {
      for (i = 0; i<match_point; i += 2)
      {
        if (selMode & SEL_ONOUTLINE)
        {
          cpnt[i] -= err;
          cpnt[i + 1] += err;
        }

        if ((cpnt[i] <= sp->x) && (cpnt[i + 1] >= sp->x))
        {
          rtn = TRUE;
          goto pilp_finish;
        }
      }
    }
  }

pilp_finish:
#ifdef TEST_V
  if (match_point > 0)
  {
    wsprintf(buf1, "base=%d : %ld", sp->x, cpnt[i]);
    for (i = 1; i<match_point; i++)
    {
      wsprintf(buf2, ",%ld", cpnt[i]);
      lstrcat(buf1, buf2);
    }
    lstrcat(buf1, "\n");
    qDebug(buf1);
  }
#endif 

pilp_complete:
  if (cpnt != NULL)
    HCMemFree((LPSTR)cpnt);
  return(rtn);
}

static int getHCrossingPoint(LPPOINT pnts, int npnum, long basey, long *cpbuff)
{
  int 	match_point, i;
  int 	stt_slp, old_slp, cur_slp;
  long 	*cpb;
  long	sy, ey, sx, ex;

  match_point = 0;
  cpb = cpbuff;
  i = 0;
  stt_slp = old_slp = 20;

  for (i = 0; i<npnum - 1; i++)
  {
    sy = pnts[i].y;
    ey = pnts[i + 1].y;

    if (((sy <= basey) && (ey >= basey)) ||
      ((sy >= basey) && (ey <= basey)))
    {
      if (match_point >= MAX_CROSSPNT_BUFFLEN)
      {
        qDebug("Cross point buffer overflow\n");
        break;
      }
      else
      {
        if (sy != ey)
        {
          long	mx, my, yo;

          cur_slp = get_direction_flag(ey - sy);
          if (stt_slp == 20)
            stt_slp = cur_slp;

          if (basey == sy)      // include start point
          {
#ifdef  TEST_VALUE
            printf("S(s:%d,%d, %d)", old_slp, cur_slp, i);
#endif 

            if (cur_slp != old_slp)
            {
              *cpb = pnts[i].x;
              cpb++;
              match_point++;
            }
          }
          else if (basey == ey)
          {
            // if( !IsGlyphEndPoint(&(pnts[i+1]), npnum - pi - 2) || (stt_slp != cur_slp) )
            if ((i + 2 <= npnum) || (stt_slp != cur_slp))
            {
              *cpb = pnts[i + 1].x;
              cpb++;
              match_point++;
            }
          }
          else
          {
            if (sy > ey)
            {
              sy = pnts[i + 1].y;
              sx = pnts[i + 1].x;
              ey = pnts[i].y;
              ex = pnts[i].x;
            }
            else
            {
              sx = pnts[i].x;
              ex = pnts[i + 1].x;
            }

            mx = (ex - sx);
            my = (ey - sy);
            yo = (basey - sy);
            *cpb = (long)((double)mx * yo / my) + sx;
            cpb++;
            match_point++;
          }
          old_slp = cur_slp;
        }
      }
    }
  }

  return(match_point);
}

static int get_direction_flag(long differ)
{
  if (differ < 0)
    return(-1);
  else if (differ > 0)
    return(1);
  else
    return(0);
}

static int SortPosition(long *dp, int pnum)
{
  int		i, j, snd;
  long		t;

  if (pnum == 2)
  {
    if (dp[0] > dp[1])
    {
      t = dp[1];
      dp[1] = dp[0];
      dp[0] = t;
    }
    return(2);
  }

  else if (pnum > 2)
  {
    snd = pnum;
    for (i = 0; i<pnum; i++)
    {
      snd--;
      for (j = 0; j<snd; j++)
      {
        if (dp[j] > dp[j + 1])
        {
          t = dp[j + 1];
          dp[j + 1] = dp[j];
          dp[j] = t;
        }
      }
    }

    // delete equal point
    /***
    i = 0;
    while (i+1 < pnum)
    {
    if(dp[i] == dp[i+1])
    {
    pnum -= 2;
    for (j=i ; j<pnum ; j++)
    dp[j] = dp[j+2];
    }
    else
    i++;
    }
    ****/
  }
  else
    return(0);
  return(pnum);
}

long GetVertyLineLen(int ang1, int ang2, long len)
{
  int		dif;

  dif = ABS((ang1 - ang2));
  if (dif >= 3600)
    dif -= 3600;
  if (dif >= 1800)
    dif = 3600 - dif;

  if (dif >= 900)
    dif = dif - 900;
  else
    dif = 900 - dif;
  if (dif == 0)
    return(len);
  else
    return((long)(len * cos(ANGLE_TO_RADIAN(dif))));
}

RECTANGLE GetCurvePolygonRect(int pn, LP_CPOINT npoly)
{
  int			i;
  RECTANGLE	rect;
  // RECTANGLE	area;

  if ((pn < 1) || (npoly == NULL))
  {
    rect.xs = rect.ys = 0;
    rect.xe = rect.ye = 1;
    return(rect);
  }
  rect.xs = rect.xe = npoly->x;
  rect.ys = rect.ye = npoly->y;

  npoly++;
  for (i = 1; i<pn; i++)
  {
    if (rect.xs > npoly->x)
      rect.xs = npoly->x;
    if (rect.ys > npoly->y)
      rect.ys = npoly->y;
    if (rect.xe < npoly->x)
      rect.xe = npoly->x;
    if (rect.ye < npoly->y)
      rect.ye = npoly->y;
    npoly++;
  }
  return(rect);
}

RECTANGLE GetCurvePolygonRectPure(int pn, LP_CPOINT npoly)
{
  int			i;
  RECTANGLE	rect;

  if ((pn < 1) || (npoly == NULL))
  {
    rect.xs = rect.ys = 0;
    rect.xe = rect.ye = 1;
    return(rect);
  }

  rect.xs = rect.xe = npoly->x;
  rect.ys = rect.ye = npoly->y;

  npoly++;
  for (i = 1; i<pn; i++)
  {
    if (npoly->a & CPOINT_CURV)	// is Bezier Segment
    {
      npoly++;
      npoly++;
      i += 2;
    }
    if (rect.xs > npoly->x)
      rect.xs = npoly->x;
    if (rect.ys > npoly->y)
      rect.ys = npoly->y;
    if (rect.xe < npoly->x)
      rect.xe = npoly->x;
    if (rect.ye < npoly->y)
      rect.ye = npoly->y;
    npoly++;
  }
  return(rect);
}

RECTANGLE GetPOINTPolygonRect(int pn, LPPOINT npoly)
{
  int			i;
  RECTANGLE	rect;

  if ((pn < 1) || (npoly == NULL))
  {
    rect.xs = rect.ys = 0;
    rect.xe = rect.ye = 1;
    return(rect);
  }

  rect.xs = rect.xe = npoly->x;
  rect.ys = rect.ye = npoly->y;

  npoly++;
  for (i = 1; i<pn; i++)
  {
    if (rect.xs > npoly->x)
      rect.xs = npoly->x;
    if (rect.ys > npoly->y)
      rect.ys = npoly->y;
    if (rect.xe < npoly->x)
      rect.xe = npoly->x;
    if (rect.ye < npoly->y)
      rect.ye = npoly->y;
    npoly++;
  }
  return(rect);
}

RECT GetPointPolygonRect(int pn, LPPOINT npoly)
{
  int		i;
  RECT	rect;

  if ((pn < 1) || (npoly == NULL))
  {
    rect.left = rect.top = 0;
    rect.right = rect.bottom = 1;
    return(rect);
  }

  rect.left = rect.right = npoly->x;
  rect.top = rect.bottom = npoly->y;
  npoly++;
  for (i = 1; i<pn; i++)
  {
    if (rect.left > npoly->x)
      rect.left = npoly->x;
    if (rect.top > npoly->y)
      rect.top = npoly->y;
    if (rect.right < npoly->x)
      rect.right = npoly->x;
    if (rect.bottom < npoly->y)
      rect.bottom = npoly->y;
    npoly++;
  }
  return(rect);
}

#endif

static POINT po1, po2, po3, po4;


// Bezier Curve Blending Table
static long Blending[51][4] =
{
  65536,     0,     0,     0,
    61681,  3776,    77,     0,
    57982,  7247,   301,     4,
    54433, 10423,   665,    14,
    51032, 13312,  1157,    33,
    47775, 15925,  1769,    65,
    44660, 18270,  2491,   113,
    41684, 20357,  3314,   179,
    38843, 22196,  4227,   268,
    36134, 23795,  5223,   382,
    33554, 25165,  6291,   524,
    31100, 26315,  7422,   697,
    28768, 27254,  8606,   905,
    26556, 27992,  9835,  1151,
    24461, 28538, 11098,  1438,
    22478, 28901, 12386,  1769,
    20606, 29091, 13690,  2147,
    18841, 29118, 15000,  2575,
    17179, 28991, 16307,  3057,
    15619, 28718, 17601,  3596,
    14155, 28311, 18874,  4194,
    12786, 27778, 20115,  4855,
    11509, 27128, 21315,  5582,
    10319, 26372, 22465,  6379,
    9214, 25518, 23555,  7247,
    8192, 24576, 24576,  8192,
    7247, 23555, 25518,  9214,
    6379, 22465, 26372, 10319,
    5582, 21315, 27128, 11509,
    4855, 20115, 27778, 12786,
    4194, 18874, 28311, 14155,
    3596, 17601, 28718, 15619,
    3057, 16307, 28991, 17179,
    2575, 15000, 29118, 18841,
    2147, 13690, 29091, 20606,
    1769, 12386, 28901, 22478,
    1438, 11098, 28538, 24461,
    1151,  9835, 27992, 26556,
    905,  8606, 27254, 28768,
    697,  7422, 26315, 31100,
    524,  6291, 25165, 33554,
    382,  5223, 23795, 36134,
    268,  4227, 22196, 38843,
    179,  3314, 20357, 41684,
    113,  2491, 18270, 44660,
    65,  1769, 15925, 47775,
    33,  1157, 13312, 51032,
    14,   665, 10423, 54433,
    4,   301,  7247, 57982,
    0,    77,  3776, 61681,
    0,     0,     0, 65536,
};

static void GetBlendingPointDbl(int i, LPPOINT m)
{
  double dx, dy;

  dx = ((double)po1.x * Blending[i][0] + (double)po2.x * Blending[i][1] +
    (double)po3.x * Blending[i][2] + (double)po4.x * Blending[i][3]);
  dy = ((double)po1.y * Blending[i][0] + (double)po2.y * Blending[i][1] +
    (double)po3.y * Blending[i][2] + (double)po4.y * Blending[i][3]);

  m->x = (long)((dx + 32768) / 65536);
  m->y = (long)((dy + 32768) / 65536);
}

int ConvertBezToLines(LPPOINT bezp, LPPOINT linep, int nErr)
{
  int				i, Leng, linepnt;
  float			x_len, y_len;

  po1 = bezp[0];
  po2 = bezp[1];
  po3 = bezp[2];
  po4 = bezp[3];

  x_len = (float)(po1.x - po4.x);
  y_len = (float)(po1.y - po4.y);
  y_len = y_len * y_len + x_len * x_len;
  if (y_len > 0)
    x_len = (float)sqrt(y_len);
  else
    x_len = (float)0.0;

  if (x_len <= 4 * nErr)
    Leng = 20;
  else
    Leng = (int)(200 * nErr / (x_len)) + 1;

  linep[0] = po1;
  linepnt = 1;
  i = Leng;

  do
  {
    if (i > 50)
      i = 50;
    GetBlendingPointDbl(i, &linep[linepnt]);
    linepnt++;

    if (i == 50)
      break;
    i += Leng;
  } while (1);
  return(linepnt);
}

int ConvertBezToLines(LPPOINT bezp, LPPOINT linep)
{
  return ConvertBezToLines(bezp, linep, 1);
}

int SplitBezierSegmentByLine(LP_CPOINT bezseg, LPPOINT cutline, int seg)
{
  POINT	bezPoint[4];
  CPOINT	splitbez[16];
  int		pnum, start, pbreak[4];

  for (int i = 0; i < 4; i++)
  {
    bezPoint[i].x = bezseg[i].x;
    bezPoint[i].y = bezseg[i].y;
  }
  memset(pbreak, 0, sizeof(int) * 4);
  pnum = SplitBezierByLine(bezPoint, cutline, splitbez, pbreak);
  if ((pnum > 0) && (pbreak[seg] > 0))
  {
    pnum = (seg == 0) ? pbreak[seg] : (pbreak[seg] - pbreak[seg - 1]);
    start = (seg == 0) ? 0 : pbreak[seg - 1];
    // return specified segment only
    if (pnum > 0)
      memcpy(bezseg, &(splitbez[start]), sizeof(CPOINT) * pnum);
  }
  else
    pnum = 0;
  return(pnum);
}

int SplitBezierByLine(LPPOINT bezp, LPPOINT cutt, LP_CPOINT splitbez, int* first_seg)
{
  LPPOINT		source;
  int				i, fstseg, pnum;
  POINT			cross, tmp;
  int				start, maded;
  KCurveTracer ct;

  source = (LPPOINT) new POINT[60];
  fstseg = ConvertBezToLines(bezp, source);
  if (fstseg < 2)
  {
    //_TRACE("Bezier to Line conversion fault\n");
    delete[] source;
    return(0);
  }
  start = 0;
  maded = 0;
  for (i = 0; i < fstseg - 1; i++)
  {
    if (IsLineLineCross(&(source[i]), cutt, &cross))
    {
      tmp = source[i + 1];
      source[i + 1] = cross;
      // make bezier from [start] to [i+1]
      pnum = i + 1 - start + 1;
      maded += ct.LinePolygonToCurveCP(&(source[start]), pnum, &(splitbez[maded]));
      if (first_seg != NULL)
      {
        *first_seg = maded;
        first_seg++;
      }
      source[i] = cross;
      source[i + 1] = tmp;
      start = i;
    }
  }

  if (start < fstseg - 2)
  {
    pnum = fstseg - start;
    maded += ct.LinePolygonToCurveCP(&(source[start]), pnum, &(splitbez[maded]));
  }
  else if (start == fstseg - 2)
  {
    if ((source[start].x != source[start + 1].x) || (source[start].y != source[start + 1].y))
    {
      splitbez[maded].x = source[start].x;
      splitbez[maded].y = source[start].y;
      splitbez[maded].a = CPOINT_LINE;
      maded++;
      splitbez[maded].x = source[start + 1].x;
      splitbez[maded].y = source[start + 1].y;
      splitbez[maded].a = CPOINT_LINE;
      maded++;
    }
  }
  delete[] source;
  if (first_seg != NULL)
  {
    *first_seg = maded;
  }
  return(maded);
}

void HFlopCPoint(int pnum, LP_CPOINT pn, long origin)
{
  int				i;
  long			posx;

  for (i = 0; i < pnum; i++)
  {
    posx = (pn[i].x - origin);
    pn[i].x = origin - posx;
  }
}

void VFlopCPoint(int pnum, LP_CPOINT pn, long origin)
{
  int				i;
  long			posy;

  for (i = 0; i < pnum; i++)
  {
    posy = (pn[i].y - origin);
    pn[i].y = origin - posy;
  }
}
