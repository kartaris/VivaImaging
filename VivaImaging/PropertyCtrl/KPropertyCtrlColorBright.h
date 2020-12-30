#pragma once

#include "KPropertyCtrlBase.h"

// KPropertyCtrlColorBright

class KPropertyCtrlColorBright : public KPropertyCtrlBase
{
	DECLARE_DYNAMIC(KPropertyCtrlColorBright)

public:
  KPropertyCtrlColorBright();
  KPropertyCtrlColorBright(CWnd* parent, RECT& rect);
	virtual ~KPropertyCtrlColorBright();

  CStatic mLabelBright;
  CSliderCtrl mSliderBright;
  CEdit mEditBright;

  CStatic mLabelContrast;
  CSliderCtrl mSliderContrast;
  CEdit mEditContrast;

  CButton mPreview;
  CButton mAll;
  CButton mApply;

  int mBrightValue;
  int mContrastValue;
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


