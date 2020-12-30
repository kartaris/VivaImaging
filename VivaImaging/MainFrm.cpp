// Copyright (C) 2019 K2Mobile
// All rights reserved.

// MainFrm.cpp: CMainFrame 클래스의 구현
//

#include "stdafx.h"
#include "VivaImaging.h"

#include "MainFrm.h"
#include "VivaImagingView.h"
#include "Core\KConstant.h"
#include "Core\KSyncCoreEngine.h"
#include "Document\KImageBase.h"
#include "Platform\Graphics.h"
#include "Platform\Reg.h"

#include "CDlgSetting.h"
#include "KDlgLoadProgress.h"
#include "KDlgUploadAs.h"
#include "KDlgShowMessage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWndEx)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWndEx)
	ON_WM_CREATE()
  ON_WM_TIMER()
  ON_WM_SETTINGCHANGE()
  ON_WM_DROPFILES()
  ON_WM_CLOSE()
  ON_WM_QUERYENDSESSION()
  ON_WM_ENDSESSION()
  ON_COMMAND_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_WINDOWS_7, &CMainFrame::OnApplicationLook)
	ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_WINDOWS_7, &CMainFrame::OnUpdateApplicationLook)
  ON_COMMAND(ID_FILE_PRINT, &CMainFrame::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CMainFrame::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CMainFrame::OnFilePrintPreview)
	ON_UPDATE_COMMAND_UI(ID_FILE_PRINT_PREVIEW, &CMainFrame::OnUpdateFilePrintPreview)
  ON_UPDATE_COMMAND_UI(ID_FILE_SERVER_ECM, OnUpdateFileServerEcm)
  ON_COMMAND(ID_FILE_SERVER_OPEN_FOLDER, OnFileServerOpenFolder)
  ON_UPDATE_COMMAND_UI(ID_FILE_SERVER_OPEN_FOLDER, OnUpdateFileServerOpenFolder)
  ON_COMMAND(ID_FILE_SERVER_OPEN_FILE, OnFileServerOpenFile)
  ON_UPDATE_COMMAND_UI(ID_FILE_SERVER_OPEN_FILE, OnUpdateFileServerOpenFile)
  ON_COMMAND(ID_FILE_SERVER_SAVE_AS, OnFileServerSaveAs)
  ON_UPDATE_COMMAND_UI(ID_FILE_SERVER_SAVE_AS, OnUpdateFileServerSaveAs)
  ON_COMMAND(ID_FILE_SETTING, OnFileSetting)
  ON_UPDATE_COMMAND_UI(ID_FILE_SETTING, OnUpdateFileSetting)
  ON_COMMAND(ID_FILE_AUTO_RENAME, OnFileAutoRename)
  ON_UPDATE_COMMAND_UI(ID_FILE_AUTO_RENAME, OnUpdateAutoRename)
  ON_COMMAND(ID_EDIT_EDITMODE, OnEditEditMode)
  ON_UPDATE_COMMAND_UI(ID_EDIT_EDITMODE, OnUpdateEditEditMode)
  ON_REGISTERED_MESSAGE(AFX_WM_ON_BEFORE_SHOW_RIBBON_ITEM_MENU, OnShowRibbonItemMenu)
  ON_COMMAND(ID_VIEW_THUMBNAILWND, &CMainFrame::OnViewThumbnailWindow)
  ON_UPDATE_COMMAND_UI(ID_VIEW_THUMBNAILWND, &CMainFrame::OnUpdateViewThumbnailWindow)
  ON_COMMAND(ID_VIEW_OUTPUTWND, &CMainFrame::OnViewOutputWindow)
  ON_UPDATE_COMMAND_UI(ID_VIEW_OUTPUTWND, &CMainFrame::OnUpdateViewOutputWindow)
  ON_COMMAND(ID_VIEW_PROPERTYWND, &CMainFrame::OnViewPropertyWindow)
  ON_UPDATE_COMMAND_UI(ID_VIEW_PROPERTYWND, &CMainFrame::OnUpdateViewPropertyWindow)
  ON_MESSAGE(WM_SYNC_COMMAND_MESSAGE, OnSyncCommandMsg)
  ON_MESSAGE(THREADED_CALL_RESULT, OnThreadedCallMessage)
  ON_MESSAGE(WMA_CANCEL_PROGRESS, OnCancelProgress)
END_MESSAGE_MAP()

// CMainFrame 생성/소멸
KSyncCoreEngine* gpSyncCoreEngine = NULL;
int mTryCount = 0;

CMainFrame::CMainFrame()
{
  m_syncTimer = NULL;
  m_MainJob = JOB_NONE;
  mpProgreeDialog = NULL;

  m_DelayedJobTimer = NULL;
  mCommandOption = 0;// GO_READONLY;
	theApp.m_nAppLook = theApp.GetInt(_T("ApplicationLook"), ID_VIEW_APPLOOK_VS_2008);
}

