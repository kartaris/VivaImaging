#include "stdafx.h"
#include "KWiaControl.h"
#include "KWiaTransferCallback.h"

#include "..\..\Core\KSettings.h"
#include "..\Platform.h"

#include <Sti.h>
#include <atlsafe.h>

KWiaControl::KWiaControl()
{
  m_WiaDevMgr = NULL;
  mParentWnd = NULL;
}


KWiaControl::~KWiaControl()
{
  ReleaseManager();
}

BOOL KWiaControl::Init(HWND hWnd)
{
  mParentWnd = hWnd;
  return TRUE;
}

void KWiaControl::CheckReadyManager()
{
  if (m_WiaDevMgr == NULL)
  {
    HRESULT h = K_CoInitializeEx(0, COINIT_APARTMENTTHREADED);
    if (FAILED(h))
      StoreLogHistory(_T(__FUNCTION__), h, SYNC_MSG_LOG);

    HRESULT hr = CoCreateInstance(CLSID_WiaDevMgr2, NULL, CLSCTX_LOCAL_SERVER, IID_IWiaDevMgr2, (LPVOID*)&m_WiaDevMgr);

    if (FAILED(hr) || (m_WiaDevMgr == NULL))
    {
      StoreLogHistory(_T(__FUNCTION__), hr, SYNC_MSG_LOG);
    }
    else
    {
      TCHAR msg[64];
      StringCchPrintf(msg, 64, _T("Mgr=0x%x"), m_WiaDevMgr);
      StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
    }
  }
}

void KWiaControl::ReleaseManager()
{
  if (m_WiaDevMgr != NULL)
  {
    m_WiaDevMgr->Release();
    K_CoUninitialize();
    m_WiaDevMgr = NULL;
  }
}

void KWiaControl::GetDataSourceList(KImageCaptureDeviceVector& vector)
{
  CheckReadyManager();
  if (m_WiaDevMgr != NULL)
  {
    IEnumWIA_DEV_INFO *pWiaEnumDevInfo = NULL;
    HRESULT hr = m_WiaDevMgr->EnumDeviceInfo(WIA_DEVINFO_ENUM_LOCAL, &pWiaEnumDevInfo);

    if (SUCCEEDED(hr))
      hr = pWiaEnumDevInfo->Reset();

    if (SUCCEEDED(hr))
    {
      while (S_OK == hr)
      {
        IWiaPropertyStorage *pWiaPropertyStorage = NULL;
        hr = pWiaEnumDevInfo->Next(1, &pWiaPropertyStorage, NULL);

        //
        // pWiaEnumDevInfo->Next will return S_FALSE when the list is
        // exhausted, so check for S_OK before using the returned
        // value.
        //
        if (hr == S_OK)
        {
          PROPSPEC PropSpec[5] = { 0 };
          PROPVARIANT PropVar[5] = { 0 };

          PropSpec[0].ulKind = PRSPEC_PROPID;
          PropSpec[0].propid = WIA_DIP_DEV_ID;
          PropSpec[1].ulKind = PRSPEC_PROPID;
          PropSpec[1].propid = WIA_DIP_DEV_NAME;
          PropSpec[2].ulKind = PRSPEC_PROPID;
          PropSpec[2].propid = WIA_DIP_VEND_DESC;
          PropSpec[3].ulKind = PRSPEC_PROPID;
          PropSpec[3].propid = WIA_DIP_DEV_DESC;
          PropSpec[4].ulKind = PRSPEC_PROPID;
          PropSpec[4].propid = WIA_DIP_DEV_TYPE; // WIA_DIP_PORT_NAME;


          hr = pWiaPropertyStorage->ReadMultiple(5, PropSpec, PropVar);

          if (SUCCEEDED(hr))
          {
            KImageCaptureDevice* device = new KImageCaptureDevice;

            memset(device, 0, sizeof(KImageCaptureDevice));
            device->type = K_WIA_DEVICE;

            TCHAR devdesc[32] = _T("?");
            TCHAR portname[32] = _T("unknown");
            // name
            if (VT_BSTR == PropVar[0].vt)
            {
              device->bstrDeviceID = SysAllocString(PropVar[0].bstrVal);
              StringCchCopy(device->DeviceIDS, DEVICE_SID_BUFF_LEN, PropVar[0].bstrVal);
            }
            if (VT_BSTR == PropVar[1].vt)
              StringCchCopy(device->DeviceName, DEVICE_NAME_BUFF_LEN, PropVar[1].bstrVal);
            if (VT_BSTR == PropVar[2].vt)
              StringCchCopy(device->Manufacture, MANUFACTURE_BUFF_LEN, PropVar[2].bstrVal);

            if (VT_BSTR == PropVar[3].vt)
              StringCchCopy(devdesc, MANUFACTURE_BUFF_LEN, PropVar[3].bstrVal);
            if (VT_I4 == PropVar[4].vt)
              StringCchPrintf(portname, 32, _T("%d"), PropVar[4].intVal);

            TCHAR info[256];
            StringCchPrintf(info, 256, _T("ID:%s, Name:%s, Man:%s, dev:%s, type:%s"),
              device->DeviceIDS, device->DeviceName, device->Manufacture, devdesc, portname);

            StoreLogHistory(_T(__FUNCTION__), info, SYNC_MSG_LOG | SYNC_MSG_DEBUG);

            FreePropVariantArray(5, PropVar);

            vector.push_back(device);
          }

          pWiaPropertyStorage->Release();
          pWiaPropertyStorage = NULL;
        }
      }

      //
      // If the result of the enumeration is S_FALSE (which
      // is normal), change it to S_OK.
      //
      if (S_FALSE == hr)
      {
        hr = S_OK;
      }

      //
      // Release the enumerator
      //
      pWiaEnumDevInfo->Release();
      pWiaEnumDevInfo = NULL;
    }
  }
  ReleaseManager();
}

