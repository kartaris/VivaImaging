// Copyright (C) 2019 K2Mobile
// All rights reserved.
#pragma once

#include "Platform\twain\KTwainControl.h"
#include "Platform\wia\KWiaControl.h"

// KDlgSelectImageSource 대화 상자

#define WM_SCAN_IMAGE_DONE (WM_USER + 402)

class KDlgSelectImageSource : public CDialogEx
{
	DECLARE_DYNAMIC(KDlgSelectImageSource)

public:
	KDlgSelectImageSource(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~KDlgSelectImageSource();

  void SetWorkingPath(LPCTSTR workingPath);
  int StartScan(KImageCaptureDevice* device);
  BOOL ContinueScanData();
  BOOL ReadScanImage();
  BOOL EndScan();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_SELECT_IMAGE_SOURCE };
#endif
  CWnd* mParentView;

  KTwainControl* m_pTwainControl;
  KWiaControl m_WiaControl;
  LPTSTR mActiveDeviceName;
  LPTSTR mWorkingPath;

  KImageCaptureDeviceVector m_SourceDevices;

  CListBox mImageSourceList;

  UINT_PTR mScanCheckTimer;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
  virtual BOOL OnInitDialog();
  virtual void OnCancel();
  afx_msg void OnLbnDblclkSourceList();
  afx_msg void OnClickedScan();
  afx_msg void OnTimer(UINT_PTR nIDEvent);
  afx_msg LRESULT OnKTwainMessage(WPARAM wParam, LPARAM lParam);
  afx_msg LRESULT OnKWiaMessage(WPARAM wParam, LPARAM lParam);

};