CMainFrame::~CMainFrame()
{
  if (m_syncTimer != NULL)
  {
    KillTimer(m_syncTimer);
    m_syncTimer = NULL;
  }

  if (gpSyncCoreEngine != NULL)
  {
    delete gpSyncCoreEngine;
    gpSyncCoreEngine = NULL;
  }
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	BOOL bNameValid;

	m_wndRibbonBar.Create(this);
	m_wndRibbonBar.LoadFromResource(IDR_RIBBON);

  // add shapes RibbonGallery
  CMFCRibbonBaseElement* rbtn = m_wndRibbonBar.FindByID(ID_IMAGE_ANNOTATE_SHAPES);
  if ((rbtn != NULL) && (rbtn->GetRuntimeClass() == RUNTIME_CLASS(CMFCRibbonGallery)))
  {
    CMFCRibbonGallery* rgallery = (CMFCRibbonGallery*)rbtn;

    rgallery->SetButtonMode();
    rgallery->SetIconsInRow(12);

    //rgallery->AddGroup(_T("Recently Used Shapes"), IDB_SHAPE1, 20);
    //rgallery->AddGroup(_T("Lines"), IDB_SHAPE2, 20);
    rgallery->AddGroup(_T("Basic Shapes"), IDB_SHAPE3, 20);
    rgallery->AddGroup(_T("Block Arrows"), IDB_SHAPE4, 20);
    rgallery->AddGroup(_T("Flowchart"), IDB_SHAPE5, 20);
    rgallery->AddGroup(_T("Stars and Banners"), IDB_SHAPE7, 20);
    rgallery->AddGroup(_T("Callouts"), IDB_SHAPE6, 20);

  }

  /*
  if (mResourceLanguage != 0x412) // not KR
    UpdateRibbonBarText();
  */
	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("상태 표시줄을 만들지 못했습니다.\n");
		return -1;      // 만들지 못했습니다.
	}

	CString strTitlePane1;
	CString strTitlePane2;
	bNameValid = strTitlePane1.LoadString(IDS_STATUS_PANE1);
	ASSERT(bNameValid);
	bNameValid = strTitlePane2.LoadString(IDS_STATUS_PANE2);
	ASSERT(bNameValid);

  m_pane1 = new CMFCRibbonStatusBarPane(ID_STATUSBAR_PANE1, strTitlePane1, TRUE);
  m_pane1->SetAlmostLargeText(_T("Connecting to server"));
  m_wndStatusBar.AddElement(m_pane1, strTitlePane1);
  m_pane2 = new CMFCRibbonStatusBarPane(ID_STATUSBAR_PANE2, strTitlePane2, TRUE);
  m_pane2->SetAlmostLargeText(_T("Connecting"));
	m_wndStatusBar.AddExtendedElement(m_pane2, strTitlePane2);

	// Visual Studio 2005 스타일 도킹 창 동작을 활성화합니다.
	CDockingManager::SetDockingMode(DT_SMART);
	// Visual Studio 2005 스타일 도킹 창 자동 숨김 동작을 활성화합니다.
	EnableAutoHidePanes(CBRS_ALIGN_ANY);

	// 메뉴 항목 이미지를 로드합니다(표준 도구 모음에 없음).
	CMFCToolBar::AddToolBarForImageCollection(IDR_MENU_IMAGES, theApp.m_bHiColorIcons ? IDB_MENU_IMAGES_24 : 0);

	// 도킹 창을 만듭니다.
	if (!CreateDockingWindows())
	{
		TRACE0("도킹 창을 만들지 못했습니다.\n");
		return -1;
	}

	m_wndFileView.EnableDocking(CBRS_ALIGN_ANY);
	//m_wndClassView.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndFileView);
	CDockablePane* pTabbedBar = nullptr;
	//m_wndClassView.AttachToTabWnd(&m_wndFileView, DM_SHOW, TRUE, &pTabbedBar);
	m_wndOutput.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndOutput);
	m_wndProperties.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndProperties);

	// 보관된 값에 따라 비주얼 관리자 및 스타일을 설정합니다.
	OnApplicationLook(theApp.m_nAppLook);

  //ready timer for use schedule
  StartSyncTimer(TRUE);
	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWndEx::PreCreateWindow(cs) )
		return FALSE;
	//  Window 클래스 또는 스타일을 수정합니다.
  cs.style = WS_OVERLAPPED | WS_CAPTION | FWS_ADDTOTITLE
    | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU | WS_SIZEBOX;

	return TRUE;
}

BOOL CMainFrame::CreateDockingWindows()
{
	BOOL bNameValid;

	// 클래스 뷰를 만듭니다.
#if 0
	CString strClassView;
	bNameValid = strClassView.LoadString(IDS_CLASS_VIEW);
	ASSERT(bNameValid);
	if (!m_wndClassView.Create(strClassView, this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_CLASSVIEW, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT | CBRS_FLOAT_MULTI))
	{
		TRACE0("클래스 뷰 창을 만들지 못했습니다.\n");
		return FALSE; // 만들지 못했습니다.
	}
#endif

	// 파일 뷰를 만듭니다.
	CString strFileView;
	bNameValid = strFileView.LoadString(AfxGetInstanceHandle(), IDS_THUMBNAIL_VIEW, mResourceLanguage);
	ASSERT(bNameValid);
	if (!m_wndFileView.Create(strFileView, this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_FILEVIEW, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT| CBRS_FLOAT_MULTI))
	{
		TRACE0("파일 뷰 창을 만들지 못했습니다.\n");
		return FALSE; // 만들지 못했습니다.
	}

  CSize minSize(DialogX2Pixel(GetSafeHwnd(), 20), 0);
  m_wndFileView.SetMinSize(minSize);

	// 출력 창을 만듭니다.
	CString strOutputWnd;
	bNameValid = strOutputWnd.LoadString(AfxGetInstanceHandle(), IDS_OUTPUT_WND, mResourceLanguage);
	ASSERT(bNameValid);
	if (!m_wndOutput.Create(strOutputWnd, this, CRect(0, 0, 100, 100), TRUE, ID_VIEW_OUTPUTWND, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_BOTTOM | CBRS_FLOAT_MULTI))
	{
		TRACE0("출력 창을 만들지 못했습니다.\n");
		return FALSE; // 만들지 못했습니다.
	}

	// 속성 창을 만듭니다.
	CString strPropertiesWnd;
	bNameValid = strPropertiesWnd.LoadString(AfxGetInstanceHandle(), IDS_PROPERTIES_WND, mResourceLanguage);
	ASSERT(bNameValid);
	if (!m_wndProperties.Create(strPropertiesWnd, this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_PROPERTIESWND, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_RIGHT | CBRS_FLOAT_MULTI))
	{
		TRACE0("속성 창을 만들지 못했습니다.\n");
		return FALSE; // 만들지 못했습니다.
	}

	SetDockingWindowIcons(theApp.m_bHiColorIcons);
	return TRUE;
}

