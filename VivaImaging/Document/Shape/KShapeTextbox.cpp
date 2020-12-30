/**
* @file KShapeTextbox.cpp
* @date 2018.05
* @brief KShapeTextbox class file
*/
#include "stdafx.h"
#include "KShapeTextbox.h"
#include "..\KDragAction.h"
#include "..\..\Platform\Graphics.h"

KShapeTextbox::KShapeTextbox()
  : KShapeBase()
{
  Rect.left = Rect.right = Rect.top = Rect.bottom = 0;
  Outline.Color = 0;
  Outline.Width = 1;
  Outline.Style = 0;
  Fill = 0;
  TextColor = 0xFFD0D0D0;
  TextBuffer = NULL;
  FontFaceName[0] = '\0';
  FontPointSize = 0;
  FontAttribute = 0;
}

KShapeTextbox::KShapeTextbox(RECT rect)
{
  Rect = rect;
  Outline.Color = 0;
  Outline.Width = 1;
  Outline.Style = 0;
  Fill = 0;
  TextColor = 0xFFD0D0D0;
  TextBuffer = NULL;
  FontFaceName[0] = '\0';
  FontPointSize = 0;
  FontAttribute = 0;
}

KShapeTextbox::KShapeTextbox(RECT rect, DWORD textcolor, LPCTSTR facename, int pointSize, DWORD attribute)
{
  Rect = rect;
  Outline.Color = 0;
  Outline.Width = 1;
  Outline.Style = 0;
  Fill = 0;
  TextBuffer = NULL;
  TextColor = textcolor;
  if (facename != NULL)
    StringCchCopy(FontFaceName, LF_FACESIZE, facename);
  else
    FontFaceName[0] = '\0';
  FontPointSize = pointSize;
  FontAttribute = attribute;
}

KShapeTextbox::~KShapeTextbox()
{
  if (TextBuffer != NULL)
  {
    delete[] TextBuffer;
    TextBuffer = NULL;
  }

}

BOOL KShapeTextbox::Move(POINT& offset)
{
  Rect.left += offset.x;
  Rect.right += offset.x;
  Rect.top += offset.y;
  Rect.bottom += offset.y;
  return TRUE;
}

EditHandleType KShapeTextbox::HitObjectsHandle(KImageDrawer& drawer, POINT& pt)
{
  return drawer.HitRectangleHandle(Rect, pt);
}

EditHandleType KShapeTextbox::HitOver(KImageDrawer& drawer, POINT& pt)
{
  if (drawer.HitOver(Rect, ShapeType::ShapeRectangle, pt) > EditHandleType::HandleNone)
    //if (IsOver(Rect, pt))
    return EditHandleType::MoveObjectHandle;

  return EditHandleType::HandleNone;
}

BOOL KShapeTextbox::HitOver(POINT& pt)
{
  if ((Rect.left <= pt.x) && (pt.x <= Rect.right) &&
    (Rect.top <= pt.y) && (pt.y <= Rect.bottom))
  {
    return TRUE;
  }
  return FALSE;
}

void KShapeTextbox::GetRectangle(RECT& rect)
{
  memcpy(&rect, &Rect, sizeof(RECT));
}

void KShapeTextbox::GetNormalRectangle(RECT& rect)
{
  memcpy(&rect, &Rect, sizeof(RECT));
}

RECT KShapeTextbox::GetBounds(KImageDrawer* drawer)
{
  RECT rect;
  memcpy(&rect, &Rect, sizeof(RECT));
  NormalizeRect(rect);

  int pen_hw = 1;

  if ((drawer != NULL) && IsSelected())
  {
    int hw = (drawer->GetHandleImageSize() + 1) / 2;
    if (pen_hw < hw)
      pen_hw = hw;
  }

  if (pen_hw > 0)
  {
    rect.left -= pen_hw;
    rect.top -= pen_hw;
    rect.right += pen_hw;
    rect.bottom += pen_hw;
  }
  return rect;
}

void KShapeTextbox::Draw(Graphics& g, KImageDrawer& info, int flag)
{
  RECT r = Rect;
  info.ImageToScreen(r);
  drawShape(g, info, r, flag);
}

