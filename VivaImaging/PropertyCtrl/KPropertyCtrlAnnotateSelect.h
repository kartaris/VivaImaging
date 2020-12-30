#pragma once

#include "KPropertyCtrlBase.h"
#include "..\Control\KImageButton.h"
#include "..\Control\KColorPickerCtrl.h"
#include "..\Control\KLineComboBox.h"
#include "..\Control\KFontComboBox.h"

class KPropertyCtrlAnnotateSelect : public KPropertyCtrlBase
{
	DECLARE_DYNAMIC(KPropertyCtrlAnnotateSelect)

public:
  KPropertyCtrlAnnotateSelect();
  KPropertyCtrlAnnotateSelect(CWnd* parent, RECT& rect);
	virtual ~KPropertyCtrlAnnotateSelect();

  void UpdateShapeStyle(void *obj);
  int GetFontAttribute();

  CStatic mLabelColor;
  KColorPickerCtrl mColorPicker;

  CStatic mLabelLineStyle;
  KLineComboBox mComboLineStyle;

  CStatic mLabelLineWidth;
  KLineComboBox mComboLineWidth;

  KFontComboBox mFaceNameCombo;
  KFontComboBox mFontSizeCombo;
  KImageButton mBold;
  KImageButton mItalic;
  KImageButton mUnderline;

  CButton mApply;

  CStatic mLabel1;
  KImageButton mAlignLeft;
  KImageButton mAlignCenter;
  KImageButton mAlignRight;

  KImageButton mAlignTop;
  KImageButton mAlignVCenter;
  KImageButton mAlignBottom;

  CStatic mLabel2;
  KImageButton mBringTop;
  KImageButton mBringUp;
  KImageButton mBringDown;
  KImageButton mBringBottom;

  void arrangeControls();


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