void CMainFrame::SetDockingWindowIcons(BOOL bHiColorIcons)
{
	HICON hFileViewIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_FILE_VIEW_HC : IDI_FILE_VIEW), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	m_wndFileView.SetIcon(hFileViewIcon, FALSE);

#if 0
	HICON hClassViewIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_CLASS_VIEW_HC : IDI_CLASS_VIEW), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	m_wndClassView.SetIcon(hClassViewIcon, FALSE);
#endif

	HICON hOutputBarIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_OUTPUT_WND_HC : IDI_OUTPUT_WND), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	m_wndOutput.SetIcon(hOutputBarIcon, FALSE);

	HICON hPropertiesBarIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_PROPERTIES_WND_HC : IDI_PROPERTIES_WND), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	m_wndProperties.SetIcon(hPropertiesBarIcon, FALSE);
}

// CMainFrame 진단

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWndEx::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWndEx::Dump(dc);
}
#endif //_DEBUG

LRESULT CMainFrame::OnShowRibbonItemMenu(WPARAM wp, LPARAM lp)
{
  CMFCRibbonBaseElement* pElem = (CMFCRibbonBaseElement*)lp;
  CVivaImagingDoc* doc = (CVivaImagingDoc*)GetActiveDocument();
  KActionList* actionManager = NULL;
  if (doc != NULL)
    actionManager = doc->GetActionManager();

  if ((pElem == NULL) || (doc == NULL))
    return 0;

  switch (pElem->GetID())
  {
    case ID_EDIT_UNDO:
    {
      CMFCRibbonUndoButton* undoButton = DYNAMIC_DOWNCAST(CMFCRibbonUndoButton, pElem);
      if ((undoButton != NULL) && (actionManager != NULL))
      {
        CString name;
        int count = undoButton->GetActionNumber();
        undoButton->CleanUpUndoList();
        count = actionManager->GetUndoCount();
        for (int i = count-1; i >= 0; i--)
        {
          KEditAction* a = actionManager->GetAction(i);
          if (a != NULL)
          {
            int idx = (int)a->GetType();
            name.LoadString(AfxGetInstanceHandle(), IDS_ACT_INSERT_PAGE + idx, mResourceLanguage);

            undoButton->AddUndoAction(name);
          }
        }
      }
    }
  }
  return 0;
}

void CMainFrame::UpdateRibbonBarUndoAction()
{
#if 0
  CMFCRibbonCategory* c;

  LPCTSTR categoryNames[] = {
    _T("Preview"),
    _T("Home"),
    _T("Image"),
  };

  int count = m_wndRibbonBar.GetCategoryCount();
  for (int i = 0; i < count; i++)
  {
    c = m_wndRibbonBar.GetCategory(i);
    if (c)
    {
      LPCTSTR name  = c->GetName();
      c->SetName(categoryNames[i]);
      name = c->GetName();
    }
  }

  CMFCRibbonBaseElement* e;
  //e = m_wndRibbonBar.FindByID(UINT uiCmdID, FALSE, FALSE);
#endif
}

// CMainFrame 메시지 처리기
void CMainFrame::SetDelayedJob(MainJobType job, LPCTSTR name)
{
  m_MainJob = job;
  m_JobArg = name;
  /*
  if (m_DelayedJobTimer != NULL)
    m_DelayedJobTimer = SetTimer(DELAYED_JOB_TIMER, 100, NULL);
  */
}

void CMainFrame::SetCommandOption(int option)
{
  mCommandOption = option;

  // initial option is readonly, not-editable
  mGlobalOption = mCommandOption & ~GO_EDITABLE;
  mGlobalOption |= GO_READONLY;
}

