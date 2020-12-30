/**
* @file KShapeEllipse.cpp
* @date 2018.05
* @brief KShapeEllipse class file
*/
#include "stdafx.h"
#include "KShapeEllipse.h"
#include "KShapeRectangle.h"
#include "..\KDragAction.h"

KShapeEllipse::KShapeEllipse()
  : KShapeBase()
{
  Rect.left = Rect.right = Rect.top = Rect.bottom = 0;
  Outline.Color = 0;
  Outline.Width = 1;
  Outline.Style = 0;
  Fill = 0;
  StartAngle = 0;
  EndAngle = 36000;
  Closed = 1;
}

KShapeEllipse::KShapeEllipse(RECT rect)
{
  Rect = rect;
  Outline.Color = 0;
  Outline.Width = 1;
  Outline.Style = 0;
  Fill = 0;
  StartAngle = 0;
  EndAngle = 36000;
  Closed = 1;
}

KShapeEllipse::KShapeEllipse(RECT rect, DWORD fillcolor, OutlineStyle linestyle)
{
  Rect = rect;
  Outline = linestyle;
  Fill = fillcolor;
  StartAngle = 0;
  EndAngle = 36000;
  Closed = 1;
}

KShapeEllipse::KShapeEllipse(RECT rect, DWORD fillcolor, OutlineStyle linestyle, int start, int end, BYTE closed)
{
  Rect = rect;
  Outline = linestyle;
  Fill = fillcolor;
  StartAngle = start;
  EndAngle = end;
  Closed = closed;
}

KShapeEllipse::~KShapeEllipse()
{

}

BOOL KShapeEllipse::Move(POINT& offset)
{
  Rect.left += offset.x;
  Rect.right += offset.x;
  Rect.top += offset.y;
  Rect.bottom += offset.y;
  return TRUE;
}

EditHandleType KShapeEllipse::HitObjectsHandle(KImageDrawer& drawer, POINT& pt)
{
  return drawer.HitRectangleHandle(Rect, pt);
}

EditHandleType KShapeEllipse::HitOver(KImageDrawer& drawer, POINT& pt)
{
  if (drawer.HitOver(Rect, ShapeType::ShapeEllipse, pt) > EditHandleType::HandleNone)
    return EditHandleType::MoveObjectHandle;

  return EditHandleType::HandleNone;
}

BOOL KShapeEllipse::HitOver(POINT& pt)
{
  int pen_half_width = Outline.Width / 2 + 1;
  if (((Rect.left - pen_half_width) <= pt.x) && (pt.x <= (Rect.right + pen_half_width)) &&
    ((Rect.top - pen_half_width) <= pt.y) && (pt.y <= (Rect.bottom + pen_half_width)))
  {
    float a = (float)(Rect.right - Rect.left) / 2;
    float b = (float)(Rect.bottom - Rect.top) / 2;
    float x = (float)pt.x - (float)(Rect.right + Rect.left) / 2;
    float y = (float)pt.y - (float)(Rect.bottom + Rect.top) / 2;
    float r = x * x / (a * a) + y * y / (b * b);

    return (r <= 1.0);
  }
  return FALSE;
} 

void KShapeEllipse::GetRectangle(RECT& rect)
{
  memcpy(&rect, &Rect, sizeof(RECT));
}

void KShapeEllipse::GetNormalRectangle(RECT& rect)
{
  memcpy(&rect, &Rect, sizeof(RECT));
}

RECT KShapeEllipse::GetBounds(KImageDrawer* drawer)
{
  RECT rect;
  memcpy(&rect, &Rect, sizeof(RECT));
  NormalizeRect(rect);

  int pen_hw = Outline.Width / 2 + 1;
  AddToBounds(rect, drawer, pen_hw);
  return rect;
}

void KShapeEllipse::Draw(Graphics& g, KImageDrawer& info, int flag)
{
  RECT r = Rect;
  info.ImageToScreen(r);
  drawShape(g, info, r, flag);
}

void KShapeEllipse::drawShape(Graphics& g, KImageDrawer& info, RECT& r, int flag)
{
  BYTE a = GetAlphaValue(Fill);
  if (a > 0)
  {
    SolidBrush brush(Fill);
    g.FillEllipse(&brush, r.left, r.top, r.right - r.left, r.bottom - r.top);
  }

  if (Outline.Style > (BYTE)LineStyle::LineHollow)
  {
    float width = Outline.Width * info.currentZoom;
    Pen pen(Color(Outline.Color), width);

    if (Outline.Style == (BYTE)LineStyle::LineDot)
      pen.SetDashStyle(DashStyleDot);
    else if (Outline.Style == (BYTE)LineStyle::LineDash)
      pen.SetDashStyle(DashStyleDash);
    else if (Outline.Style == (BYTE)LineStyle::LineDashDot)
      pen.SetDashStyle(DashStyleDashDot);

    g.DrawEllipse(&pen, r.left, r.top, r.right - r.left, r.bottom - r.top);
  }

  if (IsSelected() && !(flag & DRAW_NO_HANDLE))
  {
    info.DrawBoundingHandle(g, r, 0);
  }
}

