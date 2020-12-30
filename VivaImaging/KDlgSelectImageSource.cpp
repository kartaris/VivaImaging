// KDlgSelectImageSource.cpp: 구현 파일
//

#include "stdafx.h"
#include "VivaImaging.h"
#include "KDlgSelectImageSource.h"
#include "afxdialogex.h"

#include "KDlgConfigureSource.h"
#include "Core\KSettings.h"
#include "Core\KConstant.h"
#include "Platform\KBitmap.h"
#include "Platform\Platform.h"
#include "Platform\Graphics.h"

#include <Wia.h>

#define TIMER_SCAN_MESSAGE_POLL 8010

// KDlgSelectImageSource 대화 상자

IMPLEMENT_DYNAMIC(KDlgSelectImageSource, CDialogEx)

KDlgSelectImageSource::KDlgSelectImageSource(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DLG_SELECT_IMAGE_SOURCE, pParent)
{
  mScanCheckTimer = NULL;
  mParentView = pParent;
  m_pTwainControl = NULL;
  mActiveDeviceName = NULL;
  mWorkingPath = NULL;
}

KDlgSelectImageSource::~KDlgSelectImageSource()
{
  if (mActiveDeviceName != NULL)
  {
    delete[] mActiveDeviceName;
    mActiveDeviceName = NULL;
  }
  if (mWorkingPath != NULL)
  {
    delete[] mWorkingPath;
    mWorkingPath = NULL;
  }
  KTwainControl::ClearDataSourceList(m_SourceDevices);
}

void KDlgSelectImageSource::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_SOURCE_LIST, mImageSourceList);
}


BEGIN_MESSAGE_MAP(KDlgSelectImageSource, CDialogEx)
  ON_LBN_DBLCLK(IDC_SOURCE_LIST, &KDlgSelectImageSource::OnLbnDblclkSourceList)
  ON_BN_CLICKED(IDC_SCAN, &KDlgSelectImageSource::OnClickedScan)
  ON_WM_TIMER()
  ON_MESSAGE(WM_KTWAIN_MSG, OnKTwainMessage)
  ON_MESSAGE(WM_KWIA_MSG, OnKWiaMessage)
END_MESSAGE_MAP()


// KDlgSelectImageSource 메시지 처리기


BOOL KDlgSelectImageSource::OnInitDialog()
{
  CDialogEx::OnInitDialog();

  if (mResourceLanguage != 0x409)
  {
    CString str;
    str.LoadString(AfxGetInstanceHandle(), IDS_SELECT_IMAGE_SOURCE, mResourceLanguage);
    SetWindowText(str);
  }

  // Twain Devices.
  m_pTwainControl = KTwainControl::CreateController(GetSafeHwnd());
  m_pTwainControl->GetDataSourceList(m_SourceDevices);

  m_WiaControl.Init(GetSafeHwnd());
  m_WiaControl.GetDataSourceList(m_SourceDevices);

  TCHAR name[256];
  static LPCTSTR driverTypeName[] = {
    _T("[Twain]"),
    _T("[WIA]"),
    _T("?")
  };

  KImageCaptureDeviceIterator it = m_SourceDevices.begin();
  while (it != m_SourceDevices.end())
  {
    KImageCaptureDevice* device = *it;
    StringCchCopy(name, 256, device->DeviceName);
    StringCchCat(name, 256, _T(" - "));
    StringCchCat(name, 256, driverTypeName[(device->type & 1)]);

    mImageSourceList.AddString(name);
    it++;
  }

  if (gpUiFont != NULL)
  {
    int dlg_ctrl_ids[] = {
      IDC_SOURCE_LIST,
      IDC_SCAN,
      IDCANCEL
    };

    SetDlgItemFont(this, dlg_ctrl_ids, sizeof(dlg_ctrl_ids) / sizeof(int), gpUiFont);
  }

  return TRUE;  // return TRUE unless you set the focus to a control
                // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void KDlgSelectImageSource::OnCancel()
{
  TCHAR msg[256] = _T("");

  try
  {
    EndScan();

    if (m_pTwainControl != NULL)
    {
      if (m_pTwainControl->GetState() >= 4) // scanning
        m_pTwainControl->UnloadDataSource();

      m_pTwainControl->DisconnectDSM();
    }

    KTwainControl::CloseController();
  }
  catch (CException *e)
  {
    e->GetErrorMessage(msg, 256);
  }

  CDialogEx::OnOK();
}

void KDlgSelectImageSource::OnLbnDblclkSourceList()
{
  int index = mImageSourceList.GetCurSel();
  if (index >= 0)
  {
    KImageCaptureDeviceIterator it = m_SourceDevices.begin();
    if (index > 0)
      it += index;
    KImageCaptureDevice* device = *it;

    // open DeviceConfigure dialog
    KDlgConfigureSource dlg(this, m_pTwainControl, device->TwainId);
    dlg.DoModal();
  }
}


void KDlgSelectImageSource::OnClickedScan()
{
  int index = mImageSourceList.GetCurSel();
  if (index >= 0)
  {
    KImageCaptureDeviceIterator it = m_SourceDevices.begin();
    if (index > 0)
      it += index;
    KImageCaptureDevice* device = *it;
    int n = 0;
    TCHAR msg[256] = _T("");

    try
    {
      n = StartScan(device);
    }
    catch (CException *e)
    {
      e->GetErrorMessage(msg, 256);
    }

    if (lstrlen(msg) > 0)
      StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG | SYNC_MSG_DEBUG);

    if (n != SCAN_SUCCESS)
    {
      StringCchPrintf(msg, 128, _T("Failure scan image\n(Device=%s, Error=%d)"), device->DeviceName, n);
      StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG | SYNC_MSG_DEBUG);
      AfxMessageBox(msg);
    }
  }
}

