// KDlgConfigureSource.cpp: 구현 파일
//

#include "stdafx.h"
#include "VivaImaging.h"
#include "KDlgConfigureSource.h"
#include "afxdialogex.h"

#include "Platform\Platform.h"
#include "Core\KSettings.h"
#include "Core\KConstant.h"

// KDlgConfigureSource 대화 상자

IMPLEMENT_DYNAMIC(KDlgConfigureSource, CDialogEx)

KDlgConfigureSource::KDlgConfigureSource(CWnd* pParent, KTwainControl* twainControl, UINT32 deviceID)
	: CDialogEx(IDD_DLG_CONFIGURE_SOURCE, pParent)
{
  m_DeviceID = deviceID;
  mFont = NULL;
  m_pTwainControl = twainControl;
}

KDlgConfigureSource::~KDlgConfigureSource()
{
  if (mFont != NULL)
  {
    mFont->DeleteObject();
    delete mFont;
    mFont = NULL;
  }
}

void KDlgConfigureSource::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_CONFIGURE_SOURCE, m_ConfigureSourceCtrl);
}


BEGIN_MESSAGE_MAP(KDlgConfigureSource, CDialogEx)
  ON_BN_CLICKED(IDC_SCAN, &KDlgConfigureSource::OnClickedScan)
END_MESSAGE_MAP()


// KDlgConfigureSource 메시지 처리기

BOOL KDlgConfigureSource::OnInitDialog()
{
  CDialogEx::OnInitDialog();

  if (mResourceLanguage != 0x409)
  {
    CString str;
    str.LoadString(AfxGetInstanceHandle(), IDS_CONFIGURE_SOURCE, mResourceLanguage);
    SetWindowText(str);
  }

  if (m_pTwainControl == NULL)
    m_pTwainControl = KTwainControl::CreateController(GetSafeHwnd());

  if (m_pTwainControl->GetState() == 2)
  {
    m_pTwainControl->ConnectDSM();
  }
  if (m_pTwainControl->GetState() == 3)
  {
    m_pTwainControl->LoadDataSource(m_DeviceID);
  }

  if (m_pTwainControl->GetState() >= 4)
  {
    char msg[1024];
    m_pTwainControl->GetDataSourceInfo(msg, 1024);
    CWnd* text = GetDlgItem(IDC_STATIC_DEVICE_INFO);
    if (text != NULL)
    {
      LPTSTR str = MakeUnicodeString(msg);
      text->SetWindowTextW(str);
      delete[] str;
    }

    if (mFont != NULL)
      m_ConfigureSourceCtrl.SetFont(mFont);

    if (m_pTwainControl->GetState() >= 4)
    {
      m_ConfigureSourceCtrl.CreateSourceCapsChilds(m_pTwainControl, m_DeviceID);
    }
  }
  return TRUE;  // return TRUE unless you set the focus to a control
                // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void KDlgConfigureSource::OnSetFont(CFont* pFont)
{
  HFONT font = (HFONT)pFont->GetSafeHandle();
  LOGFONT logFont;
  CFont *f = CFont::FromHandle(font);

  if (f->GetLogFont(&logFont))
  {
    if (mFont == NULL)
      mFont = new CFont();
    else
      mFont->Detach();
    mFont->CreateFontIndirect(&logFont);
  }
}

void KDlgConfigureSource::OnClickedScan()
{
  /*
  if (m_pTwainControl->GetState() == 2)
  {
    m_pTwainControl->ConnectDSM();
    m_pTwainControl->LoadDataSource(m_DeviceID);
  }
  */
  if (m_pTwainControl->GetState() >= 4) // scanning ready
  {
    if (!m_pTwainControl->EnableDS(GetSafeHwnd(), TRUE))
    {
      StoreLogHistory(_T(__FUNCTION__), _T("Failure on EnableDS"), SYNC_MSG_LOG | SYNC_MSG_DEBUG);
      return;
    }
  }
  else
  {
    StoreLogHistory(_T(__FUNCTION__), _T("DataSource not loaded"), SYNC_MSG_LOG | SYNC_MSG_DEBUG);
  }
}

void KDlgConfigureSource::OnCancel()
{
  if (m_pTwainControl->GetState() >= 5) // scanning
    m_pTwainControl->DisableDS();

  /*
  if (m_pTwainControl->GetState() >= 4) // scanning
    m_pTwainControl->UnloadDataSource();
  m_pTwainControl->DisconnectDSM();
  */
  CDialogEx::OnCancel();
}
