// Copyright (C) 2019 K2Mobile
// All rights reserved.

#pragma once

/////////////////////////////////////////////////////////////////////////////
// COutputList 창

class COutputList : public CListBox
{
// 생성입니다.
public:
	COutputList();

// 구현입니다.
public:
	virtual ~COutputList();

protected:
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnEditCopy();
	afx_msg void OnEditClear();
	afx_msg void OnViewOutput();

	DECLARE_MESSAGE_MAP()
};

class COutputWnd : public CDockablePane
{
// 생성입니다.
public:
	COutputWnd();

	void UpdateFonts();
  void AppendOutputString(LPTSTR msg);

// 특성입니다.
protected:
#if 0
	CMFCTabCtrl	m_wndTabs;

	COutputList m_wndOutputDebug;
	COutputList m_wndOutputFind;
#endif
  COutputList m_wndOutputBuild;

protected:
#if 0
  void FillBuildWindow();
	void FillDebugWindow();
	void FillFindWindow();
#endif

	void AdjustHorzScroll(CListBox& wndListBox);

// 구현입니다.
public:
	virtual ~COutputWnd();
  virtual BOOL LoadState(LPCTSTR lpszProfileName,int nIndex, UINT uiID);

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);

	DECLARE_MESSAGE_MAP()
};

