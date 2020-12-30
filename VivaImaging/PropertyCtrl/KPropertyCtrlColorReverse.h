#pragma once

#include "KPropertyCtrlBase.h"

// KPropertyCtrlColorReverse

class KPropertyCtrlColorReverse : public KPropertyCtrlBase
{
	DECLARE_DYNAMIC(KPropertyCtrlColorReverse)

public:
  KPropertyCtrlColorReverse();
  KPropertyCtrlColorReverse(CWnd* parent, RECT& rect);
	virtual ~KPropertyCtrlColorReverse();

  CButton mRadioButton0;
  CButton mRadioButton1;
  CButton mRadioButton2;
  CButton mRadioButton3;

  CButton mPreview;
  CButton mAll;
  CButton mApply;

  int mReverseMode;
  int mApplyAll;

  void readyEffectJob(ImageEffectJob& effect);
  void ApplyEffect();
  void UpdatePreview();

  virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
  virtual void InitProperty(CVivaImagingDoc* doc);

protected:
  afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
  afx_msg void OnSize(UINT nType, int cx, int cy);
  afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
  afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
  DECLARE_MESSAGE_MAP()

};


