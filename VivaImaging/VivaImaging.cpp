// Copyright (C) 2019 K2Mobile
// All rights reserved.

// VivaImaging.cpp: 응용 프로그램에 대한 클래스 동작을 정의합니다.
//

#include "stdafx.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "VivaImaging.h"
#include "MainFrm.h"
#include "resource.h"
#include "VivaCommandInfo.h"

#include "Core\KConstant.h"
#include "Core\KSyncCoreEngine.h"
#include "Platform\Graphics.h"
#include "Control\KImageButton.h"
#include "Document\KShapeEditor.h"
#include "Core/HttpDownloader/KMemory.h"

#include "VivaImagingDoc.h"
#include "VivaImagingView.h"

#include "KDlgNewProject.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CVivaImagingApp

BEGIN_MESSAGE_MAP(CVivaImagingApp, CWinAppEx)
	ON_COMMAND(ID_APP_ABOUT, &CVivaImagingApp::OnAppAbout)
	// 표준 파일을 기초로 하는 문서 명령입니다.
	ON_COMMAND(ID_FILE_NEW, &CVivaImagingApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinAppEx::OnFileOpen)
	// 표준 인쇄 설정 명령입니다.
  ON_COMMAND(ID_FILE_PRINT_SETUP, &CWinAppEx::OnFilePrintSetup)
END_MESSAGE_MAP()


// CVivaImagingApp 생성

CVivaImagingApp::CVivaImagingApp()
{
	m_bHiColorIcons = TRUE;

	// 다시 시작 관리자 지원
	// m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_ALL_ASPECTS;
  // 자동 저장 사용 안함.
  m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_NO_AUTOSAVE;


#ifdef _MANAGED
	// 응용 프로그램을 공용 언어 런타임 지원을 사용하여 빌드한 경우(/clr):
	//     1) 이 추가 설정은 다시 시작 관리자 지원이 제대로 작동하는 데 필요합니다.
	//     2) 프로젝트에서 빌드하려면 System.Windows.Forms에 대한 참조를 추가해야 합니다.
	System::Windows::Forms::Application::SetUnhandledExceptionMode(System::Windows::Forms::UnhandledExceptionMode::ThrowException);
#endif

	// 문자열에 대한 서식: CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("Malan.VivaImaging.1.0"));

	// TODO: 여기에 생성 코드를 추가합니다.
	// InitInstance에 모든 중요한 초기화 작업을 배치합니다.
}

// 유일한 CVivaImagingApp 개체입니다.

CVivaImagingApp theApp;

GdiplusStartupInput gdiplusStartupInput;
ULONG_PTR gdiplusToken = NULL;


// CVivaImagingApp 초기화

