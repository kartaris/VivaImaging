#pragma once


#define WMA_CANCEL_PROGRESS (WM_USER + 430)
// KDlgLoadProgress 대화 상자

class KDlgLoadProgress : public CDialogEx
{
	DECLARE_DYNAMIC(KDlgLoadProgress)

public:
	KDlgLoadProgress(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~KDlgLoadProgress();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_LOAD_PROGRESS };
#endif

  void SetLabel(int index, LPCTSTR str);
  void SetProgress(int doneFolders, int totalFolders, int doneFiles, int totalFiles, HANDLE hError);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
  CProgressCtrl mProgress;
  virtual BOOL OnInitDialog();
  virtual void OnCancel();
};
