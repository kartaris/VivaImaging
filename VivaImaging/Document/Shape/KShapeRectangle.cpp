/**
* @file KShapeRectangle.cpp
* @date 2018.05
* @brief KShapeRectangle class file
*/
#include "stdafx.h"
#include "KShapeRectangle.h"
#include "..\KDragAction.h"

KShapeRectangle::KShapeRectangle()
  : KShapeBase()
{
  Rect.left = Rect.right = Rect.top = Rect.bottom = 0;
  Outline.Color = 0;
  Outline.Width = 1;
  Outline.Style = 0; // hollow line
  Fill = 0;
}

KShapeRectangle::KShapeRectangle(RECT rect)
{
  Rect = rect;
  Outline.Color = 0;
  Outline.Width = 1;
  Outline.Style = 0;
  Fill = 0;
}

KShapeRectangle::KShapeRectangle(RECT rect, DWORD fillcolor, OutlineStyle linestyle)
{
  Rect = rect;
  Outline = linestyle;
  Fill = fillcolor;
}

KShapeRectangle::~KShapeRectangle()
{

}

BOOL KShapeRectangle::Move(POINT& offset)
{
	Rect.left += offset.x;
	Rect.right += offset.x;
	Rect.top += offset.y;
	Rect.bottom += offset.y;
	return TRUE;
}

EditHandleType KShapeRectangle::HitObjectsHandle(KImageDrawer& drawer, POINT& pt)
{
  return drawer.HitRectangleHandle(Rect, pt);
}

EditHandleType KShapeRectangle::HitOver(KImageDrawer& drawer, POINT& pt)
{
  if (drawer.HitOver(Rect, ShapeType::ShapeRectangle, pt) > EditHandleType::HandleNone)
	//if (IsOver(Rect, pt))
		return EditHandleType::MoveObjectHandle;

	return EditHandleType::HandleNone;
}

BOOL KShapeRectangle::HitOver(POINT& pt)
{
  int pen_half_width = Outline.Width / 2 + 1;
  if (((Rect.left - pen_half_width) <= pt.x) && (pt.x <= (Rect.right + pen_half_width)) &&
    ((Rect.top - pen_half_width) <= pt.y) && (pt.y <= (Rect.bottom + pen_half_width)))
  {
    return TRUE;
  }
  return FALSE;
}

void KShapeRectangle::GetRectangle(RECT& rect)
{
  memcpy(&rect, &Rect, sizeof(RECT));
}

void KShapeRectangle::GetNormalRectangle(RECT& rect)
{
  memcpy(&rect, &Rect, sizeof(RECT));
}

RECT KShapeRectangle::GetBounds(KImageDrawer* drawer)
{
  RECT rect;
  memcpy(&rect, &Rect, sizeof(RECT));
  NormalizeRect(rect);

  int pen_hw = Outline.Width / 2 + 1;
  AddToBounds(rect, drawer, pen_hw);
  return rect;
}

void KShapeRectangle::Draw(Graphics& g, KImageDrawer& info, int flag)
{
  RECT r = Rect;
  info.ImageToScreen(r);
  drawShape(g, info, r, flag);
}

void KShapeRectangle::drawShape(Graphics& g, KImageDrawer& info, RECT& r, int flag)
{
#if 0
  RECT r = Rect;
  info.ImageToScreen(r);

  int width = (int)(Outline.Width * info.currentZoom + 0.5);
  if (width <= 0)
    width = 1;

  CPen pen;
  pen.CreatePen(PS_SOLID, width, Outline.Color);
  CPen* old_pen = pDC->SelectObject(&pen);
  CBrush brush;
  CBrush* old_brush = NULL;
  BYTE a = GetAlphaValue(Fill);
  if (a > 0)
  {
    brush.CreateSolidBrush(Fill);
    old_brush = (CBrush*)pDC->SelectObject(brush);
  }
  else
  {
    old_brush = (CBrush*)pDC->SelectStockObject(NULL_BRUSH);
  }

  pDC->Rectangle(&r);

  pDC->SelectObject(old_pen);
  pDC->SelectObject(old_brush);
#else
  BYTE a = GetAlphaValue(Fill);
  if (a > 0)
  {
    SolidBrush brush(Fill);
    g.FillRectangle(&brush, r.left, r.top, r.right - r.left, r.bottom - r.top);
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

    g.DrawRectangle(&pen, r.left, r.top, r.right - r.left, r.bottom - r.top);
  }
#endif

  if (IsSelected() && !(flag & DRAW_NO_HANDLE))
  {
    info.DrawBoundingHandle(g, r, 0);
  }
}

