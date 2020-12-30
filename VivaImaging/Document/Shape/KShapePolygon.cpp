/**
* @file KShapePolygon.cpp
* @date 2018.05
* @brief KShapePolygon class file
*/
#include "stdafx.h"
#include "KShapePolygon.h"
#include "..\KDragAction.h"
#include "mathmatics.h"

/**
* @class Line
* @brief Line 개체 클래스
*/
KShapePolygon::KShapePolygon()
  : KShapeBase()
{
  Rect.left = Rect.right = Rect.top = Rect.bottom = 0;
  Points = NULL;
  allocNumber = 0;
  PointNumber = 0;
  Outline.Color = 0;
  Outline.Width = 1;
  Outline.Style = 0;
  StartArrow = 0;
  EndArrow = 0;
  Fill = 0;
  Closed = FALSE;
}

KShapePolygon::KShapePolygon(LP_CPOINT pts, int point_num, BOOL closed)
  : KShapeBase()
{
  Points = NULL;
  allocNumber = 0;
  PointNumber = 0;
  Outline.Color = 0;
  Outline.Width = 1;
  Outline.Style = 0;
  StartArrow = 0;
  EndArrow = 0;
  Fill = 0;
  Closed = closed;
  AddPoints(pts, point_num);
}

KShapePolygon::KShapePolygon(LP_CPOINT pts, int point_num, DWORD fillcolor, OutlineStyle linestyle, BOOL closed)
  : KShapeBase()
{
  Points = NULL;
  allocNumber = 0;
  PointNumber = 0;
  Outline = linestyle;
  StartArrow = 0;
  EndArrow = 0;
  Fill = fillcolor;
  Closed = closed;
  AddPoints(pts, point_num);
}

KShapePolygon::~KShapePolygon()
{
  if (Points != NULL)
  {
    delete[] Points;
    Points = NULL;
  }
  allocNumber = 0;
  PointNumber = 0;
}

/**
* @brief 개체의 위치를 지정한 만큼 이동한다.
* @param offset : 이동할 양
* @return bool : 성공적으로 이동되면 true를 리턴한다.
*/
BOOL KShapePolygon::Move(POINT offset)
{
  if ((PointNumber > 0) && (Points != NULL))
  {
    for (int c = 0; c < PointNumber; c++)
    {
      Points[c].x += offset.x;
      Points[c].y += offset.y;
    }
  }
  return TRUE;
}

/**
* @brief 핸들의 이동에 따라 개체의 크기를 변경하는 가상 함수.
* @param type : 핸들 종류
* @return offset : 핸들의 이동량
*/
BOOL KShapePolygon::ResizeByObjectHandle(EditHandleType type, POINT offset)
{
  if ((offset.x != 0) || (offset.y != 0))
  {
      if (type == EditHandleType::ResizeLeftTop)
      {
        // TODO : 
      }
      else if (type == EditHandleType::ResizeRightBottom)
      {
        // TODO : 
      }
      return TRUE;
  }
  return FALSE;
}

BOOL KShapePolygon::AddPoints(LPPOINT points, int number)
{
  int need = PointNumber + number;
  if (need > allocNumber)
  {
    allocNumber = need / 16 + 1;
    allocNumber = allocNumber * 16;
    LP_CPOINT buff = new CPOINT[allocNumber];
    if (PointNumber > 0)
    {
      memcpy(buff, Points, sizeof(CPOINT) * PointNumber);
      delete[] Points;
    }
    Points = buff;
  }
  for (int i = 0; i < number; i++)
  {
    Points[PointNumber].x = points[i].x;
    Points[PointNumber].y = points[i].y;
    Points[PointNumber].a = 0;
    PointNumber++;
  }

  Rect = CalcRectangle();
  return TRUE;
}

BOOL KShapePolygon::AddPoints(LP_CPOINT points, int count)
{
  int need = PointNumber + count;
  if (need > allocNumber)
  {
    allocNumber = need / 16 + 1;
    allocNumber = allocNumber * 16;
    LP_CPOINT buff = new CPOINT[allocNumber];
    if (PointNumber > 0)
    {
      memcpy(buff, Points, sizeof(CPOINT) * PointNumber);
      delete[] Points;
    }
    Points = buff;
  }
  for (int i = 0; i < count; i++)
  {
    Points[PointNumber].x = points[i].x;
    Points[PointNumber].y = points[i].y;
    Points[PointNumber].p = points[i].p;
    Points[PointNumber].a = points[i].a;

    if (Points[PointNumber].p == 0)
      Points[PointNumber].p = 100;
    else if (Points[PointNumber].p > 1200)
      Points[PointNumber].p = 1200;

    PointNumber++;
  }

  Rect = CalcRectangle();
  return TRUE;
}

