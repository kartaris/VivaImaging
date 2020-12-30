#pragma once

// KPropertyGridProperty

#define WMSG_PROPERTY_CHANGED (WM_USER + 420)

class KPropertyGridProperty : public CMFCPropertyGridProperty
{

public:
	KPropertyGridProperty(
    HWND hWnd, UINT id,
    const CString& strName,
    const _variant_t& varValue,
    LPCTSTR lpszDescr = NULL,
    DWORD_PTR dwData = 0);

	virtual ~KPropertyGridProperty();

  HWND mParent;
  UINT mID;
  virtual BOOL OnEndEdit();
};


