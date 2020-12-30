#pragma once


// KDlgShowMessage 대화 상자

class KDlgShowMessage : public CDialogEx
{
	DECLARE_DYNAMIC(KDlgShowMessage)

public:
	KDlgShowMessage(CWnd* pParent, CString& label, CString& msg);   // 표준 생성자입니다.
	virtual ~KDlgShowMessage();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_SHOW_MESSAGES };
#endif

  CString mLabel;
  CString mMessage;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
  virtual BOOL OnInitDialog();

};