BOOL CVivaImagingApp::InitInstance()
{
	// 응용 프로그램 매니페스트가 ComCtl32.dll 버전 6 이상을 사용하여 비주얼 스타일을
	// 사용하도록 지정하는 경우, Windows XP 상에서 반드시 InitCommonControlsEx()가 필요합니다. 
	// InitCommonControlsEx()를 사용하지 않으면 창을 만들 수 없습니다.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 응용 프로그램에서 사용할 모든 공용 컨트롤 클래스를 포함하도록
	// 이 항목을 설정하십시오.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

  // Initialize GDI+.
  GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

  GetModuleFileName(NULL, gModulePathName, KMAX_PATH);
  LPTSTR moduleFileName = AllocString(gModulePathName);
  PathRemoveFileSpec(gModulePathName);

  TCHAR moduleVersion[32];
  GetFileVersionInfoString(moduleFileName, moduleVersion, 32);

  // ready EngineSessionID
  DWORD sid = KSyncCoreEngine::MakeSessionID();
  if (sid == INVALID_SESSION_ID)
  {
    CString str;
    str.LoadString(AfxGetInstanceHandle(), IDS_UNAVAILABLE_SESSION, mResourceLanguage);
    AfxMessageBox(str);
    return FALSE;
  }

  TCHAR msg[128];
  StringCchPrintf(msg, 128, _T("Module Version=%s, SessionID=%d"), moduleVersion, sid);
  StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);

  if (m_lpCmdLine != NULL)
    StoreLogHistory(_T(__FUNCTION__), m_lpCmdLine, SYNC_MSG_LOG);

  KMemoryInit();

  KImageBase::gVivaCustomClipboardFormat = RegisterClipboardFormat(_T("VivaImagingDataFm"));

  mResourceLanguage = GetUserDefaultUILanguage(); // (EN)0x409, (KR)0x412

  srand(GetTickCount()); // init random

  if (gpUiFont == NULL)
  {
    NONCLIENTMETRICS mt;
    mt.cbSize = sizeof(NONCLIENTMETRICS);
    SystemParametersInfo(SPI_GETNONCLIENTMETRICS, 0, &mt, 0);

    /*
    int dpi = GetDpiForWindow(GetSafeHwnd());
    if (dpi != 96)
    mt.lfMenuFont.lfHeight = mt.lfMenuFont.lfHeight * dpi / 96;
    */
    gpUiFont = new CFont();
    gpUiFont->CreateFontIndirect(&mt.lfMenuFont);
    StringCchCopy(gDefaultFacename, LF_FACESIZE, mt.lfMenuFont.lfFaceName);

    CDC dc;
    HWND desktopWnd = GetDesktopWindow();
    HDC pDC = GetDC(desktopWnd);
    dc.CreateCompatibleDC(CDC::FromHandle(pDC));
    CFont* old_font = (CFont*)dc.SelectObject(gpUiFont);
    TEXTMETRIC tm;
    dc.GetTextMetrics(&tm);
    dc.SelectObject(old_font);
    dc.DeleteDC();
    ReleaseDC(desktopWnd, pDC);

    gDlgFontBaseWidth = tm.tmAveCharWidth;
    gDlgFontBaseHeight = tm.tmHeight * 2 / 3;

    // pix = dlgunit * basey / 8;
    //25 = 10 * 20 / 8
    //basey = gDlgFontBaseHeight * 8 / 10;

    TCHAR msg[128];
    StringCchPrintf(msg, 128, _T("Dialog 10 = %d pixel, UI font height=%d, BaseWidth=%d, BaseHeight=%d"), 
      DialogY2Pixel(10), mt.lfMenuFont.lfHeight, gDlgFontBaseWidth, gDlgFontBaseHeight);
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG| SYNC_MSG_DEBUG);
  }

	CWinAppEx::InitInstance();


	// OLE 라이브러리를 초기화합니다.
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();

	EnableTaskbarInteraction(FALSE);

	// RichEdit 컨트롤을 사용하려면 AfxInitRichEdit2()가 있어야 합니다.
	// AfxInitRichEdit2();

  // will be registered from installer
  // InitApplicationsRegistry(moduleFileName);
  // InitShellRegistry(moduleFileName);

	SetRegistryKey(_T("Destiny Imaging"));
	LoadStdProfileSettings(8);  // MRU를 포함하여 표준 INI 파일 옵션을 로드합니다.

  // read setting values from registry.
  LoadDefaultSetting();
  KShapeEditor::LoadDefaultProperty();

	InitContextMenuManager();

	InitKeyboardManager();

	InitTooltipManager();
	CMFCToolTipInfo ttParams;
	ttParams.m_bVislManagerTheme = TRUE;
	theApp.GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL,
		RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);

	// 응용 프로그램의 문서 템플릿을 등록합니다.  문서 템플릿은
	//  문서, 프레임 창 및 뷰 사이의 연결 역할을 합니다.
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CVivaImagingDoc),
		RUNTIME_CLASS(CMainFrame),       // 주 SDI 프레임 창입니다.
		RUNTIME_CLASS(CVivaImagingView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	// 표준 셸 명령, DDE, 파일 열기에 대한 명령줄을 구문 분석합니다.
	// CCommandLineInfo cmdInfo;
  KVivaCommandInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// DDE Execute 열기를 활성화합니다.
	EnableShellOpen();
	RegisterShellFileTypes(TRUE);

	// 명령줄에 지정된 명령을 디스패치합니다.
	// 응용 프로그램이 /RegServer, /Register, /Unregserver 또는 /Unregister로 시작된 경우 FALSE를 반환합니다.
  if (!ProcessShellCommand(cmdInfo))
    return FALSE;

  cmdInfo.Dump();

  if (cmdInfo.mOpenLocalFolder.GetLength() > 0)
    ((CMainFrame*)m_pMainWnd)->SetDelayedJob(JOB_OPEN_LOCAL_FOLDER, cmdInfo.mOpenLocalFolder);
  else if(cmdInfo.mOpenServerFolder.GetLength() > 0)
    ((CMainFrame*)m_pMainWnd)->SetDelayedJob(JOB_OPEN_SERVER_FOLDER, cmdInfo.mOpenServerFolder);
  else if (cmdInfo.mOpenServerFile.GetLength() > 0)
    ((CMainFrame*)m_pMainWnd)->SetDelayedJob(JOB_OPEN_SERVER_FILE, cmdInfo.mOpenServerFile);
  else
  {
    ((CMainFrame*)m_pMainWnd)->SetDelayedJob(JOB_CHECK_UNFINISHED, NULL);
#ifdef _DEBUG_OPEN_SERVER_FOLDER
    // openserverfolder:1MQ2u64-N8H)
    CString folder(_T("1MQ2u64-N8H"));
    ((CMainFrame*)m_pMainWnd)->SetDelayedJob(JOB_OPEN_SERVER_FOLDER, folder);
#endif
  }

  if (cmdInfo.mUploadServerFolder.GetLength() > 0)
    StringCchCopy(mUploadServerFolderOID, MAX_OID, cmdInfo.mUploadServerFolder);

  // 정책 받지 않기. 이전 설정값 우선 사용.
  cmdInfo.mSystemOption = mGlobalOption;

  ((CMainFrame*)m_pMainWnd)->SetCommandOption(cmdInfo.mSystemOption);

	// 창 하나만 초기화되었으므로 이를 표시하고 업데이트합니다.
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();
	// 접미사가 있을 경우에만 DragAcceptFiles를 호출합니다.
	//  SDI 응용 프로그램에서는 ProcessShellCommand 후에 이러한 호출이 발생해야 합니다.
	// 끌어서 놓기에 대한 열기를 활성화합니다.
	m_pMainWnd->DragAcceptFiles();
	return TRUE;
}