void KShapeTextbox::drawShape(Graphics& g, KImageDrawer& info, RECT& r, int flag)
{
  DWORD bgcolor = (flag & DRAW_WHITE_BACKGROUND) ? 0x80FFFFFF : Fill;

  BYTE a = GetAlphaValue(bgcolor);
  if (a > 0)
  {
    SolidBrush brush(bgcolor);
    g.FillRectangle(&brush, r.left, r.top, r.right - r.left, r.bottom - r.top);
  }

  /*
  if (Outline.Style >LineHollow)
  {
    float width = Outline.Width * info.currentZoom;
    Pen pen(Color(Outline.Color), width);
    g.DrawRectangle(&pen, r.left, r.top, r.right - r.left, r.bottom - r.top);
  }
  */
  int len = lstrlen(TextBuffer);
  if (len > 0)
  {
    LOGFONT logfont;
    memset(&logfont, 0, sizeof(LOGFONT));
    StringCchCopy(logfont.lfFaceName, LF_FACESIZE, FontFaceName);
    logfont.lfHeight = PointSizeToCharacterHeight(FontPointSize);
    logfont.lfWeight = (FontAttribute & FONT_BOLD) ? FW_BOLD : FW_NORMAL;
    logfont.lfItalic = !!(FontAttribute & FONT_ITALIC);
    logfont.lfUnderline = !!(FontAttribute & FONT_UNDERLINE);

    logfont.lfHeight = (LONG)(logfont.lfHeight * info.currentZoom);
    HDC hdc = g.GetHDC();
    Gdiplus::Font font(hdc, &logfont);
    Gdiplus::SolidBrush brush(TextColor);
    g.ReleaseHDC(hdc);

    RectF rf((REAL)r.left, (REAL)r.top, (REAL)(r.right - r.left), (REAL)(r.bottom - r.top));
    g.DrawString(TextBuffer, len, &font, rf, NULL, &brush);
  }

  if (IsSelected() && !(flag & DRAW_NO_HANDLE))
  {
    info.DrawBoundingHandle(g, r, 0);
  }
}

BOOL KShapeTextbox::SetRectangle(RECT& rect)
{
  NormalizeRect(rect);
  if ((Rect.right != rect.right) || (Rect.bottom != rect.bottom) || (Rect.top != rect.top) || (Rect.left != rect.left))
  {
    memcpy(&Rect, &rect, sizeof(RECT));
    return TRUE;
  }
  return FALSE;
}

void KShapeTextbox::OnRenderRubber(Graphics& g, KDragAction* drag, KImageDrawer& info, int flag)
{
  RECT r = drag->MakeResizedRect(Rect);
  info.ImageToScreen(r);
  drawShape(g, info, r, flag | DRAW_WHITE_BACKGROUND);
}

void KShapeTextbox::OnDragEnd(KDragAction* drag, POINT& amount)
{
  Rect = drag->MakeResizedRect(Rect, amount);
  NormalizeRect(Rect);
}

void KShapeTextbox::GetDragBounds(RECT& bound, KDragAction* drag)
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

/*
BOOL KShapeTextbox::GetFillColor(DWORD& color)
{
  color = Fill;
  return TRUE;
}

BOOL KShapeTextbox::SetFillColor(DWORD& color)
{
  Fill = color;
  return TRUE;
}
*/
BOOL KShapeTextbox::GetTextColor(DWORD& color)
{
  color = TextColor;
  return TRUE;
}

BOOL KShapeTextbox::SetTextColor(DWORD& color)
{
  TextColor = color;
  return TRUE;
}

BOOL KShapeTextbox::GetFont(LPTSTR faceName, int& pointSize, DWORD& attribute)
{
  StringCchCopy(faceName, LF_FACESIZE, FontFaceName);
  pointSize = FontPointSize;
  attribute = FontAttribute;
  return TRUE;
}

BOOL KShapeTextbox::SetFont(LPCTSTR faceName, int pointSize, DWORD attribute)
{
  StringCchCopy(FontFaceName, LF_FACESIZE, faceName);
  FontPointSize = pointSize;
  FontAttribute = attribute;
  return TRUE;
}

BOOL KShapeTextbox::SetText(LPCTSTR text)
{
  if (TextBuffer != NULL)
  {
    delete[] TextBuffer;
    TextBuffer = NULL;
  }
  if (text != NULL)
  {
    int len = lstrlen(text);
    if (len > 0)
    {
      TextBuffer = new TCHAR[len + 1];
      StringCchCopy(TextBuffer, len + 1, text);
      return TRUE;
    }
  }
  return FALSE;
}

#ifdef USE_XML_STORAGE
HRESULT KShapeTextbox::ReadAttributes(IXmlReader* pReader)
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

