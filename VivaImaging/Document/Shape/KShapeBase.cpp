/**
* @file KShapeBase.cpp
* @date 2018.05
* @brief KShapeBase class file
*/

#include "stdafx.h"
#include "KShapeBase.h"
#include "KShapeLine.h"
#include "KShapeRectangle.h"
#include "KShapeEllipse.h"
#include "KShapePolygon.h"
#include "KShapeTextbox.h"
#include "KShapeSmart.h"

UINT KShapeBase::gObjectID = 1;

TCHAR vivaXmlDocName[] = _T("VivaImagingDocument");

TCHAR tagDocInformation[] = _T("DocInformation");
TCHAR tagCreator[] = _T("Creator");
TCHAR tagCreatorOID[] = _T("CreatorOID");
TCHAR tagCreatedDate[] = _T("CreatedDate");
TCHAR tagLastModifier[] = _T("LastModifier");
TCHAR tagLastModifierOID[] = _T("LastModifierOID");
TCHAR tagLastModifiedDate[] = _T("LastModifiedDate");

TCHAR tagFolder[] = _T("Folder");
TCHAR tagImage[] = _T("Image");
TCHAR tagName[] = _T("Name");
TCHAR tagFileName[] = _T("FileName");
TCHAR tagEcmInfo[] = _T("EcmInfo");

TCHAR tagExtDocType[] = _T("ExtDocType");
TCHAR tagExtDocTypeOid[] = _T("Oid");
TCHAR tagExtDocTypeName[] = _T("Name");

TCHAR tagExtDocTypeColumn[] = _T("Column");
TCHAR tagExtDocTypeColumnKey[] = _T("Key");
TCHAR tagExtDocTypeColumnType[] = _T("Type");
TCHAR tagExtDocTypeColumnName[] = _T("Name");
TCHAR tagExtDocTypeColumnMaxLength[] = _T("MaxLength");
TCHAR tagExtDocTypeColumnValue[] = _T("Value");

TCHAR tagMetaData[] = _T("MetaData");
TCHAR tagKey[] = _T("Key");

TCHAR tagAnnotates[] = _T("Annotates");

TCHAR tagEllipse[] = _T("Ellipse");
TCHAR tagRectangle[] = _T("Rectangle");
TCHAR tagSmartShape[] = _T("SmartShape");

TCHAR tagLeft[] = _T("Left");
TCHAR tagRight[] = _T("Right");
TCHAR tagTop[] = _T("Top");
TCHAR tagBottom[] = _T("Bottom");

TCHAR tagFill[] = _T("Fill");
TCHAR tagOutline[] = _T("Outline");
TCHAR tagColor[] = _T("Color");
TCHAR tagWidth[] = _T("Width");
TCHAR tagStyle[] = _T("Style");

TCHAR tagLine[] = _T("Line");
TCHAR tagStartX[] = _T("StartX");
TCHAR tagStartY[] = _T("StartY");
TCHAR tagEndX[] = _T("EndX");
TCHAR tagEndY[] = _T("EndY");

TCHAR tagPolygon[] = _T("Polygon");
TCHAR tagNumberOfPoints[] = _T("NumberOfPoints");
TCHAR tagClosed[] = _T("Closed");

TCHAR tagTextbox[] = _T("Textbox");
TCHAR tagTextColor[] = _T("TextColor");
TCHAR tagFontInfo[] = _T("Font");
TCHAR tagFaceName[] = _T("Facename");
TCHAR tagPointSize[] = _T("PointSize");
TCHAR tagFontBold[] = _T("Bold");
TCHAR tagFontItalic[] = _T("Italic");
TCHAR tagFontUnderline[] = _T("Underline");

LPTSTR tagBoolValue[] = {
  _T("False"),
  _T("True")
};

KShapeBase::KShapeBase()
{
  childIndex = 0;
  ObjectFlag = KSHAPE_VISIBLE;
  ObjectId = gObjectID++;
}