static void getWiaItemTypeStr(LPTSTR str, int len, LONG itemType)
{
  if (itemType & WiaItemTypeRoot)
    StringCchCat(str, len, _T("WiaRoot,"));
  if (itemType & WiaItemTypeDevice)
    StringCchCat(str, len, _T("WiaDevice,"));
  if (itemType & WiaItemTypeDocument)
    StringCchCat(str, len, _T("WiaDocument,"));
  if (itemType & WiaItemTypeFile)
    StringCchCat(str, len, _T("WiaFile,"));
  if (itemType & WiaItemTypeFolder)
    StringCchCat(str, len, _T("WiaFolder,"));
  if (itemType & WiaItemTypeImage)
    StringCchCat(str, len, _T("WiaImage,"));
  if (itemType & WiaItemTypeTransfer)
    StringCchCat(str, len, _T("WiaTransfer,"));
}

static void GetDocumentHandleCapStr(LPTSTR str, int len, UINT cap)
{
  str[0] = '\0';

  if (FILM_TPA & cap)
    StringCchCat(str, 256, _T(",FILM_TPA"));
  if (DETECT_FILM_TPA & cap)
    StringCchCat(str, 256, _T(",DETECT_FILM_TPA"));
  if (STOR & cap)
    StringCchCat(str, 256, _T(",STOR"));
  if (DETECT_STOR & cap)
    StringCchCat(str, 256, _T(",DETECT_STOR"));
  if (ADVANCED_DUP & cap)
    StringCchCat(str, 256, _T(",ADVANCED_DUP"));
  if (AUTO_SOURCE & cap)
    StringCchCat(str, 256, _T(",AUTO_SOURCE"));
  if (IMPRINTER & cap)
    StringCchCat(str, 256, _T(",IMPRINTER"));
  if (ENDORSER & cap)
    StringCchCat(str, 256, _T(",ENDORSER"));
  if (BARCODE_READER & cap)
    StringCchCat(str, 256, _T(",BARCODE_READER"));
  if (PATCH_CODE_READER & cap)
    StringCchCat(str, 256, _T(",PATCH_CODE_READER"));
  if (MICR_READER & cap)
    StringCchCat(str, 256, _T(",MICR_READER"));

  if (DETECT_DUP_AVAIL & cap)
    StringCchCat(str, 256, _T(",DETECT_DUP_AVAIL"));
  if (DETECT_FEED_AVAIL & cap)
    StringCchCat(str, 256, _T(",DETECT_FEED_AVAIL"));
  if (DETECT_DUP & cap)
    StringCchCat(str, 256, _T(",DETECT_DUP"));
  if (DETECT_FEED & cap)
    StringCchCat(str, 256, _T(",DETECT_FEED"));
  if (DETECT_SCAN & cap)
    StringCchCat(str, 256, _T(",DETECT_SCAN"));
  if (DETECT_FLAT & cap)
    StringCchCat(str, 256, _T(",DETECT_FLAT"));
  if (DUP & cap)
    StringCchCat(str, 256, _T(",DUP"));
  if (FLAT & cap)
    StringCchCat(str, 256, _T(",FLAT"));
  if (FEED & cap)
    StringCchCat(str, 256, _T(",FEED"));

}