BOOL KShapeRectangle::SetRectangle(RECT& rect)
{
  NormalizeRect(rect);
  if ((Rect.right != rect.right) || (Rect.bottom != rect.bottom) || (Rect.top != rect.top) || (Rect.left != rect.left))
  {
    memcpy(&Rect, &rect, sizeof(RECT));
    return TRUE;
  }
  return FALSE;
}

BOOL KShapeRectangle::IsOver(RECT r, POINT pt)
{
	return ((r.left <= pt.x) && (r.right >= pt.x) && (r.top <= pt.y) && (r.bottom >= pt.y));
}

void KShapeRectangle::OnRenderRubber(Graphics& g, KDragAction* drag, KImageDrawer& info, int flag)
{
  RECT r = drag->MakeResizedRect(Rect);
  info.ImageToScreen(r);
  drawShape(g, info, r, flag);
}

void KShapeRectangle::OnDragEnd(KDragAction* drag, POINT& amount)
{
  Rect = drag->MakeResizedRect(Rect, amount);
  NormalizeRect(Rect);
}

void KShapeRectangle::GetDragBounds(RECT& bound, KDragAction* drag)
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

BOOL KShapeRectangle::GetFillColor(DWORD& color)
{
  color = Fill;
  return TRUE;
}

BOOL KShapeRectangle::GetOutlineStyle(OutlineStyle& linestyle)
{
  memcpy(&linestyle, &Outline, sizeof(OutlineStyle));
  return TRUE;
}

BOOL KShapeRectangle::SetFillColor(DWORD& color)
{
  Fill = color;
  return TRUE;
}

BOOL KShapeRectangle::SetOutlineStyle(OutlineStyle& linestyle)
{
  memcpy(&Outline, &linestyle, sizeof(OutlineStyle));
  return TRUE;
}

#ifdef USE_XML_STORAGE
HRESULT KShapeRectangle::ReadAttributes(IXmlReader* pReader)
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

HRESULT KShapeRectangle::Read(CComPtr<IXmlReader> pReader)
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

HRESULT KShapeRectangle::Store(CComPtr<IXmlWriter> pWriter)
{
  HRESULT hr;

  if (FAILED(hr = pWriter->WriteStartElement(NULL, tagRectangle, NULL)))
  {
    wprintf(L"Error, Method: WriteStartElement, error is %08.8lx", hr);
    return E_FAIL;
  }

  StoreRect(Rect, pWriter);
  StoreFill(Fill, pWriter);
  StoreOutline(Outline, pWriter);

  if (FAILED(hr = pWriter->WriteEndElement()))
  {
    wprintf(L"Error, Method: WriteEndElement, error is %08.8lx", hr);
    return -1;
  }

  return 1;
}
#endif // USE_XML_STORAGE

BOOL KShapeRectangle::LoadFromBufferExt(KMemoryStream& mf)
{
  mf.Read(&Rect, sizeof(RECT));
  mf.Read(&Outline, sizeof(OutlineStyle));
  mf.Read(&Fill, sizeof(DWORD));
  return TRUE;
}

BOOL KShapeRectangle::StoreToBufferExt(KMemoryStream& mf)
{
  mf.Write(&Rect, sizeof(RECT));
  mf.Write(&Outline, sizeof(OutlineStyle));
  mf.Write(&Fill, sizeof(DWORD));
  return TRUE;
}