KShapeBase::KShapeBase(KShapeBase* p)
{
  childIndex = 0;
  ObjectFlag = KSHAPE_VISIBLE;
  ObjectId = gObjectID++;
}

KShapeBase::~KShapeBase()
{
}

void KShapeBase::SetShapeFlag(BOOL bSet, int flag)
{
  if (bSet)
    ObjectFlag |= flag;
  else
    ObjectFlag &= ~flag;
}

RECT KShapeBase::GetBounds(KImageDrawer* drawer)
{
  RECT rect;
  memset(&rect, 0, sizeof(RECT));
  return rect;
}

RECT KShapeBase::GetBounds(RECT& rect, KImageDrawer* drawer)
{
  return rect;
}

void KShapeBase::AddToBounds(RECT& rect, KImageDrawer* drawer, int penHalfWidth)
{
  if ((drawer != NULL) && IsSelected())
  {
    int hw = (drawer->GetHandleImageSize() + 1) / 2;
    if (penHalfWidth < hw)
      penHalfWidth = hw;
  }

  if (penHalfWidth > 0)
  {
    rect.left -= penHalfWidth;
    rect.top -= penHalfWidth;
    rect.right += penHalfWidth;
    rect.bottom += penHalfWidth;
  }
}

int KShapeBase::GetPenWidth()
{
  OutlineStyle linestyle;
  if (GetOutlineStyle(linestyle))
    return linestyle.Width;
  return 0;
}

BOOL KShapeBase::HitTest(KImageDrawer& info)
{
  RECT r = GetBounds(&info);
  return !((r.right < info.PaintRect.left) && (info.PaintRect.right < r.left) &&
    (r.bottom < info.PaintRect.top) && (info.PaintRect.bottom < r.top));
}

KShapeBase* KShapeBase::createShape(ShapeType shape_type)
{
  KShapeBase* s = NULL;

  if (shape_type == ShapeType::ShapeLine)
    s = new KShapeLine();
  else if (shape_type == ShapeType::ShapeRectangle)
    s = new KShapeRectangle();
  else if (shape_type == ShapeType::ShapeEllipse)
    s = new KShapeEllipse();
#if 0
  else if (shape_type == ShapeTriangle)
    s = new KShapeTriangle();
  else if (shape_type == ShapeRhombus)
    s = new KShapeRhombus();
  else if (shape_type == ShapePentagon)
    s = new KShapePentagon();
  else if (shape_type == ShapeStar)
    s = new KShapeStar();
#endif
  else if (shape_type == ShapeType::ShapePolygon)
    s = new KShapePolygon();
  else if (shape_type == ShapeType::ShapeTextbox)
    s = new KShapeTextbox();
  else
  {
    s = KShapeSmartBaseObject::CreateShapeObject(shape_type);
    /*
    if (KShapeSmartLineObject::IsTypeOf(shape_type)) // shape_type == ShapeType::ShapeSmartLine)
      s = new KShapeSmartLineObject();
    else if (shape_type == ShapeType::ShapeSmartRect)
      s = new KShapeSmartRectObject();
    else if (shape_type == ShapeType::ShapeSmartGroup)
      s = new KShapeSmartGroupObject();
    else if (shape_type == ShapeType::ShapeSmartCallout)
      s = new KShapeSmartCalloutObject();
    */
  }
  return s;
}

