#include "stdafx.h"

#include "KTwainControl.h"
#include "Common.h"
#include "../Platform.h"
#include "..\..\Core\KSettings.h"

KTwainControl* gpKTwainControl = NULL;

KTwainControl::KTwainControl()
{
  m_TWAINApp = NULL;
  mAppID = NULL;
  mParentWnd = NULL;
}


KTwainControl::~KTwainControl()
{
  if (m_TWAINApp != NULL)
  {
    delete m_TWAINApp;
    m_TWAINApp = NULL;
  }
}

//////////////////////////////////////////////////////////////////////////////
/**
* Callback funtion for DS.  This is a callback function that will be called by
* the source when it is ready for the application to start a scan. This
* callback needs to be registered with the DSM before it can be called.
* It is important that the application returns right away after recieving this
* message.  Set a flag and return.  Do not process the callback in this function.
*/
TW_UINT16 FAR PASCAL DSMCallback(pTW_IDENTITY _pOrigin,
  pTW_IDENTITY _pDest,
  TW_UINT32    _DG,
  TW_UINT16    _DAT,
  TW_UINT16    _MSG,
  TW_MEMREF    _pData)
{
  //UNUSEDARG(_pDest);
  //UNUSEDARG(_DG);
  //UNUSEDARG(_DAT);
  //UNUSEDARG(_pData);

  TW_UINT16 twrc = TWRC_FAILURE;
  // _pData stores the RefCon from the when the callback was registered
  // RefCon is a TW_INT32 and can not store a pointer for 64bit

  // we are only waiting for callbacks from our datasource, so validate
  // that the originator.
  if (0 != _pOrigin
    && 0 != gpKTwainControl
    && gpKTwainControl->GetDataSource()
    && _pOrigin->Id == gpKTwainControl->GetDataSource()->Id)
  {
    switch (_MSG)
    {
      case MSG_XFERREADY:
      case MSG_CLOSEDSREQ:
      case MSG_CLOSEDSOK:
      case MSG_NULL:
        gpKTwainControl->OnDSMessage(_MSG);
        twrc = TWRC_SUCCESS;
        break;

      default:
      {
        TCHAR msg[128];
        StringCchPrintf(msg, 128, _T("Error - Unknown Callback Message(0x%x)"), _MSG);
        StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG | SYNC_MSG_DEBUG);

        gpKTwainControl->OnDSMessage(MSG_NULL);
        twrc = TWRC_FAILURE;
        break;
      }
    }
  }

  // Force a refresh, so that we process the message...
  // g_pTWAINApp->RedrawWindow();

  // All done...
  return twrc;
}

KTwainControl* KTwainControl::CreateController(HWND hWnd)
{
  if (gpKTwainControl == NULL)
  {
    gpKTwainControl = new KTwainControl();
    gpKTwainControl->Init(hWnd);
  }
  else
  {
    StoreLogHistory(_T(__FUNCTION__), _T("Already exist KTwainControl instance"), SYNC_MSG_LOG | SYNC_MSG_DEBUG);
    gpKTwainControl->mParentWnd = hWnd;
  }
  return gpKTwainControl;
}

void KTwainControl::CloseController()
{
  if (gpKTwainControl != NULL)
  {
    delete gpKTwainControl;
    gpKTwainControl = NULL;
  }
}

BOOL KTwainControl::Init(HWND hWnd)
{

  if (m_TWAINApp == NULL)
  {
    m_TWAINApp = new TwainApp(hWnd);

    // Set up our Unique Application Identity
    mAppID = m_TWAINApp->getAppIdentity();

    mAppID->Version.MajorNum = 2;
    mAppID->Version.MinorNum = 1;
    mAppID->Version.Language = TWLG_KOREAN; // TWLG_ENGLISH_USA;
    mAppID->Version.Country = TWCY_KOREA; //  TWCY_USA;
    SSTRCPY(mAppID->Version.Info, sizeof(mAppID->Version.Info), "1.0.0");
    mAppID->ProtocolMajor = TWON_PROTOCOLMAJOR;
    mAppID->ProtocolMinor = TWON_PROTOCOLMINOR;
    mAppID->SupportedGroups = DF_APP2 | DG_IMAGE | DG_CONTROL;
    SSTRCPY(mAppID->Manufacturer, sizeof(mAppID->Manufacturer), "K2Mobile.co.kr");
    SSTRCPY(mAppID->ProductFamily, sizeof(mAppID->ProductFamily), "DestinyImaging");
    SSTRCPY(mAppID->ProductName, sizeof(mAppID->ProductName), "DestinyImagingSolution");
  }
  else
  {
    m_TWAINApp->SetParent(hWnd);
  }
  mParentWnd = hWnd;
  return TRUE;
}