int CVivaImagingApp::ExitInstance()
{
	//TODO: 추가한 추가 리소스를 처리합니다.
	AfxOleTerm(FALSE);

  StoreDefaultSetting();
  KShapeEditor::StoreDefaultProperty();

  KThumbListView::ClearStatic();
  KImageButton::ClearStatic();
  KShapeEditor::ClearStatic();

  if (gpUiFont != NULL)
  {
    delete gpUiFont;
    gpUiFont = NULL;
  }

  if (gImageFilenameFormat != NULL)
  {
    delete[] gImageFilenameFormat;
    gImageFilenameFormat = NULL;
  }

  GdiplusShutdown(gdiplusToken);

  KMemoryDump();

  TCHAR msg[64];
  StringCchPrintf(msg, 64, _T("CloseSessionID : %d"), mSessionIdx);
  StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);

  KSyncCoreEngine::ClearSessionID();

	return CWinAppEx::ExitInstance();
}

LPCTSTR regKeyDefaultFileType = _T("DefaultFileType");
LPCTSTR regKeyGlobalOption = _T("GlobalOption");
LPCTSTR regKeyDefaultFilenameFormat = _T("DefaultFilenameFormat");
LPCTSTR defaultFilenameFormat = _T("$folder-image$$$uoid-$date-$time-$rand");
LPCTSTR regKeyDefaultUploadFolderPath = _T("DefaultUploadFolderPath");
LPCTSTR regKeyDefaultUploadFolderOID = _T("DefaultUploadFolderOID");
LPCTSTR regKeyDefaultWorkingBaseFolder = _T("DefaultWorkingBaseFolder");

#include "Platform\Reg.h"

void CVivaImagingApp::LoadDefaultSetting()
{
  // default values
  if (gImageFilenameFormat == NULL)
    gImageFilenameFormat = AllocString(defaultFilenameFormat);

  CString value;

  if (LoadRegistryValue(regKeyDefaultFileType, value) &&
    (value.GetLength() > 0))
    gSaveFormat = (ImagingSaveFormat)_wtoi(value);

  if (LoadRegistryValue(regKeyGlobalOption, value) &&
    (value.GetLength() > 0))
    mGlobalOption = _wtoi(value);

  if (LoadRegistryValue(regKeyDefaultFilenameFormat, value) &&
    (value.GetLength() > 0))
  {
    if (gImageFilenameFormat != NULL)
      delete[] gImageFilenameFormat;
    gImageFilenameFormat = AllocString((LPCTSTR)value);
  }

#ifdef _UNUSED
  if (LoadRegistryValue(regKeyDefaultUploadFolderPath, value) &&
    (value.GetLength() > 0))
  {
    StringCchCopy(mDefaultUploadFolderPath, MAX_PATH, value);
  }
  if (LoadRegistryValue(regKeyDefaultUploadFolderOID, value) &&
    (value.GetLength() > 0))
  {
    StringCchCopy(mDefaultUploadFolderOID, 16, value);
  }
#endif

  if (LoadRegistryValue(regKeyDefaultWorkingBaseFolder, value) &&
    (value.GetLength() > 0))
  {
    StringCchCopy(mDefaultWorkingBaseFolder, MAX_PATH, value);
  }
}

