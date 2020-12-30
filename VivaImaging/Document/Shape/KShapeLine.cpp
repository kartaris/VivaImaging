/**
* @file KShapeLine.cpp
* @date 2018.05
* @brief KShapeLine class file
*/
#include "stdafx.h"
#include "KShapeLine.h"
#include "KShapeRectangle.h"
#include "mathmatics.h"

#include "..\KDragAction.h"

/**
* @class Line
* @brief Line 개체 클래스
*/
KShapeLine::KShapeLine()
  : KShapeBase()
{
  StartPoint.x = StartPoint.y = 0;
  EndPoint.x = EndPoint.y = 0;
  Outline.Color = 0;
  Outline.Width = 1;
  Outline.Style = 0;
  StartArrow = 0;
  EndArrow = 0;
}

KShapeLine::KShapeLine(POINT sp, POINT ep)
  : KShapeBase()
{
  StartPoint = sp;
  EndPoint = ep;
  Outline.Color = 0;
  Outline.Width = 1;
  Outline.Style = 0;
  StartArrow = 0;
  EndArrow = 0;
}

KShapeLine::KShapeLine(POINT sp, POINT ep, OutlineStyle linestyle, BYTE start, BYTE end)
  : KShapeBase()
{
  StartPoint = sp;
  EndPoint = ep;
  Outline = linestyle;
  StartArrow = start;
  EndArrow = end;
}

KShapeLine::~KShapeLine()
{

}

EditHandleType KShapeLine::HitObjectsHandle(KImageDrawer& drawer, POINT& pt)
{
  return drawer.HitLineHandle(StartPoint, EndPoint, pt);
}

EditHandleType KShapeLine::HitOver(KImageDrawer& drawer, POINT& pt)
{
  int pen_half_width = Outline.Width / 2 + 1;
  return drawer.HitOverLine(StartPoint, EndPoint, pen_half_width, pt);
}

BOOL KShapeLine::HitOver(POINT& pt)
{
  int pen_half_width = Outline.Width / 2 + 1;
  return PointIsonLine(&StartPoint, &EndPoint, &pt, pen_half_width);
}

/**
* @brief 개체의 위치를 지정한 만큼 이동한다.
* @param offset : 이동할 양
* @return bool : 성공적으로 이동되면 true를 리턴한다.
*/
BOOL KShapeLine::Move(POINT& offset)
{
  StartPoint.x += offset.x;
  StartPoint.y += offset.y;
  EndPoint.x += offset.x;
  EndPoint.y += offset.y;
  return TRUE;
}

/**
* @brief 핸들의 이동에 따라 개체의 크기를 변경하는 가상 함수.
* @param type : 핸들 종류
* @return offset : 핸들의 이동량
*/
BOOL KShapeLine::ResizeByObjectHandle(EditHandleType type, POINT offset)
{
  if ((offset.x != 0) || (offset.y != 0))
  {
      if (type == EditHandleType::ResizeLeft)
      {
        StartPoint.x += offset.x;
        StartPoint.y += offset.y;
      }
      else if (type == EditHandleType::ResizeRight)
      {
        EndPoint.x += offset.x;
        EndPoint.y += offset.y;
      }
      return TRUE;
  }
  return FALSE;
}

BOOL KShapeLine::SetEndPoint(POINT& point, BOOL bFinish)
{
  if ((EndPoint.x != point.x) || (EndPoint.y != point.x) || bFinish)
  {
    EndPoint = point;
    return TRUE;
  }
  return FALSE;
}

void KShapeLine::Draw(Graphics& g, KImageDrawer& info, int flag)
{
  POINT sp = StartPoint;
  info.ImageToScreen(sp);
  POINT ep = EndPoint;
  info.ImageToScreen(ep);
  drawShape(g, info, sp, ep, flag);
}

