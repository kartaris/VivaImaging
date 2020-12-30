#pragma once

#include "KPropertyCtrlBase.h"
#include "..\Document\KImagePage.h"

// KPropertyCtrlResize

class KPropertyCtrlResize : public KPropertyCtrlBase
{
	DECLARE_DYNAMIC(KPropertyCtrlResize)

public:
  KPropertyCtrlResize();
  KPropertyCtrlResize(CWnd* parent, RECT& rect);
	virtual ~KPropertyCtrlResize();

  CButton mByRatioCheck;
  CStatic mLabel1;
  CEdit mEditWidth;
  CStatic mUnit1;
  CStatic mLabel2;
  CEdit mEditHeight;
  CStatic mUnit2;
  CButton mPreserveRatioCheck;

  CButton mAll;
  CButton mApply;

  int mWithRatio;
  int mWidth;
  int mHeight;
  int mSourceWidth;
  int mSourceHeight;
  int mEditBy;
  int mPreserveRatio;
  int mApplyAll;

  void UpdateValueToControl(int mask);
  void changeValuesAsRatio(BOOL asRatio);
  void SetValueAsRatio(BOOL asRatio);

  void readyEffectJob(ImageEffectJob& effect);
  void UpdatePreview();
  void ApplyEffect();

  static void DrawPropertyCtrlPreview(CDC* pDC, KImageDrawer& info, ImageEffectJob* effect);
  static int MouseHitTest(POINT& point, KImageDrawer& info, ImageEffectJob* effect);
  static int MouseDragging(MouseState state, POINT& point, KImageDrawer& info, ImageEffectJob* effect);

  virtual void InitProperty(CVivaImagingDoc* doc);

  virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

  virtual PropertyMode GetCtrlMode() { return PropertyMode::PModeResize; }
  virtual BOOL OnPropertyValueChanged(MouseState state, ImageEffectJob* effect);

protected:
  afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
  afx_msg void OnSize(UINT nType, int cx, int cy);
  afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
  afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
  DECLARE_MESSAGE_MAP()

};