static LPTSTR wia2CategoryNames[] = {
  _T("Auto"),
  _T("Feeder"),
  _T("FeederBack"),
  _T("FeederFront"),
  _T("Film"),
  _T("File"),
  _T("Flatbed"),
  _T("Folder"),
  _T("Root"),
  _T("Unknown")
};

static LPTSTR GetWia2CategoryName(GUID categoryGUID)
{
  int i = 9;
  static GUID wia2_categories[] = {
  WIA_CATEGORY_AUTO,
  WIA_CATEGORY_FEEDER,
  WIA_CATEGORY_FEEDER_BACK,
  WIA_CATEGORY_FEEDER_FRONT,
  WIA_CATEGORY_FILM,
  WIA_CATEGORY_FINISHED_FILE,
  WIA_CATEGORY_FLATBED,
  WIA_CATEGORY_FOLDER,
  WIA_CATEGORY_ROOT
  };

  for (int i = 0; i < 9; i++)
  {
    if (categoryGUID == wia2_categories[i])
      return wia2CategoryNames[i];
  }
  return wia2CategoryNames[9];
}

static void dumpWiaItem(IWiaItem2 *pWiaItem, int depth)
{
  TCHAR str[256] = {};

  LONG lItemType = 0;
  HRESULT hr = pWiaItem->GetItemType(&lItemType);

  for (int i = 0; i < depth; i++)
    str[i] = ' ';
  str[depth] = '\0';
  getWiaItemTypeStr(str, 256, lItemType);

  IWiaPropertyStorage* pWiaPropertyStorage;
  hr = pWiaItem->QueryInterface(IID_IWiaPropertyStorage, (void**)&pWiaPropertyStorage);
  if (SUCCEEDED(hr))
  {
    PROPSPEC PropSpec[5] = { 0 };
    PROPVARIANT PropVar[5] = { 0 };

    if (lItemType & WiaItemTypeRoot)
    {
      PropSpec[0].ulKind = PRSPEC_PROPID;
      PropSpec[0].propid = WIA_DIP_DEV_ID;
      PropSpec[1].ulKind = PRSPEC_PROPID;
      PropSpec[1].propid = WIA_DIP_DEV_NAME;
      PropSpec[2].ulKind = PRSPEC_PROPID;
      PropSpec[2].propid = WIA_DIP_DEV_DESC;
      PropSpec[3].ulKind = PRSPEC_PROPID;
      PropSpec[3].propid = WIA_DIP_DEV_TYPE;
      PropSpec[4].ulKind = PRSPEC_PROPID;
      PropSpec[4].propid = WIA_DPS_DOCUMENT_HANDLING_CAPABILITIES;

      hr = pWiaPropertyStorage->ReadMultiple(5, PropSpec, PropVar);

      if (SUCCEEDED(hr))
      {
        if (VT_BSTR == PropVar[0].vt)
        {
          StringCchCat(str, 256, _T(",ID:"));
          StringCchCat(str, 256, (LPTSTR)PropVar[0].bstrVal);
        }
        if (VT_BSTR == PropVar[1].vt)
        {
          StringCchCat(str, 256, _T(",Name:"));
          StringCchCat(str, 256, PropVar[1].bstrVal);
        }
        if (VT_BSTR == PropVar[2].vt)
        {
          StringCchCat(str, 256, _T(",Desc:"));
          StringCchCat(str, 256, PropVar[2].bstrVal);
        }
        if (VT_I4 == PropVar[3].vt)
        {
          TCHAR s[16];
          static LPTSTR deviceTypeName[] = {
            _T("Default"),
            _T("Scanner"),
            _T("Camera"),
            _T("Video")
          };
          int type = (PropVar[3].intVal & 0x03);
          if (type > 3)
            type = 3;
          StringCchPrintf(s, 16, _T(",Type:%s"), deviceTypeName[type]);
          StringCchCat(str, 256, s);
        }
        if (VT_I4 == PropVar[4].vt)
        {
          TCHAR s[64];
          GetDocumentHandleCapStr(s, 64, PropVar[4].intVal);
          StringCchCat(str, 256, s);
        }
      }
      GUID itemCategoryGUID;
      hr = pWiaItem->GetItemCategory(&itemCategoryGUID);
      if (SUCCEEDED(hr))
      {
        StringCchCat(str, 256, _T(",Category:"));
        StringCchCat(str, 256, GetWia2CategoryName(itemCategoryGUID));
      }
      StoreLogHistory(_T(__FUNCTION__), str, SYNC_MSG_LOG | SYNC_MSG_DEBUG);

      for (int i = 0; i < 5; i++)
        PropVariantClear(&PropVar[i]);
    }

    if (lItemType & WiaItemTypeImage)
    {
      PropSpec[0].ulKind = PRSPEC_PROPID;
      PropSpec[0].propid = WIA_DPS_DOCUMENT_HANDLING_CAPABILITIES;
      PropSpec[1].ulKind = PRSPEC_PROPID;
      PropSpec[1].propid = WIA_IPS_ORIENTATION;
      PropSpec[2].ulKind = PRSPEC_PROPID;
      PropSpec[2].propid = WIA_IPS_PAGE_SIZE;
      PropSpec[3].ulKind = PRSPEC_PROPID;
      PropSpec[3].propid = WIA_IPS_PAGES;
      PropSpec[4].ulKind = PRSPEC_PROPID;
      PropSpec[4].propid = WIA_IPS_ROTATION;

      hr = pWiaPropertyStorage->ReadMultiple(5, PropSpec, PropVar);
      str[0] = '\0';

      GUID itemCategoryGUID;
      hr = pWiaItem->GetItemCategory(&itemCategoryGUID);
      if (SUCCEEDED(hr))
      {
        StringCchCat(str, 256, _T(",Category:"));
        StringCchCat(str, 256, GetWia2CategoryName(itemCategoryGUID));
        /*
        OLECHAR* guidString;
        StringFromCLSID(itemCategoryGUID, &guidString);
        StringCchCat(str, 256, _T("CategoryGUID:"));
        StringCchCat(str, 256, guidString);
        ::CoTaskMemFree(guidString);
        */
      }

      if (SUCCEEDED(hr))
      {
        if (VT_I4 == PropVar[0].vt)
        {
          TCHAR s[16];
          StringCchPrintf(s, 16, _T(",DocHandleCap:%d"), PropVar[0].intVal);
          StringCchCat(str, 256, s);
        }
        if (VT_I4 == PropVar[1].vt)
        {
          TCHAR s[16];
          StringCchPrintf(s, 16, _T(",Orientation:%d"), PropVar[1].intVal);
          StringCchCat(str, 256, s);
        }
        if (VT_I4 == PropVar[2].vt)
        {
          TCHAR s[16];
          StringCchPrintf(s, 16, _T(",PageSize:%d"), PropVar[2].intVal);
          StringCchCat(str, 256, s);
        }
        if (VT_I4 == PropVar[3].vt)
        {
          TCHAR s[16];
          StringCchPrintf(s, 16, _T(",Pages:%d"), PropVar[3].intVal);
          StringCchCat(str, 256, s);
        }
        if (VT_I4 == PropVar[4].vt)
        {
          TCHAR s[16];
          StringCchPrintf(s, 16, _T(",Rotation:%d"), PropVar[4].intVal);
          StringCchCat(str, 256, s);
        }
        StoreLogHistory(_T(__FUNCTION__), str, SYNC_MSG_LOG | SYNC_MSG_DEBUG);
      }

      for (int i = 0; i < 5; i++)
        PropVariantClear(&PropVar[i]);

    }

    pWiaPropertyStorage->Release();
  }
}