void CVivaImagingApp::StoreDefaultSetting()
{
  CString value;
  value.Format(_T("%d"), gSaveFormat);
  StoreRegistryValue(regKeyDefaultFileType, value);

  value.Format(_T("%d"), mGlobalOption);
  StoreRegistryValue(regKeyGlobalOption, value);

  if (gImageFilenameFormat != NULL)
  {
    value.SetString(gImageFilenameFormat);
    StoreRegistryValue(regKeyDefaultFilenameFormat, value);
  }

#ifdef _UNUSED
  StoreRegistryValue(regKeyDefaultUploadFolderPath, mDefaultUploadFolderPath);
  StoreRegistryValue(regKeyDefaultUploadFolderOID, mDefaultUploadFolderOID);
#endif
  StoreRegistryValue(regKeyDefaultWorkingBaseFolder, mDefaultWorkingBaseFolder);
}


// CVivaImagingApp 메시지 처리기


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// 대화 상자를 실행하기 위한 응용 프로그램 명령입니다.
void CVivaImagingApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// CVivaImagingApp 사용자 지정 로드/저장 방법

void CVivaImagingApp::InitApplicationsRegistry(LPCTSTR execPath)
{
  static LPTSTR IMAGING_APP_KEY = _T("Applications\\DestinyImaging.exe");

  HKEY hKey = NULL;
  DWORD cbData;
  TCHAR keyName[MAX_PATH];
  TCHAR pathName[MAX_PATH];

  HRESULT hr = HRESULT_FROM_WIN32(RegCreateKeyEx(HKEY_CLASSES_ROOT, IMAGING_APP_KEY, 0,
    NULL, REG_OPTION_NON_VOLATILE, KEY_READ | KEY_WRITE, NULL, &hKey, NULL));
  if (SUCCEEDED(hr))
  {
    HKEY hKeyIcon = NULL;
    StringCchCopy(keyName, 128, IMAGING_APP_KEY);
    StringCchCat(keyName, 128, _T("\\DefaultIcon"));
    hr = HRESULT_FROM_WIN32(RegCreateKeyEx(HKEY_CLASSES_ROOT, keyName, 0,
      NULL, REG_OPTION_NON_VOLATILE, KEY_READ | KEY_WRITE, NULL, &hKeyIcon, NULL));
    if (SUCCEEDED(hr))
    {
      StringCchCopy(pathName, MAX_PATH, execPath);
      StringCchCat(pathName, MAX_PATH, _T(",0"));
      cbData = (lstrlen(pathName) + 1) * sizeof(TCHAR);
      RegSetValueEx(hKeyIcon, NULL, 0, REG_SZ, (LPBYTE)pathName, cbData);
    }

    // shell-open-command
    HKEY hKeyShell = NULL;
    StringCchCopy(keyName, 128, IMAGING_APP_KEY);
    StringCchCat(keyName, 128, _T("\\shell"));
    hr = HRESULT_FROM_WIN32(RegCreateKeyEx(HKEY_CLASSES_ROOT, keyName, 0,
      NULL, REG_OPTION_NON_VOLATILE, KEY_READ | KEY_WRITE, NULL, &hKeyShell, NULL));
    if (SUCCEEDED(hr))
    {
      HKEY hKeyOpen = NULL;
      StringCchCat(keyName, 128, _T("\\open"));
      hr = HRESULT_FROM_WIN32(RegCreateKeyEx(HKEY_CLASSES_ROOT, keyName, 0,
        NULL, REG_OPTION_NON_VOLATILE, KEY_READ | KEY_WRITE, NULL, &hKeyOpen, NULL));
      if (SUCCEEDED(hr))
      {
        HKEY hKeyCommand = NULL;
        StringCchCat(keyName, 128, _T("\\command"));
        hr = HRESULT_FROM_WIN32(RegCreateKeyEx(HKEY_CLASSES_ROOT, keyName, 0,
          NULL, REG_OPTION_NON_VOLATILE, KEY_READ | KEY_WRITE, NULL, &hKeyCommand, NULL));
        if (SUCCEEDED(hr))
        {
          StringCchCopy(pathName, MAX_PATH, execPath);
          StringCchCat(pathName, MAX_PATH, _T(" %1"));
          cbData = (lstrlen(pathName) + 1) * sizeof(TCHAR);
          RegSetValueEx(hKeyCommand, NULL, 0, REG_SZ, (LPBYTE)pathName, cbData);

          RegCloseKey(hKeyCommand);
        }

        RegCloseKey(hKeyOpen);
      }

      RegCloseKey(hKeyShell);
    }

    static LPTSTR SUPPORTED_FILE_TYPES[6] = {
      _T(".png"),
      _T(".jpg"),
      _T(".jpeg"),
      _T(".bmp"),
      _T(".tif"),
      _T(".tiff"),
    };

    // supported
    StringCchCopy(keyName, 128, IMAGING_APP_KEY);
    StringCchCat(keyName, 128, _T("\\SupportedTypes"));
    hr = HRESULT_FROM_WIN32(RegCreateKeyEx(HKEY_CLASSES_ROOT, keyName, 0,
      NULL, REG_OPTION_NON_VOLATILE, KEY_READ | KEY_WRITE, NULL, &hKeyShell, NULL));
    if (SUCCEEDED(hr))
    {
      for (int i = 0; i < 6; i++)
      {
        RegSetValueEx(hKeyShell, SUPPORTED_FILE_TYPES[i], 0, REG_SZ, (LPBYTE)NULL, 0);
      }
      RegCloseKey(hKeyShell);
    }
  }
  else
  {
    StoreLogHistory(_T(__FUNCTION__), _T("Cannot access to HKCR registry"), SYNC_MSG_LOG);
  }
}