void CMainFrame::OnTimer(UINT_PTR nIDEvent)
{
  if (gpSyncCoreEngine == NULL)
  {
    CString str;
    str.LoadString(IDS_READY);
    m_pane1->SetText(str);
    str.LoadString(IDS_OFFLINE);
    m_pane2->SetText(str);

    gpSyncCoreEngine = new KSyncCoreEngine();
  }

  int status = gpSyncCoreEngine->GetSyncStatus();
  if (status == SYNC_STATUS_INIT)
  {
    mTryCount = 0;

    if (!gpSyncCoreEngine->Ready(TRUE))
    {
      CString str;
      str.LoadString(AfxGetInstanceHandle(), IDS_UNAVAILABLE_SESSION, mResourceLanguage);
      AfxMessageBox(str);

      DestroyWindow();
      return;
    }

    status = gpSyncCoreEngine->GetSyncStatus();
  }

  mTryCount++;
  if (status < SYNC_STATUS_CONNECTED)
  {
    if (!gpSyncCoreEngine->CheckAutoLogin(TRUE)) // 로그인 확인
    {
      // 로그인 안된 상태이면 quit
      if (mTryCount == 1)
      {
        AfxMessageBox(IDS_NO_LOGIN_QUIT, MB_OK);

        // DestroyWindow();
        mGlobalOption = mCommandOption;

        StoreLogHistory(_T(__FUNCTION__), _T("Quit by NoLogin"), SYNC_MSG_LOG);
      }
    }

    // remaining will be done by SYNC_CMD_LOGIN
    StartSyncTimer(FALSE);
    return;
  }

  if (status == SYNC_STATUS_CONNECTED)
  {
    // 로그인된 상태이면 바로 준비완료됨.
    //gpSyncCoreEngine->InitializeServerSetting(2, 0);
    StartSyncTimer(FALSE);

    gpCoreEngine->OnLogged(TRUE);
    gpSyncCoreEngine->SetSyncStatus(SYNC_STATUS_IDLE);
    status = SYNC_STATUS_IDLE;

    CString str;
    str.LoadString(IDS_ONLINE);
    m_pane2->SetText(str);
  }

  if (status == SYNC_STATUS_IDLE)
  {
    CVivaImagingView* view = (CVivaImagingView*)GetActiveView();
    if (m_MainJob == JOB_OPEN_LOCAL_FOLDER)
    {
      view->OpenLocalFolder(m_JobArg);
    }
    else if (m_MainJob == JOB_OPEN_SERVER_FOLDER)
    {
      view->OpenServerFolder(m_JobArg);
    }
    else if (m_MainJob == JOB_OPEN_SERVER_FILE)
    {
      view->OpenServerFile(m_JobArg);
    }
    else if (m_MainJob == JOB_CHECK_UNFINISHED)
    {
      CString filename;

      LoadRegistryValue(strLastUnfinishedDocument, filename);
      if (filename.GetLength() > 0)
      {
        TCHAR folderOID[MAX_OID] = _T("");
        LPTSTR p = StrChr(filename, '|');
        if (p != NULL)
        {
          p++;
          StringCchCopy(folderOID, MAX_OID, p);
          p--;
          *p = '\0';
        }

        if (IsFileExist(filename) && (lstrlen(folderOID) > 0))
        {
          CString msg;
          msg.LoadString(AfxGetInstanceHandle(), IDS_CONFIRM_LAST_UPLOAD, mResourceLanguage);
          msg += _T("\n");
          msg += filename;

          // clear stored info anyway
          StoreRegistryValue(strLastUnfinishedDocument, filename);

          if (AfxMessageBox(msg, MB_RETRYCANCEL) == IDRETRY)
          {
            view->OpenAndUploadLocalFile(filename, folderOID);
            return;
          }
        }
        else
        {
          TCHAR msg[300] = _T("Not found : ");
          StringCchCat(msg, 300, filename);
          StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG | SYNC_MSG_DEBUG);
        }
      }

      mGlobalOption = mCommandOption;
      // start new document on secure disk
      CVivaImagingDoc* doc = (CVivaImagingDoc*)GetActiveDocument();
      if (doc->IsNewDocument() && (mGlobalOption & GO_TEMPFOLDERONSECUREDISK))
        doc->OnNewDocument();
    }

    m_MainJob = JOB_NONE;
  }
}

void CMainFrame::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
{
  CFrameWndEx::OnSettingChange(uFlags, lpszSection);
  m_wndOutput.UpdateFonts();
}

void CMainFrame::OnDropFiles(HDROP hdrop)
{
  CFrameWndEx::OnDropFiles(hdrop);
}

void CMainFrame::OnClose()
{
  CloseBgTask();
  CFrameWndEx::OnClose();
}

BOOL CMainFrame::OnQueryEndSession()
{
  // file save
  // OnFileSave();
  return TRUE;
}

void CMainFrame::OnEndSession(BOOL bEnding)
{
  /*
  RECT rect;
  GetWindowRect(&rect);
  StoreDialogLocation(main_window_name, rect);
  */

  // no need DestroyWindow();
}

void CMainFrame::OnTaskStart(KBgTask* task)
{
  BgTaskType type = task->GetType();

  if (type < BGTASK_EFFECT_RENDER)
  {
    // task 진행중에는 READONLY
    mGlobalOption &= ~GO_EDITABLE;
    mGlobalOption |= GO_READONLY;
  }

  if (type == BGTASK_UPLOAD)
  {
    CString str;
    str.LoadString(IDS_UPLOADING);
    m_pane1->SetText(str);
    if (mpProgreeDialog != NULL)
      delete mpProgreeDialog;

    mpProgreeDialog = new KDlgLoadProgress(this);
    mpProgreeDialog->ShowWindow(SW_SHOW);
  }
  else if (type == BGTASK_FOLDER_DOWNLOAD)
  {
    CString str;
    str.LoadString(IDS_DOWNLOADING);
    m_pane1->SetText(str);
  }
}

void CMainFrame::OnTaskProgress(TaskProgressInfo* info)
{
  if (mpProgreeDialog != NULL)
  {
    mpProgreeDialog->SetProgress(info->doneFolders, info->totalFolders, info->doneFiles, info->totalFiles, info->hErrorMsg);
  }
}

void CMainFrame::OnTaskEnd(KBgTask* task)
{
  BgTaskType type = task->GetType();

  switch (type)
  {
  case BGTASK_LOCAL_SCAN:
  case BGTASK_FOLDER_DOWNLOAD:
  case BGTASK_UPLOAD:
    mGlobalOption = mCommandOption;
    break;
  }

  if (mpProgreeDialog != NULL)
  {
    mpProgreeDialog->DestroyWindow();
    delete mpProgreeDialog;
    mpProgreeDialog = NULL;
  }

  m_pane1->SetText(_T("Ready"));

  if (task->mError.GetLength() > 0)
  {
    CString label;
    CString sub;
    label.LoadString(IDS_TASK_NAME1 + type);
    sub += _T(" : ");
    sub.LoadString(IDS_TASK_ERROR_OCCURRED);
    label += sub;

    KDlgShowMessage messagebox(this, label, task->mError);
    messagebox.DoModal();
  }
}