IWiaItem2* EnumerateChildItems(IWiaItem2 *pWiaItem, LONG needed_type, BOOL dump, int depth)
{
  if (NULL == pWiaItem)
  {
    return NULL;
  }

  LONG lItemType = 0;
  HRESULT hr = pWiaItem->GetItemType(&lItemType);
  
  if (SUCCEEDED(hr))
  {
    if (dump)
    {
      dumpWiaItem(pWiaItem, depth);
    }

    if (needed_type & lItemType)
      return pWiaItem;

    // If it is a folder, or it has attachments, enumerate its children.
    if (lItemType & WiaItemTypeFolder || lItemType & WiaItemTypeHasAttachments)
    {
      // Get the child item enumerator for this item.
      //IEnumWiaItem *pEnumWiaItem = NULL; //XP or earlier
      IEnumWiaItem2 *pEnumWiaItem = NULL; //Vista or later

      hr = pWiaItem->EnumChildItems(NULL, &pEnumWiaItem);
      if (SUCCEEDED(hr))
      {
        // Loop until you get an error or pEnumWiaItem->Next returns
        // S_FALSE to signal the end of the list.
        while (S_OK == hr)
        {
          // Get the next child item.
          //IWiaItem *pChildWiaItem = NULL; //XP or earlier
          IWiaItem2 *pChildWiaItem = NULL; //Vista or later

          hr = pEnumWiaItem->Next(1, &pChildWiaItem, NULL);

          // pEnumWiaItem->Next will return S_FALSE when the list is
          // exhausted, so check for S_OK before using the returned
          // value.
          if (S_OK == hr)
          {
            hr = pChildWiaItem->GetItemType(&lItemType);
            if ((S_OK == hr) && (needed_type & lItemType))
              return pChildWiaItem;


            // Recurse into this item.
            IWiaItem2* r = EnumerateChildItems(pChildWiaItem, needed_type, dump, depth+1);

            // Release this item.
            pChildWiaItem->Release();
            pChildWiaItem = NULL;

            if (r != NULL)
              return r;
          }
        }

        //
        // If the result of the enumeration is S_FALSE (which
        // is normal), change it to S_OK.
        //
        if (S_FALSE == hr)
        {
          hr = S_OK;
        }

        // Release the enumerator.
        pEnumWiaItem->Release();
        pEnumWiaItem = NULL;
      }
    }
  }
  return  NULL;
}