BOOL KShapePolygon::FlattenPolygon()
{
  // dumpPoints();
  if (PointNumber > 2)
  {
    LP_CPOINT buff = new CPOINT[PointNumber];

    memcpy(buff, Points, sizeof(CPOINT) * PointNumber);

    int pnum = ArrangeDragPoints(buff, PointNumber);

    delete[] Points;
    PointNumber = 0;
    allocNumber = 0;

    if (pnum > 1)
    {
#ifdef USE_CURVE_TRACE
      LP_CPOINT cPoints = new CPOINT[pnum * 2];
      pnum = FitCurvePoint(buff, pnum, 30, 20, FIT_BY_DUAL, cPoints);
      delete[] buff;

      if (pnum > 2)
      {
        PointNumber = allocNumber = pnum;
        Points = new CPOINT[allocNumber];
        memcpy(Points, cPoints, sizeof(CPOINT) * allocNumber);
        delete[] cPoints;
        return TRUE;
    }
      else
      {
        delete[] cPoints;
      }
#else
      PointNumber = allocNumber = pnum;
      Points = new CPOINT[allocNumber];
      memcpy(Points, buff, sizeof(CPOINT) * PointNumber);
      for (int i = 0; i < PointNumber; i++)
      {
        Points[i].a = CPOINT_LINE;
      }
      delete[] buff;
#endif

      // Bezier 커브로 변환
      MakeCurvePolygon();
      // dumpPoints();

      return TRUE;
    }
  }
  return FALSE;
}

BOOL KShapePolygon::MakeCurvePolygon()
{
  if (PointNumber > 4)
  {
    for (int i = 0; i < PointNumber; i++)
    {
      if (Points[i].a & CPOINT_CURV)
        return FALSE;
    }
  }

  dumpPoints();

  KCurveTracer ct;
  int buffSize = PointNumber * 3 + 10;
  LP_CPOINT dest = new CPOINT[buffSize];
  int resultCount = ct.FitCurvePoint(Points, PointNumber, FIT_BY_DUAL, dest, buffSize);

  if (resultCount > 0)
  {
    delete[] Points;
    Points = new CPOINT[resultCount];
    memcpy(Points, dest, sizeof(CPOINT) * resultCount);
    PointNumber = resultCount;
  }
  delete[] dest;
  dumpPoints();
  return (resultCount > 0);
}

void KShapePolygon::dumpPoints()
{
  TCHAR msg[64];

  StringCchPrintf(msg, 64, _T("PointNumber=%d\n"), PointNumber);
  OutputDebugString(msg);
  for (int i = 0; i < PointNumber; i++)
  {
    StringCchPrintf(msg, 64, _T("(%d,%d),p=%d,a=%d\n"), Points[i].x, Points[i].y, Points[i].p, Points[i].a);
    OutputDebugString(msg);
  }
}

BOOL KShapePolygon::Move(POINT& offset)
{
  for (int i = 0; i < PointNumber; i++)
  {
    Points[i].x += offset.x;
    Points[i].y += offset.y;
  }
  Rect = CalcRectangle();
  return TRUE;
}

EditHandleType KShapePolygon::HitObjectsHandle(KImageDrawer& drawer, POINT& pt)
{
  return drawer.HitRectangleHandle(Rect, pt);
}

EditHandleType KShapePolygon::HitOver(KImageDrawer& drawer, POINT& pt)
{
  if (!Closed && (PointNumber > 0))
  {
    if (drawer.IsOverPoint(Points[0].x, Points[0].y, pt))
      return EditHandleType::MoveObjectHandle;

    for (int i = 1; i < PointNumber; i++)
    {
      if (drawer.IsOverPoint(Points[i].x, Points[i].y, pt))
        return EditHandleType::MoveObjectHandle;
    }
  }

  if (drawer.HitOver(Rect, ShapeType::ShapeRectangle, pt) > EditHandleType::HandleNone)
    return EditHandleType::MoveObjectHandle;

  return EditHandleType::HandleNone;
}