void CMainFrame::OnApplicationLook(UINT id)
{
	CWaitCursor wait;

	theApp.m_nAppLook = id;
  CMFCVisualManager* pVisual = NULL;

	switch (theApp.m_nAppLook)
	{
	case ID_VIEW_APPLOOK_WIN_2000:
    pVisual = (CMFCVisualManager*)RUNTIME_CLASS(CMFCVisualManager);
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManager));
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_OFF_XP:
    pVisual = (CMFCVisualManager*)RUNTIME_CLASS(CMFCVisualManagerOfficeXP);
    CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOfficeXP));
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_WIN_XP:
		CMFCVisualManagerWindows::m_b3DTabsXPTheme = TRUE;
    pVisual = (CMFCVisualManager*)RUNTIME_CLASS(CMFCVisualManagerWindows);
    CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_OFF_2003:
    pVisual = (CMFCVisualManager*)RUNTIME_CLASS(CMFCVisualManagerOffice2003);
    CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2003));
		CDockingManager::SetDockingMode(DT_SMART);
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_VS_2005:
    pVisual = (CMFCVisualManager*)RUNTIME_CLASS(CMFCVisualManagerVS2005);
    CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2005));
		CDockingManager::SetDockingMode(DT_SMART);
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_VS_2008:
    pVisual = (CMFCVisualManager*)RUNTIME_CLASS(CMFCVisualManagerVS2008);
    CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2008));
		CDockingManager::SetDockingMode(DT_SMART);
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_WINDOWS_7:
    pVisual = (CMFCVisualManager*)RUNTIME_CLASS(CMFCVisualManagerWindows7);
    CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows7));
		CDockingManager::SetDockingMode(DT_SMART);
		m_wndRibbonBar.SetWindows7Look(TRUE);
		break;

	default:
		switch (theApp.m_nAppLook)
		{
		case ID_VIEW_APPLOOK_OFF_2007_BLUE:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_LunaBlue);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_BLACK:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_ObsidianBlack);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_SILVER:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Silver);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_AQUA:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Aqua);
			break;
		}

    pVisual = (CMFCVisualManager*)RUNTIME_CLASS(CMFCVisualManagerOffice2007);
    CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2007));
		CDockingManager::SetDockingMode(DT_SMART);
		m_wndRibbonBar.SetWindows7Look(FALSE);
	}

	m_wndOutput.UpdateFonts();
	RedrawWindow(nullptr, nullptr, RDW_ALLCHILDREN | RDW_INVALIDATE | RDW_UPDATENOW | RDW_FRAME | RDW_ERASE);

	theApp.WriteInt(_T("ApplicationLook"), theApp.m_nAppLook);
}

void CMainFrame::OnUpdateApplicationLook(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio(theApp.m_nAppLook == pCmdUI->m_nID);
}

void CMainFrame::OnFilePrint()
{
	if (IsPrintPreview())
	{
		PostMessage(WM_COMMAND, AFX_ID_PREVIEW_PRINT);
	}
}

void CMainFrame::OnFilePrintPreview()
{
	if (IsPrintPreview())
	{
		PostMessage(WM_COMMAND, AFX_ID_PREVIEW_CLOSE);  // 인쇄 미리 보기 모드를 닫습니다.
	}
}

void CMainFrame::OnUpdateFilePrintPreview(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(IsPrintPreview());
}


void CMainFrame::OnUpdateFileServerEcm(CCmdUI* pCmdUI)
{
  pCmdUI->Enable(gpCoreEngine->IsECMDiskReadable());
}

void CMainFrame::OnFileServerOpenFolder()
{
  POSITION p = AfxGetApp()->GetFirstDocTemplatePosition();
  CDocTemplate* docTemp = AfxGetApp()->GetNextDocTemplate(p);
  if (docTemp != NULL)
  {
    if (docTemp->OpenDocumentFile(NULL) != NULL)
    {
      CVivaImagingView* view = (CVivaImagingView*)GetActiveView();
      view->DoFileOpenServerFolder();
      /*
      LPCTSTR keyServerOpenFolder = _T("ServerOpenFolder");
      TCHAR folderOID[MAX_OID] = _T("");
      if (gpSyncCoreEngine->SelectSyncServerFolderLocation(GetSafeHwnd(), keyServerOpenFolder, folderOID))
      {
        CloseBgTask();
        view->OpenServerFolder(folderOID);
      }
      */
    }
  }
}

void CMainFrame::OnUpdateFileServerOpenFolder(CCmdUI* pCmdUI)
{
  pCmdUI->Enable(gpCoreEngine->IsECMDiskReadable());
}

void CMainFrame::OnFileServerOpenFile()
{
  POSITION p = AfxGetApp()->GetFirstDocTemplatePosition();
  CDocTemplate* docTemp = AfxGetApp()->GetNextDocTemplate(p);
  if (docTemp != NULL)
  {
    if (docTemp->OpenDocumentFile(NULL) != NULL)
    {
      CVivaImagingView* view = (CVivaImagingView*)GetActiveView();
      view->DoFileOpenServerFile();
    }
  }
}

void CMainFrame::OnUpdateFileServerOpenFile(CCmdUI* pCmdUI)
{
  pCmdUI->Enable(gpCoreEngine->IsECMDiskReadable());
}

void CMainFrame::OnFileServerSaveAs()
{
  CVivaImagingDoc* doc = (CVivaImagingDoc*)GetActiveDocument();
  CString pathname = doc->GetPathName();
  LPTSTR n = StrRChr(pathname, NULL, '\\');
  CString name;

  if (n != NULL)
    name.SetString(++n);
  else
    name = pathname;

  KDlgUploadAs dlg(this);

  dlg.SetUploadInfos(doc->GetServerUploadFolder(), 
    doc->GetWorkingPath(), name, doc->GetCurrentFormat(), mGlobalOption);

  if (dlg.DoModal() == IDOK)
  {
    CVivaImagingView* v = (CVivaImagingView*)GetActiveView();

    v->DoFileSaveAs(dlg.mServerFolderOID, dlg.mFilename, dlg.mFormat, dlg.mFlag);
  }
}