BOOL calcLineCrossPoint(Point* p1, Point* p2, Point* s1, Point* s2, Point* dp)
{
  double		a1, b1;
  double		a2, b2;
  double		dX, dY;

  if ((p2->X == p1->X) || (s2->X == s1->X))
  {
    if ((p2->X == p1->X) && (s2->X == s1->X))	// vertical line
      return(FALSE);
    if (p2->X == p1->X)
    {
      a2 = (double)(s2->Y - s1->Y) / (s2->X - s1->X);
      b2 = (s1->Y - a2 * s1->X);
      dY = (double)a2 * p2->X + b2;
      dp->X = p2->X;
      dp->Y = (int)(dY + 0.5);
      return(TRUE);
    }
    if (s2->X == s1->X)
    {
      a1 = (double)(p2->Y - p1->Y) / (p2->X - p1->X);
      b1 = (p1->Y - a1 * p1->X);
      dY = (double)a1 * s2->X + b1;
      dp->X = s2->X;
      dp->Y = (int)(dY + 0.5);
      return(TRUE);
    }
  }

  a1 = (double)(p2->Y - p1->Y) / (p2->X - p1->X);
  b1 = (p1->Y - a1 * p1->X);
  a2 = (double)(s2->Y - s1->Y) / (s2->X - s1->X);
  b2 = (s1->Y - a2 * s1->X);

  if (a1 == a2)
    return(FALSE);
  dX = (double)(b1 - b2) / (a2 - a1);
  dY = (double)a1 * dX + b1;
  dp->X = (int)(dX + 0.5);
  dp->Y = (int)(dY + 0.5);

  // is there horizontal line ?
  if (p2->Y == p1->Y)
    dp->Y = p2->Y;
  else if (s2->Y == s1->Y)
    dp->Y = s2->Y;
  return(TRUE);
}

int KShapeBase::ReadyShapePolygon(ShapeType type, Point* p, int count, RECT& r)
{
  if (type == ShapeType::ShapeTriangle)
  {
    p[0].X = (r.left + r.right) / 2;
    p[0].Y = r.top;
    p[1].X = r.left;
    p[1].Y = r.bottom;
    p[2].X = r.right;
    p[2].Y = r.bottom;
    return 3;
  }
  else if (type == ShapeType::ShapeRhombus)
  {
    p[0].X = (r.left + r.right) / 2;
    p[0].Y = r.top;
    p[1].X = r.left;
    p[1].Y = (r.top + r.bottom) / 2;
    p[2].X = (r.left + r.right) / 2;
    p[2].Y = r.bottom;
    p[3].X = r.right;
    p[3].Y = (r.top + r.bottom) / 2;
    return 4;
  }
  else if (type == ShapeType::ShapePentagon)
  {
    int xr = (int)((r.right - r.left) * 0.194 + 0.5);
    int yr = (int)((r.bottom - r.top) * 0.384 + 0.5);
    p[0].X = r.right - xr;
    p[0].Y = r.bottom;
    p[1].X = r.right;
    p[1].Y = r.top + yr;
    p[2].X = (r.left + r.right) / 2;
    p[2].Y = r.top;
    p[3].X = r.left;
    p[3].Y = r.top + yr;
    p[4].X = r.left + xr;
    p[4].Y = r.bottom;
    return 5;
  }
  else if (type == ShapeType::ShapeStar)
  {
    int xr = (int)((r.right - r.left) * 0.194 + 0.5);
    int yr = (int)((r.bottom - r.top) * 0.384 + 0.5);
    p[0].X = r.right - xr;
    p[0].Y = r.bottom;
    p[2].X = r.right;
    p[2].Y = r.top + yr;
    p[4].X = (r.left + r.right) / 2;
    p[4].Y = r.top;
    p[6].X = r.left;
    p[6].Y = r.top + yr;
    p[8].X = r.left + xr;
    p[8].Y = r.bottom;

    calcLineCrossPoint(&p[2], &p[8], &p[0], &p[4], &p[1]);
    calcLineCrossPoint(&p[2], &p[6], &p[0], &p[4], &p[3]);
    calcLineCrossPoint(&p[2], &p[6], &p[4], &p[8], &p[5]);
    calcLineCrossPoint(&p[0], &p[6], &p[4], &p[8], &p[7]);
    calcLineCrossPoint(&p[0], &p[6], &p[2], &p[8], &p[9]);
    return 10;
  }
  return 0;
}

