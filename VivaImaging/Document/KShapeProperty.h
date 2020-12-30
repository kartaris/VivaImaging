#pragma once

#include "KImageBase.h"
#include "KShapeContainer.h"

/**
* @brief ShapeEditor 加己蔼阑 包府窍绰  Class.
*/

#define MAX_LINE_WIDTH 32

class KShapeProperty
{
public:
  KShapeProperty();
  ~KShapeProperty();

  DWORD mShapeFillColor;
  OutlineStyle mLineStyle;
  int mStartArrow;
  int mEndArrow;
  DWORD mTextColor;
  CString mFacename;
  int mFontSize;
  int mFontAttribute;

  DWORD GetShapeFill();
  void SetShapeFill(DWORD fill);
  void GetLineStyle(OutlineStyle& outline);
  void SetLineStyle(OutlineStyle& outline);
  void SetLineColor(DWORD color);
  void SetLineStyle(int style);
  void SetLineWidth(int width);
  int GetStartArrow();
  int GetEndArrow();
  void SetStartArrow(int a);
  void SetEndArrow(int a);
  LPCTSTR GetFacename();
  void SetFacename(LPCTSTR name);
  int GetFontSize();
  void SetFontSize(int pt);
  DWORD GetTextColor();
  void SetTextColor(DWORD fill);
  int ToggleFontStyle(int attr);
  void GetFontStyle(LPTSTR facename, int& pointSize, DWORD& attr);

  void GetLineStyle(OutlineStyle& linestyle, BYTE& startArrow, BYTE& endArrow);
  void GetShapeStyle(DWORD& fillcolor, OutlineStyle& linestyle);
  void GetTextStyle(DWORD& textcolor, LPTSTR facename, int buffSize, int& pointSize, DWORD& attr);

  void StoreProperty();
  void LoadProperty();
};