void CMainFrame::OnUpdateFileServerSaveAs(CCmdUI* pCmdUI)
{
  CVivaImagingDoc* doc = (CVivaImagingDoc*)GetActiveDocument();
  pCmdUI->Enable(doc->IsEditable() && gpCoreEngine->IsECMDiskWritable());
}

void CMainFrame::OnFileSetting()
{
  CDlgSetting dlg(this);

  dlg.mStrUploadFolder.SetString(mDefaultUploadFolderPath);
  dlg.mStrUploadFolderOID.SetString(mDefaultUploadFolderOID);
  dlg.mStrWorkingFolder.SetString(mDefaultWorkingBaseFolder);

  dlg.mStrNewFilenameFormat.SetString(gImageFilenameFormat);
  dlg.mUploadFileType = gSaveFormat;
  dlg.mMergeAnnotatesToImage = !!(mGlobalOption & GO_MERGEANNOTATE);
  dlg.mChangeSelectAfterDraw = !!(mGlobalOption & GO_AUTOCHANGESELECT);

  if (dlg.DoModal())
  {
    StringCchCopy(mDefaultUploadFolderPath, MAX_PATH, dlg.mStrUploadFolder);
    StringCchCopy(mDefaultUploadFolderOID, 16, dlg.mStrUploadFolderOID);
    StringCchCopy(mDefaultWorkingBaseFolder, MAX_PATH, dlg.mStrWorkingFolder);

    if (gImageFilenameFormat != NULL)
      delete[] gImageFilenameFormat;
    gImageFilenameFormat = AllocString(dlg.mStrNewFilenameFormat);

    gSaveFormat = (ImagingSaveFormat)dlg.mUploadFileType;

    if (dlg.mMergeAnnotatesToImage)
      mGlobalOption |= GO_MERGEANNOTATE;
    else
      mGlobalOption &= ~GO_MERGEANNOTATE;

    if (dlg.mChangeSelectAfterDraw)
      mGlobalOption |= GO_AUTOCHANGESELECT;
    else
      mGlobalOption &= ~GO_AUTOCHANGESELECT;

    CWinApp* app = AfxGetApp();
    ((CVivaImagingApp*) app)->StoreDefaultSetting();
  }
}

void CMainFrame::OnUpdateFileSetting(CCmdUI* pCmdUI)
{
  pCmdUI->Enable(TRUE);
}

void CMainFrame::OnFileAutoRename()
{
  CVivaImagingDoc* doc = (CVivaImagingDoc*)GetActiveDocument();
  if (doc->ImageAutoRename() > 0)
    m_wndProperties.InitProperty(doc);
}

void CMainFrame::OnUpdateAutoRename(CCmdUI* pCmdUI)
{
  CVivaImagingDoc* doc = (CVivaImagingDoc*)GetActiveDocument();
  pCmdUI->Enable(doc->IsEditable()); //  mGlobalOption & GO_EDITABLE);
}

void CMainFrame::OnEditEditMode()
{
  if (mGlobalOption & GO_EDITABLE)
    mGlobalOption &= ~GO_READONLY;
}

void CMainFrame::OnUpdateEditEditMode(CCmdUI* pCmdUI)
{
  pCmdUI->Enable((mGlobalOption & (GO_READONLY|GO_EDITABLE)) == (GO_READONLY | GO_EDITABLE));
  pCmdUI->SetCheck(!(mGlobalOption & GO_READONLY));
}

void CMainFrame::OnViewOutputWindow()
{
  // 현재 상태에 따라 창을 표시하거나 활성화합니다.
  // 창을 닫으려면 창 프레임의 [x] 단추를 사용해야 합니다.
  BOOL isVisible = m_wndOutput.IsVisible();
  m_wndOutput.ShowPane(!isVisible, FALSE, TRUE);
  //m_wndOutput.SetFocus();
}

void CMainFrame::OnUpdateViewOutputWindow(CCmdUI* pCmdUI)
{
  pCmdUI->Enable(TRUE);
  pCmdUI->SetCheck(m_wndOutput.IsVisible());
}

void CMainFrame::OnViewPropertyWindow()
{
  // 현재 상태에 따라 창을 표시하거나 활성화합니다.
  // 창을 닫으려면 창 프레임의 [x] 단추를 사용해야 합니다.
  //m_wndProperties.ShowPane(TRUE, FALSE, TRUE);
  BOOL isVisible = m_wndProperties.IsVisible();
  m_wndProperties.ShowPane(!isVisible, FALSE, TRUE);

  m_wndProperties.SetFocus();
}

void CMainFrame::OnUpdateViewPropertyWindow(CCmdUI* pCmdUI)
{
  pCmdUI->Enable(TRUE);
  pCmdUI->SetCheck(m_wndProperties.IsVisible());
}

void CMainFrame::OnViewThumbnailWindow()
{
  BOOL isVisible = m_wndFileView.IsVisible();
  m_wndFileView.ShowPane(!isVisible, FALSE, TRUE);
}
void CMainFrame::OnUpdateViewThumbnailWindow(CCmdUI* pCmdUI)
{
  pCmdUI->Enable(TRUE);
  pCmdUI->SetCheck(m_wndFileView.IsVisible());
}

void CMainFrame::ActivateFrame(int nCmdShow)
{
  CFrameWndEx::ActivateFrame(nCmdShow);
  SetActivePageIndex(0);

  //m_wndFileView.OnActivePageChanged(INIT_VIEW| UPDATE_ACTIVE_VIEW|BEGIN_THUMB_RENDER);
  OnActivePageChanged(INIT_VIEW | UPDATE_ACTIVE_VIEW | CHANGED_ACTIVE_PAGE | BEGIN_THUMB_RENDER);
}

