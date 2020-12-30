#include "stdafx.h"

#include "KShapeProperty.h"
#include "../Platform/Reg.h"
#include "../Platform/Graphics.h"

KShapeProperty::KShapeProperty()
{
  mShapeFillColor = 0xFFFF0000;
  mLineStyle = { 0xFF0000FF, 1, 1 };
  mStartArrow = 0;
  mEndArrow = 0;
  mTextColor = 0xFFD0D0D0;
  mFacename = _T("±¼¸²");
  mFontSize = 16;
  mFontAttribute = 0;
}

KShapeProperty::~KShapeProperty()
{
}

DWORD KShapeProperty::GetShapeFill()
{
  return mShapeFillColor;
}

void KShapeProperty::SetShapeFill(DWORD fill)
{
  mShapeFillColor = fill;
}

void KShapeProperty::GetLineStyle(OutlineStyle& outline)
{
  outline = mLineStyle;
}

void KShapeProperty::SetLineStyle(OutlineStyle& outline)
{
  mLineStyle = outline;
}

void KShapeProperty::SetLineColor(DWORD color)
{
  mLineStyle.Color = color;
}

void KShapeProperty::SetLineStyle(int style)
{
  mLineStyle.Style = style;
}

void KShapeProperty::SetLineWidth(int width)
{
  mLineStyle.Width = width;
}

int KShapeProperty::GetStartArrow()
{
  return mStartArrow;
}

int KShapeProperty::GetEndArrow()
{
  return mEndArrow;
}

void KShapeProperty::SetStartArrow(int a)
{
  mStartArrow = a;
}

void KShapeProperty::SetEndArrow(int a)
{
  mEndArrow = a;
}

LPCTSTR KShapeProperty::GetFacename()
{
  return mFacename;
}

void KShapeProperty::SetFacename(LPCTSTR name)
{
  mFacename = name;
}

int KShapeProperty::GetFontSize()
{
  return mFontSize;
}

void KShapeProperty::SetFontSize(int pt)
{
  mFontSize = pt;
}

DWORD KShapeProperty::GetTextColor()
{
  return mTextColor;
}

void KShapeProperty::SetTextColor(DWORD fill)
{
  mTextColor = fill;
}

int KShapeProperty::ToggleFontStyle(int attr)
{
  mFontAttribute ^= attr;
  return mFontAttribute;
}

void KShapeProperty::GetFontStyle(LPTSTR facename, int& pointSize, DWORD& attr)
{
  StringCchCopy(facename, LF_FACESIZE, mFacename);
  pointSize = mFontSize;
  attr = mFontAttribute;
}


void KShapeProperty::GetLineStyle(OutlineStyle& linestyle, BYTE& startArrow, BYTE& endArrow)
{
  linestyle = mLineStyle;
  startArrow = mStartArrow;
  endArrow = mEndArrow;
}

void KShapeProperty::GetShapeStyle(DWORD& fillcolor, OutlineStyle& linestyle)
{
  fillcolor = mShapeFillColor;
  linestyle = mLineStyle;
}

void KShapeProperty::GetTextStyle(DWORD& textcolor, LPTSTR facename, int buffSize, int& pointSize, DWORD& attr)
{
  textcolor = mTextColor;
  StringCchCopy(facename, buffSize, (LPCTSTR)mFacename);
  pointSize = mFontSize;
  attr = mFontAttribute;
}

static PCTSTR VIVA_SHAPE_EDITOR_DEFAULT_PROPERTY_KEY = L"Software\\Destiny Imaging\\VivaImaging\\ShapeProperty";
LPCTSTR SHAPE_PROPERTY_FILL_COLOR = _T("FillColor");
LPCTSTR SHAPE_PROPERTY_OUTLINE_STYLE = _T("OutlineStyle");
LPCTSTR SHAPE_PROPERTY_OUTLINE_WIDTH = _T("OutlineWidth");
LPCTSTR SHAPE_PROPERTY_OUTLINE_COLOR = _T("OutlineColor");
LPCTSTR SHAPE_PROPERTY_LINE_STARTARROW = _T("OutlineStartArrow");
LPCTSTR SHAPE_PROPERTY_LINE_ENDARROW = _T("OutlineEndArrow");