#ifdef USE_XML_STORAGE
HRESULT KShapeBase::ReadFill(DWORD& Fill, CComPtr<IXmlReader> pReader)
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
      if (FAILED(hr = pReader->GetPrefix(&pwszPrefix, &cwchPrefix)))
      {
        TCHAR msg[256];
        StringCchPrintf(msg, 256, _T("Error getting prefix, error is %08.8lx"), hr);
        return hr;
      }

      if (FAILED(hr = pReader->GetLocalName(&pwszLocalName, NULL)))
      {
        TCHAR msg[256];
        StringCchPrintf(msg, 256, _T("Error getting local name, error is %08.8lx"), hr);
        return hr;
      }

      if (FAILED(hr = pReader->GetValue(&pwszValue, NULL)))
      {
        TCHAR msg[256];
        StringCchPrintf(msg, 256, _T("Error getting value, error is %08.8lx"), hr);
        return hr;
      }
      /*
      if (cwchPrefix > 0)
        wprintf(L"Attr: %s:%s=\"%s\" \n", pwszPrefix, pwszLocalName, pwszValue);
      else
        wprintf(L"Attr: %s=\"%s\" \n", pwszLocalName, pwszValue);
      */
      if ((pwszLocalName != NULL) && (pwszValue != NULL))
      {
        if (CompareStringNC(pwszLocalName, tagColor) == 0)
        {
          DWORD val;
          if (swscanf_s(pwszValue, _T("0x%X"), &val) == 1)
            Fill = val;
        }
      }
    }

    hr = pReader->MoveToNextAttribute();
  }
  return hr;
}

HRESULT KShapeBase::ReadOutline(OutlineStyle& outline, CComPtr<IXmlReader> pReader)
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
      if (FAILED(hr = pReader->GetPrefix(&pwszPrefix, &cwchPrefix)))
      {
        TCHAR msg[256];
        StringCchPrintf(msg, 256, _T("Error getting prefix, error is %08.8lx"), hr);
        return hr;
      }

      if (FAILED(hr = pReader->GetLocalName(&pwszLocalName, NULL)))
      {
        TCHAR msg[256];
        StringCchPrintf(msg, 256, _T("Error getting local name, error is %08.8lx"), hr);
        return hr;
      }

      if (FAILED(hr = pReader->GetValue(&pwszValue, NULL)))
      {
        TCHAR msg[256];
        StringCchPrintf(msg, 256, _T("Error getting value, error is %08.8lx"), hr);
        return hr;
      }
      /*
      if (cwchPrefix > 0)
        wprintf(L"Attr: %s:%s=\"%s\" \n", pwszPrefix, pwszLocalName, pwszValue);
      else
        wprintf(L"Attr: %s=\"%s\" \n", pwszLocalName, pwszValue);
      */
      if ((pwszLocalName != NULL) && (pwszValue != NULL))
      {
        if (CompareStringNC(pwszLocalName, tagColor) == 0)
        {
          DWORD val;
          if (swscanf_s(pwszValue, _T("0x%X"), &val) == 1)
            outline.Color = val;
        }
        else if (CompareStringNC(pwszLocalName, tagWidth) == 0)
        {
          DWORD val;
          if (swscanf_s(pwszValue, _T("%d"), &val) == 1)
            outline.Width = (BYTE)val;
        }
        else if (CompareStringNC(pwszLocalName, tagStyle) == 0)
        {
          DWORD val;
          if (swscanf_s(pwszValue, _T("%d"), &val) == 1)
            outline.Style = (BYTE)val;
        }
      }
    }

    hr = pReader->MoveToNextAttribute();
  }
  return hr;
}