void CMainFrame::AppendOutputString(LPTSTR msg)
{
  m_wndOutput.AppendOutputString(msg);
}

void CMainFrame::SetActivePageIndex(int pageIndex)
{
  CVivaImagingView* v = (CVivaImagingView*)GetActiveView();
  if ((pageIndex >= 0) && v->SetActivePageIndex(pageIndex))
    OnActivePageChanged(CHANGED_ACTIVE_PAGE);
}

void CMainFrame::OnActivePageChanged(int flag)
{
  BOOL bVisible = !(mCommandOption & GO_READONLY);
  if (m_wndOutput.IsVisible() != bVisible)
  {
    m_wndOutput.ShowPane(bVisible, bVisible, bVisible);
    RecalcLayout();
  }

  // 속성창은 readonly에서도 보이도록 함.
  /*
  if (m_wndProperties.IsVisible() != bVisible)
    m_wndProperties.ShowPane(bVisible, bVisible, bVisible);
  */

  m_wndFileView.OnActivePageChanged(flag);

  if (flag & CHANGED_ACTIVE_PAGE)
  {
    CVivaImagingDoc* doc = (CVivaImagingDoc*)GetActiveDocument();
    int w = 0;
    int h = 0;
    int c = 0;
    LPCTSTR name = NULL;
    if (doc != NULL)
    {
      // 폴더에서는 강제로 이미지 뷰 모드로 전환한다.
      KImageBase* p = doc->GetImagePage(EXCLUDE_FOLDED, -1);
      BOOL isImagePage = (p != NULL) && (p->GetType() == IMAGE_PAGE);
      PropertyMode proposed = PropertyMode::PModeImageView;
      if (!isImagePage)
      {
        proposed = PropertyMode::PModeImageView;
      }
      else
      {
        CVivaImagingView* v = (CVivaImagingView*)GetActiveView();
        proposed = v->GetImageMode();
      }
      m_wndProperties.SetPropertyMode(proposed);

      m_wndProperties.InitProperty(doc);
      /*
      doc->GetCurrentPageInfo(-1, w, h, c, &name);
      m_wndProperties.UpdateImageInfo(w, h, c, name);
      */
    }
  }

  if (flag & UPDATE_ACTIVE_VIEW)
  {
    CVivaImagingView* v = (CVivaImagingView*)GetActiveView();
    if (v != NULL)
    {
      v->SetImageMode(PropertyMode::PModeImageView);
      v->OnActivePageChanged(flag);
    }
  }
}

void CMainFrame::UpdateThumbView(KIntVector& pageIdList, int flag)
{
  m_wndFileView.UpdateThumbView(pageIdList, flag);
}
void CMainFrame::UpdateThumbView(BOOL updateAll, int index, int flag)
{
  m_wndFileView.UpdateThumbView(updateAll, index, flag);
}

void CMainFrame::OnPropertyChanged(LPCTSTR key, int value)
{

}

void CMainFrame::OnChangeDocType()
{
  CVivaImagingView* view = (CVivaImagingView*)GetActiveView();
  view->OnFileServerSetDocType();
}

void CMainFrame::OnPropertyChanged(LPCTSTR key, LPCTSTR str)
{
  CVivaImagingView* view = (CVivaImagingView*)GetActiveView();
  int n = view->PropertyChange(key, str);
  /*
  CVivaImagingDoc* doc = (CVivaImagingDoc*)GetActiveDocument();
  int n = doc->PropertyChange(key, str);
  */

  if (n & CHANGED_PAGE_NAME)
  {
    // redraw thumb window
    // 
    // OnActivePageChanged(CHANGED_ACTIVE_PAGE);
    OnActivePageChanged(CHANGED_PAGE_NAME);
  }
  OnActivePageChanged(n);
}

void CMainFrame::OnPropertyModeChanged(PropertyMode mode)
{
  m_wndProperties.SetPropertyMode(mode);
  if (mode > PropertyMode::PModeImageView)
  {
    CVivaImagingDoc* doc = (CVivaImagingDoc*)GetActiveDocument();
    //KImageBase* p = doc->GetImagePage(EXCLUDE_FOLDED, -1);
    m_wndProperties.InitProperty(doc);
  }
}

void CMainFrame::UpdatePropertyMode()
{
  CVivaImagingDoc* doc = (CVivaImagingDoc*)GetActiveDocument();
  //KImageBase* p = doc->GetImagePage(EXCLUDE_FOLDED, -1);
  m_wndProperties.InitProperty(doc);
}

void CMainFrame::OnPropertyValueChanged(PropertyMode mode, MouseState state, ImageEffectJob* effect)
{
  m_wndProperties.OnPropertyValueChanged(mode, state, effect);
}

KPropertyCtrlBase* CMainFrame::GetPropertyCtrl()
{
  return m_wndProperties.GetPropertyCtrl();
}

void CMainFrame::SetViewImageMode(PropertyMode mode)
{
  CVivaImagingView* v = (CVivaImagingView*)GetActiveView();
  v->SetImageMode(mode);
}

void CMainFrame::UpdatePreview(ImageEffectJob* effect)
{
  CVivaImagingView* v = (CVivaImagingView*)GetActiveView();
  if (v != NULL)
  {
    v->UpdatePreview(effect);
  }
}