BOOL KTwainControl::ConnectDSM()
{
  if (m_TWAINApp->m_DSMState < 3)
  {
    m_TWAINApp->connectDSM();
  }
  return (m_TWAINApp->m_DSMState >= 3);
}

BOOL KTwainControl::DisconnectDSM()
{
  if (m_TWAINApp->m_DSMState > 3)
  {
    m_TWAINApp->unloadDS();
  }
  if (m_TWAINApp->m_DSMState >= 3)
  {
    m_TWAINApp->disconnectDSM();
  }
  return (m_TWAINApp->m_DSMState < 3);
}

int KTwainControl::GetDataSourceList(KImageCaptureDeviceVector& vector)
{
  int count = 0;

  if (m_TWAINApp->m_DSMState < 3)
    m_TWAINApp->connectDSM();

  if (m_TWAINApp->m_DSMState >= 3)
  {
    pTW_IDENTITY pID = NULL;
    int   i = 0;
    int   index = 0;

    while (NULL != (pID = m_TWAINApp->getDataSource((TW_INT16)i)))
    {
      KImageCaptureDevice* dev = new KImageCaptureDevice;
      
      memset(dev, 0, sizeof(KImageCaptureDevice));
      dev->type = K_TWAIN_DEVICE;
      dev->TwainId = pID->Id;
      ToUnicodeString(dev->DeviceName, DEVICE_NAME_BUFF_LEN, pID->ProductName, strlen(pID->ProductName));
      ToUnicodeString(dev->Manufacture, MANUFACTURE_BUFF_LEN, pID->Manufacturer, strlen(pID->Manufacturer));

      TCHAR info[256];
      StringCchPrintf(info, 256, _T("ID:%d, Name:%s, Man:%s, supported:0x%X"),
        dev->TwainId, dev->DeviceName, dev->Manufacture, pID->SupportedGroups);
      StoreLogHistory(_T(__FUNCTION__), info, SYNC_MSG_LOG | SYNC_MSG_DEBUG);

      vector.push_back(dev);
      count++;
      i++;
    }

    m_TWAINApp->disconnectDSM();
  }
  return count;
}

BOOL KTwainControl::LoadDataSource(UINT32 deviceID)
{
  if (m_TWAINApp != NULL)
  {
    // check if it is online
#if 0 // cannot usable before loadDS
    TW_CAPABILITY Cap;
    Cap.Cap = CAP_DEVICEONLINE;
    Cap.hContainer = 0;

    if (TWCC_SUCCESS == m_TWAINApp->get_CAP(Cap, MSG_GET))
    {
      // getCurrent(&Cap, fileformat);
      TW_UINT32 val = 0;
      getCurrent(&Cap, val);
    }
#endif

    m_TWAINApp->loadDS(deviceID);
    return (m_TWAINApp->GetDataSource() != NULL);
  }
  return FALSE;
}

BOOL KTwainControl::UnloadDataSource()
{
  if (m_TWAINApp != NULL)
  {
    m_TWAINApp->unloadDS();
    return TRUE;
  }
  return FALSE;
}

void KTwainControl::GetDataSourceInfo(char *buff, int length)
{
  pTW_IDENTITY source = m_TWAINApp->GetDataSource();
  if (source != NULL)
    sprintf_s(buff, length, "Scanner: %s\nManufacturer: %s\nProductFamily: %s\nInfo: %s",
      source->ProductName, source->Manufacturer, source->ProductFamily,
      source->Version.Info);
  else
    buff[0] = '\0';
}

