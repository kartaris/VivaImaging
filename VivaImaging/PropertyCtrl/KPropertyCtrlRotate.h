#pragma once

#include "KPropertyCtrlBase.h"
#include "..\Document\KImagePage.h"

// KPropertyCtrlRotate

class KPropertyCtrlRotate : public KPropertyCtrlBase
{
	DECLARE_DYNAMIC(KPropertyCtrlRotate)

public:
  KPropertyCtrlRotate();
  KPropertyCtrlRotate(CWnd* parent, RECT& rect);
	virtual ~KPropertyCtrlRotate();

  CButton mRadioButton0;
  CButton mRadioButton1;
  CButton mRadioButton2;
  CButton mRadioButton3;
  CButton mRadioButton4;
  CButton mRadioButton5;

  CButton mAll;
  CButton mApply;

  RotateMode mRotateMode;
  int mApplyAll;

  void readyEffectJob(ImageEffectJob& effect);
  void UpdatePreview();
  void ApplyEffect();

  static void DrawPropertyCtrlPreview(CDC* pDC, KImageDrawer& info, ImageEffectJob* effect);

  virtual void InitProperty(CVivaImagingDoc* doc);
  virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

protected:
  afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
  afx_msg void OnSize(UINT nType, int cx, int cy);
  afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
  afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
  DECLARE_MESSAGE_MAP()

};