LPCTSTR SHAPE_PROPERTY_TEXT_COLOR = _T("TextColor");
LPCTSTR SHAPE_PROPERTY_TEXT_FACENAME = _T("TextFacename");
LPCTSTR SHAPE_PROPERTY_FONT_SIZE = _T("TextFontSize");
LPCTSTR SHAPE_PROPERTY_FONT_ATTRIBUTE = _T("TextFontAttribute");

void KShapeProperty::StoreProperty()
{
  HKEY hKey = NULL;
  DWORD cbData;

  HRESULT hr = HRESULT_FROM_WIN32(RegCreateKeyEx(HKEY_CURRENT_USER, 
    VIVA_SHAPE_EDITOR_DEFAULT_PROPERTY_KEY, 0,
    NULL, REG_OPTION_NON_VOLATILE, KEY_READ|KEY_WRITE, NULL, &hKey, NULL));

  if (SUCCEEDED(hr))
  {
    WCHAR facename[LF_FACESIZE];
    DWORD value;

    cbData = sizeof(DWORD);
    hr = HRESULT_FROM_WIN32(RegSetValueEx(hKey, SHAPE_PROPERTY_FILL_COLOR, NULL,
      REG_DWORD, reinterpret_cast<LPBYTE>(&mShapeFillColor), cbData));

    cbData = sizeof(DWORD);
    value = mLineStyle.Style;
    hr = HRESULT_FROM_WIN32(RegSetValueEx(hKey, SHAPE_PROPERTY_OUTLINE_STYLE, NULL,
      REG_BINARY, reinterpret_cast<LPBYTE>(&value), cbData));

    value = mLineStyle.Width;
    hr = HRESULT_FROM_WIN32(RegSetValueEx(hKey, SHAPE_PROPERTY_OUTLINE_WIDTH, NULL,
      REG_BINARY, reinterpret_cast<LPBYTE>(&value), cbData));

    cbData = sizeof(DWORD);
    hr = HRESULT_FROM_WIN32(RegSetValueEx(hKey, SHAPE_PROPERTY_OUTLINE_COLOR, NULL,
      REG_DWORD, reinterpret_cast<LPBYTE>(&mLineStyle.Color), cbData));

    cbData = sizeof(int);
    hr = HRESULT_FROM_WIN32(RegSetValueEx(hKey, SHAPE_PROPERTY_LINE_STARTARROW, NULL,
      REG_DWORD, reinterpret_cast<LPBYTE>(&mStartArrow), cbData));

    hr = HRESULT_FROM_WIN32(RegSetValueEx(hKey, SHAPE_PROPERTY_LINE_ENDARROW, NULL,
      REG_DWORD, reinterpret_cast<LPBYTE>(&mEndArrow), cbData));

    hr = HRESULT_FROM_WIN32(RegSetValueEx(hKey, SHAPE_PROPERTY_TEXT_COLOR, NULL,
      REG_DWORD, reinterpret_cast<LPBYTE>(&mTextColor), cbData));

    StringCchCopy(facename, LF_FACESIZE, (LPCTSTR)mFacename);
    cbData = lstrlen(facename) * sizeof(WCHAR);
    hr = HRESULT_FROM_WIN32(RegSetValueEx(hKey, SHAPE_PROPERTY_TEXT_FACENAME, NULL,
      REG_SZ, reinterpret_cast<LPBYTE>(facename), cbData));

    cbData = sizeof(int);
    hr = HRESULT_FROM_WIN32(RegSetValueEx(hKey, SHAPE_PROPERTY_FONT_SIZE, NULL,
      REG_DWORD, reinterpret_cast<LPBYTE>(&mFontSize), cbData));

    hr = HRESULT_FROM_WIN32(RegSetValueEx(hKey, SHAPE_PROPERTY_FONT_ATTRIBUTE, NULL,
      REG_DWORD, reinterpret_cast<LPBYTE>(&mFontAttribute), cbData));

    RegCloseKey(hKey);
  }
}

