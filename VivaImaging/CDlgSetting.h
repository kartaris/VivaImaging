#pragma once

// CDlgSetting 대화 상자
// Copyright (C) 2019 K2Mobile
// All rights reserved.

#include "Core/KConstant.h"

class CDlgSetting : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgSetting)

public:
	CDlgSetting(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CDlgSetting();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_FILE_SETTING };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
  virtual void OnOK();

	DECLARE_MESSAGE_MAP()
public:
  virtual BOOL OnInitDialog();
  ImagingSaveFormat mUploadFileType;
  BOOL mMergeAnnotatesToImage;
  BOOL mChangeSelectAfterDraw;
  CString mStrWorkingFolder;
  CString mStrUploadFolder;
  CString mStrUploadFolderOID;
  CString mStrNewFilenameFormat;

  CEdit mEditWorkingFolder;
  CEdit mEditUploadFolder;
  CEdit mEditNewFilenameFormat;
  CComboBox mComboUploadFileType;
  CButton mCheckMergeAnnotates;
  CButton mCheckChangeSelectAfterDraw;
  afx_msg void OnBnClickedSelectWorkingFolder();
  afx_msg void OnBnClickedSelectServerFolder();
  afx_msg LRESULT OnThreadedCallMessage(WPARAM wParam, LPARAM lParam);
};
