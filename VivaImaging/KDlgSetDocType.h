#pragma once
#include "Core\KConstant.h"
#include "Core\KEcmDocType.h"

#include <vector>
#include <string>

typedef std::pair<std::wstring, std::wstring> StringStringPair;
typedef std::vector<StringStringPair> DocTypeVector;
typedef DocTypeVector::iterator DocTypeVectorIterator;

typedef std::vector<KEcmDocTypeInfo*> DocTypeInfoVector;
typedef DocTypeInfoVector::iterator DocTypeInfoVectorIterator;

#define DOCTYPE_FOR_GLOBAL

// KDlgSetDocType 대화 상자

class KDlgSetDocType : public CDialogEx
{
	DECLARE_DYNAMIC(KDlgSetDocType)

public:
	KDlgSetDocType(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~KDlgSetDocType();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_SET_DOCTYPE };
#endif

	CString mServerWorkingPath;
	DocTypeVector mAvailableDocTypeVector;

private:
	int OnSelectDocTypeOid(LPCTSTR selectedDocTypeOid);
#ifdef DOCTYPE_FOR_GLOBAL
	DocTypeInfoVector mDocTypeInfoVector;

	void UpdateGlobalDocType();
	void FromXDocTypeSt(KEcmDocTypeInfo& docTypeInfo, void* xdocType);
#else
	void UpdateAvaiableDocType();
	void UpdateDocTypes(LPCTSTR docTypes);
	void AddDocType(LPCTSTR docType);
#endif

	LRESULT OnThreadedCallMessage(WPARAM wParam, LPARAM lParam);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

public:
  afx_msg BOOL OnInitDialog();
  afx_msg void OnBnClickedSelectServerFolder();
	afx_msg void OnLbnSelchangeListDoctype();

	CListBox mAvailableDocTypes;
	CListBox mMetadataList;
	CEdit mServerFolderPathEdit;

  TCHAR mServerFolderOID[MAX_OID];
  TCHAR mDocTypeOid[MAX_OID];
  KEcmDocTypeInfo mDocTypeInfo;

};