void CVivaImagingApp::InitShellRegistry(LPCTSTR execPath)
{
  static LPTSTR IMAGING_PROGID_KEY = _T("Software\\Classes\\DestinyImaging.1");
  static LPTSTR IMAGING_PROGIDS = _T("DestinyImaging.1");
  static LPTSTR IMAGING_PROGID_NAME = _T("Destiny Imaging");
  //static LPTSTR IMAGING_DEFAULT_ICON = _T("C:\\Cyberdigm\\DestinyImaging\\DestinyImaging.EXE,0");
  static LPTSTR IMAGING_APP_NAME = _T("Destiny Imaging 1.0");
  static LPTSTR IMAGING_COMPANY_NAME = _T("Malan.kr");
  static LPTSTR IMAGING_APPLICATION_DESCRIPTION = _T("Destiny Imaging");
  static LPTSTR IMAGING_APPLICATION_NAME = _T("Destiny Imaging");

  static LPTSTR XML_FILE_KEYS[6] = {
    //_T("Software\\Classes\\.xml\\OpenWithProgids"),
    _T("Software\\Classes\\.png\\OpenWithProgids"),
    _T("Software\\Classes\\.jpg\\OpenWithProgids"),
    _T("Software\\Classes\\.jpeg\\OpenWithProgids"),
    _T("Software\\Classes\\.bmp\\OpenWithProgids"),
    _T("Software\\Classes\\.tif\\OpenWithProgids"),
    _T("Software\\Classes\\.tiff\\OpenWithProgids"),
  };
  static LPTSTR EMPTY_STR = _T("");

  HKEY hKey = NULL;
  WORD rtn = 1; // default is Enable
  DWORD cbData;
  TCHAR pathName[MAX_PATH];

  HRESULT hr = HRESULT_FROM_WIN32(RegCreateKeyEx(HKEY_LOCAL_MACHINE, IMAGING_PROGID_KEY, 0,
    NULL, REG_OPTION_NON_VOLATILE, KEY_READ| KEY_WRITE, NULL, &hKey, NULL));

  if (SUCCEEDED(hr))
  {
    cbData = (lstrlen(IMAGING_PROGID_NAME) + 1) * sizeof(TCHAR);
    RegSetValueEx(hKey, NULL, 0, REG_SZ, (LPBYTE)IMAGING_PROGID_NAME, cbData);

    cbData = (lstrlen(IMAGING_APP_NAME) + 1) * sizeof(TCHAR);
    RegSetValueEx(hKey, _T("FriendlyAppName"), 0, REG_SZ, (LPBYTE)IMAGING_APP_NAME, cbData);

    HKEY hKeyApp = NULL;
    TCHAR keyName[128];
    StringCchCopy(keyName, 128, IMAGING_PROGID_KEY);
    StringCchCat(keyName, 128, _T("\\Application"));
    HRESULT hr = HRESULT_FROM_WIN32(RegCreateKeyEx(HKEY_LOCAL_MACHINE, keyName, 0,
      NULL, REG_OPTION_NON_VOLATILE, KEY_READ | KEY_WRITE, NULL, &hKeyApp, NULL));
    if (SUCCEEDED(hr))
    {
      cbData = (lstrlen(IMAGING_COMPANY_NAME) + 1) * sizeof(TCHAR);
      RegSetValueEx(hKeyApp, _T("ApplicationCompany"), 0, REG_SZ, (LPBYTE)IMAGING_COMPANY_NAME, cbData);
      cbData = (lstrlen(IMAGING_APPLICATION_DESCRIPTION) + 1) * sizeof(TCHAR);
      RegSetValueEx(hKeyApp, _T("ApplicationDescription"), 0, REG_SZ, (LPBYTE)IMAGING_APPLICATION_DESCRIPTION, cbData);
      cbData = (lstrlen(IMAGING_APPLICATION_NAME) + 1) * sizeof(TCHAR);
      RegSetValueEx(hKeyApp, _T("ApplicationName"), 0, REG_SZ, (LPBYTE)IMAGING_APPLICATION_NAME, cbData);

      StringCchCopy(pathName, MAX_PATH, execPath);
      StringCchCat(pathName, MAX_PATH, _T(",0"));
      cbData = (lstrlen(pathName) + 1) * sizeof(TCHAR);
      RegSetValueEx(hKeyApp, _T("ApplicationIcon"), 0, REG_SZ, (LPBYTE)pathName, cbData);
      RegCloseKey(hKeyApp);
    }

    StringCchCopy(keyName, 128, IMAGING_PROGID_KEY);
    StringCchCat(keyName, 128, _T("\\DefaultIcon"));
    hr = HRESULT_FROM_WIN32(RegCreateKeyEx(HKEY_LOCAL_MACHINE, keyName, 0,
      NULL, REG_OPTION_NON_VOLATILE, KEY_READ | KEY_WRITE, NULL, &hKeyApp, NULL));
    if (SUCCEEDED(hr))
    {

    }

    StringCchCopy(keyName, 128, IMAGING_PROGID_KEY);
    StringCchCat(keyName, 128, _T("\\shell"));
    hr = HRESULT_FROM_WIN32(RegCreateKeyEx(HKEY_LOCAL_MACHINE, keyName, 0,
      NULL, REG_OPTION_NON_VOLATILE, KEY_READ | KEY_WRITE, NULL, &hKeyApp, NULL));
    if (SUCCEEDED(hr))
    {
      HKEY hKeyOpen = NULL;
      StringCchCat(keyName, 128, _T("\\open"));
      hr = HRESULT_FROM_WIN32(RegCreateKeyEx(HKEY_LOCAL_MACHINE, keyName, 0,
        NULL, REG_OPTION_NON_VOLATILE, KEY_READ | KEY_WRITE, NULL, &hKeyOpen, NULL));
      if (SUCCEEDED(hr))
      {
        HKEY hKeyCommand = NULL;
        StringCchCat(keyName, 128, _T("\\command"));
        hr = HRESULT_FROM_WIN32(RegCreateKeyEx(HKEY_LOCAL_MACHINE, keyName, 0,
          NULL, REG_OPTION_NON_VOLATILE, KEY_READ | KEY_WRITE, NULL, &hKeyCommand, NULL));
        if (SUCCEEDED(hr))
        {
          StringCchCopy(pathName, MAX_PATH, execPath);
          StringCchCat(pathName, MAX_PATH, _T(" %1"));
          cbData = (lstrlen(pathName) + 1) * sizeof(TCHAR);
          RegSetValueEx(hKeyCommand, NULL, 0, REG_SZ, (LPBYTE)pathName, cbData);

          RegCloseKey(hKeyCommand);
        }

        RegCloseKey(hKeyOpen);
      }

      RegCloseKey(hKeyApp);
    }

    RegCloseKey(hKey);
    StoreLogHistory(_T(__FUNCTION__), _T("Updated"), SYNC_MSG_LOG);

    TCHAR msg[128];
    for (int i = 0; i < 6; i++)
    {
      hr = HRESULT_FROM_WIN32(RegCreateKeyEx(HKEY_LOCAL_MACHINE, XML_FILE_KEYS[i], 0,
        NULL, REG_OPTION_NON_VOLATILE, KEY_READ | KEY_WRITE, NULL, &hKey, NULL));

      StringCchCopy(msg, 128, XML_FILE_KEYS[i]);
      if (SUCCEEDED(hr))
      {
        cbData = (lstrlen(EMPTY_STR) + 1) * sizeof(TCHAR);
        RegSetValueEx(hKey, IMAGING_PROGIDS, 0, REG_SZ, (LPBYTE)EMPTY_STR, cbData);
        RegCloseKey(hKey);
        StringCchCat(msg, 128, _T(" : Ok"));
      }
      else
      {
        StringCchCat(msg, 128, _T(" : Fail"));
      }
      StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
    }
  }
  else
  {
    StoreLogHistory(_T(__FUNCTION__), _T("Cannot access to HKCU registry"), SYNC_MSG_LOG);
  }

  SetAppID(_T("DestinyImaging.1"));
}