void KDlgSelectImageSource::OnTimer(UINT_PTR nIDEvent)
{
  if (TIMER_SCAN_MESSAGE_POLL == nIDEvent)
  {
    // CheckTwainMessage
    TW_EVENT twEvent = { 0 };
    MSG Msg;
    twEvent.pEvent = (TW_MEMREF)&Msg;
    twEvent.TWMessage = MSG_NULL;
    TW_UINT16  twRC = TWRC_NOTDSEVENT;
    twRC = _DSM_Entry(m_pTwainControl->GetAppID(),
      m_pTwainControl->GetDataSource(), // m_pDataSource,
      DG_CONTROL,
      DAT_EVENT,
      MSG_PROCESSEVENT,
      (TW_MEMREF)&twEvent);

    if (twRC == TWRC_DSEVENT)
    {
      // check for message from Source
      switch (twEvent.TWMessage)
      {
      case MSG_XFERREADY:
      case MSG_CLOSEDSREQ:
      case MSG_NULL:
        m_pTwainControl->SetDSMessage(twEvent.TWMessage);
        break;

      case MSG_CLOSEDSOK:
        StoreLogHistory(_T(__FUNCTION__), _T("MSG_CLOSEDSOK in MSG_PROCESSEVENT loop for Scan"), SYNC_MSG_LOG | SYNC_MSG_DEBUG);
        break;

      default:
        StoreLogHistory(_T(__FUNCTION__), _T("Unknown message in MSG_PROCESSEVENT loop for Scan"), SYNC_MSG_LOG | SYNC_MSG_DEBUG);
        break;
      }
    }

    OnKTwainMessage((WPARAM)twRC, 0);
    /*
    if (twRC == MSG_XFERREADY)   {
      m_pTwainControl->SetState(6);
      //updateIMAGEINFO();
      //UpdateImageInfo();
      ReadScanImage();
      m_pTwainControl->SetState((TW_UINT16)-1);
    }

    if (twRC == MSG_CLOSEDSREQ)
    {
    }
    */
  }
}

LRESULT KDlgSelectImageSource::OnKTwainMessage(WPARAM wParam, LPARAM lParam)
{
  static LPCTSTR paramNames[] = {
    _T("XferReady"),
    _T("CloseDSReq"),
    _T("CloseDSOK"),
    _T("Unknown")
  };

  int i = 0;
  switch (wParam)
  {
  case MSG_XFERREADY:
    i = 0;
    break;

  case MSG_CLOSEDSREQ:
    i = 1;
    break;

  case MSG_CLOSEDSOK:
    i = 2;
    break;

  default:
    i = 3;
    break;
  }

  TCHAR msg[128];
  StringCchPrintf(msg, 128, _T("wParam=%s"), paramNames[i]);

  StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG | SYNC_MSG_DEBUG);

  switch (wParam)
  {
    case MSG_XFERREADY:
      ReadScanImage();
      break;

    case MSG_CLOSEDSREQ:
      EndScan();
      break;

    case MSG_CLOSEDSOK:
      m_pTwainControl->SetState((TW_UINT16)-1);;
      break;

  }
  return 0;
}