HRESULT KShapeBase::ReadFontInfo(LPTSTR faceName, int& pointSize, DWORD& fontAttribute, CComPtr<IXmlReader> pReader)
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
      faceName[0] = '\0';
      fontAttribute = 0;
      pointSize = 0;

      if (FAILED(hr = pReader->GetPrefix(&pwszPrefix, &cwchPrefix))) {
        TCHAR msg[256];
        StringCchPrintf(msg, 256, _T("Error getting prefix, error is %08.8lx"), hr);
        return hr;
      }

      if (FAILED(hr = pReader->GetLocalName(&pwszLocalName, NULL))) {
        TCHAR msg[256];
        StringCchPrintf(msg, 256, _T("Error getting local name, error is %08.8lx"), hr);
        return hr;
      }

      if (FAILED(hr = pReader->GetValue(&pwszValue, NULL))) {
        TCHAR msg[256];
        StringCchPrintf(msg, 256, _T("Error getting value, error is %08.8lx"), hr);
        return hr;
      }
      /*
      if (cwchPrefix > 0)
        wprintf(L"Attr: %s:%s=\"%s\" \n", pwszPrefix, pwszLocalName, pwszValue);
      else
        wprintf(L"Attr: %s=\"%s\" \n", pwszLocalName, pwszValue);
      */
      if (CompareStringNC(pwszLocalName, tagFaceName) == 0)
      {
        StringCchCopy(faceName, LF_FACESIZE, pwszValue);
      }
      else if (CompareStringNC(pwszLocalName, tagPointSize) == 0)
      {
        int val;
        if (swscanf_s(pwszValue, _T("%d"), &val) == 1)
          pointSize = val;
      }
      else if (CompareStringNC(pwszLocalName, tagFontBold) == 0)
      {
        if (CompareStringNC(pwszValue, tagBoolValue[1]) == 0)
          fontAttribute |= FONT_BOLD;
      }
      else if (CompareStringNC(pwszLocalName, tagFontItalic) == 0)
      {
        if (CompareStringNC(pwszValue, tagBoolValue[1]) == 0)
          fontAttribute |= FONT_ITALIC;
      }
      else if (CompareStringNC(pwszLocalName, tagFontUnderline) == 0)
      {
        if (CompareStringNC(pwszValue, tagBoolValue[1]) == 0)
          fontAttribute |= FONT_UNDERLINE;
      }
    }

    hr = pReader->MoveToNextAttribute();
  }
  return hr;
}


HRESULT KShapeBase::StoreStartEndPoint(POINT& sp, POINT& ep, CComPtr<IXmlWriter> pWriter)
{
  HRESULT hr;
  TCHAR val[16];
  StringCchPrintf(val, 16, _T("%d"), sp.x);

  if (FAILED(hr = pWriter->WriteAttributeString(NULL, tagStartX, NULL, val)))
  {
    wprintf(L"Error, Method: WriteAttributeString, error is %08.8lx", hr);
    return -1;
  }
  StringCchPrintf(val, 16, _T("%d"), sp.y);
  if (FAILED(hr = pWriter->WriteAttributeString(NULL, tagStartY, NULL, val)))
  {
    wprintf(L"Error, Method: WriteAttributeString, error is %08.8lx", hr);
    return -1;
  }
  StringCchPrintf(val, 16, _T("%d"), ep.x);
  if (FAILED(hr = pWriter->WriteAttributeString(NULL, tagEndX, NULL, val)))
  {
    wprintf(L"Error, Method: WriteAttributeString, error is %08.8lx", hr);
    return -1;
  }
  StringCchPrintf(val, 16, _T("%d"), ep.y);
  if (FAILED(hr = pWriter->WriteAttributeString(NULL, tagEndY, NULL, val)))
  {
    wprintf(L"Error, Method: WriteAttributeString, error is %08.8lx", hr);
    return -1;
  }
  return S_OK;
}

HRESULT KShapeBase::StoreRect(RECT& rect, CComPtr<IXmlWriter> pWriter)
{
  HRESULT hr;
  TCHAR val[16];
  StringCchPrintf(val, 16, _T("%d"), rect.left);

  if (FAILED(hr = pWriter->WriteAttributeString(NULL, tagLeft, NULL, val)))
  {
    wprintf(L"Error, Method: WriteAttributeString, error is %08.8lx", hr);
    return -1;
  }
  StringCchPrintf(val, 16, _T("%d"), rect.right);
  if (FAILED(hr = pWriter->WriteAttributeString(NULL, tagRight, NULL, val)))
  {
    wprintf(L"Error, Method: WriteAttributeString, error is %08.8lx", hr);
    return -1;
  }
  StringCchPrintf(val, 16, _T("%d"), rect.top);
  if (FAILED(hr = pWriter->WriteAttributeString(NULL, tagTop, NULL, val)))
  {
    wprintf(L"Error, Method: WriteAttributeString, error is %08.8lx", hr);
    return -1;
  }
  StringCchPrintf(val, 16, _T("%d"), rect.bottom);
  if (FAILED(hr = pWriter->WriteAttributeString(NULL, tagBottom, NULL, val)))
  {
    wprintf(L"Error, Method: WriteAttributeString, error is %08.8lx", hr);
    return -1;
  }
  return S_OK;
}

