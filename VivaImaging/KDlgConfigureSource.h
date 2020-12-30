// KDlgConfigureSource 대화 상자
// Copyright (C) 2019 K2Mobile
// All rights reserved.
#pragma once

#include "Control\KConfigureSourceCtrl.h"
#include "Platform\twain\KTwainControl.h"

class KDlgConfigureSource : public CDialogEx
{
	DECLARE_DYNAMIC(KDlgConfigureSource)

public:
	KDlgConfigureSource(CWnd* pParent = nullptr, KTwainControl* twainControl = NULL, UINT32 deviceID = 0);   // 표준 생성자입니다.
	virtual ~KDlgConfigureSource();

  UINT32 m_DeviceID;
  KTwainControl* m_pTwainControl;
  CFont* mFont;

  KConfigureSourceCtrl m_ConfigureSourceCtrl;
  


// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_CONFIGURE_SOURCE };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
  virtual BOOL OnInitDialog();
  virtual void OnSetFont(CFont* pFont);
  virtual void OnCancel();
  afx_msg void OnClickedScan();
};