#define WIA_BY_DEVICE_DLG

int KWiaControl::ScanImage(HWND hWnd, BSTR deviceIDS, LPTSTR workingPath)
{
  HRESULT hr;

  CheckReadyManager();
  AfxOleGetMessageFilter()->SetMessagePendingDelay(60000); // 30 sec

  if (m_WiaDevMgr != NULL)
  {
    IWiaItem2* wiaItem2Root = NULL;

    hr = m_WiaDevMgr->CreateDevice(0, deviceIDS, &wiaItem2Root);
    if (FAILED(hr))
    {
      StoreLogHistory(_T(__FUNCTION__), hr, SYNC_MSG_LOG | SYNC_MSG_DEBUG);
      ReleaseManager();
      return SCAN_DEVICE_NOT_READY;
    }

    if (wiaItem2Root == NULL)
    {
      ReleaseManager();
      return SCAN_WIA_NO_INTERFACE;
    }

    // dump
    //EnumerateChildItems(wiaItem2Root, 0, TRUE, 0);
    int rtn = SCAN_SUCCESS;

#ifdef WIA_BY_DEVICE_DLG
    LONG numberOfFiles = 0;
    BSTR* pFilenames = new BSTR[0];
    IWiaItem2* target[10] = { NULL };

    if (!IsDirectoryExist(workingPath))
      CreateDirectory(workingPath);

    CComBSTR folderName = SysAllocString(workingPath);
    CComBSTR fileName("scan");

    hr = wiaItem2Root->DeviceDlg(0, mParentWnd, folderName, fileName,
      &numberOfFiles, &pFilenames, target);

    if (SUCCEEDED(hr))
    {
      for (int i = 0; i < numberOfFiles; i++)
      {
        LPTSTR resultFilename = AllocString(pFilenames[i]);
        ::PostMessage(mParentWnd, WM_KWIA_MSG, (WPARAM)1, (LPARAM)resultFilename);

        SysFreeString(pFilenames[i]);

        if (target[i] != NULL)
          target[i]->Release();
      }

      /*
      if ((target[0] != NULL) && (numberOfFiles == 0))
      {
        IWiaTransfer *pWiaTransfer = NULL;
        hr = target[0]->QueryInterface(IID_IWiaTransfer, (void**)&pWiaTransfer);
        if (SUCCEEDED(hr))
        {
          KWiaTransferCallback *pWiaClassCallback = new KWiaTransferCallback;
          if (pWiaClassCallback)
          {
            pWiaClassCallback->SetOption(_T("C:\\Temp"), _T("BMP"));

            hr = pWiaTransfer->Download(0, pWiaClassCallback);
          }

          pWiaTransfer->Release();
        }
        else
        {
          StoreLogHistory(_T(__FUNCTION__), hr, SYNC_MSG_LOG | SYNC_MSG_DEBUG);
        }
        target[0]->Release();
      }
      */

      if (pFilenames != NULL)
        CoTaskMemFree(pFilenames);
    }
    else
    {
      StoreLogHistory(_T(__FUNCTION__), hr, SYNC_MSG_LOG | SYNC_MSG_DEBUG);
    }
#else
    // find target items
    IWiaItem2* target = EnumerateChildItems(wiaItem2Root, WiaItemTypeFile, FALSE, 0);
    if (target != NULL)
    {
      IWiaTransfer *pWiaTransfer = NULL;
      hr = target->QueryInterface(IID_IWiaTransfer, (void**)&pWiaTransfer);

      if (SUCCEEDED(hr))
      {
        LPTSTR resultFilename = NULL;
        //
        // Create our callback class
        //
        KWiaTransferCallback *pWiaClassCallback = new KWiaTransferCallback;
        if (pWiaClassCallback)
        {
          pWiaClassCallback->SetOption(_T("C:\\Temp"), _T("BMP"));

          hr = pWiaTransfer->Download(0, pWiaClassCallback);

          if (FAILED(hr))
            rtn = SCAN_WIA_FAIL_DOWNLOAD;

          TCHAR msg[256] = _T("WIA Download : ");
          //StringCchPrintf(msg, 32, _T("Msg=%d"), pWiaTransferParams->lMessage);

          if (pWiaClassCallback->mFilename != NULL)
          {
            resultFilename = AllocString(pWiaClassCallback->mFilename);
            StringCchCat(msg, 256, pWiaClassCallback->mFilename);
          }
          else
          {
            StringCchCat(msg, 256, _T("empty"));
          }
          StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);

          pWiaClassCallback->Release();
          delete pWiaClassCallback;
        }

        //
        // Release the IWiaDataTransfer
        //
        if (pWiaTransfer != NULL)
        {
          pWiaTransfer->Release();
          pWiaTransfer = NULL;
        }

        if (resultFilename != NULL)
          ::PostMessage(mParentWnd, WM_KWIA_MSG, (WPARAM)1, (LPARAM)resultFilename);
      }
      else
      {
        rtn = SCAN_WIA_NO_INTERFACE;
      }

      target->Release();
    }
#endif
    wiaItem2Root->Release();
    ReleaseManager();
    return rtn;
  }
  return SCAN_WIA_NOT_READY;
}