BOOL KShapePolygon::HitOver(POINT& pt)
{
  if ((Rect.left <= pt.x) && (pt.x <= Rect.right) &&
    (Rect.top <= pt.y) && (pt.y <= Rect.bottom))
  {
    return TRUE;
  }
  return FALSE;
}

RECT KShapePolygon::CalcRectangle()
{
  RECT r = { 0, 0, 0, 0 };

  if (PointNumber > 0)
  {
    r.left = r.right = Points[0].x;
    r.top = r.bottom = Points[0].y;
    for (int i = 1; i < PointNumber; i++)
    {
      if (r.left > Points[i].x)
        r.left = Points[i].x;
      if (r.top > Points[i].y)
        r.top = Points[i].y;
      if (r.right < Points[i].x)
        r.right = Points[i].x;
      if (r.bottom < Points[i].y)
        r.bottom = Points[i].y;
    }
  }
  return r;
}

BOOL KShapePolygon::IsPressureSensitiveDrawing()
{
  if (PointNumber > 0)
    return (Points[0].p > 0);
  return FALSE;
}

void KShapePolygon::GetRectangle(RECT& rect)
{
  memcpy(&rect, &Rect, sizeof(RECT));
}

void KShapePolygon::GetNormalRectangle(RECT& rect)
{
  memcpy(&rect, &Rect, sizeof(RECT));
}

RECT KShapePolygon::GetBounds(KImageDrawer* drawer)
{
  RECT r = Rect;
  int pen_hw = Outline.Width / 2 + 1;
  AddToBounds(r, drawer, pen_hw);
  return r;
}

Gdiplus::Point calcPathPoint(int x, int y, KDragAction* drag, KImageDrawer& info, RECT& base, RECT& conv)
{
  Gdiplus::Point s;

  if ((drag != NULL) && (drag->mouseDragMode > MouseDragMode::CreateNewObject))
  {
    POINT p = { x, y };
    drag->MakeResizedPoint(p, base, conv);
    s = Point(p.x, p.y);
  }
  else
  {
    s = Point(x, y);
  }
  info.ImageToScreen(s);
  return s;
}

void KShapePolygon::readyGraphicsPath(GraphicsPath& path, KImageDrawer& info, KDragAction* drag)
{
  Gdiplus::Point s, e;
  RECT r;

  if ((drag != NULL) && (drag->mouseDragMode > MouseDragMode::CreateNewObject))
    r = drag->MakeResizedRect(Rect);

  s = calcPathPoint(Points[0].x, Points[0].y, drag, info, Rect, r);

  for (int i = 1; i < PointNumber; i++)
  {
    e = calcPathPoint(Points[i].x, Points[i].y, drag, info, Rect, r);

    if ((Points[i].a & CPOINT_CURV) && (Points[i + 1].a & CPOINT_CURV) && ((i+2) < PointNumber))
    {
#ifdef _DEBUG
      ASSERT((Points[i + 1].a & CPOINT_CURV) == CPOINT_CURV);
#endif

      Point b = calcPathPoint(Points[i + 1].x, Points[i + 1].y, drag, info, Rect, r);
      Point c = calcPathPoint(Points[i + 2].x, Points[i + 2].y, drag, info, Rect, r);

      path.AddBezier(s, e, b, c);
      i += 2;
      s = c;
    }
    else
    {
      path.AddLine(s, e);
      s = e;
    }
  }

}

#define THRESHOLD_PRESSURE_LEVEL 50

int getEstimateLen(Gdiplus::Point s, Gdiplus::Point e)
{
  int dx = s.X - e.X;
  int dy = s.Y - e.Y;
  return (int)sqrt(dx * dx + dy * dy);
}

int getEstimateLen(Gdiplus::Point s, Gdiplus::Point e, Gdiplus::Point b, Gdiplus::Point c)
{
  int len = getEstimateLen(s, e);
  len += getEstimateLen(e, b);
  len += getEstimateLen(b, c);
  return len;
}

