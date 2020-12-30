#pragma once

#include "KPropertyCtrlBase.h"

// KPropertyCtrlMonochrome

class KPropertyCtrlMonochrome : public KPropertyCtrlBase
{
	DECLARE_DYNAMIC(KPropertyCtrlMonochrome)

public:
  KPropertyCtrlMonochrome();
  KPropertyCtrlMonochrome(CWnd* parent, RECT& rect);
	virtual ~KPropertyCtrlMonochrome();

  CButton mNone;
  CButton mBlackAndWhiteCheck;
  CButton mGrayscaleCheck;

  CStatic mLabel1;
  CSliderCtrl mGammaSlider;
  CEdit mGammaEdit;

  CButton mPreview;
  CButton mAll;
  CButton mApply;

  int mGrayscale;
  int mGammaValue; // 10 ~ 500
  int mApplyAll;

  void SetEnableGrayscale(BOOL enable);
  void readyEffectJob(ImageEffectJob& effect);
  void UpdatePreview();
  void InitProperty(CVivaImagingDoc* doc);
  void ApplyEffect();

  virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

protected:
  afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
  afx_msg void OnSize(UINT nType, int cx, int cy);
  afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
  afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
  afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
  DECLARE_MESSAGE_MAP()

};