int KTwainControl::GetState()
{
  if (m_TWAINApp != NULL)
    return m_TWAINApp->GetState();
  return 0;
}

void KTwainControl::SetState(int state)
{
  if (m_TWAINApp != NULL)
  {
    m_TWAINApp->SetState(state);

    TCHAR msg[16];
    StringCchPrintf(msg, 16, _T("%d"), state);
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG | SYNC_MSG_DEBUG);
  }
}

void KTwainControl::ClearDataSourceList(KImageCaptureDeviceVector& vector)
{
  KImageCaptureDeviceIterator it = vector.begin();
  while (it != vector.end())
  {
    KImageCaptureDevice* device = *it;
    delete device;
    it++;
  }
  vector.clear();
}

pTW_IDENTITY KTwainControl::GetDataSource()
{
  if (m_TWAINApp != NULL)
    return m_TWAINApp->GetDataSource();
  return NULL;
}

TW_INT16 KTwainControl::GetCAP(TW_CAPABILITY& cap)
{
  if (m_TWAINApp != NULL)
    return m_TWAINApp->get_CAP(cap, MSG_GET);
  return 0;
}

TW_INT16 KTwainControl::GetLabel(TW_UINT16 cap, string &sLable)
{
  if (m_TWAINApp != NULL)
    return m_TWAINApp->GetLabel(cap, sLable);
  return 0;
}

TW_INT16 KTwainControl::QuerySupport_CAP(TW_UINT16 cap, TW_UINT32& qs)
{
  if (m_TWAINApp != NULL)
    return m_TWAINApp->QuerySupport_CAP(cap, qs);
  return 0;
}

TW_UINT16 KTwainControl::SetCapabilityOneValue(TW_UINT16 Cap, const int value, TW_UINT16 type)
{
  if (m_TWAINApp != NULL)
    return m_TWAINApp->set_CapabilityOneValue(Cap, value, type);
  return 0;
}

TW_UINT16 KTwainControl::SetCapabilityOneValue(TW_UINT16 Cap, const pTW_FIX32 value)
{
  if (m_TWAINApp != NULL)
    return m_TWAINApp->set_CapabilityOneValue(Cap, value);
  return 0;
}

TW_UINT16 KTwainControl::SetCapabilityOneValue(TW_UINT16 Cap, const pTW_FRAME value)
{
  if (m_TWAINApp != NULL)
    return m_TWAINApp->set_CapabilityOneValue(Cap, value);
  return 0;
}

BOOL KTwainControl::EnableADF(HWND hWnd, BOOL enableADF)
{
  if (m_TWAINApp != NULL)
  {
    TW_UINT16 r = m_TWAINApp->set_CapabilityOneValue(CAP_FEEDERENABLED, enableADF, TWTY_BOOL);
    if (r == TWRC_SUCCESS)
    {
      r = m_TWAINApp->set_CapabilityOneValue(CAP_AUTOFEED, enableADF, TWTY_BOOL);

      StoreLogHistory(_T(__FUNCTION__), _T("ADF enabled"), SYNC_MSG_LOG | SYNC_MSG_DEBUG);
      return TRUE;
    }
  }
  return FALSE;
}

BOOL KTwainControl::EnableDS(TW_HANDLE hWnd, BOOL bShowUI)
{
  if (m_TWAINApp != NULL)
  {
    if (m_TWAINApp->enableDS(hWnd, bShowUI))
    {
      StoreLogHistory(_T(__FUNCTION__), _T("DS enabled"), SYNC_MSG_LOG | SYNC_MSG_DEBUG);
      return TRUE;
    }
  }
  return FALSE;
}

BOOL KTwainControl::DisableDS()
{
  if (m_TWAINApp != NULL)
  {
    m_TWAINApp->disableDS();
    StoreLogHistory(_T(__FUNCTION__), _T("DS disabled"), SYNC_MSG_LOG | SYNC_MSG_DEBUG);
    return TRUE;
  }
  return FALSE;
}