void KShapeLine::drawShape(Graphics& g, KImageDrawer& info, POINT& sp, POINT& ep, int flag)
{
#if 0
  int width = (int)(LineStyle.Width * info.currentZoom + 0.5);
  if (width <= 0)
    width = 1;

  CPen pen;
  pen.CreatePen(PS_SOLID, width, Outline.Color);
  CPen* old_pen = pDC->SelectObject(&pen);

  pDC->MoveTo(sp);
  pDC->LineTo(ep);
  pDC->SelectObject(old_pen);
#else
  float width = Outline.Width * info.currentZoom;
  Pen pen(Color(Outline.Color), width);

  if (Outline.Style == (BYTE)LineStyle::LineDot)
    pen.SetDashStyle(DashStyleDot);
  else if (Outline.Style == (BYTE)LineStyle::LineDash)
    pen.SetDashStyle(DashStyleDash);
  else if (Outline.Style == (BYTE)LineStyle::LineDashDot)
    pen.SetDashStyle(DashStyleDashDot);

  g.DrawLine(&pen, Point(sp.x, sp.y), Point(ep.x, ep.y));
#endif

  if (IsSelected() && !(flag & DRAW_NO_HANDLE))
  {
    info.DrawBoundingHandle(g, sp, ep, 0);
  }

}

void KShapeLine::GetRectangle(RECT& rect)
{
  rect.left = StartPoint.x;
  rect.top = StartPoint.y;
  rect.bottom = EndPoint.y;
  rect.right = EndPoint.x;
}

void KShapeLine::GetNormalRectangle(RECT& rect)
{
  if (StartPoint.x <= EndPoint.x)
  {
    rect.left = StartPoint.x;
    rect.right = EndPoint.x;
  }
  else
  {
    rect.left = EndPoint.x;
    rect.right = StartPoint.x;
  }
  if (StartPoint.y <= EndPoint.y)
  {
    rect.top = StartPoint.y;
    rect.bottom = EndPoint.y;
  }
  else
  {
    rect.top = EndPoint.y;
    rect.bottom = StartPoint.y;
  }
}

RECT KShapeLine::GetBounds(KImageDrawer* drawer)
{
  RECT rect;
  GetNormalRectangle(rect);

  int pen_hw = Outline.Width / 2 + 1;
  AddToBounds(rect, drawer, pen_hw);
  return rect;
}

void KShapeLine::OnRenderRubber(Graphics& g, KDragAction* drag, KImageDrawer& info, int flag)
{
  POINT sp = StartPoint;
  POINT ep = EndPoint;
  drag->MakeResizedRect(sp, ep);

  info.ImageToScreen(sp);
  info.ImageToScreen(ep);
  drawShape(g, info, sp, ep, flag);
}

void KShapeLine::OnDragEnd(KDragAction* drag, POINT& amount)
{
  POINT sp = StartPoint;
  POINT ep = EndPoint;
  drag->MakeResizedRect(sp, ep, amount);
  StartPoint = sp;
  EndPoint = ep;
}

void KShapeLine::GetDragBounds(RECT& bound, KDragAction* drag)
{
  POINT s = StartPoint;
  POINT e = EndPoint;
  drag->MakeResizedRect(s, e);

  if (s.x <= e.x)
  {
    bound.left = s.x;
    bound.right = e.x;
  }
  else
  {
    bound.left = e.x;
    bound.right = s.x;
  }
  if (s.y <= e.y)
  {
    bound.top = s.y;
    bound.bottom = e.y;
  }
  else
  {
    bound.top = e.y;
    bound.bottom = s.y;
  }

  int pen_hw = Outline.Width / 2 + 1;
  if (pen_hw > 0)
  {
    bound.left -= pen_hw;
    bound.top -= pen_hw;
    bound.right += pen_hw;
    bound.bottom += pen_hw;
  }
}

BOOL KShapeLine::GetOutlineStyle(OutlineStyle& linestyle)
{
  memcpy(&linestyle, &Outline, sizeof(OutlineStyle));
  return TRUE;
}

BOOL KShapeLine::SetOutlineStyle(OutlineStyle& linestyle)
{
  memcpy(&Outline, &linestyle, sizeof(OutlineStyle));
  return TRUE;
}

