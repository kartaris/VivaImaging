#pragma once

#include "Core\KConstant.h"

// KDlgUploadAs 대화 상자

class KDlgUploadAs : public CDialogEx
{
	DECLARE_DYNAMIC(KDlgUploadAs)

public:
	KDlgUploadAs(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~KDlgUploadAs();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_UPLOAD_AS };
#endif

  void SetUploadInfos(LPCTSTR serverFolderOID, LPCTSTR localWorkingPath, LPCTSTR name, ImagingSaveFormat format, UINT flag);

  ImagingSaveFormat mFormat;
  TCHAR mServerFolderOID[MAX_OID];
  CString mFilename;
  CString mLocalWorkingPath;
  CString mServerWorkingPath;
  UINT mFlag;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
  virtual void OnOK();

	DECLARE_MESSAGE_MAP()
public:
  virtual BOOL OnInitDialog();
  CEdit mEditLocalWorkingPath;
  CComboBox mComboUploadFileType;
  CComboBox mComboUploadFolderOption;
  CEdit mEditFilename;
  CEdit mEditServerWorkingPath;
  afx_msg void OnBnClickedSelectServerFolder();
  afx_msg LRESULT OnThreadedCallMessage(WPARAM wParam, LPARAM lParam);

  afx_msg void OnCbnSelendokComboUploadFileType();
};