BOOL KTwainControl::GetNativeBitmaps(KVoidPtrArray& imageArray)
{
  bool bPendingXfers = true;
  TW_UINT16   twrc = TWRC_SUCCESS;

  while (bPendingXfers)
  {
    TW_MEMREF hImg = 0;
    twrc = m_TWAINApp->DSM_Entry(DG_IMAGE, DAT_IMAGENATIVEXFER, MSG_GET, (TW_MEMREF)&hImg);

    if (TWRC_XFERDONE == twrc)
    {
      PBITMAPINFOHEADER pDIB = (PBITMAPINFOHEADER)_DSM_LockMemory(hImg);
      DWORD dwPaletteSize = 0;

      switch (pDIB->biBitCount)
      {
      case 1:
        dwPaletteSize = 2;
        break;
      case 8:
        dwPaletteSize = 256;
        break;
      case 24:
        break;
      default:
        break;
      }

      if (pDIB->biSizeImage == 0)
      {
        pDIB->biSizeImage = ((((pDIB->biWidth * pDIB->biBitCount) + 31) & ~31) / 8) * pDIB->biHeight;

        // If a compression scheme is used the result may infact be larger
        // Increase the size to account for this.
        if (pDIB->biCompression != 0)
        {
          pDIB->biSizeImage = (pDIB->biSizeImage * 3) / 2;
        }
      }

      int nImageSize = pDIB->biSizeImage + (sizeof(RGBQUAD)*dwPaletteSize) + sizeof(BITMAPINFOHEADER);
      HANDLE hb = GlobalAlloc(0, nImageSize);
      LPBYTE buff = (LPBYTE)GlobalLock(hb); // new BYTE[nImageSize];
      memcpy(buff, pDIB, nImageSize);
      GlobalUnlock(hb);

      imageArray.push_back(hb);

      _DSM_UnlockMemory(hImg);
      _DSM_Free(hImg);

      // check more images exist
      TW_PENDINGXFERS pendxfers;
      memset(&pendxfers, 0, sizeof(pendxfers));

      twrc = m_TWAINApp->DSM_Entry(DG_CONTROL, DAT_PENDINGXFERS, MSG_ENDXFER, (TW_MEMREF)&pendxfers);

      if (TWRC_SUCCESS == twrc)
      {
        if (0 == pendxfers.Count)
        {
          bPendingXfers = false;
        }
      }
      else
      {
        bPendingXfers = false;
      }
    }
    else if (TWRC_CANCEL == twrc)
    {
      StoreLogHistory(_T(__FUNCTION__), _T("Canceled transfer image"), SYNC_MSG_LOG | SYNC_MSG_DEBUG);
      break;
    }
    else if (TWRC_FAILURE == twrc)
    {
      StoreLogHistory(_T(__FUNCTION__), _T("Failed to transfer image"), SYNC_MSG_LOG | SYNC_MSG_DEBUG);
      break;
    }
  }

  if (bPendingXfers == true)
    twrc = m_TWAINApp->DoAbortXfer();

  // adjust our state now that the scanning session is done
  m_TWAINApp->SetState(5);
  return TRUE;
}

void KTwainControl::OnDSMessage(TW_UINT16 msg)
{
  if (mParentWnd == NULL)
  {
    StoreLogHistory(_T(__FUNCTION__), _T("No parent to send MSG"), SYNC_MSG_LOG | SYNC_MSG_DEBUG);
    return;
  }

  switch (msg)
  {
  case MSG_XFERREADY:
  case MSG_CLOSEDSREQ:
  case MSG_CLOSEDSOK:
    ::PostMessage(mParentWnd, WM_KTWAIN_MSG, (WPARAM)msg, 0);
  case MSG_NULL:
    break;
  }
}

BOOL KTwainControl::initiateTransfer_File(TW_UINT16 fileformat, KVoidPtrArray* pArray, LPCTSTR workingPath)
{
  LPCSTR path = MakeUtf8String(workingPath);
  m_TWAINApp->SetPath(path);
  delete[] path;

  if ((workingPath != NULL) && !IsDirectoryExist(workingPath))
    CreateDirectory(workingPath);

  m_TWAINApp->initiateTransfer_File(fileformat, pArray);
  return TRUE;
}

BOOL KTwainControl::initiateTransfer_Memory()
{
  m_TWAINApp->initiateTransfer_Memory();
  return TRUE;
}