int KShapePolygon::getPathSegment(int o, GraphicsPath& path, KImageDrawer& info, float& penWidth, KDragAction* drag)
{
  Gdiplus::Point s, e;
  RECT r;

  path.Reset();
  if (o >= PointNumber)
    return -1;

  s = calcPathPoint(Points[o].x, Points[o].y, drag, info, Rect, r);
  WORD p = Points[o].p;

  float n_width;
  float sum_width = 0;
  float sum_len = 0;
  int len;

  for (int i = o + 1; i < PointNumber; i++)
  {
    e = calcPathPoint(Points[i].x, Points[i].y, drag, info, Rect, r);

    if ((Points[i].a & CPOINT_CURV) && (Points[i + 1].a & CPOINT_CURV))
    {
#ifdef _DEBUG
      ASSERT((Points[i + 1].a & CPOINT_CURV) == CPOINT_CURV);
#endif

      Point b = calcPathPoint(Points[i + 1].x, Points[i + 1].y, drag, info, Rect, r);
      Point c = calcPathPoint(Points[i + 2].x, Points[i + 2].y, drag, info, Rect, r);

      n_width = Outline.Width * info.currentZoom;
      n_width = n_width * Points[i + 1].p / PRESSURE_SENSITIVE_WIDTH_BASE;

      path.AddBezier(s, e, b, c);
      len = getEstimateLen(s, e, b, c);
      i += 2;
      s = c;
    }
    else
    {
      n_width = Outline.Width * info.currentZoom;
      n_width = n_width * Points[i].p / PRESSURE_SENSITIVE_WIDTH_BASE;

      path.AddLine(s, e);
      len = getEstimateLen(s, e);
      s = e;
    }

    if (sum_width > 0)
    {
      float width_diff = abs(sum_width / sum_len - n_width);
      if (width_diff > 0.5)
      {
        penWidth = sum_width / sum_len;
        return i;
      }
    }
    else
    {
      sum_width += n_width * len;
      sum_len += len;
    }
  }
  if (sum_len > 0)
    penWidth = sum_width / sum_len;
  else
    penWidth = 1.0;
  return PointNumber;
}

void KShapePolygon::Draw(Graphics& g, KImageDrawer& info, int flag)
{
  if (PointNumber > 1)
  {
    RECT r = Rect;
    info.ImageToScreen(r);

    if (IsPressureSensitiveDrawing())
    {
      int offset = 0;
      float penWidth;
      GraphicsPath path;

      while (offset < PointNumber)
      {
        offset = getPathSegment(offset, path, info, penWidth, NULL);

        drawPathShape(g, info, &path, flag, penWidth);
      }

#ifdef _DEBUG_BEZIER_SEGMENT
      Gdiplus::Point s, e;
      Gdiplus::SolidBrush brush(0xFFFFFFFF);
      Gdiplus::Font font(_T("Arial"), 8, 0);
      Pen pen(Color(0xFFFF00FF), 1);

      for (int i = 1; i < (PointNumber + 1); i++)
      {
        s = calcPathPoint(Points[i - 1].x, Points[i - 1].y, NULL, info, Rect, r);
        if (info.GetZoom() > 4)
        {
          Gdiplus::PointF org(s.X, s.Y);
          TCHAR str[16];
          StringCchPrintf(str, 16, _T("%d"), i - 1);
          g.DrawString(str, lstrlen(str), &font, org, &brush);
        }

        if (i >= (PointNumber - 1)) 
          break;
        if ((Points[i].a & CPOINT_CURV) && (Points[i + 1].a & CPOINT_CURV))
        {
          s = calcPathPoint(Points[i-1].x, Points[i - 1].y, NULL, info, Rect, r);
          e = calcPathPoint(Points[i].x, Points[i].y, NULL, info, Rect, r);
          g.DrawLine(&pen, s, e);

          s = calcPathPoint(Points[i+2].x, Points[i+2].y, NULL, info, Rect, r);
          e = calcPathPoint(Points[i+1].x, Points[i+1].y, NULL, info, Rect, r);
          g.DrawLine(&pen, s, e);

          i += 2;
        }
      }
#endif
    }
    else
    {
      GraphicsPath path;
      readyGraphicsPath(path, info, NULL);

      drawPathShape(g, info, &path, flag);
    }

    if (IsSelected() && !(flag & DRAW_NO_HANDLE))
    {
      info.DrawBoundingHandle(g, r, 0);
    }
  }
}

