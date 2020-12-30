#pragma once

#include "KPropertyCtrlBase.h"

// KPropertyCtrlColorHSV

class KPropertyCtrlColorHSV : public KPropertyCtrlBase
{
	DECLARE_DYNAMIC(KPropertyCtrlColorHSV)

public:
  KPropertyCtrlColorHSV();
  KPropertyCtrlColorHSV(CWnd* parent, RECT& rect);
	virtual ~KPropertyCtrlColorHSV();

  CStatic mLabelHue;
  CSliderCtrl mSliderHue;
  CEdit mEditHue;

  CStatic mLabelSaturation;
  CSliderCtrl mSliderSaturation;
  CEdit mEditSaturation;

  CStatic mLabelValue;
  CSliderCtrl mSliderValue;
  CEdit mEditValue;

  CButton mPreview;
  CButton mAll;
  CButton mApply;

  int mHueValue;
  int mSaturationValue;
  int mValueValue;
  int mApplyAll;

  void readyEffectJob(ImageEffectJob& effect);
  void ApplyEffect();
  void UpdatePreview();
  void UpdateControlValue();

  virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

protected:
  afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
  afx_msg void OnSize(UINT nType, int cx, int cy);
  afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
  afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
  afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
  DECLARE_MESSAGE_MAP()

};


