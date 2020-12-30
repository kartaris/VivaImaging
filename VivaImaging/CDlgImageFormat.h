#pragma once

#include "Core/KConstant.h"

// CDlgImageFormat 대화 상자

class CDlgImageFormat : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgImageFormat)

public:
	CDlgImageFormat(CWnd* pParent = nullptr, ImagingSaveFormat defaultFormat = FORMAT_ORIGIN);   // 표준 생성자입니다.
	virtual ~CDlgImageFormat();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_IMAGE_FORMAT };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual void OnOK();

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	ImagingSaveFormat mUploadFileType;
	CComboBox mComboUploadFileType;
};
