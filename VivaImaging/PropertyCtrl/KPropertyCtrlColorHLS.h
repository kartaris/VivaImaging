#pragma once

#include "KPropertyCtrlBase.h"

// KPropertyCtrlColorHLS

class KPropertyCtrlColorHLS : public KPropertyCtrlBase
{
	DECLARE_DYNAMIC(KPropertyCtrlColorHLS)

public:
  KPropertyCtrlColorHLS();
  KPropertyCtrlColorHLS(CWnd* parent, RECT& rect);
	virtual ~KPropertyCtrlColorHLS();

  CStatic mLabelHue;
  CSliderCtrl mSliderHue;
  CEdit mEditHue;

  CStatic mLabelLightness;
  CSliderCtrl mSliderLightness;
  CEdit mEditLightness;

  CStatic mLabelSaturation;
  CSliderCtrl mSliderSaturation;
  CEdit mEditSaturation;

  CButton mPreview;
  CButton mAll;
  CButton mApply;

  int mHueValue;
  int mLightnessValue;
  int mSaturationValue;
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