void CVivaImagingApp::PreLoadState()
{
	BOOL bNameValid;
	CString strName;
	bNameValid = strName.LoadString(IDS_EDIT_MENU);
	ASSERT(bNameValid);
  
  GetContextMenuManager()->AddMenu(strName, IDR_POPUP_IMAGE_VIEW);
  GetContextMenuManager()->AddMenu(strName, IDR_POPUP_BLOCK_EDIT);
  GetContextMenuManager()->AddMenu(strName, IDR_POPUP_SHAPE_EDIT);
  bNameValid = strName.LoadString(IDS_EXPLORER);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EXPLORER);
  GetContextMenuManager()->AddMenu(strName, IDR_THUMBNAIL_MENU);
}

void CVivaImagingApp::LoadCustomState()
{
}

void CVivaImagingApp::SaveCustomState()
{
}

// CVivaImagingApp 메시지 처리기
CDocument* CVivaImagingApp::OpenDocumentFile(LPCTSTR lpszFileName)
{
  CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
  if (pFrame)
    pFrame->CloseBgTask();

  CDocument* doc = CWinAppEx::OpenDocumentFile(lpszFileName, FALSE);
  if (doc != NULL)
  {
    pFrame = (CMainFrame *)AfxGetMainWnd();
    if (pFrame)
      pFrame->OnActivePageChanged(CHANGED_ACTIVE_PAGE | CHANGED_NUMBER_OF_PAGES);
  }
  else
  {
    TCHAR msg[256] = _T("Failed : ");
    StringCchCat(msg, 256, lpszFileName);
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
  }
  return doc;
}