#ifdef USE_XML_STORAGE
HRESULT KShapeLine::ReadAttributes(IXmlReader* pReader)
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

      if (CompareStringNC(pwszLocalName, tagStartX) == 0)
      {
        int val;
        if (swscanf_s(pwszValue, _T("%d"), &val) == 1)
          StartPoint.x = val;
      }
      else if (CompareStringNC(pwszLocalName, tagStartY) == 0)
      {
        int val;
        if (swscanf_s(pwszValue, _T("%d"), &val) == 1)
          StartPoint.y = val;
      }
      else if (CompareStringNC(pwszLocalName, tagEndX) == 0)
      {
        int val;
        if (swscanf_s(pwszValue, _T("%d"), &val) == 1)
          EndPoint.x = val;
      }
      else if (CompareStringNC(pwszLocalName, tagEndY) == 0)
      {
        int val;
        if (swscanf_s(pwszValue, _T("%d"), &val) == 1)
          EndPoint.y = val;
      }
    }

    hr = pReader->MoveToNextAttribute();
  }
  return hr;
}

HRESULT KShapeLine::Read(CComPtr<IXmlReader> pReader)
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
      if (CompareStringNC(pwszLocalName, tagOutline) == 0)
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

HRESULT KShapeLine::Store(CComPtr<IXmlWriter> pWriter)
{
  HRESULT hr;

  if (FAILED(hr = pWriter->WriteStartElement(NULL, tagLine, NULL)))
  {
    wprintf(L"Error, Method: WriteStartElement, error is %08.8lx", hr);
    return hr;
  }

  TCHAR val[16];
  StringCchPrintf(val, 16, _T("%d"), StartPoint.x);
  if (FAILED(hr = pWriter->WriteAttributeString(NULL, tagStartX, NULL, val)))
  {
    wprintf(L"Error, Method: WriteAttributeString, error is %08.8lx", hr);
    return hr;
  }
  StringCchPrintf(val, 16, _T("%d"), StartPoint.y);
  if (FAILED(hr = pWriter->WriteAttributeString(NULL, tagStartY, NULL, val)))
  {
    wprintf(L"Error, Method: WriteAttributeString, error is %08.8lx", hr);
    return hr;
  }
  StringCchPrintf(val, 16, _T("%d"), EndPoint.x);
  if (FAILED(hr = pWriter->WriteAttributeString(NULL, tagEndX, NULL, val)))
  {
    wprintf(L"Error, Method: WriteAttributeString, error is %08.8lx", hr);
    return hr;
  }
  StringCchPrintf(val, 16, _T("%d"), EndPoint.y);
  if (FAILED(hr = pWriter->WriteAttributeString(NULL, tagEndY, NULL, val)))
  {
    wprintf(L"Error, Method: WriteAttributeString, error is %08.8lx", hr);
    return hr;
  }

  hr = StoreOutline(Outline, pWriter);
  if (FAILED(hr))
    return hr;

  if (FAILED(hr = pWriter->WriteEndElement()))
  {
    wprintf(L"Error, Method: WriteEndElement, error is %08.8lx", hr);
    return hr;
  }

  return S_OK;
}
#endif // USE_XML_STORAGE

BOOL KShapeLine::LoadFromBufferExt(KMemoryStream& mf)
{
  mf.Read(&StartPoint, sizeof(POINT));
  mf.Read(&EndPoint, sizeof(POINT));
  mf.Read(&Outline, sizeof(OutlineStyle));
  StartArrow = mf.ReadByte();
  EndArrow = mf.ReadByte();
  return TRUE;
}

BOOL KShapeLine::StoreToBufferExt(KMemoryStream& mf)
{
  mf.Write(&StartPoint, sizeof(POINT));
  mf.Write(&EndPoint, sizeof(POINT));
  mf.Write(&Outline, sizeof(OutlineStyle));
  mf.WriteByte(StartArrow);
  mf.WriteByte(EndArrow);
  return TRUE;
}
