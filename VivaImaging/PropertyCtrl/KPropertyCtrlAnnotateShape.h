#pragma once

#include "KPropertyCtrlBase.h"
#include "..\Control\KColorPickerCtrl.h"
#include "..\Control\KLineComboBox.h"
// KPropertyCtrlAnnotateShape

class KPropertyCtrlAnnotateShape : public KPropertyCtrlBase
{
	DECLARE_DYNAMIC(KPropertyCtrlAnnotateShape)

public:
  KPropertyCtrlAnnotateShape();
  KPropertyCtrlAnnotateShape(CWnd* parent, RECT& rect);
	virtual ~KPropertyCtrlAnnotateShape();

  void UpdateCurrentStyle();

  KColorPickerCtrl mColorPicker;

  CStatic mLabelLineStyle;
  KLineComboBox mComboLineStyle;

  CStatic mLabelLineWidth;
  KLineComboBox mComboLineWidth;

  virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
  virtual BOOL GetShapeStyle(DWORD& fillcolor, OutlineStyle& linestyle);

protected:
  afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
  afx_msg void OnSize(UINT nType, int cx, int cy);
  afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
  afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
  afx_msg LRESULT OnChangeColor(WPARAM wParam, LPARAM lParam);
  DECLARE_MESSAGE_MAP()

};