BOOL KShapeEllipse::SetRectangle(RECT& rect)
{
  NormalizeRect(rect);
  if ((Rect.right != rect.right) || (Rect.bottom != rect.bottom) || (Rect.top != rect.top) || (Rect.left != rect.left))
  {
    memcpy(&Rect, &rect, sizeof(RECT));
    return TRUE;
  }
  return FALSE;
}

void KShapeEllipse::OnRenderRubber(Graphics& g, KDragAction* drag, KImageDrawer& info, int flag)
{
  RECT r = drag->MakeResizedRect(Rect);
  info.ImageToScreen(r);

  drawShape(g, info, r, flag);
}

void KShapeEllipse::OnDragEnd(KDragAction* drag, POINT& amount)
{
  Rect = drag->MakeResizedRect(Rect, amount);
  NormalizeRect(Rect);
}

void KShapeEllipse::GetDragBounds(RECT& bound, KDragAction* drag)
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

BOOL KShapeEllipse::GetFillColor(DWORD& color)
{
  color = Fill;
  return TRUE;
}

BOOL KShapeEllipse::GetOutlineStyle(OutlineStyle& linestyle)
{
  memcpy(&linestyle, &Outline, sizeof(OutlineStyle));
  return TRUE;
}

BOOL KShapeEllipse::SetFillColor(DWORD& color)
{
  Fill = color;
  return TRUE;
}

BOOL KShapeEllipse::SetOutlineStyle(OutlineStyle& linestyle)
{
  memcpy(&Outline, &linestyle, sizeof(OutlineStyle));
  return TRUE;
}

#ifdef USE_XML_STORAGE
HRESULT KShapeEllipse::ReadAttributes(IXmlReader* pReader)
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

      if (CompareStringNC(pwszLocalName, tagLeft) == 0)
      {
        int val;
        if (swscanf_s(pwszValue, _T("%d"), &val) == 1)
          Rect.left = val;
      }
      else if (CompareStringNC(pwszLocalName, tagTop) == 0)
      {
        int val;
        if (swscanf_s(pwszValue, _T("%d"), &val) == 1)
          Rect.top = val;
      }
      else if (CompareStringNC(pwszLocalName, tagRight) == 0)
      {
        int val;
        if (swscanf_s(pwszValue, _T("%d"), &val) == 1)
          Rect.right = val;
      }
      else if (CompareStringNC(pwszLocalName, tagBottom) == 0)
      {
        int val;
        if (swscanf_s(pwszValue, _T("%d"), &val) == 1)
          Rect.bottom = val;
      }
    }

    hr = pReader->MoveToNextAttribute();
  }
  return hr;
}

HRESULT KShapeEllipse::Read(CComPtr<IXmlReader> pReader)
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

HRESULT KShapeEllipse::Store(CComPtr<IXmlWriter> pWriter)
{
  HRESULT hr;

  if (FAILED(hr = pWriter->WriteStartElement(NULL, tagEllipse, NULL)))
  {
    TCHAR msg[256];
    StringCchPrintf(msg, 256, _T("Error, Method: WriteStartElement, error is %08.8lx"), hr);
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
    return hr;
  }

  StoreRect(Rect, pWriter);

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
#endif // USE_XML_STORAGE

BOOL KShapeEllipse::LoadFromBufferExt(KMemoryStream& mf)
{
  mf.Read(&Rect, sizeof(RECT));
  mf.Read(&Outline, sizeof(OutlineStyle));
  mf.Read(&Fill, sizeof(DWORD));
  mf.Read(&StartAngle, sizeof(int));
  mf.Read(&EndAngle, sizeof(int));
  Closed = mf.ReadByte();
  return TRUE;
}

BOOL KShapeEllipse::StoreToBufferExt(KMemoryStream& mf)
{
  mf.Write(&Rect, sizeof(RECT));
  mf.Write(&Outline, sizeof(OutlineStyle));
  mf.Write(&Fill, sizeof(DWORD));
  mf.Write(&StartAngle, sizeof(int));
  mf.Write(&EndAngle, sizeof(int));
  mf.WriteByte(Closed);
  return TRUE;
}
