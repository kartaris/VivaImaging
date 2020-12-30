#pragma once

#include "KPropertyCtrlBase.h"
#include "..\Document\KImagePage.h"

#include "..\Control\KImageOutputLevelCtrl.h"

// KPropertyCtrlColorLevel

class KPropertyCtrlColorLevel : public KPropertyCtrlBase
{
	DECLARE_DYNAMIC(KPropertyCtrlColorLevel)

public:
  KPropertyCtrlColorLevel();
  KPropertyCtrlColorLevel(CWnd* parent, RECT& rect);
	virtual ~KPropertyCtrlColorLevel();

  CButton mCommonApplyChannels;
  CButton mSeparateApplyChannels;

  CStatic mLabel1;
  CComboBox mChannelCombo;
  CStatic mLabel2;
  KImageOutputLevelCtrl mInputCtrl;
  CStatic mLabel3;
  KImageOutputLevelCtrl mOutputCtrl;

  CButton mPreview;
  CButton mAll;
  CButton mApply;
  CButton mReset;

  BOOL mSeparate;
  int mChannels;
  BYTE mLevelParams[15];

  // KImageBase* mpImage;
  BYTE* mpHistogramBuffer;

  int mApplyAll;

  void OnApplyMethod(BOOL bSeparately);

  virtual void InitProperty(CVivaImagingDoc* doc);

  void ResetProprety();
  void UpdateCurrentLevelValues();

  void readyEffectJob(ImageEffectJob& effect);
  void ApplyEffect();
  void UpdatePreview();

  virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

protected:
  afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
  afx_msg void OnSize(UINT nType, int cx, int cy);
  afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
  afx_msg LRESULT OnNOLChanged(WPARAM wParam, LPARAM lParam);
  afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
  DECLARE_MESSAGE_MAP()

};