void KShapePolygon::drawPathShape(Graphics& g, KImageDrawer& info, GraphicsPath* path, int flag, float penWidth)
{
  BYTE a = GetAlphaValue(Fill);
  if ((a > 0) && Closed)
  {
    SolidBrush brush(Fill);
    g.FillPath(&brush, path);
  }

  if (Outline.Style > (BYTE)LineStyle::LineHollow)
  {
    /*
    float width = Outline.Width * info.currentZoom;
    if (prs > 0)
      width = width * prs / PRESSURE_SENSITIVE_WIDTH_BASE;
    */

    Pen pen(Color(Outline.Color), penWidth);
    pen.SetLineCap(LineCap::LineCapRound, LineCap::LineCapRound, DashCap::DashCapRound);

    if (Outline.Style == (BYTE)LineStyle::LineDot)
      pen.SetDashStyle(DashStyleDot);
    else if (Outline.Style == (BYTE)LineStyle::LineDash)
      pen.SetDashStyle(DashStyleDash);
    else if (Outline.Style == (BYTE)LineStyle::LineDashDot)
      pen.SetDashStyle(DashStyleDashDot);

    g.DrawPath(&pen, path);
  }
}

void KShapePolygon::OnRenderRubber(Graphics& g, KDragAction* drag, KImageDrawer& info, int flag)
{
  if (PointNumber > 1)
  {
    GraphicsPath path;
    readyGraphicsPath(path, info, drag);

    RECT r = drag->MakeResizedRect(Rect);
    info.ImageToScreen(r);

    drawPathShape(g, info, &path, flag);
  }
}

void KShapePolygon::OnDragEnd(KDragAction* drag, POINT& amount)
{
  if (PointNumber > 0)
  {
    POINT s;
    RECT r = drag->MakeResizedRect(Rect, amount);

    for (int i = 0; i < PointNumber; i++)
    {
      s.x = Points[i].x;
      s.y = Points[i].y;
      drag->MakeResizedPoint(s, Rect, r);
      Points[i].x = s.x;
      Points[i].y = s.y;
    }
    Rect = CalcRectangle();
  }
}

void KShapePolygon::GetDragBounds(RECT& bound, KDragAction* drag)
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

BOOL KShapePolygon::GetFillColor(DWORD& color)
{
  if (!Closed)
    return FALSE;

  color = Fill;
  return TRUE;
}

BOOL KShapePolygon::GetOutlineStyle(OutlineStyle& linestyle)
{
  memcpy(&linestyle, &Outline, sizeof(OutlineStyle));
  return TRUE;
}

BOOL KShapePolygon::SetFillColor(DWORD& color)
{
  Fill = color;
  return TRUE;
}

BOOL KShapePolygon::SetOutlineStyle(OutlineStyle& linestyle)
{
  memcpy(&Outline, &linestyle, sizeof(OutlineStyle));
  return TRUE;
}

#ifdef USE_XML_STORAGE

HRESULT KShapePolygon::ReadAttributes(IXmlReader* pReader)
{
  const WCHAR* pwszPrefix;
  const WCHAR* pwszLocalName;
  const WCHAR* pwszValue;

  HRESULT hr = pReader->MoveToFirstAttribute();
  while (hr != S_FALSE)
  {
    if (!pReader->IsDefault())
    {
      UINT cwchPrefix;
      if (FAILED(hr = pReader->GetPrefix(&pwszPrefix, &cwchPrefix))) {
        TCHAR msg[256];
        StringCchPrintf(msg, 256, _T("Error getting prefix, error is %08.8lx"), hr);
        StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
        return hr;
      }

      if (FAILED(hr = pReader->GetLocalName(&pwszLocalName, NULL))) {
        TCHAR msg[256];
        StringCchPrintf(msg, 256, _T("Error getting local name, error is %08.8lx"), hr);
        StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
        return hr;
      }

      if (FAILED(hr = pReader->GetValue(&pwszValue, NULL))) {
        TCHAR msg[256];
        StringCchPrintf(msg, 256, _T("Error getting value, error is %08.8lx"), hr);
        StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
        return hr;
      }

      /*
      if (cwchPrefix > 0)
      wprintf(L"Attr: %s:%s=\"%s\" \n", pwszPrefix, pwszLocalName, pwszValue);
      else
      wprintf(L"Attr: %s=\"%s\" \n", pwszLocalName, pwszValue);
      */

      if (CompareStringNC(pwszLocalName, tagClosed) == 0)
      {
        Closed = (CompareStringNC(pwszValue, tagBoolValue[1]) == 0);
      }
    }

    hr = pReader->MoveToNextAttribute();
  }
  return hr;
}

