#pragma once

#include "KPropertyCtrlBase.h"
#include "..\Document\KImagePage.h"

// KPropertyCtrlCrop

class KPropertyCtrlCrop : public KPropertyCtrlBase
{
	DECLARE_DYNAMIC(KPropertyCtrlCrop)

public:
  KPropertyCtrlCrop();
  KPropertyCtrlCrop(CWnd* parent, RECT& rect);
	virtual ~KPropertyCtrlCrop();

  CButton mByRatioCheck;

  CStatic mLabel1;
  CEdit mEditLeft;
  CStatic mUnit1;
  CStatic mLabel2;
  CEdit mEditTop;
  CStatic mUnit2;
  CStatic mLabel3;
  CEdit mEditRight;
  CStatic mUnit3;
  CStatic mLabel4;
  CEdit mEditBottom;
  CStatic mUnit4;

  CButton mAll;
  CButton mApply;

  int mByRatio;
  int mLeft;
  int mTop;
  int mRight;
  int mBottom;
  int mSourceWidth;
  int mSourceHeight;
  int mApplyAll;

  virtual PropertyMode GetCtrlMode() { return PropertyMode::PModeCrop; }
  virtual BOOL OnPropertyValueChanged(MouseState state, ImageEffectJob* effect);

  void UpdateValueToControl(int mask);
  void changeValuesAsRatio(BOOL asRatio);
  void SetValueAsRatio(BOOL asRatio);

  void readyEffectJob(ImageEffectJob& effect);
  void ApplyEffect();
  void UpdatePreview();

  static void DrawPropertyCtrlPreview(CDC* pDC, KImageDrawer& info, ImageEffectJob* effect);
  static int MouseHitTest(POINT& point, KImageDrawer& info, ImageEffectJob* effect);
  static int MouseDragging(MouseState state, POINT& point, KImageDrawer& info, ImageEffectJob* effect);

  virtual void InitProperty(CVivaImagingDoc* doc);

  virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

protected:
  afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
  afx_msg void OnSize(UINT nType, int cx, int cy);
  afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
  afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
  DECLARE_MESSAGE_MAP()

};