CDocument* CVivaImagingApp::OpenDocumentFile(
  LPCTSTR lpszFileName,
  BOOL bAddToMRU)
{
  CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
  if (pFrame)
    pFrame->CloseBgTask();

  CDocument* doc = CWinAppEx::OpenDocumentFile(lpszFileName, bAddToMRU);
  if (doc != NULL)
  {
    pFrame = (CMainFrame *)AfxGetMainWnd();
    if (pFrame)
      pFrame->OnActivePageChanged(CHANGED_ACTIVE_PAGE | CHANGED_NUMBER_OF_PAGES);
  }
  return doc;
}

void CVivaImagingApp::OnFileNew()
{
  //KDlgNewProject dlgNewProject;
  //if (dlgNewProject.DoModal())
  
  /*
  CWinAppEx::OnFileNew();
  */
  CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
  if (pFrame)
    pFrame->CloseBgTask();

  POSITION p = GetFirstDocTemplatePosition();
  CDocTemplate* docTemp = GetNextDocTemplate(p);
  if (docTemp != NULL)
  {
    if (docTemp->OpenDocumentFile(NULL) != NULL)
    {
      pFrame = (CMainFrame *)AfxGetMainWnd();
      if (pFrame)
        pFrame->OnActivePageChanged(CHANGED_ACTIVE_PAGE | CHANGED_NUMBER_OF_PAGES);
    }
  }
}