int KShapePolygon::parsePointValues(LPCTSTR szValue)
{
  POINT pt;
  int count = 0;

  while ((szValue != NULL) && (*szValue != '\0'))
  {
    LPCTSTR n = StrChr(szValue, ',');
    if ((n != NULL) &&
      (swscanf_s(szValue, _T("%d"), &pt.x) == 1))
    {
      szValue = n + 1;
      n = StrChr(szValue, ',');
      if ((n != NULL) &&
        (swscanf_s(szValue, _T("%d"), &pt.y) == 1))
      {
        AddPoints(&pt, 1);
        szValue = n + 1;
        count++;
      }
      else
      {
        szValue = NULL;
      }
    }
    else
    {
      szValue = NULL;
    }
  }
  return count;
}

HRESULT KShapePolygon::Read(CComPtr<IXmlReader> pReader)
{
  HRESULT hr;

  if (FAILED(hr = ReadAttributes(pReader)))
  {
    TCHAR msg[256];
    StringCchPrintf(msg, 256, _T("Error reading attributes, error is %08.8lx"), hr);
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
    return hr;
  }
  if (FAILED(hr = pReader->MoveToElement()))
  {
    TCHAR msg[256];
    StringCchPrintf(msg, 256, _T("Error moving to the element that owns the current attribute node, error is %08.8lx"), hr);
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
    return hr;
  }
  if (pReader->IsEmptyElement())
    return S_OK;

  XmlNodeType nodeType;
  const WCHAR* pwszPrefix;
  const WCHAR* pwszLocalName;
  const WCHAR* pwszValue;
  UINT cwchPrefix;

  while (S_OK == (hr = pReader->Read(&nodeType)))
  {
    switch (nodeType) {

    case XmlNodeType_Element:
      if (FAILED(hr = pReader->GetPrefix(&pwszPrefix, &cwchPrefix)))
      {
        TCHAR msg[256];
        StringCchPrintf(msg, 256, _T("Error getting prefix on Element, error is %08.8lx"), hr);
        StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
        return hr;
      }
      if (FAILED(hr = pReader->GetLocalName(&pwszLocalName, NULL)))
      {
        TCHAR msg[256];
        StringCchPrintf(msg, 256, _T("Error getting local name on Element, error is %08.8lx"), hr);
        StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
        return hr;
      }
      /*
      if (cwchPrefix > 0)
      wprintf(L"Element: %s:%s\n", pwszPrefix, pwszLocalName);
      else
      wprintf(L"Element: %s\n", pwszLocalName);
      */
      if (CompareStringNC(pwszLocalName, tagFill) == 0)
      {
        hr = ReadFill(Fill, pReader);
        if (FAILED(hr))
          return hr;
      }
      else if (CompareStringNC(pwszLocalName, tagOutline) == 0)
      {
        hr = ReadOutline(Outline, pReader);
        if (FAILED(hr))
          return hr;
      }

      if (FAILED(hr = pReader->MoveToElement())) {
        TCHAR msg[256];
        StringCchPrintf(msg, 256, _T("Error moving to the element that owns the current attribute node, error is %08.8lx"), hr);
        StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
        return hr;
      }
      /*
      if (pReader->IsEmptyElement())
      wprintf(L" (empty element)\n");
      */
      break;

    case XmlNodeType_EndElement:
      if (FAILED(hr = pReader->GetPrefix(&pwszPrefix, &cwchPrefix))) {
        TCHAR msg[256];
        StringCchPrintf(msg, 256, _T("Error getting prefix on EndElement, error is %08.8lx"), hr);
        StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
        return hr;
      }
      if (FAILED(hr = pReader->GetLocalName(&pwszLocalName, NULL))) {
        TCHAR msg[256];
        StringCchPrintf(msg, 256, _T("Error getting local name on EndElement, error is %08.8lx"), hr);
        StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
        return hr;
      }
      return S_OK;
      break;

    case XmlNodeType_Text:

    case XmlNodeType_Whitespace:
      if (FAILED(hr = pReader->GetValue(&pwszValue, NULL))) {
        TCHAR msg[256];
        StringCchPrintf(msg, 256, _T("Error getting value in Text, error is %08.8lx"), hr);
        StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
        return hr;
      }
      parsePointValues(pwszValue);
      // wprintf(L"Whitespace text: >%s<\n", pwszValue);
      break;

    case XmlNodeType_CDATA:
      if (FAILED(hr = pReader->GetValue(&pwszValue, NULL))) {
        TCHAR msg[256];
        StringCchPrintf(msg, 256, _T("Error getting value in CDATA, error is %08.8lx"), hr);
        StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
        return hr;
      }
      //wprintf(L"CDATA: %s\n", pwszValue);
      break;

    case XmlNodeType_ProcessingInstruction:
      if (FAILED(hr = pReader->GetLocalName(&pwszLocalName, NULL))) {
        TCHAR msg[256];
        StringCchPrintf(msg, 256, _T("Error getting name on ProcessingInstruction, error is %08.8lx"), hr);
        StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
        return hr;
      }
      if (FAILED(hr = pReader->GetValue(&pwszValue, NULL))) {
        TCHAR msg[256];
        StringCchPrintf(msg, 256, _T("Error getting value on ProcessingInstruction, error is %08.8lx"), hr);
        StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
        return hr;
      }
      // wprintf(L"Processing Instruction name:%S value:%S\n", pwszLocalName, pwszValue);
      break;

    case XmlNodeType_Comment:
      if (FAILED(hr = pReader->GetValue(&pwszValue, NULL))) {
        TCHAR msg[256];
        StringCchPrintf(msg, 256, _T("Error getting value on Comment, error is %08.8lx"), hr);
        StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
        return hr;
      }
      //wprintf(L"Comment: %s\n", pwszValue);
      break;
    } // end of switch
  } // end of while
  return S_OK;
}

