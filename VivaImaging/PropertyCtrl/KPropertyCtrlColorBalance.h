#pragma once

#include "KPropertyCtrlBase.h"

// KPropertyCtrlColorBalance

class KPropertyCtrlColorBalance : public KPropertyCtrlBase
{
	DECLARE_DYNAMIC(KPropertyCtrlColorBalance)

public:
  KPropertyCtrlColorBalance();
  KPropertyCtrlColorBalance(CWnd* parent, RECT& rect);
	virtual ~KPropertyCtrlColorBalance();

  CStatic mLabelRed;
  CSliderCtrl mSliderRed;
  CEdit mEditRed;

  CStatic mLabelGreen;
  CSliderCtrl mSliderGreen;
  CEdit mEditGreen;

  CStatic mLabelBlue;
  CSliderCtrl mSliderBlue;
  CEdit mEditBlue;

  CButton mPreview;
  CButton mAll;
  CButton mApply;

  int mRedValue;
  int mGreenValue;
  int mBlueValue;
  int mApplyAll;

  void readyEffectJob(ImageEffectJob& effect);
  void ApplyEffect();
  void UpdatePreview();
  void UpdateControlValue();
  void moveCtrlItems(int yoffset);

  virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

protected:
  afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
  afx_msg void OnSize(UINT nType, int cx, int cy);
  afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
  afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
  afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
  DECLARE_MESSAGE_MAP()

};