void KShapeProperty::LoadProperty()
{
  HKEY hKey = NULL;
  BOOL rtn = TRUE; // default is Enable
  DWORD data;
  DWORD cbData;

  HRESULT hr = HRESULT_FROM_WIN32(RegCreateKeyEx(HKEY_CURRENT_USER,
    VIVA_SHAPE_EDITOR_DEFAULT_PROPERTY_KEY, 0,
    NULL, REG_OPTION_NON_VOLATILE, KEY_READ, NULL, &hKey, NULL));

  if (SUCCEEDED(hr))
  {
    WCHAR facename[LF_FACESIZE];

    cbData = sizeof(DWORD);
    hr = HRESULT_FROM_WIN32(RegQueryValueEx(hKey, SHAPE_PROPERTY_FILL_COLOR, NULL,
      NULL, reinterpret_cast<LPBYTE>(&data), &cbData));
    if (SUCCEEDED(hr))
      mShapeFillColor = (DWORD)data;

    hr = HRESULT_FROM_WIN32(RegQueryValueEx(hKey, SHAPE_PROPERTY_OUTLINE_STYLE, NULL,
      NULL, reinterpret_cast<LPBYTE>(&data), &cbData));
    if (SUCCEEDED(hr))
    {
      mLineStyle.Style = (BYTE)data;
      if ((LineStyle)mLineStyle.Style > LineStyle::LineMax)
        mLineStyle.Style = (BYTE)LineStyle::LineSolid;
    }

    hr = HRESULT_FROM_WIN32(RegQueryValueEx(hKey, SHAPE_PROPERTY_OUTLINE_WIDTH, NULL,
      NULL, reinterpret_cast<LPBYTE>(&data), &cbData));
    if (SUCCEEDED(hr))
    {
      mLineStyle.Width = (BYTE)data;
      if (mLineStyle.Width > MAX_LINE_WIDTH)
        mLineStyle.Width = 1;
    }

    hr = HRESULT_FROM_WIN32(RegQueryValueEx(hKey, SHAPE_PROPERTY_OUTLINE_COLOR, NULL,
      NULL, reinterpret_cast<LPBYTE>(&data), &cbData));
    if (SUCCEEDED(hr))
      mLineStyle.Color = (DWORD)data;

    hr = HRESULT_FROM_WIN32(RegQueryValueEx(hKey, SHAPE_PROPERTY_LINE_STARTARROW, NULL,
      NULL, reinterpret_cast<LPBYTE>(&data), &cbData));
    if (SUCCEEDED(hr))
      mStartArrow = (int)data;

    hr = HRESULT_FROM_WIN32(RegQueryValueEx(hKey, SHAPE_PROPERTY_LINE_ENDARROW, NULL,
      NULL, reinterpret_cast<LPBYTE>(&data), &cbData));
    if (SUCCEEDED(hr))
      mEndArrow = (int)data;

    hr = HRESULT_FROM_WIN32(RegQueryValueEx(hKey, SHAPE_PROPERTY_TEXT_COLOR, NULL,
      NULL, reinterpret_cast<LPBYTE>(&data), &cbData));
    if (SUCCEEDED(hr))
      mTextColor = (DWORD)data;

    cbData = LF_FACESIZE * sizeof(WCHAR);
    hr = HRESULT_FROM_WIN32(RegQueryValueEx(hKey, SHAPE_PROPERTY_TEXT_FACENAME, NULL,
      NULL, reinterpret_cast<LPBYTE>(facename), &cbData));
    if (SUCCEEDED(hr))
      mFacename.SetString((LPCWSTR)facename);
    else
      mFacename.SetString(gDefaultFacename);

    hr = HRESULT_FROM_WIN32(RegQueryValueEx(hKey, SHAPE_PROPERTY_FONT_SIZE, NULL,
      NULL, reinterpret_cast<LPBYTE>(&data), &cbData));
    if (SUCCEEDED(hr))
      mFontSize = (DWORD)data;

    hr = HRESULT_FROM_WIN32(RegQueryValueEx(hKey, SHAPE_PROPERTY_FONT_ATTRIBUTE, NULL,
      NULL, reinterpret_cast<LPBYTE>(&data), &cbData));
    if (SUCCEEDED(hr))
      mFontAttribute = (DWORD)data;

    RegCloseKey(hKey);
  }
}