LRESULT KDlgSelectImageSource::OnKWiaMessage(WPARAM wParam, LPARAM lParam)
{
  LONG rtn = 0;
  if (wParam == 1)
  {
    LPCTSTR filename = (LPCTSTR)lParam;
    if ((filename != NULL) && (lstrlen(filename) > 0))
    {
#ifdef _TRANSFER_WITH_BITMAP
      KBitmap bm;
      HBITMAP h = bm.LoadImageFromFile(filename, 0);
      if (h != NULL)
      {
        if (mParentView != NULL)
        {
          KVoidPtrArray* imageArray = new KVoidPtrArray;

          HANDLE h = bm.GetDIB();
          imageArray->push_back(h);
          mParentView->PostMessage(WM_SCAN_IMAGE_DONE, (WPARAM)imageArray, (LPARAM)mActiveDeviceName);
        }
      }
      // remove temp files
#ifndef _DEBUG
      DeleteFile(filename);
#endif
#else
      if (mParentView != NULL)
      {
        KVoidPtrArray* imageArray = new KVoidPtrArray;

        int len = lstrlen(filename) + 2;
        HANDLE h = GlobalAlloc(GHND, sizeof(TCHAR) * len);
        LPTSTR p = (LPTSTR)GlobalLock(h);
        if (p != NULL)
        {
          StringCchCopy(p, len, filename);
          GlobalUnlock(h);
          imageArray->push_back(h);
          mParentView->PostMessage(WM_SCAN_IMAGE_DONE, (WPARAM)imageArray, (LPARAM)mActiveDeviceName);
        }
        else
        {
          delete imageArray;
        }
      }
#endif
    }
  }
  return rtn;
}

void KDlgSelectImageSource::SetWorkingPath(LPCTSTR workingPath)
{
  if (mWorkingPath != NULL)
    delete[] mWorkingPath;
  mWorkingPath = AllocString(workingPath);
}

int KDlgSelectImageSource::StartScan(KImageCaptureDevice* device)
{
  int rtn = SCAN_BAD_DEVICE_TYPE;
  if (device->type == K_TWAIN_DEVICE)
  {
    if (m_pTwainControl->GetState() > 4)
    {
      StoreLogHistory(_T(__FUNCTION__), _T("Scan in progress"), SYNC_MSG_LOG | SYNC_MSG_DEBUG);
      return SCAN_IN_PROCESS;
    }
    if (!m_pTwainControl->ConnectDSM())
    {
      StoreLogHistory(_T(__FUNCTION__), _T("Cannot connect to DSM"), SYNC_MSG_LOG | SYNC_MSG_DEBUG);
      return SCAN_CONNECT_DSM_FAIL;
    }

    if (m_pTwainControl->LoadDataSource(device->TwainId))
    {
      // by ADF
      m_pTwainControl->EnableADF(GetSafeHwnd(), TRUE);

      if (mActiveDeviceName != NULL)
        delete[] mActiveDeviceName;
      mActiveDeviceName = AllocString(device->DeviceName);


      if (!m_pTwainControl->EnableDS(GetSafeHwnd(), TRUE))
      {
        StoreLogHistory(_T(__FUNCTION__), _T("Failure on EnableDS"), SYNC_MSG_LOG | SYNC_MSG_DEBUG);
        return SCAN_ENABLE_DS_FAIL;
      }

      if (m_pTwainControl->GetState() >= 5) // ready to scan
      {
        if (!m_pTwainControl->IsUseCallback())
          mScanCheckTimer = SetTimer(TIMER_SCAN_MESSAGE_POLL, 200, NULL);

        rtn = SCAN_SUCCESS;
      }
    }
    else
    {
      StoreLogHistory(_T(__FUNCTION__), _T("Device Not Ready"), SYNC_MSG_LOG | SYNC_MSG_DEBUG);
      return SCAN_DS_NOT_READY;
    }
  }
  else if (device->type == K_WIA_DEVICE)
  {
    if (mActiveDeviceName != NULL)
      delete[] mActiveDeviceName;
    mActiveDeviceName = AllocString(device->DeviceName);
    rtn = m_WiaControl.ScanImage(GetSafeHwnd(), device->bstrDeviceID, mWorkingPath);
  }
  if (rtn = SCAN_SUCCESS)
  {
    if (mActiveDeviceName != NULL)
      delete[] mActiveDeviceName;
    mActiveDeviceName = AllocString(device->DeviceName);
  }
  return rtn;
}

