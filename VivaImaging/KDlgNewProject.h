// Copyright (C) 2019 K2Mobile
// All rights reserved.
#pragma once


// KDlgNewProject 대화 상자

class KDlgNewProject : public CDialogEx
{
	DECLARE_DYNAMIC(KDlgNewProject)

public:
	KDlgNewProject(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~KDlgNewProject();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_NEW_PROJECT1 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
};
