// Copyright (C) 2019 K2Mobile
// All rights reserved.

// MainFrm.h: CMainFrame 클래스의 인터페이스
//

#pragma once
#include "FileView.h"
#include "ClassView.h"
#include "OutputWnd.h"
#include "PropertiesWnd.h"
#include "Core\KBgTask.h"

// flags on OnActivePageChanged
#define CHANGED_NUMBER_OF_PAGES 1
#define CHANGED_ACTIVE_PAGE 2
#define CHANGED_PAGE_NAME 4
#define UPDATE_ACTIVE_VIEW 8
#define INIT_VIEW 0x10
#define BEGIN_THUMB_RENDER 0x20

#define CHANGED_ALL 3

// timers
#define SYNC_TIMER_ID 2001
#define DELAYED_JOB_TIMER 2004

class KDlgLoadProgress;

typedef enum TagMainJobType
{
  JOB_NONE,
  JOB_OPEN_LOCAL_FOLDER,
  JOB_OPEN_SERVER_FOLDER,
  JOB_OPEN_SERVER_FILE,
  JOB_CHECK_UNFINISHED,
  JOB_UNKNOWN
}MainJobType;

class CMainFrame : public CFrameWndEx
{
	
protected: // serialization에서만 만들어집니다.
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// 특성입니다.
public:

// 작업입니다.
public:
  void UpdateRibbonBarUndoAction();

  void SetActivePageIndex(int pageIndex);
  void OnActivePageChanged(int flag);
  void UpdateThumbView(KIntVector& pageIdList, int flag);
  void UpdateThumbView(BOOL updateAll, int index, int flag);
  void AppendOutputString(LPTSTR msg);
  void OnPropertyChanged(LPCTSTR key, int value);
  void OnChangeDocType();
  void OnPropertyChanged(LPCTSTR key, LPCTSTR str);

  void OnPropertyModeChanged(PropertyMode mode);
  void UpdatePropertyMode();
  void OnPropertyValueChanged(PropertyMode mode, MouseState state, ImageEffectJob* effect);

  KPropertyCtrlBase* GetPropertyCtrl();

  // called from PropertyWindow
  void SetViewImageMode(PropertyMode mode);
  void UpdatePreview(ImageEffectJob* effect);
  void ApplyProperty(ImageEffectJob* effect);
  void CloseProperty(ImageEffectJob* effect);

// 재정의입니다.
public:
  virtual void ActivateFrame(int nCmdShow);
  virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// 구현입니다.
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

  void SetDelayedJob(MainJobType job, LPCTSTR name);
  void SetCommandOption(int option);
  void OnTaskStart(KBgTask* task);
  void OnTaskProgress(TaskProgressInfo* info);
  void OnTaskEnd(KBgTask* task);

  void StartSyncTimer(BOOL bStart);
  void CloseBgTask();

protected:  // 컨트롤 모음이 포함된 멤버입니다.
	CMFCRibbonBar     m_wndRibbonBar;
	CMFCRibbonApplicationButton m_MainButton;
	CMFCToolBarImages m_PanelImages;
	CMFCRibbonStatusBar  m_wndStatusBar;
  CMFCRibbonStatusBarPane* m_pane1;
  CMFCRibbonStatusBarPane* m_pane2;
  CFileView         m_wndFileView;
	//CClassView        m_wndClassView;
	COutputWnd        m_wndOutput;
	CPropertiesWnd    m_wndProperties;
  KDlgLoadProgress* mpProgreeDialog;

  UINT_PTR m_syncTimer;
  MainJobType m_MainJob;
  CString m_JobArg;
  UINT_PTR m_DelayedJobTimer;
  int mCommandOption;

// 생성된 메시지 맵 함수
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
  afx_msg void OnTimer(UINT_PTR nIDEvent);
  afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
  afx_msg void OnDropFiles(HDROP hDropInfo);
  afx_msg void OnClose();
  afx_msg BOOL OnQueryEndSession();
  afx_msg void OnEndSession(BOOL bEnding);
  afx_msg void OnApplicationLook(UINT id);
	afx_msg void OnUpdateApplicationLook(CCmdUI* pCmdUI);
  afx_msg void OnFilePrint();
	afx_msg void OnFilePrintPreview();
  afx_msg void OnUpdateFileServerEcm(CCmdUI* pCmdUI);
  afx_msg void OnFileServerOpenFolder();
  afx_msg void OnUpdateFileServerOpenFolder(CCmdUI* pCmdUI);
  afx_msg void OnFileServerOpenFile();
  afx_msg void OnUpdateFileServerOpenFile(CCmdUI* pCmdUI);
  afx_msg void OnFileServerSaveAs();
  afx_msg void OnUpdateFileServerSaveAs(CCmdUI* pCmdUI);
  afx_msg void OnFileSetting();
  afx_msg void OnUpdateFileSetting(CCmdUI* pCmdUI);
  afx_msg void OnFileAutoRename();
  afx_msg void OnUpdateAutoRename(CCmdUI* pCmdUI);
  afx_msg void OnEditEditMode();
  afx_msg void OnUpdateEditEditMode(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFilePrintPreview(CCmdUI* pCmdUI);
  afx_msg LRESULT OnShowRibbonItemMenu(WPARAM wp, LPARAM lp);
  afx_msg void OnViewOutputWindow();
  afx_msg void OnUpdateViewOutputWindow(CCmdUI* pCmdUI);
  afx_msg void OnViewPropertyWindow();
  afx_msg void OnUpdateViewPropertyWindow(CCmdUI* pCmdUI);
  afx_msg void OnViewThumbnailWindow();
  afx_msg void OnUpdateViewThumbnailWindow(CCmdUI* pCmdUI);
  afx_msg LRESULT OnSyncCommandMsg(WPARAM wParam, LPARAM lParam);
  afx_msg LRESULT OnThreadedCallMessage(WPARAM wParam, LPARAM lParam);
  afx_msg LRESULT OnCancelProgress(WPARAM wParam, LPARAM lParam);
  DECLARE_MESSAGE_MAP()

	BOOL CreateDockingWindows();
	void SetDockingWindowIcons(BOOL bHiColorIcons);
};


