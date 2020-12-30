#include "stdafx.h"
#include "KWiaTransferCallback.h"
#include "..\..\Core\KSettings.h"


static const GUID IID_KWIA_TRANSFER_CB = { 0x59f4a881, 0x5464, 0x4409,{ 0x90, 0x52, 0x8c, 0x1d, 0x15, 0x12, 0x8e, 0xfa } };

KWiaTransferCallback::KWiaTransferCallback()
{
  mDownloadFolder[0] = '\0';
  mFilename[0] = '\0';
  mFileExtension[0] = '\0';
  mIndex = 0;
}


KWiaTransferCallback::~KWiaTransferCallback()
{
}

HRESULT STDMETHODCALLTYPE KWiaTransferCallback::QueryInterface(REFIID riid, void **ppvObject)
{
  HRESULT r = S_OK;
  *ppvObject = NULL;

  //  Multiple inheritance requires an explicit cast
  if (riid == IID_IUnknown)
    *ppvObject = (KWiaTransferCallback*)this;
  else if (riid == IID_KWIA_TRANSFER_CB)
    *ppvObject = (KWiaTransferCallback*)this;
  else
    r = E_NOINTERFACE;

  //Return a pointer to the new interface and thus call AddRef() for the new index
  if (r == S_OK)
    this->AddRef();
  return r;
}

ULONG STDMETHODCALLTYPE KWiaTransferCallback::AddRef()
{
  InterlockedIncrement(&RefCount);
  return RefCount;
}

ULONG STDMETHODCALLTYPE KWiaTransferCallback::Release()
{
  ULONG ulRefCount = InterlockedDecrement(&RefCount);
  if (RefCount == 0)
    delete this;
  return ulRefCount;
}

void STDMETHODCALLTYPE KWiaTransferCallback::SetOption(LPCTSTR downloadFolder, LPCTSTR extension)
{
  StringCchCopy(mDownloadFolder, FILENAME_MAX, downloadFolder);
  StringCchCopy(mFileExtension, 6, extension);
}

HRESULT STDMETHODCALLTYPE KWiaTransferCallback::TransferCallback(LONG lFlags, WiaTransferParams *pWiaTransferParams)
{
  HRESULT hr = S_OK;

  switch (pWiaTransferParams->lMessage)
  {
  case WIA_TRANSFER_MSG_STATUS:
      break;
  case WIA_TRANSFER_MSG_END_OF_STREAM:
      break;
  case WIA_TRANSFER_MSG_END_OF_TRANSFER:
      break;
  default:
    break;
  }

  // skip status
  if ((pWiaTransferParams->lMessage > WIA_TRANSFER_MSG_STATUS) &&
    (pWiaTransferParams->lMessage <= WIA_TRANSFER_MSG_END_OF_TRANSFER))
  {
    static LPTSTR wia_cb_msgname[] = {
      _T("Status"),
      _T("EndOfStream"),
      _T("EndOfTransfer"),
      _T("Unknown"),
    };

    TCHAR msg[64];
    int logflag = SYNC_MSG_LOG;

    int sid = pWiaTransferParams->lMessage - WIA_TRANSFER_MSG_STATUS;
    StringCchPrintf(msg, 64, _T("Msg=%s"), wia_cb_msgname[sid]);
    /*
    if (pWiaTransferParams->lMessage == WIA_TRANSFER_MSG_STATUS)
    {
      TCHAR str[32];
      StringCchPrintf(str, 32, _T(", %d"), pWiaTransferParams->lPercentComplete);
      StringCchCat(msg, 64, str);
      logflag = SYNC_MSG_LOG | SYNC_MSG_DEBUG;
    }
    */
    StoreLogHistory(_T(__FUNCTION__), msg, logflag);
  }

  return hr;
}

HRESULT STDMETHODCALLTYPE KWiaTransferCallback::GetNextStream(LONG lFlags, BSTR bstrItemName, BSTR bstrFullItemName, IStream **ppDestination)
{
  HRESULT hr = S_OK;

  //  Return a new stream for this item's data.
  StringCchPrintf(mFilename, FILENAME_MAX, _T("%s\\%s-%d.%s"), mDownloadFolder,
    bstrItemName, mIndex, mFileExtension);

  hr = SHCreateStreamOnFile(mFilename, STGM_CREATE|STGM_WRITE, ppDestination);

  TCHAR msg[300];
  StringCchPrintf(msg, 300, _T("Name=%s, hr=0x%x"), mFilename, hr);
  StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);

  return hr;
}