#pragma once

#include "KPropertyCtrlBase.h"
#include "..\Control\KColorPickerCtrl.h"
// KPropertyCtrlEditImage

class KPropertyCtrlEditImage : public KPropertyCtrlBase
{
	DECLARE_DYNAMIC(KPropertyCtrlEditImage)

public:
  KPropertyCtrlEditImage();
  KPropertyCtrlEditImage(CWnd* parent, RECT& rect);
	virtual ~KPropertyCtrlEditImage();

  CStatic mLabel1;
  KColorPickerCtrl mColorPicker;

  CButton mAll;
  CButton mFill;
  CButton mCopy;
  CButton mPaste;

  virtual void OnCloseProperty();
  virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
  virtual BOOL GetFillColor(DWORD& fillcolor);

  void UpdateCurrentStyle();

protected:
  afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
  afx_msg void OnSize(UINT nType, int cx, int cy);
  afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
  afx_msg LRESULT OnChangeColor(WPARAM wParam, LPARAM lParam);
  afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
  DECLARE_MESSAGE_MAP()

};