HRESULT KShapeTextbox::Read(CComPtr<IXmlReader> pReader)
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
      if (CompareStringNC(pwszLocalName, tagTextColor) == 0)
      {
        hr = ReadFill(TextColor, pReader);
        if (FAILED(hr))
          return hr;
      }
      else if (CompareStringNC(pwszLocalName, tagFontInfo) == 0)
      {
        //hr = ReadFontInfo(Outline, pReader);
        hr = ReadFontInfo(FontFaceName, FontPointSize, FontAttribute, pReader);

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
      if (lstrlen(pwszValue) > 0)
      {
        if (TextBuffer != NULL)
          delete[] TextBuffer;
        TextBuffer = AllocString(pwszValue);
      }
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


HRESULT KShapeTextbox::Store(CComPtr<IXmlWriter> pWriter)
{
  HRESULT hr;

  if (FAILED(hr = pWriter->WriteStartElement(NULL, tagTextbox, NULL)))
  {
    wprintf(L"Error, Method: WriteStartElement, error is %08.8lx", hr);
    return -1;
  }

  TCHAR val[16];
  StringCchPrintf(val, 16, _T("%d"), Rect.left);
  if (FAILED(hr = pWriter->WriteAttributeString(NULL, tagLeft, NULL, val)))
  {
    wprintf(L"Error, Method: WriteAttributeString, error is %08.8lx", hr);
    return -1;
  }
  StringCchPrintf(val, 16, _T("%d"), Rect.right);
  if (FAILED(hr = pWriter->WriteAttributeString(NULL, tagRight, NULL, val)))
  {
    wprintf(L"Error, Method: WriteAttributeString, error is %08.8lx", hr);
    return -1;
  }
  StringCchPrintf(val, 16, _T("%d"), Rect.top);
  if (FAILED(hr = pWriter->WriteAttributeString(NULL, tagTop, NULL, val)))
  {
    wprintf(L"Error, Method: WriteAttributeString, error is %08.8lx", hr);
    return -1;
  }
  StringCchPrintf(val, 16, _T("%d"), Rect.bottom);
  if (FAILED(hr = pWriter->WriteAttributeString(NULL, tagBottom, NULL, val)))
  {
    wprintf(L"Error, Method: WriteAttributeString, error is %08.8lx", hr);
    return -1;
  }

  if ((TextBuffer != NULL) && (lstrlen(TextBuffer) > 0))
  {
    if (FAILED(hr = pWriter->WriteString(TextBuffer)))
    {
      wprintf(L"Error, Method: WriteString, error is %08.8lx", hr);
      return -1;
    }
  }

  StoreTextColor(TextColor, pWriter);
  StoreFontInfo(FontFaceName, FontPointSize, FontAttribute, pWriter);

  if (FAILED(hr = pWriter->WriteEndElement()))
  {
    wprintf(L"Error, Method: WriteEndElement, error is %08.8lx", hr);
    return -1;
  }

  return 1;
}
#endif // USE_XML_STORAGE

BOOL KShapeTextbox::LoadFromBufferExt(KMemoryStream& mf)
{
  mf.Read(&Rect, sizeof(RECT));
  mf.Read(&Outline, sizeof(OutlineStyle));
  mf.Read(&Fill, sizeof(DWORD));

  mf.Read(&TextColor, sizeof(DWORD));
  mf.Read(&FontFaceName, sizeof(TCHAR) * LF_FACESIZE);
  mf.Read(&FontPointSize, sizeof(int));
  mf.Read(&FontAttribute, sizeof(DWORD));

  UINT len;
  mf.Read(&len, sizeof(UINT));
  TextBuffer = new TCHAR[len + 1];
  if (len > 0)
    mf.Read(TextBuffer, sizeof(TCHAR) * len);
  TextBuffer[len] = '\0';

  return TRUE;
}

BOOL KShapeTextbox::StoreToBufferExt(KMemoryStream& mf)
{
  mf.Write(&Rect, sizeof(RECT));
  mf.Write(&Outline, sizeof(OutlineStyle));
  mf.Write(&Fill, sizeof(DWORD));

  mf.Write(&TextColor, sizeof(DWORD));
  mf.Write(&FontFaceName, sizeof(TCHAR) * LF_FACESIZE);
  mf.Write(&FontPointSize, sizeof(int));
  mf.Write(&FontAttribute, sizeof(DWORD));

  UINT len = 0;
  if (TextBuffer != NULL)
    len = lstrlen(TextBuffer);
  mf.Write(&len, sizeof(UINT));
  if (len > 0)
    mf.Write(TextBuffer, sizeof(TCHAR) * len);

  return TRUE;
}