HRESULT KShapePolygon::Store(CComPtr<IXmlWriter> pWriter)
{
  HRESULT hr;

  if (FAILED(hr = pWriter->WriteStartElement(NULL, tagPolygon, NULL)))
  {
    TCHAR msg[256];
    StringCchPrintf(msg, 256, _T("Error, Method: WriteStartElement, error is %08.8lx"), hr);
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
    return hr;
  }

  TCHAR val[32];
  /*
  StringCchPrintf(val, 16, _T("%d"), PointNumber);
  if (FAILED(hr = pWriter->WriteAttributeString(NULL, tagNumberOfPoints, NULL, val)))
  {
    wprintf(L"Error, Method: WriteAttributeString, error is %08.8lx", hr);
    return -1;
  }
  */

  if (FAILED(hr = pWriter->WriteAttributeString(NULL, tagClosed, NULL, tagBoolValue[Closed])))
  {
    TCHAR msg[256];
    StringCchPrintf(msg, 256, _T("Error, Method: WriteAttributeString, error is %08.8lx"), hr);
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
    return hr;
  }

  if (PointNumber > 0)
  {
    for (int i = 0; i < PointNumber; i++)
    {
      StringCchPrintf(val, 32, _T("%d,%d,"), Points[i].x, Points[i].y);
      pWriter->WriteString(val);
    }
  }
  hr = StoreFill(Fill, pWriter);
  if (FAILED(hr))
    return hr;

  hr = StoreOutline(Outline, pWriter);
  if (FAILED(hr))
    return hr;

  if (FAILED(hr = pWriter->WriteEndElement()))
  {
    TCHAR msg[256];
    StringCchPrintf(msg, 256, _T("Error, Method: WriteEndElement, error is %08.8lx"), hr);
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
    return hr;
  }

  return S_OK;
}

#endif //USE_XML_STORAGE

BOOL KShapePolygon::LoadFromBufferExt(KMemoryStream& mf)
{
  mf.Read(&Rect, sizeof(RECT));
  mf.Read(&Outline, sizeof(OutlineStyle));
  mf.Read(&Fill, sizeof(DWORD));
  mf.Read(&PointNumber, sizeof(int));

  allocNumber = PointNumber;
  Points = new CPOINT[allocNumber];
  mf.Read(Points, sizeof(CPOINT) * PointNumber);

  Closed = mf.ReadByte();
  return TRUE;
}

BOOL KShapePolygon::StoreToBufferExt(KMemoryStream& mf)
{
  mf.Write(&Rect, sizeof(RECT));
  mf.Write(&Outline, sizeof(OutlineStyle));
  mf.Write(&Fill, sizeof(DWORD));

  mf.Write(&PointNumber, sizeof(int));
  mf.Write(Points, sizeof(CPOINT) * PointNumber);

  mf.WriteByte(Closed);
  return TRUE;
}