HRESULT KShapeBase::StoreFill(DWORD fillColor, CComPtr<IXmlWriter> pWriter)
{
  HRESULT hr;

  if (FAILED(hr = pWriter->WriteStartElement(NULL, tagFill, NULL)))
  {
    TCHAR msg[256];
    StringCchPrintf(msg, 256, _T("Error, Method: WriteStartElement, error is %08.8lx"), hr);
    return hr;
  }

  TCHAR val[16];
  StringCchPrintf(val, 16, _T("0x%X"), fillColor);
  if (FAILED(hr = pWriter->WriteAttributeString(NULL, tagColor, NULL, val)))
  {
    TCHAR msg[256];
    StringCchPrintf(msg, 256, _T("Error, Method: WriteAttributeString, error is %08.8lx"), hr);
    return hr;
  }

  if (FAILED(hr = pWriter->WriteEndElement()))
  {
    TCHAR msg[256];
    StringCchPrintf(msg, 256, _T("Error, Method: WriteEndElement, error is %08.8lx"), hr);
    return hr;
  }

  return S_OK;
}

HRESULT KShapeBase::StoreOutline(OutlineStyle& outline, CComPtr<IXmlWriter> pWriter)
{
  HRESULT hr;

  if (FAILED(hr = pWriter->WriteStartElement(NULL, tagOutline, NULL)))
  {
    TCHAR msg[256];
    StringCchPrintf(msg, 256, _T("Error, Method: WriteStartElement, error is %08.8lx"), hr);
    return hr;
  }

  TCHAR val[16];
  StringCchPrintf(val, 16, _T("0x%X"), outline.Color);
  if (FAILED(hr = pWriter->WriteAttributeString(NULL, tagColor, NULL, val)))
  {
    TCHAR msg[256];
    StringCchPrintf(msg, 256, _T("Error, Method: WriteAttributeString, error is %08.8lx"), hr);
    return hr;
  }
  StringCchPrintf(val, 16, _T("%d"), outline.Width);
  if (FAILED(hr = pWriter->WriteAttributeString(NULL, tagWidth, NULL, val)))
  {
    TCHAR msg[256];
    StringCchPrintf(msg, 256, _T("Error, Method: WriteAttributeString, error is %08.8lx"), hr);
    return hr;
  }
  StringCchPrintf(val, 16, _T("%d"), outline.Style);
  if (FAILED(hr = pWriter->WriteAttributeString(NULL, tagStyle, NULL, val)))
  {
    TCHAR msg[256];
    StringCchPrintf(msg, 256, _T("Error, Method: WriteAttributeString, error is %08.8lx"), hr);
    return -1;
  }

  if (FAILED(hr = pWriter->WriteEndElement()))
  {
    TCHAR msg[256];
    StringCchPrintf(msg, 256, _T("Error, Method: WriteEndElement, error is %08.8lx"), hr);
    return hr;
  }

  return S_OK;
}

