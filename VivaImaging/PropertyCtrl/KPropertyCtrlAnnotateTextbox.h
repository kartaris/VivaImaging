#pragma once

#include "KPropertyCtrlBase.h"
#include "..\Control\KColorPickerCtrl.h"
#include "..\Control\KImageButton.h"
#include "..\Control\KFontComboBox.h"

// KPropertyCtrlAnnotateTextbox

class KPropertyCtrlAnnotateTextbox : public KPropertyCtrlBase
{
	DECLARE_DYNAMIC(KPropertyCtrlAnnotateTextbox)

public:
  KPropertyCtrlAnnotateTextbox();
  KPropertyCtrlAnnotateTextbox(CWnd* parent, RECT& rect);
	virtual ~KPropertyCtrlAnnotateTextbox();

  KColorPickerCtrl mColorPicker;

  KFontComboBox mFaceNameCombo;
  KFontComboBox mFontSizeCombo;
  KImageButton mBold;
  KImageButton mItalic;
  KImageButton mUnderline;

  void InitFontList();
  int AddFontFamily(const LOGFONT* lf);
  // void UpdateCurrentStyle();
  void UpdateShapeStyle(void *obj);

  virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

protected:
  afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
  afx_msg void OnSize(UINT nType, int cx, int cy);
  afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
  afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
  afx_msg LRESULT OnChangeColor(WPARAM wParam, LPARAM lParam);
  DECLARE_MESSAGE_MAP()

};


