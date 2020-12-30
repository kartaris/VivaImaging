#pragma once

#include "KPropertyCtrlBase.h"
#include "..\Control\KColorPickerCtrl.h"
#include "..\Control\KLineComboBox.h"
// KPropertyCtrlAnnotateArrow

class KPropertyCtrlAnnotateArrow : public KPropertyCtrlBase
{
	DECLARE_DYNAMIC(KPropertyCtrlAnnotateArrow)

public:
  KPropertyCtrlAnnotateArrow();
  KPropertyCtrlAnnotateArrow(CWnd* parent, RECT& rect);
	virtual ~KPropertyCtrlAnnotateArrow();

  void UpdateCurrentStyle();

  KColorPickerCtrl mColorPicker;

  CStatic mLabelLineStyle;
  KLineComboBox mComboLineStyle;

  CStatic mLabelLineWidth;
  KLineComboBox mComboLineWidth;

  // start arrow
  CStatic mLabelStartArrow;
  KLineComboBox mComboStartArrow;

  // end arrow
  CStatic mLabelEndArrow;
  KLineComboBox mComboEndArrow;

  virtual BOOL GetLineStyle(OutlineStyle& linestyle, BYTE& startArrow, BYTE& endArrow);
  virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

protected:
  afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
  afx_msg void OnSize(UINT nType, int cx, int cy);
  afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
  afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
  afx_msg LRESULT OnChangeColor(WPARAM wParam, LPARAM lParam);
  DECLARE_MESSAGE_MAP()

};