HRESULT KShapeBase::StoreTextColor(DWORD color, CComPtr<IXmlWriter> pWriter)
{
  HRESULT hr;

  if (FAILED(hr = pWriter->WriteStartElement(NULL, tagTextColor, NULL)))
  {
    TCHAR msg[256];
    StringCchPrintf(msg, 256, _T("Error, Method: WriteStartElement, error is %08.8lx"), hr);
    return hr;
  }

  TCHAR val[16];
  StringCchPrintf(val, 16, _T("0x%X"), color);
  if (FAILED(hr = pWriter->WriteAttributeString(NULL, tagColor, NULL, val)))
  {
    TCHAR msg[256];
    StringCchPrintf(msg, 256, _T("Error, Method: WriteAttributeString, error is %08.8lx"), hr);
    return hr;
  }

  if (FAILED(hr = pWriter->WriteEndElement()))
  {
    TCHAR msg[256];
    StringCchPrintf(msg, 256, _T("Error, Method: WriteEndElement, error is %08.8lx"), hr);
    return hr;
  }

  return S_OK;
}

HRESULT KShapeBase::StoreFontInfo(LPCTSTR faceName, int pointSize, int fontAttribute, CComPtr<IXmlWriter> pWriter)
{
  HRESULT hr;

  if (FAILED(hr = pWriter->WriteStartElement(NULL, tagFontInfo, NULL)))
  {
    TCHAR msg[256];
    StringCchPrintf(msg, 256, _T("Error, Method: WriteStartElement, error is %08.8lx"), hr);
    return hr;
  }

  if (faceName != NULL)
  {
    if (FAILED(hr = pWriter->WriteAttributeString(NULL, tagFaceName, NULL, faceName)))
    {
      TCHAR msg[256];
      StringCchPrintf(msg, 256, _T("Error, Method: WriteAttributeString, error is %08.8lx"), hr);
      return hr;
    }
  }

  TCHAR val[16];
  StringCchPrintf(val, 16, _T("%d"), pointSize);
  if (FAILED(hr = pWriter->WriteAttributeString(NULL, tagPointSize, NULL, val)))
  {
    TCHAR msg[256];
    StringCchPrintf(msg, 256, _T("Error, Method: WriteAttributeString, error is %08.8lx"), hr);
    return hr;
  }
  if (fontAttribute & FONT_BOLD)
  {
    if (FAILED(hr = pWriter->WriteAttributeString(NULL, tagFontBold, NULL, tagBoolValue[1])))
    {
      TCHAR msg[256];
      StringCchPrintf(msg, 256, _T("Error, Method: WriteAttributeString, error is %08.8lx"), hr);
      return hr;
    }
  }
  if (fontAttribute & FONT_ITALIC)
  {
    if (FAILED(hr = pWriter->WriteAttributeString(NULL, tagFontItalic, NULL, tagBoolValue[1])))
    {
      TCHAR msg[256];
      StringCchPrintf(msg, 256, _T("Error, Method: WriteAttributeString, error is %08.8lx"), hr);
      return hr;
    }
  }
  if (fontAttribute & FONT_UNDERLINE)
  {
    if (FAILED(hr = pWriter->WriteAttributeString(NULL, tagFontUnderline, NULL, tagBoolValue[1])))
    {
      TCHAR msg[256];
      StringCchPrintf(msg, 256, _T("Error, Method: WriteAttributeString, error is %08.8lx"), hr);
      return hr;
    }
  }

  if (FAILED(hr = pWriter->WriteEndElement()))
  {
    TCHAR msg[256];
    StringCchPrintf(msg, 256, _T("Error, Method: WriteEndElement, error is %08.8lx"), hr);
    return hr;
  }

  return S_OK;
}

#endif // USE_XML_STORAGE

KShapeBase* KShapeBase::LoadFromBuffer(KMemoryStream& mf, int* readed)
{
  DWORD b = mf.GetDWord();
  UINT size = mf.GetDWord();
  KShapeBase *s = createShape((ShapeType)b);

  if (readed != NULL)
    *readed = sizeof(DWORD) * 2 + size;

  if (s != NULL)
  {
    UINT sp = mf.GetPosition();
    s->ObjectId = mf.GetDWord();
    s->ObjectFlag = mf.GetDWord();
    // forcely visible
    s->ObjectFlag |= KSHAPE_VISIBLE;

    if (s->LoadFromBufferExt(mf))
    {
      // check readed buffer size
      UINT readsize = mf.GetPosition() - sp;
      if (readsize != size)
      {
        TCHAR msg[128];
        StringCchPrintf(msg, 128, _T("Load size mismatch(type=%d, head_size=%d, readed=%d)"), b, size, readed);
        StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG | SYNC_MSG_DEBUG);
      }
      return s;
    }
    delete s;
  }
  return NULL;
}