BOOL KDlgSelectImageSource::ContinueScanData()
{
#if 0
  if (m_DSMessage == MSG_XFERREADY)
  {
    // move to state 6 as a result of the data source. We can start a scan now.
    m_DSMState = 6;
    updateIMAGEINFO();
    UpdateImageInfo();
    ReadScanImage();
    m_DSMessage = (TW_UINT16)-1;
  }

  if (!m_bKeepEnabled || (m_DSMessage == MSG_CLOSEDSREQ))
  {
    if (mScanCheckTimer != NULL)
    {
      KillTimer(mScanCheckTimer);
      mScanCheckTimer = NULL;
    }

    m_pTwainControl->DisableDC();
    m_pTwainControl->UnloadDataSource();
    m_pTwainControl->DisconnectDSM();
  }
#endif
  return FALSE;
}

BOOL KDlgSelectImageSource::ReadScanImage()
{
  if (m_pTwainControl->GetState() >= 4)
  {
    TW_UINT32       mech;
    TW_CAPABILITY   Cap;

    Cap.Cap = ICAP_XFERMECH;
    Cap.hContainer = 0;

    m_pTwainControl->SetState(6);

    if ((TWCC_SUCCESS != m_pTwainControl->GetCAP(Cap))
      || !getCurrent(&Cap, mech))
    {
      StoreLogHistory(_T(__FUNCTION__), _T("Could not get the transfer mechanism"), SYNC_MSG_LOG | SYNC_MSG_DEBUG);
      return FALSE;
    }

    // for testing
    // mech = TWSX_FILE;

    if (Cap.hContainer)
    {
      _DSM_Free(Cap.hContainer);
    }

    KVoidPtrArray* imageArray = new KVoidPtrArray;

    switch ((TW_UINT16)mech)
    {
      case TWSX_NATIVE:
        m_pTwainControl->GetNativeBitmaps(*imageArray);
        break;

      case TWSX_FILE:
      {
        TW_UINT32 fileformat = TWFF_TIFF;
        Cap.Cap = ICAP_IMAGEFILEFORMAT;
        Cap.hContainer = 0;

        if (TWCC_SUCCESS == m_pTwainControl->GetCAP(Cap))
        {
          getCurrent(&Cap, fileformat);
        }

        if (Cap.hContainer)
        {
          _DSM_Free(Cap.hContainer);
        }

        m_pTwainControl->initiateTransfer_File((TW_UINT16)fileformat, imageArray, mWorkingPath);
      }
      break;

      case TWSX_MEMORY:
        m_pTwainControl->initiateTransfer_Memory();
        break;
    }

    if (imageArray->size() > 0)
    {
      if (mParentView != NULL)
        mParentView->PostMessage(WM_SCAN_IMAGE_DONE, (WPARAM)imageArray, (LPARAM)mActiveDeviceName);
    }

    return TRUE;
  }
  else
  {
    StoreLogHistory(_T(__FUNCTION__), _T("Twain device not Ready"), SYNC_MSG_LOG | SYNC_MSG_DEBUG);
    return FALSE;
  }
}

BOOL KDlgSelectImageSource::EndScan()
{
  if (mScanCheckTimer != NULL)
  {
    KillTimer(mScanCheckTimer);
    mScanCheckTimer = NULL;
  }

  if (m_pTwainControl != NULL)
  {
    if (m_pTwainControl->GetState() >= 5)
      m_pTwainControl->DisableDS();

    if (m_pTwainControl->GetState() >= 4)
      m_pTwainControl->UnloadDataSource();

    // do not disconnect until close
    /*
    m_pTwainControl->DisconnectDSM();
    */
  }
  return TRUE;
}
