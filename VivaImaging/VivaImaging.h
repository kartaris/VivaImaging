// Copyright (C) 2019 K2Mobile
// All rights reserved.

// VivaImaging.h: VivaImaging 응용 프로그램의 기본 헤더 파일
//
#pragma once

#ifndef __AFXWIN_H__
	#error "PCH에 대해 이 파일을 포함하기 전에 'stdafx.h'를 포함합니다."
#endif

#include "resource.h"       // 주 기호입니다.

//. extern WORD mResourceLanguage;

// CVivaImagingApp:
// 이 클래스의 구현에 대해서는 VivaImaging.cpp을(를) 참조하세요.
//

class CVivaImagingApp : public CWinAppEx
{
public:
	CVivaImagingApp();


// 재정의입니다.
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

  void LoadDefaultSetting();
  void StoreDefaultSetting();

// 구현입니다.
	UINT  m_nAppLook;
	BOOL  m_bHiColorIcons;

	void InitApplicationsRegistry(LPCTSTR execPath);

  void InitShellRegistry(LPCTSTR execPath);

	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();
  virtual CDocument* OpenDocumentFile(LPCTSTR lpszFileName);
  virtual CDocument* OpenDocumentFile(LPCTSTR lpszFileName, BOOL bAddToMRU);
  afx_msg void OnFileNew();
  afx_msg void OnAppAbout();
  DECLARE_MESSAGE_MAP()
};

extern CVivaImagingApp theApp;