void CMainFrame::ApplyProperty(ImageEffectJob* effect)
{
  CVivaImagingView* v = (CVivaImagingView*)GetActiveView();
  if (v != NULL)
  {
    if (IsSelectShapeMode(effect->job))
    // if ((effect->job == PModeSelectRectangle) || (effect->job == PModeSelectEllipse))
    {
      DWORD fillcolor = 0;
      KPropertyCtrlBase* prop = GetPropertyCtrl();

      effect->job = m_wndProperties.GetPropertyMode();

      if (prop->GetFillColor(fillcolor))
      {
        effect->param[5] = fillcolor;
        v->FillSelectedArea(effect);
      }
    }
    else if (effect->job == PropertyMode::PModeCopyRectangle)
    {
      effect->job = m_wndProperties.GetPropertyMode();
      v->CopySelectedArea(effect);
    }
    else if (effect->job == PropertyMode::PModePasteImage)
    {
      effect->job = m_wndProperties.GetPropertyMode();
      v->DoEditPaste();
    }
    else if (effect->job == PropertyMode::PModeAnnotateSelect)
    {
      v->ShapeEditCommand(effect->channel, effect->param, effect->extra);
    }
    else
    {
      v->ApplyEffect(effect);

      // reset property
      KPropertyCtrlBase* prop = GetPropertyCtrl();
      CVivaImagingDoc* doc = (CVivaImagingDoc*)GetActiveDocument();
      //KImageBase* p = doc->GetImagePage(EXCLUDE_FOLDED, -1);
      prop->InitProperty(doc);
    }
  }
}

void CMainFrame::CloseProperty(ImageEffectJob* effect)
{
  CVivaImagingView* v = (CVivaImagingView*)GetActiveView();
  if (v != NULL)
  {
    if ((effect->job == PropertyMode::PModeSelectRectangle) || (effect->job == PropertyMode::PModeSelectEllipse))
    {
      v->CloseProperty(effect);
    }
  }
}

LRESULT CMainFrame::OnSyncCommandMsg(WPARAM wParam, LPARAM lParam)
{
  switch (wParam)
  {
    case SYNC_CMD_LOGIN:
    {
      if (gpCoreEngine->mStatus < SYNC_STATUS_CONNECTED)
      {
        gpCoreEngine->InitializeServerSetting(2, FALSE);

        // 보안 디스크 초기화 작업이 끝난 다음에 백업 작업이 시작되도록 함.
        if ((gpCoreEngine->mStatus == SYNC_STATUS_CONNECTED) &&
          gpCoreEngine->IsSecurityDiskReady())
        {
          gpCoreEngine->OnLogged(TRUE);
          StartSyncTimer(TRUE);
        }
        else
        {
          TCHAR msg[128];
          StringCchPrintf(msg, 128, _T("wait until SecurityDisk ready, status=%d"), gpCoreEngine->mStatus);
          StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG | SYNC_MSG_DEBUG);
        }
      }
      break;
    }

    case SYNC_CMD_DRIVE_READY:
    {
      TCHAR msg[128];
      StringCchPrintf(msg, 128, _T("CMD_DRIVE_READY, status=%d"), gpCoreEngine->mStatus);
      StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG | SYNC_MSG_DEBUG);
      if (gpCoreEngine->mStatus < SYNC_STATUS_LOGGED)
      {
        // direct process without waiting
        if (gpCoreEngine->mStatus < SYNC_STATUS_CONNECTED)
        {
          gpCoreEngine->InitializeServerSetting(2, FALSE);
        }

        if (gpCoreEngine->mStatus == SYNC_STATUS_CONNECTED)
        {
          gpCoreEngine->OnLogged(TRUE);
          StartSyncTimer(TRUE);
        }
      }
      return 0;
    }

    case SYNC_CMD_LOGOUT:
    {
      if (lParam != LOGOUT_BY_TIMEOUT)
      {
        // local-login or logout 진행중인 작업 중지
        gpCoreEngine->OnLogged(FALSE);
        gpCoreEngine->InitializeServerSetting(0, (lParam == 0) ? TRUE : FALSE);
      }
      else
      {
        TCHAR msg[126] = _T("LOGOUT");
        if (gpCoreEngine->GetSyncStatus() >= SYNC_STATUS_LOGGED)
        {
          StringCchCat(msg, 126, _T(" by timeout"));
          gpCoreEngine->SetSyncStatus(SYNC_STATUS_STOP);
        }
        else
        {
          StringCchCat(msg, 126, _T(" ignored"));
        }
        StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
        StartSyncTimer(FALSE); // 로그인될때까지 timer만 종료
      }
      lParam = 0;
      return 0;
    }
  }
  return 0;
}

LRESULT CMainFrame::OnThreadedCallMessage(WPARAM wParam, LPARAM lParam)
{
  /*
  if (wParam == SELECT_FOLDER)
  {
    THREAD_CALL_DATA *cd = (THREAD_CALL_DATA *)lParam;

    if (cd != NULL)
    {
      if ((cd->local_path != NULL) && (lstrcmp(keyServerOpenFolder, cd->local_path) == 0))
      {
        CVivaImagingView* view = (CVivaImagingView*)GetActiveView();
        view->OpenServerFolder(cd->folder_oid);
      }
    }

    if (lParam != NULL)
      free((void *)lParam);
  }

  */
  return 0;
}

void CMainFrame::StartSyncTimer(BOOL bStart)
{
  if (bStart)
  {
    if (m_syncTimer == NULL)
      m_syncTimer = SetTimer(SYNC_TIMER_ID, 50, NULL);
  }
  else
  {
    if (m_syncTimer != NULL)
    {
      KillTimer(m_syncTimer);
      m_syncTimer = NULL;
    }
  }
}

void CMainFrame::CloseBgTask()
{
  if (GetSafeHwnd())
  {
    CVivaImagingView* view = (CVivaImagingView*)GetActiveView();
    if (view)
      view->StopAllTask();

  }
}

LRESULT CMainFrame::OnCancelProgress(WPARAM wParam, LPARAM lParam)
{
  CVivaImagingView* view = (CVivaImagingView*)GetActiveView();
  view->StopAllTask();
  return -0;
}
