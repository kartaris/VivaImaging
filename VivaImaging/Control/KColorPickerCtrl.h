#pragma once

// KColorPickerCtrl
#include "..\Platform\KColor.h"
#include "..\Platform\KBitmap.h"

#define WMA_SELECT_KCOLOR (WM_USER + 350)
#define WMA_CHANGE_COLOR (WM_USER + 352)

#define NO_CUSTOM_PALETTE 20

#define KCOLOR_WITH_HEXA 1
#define KCOLOR_WITH_ALPHA 2
#define KCOLOR_WITH_SLIDER 4
#define KCOLOR_WITH_DUAL 8

class KColorPickerCtrl : public CWnd
{
	DECLARE_DYNAMIC(KColorPickerCtrl)

public:
	KColorPickerCtrl();
	virtual ~KColorPickerCtrl();

  BOOL RegisterWndClass();

  BOOL Create(DWORD windowStyle, RECT& rect, CWnd* parent, int controlId, int style);
  void SetMode(int mode);
  void SetColor(HSV_C hsv, BOOL foreColor);

  static DWORD MakeARGBColor(RGB_C rgb, BYTE alpha);

  void SetARGBColor(DWORD color, BOOL foreColor);
  DWORD GetARGBColor(BOOL foreColor);

  CStatic mLabelHexa;
  CEdit mEditHexaColor;

  CStatic mLabelAlpha;
  CSliderCtrl mSliderAlpha;
  CEdit mEditAlpha;

  CStatic mLabelHue;
  CSliderCtrl mSliderHue;
  CEdit mEditHue;

  CStatic mLabelSaturation;
  CSliderCtrl mSliderSaturation;
  CEdit mEditSaturation;

  CStatic mLabelValue;
  CSliderCtrl mSliderValue;
  CEdit mEditValue;

  HSV_C mHSV;
  HSV_C mBgHSV;
  BYTE mAlpha;
  BYTE mForeColor;

  HSV_C mCustomPalette[NO_CUSTOM_PALETTE];

  int mStyle;
  int mMouseDragMode;

  KBitmap mCacheMap;
  KBitmap mHueMap;
  CButton mAdd;

  void UpdateScreen(int mode);
  void OnToggleForeBackColor();
  void OnSelectColorMap(int mode, HSV_C& hsv);
  int ColorModeFromPoint(POINT& point, HSV_C* hsv, int mode);
  void ReadyHSVCacheMap();
  void ReadyHueCacheMap();

  void UpdateColorSlider(int param);
  void UpdateCurrentColor(BOOL bNotify = TRUE);

  void AddToCustomPalette(HSV_C hsv);

	void DrawBackColor(CDC* pDC);
	void DrawFrontColor(CDC* pDC);

  virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);


protected:
	DECLARE_MESSAGE_MAP()
public:
  afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
  afx_msg void OnPaint();
  afx_msg void OnSize(UINT nType, int cx, int cy);
  afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
  afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
  afx_msg void OnMouseMove(UINT nFlags, CPoint point);
  afx_msg void OnMouseLeave();
  afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
  afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
  afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
  afx_msg void OnTimer(UINT_PTR timerEvent);

};