BOOL KShapeBase::LoadFromBufferExt(KMemoryStream& mf)
{
  TCHAR msg[128];
  StringCchPrintf(msg, 128, _T("Not implemented(type=%d)"), GetShapeType());
  StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG | SYNC_MSG_DEBUG);
  return FALSE;
}

int KShapeBase::StoreToBuffer(KMemoryStream& mf)
{
  mf.WriteDword((DWORD)GetShapeType());
  mf.WriteDword(0); // size of block
  DWORD p = mf.GetPosition();

  mf.WriteDword(ObjectId);
  mf.WriteDword(ObjectFlag & KSHAPE_STORED_FLAGS);

  if (StoreToBufferExt(mf))
  {
    /*
    DWORD n = mf.GetPosition();
    mf.Seek(p, FilePosition::begin);
    mf.WriteDword(n - p - sizeof(DWORD)); // size of block
    mf.Seek(n, FilePosition::begin);
    */
    mf.UpdateBlockSize(p);
    return TRUE;
  }
  return FALSE;
}

BOOL KShapeBase::StoreToBufferExt(KMemoryStream& mf)
{
  TCHAR msg[128];
  StringCchPrintf(msg, 128, _T("Not implemented(type=%d)"), GetShapeType());
  StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG | SYNC_MSG_DEBUG);
  return FALSE;
}


BOOL DoesListContainValue(KShapeVector& shapeList, KShapeBase* s)
{
  KShapeIterator it = shapeList.begin();
  while (it != shapeList.end())
  {
    if (*it == s)
      return TRUE;
    it++;
  }
  return FALSE;
}


BOOL RectContains(RECT& src, RECT r)
{
  return ((src.left <= r.left) && (r.right <= src.right) &&
    (src.top <= r.top) && (r.bottom <= src.bottom));

  /*
  return ((src.left < r.right) && (src.right > r.left) &&
    (src.top < r.bottom) && (src.bottom > r.top));
    */
}

void AddRect(RECT& dst, RECT& src)
{
  if ((dst.left == dst.right) || (dst.top == dst.bottom))
    memcpy(&dst, &src, sizeof(RECT));
  else
  {
    if (dst.left > src.left)
      dst.left = src.left;
    if (dst.right < src.right)
      dst.right = src.right;
    if (dst.top > src.top)
      dst.top = src.top;
    if (dst.bottom < src.bottom)
      dst.bottom = src.bottom;
  }
}

void AddRect(RECT& dst, int cx, int cy)
{
  dst.left -= cx;
  dst.right += cx;
  dst.top -= cy;
  dst.bottom += cy;
}

void OrRect(RECT& dst, RECT& src)
{
  if (dst.left > src.left)
    dst.left = src.left;
  if (dst.right < src.right)
    dst.right = src.right;
  if (dst.top > src.top)
    dst.top = src.top;
  if (dst.bottom < src.bottom)
    dst.bottom = src.bottom;
}

void ExpandRect(RECT& dst, int cx, int cy)
{
  dst.left -= cx;
  dst.right += cx;
  dst.top -= cy;
  dst.bottom += cy;
}

BYTE GetAlphaValue(COLORREF color)
{
  BYTE a = (color >> 24) & 0x0FF;
  return a;
}

void NormalizeRect(RECT& rect)
{
  int t;
  if (rect.left > rect.right)
  {
    t = rect.left;
    rect.left = rect.right;
    rect.right = t;
  }
  if (rect.top > rect.bottom)
  {
    t = rect.top;
    rect.top = rect.bottom;
    rect.bottom = t;
  }
}
