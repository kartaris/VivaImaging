#pragma once


// KDlgViewZoom 대화 상자

class KDlgViewZoom : public CDialogEx
{
	DECLARE_DYNAMIC(KDlgViewZoom)

public:
	KDlgViewZoom(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~KDlgViewZoom();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_VIEW_ZOOM };
#endif

  float mZoom;
  BOOL mbFitTo;

  void SetZoom(float zoom, BOOL bFitTo);
  float GetZoom();

  virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
  virtual void OnOK();

	DECLARE_MESSAGE_MAP()
public:
  virtual BOOL OnInitDialog();
  int mRatio;
  CButton mFitTo;
  CEdit mEditZoom;
  CSliderCtrl mSliderZoom;
  afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
};
