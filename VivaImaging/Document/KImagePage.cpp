#include "stdafx.h"
#include <string.h>
#include "KImagePage.h"
#include "KImageFolder.h"
#include "wincodecsdk.h"
#include "propvarutil.h"
#include "../Core/KConstant.h"
#include "../Core/KMetadataInfo.h"
#include "../Core/HttpDownloader/KMemory.h"
#include "../Platform/crypting.h"

int KImagePage::g_DefaultPageNameIndex = 1;

KImagePage::KImagePage()
  : KImageBase(NULL)
{
  m_LocalFilename = NULL;
  m_TempFilename = NULL;
  m_MultiPageIndex = 0;
  m_Tag = NULL;
  mHasEffect = FALSE;
  mpPreviewEffect = NULL;
  m_Image = NULL;
  m_CacheImage = NULL;
  m_ThumbImage = NULL;
}

KImagePage::KImagePage(KImageBase* p)
  : KImageBase(p)
{
  m_LocalFilename = NULL;
  m_TempFilename = NULL;
  m_MultiPageIndex = 0;
  m_Tag = NULL;
  mHasEffect = FALSE;
  mpPreviewEffect = NULL;
  m_Image = NULL;
  m_CacheImage = NULL;
  m_ThumbImage = NULL;
}

KImagePage::KImagePage(KImageBase* p, LPCTSTR pFilename)
  : KImageBase(p)
{
  m_LocalFilename = NULL;
  m_TempFilename = NULL;
  m_MultiPageIndex = 0;
  m_Tag = NULL;
  mHasEffect = FALSE;
  mpPreviewEffect = NULL;
  m_Image = NULL;
  m_CacheImage = NULL;
  m_ThumbImage = NULL;

  if (pFilename != NULL)
  {
    int len = lstrlen(pFilename) + 1;
    m_LocalFilename = new TCHAR[len];
    StringCchCopy(m_LocalFilename, len, pFilename);

    LPCTSTR n = StrRChr(pFilename, NULL, '\\');
    if (n != NULL)
      n++;
    else
      n = pFilename;
    SetName(n);
  }
}

KImagePage::KImagePage(KImageBase* p, PBITMAPINFOHEADER pDIB)
  : KImageBase(p)
{
  m_LocalFilename = NULL;
  m_TempFilename = NULL;
  m_MultiPageIndex = 0;
  m_Tag = NULL;
  mHasEffect = FALSE;
  mpPreviewEffect = NULL;
  m_Image = NULL;
  m_ThumbImage = NULL;
  m_Image = new KBitmap();
  m_Image->LoadImageFromDIB(pDIB);
  m_CacheImage = NULL;

  m_Annotates.SetSize(m_Image->width, m_Image->height);
}

KImagePage::~KImagePage()
{
  Clear();
}

void KImagePage::Clear()
{
  if (m_LocalFilename != NULL)
  {
    delete m_LocalFilename;
    m_LocalFilename = NULL;
  }
  if (m_TempFilename != NULL)
  {
    delete m_TempFilename;
    m_TempFilename = NULL;
  }
  if (m_Tag != NULL)
  {
    delete m_Tag;
    m_Tag = NULL;
  }
  if (m_Image != NULL)
  {
    delete m_Image;
    m_Image = NULL;
  }
  if (m_CacheImage != NULL)
  {
    delete m_CacheImage;
    m_CacheImage = NULL;
  }
  if (m_ThumbImage != NULL)
  {
    delete m_ThumbImage;
    m_ThumbImage = NULL;
  }
  if (mpPreviewEffect != NULL)
  {
    delete mpPreviewEffect;
    mpPreviewEffect = NULL;
  }
  KImageBase::Clear();
}

void KImagePage::SetModified(BOOL bRecurr, PageModifiedFlag modified)
{
  KImageBase::SetModified(bRecurr, modified);
  if (modified & PageModifiedFlag::IMAGE_MODIFIED)
  {
    // clear thumb image to update
    if (m_ThumbImage != NULL)
    {
      delete m_ThumbImage;
      m_ThumbImage = NULL;
    }
  }
}

BOOL KImagePage::IsEditable(int flag)
{
  if (mPermission & SFP_WRITE)
  {
    if (flag & EDITABLE_TRUECOLOR)
      return ((m_Image != NULL) && (m_Image->bitCount >= 24));
    return TRUE;
  }
  return FALSE;
}

KBitmap* KImagePage::GetImage(LPCTSTR folderPath, ImageEffectJob* effect, BOOL originImage)
{
  if (originImage && (m_Image != NULL))
  {
    return m_Image;
  }

#ifdef _RENDER_BY_BG_THREAD
  if (m_Image == NULL)
    renderImage(folderPath, effect);
#else
  if (m_CacheImage == NULL)
    renderImage(folderPath, effect);
#endif

  return((m_CacheImage != NULL) ? m_CacheImage : m_Image);
};

BOOL KImagePage::IsNeedRenderThumbImage(LPTSTR folderPath, int cx, int cy)
{
  if (m_Image == NULL)
  {
    if ((m_LocalFilename == NULL) ||
      (mFlags & IS_FILE_NOT_READY))
      return FALSE;

    LPTSTR fullName = AllocPathName(folderPath, m_LocalFilename);
    BOOL file_exist = IsFileExistAttribute(fullName);
    delete[] fullName;
    if (!file_exist)
      return FALSE;
  }

  if (m_ThumbImage != NULL)
  {
    if ((m_ThumbImage->width >= (cx / 2)) ||
      (m_ThumbImage->height >= (cy / 2)))
      return FALSE;
  }
  return TRUE;
}

BOOL KImagePage::RenderThumbImage(CDC* pDC, LPTSTR folderPath, int cx, int cy)
{
  if (m_Image == NULL)
  {
    if ((m_LocalFilename == NULL) ||
      (mFlags & IS_FILE_NOT_READY))
      return FALSE;

    LPTSTR fullName = AllocPathName(folderPath, m_LocalFilename);

    if (!IsFileExistAttribute(fullName))
    {
      delete[] fullName;
      return FALSE;
    }
    if (m_Image == NULL)
    {
      m_Image = new KBitmap();
      if (m_Image->LoadImageFromFile(fullName) == NULL)
      {
        // if fail to load
        delete m_Image;
        m_Image = NULL;
        mFlags |= IS_FILE_NOT_READY;
      }
    }

    delete[] fullName;
  }

  if ((m_Image != NULL) && (m_Image->handle() != NULL))
  {
    float dx = (float)m_Image->width / cx;
    float dy = (float)m_Image->height / cy;
    if (dx < dy)
      dx = dy;
    int ix = (int)(m_Image->width / dx);
    int iy = (int)(m_Image->height / dx);

    if (m_ThumbImage == NULL)
      m_ThumbImage = new KBitmap();
    m_ThumbImage->CreateBitmap(ix, iy, 32, FALSE, NULL, 0, NULL);

    m_ThumbImage->drawImage(0, 0, ix, iy, m_Image, 0, 0, m_Image->width, m_Image->height);

    // draw Annotates
    CDC hdc;
    hdc.CreateCompatibleDC(pDC);
    HBITMAP oldbmp = (HBITMAP)hdc.SelectObject(m_ThumbImage->handle());
    Graphics g(hdc.GetSafeHdc());

    KImageDrawer drawer;
    drawer.Set(m_Image->width, m_Image->height, (float)1.0 / dx, ix, iy);
    DrawEditing(g, drawer, PropertyMode::PModeImageView, DRAW_NO_HANDLE);
    hdc.SelectObject(oldbmp);
    hdc.DeleteDC();

#ifdef _DEBUG_THUMB_UPDATE
    TCHAR msg[256] = _T("Render thumb image : ");
    if (m_LocalFilename != NULL)
      StringCchCat(msg, 256, m_LocalFilename);
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG | SYNC_MSG_DEBUG);
#endif
    return TRUE;
  }
  return FALSE;
}

void KImagePage::ClearThumbImage(BOOL bRecurr)
{
  if (m_ThumbImage != NULL)
  {
    delete m_ThumbImage;
    m_ThumbImage = NULL;
  }
}

void KImagePage::ClearPreviewImage()
{
  if (m_CacheImage != NULL)
  {
#ifdef _DEBUG
    TCHAR msg[256] = _T("Clear temp-cache image : ");
    if (m_LocalFilename != NULL)
      StringCchCat(msg, 256, m_LocalFilename);
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG | SYNC_MSG_DEBUG);
#endif

    delete m_CacheImage;
    m_CacheImage = NULL;
  }
  if (mpPreviewEffect != NULL)
  {
    delete mpPreviewEffect;
    mpPreviewEffect = NULL;
  }
  mHasEffect = FALSE;
}

SIZE KImagePage::GetImageSizeRatio()
{
  SIZE sz = { 0, 0 };
  if (m_Image != NULL)
  {
    sz.cx = m_Image->width;
    sz.cy = m_Image->height;
  }
  else if (m_ThumbImage != NULL)
  {
    sz.cx = m_ThumbImage->width;
    sz.cy = m_ThumbImage->height;
  }
  return sz;
}

BOOL KImagePage::hasDifferentFolder(LPCTSTR workingPath)
{
  LPTSTR filePath = NULL;
  SplitPathNameAlloc(m_LocalFilename, &filePath, NULL);
  if (filePath == NULL)
    return FALSE;
  delete[] filePath;

  if (IsChildFolderOrFile(workingPath, m_LocalFilename, FALSE))
    return FALSE;

  return TRUE;
}

void KImagePage::ClearCacheImage(LPCTSTR workingPath)
{
  if (IsModified())
  {
    StoreImageFile(workingPath, NULL, NULL, TRUE);
  }
  else if (hasDifferentFolder(workingPath))
  {
    // if original file exist and not modified, copy it to current working folder.
    if (IsFullPath(m_LocalFilename) && IsFileExist(m_LocalFilename))
    {
      LPTSTR org_pathName = AllocString(m_LocalFilename);
      readyImagePathName(workingPath, TRUE);
      LPTSTR new_pathName = AllocPathName(workingPath, m_LocalFilename);

      // check path exist
      if (!IsDirectoryExist(workingPath))
        CreateDirectory(workingPath);

      BOOL r = CopyFile(org_pathName, new_pathName, FALSE);

#ifdef _DEBUG
      TCHAR msg[256];
      StringCchPrintf(msg, 256, _T("Copy %s => %s"), org_pathName, new_pathName);
      if (!r)
      {
        TCHAR errmsg[24];
        StringCchPrintf(errmsg, 24, _T(", Fail(0x%X"), GetLastError());
        StringCchCat(msg, 256, errmsg);
      }
      StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG | SYNC_MSG_DEBUG);
#endif

      delete[] org_pathName;
      delete[] new_pathName;
    }
    else
    {
      StoreImageFile(workingPath, NULL, NULL, TRUE);
    }
  }

  if (m_Image != NULL)
  {
#ifdef _DEBUG
    TCHAR msg[256];
    StringCchPrintf(msg, 256, _T("Cache image Cleared : %s(index=%d)"), m_LocalFilename, m_MultiPageIndex);
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG | SYNC_MSG_DEBUG);
#endif

    delete m_Image;
    m_Image = NULL;
  }
  ClearPreviewImage();
}

BOOL KImagePage::GetEcmInfo(LPTSTR docOID, LPTSTR fileOID, LPTSTR storageOID)
{
  if (mpEcmInfo != NULL)
  {
    LPCTSTR s = StrChr(mpEcmInfo, '|');
    if (s != NULL)
    {
      size_t len = s - mpEcmInfo;
      StringCchCopyN(docOID, MAX_OID, mpEcmInfo, len);
      s++;

      LPCTSTR e = StrChr(s, '|');
      if (e != NULL)
      {
        size_t len = e - s;
        StringCchCopyN(fileOID, MAX_OID, s, len);
        e++;
        StringCchCopy(storageOID, MAX_STGOID, e);
        return TRUE;
      }
    }
  }
  return FALSE;
}


void KImagePage::ClearEditImage()
{
  if (HasTemporaryEffect())
  {
    ClearPreviewImage();
  }
}

BOOL KImagePage::SetPathName(LPTSTR pathName)
{
  if (m_LocalFilename != NULL)
    delete[] m_LocalFilename;
  if ((pathName != NULL) && (lstrlen(pathName) > 0))
    m_LocalFilename = AllocString(pathName);
  else
    m_LocalFilename = NULL;
  return TRUE;
}

void KImagePage::SetImageDPI(UINT xDpi, UINT yDpi)
{
  if (m_Image != NULL)
    m_Image->SetImageDPI(xDpi, yDpi);
}

#if 0
BOOL KImagePage::SaveImageFile(LPCTSTR pathName)
{
  if ((m_Image != NULL) && (m_LocalFilename != NULL))
  {
    if (IsDirectoryExist(pathName) || CreateDirectory(pathName))
    {

      if (mGlobalOption & GO_MERGEANNOTATE)
        MergeAnnotateObjects();

      LPTSTR fullName = AllocPathName(pathName, m_LocalFilename);
      m_Image->StoreImage(fullName);
      delete[] fullName;
      return TRUE;
    }
    else
    {
      TCHAR msg[256] = _T("CreateDirectory fail : ");
      StringCchCat(msg, 256, pathName);
      StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG | SYNC_MSG_DEBUG);
      //delete[] path;
    }
  }
  return FALSE;
}
#endif


#if 0
BOOL KImagePage::StoreMetaTagToFrameEncoder(IWICBitmapFrameEncode *piFrameEncode, LPCTSTR folderList)
{
  IWICMetadataQueryWriter *piFrameQWriter = NULL;
  KVoidPtrArray metalist;
  BOOL rtn = FALSE;
  KEcmDocTypeInfo array;

  GetMetaDataSet(metalist, META_FOR_STORE);

  if (metalist.size() > 0)
  {
    HRESULT hr = piFrameEncode->GetMetadataQueryWriter(&piFrameQWriter);

    if (SUCCEEDED(hr))
    {
      KVoidPtrIterator it = metalist.begin();
      while (it != metalist.end())
      {
        MetaKeyNameValue* p = (MetaKeyNameValue*)*it++;

        PROPVARIANT    value;
        PropVariantInit(&value);

        if (CompareStringNC(p->key, gTiffTagImageWidth) == 0)
        {
          value.vt = VT_UI4;
          value.uintVal = m_Image->width;
        }
        else if (CompareStringNC(p->key, gTiffTagImageHeight) == 0)
        {
          value.vt = VT_UI4;
          value.uintVal = m_Image->height;
        }
        else if (CompareStringNC(p->key, gTiffTagImageBitCount) == 0)
        {
          value.vt = VT_UI2;
          value.uiVal = m_Image->bitCount;
        }
#ifdef _useMetaKeyUserComment
        else if (CompareStringNC(p->key, gMetaKeyUserComment) == 0) // UserComment
        {
          value.vt = VT_LPWSTR;
          value.pwszVal = p->value;
        }
#endif
        else if (p->value != NULL)
        {
          value.vt = VT_LPSTR;
          value.pszVal = MakeUtf8String(p->value);
        }
        else
        {
          TCHAR msg[256];
          StringCchPrintf(msg, 256, _T("Empty tag skipped=%s"), p->key);
          StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG | SYNC_MSG_DEBUG);
          continue;
        }
        hr = piFrameQWriter->SetMetadataByName(p->key, &value);

        if (FAILED(hr))
        {
          TCHAR msg[256];
          StringCchPrintf(msg, 256, _T("Error on SetMetadataByName, key=%s : "), p->key);
          TCHAR buff[256];

          FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,
            NULL, hr,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
            (LPTSTR)buff, 256, NULL);
          StringCchCat(msg, 256, buff);

          StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG | SYNC_MSG_DEBUG);
        }

        if (value.vt == VT_LPSTR)
        {
          delete[] (LPTSTR)value.pszVal;
        }
      }
#ifdef _UNUSED
      if (folderList != NULL)
      {
#ifdef _DEBUG
        TCHAR msg[256] = _T("Folders : ");
        StringCchCat(msg, 256, folderList);
        StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG | SYNC_MSG_DEBUG);
#endif

        PROPVARIANT    value;
        PropVariantInit(&value);
        value.vt = VT_LPSTR;
        value.pszVal = MakeUtf8String(folderList);

        hr = piFrameQWriter->SetMetadataByName(TagFolderListInfo, &value);
        delete[](LPTSTR)value.pszVal;
      }
#endif
      // additional metadata(store parent folder's name-key if it is on sub-folder)
      KImageBase* p = GetParent();
      if ((p != NULL) && (p->GetParent() != NULL))
      {
        LPTSTR folderPath = ((KImageFolder*)p)->GetFolderNameID();

        PROPVARIANT    value;
        PropVariantInit(&value);
        value.vt = VT_LPSTR;
        value.pszVal = MakeUtf8String(folderPath);

        hr = piFrameQWriter->SetMetadataByName(IFDTagFolderPathInfo, &value);

        delete[] folderPath;
        delete[] (LPTSTR)value.pszVal;
      }
      rtn = TRUE;
    }

    piFrameQWriter->Release();
    ClearKeyNameValueArray(metalist);
  }
  return rtn;
}

BOOL KImagePage::StoreAnnotatesToFrameEncoder(IWICBitmapFrameEncode *piFrameEncode, KEcmDocTypeInfo* pInfo)
{
  int count = m_Annotates.GetCount();

  if (TRUE)//count > 0)
  {
    IWICMetadataBlockWriter* blockWriter = NULL;

    HRESULT hr = piFrameEncode->QueryInterface(IID_IWICMetadataBlockWriter, (void**)&blockWriter);
    if (SUCCEEDED(hr) && (blockWriter != NULL))
    {
      UINT writerCount = 0;
      blockWriter->GetCount(&writerCount);
      if (writerCount > 0)
      {
        for (UINT i = 0; i < writerCount; i++)
        {
          IWICMetadataWriter* pWriter = NULL;
          GUID metaFormat;
          blockWriter->GetWriterByIndex(i, &pWriter);
          if (pWriter != NULL)
          {
            pWriter->GetMetadataFormat(&metaFormat);

            OLECHAR* guidString;
            hr = StringFromCLSID(metaFormat, &guidString);
            TCHAR msg[256];

            StringCchPrintf(msg, 256, _T("Writer:%s"), guidString);
            StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG | SYNC_MSG_DEBUG);
            CoTaskMemFree(guidString);

            PROPVARIANT vs;
            PROPVARIANT vi;
            PROPVARIANT value;

            PropVariantInit(&vs);
            PropVariantInit(&vi);
            PropVariantInit(&value);

            KMemoryStream mf;
            StoreExtraDataBlock(mf, pInfo);

            if (mf.Size() > 0)
            {
              vi.vt = VT_UI2;
              vi.uiVal = 32932; // wang annotate
              InitPropVariantFromBuffer(mf.Data(), mf.Size(), &value);
              HRESULT hr = pWriter->SetValue(&vs, &vi, &value);
              if (FAILED(hr))
                StoreLogHistory(_T(__FUNCTION__), _T("IWICMetadataWriter::SetValue fail"), SYNC_MSG_LOG | SYNC_MSG_DEBUG);
            }

            FreePropVariantArray(1, &vs);
            FreePropVariantArray(1, &vi);
            FreePropVariantArray(1, &value);

            pWriter->Release();
          }
        }
      }
      blockWriter->Release();
    }
  }
  return (count > 0);
}
#endif

LPTSTR AllocValueString(PROPVARIANT& value)
{
  LPTSTR rstr = NULL;

  switch (value.vt) {
  case VT_LPWSTR:
    rstr = AllocString(value.pwszVal);
    break;
  case VT_LPSTR:
    rstr = MakeUnicodeString(value.pszVal);
    break;
  default:
    break;
  }
  return rstr;
}
/*
BOOL SetBufferValue(PROPVARIANT& value, WORD vt, LPBYTE buff, int len)
{
  value.vt = vt;
  if ((vt & VT_VECTOR) == VT_VECTOR)
  {
    int type = (value.vt & VT_BSTR_BLOB);
    switch (type)
    {
    case VT_I1:
      value.cac.cElems = len;
      value.cac.pElems = new char[len];
      memcpy(value.cac.pElems, buff, value.cac.cElems);
      break;

    case VT_UI1:
      value.caub.cElems = len;
      value.caub.pElems = new BYTE[len];
      memcpy(value.caub.pElems, buff, value.caub.cElems);
      break;

    case VT_I8:
      value.cah.cElems = len / 8;
      value.cah.pElems = new LARGE_INTEGER[value.cah.cElems];
      memcpy(value.cah.pElems, buff, value.cah.cElems * sizeof(LARGE_INTEGER));
      break;

    case VT_UI8:
      value.cauh.cElems = len / 8;
      value.cauh.pElems = new ULARGE_INTEGER[value.cauh.cElems];
      memcpy(value.cauh.pElems, buff, sizeof(ULARGE_INTEGER) * value.cauh.cElems);
      break;
    }
  }
}
*/

LPBYTE AllocBufferValue(PROPVARIANT& value, int& len)
{
  LPBYTE buff = NULL;

  if ((value.vt & VT_VECTOR) == VT_VECTOR)
  {
    int type = (value.vt & VT_BSTR_BLOB);
    switch (type)
    {
    case VT_I1:
      len = value.cac.cElems;
      buff = new BYTE[len];
      memcpy(buff, value.cac.pElems, value.cac.cElems);
      len = 1;
      break;
    case VT_UI1:
      len = value.caub.cElems;
      buff = new BYTE[len];
      memcpy(buff, value.caub.pElems, value.caub.cElems);
      break;
    case VT_I8:
      len = value.cah.cElems;
      buff = new BYTE[8 * len];
      memcpy(buff, value.cah.pElems, 8 * len);
      len = 8 * len;
      break;
    case VT_UI8:
      len = value.cauh.cElems;
      buff = new BYTE[8*len];
      memcpy(buff, value.cauh.pElems, 8*len);
      len = 8 * len;
      break;
    }
  }

  switch (value.vt)
  {
  case VT_I1:
  case VT_UI1:
    buff = new BYTE[1];
    memcpy(buff, &value.cVal, 1);
    len = 1;
    break;
  case VT_I2:
  case VT_UI2:
    buff = new BYTE[2];
    memcpy(buff, &value.iVal, sizeof(WORD));
    len = sizeof(WORD);
    break;
  case VT_I4:
  case VT_UI4:
    buff = new BYTE[4];
    memcpy(buff, &value.lVal, sizeof(DWORD));
    len = sizeof(DWORD);
    break;
  case VT_I8:
  case VT_UI8:
    buff = new BYTE[8];
    memcpy(buff, &value.hVal, 8);
    len = 8;
    break;
  case VT_LPSTR:
    buff = (LPBYTE)AllocCString(value.pszVal);
    len = strlen((LPSTR)buff);
  default:
    break;
  }
  return buff;
}

UINT GetVariantUnsignedValue(PROPVARIANT& vi)
{
  UINT rv = 0;
  switch (vi.vt)
  {
  case VT_UI1:
    rv = vi.bVal;
    break;
  case VT_UI2:
    rv = vi.uiVal;
    break;
  case VT_UI4:
    rv = vi.ulVal;
    break;
  case VT_I1:
    rv = (UINT)vi.cVal;
    break;
  case VT_I2:
    rv = (UINT)vi.iVal;
    break;
  case VT_I4:
    rv = (UINT)vi.lVal;
    break;
  default:
  {
    TCHAR msg[128];
    StringCchPrintf(msg, 128, _T("Unprocessed VT type:%d"), vi.vt);
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG | SYNC_MSG_DEBUG);
  }
    break;
  }
  return rv;
}

int GetVariantValue(PROPVARIANT& vi)
{
  int rv = 0;
  switch (vi.vt)
  {
  case VT_I1:
    rv = (UINT)vi.cVal;
    break;
  case VT_I2:
    rv = (UINT)vi.iVal;
    break;
  case VT_I4:
    rv = (UINT)vi.lVal;
    break;
  default:
    StoreLogHistory(_T(__FUNCTION__), _T("Unprocessed VT type"), SYNC_MSG_LOG | SYNC_MSG_DEBUG);
    break;
  }
  return rv;
}

void dumpBuffer(LPBYTE buff, int len)
{
  TCHAR str[128];

  for (int y = 0; y < len; y++)
  {
    StringCchPrintf(str, 128, _T("0x%.2x,"), *buff++);
    OutputDebugString(str);
    if ((y & 0x00F) == 0x0F)
      OutputDebugString(_T("\n"));
  }
}


void makeMetaDataKeyPath(LPTSTR key, int len, ImageEncodingFormat encode, MetaDataCategory category, LPCTSTR keyName)
{
  key[0] = '\0';
  if (encode == ImageEncodingFormat::Jpeg)
  {
    if (category == MetaDataCategory::Gps)
      StringCchCopy(key, len, _T("/app1/ifd/gps"));
    else if (category == MetaDataCategory::Ifd)
      StringCchCopy(key, len, _T("/app1/ifd"));
    else if (category == MetaDataCategory::Exif)
      StringCchCopy(key, len, _T("/app1/ifd/Exif"));
    else
      StringCchCopy(key, len, _T("/app1"));
  }
  else if (encode == ImageEncodingFormat::Tiff)
  {
    if (category == MetaDataCategory::Gps)
      StringCchCopy(key, len, _T("/ifd/gps"));
    else if (category == MetaDataCategory::Ifd)
      StringCchCopy(key, len, _T("/ifd"));
    else if (category == MetaDataCategory::Exif)
      StringCchCopy(key, len, _T("/ifd/exif"));
  }
  StringCchCat(key, len, keyName);
}

void GetQueryWriterInfo(LPTSTR buff, int bufflen, IWICMetadataQueryWriter* pWriter)
{
  TCHAR path[128] = { 0 };
  UINT len = 0;
  pWriter->GetLocation(128, path, &len);

  GUID metaFormat;
  pWriter->GetContainerFormat(&metaFormat);

  OLECHAR* guidString;
  HRESULT hr = StringFromCLSID(metaFormat, &guidString);

  StringCchPrintf(buff, bufflen, _T("[%s, %s]"), path, guidString);

  CoTaskMemFree(guidString);
}

void GetMetadataReaderInfo(LPTSTR buff, int bufflen, IWICMetadataReader* pReader)
{
  TCHAR name[128] = { 0 };
  UINT len = 0;

  IWICMetadataHandlerInfo* pHandler = NULL;
  pReader->GetMetadataHandlerInfo(&pHandler);
  if (pHandler != NULL)
    pHandler->GetFriendlyName(128, name, &len);

  GUID metaFormat;
  pReader->GetMetadataFormat(&metaFormat);

  OLECHAR* guidString;
  HRESULT hr = StringFromCLSID(metaFormat, &guidString);

  StringCchPrintf(buff, bufflen, _T("[%s, %s]"), name, guidString);

  CoTaskMemFree(guidString);
}

void writeResultLog(LPCTSTR title, HRESULT hr)
{
  TCHAR msg[256];
  int type = SYNC_MSG_LOG;

  StringCchCopy(msg, 256, title);
  if (FAILED(hr))
  {
    StringCchCat(msg, 256, _T(" Fail, "));

    TCHAR buff[256];
    FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,
      NULL, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
      (LPTSTR)buff, 256, NULL);
    StringCchCat(msg, 256, buff);
    type |= SYNC_EVENT_ERROR;
  }
  else
  {
    StringCchCat(msg, 256, _T(" Ok"));
  }
  StoreLogHistory(_T(__FUNCTION__), msg, type);
}

int KImagePage::StoreExtraDataBlock(KMemoryStream& mf, KEcmDocTypeInfo* pInfo)
{
    int written = m_Annotates.StoreToBuffer(mf);

    // 확장문서 속성 저장
    if (pInfo != NULL)
      written += pInfo->StoreToBuffer(mf);

    written += mDocExtAttributes.StoreToBuffer(mf);
    return written;
}

int KImagePage::LoadExtraDataBlock(KMemoryStream& mf, KEcmDocTypeInfo*pInfo)
{
  int readed = 0;

  while (mf.Size() > readed)
  {
    DWORD dw = mf.GetDWord();
    readed += sizeof(DWORD);

#ifdef _DEBUG_STORAGE_BLOCK
    TCHAR msg[128];
    StringCchPrintf(msg, 128, _T("Pos : %d, BlockSize:%d\n"), mf.GetPosition(), blockSize);
    OutputDebugString(msg);
#endif

    if (dw == KMS_ANNOTATE_GROUP) // TIFF_ANNOTATE_BLOCK)
    {
      readed += m_Annotates.LoadFromBufferBody(mf);
    }
    else if (dw == KMS_DOCTYPE_GROUP) // TIFF_DOCTYPE_LIST)
    {
      if (pInfo != NULL)
      {
        readed += pInfo->LoadFromBuffer(mf);
      }
      else
      {
        DWORD blockSize = mf.GetDWord();
        mf.Seek(blockSize, FilePosition::current);
        readed += blockSize + sizeof(DWORD);
      }
    }
    else if (dw == KMS_METADATA_GROUP) //TIFF_DOCTYPE_INFO)
    {
      readed += mDocExtAttributes.LoadFromBuffer(mf);
    }
    else
    {
      DWORD blockSize = mf.GetDWord();
      TCHAR msg[128];
      StringCchPrintf(msg, 128, _T("Unknown tag : 0X%X, size=%d\n"), dw, blockSize);
      OutputDebugString(msg);

      mf.Seek(blockSize, FilePosition::current);
      readed += blockSize + sizeof(DWORD);
    }
  }
  return readed;
}

BOOL KImagePage::SetWICMetadataValue(UINT ifd_id, PROPVARIANT& value, KEcmDocTypeInfo* docTypeInfo)
{
  switch (ifd_id)
  {
    case 1: // GPSLatitudeRef
    case 3: // GPSLongitudeRef
    case 5: // GPSAltitudeRef
    {
      TCHAR key[32];
      int len = 0;
      StringCchPrintf(key, 32, _T("/{ushort=%d}"), ifd_id);
      LPBYTE buff = AllocBufferValue(value, len);
      SetMetaData(key, buff, len, (WORD)value.vt, MetaDataCategory::Gps);
      if (buff != NULL)
        delete[] buff;
      break;
    }

    case 2: // GPSLatitude
    case 4: // GPSLongitude
    case 6: // GPSAltitude
    {
      TCHAR key[32];
      int len = 0;
      StringCchPrintf(key, 32, _T("/{ushort=%d}"), ifd_id);
      LPBYTE buff = AllocBufferValue(value, len);
      SetMetaData(key, buff, len, (WORD)value.vt, MetaDataCategory::Gps);
      if (buff != NULL)
        delete[] buff;
      break;
    }

    case 0:
    case 7:
    case 8:
    case 9:
    case 10:
    case 11:
    case 12:
    case 13:
    case 14:
    case 15:
    case 16:
    case 17:
    {
      TCHAR key[32];
      int len = 0;
      StringCchPrintf(key, 32, _T("/{ushort=%d}"), ifd_id); // "/app1/ifd/gps"
      LPBYTE buff = AllocBufferValue(value, len);
      SetMetaData(key, buff, len, (WORD)value.vt, MetaDataCategory::Gps);
      if (buff != NULL)
        delete[] buff;
      break;
    }

    case 32932: // wang annotate
    {
      UINT count = PropVariantGetElementCount(value);
      LPBYTE buff = new BYTE[count];
      PropVariantToBuffer(value, buff, count);

  #ifdef _DEBUG_32932
      dumpBuffer(buff, count);
  #endif

      KMemoryStream mf(buff, count, ReadOnly);
      delete[] buff;

      LoadExtraDataBlock(mf, docTypeInfo);
      break;
    }

    /*
    case 32940: // folder name list
    {
      LPTSTR valueStr = AllocValueString(value);
      SetFolderNameList(valueStr);
      delete[] valueStr;

      break;
    }
    case 37512: // gTiffTagDocTypeInfo
    {
      break;
    }
    */

    case 269: // DocumentName : 폴더 경로명
    {
      LPTSTR valueStr = AllocValueString(value);
      SetParentKeyName(valueStr);
      delete[] valueStr;
      break;
    }

    case 315:// Artist
    case 272: // Model
    case 316: // HostComputer
    case 270: //ImageDescription
    case 285: // PageName 
    case 305: // Software
    case 306: // DateTime
    {
      TCHAR key[32];
      StringCchPrintf(key, 32, _T("/{ushort=%d}"), ifd_id);
      LPTSTR valueStr = AllocValueString(value);
      SetMetaData(key, valueStr, EcmExtDocTypeColumnType::None, MetaDataCategory::Ifd, NULL, FALSE);

      if (ifd_id == 285)
        SetName(valueStr);

      delete[] valueStr;
      break;
    }

    case 36867: // DateTimeOrigin
    case 36868: // DateTimeDigitized
    case 37510: // UserComment
    case 41728: // FileSource
    {
      TCHAR key[32];
      StringCchPrintf(key, 32, _T("/{ushort=%d}"), ifd_id);
      LPTSTR valueStr = AllocValueString(value);
      SetMetaData(key, valueStr, EcmExtDocTypeColumnType::None, MetaDataCategory::Exif, NULL, FALSE);
      delete[] valueStr;
      break;
    }

    case 256: // ImageWidth
    case 257: // ImageHeight
    case 258: // BitsperSample
    default:
    {
      TCHAR msg[256];
      LPTSTR valueStr = AllocValueString(value);

      StringCchPrintf(msg, 256, _T("/{ushort=%d} skipped"), ifd_id);
      if (valueStr != NULL)
      {
        StringCchCat(msg, 256, _T(":"));
        StringCchCat(msg, 256, valueStr);
        delete[] valueStr;
      }
      StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG | SYNC_MSG_DEBUG);
      break;
    }
  }
  return TRUE;
}

BOOL KImagePage::LoadMetaDataFromMetadataReader(void* p, KEcmDocTypeInfo* docTypeInfo)
{
  HRESULT hr;
  GUID metaFormat;
  IWICMetadataReader* pReader = (IWICMetadataReader * )p;

  pReader->GetMetadataFormat(&metaFormat);

  UINT itemCount = 0;
  pReader->GetCount(&itemCount);

#ifdef _DEBUG
  TCHAR readerName[256] = { 0 };
  GetMetadataReaderInfo(readerName, 128, pReader);

  TCHAR msg[256];
  StringCchPrintf(msg, 256, _T("Reader:%s has %d items"), readerName, itemCount);
  StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG | SYNC_MSG_DEBUG);
#endif

  for (UINT p = 0; p < itemCount; p++)
  {
    PROPVARIANT vs;
    PROPVARIANT vi;
    PROPVARIANT value;

    PropVariantInit(&vs);
    PropVariantInit(&vi);
    PropVariantInit(&value);
    hr = pReader->GetValueByIndex(p, &vs, &vi, &value);
    if (SUCCEEDED(hr))
    {
      if (VT_UNKNOWN == value.vt)
      {
        IWICMetadataReader* subReader = NULL;

        hr = value.punkVal->QueryInterface(IID_PPV_ARGS(&subReader));

        if (SUCCEEDED(hr))
        {
          LoadMetaDataFromMetadataReader(subReader, docTypeInfo);
          // subReader->Release();
        }

        continue;
      }

      UINT ifd_id = GetVariantUnsignedValue(vi);

      if (ifd_id > 0)
      {
        SetWICMetadataValue(ifd_id, value, docTypeInfo);
      }

      FreePropVariantArray(1, &vs);
      FreePropVariantArray(1, &vi);
      FreePropVariantArray(1, &value);
    }
  }
  return TRUE;
}

BOOL KImagePage::LoadMetaDataFromFrameDecoder(IWICBitmapFrameDecode *pIFrameDecoder, KEcmDocTypeInfo* docTypeInfo)
{
  IWICMetadataBlockReader* blockReader = NULL;

  HRESULT hr = pIFrameDecoder->QueryInterface(IID_IWICMetadataBlockReader, (void**)&blockReader);
  if (SUCCEEDED(hr) && (blockReader != NULL))
  {
    UINT readerCount = 0;
    blockReader->GetCount(&readerCount);

#ifdef _DEBUG
    TCHAR readerName[128] = { 0 };
    GUID metaFormat;
    blockReader->GetContainerFormat(&metaFormat);

    OLECHAR* guidString = NULL;
    hr = StringFromCLSID(metaFormat, &guidString);
    StringCchCat(readerName, 256, guidString);
    CoTaskMemFree(guidString);

    TCHAR msg[256];
    StringCchPrintf(msg, 256, _T("Root Reader:%s has %d items"), readerName, readerCount);
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG | SYNC_MSG_DEBUG);
#endif

    for (UINT i = 0; i < readerCount; i++)
    {
      IWICMetadataReader* pReader = NULL;
      blockReader->GetReaderByIndex(i, &pReader);
      if (pReader != NULL)
      {
        LoadMetaDataFromMetadataReader(pReader, docTypeInfo);
      }
    }
  }

#if 0
  IWICMetadataQueryReader* pMetaQueryReader = NULL;

  HRESULT hr = pIFrameDecoder->GetMetadataQueryReader(&pMetaQueryReader);
  if (SUCCEEDED(hr))
  {
    KVoidPtrArray metalist;
    BOOL rtn = FALSE;

    GetMetaDataSet(metalist, META_FOR_STORE);
    KVoidPtrIterator it = metalist.begin();
    while (it != metalist.end())
    {
      MetaKeyNameValue* p = (MetaKeyNameValue*)*it++;

      PROPVARIANT value;
      PropVariantInit(&value);
      hr = pMetaQueryReader->GetMetadataByName(p->key, &value);
      if (SUCCEEDED(hr))
      {
        if (p->value != NULL)
          delete[] p->value;
        p->value = AllocValueString(value);

        if (CompareStringNC(p->key, _T("/ifd/{ushort=285}")) == 0) // PageName
        {
          SetName(p->value);
        }
        else
        {
          SetMetaData(p->key, p->value);
        }
        FreePropVariantArray(1, &value);
      }
    };

    IEnumString* pEString = NULL;
    hr = pMetaQueryReader->GetEnumerator(&pEString);
    if (SUCCEEDED(hr))
    {
      while (SUCCEEDED(hr))
      {
        LPOLESTR pStr = NULL;
        hr = pEString->Next(1, &pStr, NULL);
        if (pStr != NULL)
        {
          StoreLogHistory(_T(__FUNCTION__), pStr, SYNC_MSG_LOG | SYNC_MSG_DEBUG);
          CoTaskMemFree(pStr);
        }
        else
        {
          hr = -1;
        }
      }

      pEString->Release();
    }

    ClearKeyNameValueArray(metalist);
  }
#endif
  return TRUE;
}

int KImagePage::MergeAnnotateObjects()
{
  int r = 0;

  if ((m_Image != NULL) && (m_Image->handle() != NULL) &&
    (m_Annotates.GetCount() > 0))
  {
    r = m_Annotates.GetCount();
    HBITMAP bm = m_Image->handle();
    HDC hdc = CreateCompatibleDC(NULL);
    HBITMAP oldbm = (HBITMAP)::SelectObject(hdc, bm);

    // draw Annotates
    Graphics g(hdc);
    KImageDrawer drawer;

    drawer.Set(m_Image->width, m_Image->height, 1.0, m_Image->width, m_Image->height);
    m_Annotates.Draw(g, drawer, DRAW_NO_HANDLE);

    ::SelectObject(hdc, oldbm);
    DeleteDC(hdc);

    // clear Annotates
    m_Annotates.Clear();
    SetModified(FALSE, PageModifiedFlag::IMAGE_MODIFIED);
  }
  return r;
}

BOOL KImagePage::LoadImageFromFrameDecoder(IWICImagingFactory* pImageFactory, IWICBitmapFrameDecode *pIBitmapFrame, KEcmDocTypeInfo* docTypeInfo)
{
  if (m_Image == NULL)
    m_Image = new KBitmap();
  else
    m_Image->Clear();

  if (m_Image->LoadImageFromFrameDecoder(pImageFactory, pIBitmapFrame) != NULL)
  {
    // load metadata
    LoadMetaDataFromFrameDecoder(pIBitmapFrame, docTypeInfo);
  }
  return TRUE;
}
void readyPropVariantFromBuffer(MetaKeyNameValue* p, PROPVARIANT& value)
{
  if (p->type == EcmExtDocTypeColumnType::Binary)
  {
    if (p->vtype & VT_VECTOR)
    {
      WORD subtype = (p->vtype & VT_TYPEMASK);
      switch (subtype)
      {
      case VT_UI1:
      case VT_I1:
        InitPropVariantFromBuffer(p->value, p->len, &value);
        break;

      case VT_UI2:
        InitPropVariantFromUInt16Vector((const WORD*)p->value, p->len / 2, &value);
        break;
      case VT_I2:
        InitPropVariantFromInt16Vector((const SHORT*)p->value, p->len / 2, &value);
        break;

      case VT_UI4:
        InitPropVariantFromUInt32Vector((const ULONG*)p->value, p->len / 4, &value);
        break;
      case VT_I4:
        InitPropVariantFromInt32Vector((const LONG*)p->value, p->len / 4, &value);
        break;

      case VT_UI8:
        InitPropVariantFromUInt64Vector((const ULONGLONG*)p->value, p->len / 8, &value);
        break;
      case VT_I8:
        InitPropVariantFromInt64Vector((const LONGLONG*)p->value, p->len / 8, &value);
        break;

      default:
      {
        TCHAR msg[128];
        StringCchPrintf(msg, 128, _T("Cannot handle vtype(%d)"), p->vtype);
        StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
      }
      break;
      }
    }
    else
    {
      switch (p->vtype)
      {
      case VT_UI1:
      case VT_I1:
        InitPropVariantFromBuffer(p->value, p->len, &value);
        break;

      case VT_UI2:
        InitPropVariantFromUInt16(*((const WORD*)p->value), &value);
        break;
      case VT_I2:
        InitPropVariantFromInt16(*((const SHORT*)p->value), &value);
        break;

      case VT_UI4:
        InitPropVariantFromUInt32(*((const ULONG*)p->value), &value);
        break;
      case VT_I4:
        InitPropVariantFromInt32(*((const LONG*)p->value), &value);
        break;

      case VT_UI8:
        InitPropVariantFromUInt64(*((const ULONGLONG*)p->value), &value);
        break;
      case VT_I8:
        InitPropVariantFromInt64(*((const LONGLONG*)p->value), &value);
        break;

      case VT_LPSTR:
        //InitPropVariantFromBuffer((const BYTE*)p->value, p->len, &value);
        value.vt = VT_LPSTR;
        value.pszVal = AllocCString((const char*)p->value, p->len);
        break;

      default:
      {
        TCHAR msg[128];
        StringCchPrintf(msg, 128, _T("Cannot handle vtype(%d)"), p->vtype);
        StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
      }
      break;
      }
    }
  }

}

void AddChildQueryWriter(IWICMetadataQueryWriter* pWriter, LPTSTR name, IWICMetadataQueryWriter* childWriter)
{
  PROPVARIANT value;
  PropVariantInit(&value);

  value.vt = VT_UNKNOWN;
  value.punkVal = childWriter;
  value.punkVal->AddRef();

  TCHAR info[128];
  GetQueryWriterInfo(info, 128, pWriter);

  TCHAR cinfo[128];
  GetQueryWriterInfo(cinfo, 128, childWriter);

  HRESULT hr = pWriter->SetMetadataByName(name, &value);

  TCHAR title[128];
  StringCchPrintf(title, 128, _T("SetMetadataByName(\"%s\", %s) on %s"), name, cinfo, info);

  writeResultLog(title, hr);

  PropVariantClear(&value);
}

int KImagePage::StoreImageFile(LPCTSTR pathName, KEcmDocTypeInfo* info, LPCTSTR firstFilename, BOOL asTemporaryIfExist)
{
  BOOL isFinal = FALSE;
  readyImagePathName(pathName, FALSE);

  if (IsDirectoryExist(pathName) || CreateDirectory(pathName))
  {
    if (!IsModified() && (firstFilename != NULL))
    {
      // 수정되지 않고 이름만 변경된 것
      if (StrCmp(m_LocalFilename, firstFilename) != 0)
      {
        LPTSTR srcfullName = AllocPathName(pathName, m_LocalFilename);
        LPTSTR dstfullName = AllocPathName(pathName, firstFilename);

        MoveFile(srcfullName, dstfullName);
        delete[] m_LocalFilename;
        m_LocalFilename = AllocString(firstFilename);
      }
      return 1;
    }

    if (firstFilename != NULL)
    {
      // 새 이름으로
      delete[] m_LocalFilename;
      m_LocalFilename = AllocString(firstFilename);
    }

    if (m_Image != NULL)
    {
      LPCTSTR saveFilename = m_LocalFilename;

      // 임시 저장이고 (MultiPageImage 파트인) 경우에 임시 이름으로 저장.
      if (asTemporaryIfExist) // && (m_MultiPageIndex > 0))
      {
        if (m_TempFilename == NULL)
          m_TempFilename = readyTempFilename(pathName);
        saveFilename = m_TempFilename;

        TCHAR msg[256] = _T("Save as temp file : ");
        StringCchCat(msg, 256, m_TempFilename);
        StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
      }

      if (mGlobalOption & GO_MERGEANNOTATE) // isFinal & (
        MergeAnnotateObjects();

      LPTSTR fullName = AllocPathName(pathName, saveFilename);

      KVoidPtrArray metalist;
      int metaFlag = META_FOR_STORE;
      if (GetEncodingFilename(fullName) == ImageEncodingFormat::Tiff) // && (encode == ImageEncodingFormat::Tiff))
      {
        metaFlag |= META_INCLUDE_ANNOTATES;
      }

      GetMetaDataSet(metalist, info, metaFlag);

      StoreImageFileEx(fullName, &metalist);

      ClearKeyNameValueArray(metalist);

      delete[] fullName;
    }
  }
  return 1;
}

void KImagePage::GetMetaDataSet(KVoidPtrArray& list, KEcmDocTypeInfo* info, int flag)
{
  KImageBase::GetMetaDataSet(list, info, flag);

  if (flag & META_INCLUDE_ANNOTATES)
  {
    KMemoryStream mf;
    StoreExtraDataBlock(mf, info);

    LPTSTR key = AllocString(_T("/{ushort=32932}"));
    appendMetaData(list, key, NULL, mf.Data(), mf.Size(), MetaDataCategory::Exif, VT_UI1 | VT_VECTOR, 0);
  }
}

LPBYTE KImagePage::GetMetaDataBinary(LPCTSTR key, int& len, MetaDataCategory& category, WORD& vtype)
{
  LPBYTE buff = NULL;

  if (CompareStringNC(key, gTiffTagImageWidth) == 0)
  {
    category = MetaDataCategory::Ifd;
    vtype = VT_UI4;
    len = sizeof(int);
    buff = new BYTE[len];
    memcpy(buff, &m_Image->width, sizeof(int));
  }
  else if (CompareStringNC(key, gTiffTagImageHeight) == 0)
  {
    category = MetaDataCategory::Ifd;
    vtype = VT_UI4;
    len = sizeof(int);
    buff = new BYTE[len];
    memcpy(buff, &m_Image->height, sizeof(int));
  }
  else if (CompareStringNC(key, gTiffTagImageBitCount) == 0)
  {
    category = MetaDataCategory::Ifd;
    vtype = VT_UI2;
    len = sizeof(WORD);
    buff = new BYTE[len];
    memcpy(buff, &m_Image->bitCount, sizeof(WORD));
  }
  return buff;
}

BOOL KImagePage::StoreImageFileEx(LPCTSTR filename, KVoidPtrArray* pMetaData)
{
  BOOL r = FALSE;
  LPTSTR filename_tmp = (LPTSTR)filename;

  if (IsFileExist(filename) || (mGlobalOption & GO_USECRYPTION))
  {
    filename_tmp = AllocAddString(filename, '_', _T("tmp"));
  }

  if (StoreImageFileExInternal(filename_tmp, pMetaData))
  {
    r = TRUE;
    if (mGlobalOption & GO_USECRYPTION)
    {
      KEncryptFile(filename_tmp, filename);
#ifndef _DEBUG
      KDeleteFile(filename_tmp);
#endif
    }
    else if (filename != filename_tmp)
    {
      DeleteFile(filename);
      r = KMoveFile(filename_tmp, filename);
    }
  }
  if (filename != filename_tmp)
    delete[] filename_tmp;
  return r;
}

BOOL KImagePage::StoreImageFileExInternal(LPCTSTR filename, KVoidPtrArray* pMetaData)
{
  IWICImagingFactory* pImageFactory = 0;

  HRESULT hr = K_CoInitializeEx(0, COINIT_MULTITHREADED);
  if (SUCCEEDED(CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER,
    IID_IWICImagingFactory, (void**)&pImageFactory)))
  {
    IWICStream* piFileStream = NULL;
    IWICBitmapEncoder* piEncoder = NULL;

    // Create a file stream.
    hr = pImageFactory->CreateStream(&piFileStream);

    // Initialize our new file stream.
    if (SUCCEEDED(hr))
    {
      // hr = piFileStream->InitializeFromFilename(filename, GENERIC_WRITE);
      hr = SHCreateStreamOnFileEx(filename, STGM_WRITE | STGM_CREATE | STGM_SHARE_DENY_NONE,
        FILE_ATTRIBUTE_NORMAL, TRUE, NULL, (IStream**)&piFileStream);
    }

    ImageEncodingFormat encode = ImageEncodingFormat::None;

    // Create the encoder.
    if (SUCCEEDED(hr))
    {
      encode = KImageBase::GetEncodingFilename(filename);

      if (encode == ImageEncodingFormat::Png)
      {
        hr = pImageFactory->CreateEncoder(GUID_ContainerFormatPng, NULL, &piEncoder);
      }
      else if (encode == ImageEncodingFormat::Jpeg)
      {
        hr = pImageFactory->CreateEncoder(GUID_ContainerFormatJpeg, NULL, &piEncoder); // CLSID_WICJpegEncoder
      }
      else if (encode == ImageEncodingFormat::Tiff)
      {
        hr = pImageFactory->CreateEncoder(GUID_ContainerFormatTiff, NULL, &piEncoder);
      }
      else // if (CompareStringICNC(ext, _T("bmp")) == 0)
      {
        hr = pImageFactory->CreateEncoder(GUID_ContainerFormatBmp, NULL, &piEncoder);
      }
    }

    // Initialize the encoder
    if (SUCCEEDED(hr))
    {
      hr = piEncoder->Initialize(piFileStream, WICBitmapEncoderNoCache);

      StoreImageWithFrameEncoder(pImageFactory, piEncoder, encode, pMetaData);

      hr = piEncoder->Commit();
      writeResultLog(_T("Encoder::Commit()"), hr);

      hr = piFileStream->Commit(STGC_DEFAULT);
      writeResultLog(_T("FileStream::Commit()"), hr);
    }

    if (piEncoder != NULL)
      piEncoder->Release();
    if (piFileStream != NULL)
      piFileStream->Release();
    pImageFactory->Release();
  }
  K_CoUninitialize();

#ifdef _DEBUG
  TCHAR msg[300] = _T("Save image : ");
  StringCchCat(msg, 300, filename);
  StringCchCat(msg, 300, (SUCCEEDED(hr) ? _T(" Ok") : _T(" Fail")));
  if (IsFileEditing(filename))
    StringCchCat(msg, 300, _T(", file locked"));
  StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG | SYNC_MSG_DEBUG);
#endif
  return TRUE;
}


static void dumpMetadataReader(IWICMetadataReader* pReader, int d)
{
  TCHAR info[256];

  for (int i = 0; i < d; i++)
    info[i] = ' ';
  info[d] = '\0';
  GetMetadataReaderInfo(&(info[d]), 256-d, pReader);

  UINT count;
  pReader->GetCount(&count);

  TCHAR buff[32];
  StringCchPrintf(buff, 32, _T(", Child=%d"), count);
  StringCchCat(info, 256, buff);
  StoreLogHistory(_T(__FUNCTION__), info, SYNC_MSG_LOG | SYNC_MSG_DEBUG);

  for (UINT i = 0; i < count; i++)
  {
    PROPVARIANT vs;
    PROPVARIANT vi;
    PROPVARIANT value;

    PropVariantInit(&vs);
    PropVariantInit(&vi);
    PropVariantInit(&value);

    pReader->GetValueByIndex(i, &vs, &vi, &value);

    if (value.vt == VT_UNKNOWN)
    {
      IWICMetadataReader* subReader = NULL;
      HRESULT hr = value.punkVal->QueryInterface(IID_PPV_ARGS(&subReader));

      if (SUCCEEDED(hr) && (subReader != NULL))
      {
        dumpMetadataReader(subReader, d + 1);
        subReader->Release();
      }
    }

    FreePropVariantArray(1, &vs);
    FreePropVariantArray(1, &vi);
    FreePropVariantArray(1, &value);
  }
}

static void dumpFrameEncoders(IWICBitmapFrameEncode* piFrameEncode)
{
  IWICMetadataBlockWriter* blockWriter = NULL;

  StoreLogHistory(_T(__FUNCTION__), _T("WICMetadataBlockWriters"), SYNC_MSG_LOG | SYNC_MSG_DEBUG);

  HRESULT hr = piFrameEncode->QueryInterface(IID_IWICMetadataBlockWriter, (void**)&blockWriter);
  if (SUCCEEDED(hr) && (blockWriter != NULL))
  {
    UINT writerCount = 0;
    blockWriter->GetCount(&writerCount);
    if (writerCount > 0)
    {
      for (UINT i = 0; i < writerCount; i++)
      {
        IWICMetadataWriter* pWriter = NULL;
        blockWriter->GetWriterByIndex(i, &pWriter);
        if (pWriter != NULL)
        {
          dumpMetadataReader(pWriter, 1);
        }
      }
    }
    blockWriter->Release();
  }
  else
  {
    writeResultLog(_T("QueryInterface(IID_IWICMetadataBlockWriter)"), hr);
  }
}

#define USE_BLOCK_WRITER // USE_SINGLE_QUERY_WRITER

BOOL KImagePage::StoreImageWithFrameEncoder(IWICImagingFactory* pImageFactory, IWICBitmapEncoder* piEncoder, 
  ImageEncodingFormat encode, KVoidPtrArray* pMetaData)
{
  IWICBitmapFrameEncode* piFrameEncode = NULL;

  HRESULT hr = piEncoder->CreateNewFrame(&piFrameEncode, NULL);

  /*
  if ((pExtraData != NULL) && (encode == ImageEncodingFormat::Tiff))
  {
    appendMetaData(*pMetaData, _T("/{ushort=32932"), NULL, pExtraData, len, MetaDataCategory::Ifd, VT_UI2 | VT_VECTOR, 0);
    // StoreExtraDataToWriter(piFrameEncode, pExtraData, len);
  }
  */

  if (SUCCEEDED(hr))
  {
    m_Image->StoreImageWithFrameEncoder(pImageFactory, piFrameEncode);

    if (pMetaData != NULL)
    {
      IWICMetadataQueryWriter* piFrameQWriter = NULL;

#ifdef USE_SINGLE_QUERY_WRITER
      HRESULT h = piFrameEncode->GetMetadataQueryWriter(&piFrameQWriter);
      if (SUCCEEDED(h))
      {
        StoreMetadataToQWriter(pImageFactory, piFrameQWriter, encode, pMetaData);
      }
      else
      {
        writeResultLog(_T("GetMetadataQueryWriter()"), h);
      }
#elif defined(USE_BLOCK_WRITER)
      IWICMetadataBlockWriter* pBlockWriter = NULL;
      HRESULT h = piFrameEncode->QueryInterface(IID_IWICMetadataBlockWriter, (void**)&pBlockWriter);

      IWICComponentFactory* pComponentFactory = NULL;
      h = pImageFactory->QueryInterface(IID_IWICComponentFactory, (void**)&pComponentFactory);

      if (SUCCEEDED(h))
      {
        hr = pComponentFactory->CreateQueryWriterFromBlockWriter(
          pBlockWriter, &piFrameQWriter);

        if (SUCCEEDED(hr))
        {
          TCHAR info[128] = { 0 };
          GetQueryWriterInfo(info, 128, piFrameQWriter);
          TCHAR msg[128] = _T("CreateQueryWriterFromBlockWriter() : ");
          if (lstrlen(info) > 0)
            StringCchCat(msg, 128, info);
          StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);

          pComponentFactory->Release();
        }
        else
        {
          writeResultLog(_T("CreateQueryWriterFromBlockWriter()"), h);
        }

        StoreMetadataToWriterCascaded(pImageFactory, piFrameQWriter, encode, pMetaData);
        // pBlockWriter->AddWriter(piFrameQWriter);
      }
      else
      {
        // writeResultLog(_T("GetMetadataQueryWriter()"), h);
        writeResultLog(_T("QueryInterface(IID_IWICComponentFactory)"), h);
      }

      if (pBlockWriter != NULL)
        pBlockWriter->Release();
#else
      HRESULT h = piFrameEncode->GetMetadataQueryWriter(&piFrameQWriter);
      if (SUCCEEDED(h))
      {
        StoreMetadataToWriterCascaded(pImageFactory, piFrameQWriter, encode, pMetaData);
      }
      else
      {
        writeResultLog(_T("GetMetadataQueryWriter()"), h);
      }

#endif
      if (piFrameQWriter != NULL)
        piFrameQWriter->Release();
    }

    dumpFrameEncoders(piFrameEncode);

    // Commit the frame.
    if (SUCCEEDED(hr))
      hr = piFrameEncode->Commit();
  }

  if (piFrameEncode)
    piFrameEncode->Release();
  return SUCCEEDED(hr);
}

BOOL KImagePage::StoreMetadataToWriterCascaded(IWICImagingFactory* pFactory, IWICMetadataQueryWriter* piQueryWriter,
  ImageEncodingFormat encode, KVoidPtrArray* pMetaData)
{
  BOOL rtn = FALSE;
  GUID vendor = GUID_VendorMicrosoft;
  IWICMetadataQueryWriter* pAppWriter = NULL;

  if (encode == ImageEncodingFormat::Jpeg)
  {
    HRESULT hr = pFactory->CreateQueryWriter(GUID_MetadataFormatApp1, &vendor, &pAppWriter);
    if (SUCCEEDED(hr))
    {
      IWICMetadataQueryWriter* pIfdWriter = NULL;
      HRESULT hr = pFactory->CreateQueryWriter(GUID_MetadataFormatIfd, &vendor, &pIfdWriter);
      if (SUCCEEDED(hr))
      {
        if (StoreMetadataToWriterEx(pIfdWriter, encode, pMetaData, MetaDataCategory::Ifd))
        {
          IWICMetadataQueryWriter* pGpsWriter = NULL;
          hr = pFactory->CreateQueryWriter(GUID_MetadataFormatGps, &vendor, &pGpsWriter);
          if (SUCCEEDED(hr))
          {
            if (StoreMetadataToWriterEx(pGpsWriter, encode, pMetaData, MetaDataCategory::Gps))
            {
              AddChildQueryWriter(pIfdWriter, L"/gps", pGpsWriter);
            }
            pGpsWriter->Release();
          }
          else
          {
            writeResultLog(_T("CreateQueryWriter(GUID_MetadataFormatGps)"), hr);
          }

          IWICMetadataQueryWriter* pExifWriter = NULL;
          hr = pFactory->CreateQueryWriter(GUID_MetadataFormatExif, &vendor, &pExifWriter);
          if (SUCCEEDED(hr))
          {
            if (StoreMetadataToWriterEx(pExifWriter, encode, pMetaData, MetaDataCategory::Exif))
            {
              AddChildQueryWriter(pIfdWriter, L"/exif", pExifWriter);
            }
            pExifWriter->Release();
          }
          else
          {
            writeResultLog(_T("CreateQueryWriter(GUID_MetadataFormatGps)"), hr);
          }

          rtn = TRUE;
          AddChildQueryWriter(pAppWriter, L"/ifd", pIfdWriter);
          pIfdWriter->Release();
        }
      }
      else
      {
        writeResultLog(_T("CreateQueryWriter(GUID_MetadataFormatIfd)"), hr);
      }

      AddChildQueryWriter(piQueryWriter, L"/app1", pAppWriter);
      pAppWriter->Release();
    }
    else
    {
      writeResultLog(_T("CreateQueryWriter(GUID_MetadataFormatApp1)"), hr);
    }
  }
  else if (encode == ImageEncodingFormat::Tiff)
  {
    IWICMetadataQueryWriter* pIfdWriter = NULL;
    HRESULT hr = pFactory->CreateQueryWriter(GUID_MetadataFormatIfd, &vendor, &pIfdWriter);
    if (SUCCEEDED(hr))
    {
      if (StoreMetadataToWriterEx(pIfdWriter, encode, pMetaData, MetaDataCategory::Ifd))
      {
        IWICMetadataQueryWriter* pGpsWriter = NULL;
        hr = pFactory->CreateQueryWriter(GUID_MetadataFormatGps, &vendor, &pGpsWriter);
        if (SUCCEEDED(hr))
        {
          if (StoreMetadataToWriterEx(pGpsWriter, encode, pMetaData, MetaDataCategory::Gps))
          {
            AddChildQueryWriter(pIfdWriter, L"/gps", pGpsWriter);
          }
          pGpsWriter->Release();
        }
        else
        {
          writeResultLog(_T("CreateQueryWriter(GUID_MetadataFormatGps)"), hr);
        }

        IWICMetadataQueryWriter* pExifWriter = NULL;
        hr = pFactory->CreateQueryWriter(GUID_MetadataFormatExif, &vendor, &pExifWriter);
        if (SUCCEEDED(hr))
        {
          if (StoreMetadataToWriterEx(pExifWriter, encode, pMetaData, MetaDataCategory::Exif))
          {
            AddChildQueryWriter(pIfdWriter, L"/exif", pExifWriter);
          }
          pExifWriter->Release();
        }
        else
        {
          writeResultLog(_T("CreateQueryWriter(GUID_MetadataFormatGps)"), hr);
        }

        rtn = TRUE;
        AddChildQueryWriter(piQueryWriter, L"/ifd", pIfdWriter);
        pIfdWriter->Release();
      }
    }
    else
    {
      writeResultLog(_T("CreateQueryWriter(GUID_MetadataFormatIfd)"), hr);
    }
  }
  return rtn;
}

BOOL KImagePage::StoreMetadataToQWriter(IWICImagingFactory* pFactory, IWICMetadataQueryWriter* piQWriter,
  ImageEncodingFormat encode, KVoidPtrArray* pMetaData)
{
  int count = 0;
  KVoidPtrIterator it = pMetaData->begin();
  while (it != pMetaData->end())
  {
    MetaKeyNameValue* p = (MetaKeyNameValue*)*it++;

    /*
    if (p->category != MetaDataCategory::Ifd) // ifd only
      continue;
    */

    PROPVARIANT value;
    PropVariantInit(&value);

    if (p->value != NULL)
    {
      if (p->type == EcmExtDocTypeColumnType::Binary)
      {
        value.vt = p->vtype;
        readyPropVariantFromBuffer(p, value);
      }
      else
      {
        value.vt = VT_LPSTR;
        value.pszVal = MakeUtf8String(p->value);
      }
    }
    else
    {
      TCHAR msg[256];
      StringCchPrintf(msg, 256, _T("Empty tag skipped=%s"), p->key);
      StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG | SYNC_MSG_DEBUG);
      continue;
    }

    TCHAR key[128];
    makeMetaDataKeyPath(key, 128, encode, p->category, p->key);
    HRESULT h = piQWriter->SetMetadataByName(key, &value);

    TCHAR title[128];
    StringCchPrintf(title, 128, _T("SetMetadataByName, key=%s : "), key);
    writeResultLog(title, h);

    if (SUCCEEDED(h))
      count++;

    if (value.vt == VT_LPSTR)
    {
      delete[] value.pszVal;
    }
    else if (p->type == EcmExtDocTypeColumnType::Binary)
    {
      FreePropVariantArray(1, &value);
    }
  }
  return (count > 0);
}

BOOL KImagePage::StoreMetadataToWriterEx(IWICMetadataQueryWriter* piQWriter, ImageEncodingFormat encode,
  KVoidPtrArray* pMetaData, MetaDataCategory category)
{
  int count = 0;
  KVoidPtrIterator it = pMetaData->begin();
  while (it != pMetaData->end())
  {
    MetaKeyNameValue* p = (MetaKeyNameValue*)*it++;

    if (category != p->category)
      continue;

    PROPVARIANT value;
    PropVariantInit(&value);

#ifdef _useMetaKeyUserComment
    else if (CompareStringNC(p->key, gMetaKeyUserComment) == 0) // UserComment
    {
      value.vt = VT_LPWSTR;
      value.pwszVal = p->value;
    }
#endif
    if (p->value != NULL)
    {
      if (p->type == EcmExtDocTypeColumnType::Binary)
      {
        value.vt = p->vtype;
        readyPropVariantFromBuffer(p, value);
      }
      else
      {
        value.vt = VT_LPSTR;
        value.pszVal = MakeUtf8String(p->value);
      }
    }
    else
    {
      TCHAR msg[256];
      StringCchPrintf(msg, 256, _T("Empty tag skipped=%s"), p->key);
      StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG | SYNC_MSG_DEBUG);
      continue;
    }

    //TCHAR key[128];
    //makeMetaDataKeyPath(key, 128, encode, p->category, p->key);
    HRESULT h = piQWriter->SetMetadataByName(p->key, &value);

    TCHAR title[128];
    StringCchPrintf(title, 128, _T("SetMetadataByName, key=%s : "), p->key);
    writeResultLog(title, h);

    if (SUCCEEDED(h))
      count++;

    if (value.vt == VT_LPSTR)
    {
      delete[](LPTSTR)value.pszVal;
    }
    else if (p->type == EcmExtDocTypeColumnType::Binary)
    {
      FreePropVariantArray(1, &value);
    }
  }
  return (count > 0);
}

BOOL KImagePage::StoreExtraDataToWriter(IWICBitmapFrameEncode* piFrameEncode, LPBYTE pExtraData, int len)
{
  IWICMetadataBlockWriter* blockWriter = NULL;

  HRESULT hr = piFrameEncode->QueryInterface(IID_IWICMetadataBlockWriter, (void**)&blockWriter);
  if (SUCCEEDED(hr) && (blockWriter != NULL))
  {
    UINT writerCount = 0;
    blockWriter->GetCount(&writerCount);
    if (writerCount > 0)
    {
      for (UINT i = 0; i < writerCount; i++)
      {
        IWICMetadataWriter* pWriter = NULL;
        blockWriter->GetWriterByIndex(i, &pWriter);
        if (pWriter != NULL)
        {
#ifdef _DEBUG
          TCHAR info[128] = { 0 };
          GetMetadataReaderInfo(info, 128, pWriter);
          StoreLogHistory(_T(__FUNCTION__), info, SYNC_MSG_LOG | SYNC_MSG_DEBUG);
#endif

          PROPVARIANT vs;
          PROPVARIANT vi;
          PROPVARIANT value;

          PropVariantInit(&vs);
          PropVariantInit(&vi);
          PropVariantInit(&value);

          vi.vt = VT_UI2;
          vi.uiVal = 32932; // wang annotate
          InitPropVariantFromBuffer(pExtraData, len, &value);

          HRESULT hr = pWriter->SetValue(&vs, &vi, &value);
          writeResultLog(_T("IWICMetadataWriter::SetValue()"), hr);

          FreePropVariantArray(1, &vs);
          FreePropVariantArray(1, &vi);
          FreePropVariantArray(1, &value);

          // pWriter->Release();
        }
      }
    }
    // blockWriter->Release();
    return TRUE;
  }
  else
  {
    writeResultLog(_T("QueryInterface(IID_IWICMetadataBlockWriter)"), hr);
    return FALSE;
  }
}

BOOL KImagePage::IsNeedRenderEffect(ImageEffectJob* effect)
{
  if (m_Image != NULL)
  {
    if ((effect != NULL) && m_Image->IsNeedRenderEffect(effect))
      return TRUE;
  }
  return FALSE;
}

BOOL KImagePage::IsNeedRenderImageEffect(ImageEffectJob* effect)
{
  if (m_Image != NULL)
  {
    if ((effect != NULL) && m_Image->IsNeedRenderEffect(effect))
    {
      // check if already has effect-cache
      if ((mpPreviewEffect != NULL) && (m_CacheImage != NULL) &&
        (CompareEffect(mpPreviewEffect, effect) == 0))
        return FALSE;

      return TRUE;
    }
  }
  return FALSE;
}

BOOL KImagePage::SetImageEffect(ImageEffectJob* effect)
{
  if (m_Image != NULL)
  {
    if (effect != NULL)
    {
      if (mpPreviewEffect == NULL)
        mpPreviewEffect = new ImageEffectJob;
      memcpy(mpPreviewEffect, effect, sizeof(ImageEffectJob));
      mHasEffect = TRUE;
      return TRUE;
    }
    else
    {
      if (mpPreviewEffect != NULL)
      {
        delete mpPreviewEffect;
        mpPreviewEffect = NULL;
      }
      mHasEffect = TRUE;
    }
  }
  return FALSE;
}

BOOL KImagePage::SetImageEffectCache(ImageEffectJob* effect, KBitmap* cacheImage)
{
  if ((m_Image != NULL) &&
    (mpPreviewEffect != NULL) &&
    (CompareEffect(mpPreviewEffect, effect) == 0))
  {
    if (m_CacheImage != NULL)
      delete m_CacheImage;
    m_CacheImage = cacheImage;
    return TRUE;
  }
  return FALSE;
}

BOOL KImagePage::renderImage(LPCTSTR folderPath, ImageEffectJob* effect)
{
  LPCTSTR name = (m_TempFilename != NULL) ? m_TempFilename : m_LocalFilename;
  if ((name == NULL) ||
    (mFlags & IS_FILE_NOT_READY))
    return FALSE;

  if (m_Image == NULL)
  {
    LPTSTR fullPath = AllocPathName(folderPath, name);
    if (!IsFileExist(fullPath))
    {
      TCHAR msg[256] = _T("No file found : ");
      StringCchCat(msg, 256, fullPath);
      StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG | SYNC_MSG_DEBUG);

      delete[] fullPath;
      return FALSE;
    }
    m_Image = new KBitmap();
    if (m_Image->LoadImageFromFile(fullPath, 0, m_MultiPageIndex) == NULL)
    {
      // if fail to load, check if encrypted
      LPTSTR kenc_temp_file = AllocAddString(fullPath, '_', _T("tmp"));
      if (!KDecryptFile(fullPath, kenc_temp_file) ||
        (m_Image->LoadImageFromFile(kenc_temp_file) == NULL))
      {
        delete m_Image;
        m_Image = NULL;
        mFlags |= IS_FILE_NOT_READY;
      }

      if (m_Image->handle() != NULL)
        StoreLogHistory(_T(__FUNCTION__), _T("CryptedFileReaded"), SYNC_MSG_DEBUG | SYNC_MSG_LOG);
      delete[] kenc_temp_file;
    }

    TCHAR msg[256] = _T("Render page image : ");
    StringCchCat(msg, 256, fullPath);
    if (m_Image == NULL)
      StringCchCat(msg, 256, _T(", fail"));

    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG | SYNC_MSG_DEBUG);
    delete[] fullPath;
  }

  if (m_Image != NULL)
  {
    if ((effect != NULL) && (effect->job > PropertyMode::PModeImageView) &&
      !KBitmap::IsSimpleImageEffect(effect))
    {
      if (m_CacheImage != NULL)
        delete m_CacheImage;
      m_CacheImage = new KBitmap(*m_Image);

      m_CacheImage->ApplyEffect(effect);
      if (mpPreviewEffect == NULL)
        mpPreviewEffect = new ImageEffectJob;
      memcpy(mpPreviewEffect, effect, sizeof(ImageEffectJob));
      mHasEffect = TRUE;

      TCHAR msg[256] = _T("Render cache image : ");
      StringCchCat(msg, 256, m_LocalFilename);
      StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG | SYNC_MSG_DEBUG);
    }
  }
  return TRUE;
}

unsigned __int64 KImagePage::GetImageCacheSize()
{
  if (m_Image != NULL)
  {
    unsigned __int64 size = (unsigned __int64) m_Image->lineBufferLength * abs(m_Image->height);

    if (m_CacheImage != NULL)
      size *= 2;

    return size;
  }
  return 0;
}

BOOL KImagePage::IsPropertyNeedImageCacheUpdate(PropertyMode mode)
{
  switch (mode)
  {
  case PropertyMode::PModeRotate:
  case PropertyMode::PModeMonochrome:
  case PropertyMode::PModeColorBalance:
  case PropertyMode::PModeColorLevel:
  case PropertyMode::PModeColorBright:
  case PropertyMode::PModeColorHLS:
  case PropertyMode::PModeColorHSV:
  case PropertyMode::PModeColorReverse:
    return TRUE;

  case PropertyMode::PModeResize:
  case PropertyMode::PModeCrop:
  default:
    return FALSE;
  }
  return FALSE;
}

BOOL KImagePage::IsPropertyNeedImageOverlayUpdate(PropertyMode mode)
{
  switch (mode)
  {
  case PropertyMode::PModeResize:
  case PropertyMode::PModeCrop:
    return TRUE;

  default:
    return FALSE;
  }
  return FALSE;
}

BOOL KImagePage::IsPropertyNeedSceenErase(ImageEffectJob* effect)
{
  if (effect->job == PropertyMode::PModeRotate)
  {
    return TRUE;
  }
  return FALSE;
}

HANDLE KImagePage::SaveAnnotateData()
{
  KMemoryStream mf;

  m_Annotates.StoreToBuffer(mf);

  HANDLE h = KMemoryAlloc(mf.Size());
  if (h != NULL)
  {
    LPBYTE m = (LPBYTE)KMemoryLock(h);
    if (m != NULL)
    {
      memcpy(m, mf.Data(), mf.Size());
      KMemoryUnlock(h);
    }
  }
  return h;
}

int KImagePage::RestoreAnnotateData(HANDLE h)
{
  KMemoryStream mf;
  int rtn = 0;

  LPBYTE buff = (LPBYTE)GlobalLock(h);
  if (buff != NULL)
  {
    mf.Attach(buff, GlobalSize(h), ReadOnly);
    m_Annotates.Clear();
    rtn = m_Annotates.LoadFromBuffer(mf);
    mf.Detach();

    GlobalUnlock(h);
  }
  return rtn;
}

int KImagePage::SetShapeSelected(KVoidPtrArray& shapeList)
{
  return m_Annotates.SetSelected(shapeList);
}

MouseState KImagePage::OnLButtonDown(PropertyMode mode, int nFlags, CPOINT& point, KImageDrawer& info, LPRECT ptUpdate)
{
  MouseState state = MouseState::MOUSE_OFF;

  if (IsAnnotateMode(mode)) // PModeAnnotateSelect <= PModeAnnotateLast
  {
    state = m_Annotates.OnLButtonDown(mode, nFlags, point, info, ptUpdate);
  }
  else if (IsSelectBlockMode(mode)) // PModeSelectRectangle <= PModePasteImage))
  {
    state = m_BlockEditor.OnLButtonDown(mode, 0, point, info, ptUpdate);
  }
  return state;
}

int KImagePage::OnMouseMove(MouseState mouseState, int nFlags, POINT& point, KImageDrawer& info, LPRECT ptUpdate)
{
  int r = 0;
  int flag = 0;
  if (nFlags & MK_CONTROL)
    flag |= WITH_XY_SAME_RATIO;
  if (nFlags & MK_SHIFT)
    flag |= AS_INTEGER_RATIO;

  if (IsAnnotateMouseMode(mouseState))
  {
    r = m_Annotates.OnMouseMove(mouseState, flag, point, ptUpdate);
  }
  else if ((MouseState::MOUSE_SELECT_RECTANGLE <= mouseState) && (mouseState <= MouseState::MOUSE_SELECT_HANDLE_MOVE))
  {
    r = m_BlockEditor.OnMouseMove(mouseState, flag, point, ptUpdate);
  }
  return r;
}

int KImagePage::OnMouseMoves(MouseState mouseState, int nFlags, LP_CPOINT points, int count, KImageDrawer& info, LPRECT ptUpdate)
{
  int r = 0;
  int flag = 0;

  if (IsAnnotateMouseMode(mouseState))
  {
    if (nFlags & MK_CONTROL)
      flag |= WITH_XY_SAME_RATIO;
    if (nFlags & MK_SHIFT)
      flag |= AS_INTEGER_RATIO;

    r = m_Annotates.OnMouseMoves(mouseState, flag, points, count, ptUpdate);
  }
  return r;
}

int KImagePage::OnLButtonUp(MouseState mouseState, int nFlags, CPOINT& point, KImageDrawer& info, LPRECT ptUpdate)
{
  int r = 0;
  int flag = 0;
  if (nFlags & MK_CONTROL)
    flag |= WITH_XY_SAME_RATIO;
  if (nFlags & MK_SHIFT)
    flag |= AS_INTEGER_RATIO;

  if (IsAnnotateMouseMode(mouseState))
  {
    r = m_Annotates.OnLButtonUp(mouseState, flag, point, ptUpdate);
    if (r & R_ANNOTATE_CHANGED)
      SetModified(FALSE, PageModifiedFlag::IMAGE_MODIFIED);
  }
  else if ((MouseState::MOUSE_SELECT_RECTANGLE <= mouseState) && (mouseState <= MouseState::MOUSE_SELECT_HANDLE_MOVE))
  {
    r = m_BlockEditor.OnLButtonUp(mouseState, flag, point, ptUpdate);
  }
  return r;
}

int KImagePage::OnRButtonDown(PropertyMode mode, int nFlags, POINT& point, KImageDrawer& info, LPRECT ptUpdate)
{
  int state = 0;

  if (IsAnnotateMode(mode))
  {
    state = m_Annotates.OnRButtonDown(mode, nFlags, point, info, ptUpdate);
  }
  else if (IsSelectBlockMode(mode)) // PModeSelectRectangle <= PModePasteImage))
  {
    state = 0;// m_BlockEditor.OnRButtonDown(mode, 0, point, info, ptUpdate);
  }

  return state;
}

int KImagePage::OnRButtonUp(MouseState mouseState, int nFlags, POINT& point, KImageDrawer& info, LPRECT ptUpdate)
{
  return 0;
}

int KImagePage::OnKeyDown(UINT nChar, UINT nFlags, PropertyMode mode, KImageDrawer& info, LPRECT ptUpdate)
{
  short shifted = GetKeyState(VK_SHIFT);

  if (nChar == VK_TAB)
  {
    if (PropertyMode::PModeAnnotateSelect == mode)
    {
      KShapeVector selectedItems;
      if (m_Annotates.GetSelectedObjects(selectedItems))
        m_Annotates.GetObjectsBound(selectedItems, *ptUpdate);
      selectedItems.clear();

      if (m_Annotates.SelectNextObject(selectedItems, !(shifted & 0xFF00)) > 0)
      {
        RECT rect;
        m_Annotates.GetObjectsBound(selectedItems, rect);
        AddRect(*ptUpdate, rect);
        return(R_REDRAW_SHAPE | R_REDRAW_HANDLE);
      }
    }
  }
  else if ((nChar == VK_LEFT) || (nChar == VK_RIGHT) ||
    (nChar == VK_UP) || (nChar == VK_DOWN))
  {
    POINT p = { 0, 0 };

    int move = info.ScreenSizeToImage(1);
    if (move <= 0)
      move = 1;
    if (shifted & 0xFF00)
      move *= 10;

    if ((nChar == VK_LEFT) || (nChar == VK_RIGHT))
      p.x = (nChar == VK_LEFT) ? -move : move;
    else if ((nChar == VK_UP) || (nChar == VK_DOWN))
      p.y = (nChar == VK_UP) ? -move : move;

    if (IsAnnotateMode(mode))
    {
      KShapeVector selectedItems;
      if (m_Annotates.GetSelectedObjects(selectedItems) > 0)
      {
        RECT rect;
        m_Annotates.GetObjectsBound(selectedItems, *ptUpdate);
        m_Annotates.MoveObjects(p);
        m_Annotates.GetObjectsBound(selectedItems, rect);
        AddRect(*ptUpdate, rect);

        return(R_ANNOTATE_CHANGED | R_REDRAW_SHAPE | R_REDRAW_HANDLE);
      }
    }
    else if (IsSelectShapeMode(mode)) // PModeSelectRectangle <=  PModeSelectEllipse)
    {
      return m_BlockEditor.MoveObjects(p, ptUpdate);
    }
  }
  else if (nChar == VK_ESCAPE)
  {
    if (IsAnnotateMode(mode))
    {
      if (m_Annotates.IsDragginState()) // cancel dragging
      {
        m_Annotates.OnDragCancel(ptUpdate);
        return(R_REDRAW_SHAPE | R_REDRAW_HANDLE);
      }
      else if (m_Annotates.FreeAllSelection(ptUpdate) > 0)
        return(R_ANNOTATE_CHANGED | R_REDRAW_SHAPE | R_REDRAW_HANDLE);
    }
    else if (IsSelectBlockMode(mode)) // (PropertyMode::PModeSelectRectangle <= PropertyMode::PModePasteImage)
    {
      if (m_BlockEditor.GetImageSelection((int*)ptUpdate))
      {
        m_BlockEditor.CloseBlockEditor();
        return (R_REDRAW_SHAPE | R_REDRAW_HANDLE);
      }
    }
    return 0;
  }
  else if (nChar == VK_CONTROL) // toggle group mode
  {
    if ((PropertyMode::PModeAnnotateSelect == mode) &&
      !m_Annotates.IsGroupSelectMode() &&
      !m_Annotates.IsDragginState()) // do not change when it already drag mode
    {
      KShapeVector selectedItems;
      if (m_Annotates.GetSelectedObjects(selectedItems) > 1)
      {
        TRACE(_T("GroupMode On\n"));
        m_Annotates.SetGroupSelectMode(TRUE);
        m_Annotates.GetObjectsBound(selectedItems, *ptUpdate);

        m_Annotates.mDragAction.SetSelectionBounds(TRUE, *ptUpdate);
        return(R_REDRAW_SHAPE | R_REDRAW_HANDLE);
      }
    }
  }
  else if (nChar == 'S') // make curve polygon
  {
    KShapeVector selectedItems;
    if (m_Annotates.GetSelectedObjects(selectedItems) > 0)
    {
      if (m_Annotates.MakeCurvePolygon(selectedItems, *ptUpdate) > 0)
        return(R_REDRAW_SHAPE | R_REDRAW_HANDLE | R_ANNOTATE_CHANGED);
    }
  }
  return 0;
}

int KImagePage::OnKeyUp(UINT nChar, UINT nFlags, PropertyMode mode, KImageDrawer& info, LPRECT ptUpdate)
{
  if (nChar == VK_CONTROL) // toggle group mode
  {
    if ((PropertyMode::PModeAnnotateSelect == mode) &&
      m_Annotates.IsGroupSelectMode() &&
      !m_Annotates.IsDragginState()) // do not change when it already drag mode
    {
      KShapeVector selectedItems;
      if (m_Annotates.GetSelectedObjects(selectedItems) > 1)
      {
        TRACE(_T("GroupMode Off\n"));
        m_Annotates.SetGroupSelectMode(FALSE);
        m_Annotates.GetObjectsBound(selectedItems, *ptUpdate);
        m_Annotates.mDragAction.SetSelectionBounds(FALSE, *ptUpdate);

        return(R_REDRAW_SHAPE | R_REDRAW_HANDLE);
      }
    }
  }
  return 0;
}

int KImagePage::SelectAll(PropertyMode mode, KImageDrawer& drawer, LPRECT updateRect)
{ 
  if (IsAnnotateMode(mode))
  {
    return m_Annotates.SelectAll(updateRect);
  }
  else if (IsSelectShapeMode(mode)) // PModeSelectRectangle <=  PModeSelectEllipse))
  {
    RECT rect;
    rect.left = rect.top = 0;
    rect.right = m_Image->width;
    rect.right = m_Image->height;
    if (updateRect != NULL)
      memcpy(updateRect, &rect, sizeof(RECT));
    return m_BlockEditor.Select(rect);
  }
  return 0;
}

int KImagePage::HasSelection(int type)
{
  int rtn = 0;
  if (type & SELECTION_ANNOTATE)
  {
    rtn = m_Annotates.GetSelectedObjectsCount(FALSE);
  }
  if (type & SELECTION_IMAGE_BLOCK)
  {
    if ((rtn == 0) && m_BlockEditor.mSelected)
      rtn = 1;
  }
  return rtn;
}

HANDLE KImagePage::CopyToClipboard(PropertyMode mode, BOOL bCut, int& clipboard_data_format)
{
  HANDLE h = NULL;
  if (IsSelectBlockMode(mode))
  {
    KBitmap* b = NULL;
    RECT rect;
    if (m_BlockEditor.GetImageSelection((int*)&rect))
    {
      if (rect.left < 0)
        rect.left = 0;
      if (rect.top < 0)
        rect.top = 0;
      if (rect.right > m_Image->width)
        rect.right = m_Image->width;
      if (rect.bottom > m_Image->height)
        rect.bottom = m_Image->height;

      if ((rect.left < rect.right) && (rect.top < rect.bottom))
      {
        PropertyMode mode = (PropertyMode)((int)m_BlockEditor.mMode - (int)PropertyMode::PModeSelectRectangle + (int)PropertyMode::PModeCopyRectangle);
        b = m_Image->CloneBitmap(rect, mode);

        TCHAR msg[128];
        StringCchPrintf(msg, 128, _T("CopyBlock shape=%s, ltrb=(%d,%d,%d,%d)"),
          GetBlockModeName((PropertyMode)m_BlockEditor.mMode), rect.left, rect.top, rect.right, rect.bottom);
        StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
      }
    }
    else if (m_BlockEditor.mMode == PropertyMode::PModePasteImage)
    {
      b = m_BlockEditor.mpFloatImage;
    }

    if (b != NULL)
    {
      h = b->GetDIB();
      if (m_BlockEditor.mMode != PropertyMode::PModePasteImage)
        delete b;
      clipboard_data_format = CF_DIB;
    }
  }
  else if (IsAnnotateMode(mode))
  {
    if (m_Annotates.GetSelectedObjectsCount(FALSE) > 0)
    {
      h = m_Annotates.GetSelectedClipboardData(bCut);
      clipboard_data_format = gVivaCustomClipboardFormat;
    }
  }
  else
  {
    // copy current image page

  }
  return h;
}

int KImagePage::PasteClipboardData(HANDLE h)
{
  return 0;
}

int KImagePage::DeleteSelection(LPRECT updateRect)
{
  int count = m_Annotates.DeleteSelection(updateRect);
  if (count > 0)
  {
    return (R_ANNOTATE_CHANGED| R_REDRAW_SHAPE | R_REDRAW_HANDLE);
  }
  return 0;
}

void KImagePage::DrawEditing(Graphics& g, KImageDrawer& info, PropertyMode mode, int flag)
{
  m_Annotates.Draw(g, info, flag);

  if ((PropertyMode::PModeSelectRectangle <= mode) && (mode <= PropertyMode::PModePasteImage))
    m_BlockEditor.DrawPreview(g, info);

  return;
}

BOOL KImagePage::GetImageSelection(int* param)
{
  return m_BlockEditor.GetImageSelection(param);
}

BOOL KImagePage::GetUpdateRect(ImageEffectJob* effect, RECT& rect)
{
  if ((PropertyMode::PModeSelectRectangle <= effect->job) && (effect->job <= PropertyMode::PModeSelectEllipse))
  {
    return m_BlockEditor.GetUpdateRect(rect, effect->param, FALSE);
  }
  return FALSE;
}

BOOL KImagePage::CloseProperty(ImageEffectJob* effect, RECT& rect)
{
  if ((PropertyMode::PModeSelectRectangle <= effect->job) && (effect->job <= PropertyMode::PModeSelectEllipse))
  {
    return m_BlockEditor.GetUpdateRect(rect, effect->param, TRUE);
  }
  return FALSE;
}

int KImagePage::CloseToolMode(PropertyMode mode, PropertyMode nextMode, LPRECT updateRect)
{
  if (IsAnnotateMode(mode)) //(PModeAnnotateSelect <= mode) && (mode <= PModeAnnotateEditText))
  {
    // Annotate 도구에서 다른 도구로 변경되면 선택을 해제한다.
    if (!IsAnnotateMode(nextMode))
      return m_Annotates.FreeAllSelection(updateRect);
  }
  else if ((PropertyMode::PModeSelectRectangle <= mode) && (mode <= PropertyMode::PModeCopyEllipse))
  {
    if (m_BlockEditor.GetImageSelection((int*)updateRect))
    {
      m_BlockEditor.CloseBlockEditor();
      return 1;
    }
  }
  else if (mode == PropertyMode::PModePasteImage)
  {
    return EndFloatingImagePaste(TRUE, updateRect);
  }
  return 0;
}

int KImagePage::ApplyEffect(ImageEffectJob* effect, BOOL bAll)
{
  // preview로 이미 적용된 경우에는 프리뷰 이미지로 대체
  if (mHasEffect)
  {
    switch (effect->job)
    {
    case PropertyMode::PModeRotate:
    case PropertyMode::PModeColorReverse:
    case PropertyMode::PModeMonochrome:
    case PropertyMode::PModeColorLevel:
    case PropertyMode::PModeColorBalance:
    case PropertyMode::PModeColorBright:
    case PropertyMode::PModeColorHLS:
    case PropertyMode::PModeColorHSV:
      if ((mpPreviewEffect != NULL) && (m_CacheImage != NULL) &&
        (CompareEffect(mpPreviewEffect, effect) == 0))
      {
        delete mpPreviewEffect;
        mpPreviewEffect = NULL;
        delete m_Image;

        m_Image = m_CacheImage;
        m_CacheImage = NULL;
        mHasEffect = FALSE;
        SetModified(TRUE, PageModifiedFlag::IMAGE_MODIFIED);

        TCHAR msg[256];
        KBitmap::GetEffectInfoString(effect, msg, 256);
        StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);

        int rtn = EFFECT_CHANGE_IMAGE;
        if (effect->job == PropertyMode::PModeRotate)
          rtn |= EFFECT_CHANGE_SIZE;

        return rtn;
      }
    }
  }

  int r = 0;
  if (m_Image != NULL)
  {
    r = m_Image->ApplyEffect(effect);
    ClearPreviewImage();
    SetModified(TRUE, PageModifiedFlag::IMAGE_MODIFIED);
  }
  return r;
}

BOOL KImagePage::StartFloatingImagePaste(PBITMAPINFOHEADER bi, UINT buffSize, LPRECT updateRect)
{
  // only available 24-bit color image
  if ((m_Image->bitCount < 24) && (bi->biBitCount != m_Image->bitCount))
    return FALSE;

  return m_BlockEditor.StartFloatingImagePaste(m_Image->width, m_Image->height, bi, buffSize, updateRect);
}

BOOL KImagePage::HasFloatingImage()
{
  return (m_BlockEditor.mpFloatImage != NULL);
}

BOOL KImagePage::GetFloatingImagePasteParam(PasteImageActionParam* p)
{
  if (m_BlockEditor.mpFloatImage != NULL)
  {
    memcpy(&p->rect, &m_BlockEditor.mRect, sizeof(RECT));
    p->image = m_BlockEditor.mpFloatImage->GetDIB();
    return TRUE;
   }
  return FALSE;
}

BOOL KImagePage::EndFloatingImagePaste(BOOL bDone, LPRECT updateRect)
{
  BOOL rtn = FALSE;

  if (m_BlockEditor.mpFloatImage != NULL)
  {
    if (updateRect != NULL)
      memcpy(updateRect, &m_BlockEditor.mRect, sizeof(RECT));
    if (bDone)
    {
      int x, y, w, h;

      TCHAR msg[256];
      StringCchPrintf(msg, 256, _T("Paste image : destination rect(%d,%d,%d,%d) size(%d,%d), source size(%d,%d)"), 
        m_BlockEditor.mRect.left, m_BlockEditor.mRect.top, m_BlockEditor.mRect.right, m_BlockEditor.mRect.bottom, 
        m_Image->width, m_Image->height, m_BlockEditor.mpFloatImage->width, m_BlockEditor.mpFloatImage->height);
      StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);

      if ((m_BlockEditor.mRect.right <= 0) || (m_BlockEditor.mRect.bottom <= 0) ||
        (m_Image->width <= m_BlockEditor.mRect.left) || (m_Image->height <= m_BlockEditor.mRect.top))
        return FALSE;

      // resize if stretched
      w = m_BlockEditor.mRect.right - m_BlockEditor.mRect.left;
      h = m_BlockEditor.mRect.bottom - m_BlockEditor.mRect.top;
      if ((w != m_BlockEditor.mpFloatImage->width) ||
        (h != m_BlockEditor.mpFloatImage->height))
      {
        m_BlockEditor.mpFloatImage->Resize(0, w, h);
      }

      if ((m_BlockEditor.mRect.left < 0) || (m_BlockEditor.mRect.top < 0))
      {
        int left = (m_BlockEditor.mRect.left < 0) ? abs(m_BlockEditor.mRect.left) : 0;
        int top = (m_BlockEditor.mRect.top < 0) ? abs(m_BlockEditor.mRect.top) : 0;

        m_BlockEditor.mpFloatImage->Crop(left, top, m_BlockEditor.mpFloatImage->width, m_BlockEditor.mpFloatImage->height);

        if (m_BlockEditor.mRect.left < 0)
          m_BlockEditor.mRect.left = 0;
        if (m_BlockEditor.mRect.top < 0)
          m_BlockEditor.mRect.top = 0;
      }

      w = m_BlockEditor.mpFloatImage->width;
      h = m_BlockEditor.mpFloatImage->height;
      x = (m_BlockEditor.mRect.left < 0) ? abs(m_BlockEditor.mRect.left) : 0;
      y = (m_BlockEditor.mRect.top < 0) ? abs(m_BlockEditor.mRect.top) : 0;

      int dx = m_BlockEditor.mRect.left;
      int dy = m_BlockEditor.mRect.top;
      if (dx < 0)
        dx = 0;
      if (dy < 0)
        dy = 0;
      if ((dx < m_Image->width) && (dy < m_Image->height))
      {
        if ((x + w) > m_Image->width)
          w = m_Image->width - x;
        if ((y + h) > m_Image->height)
          h = m_Image->height - y;

        if ((x + w) > m_BlockEditor.mpFloatImage->width)
          w = m_BlockEditor.mpFloatImage->width - x;
        if ((y + h) > m_BlockEditor.mpFloatImage->height)
          h = m_BlockEditor.mpFloatImage->height - y;

        if ((w > 0) && (h > 0))
        {
          m_Image->drawAlpha(dx, dy,
            m_BlockEditor.mpFloatImage, x, y, w, h, 255);
          SetModified(FALSE, PageModifiedFlag::IMAGE_MODIFIED);
          rtn = TRUE;
        }
      }
    }
  }
  m_BlockEditor.CloseBlockEditor();
  return rtn;
}

BOOL KImagePage::restoreImageData(HANDLE h_dib)
{
  if (h_dib != NULL)
  {
    if (m_Image != NULL)
      delete m_Image;
    m_Image = new KBitmap(h_dib);

    // clear cache and thumb image
    if (m_CacheImage != NULL)
    {
      delete m_CacheImage;
      m_CacheImage = NULL;
    }
    if (m_ThumbImage != NULL)
    {
      delete m_ThumbImage;
      m_ThumbImage = NULL;
    }
  }
  return (h_dib != NULL);
}

int KImagePage::ShapeEditCommand(int channel, int* params, void* extra, RECT& rect)
{
  KShapeVector selectedItems;
  if (m_Annotates.GetSelectedObjects(selectedItems) > 0)
  {
    m_Annotates.GetObjectsBound(selectedItems, rect);
    int r = 0;
    SHAPE_COMMAND_TYPE commandType = (SHAPE_COMMAND_TYPE)params[0];

    switch (commandType)
    {
    case SHAPE_ALIGN_LEFT:
      r = m_Annotates.AlignLeft(selectedItems);
      break;
    case SHAPE_ALIGN_CENTER:
      r = m_Annotates.AlignCenter(selectedItems);
      break;
    case SHAPE_ALIGN_RIGHT:
      r = m_Annotates.AlignRight(selectedItems);
      break;
    case SHAPE_ALIGN_TOP:
      r = m_Annotates.AlignTop(selectedItems);
      break;
    case SHAPE_ALIGN_VCENTER:
      r = m_Annotates.AlignVCenter(selectedItems);
      break;
    case SHAPE_ALIGN_BOTTOM:
      r = m_Annotates.AlignBottom(selectedItems);
      break;
    case SHAPE_ORDER_FORWARD:
      r = m_Annotates.BringForward(selectedItems);
      break;
    case SHAPE_ORDER_BACKWARD:
      r = m_Annotates.SendBackward(selectedItems);
      break;
    case SHAPE_ORDER_TOP:
      r = m_Annotates.BringToTop(selectedItems);
      break;
    case SHAPE_ORDER_BOTTOM:
      r = m_Annotates.SendToBottom(selectedItems);
      break;
    case SHAPE_APPLY_PROPERTY:
      {
        DWORD fill = params[1];
        OutlineStyle lineStyle;
        lineStyle.Color = params[2];
        lineStyle.Style = params[3];
        lineStyle.Width = params[4];
        LPCTSTR facename = (LPCTSTR)extra;
        DWORD fontColor = params[5];
        int fontSize = params[6];
        int attr = params[7];

        r = m_Annotates.ApplyProperty(selectedItems, channel,
          fill, lineStyle, facename, fontColor, fontSize, attr);
        break;
      }
    }
    if (r > 0)
    {
      RECT newrect;
      m_Annotates.GetObjectsBound(selectedItems, newrect);
      AddRect(rect, newrect);
      return(R_REDRAW_SHAPE | R_REDRAW_HANDLE | R_ANNOTATE_CHANGED);
    }
  }
  return 0;
}

void KImagePage::OnSelectionChanged(KShapeBase* shape)
{
  m_Annotates.OnSelectionChanged(shape);
}

KShapeBase* KImagePage::IsNeedTextboxEditText()
{
  return m_Annotates.IsNeedTextboxEditText();
}

void readyUniqueFilename(LPCTSTR path, LPTSTR* name)
{
  LPTSTR pathname = AllocPathName(path, *name);
  BOOL isExist = IsFileExistAttribute(pathname);
  delete[] pathname;

  if (isExist)
  {
    TCHAR tempName[KMAX_FILENAME];
    TCHAR ext[16] = _T("");
    StringCchCopy(tempName, KMAX_FILENAME, *name);
    LPTSTR e = StrRChr(tempName, NULL, '.');
    if (e != NULL)
    {
      *e = '\0';
      e++;
      StringCchCopy(ext, 16, e);
    }
    int nameLen = lstrlen(tempName);
    TCHAR randkey[10];

    while (isExist)
    {
      StringCchCopyN(tempName, KMAX_FILENAME, *name, nameLen);
      StringCchPrintf(randkey, 10, _T("-%.4x."), rand());
      StringCchCat(tempName, KMAX_FILENAME, randkey);
      if (ext != NULL)
        StringCchCat(tempName, KMAX_FILENAME, ext);

      pathname = AllocPathName(path, tempName);
      isExist = IsFileExistAttribute(pathname);
      delete[] pathname;
    }

    TCHAR msg[256];
    StringCchPrintf(msg, 256, _T("old=%s, new=%s"), *name, tempName);
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);

    delete[] *name;
    *name = AllocString(tempName);
  }
}

// '$date' : date of today, * '$time' : current time, '$uoid' : user OID string, '$user' : user ID string, '$rand' : 4 - digit random number, '$folder' : parent folder name, '$serial' : 1 - based image total index, '$index' : 1 - based image index on folder

/*
* '$$' : '$' char
* '$date' : date of today, '20190118'
* '$time' : current time, '1230'
* '$uoid' : user OID string
* '$user' : user ID string
* '$rand' : 4-digit random number
* '$folder' : parent folder name
* '$serial' : 1-based image total index
* '$index' : 1-based image index on folder
*/
void KImagePage::MakeDefaultImageFilename(LPTSTR pageName, int length, LPCTSTR formatStr, int index, int base)
{
  static LPTSTR FilenameFormatKeyDate = _T("date");
  static LPTSTR FilenameFormatKeyTime = _T("time");
  static LPTSTR FilenameFormatKeyUOID = _T("uoid");
  static LPTSTR FilenameFormatKeyUser = _T("user");
  static LPTSTR FilenameFormatKeyRand = _T("rand");
  static LPTSTR FilenameFormatKeyFolder = _T("folder");
  static LPTSTR FilenameFormatKeyIndex = _T("index");
  static LPTSTR FilenameFormatKeySerial = _T("serial");

  static TCHAR YYYMMDD_FORMAT_STR_T[] = _T("%.4d%.2d%.2d");
  static TCHAR HHMM_FORMAT_STR_T[] = _T("%.2d%.2d");

  if (formatStr == NULL)
  {
    StringCchPrintf(pageName, length, _T("Image_%.4d."), index);
    return;
  }
  int i = 0;
  SYSTEMTIME systime;
  GetLocalTime(&systime);

  while (*formatStr != '\0')
  {
    if (*formatStr == '$')
    {
      if (formatStr[1] == '$')
      {
        formatStr++;
        pageName[i++] = '$';
      }
      else
      {
        formatStr++;
        if (_wcsnicmp(formatStr, FilenameFormatKeyDate, 4) == 0)
        {
          TCHAR buff[32];
          StringCchPrintf(buff, 32, YYYMMDD_FORMAT_STR_T, systime.wYear, systime.wMonth, systime.wDay);
          StringCchCopy(&pageName[i], length - i, buff);
          i = lstrlen(pageName);
          formatStr += lstrlen(FilenameFormatKeyDate) - 1;
        }
        else if (_wcsnicmp(formatStr, FilenameFormatKeyTime, 4) == 0)
        {
          TCHAR buff[32];
          StringCchPrintf(buff, 32, HHMM_FORMAT_STR_T, systime.wHour, systime.wMinute);
          StringCchCopy(&pageName[i], length - i, buff);
          i = lstrlen(pageName);
          formatStr += lstrlen(FilenameFormatKeyTime) - 1;
        }
        else if (_wcsnicmp(formatStr, FilenameFormatKeyUOID, 4) == 0)
        {
          StringCchCopy(&pageName[i], length - i, mUserOid);
          i = lstrlen(pageName);
          formatStr += lstrlen(FilenameFormatKeyUOID) - 1;
        }
        else if (_wcsnicmp(formatStr, FilenameFormatKeyUser, 4) == 0)
        {
          StringCchCopy(&pageName[i], length - i, mUserName);
          i = lstrlen(pageName);
          formatStr += lstrlen(FilenameFormatKeyUser) - 1;
        }
        else if (_wcsnicmp(formatStr, FilenameFormatKeyRand, 4) == 0)
        {
          TCHAR buff[8];
          StringCchPrintf(buff, 8, _T("%.4x"), rand());
          StringCchCopy(&pageName[i], length - i, buff);
          i = lstrlen(pageName);
          formatStr += lstrlen(FilenameFormatKeyRand) - 1;
        }
        else if (_wcsnicmp(formatStr, FilenameFormatKeyFolder, 5) == 0)
        {
          KImageBase* parent = GetParent();
          if ((parent != NULL) && (parent->GetParent() != NULL)) // not a root
          {
            LPCTSTR name = parent->GetName();
            if ((name != NULL) && (lstrlen(name) > 0))
            {
              StringCchCopy(&pageName[i], length - i, name);
              i = lstrlen(pageName);
            }
          }
          formatStr += lstrlen(FilenameFormatKeyFolder) - 1;
        }
        else if (_wcsnicmp(formatStr, FilenameFormatKeySerial, 5) == 0)
        {
          TCHAR buff[8];
          StringCchPrintf(buff, 8, _T("%.3d"), index);
          StringCchCopy(&pageName[i], length - i, buff);
          i = lstrlen(pageName);
          formatStr += lstrlen(FilenameFormatKeySerial) - 1;
        }
        else if (_wcsnicmp(formatStr, FilenameFormatKeyIndex, 5) == 0)
        {
          TCHAR buff[8];
          StringCchPrintf(buff, 8, _T("%.3d"), index-base+1);
          StringCchCopy(&pageName[i], length - i, buff);
          i = lstrlen(pageName);
          formatStr += lstrlen(FilenameFormatKeyIndex) - 1;
        }
      }
    }
    else
    {
      pageName[i++] = *formatStr;
    }

    formatStr++;
  }
  if ((i >= 0) && (i < length))
    pageName[i++] = '.';
  if ((i >= 0) && (i < length))
    pageName[i] = '\0';
}

void KImagePage::ReadyDefaultName(LPCTSTR pathName, LPTSTR pageName, LPCTSTR defExt)
{
  if (m_Image->bitCount <= 8)
    defExt = NULL;

  while (TRUE)
  {
    MakeDefaultImageFilename(pageName, 60, gImageFilenameFormat, g_DefaultPageNameIndex++, 1);
    // StringCchPrintf(pageName, 60, _T("Image_%.4d."), g_DefaultPageNameIndex++);

    StringCchCat(pageName, 60, ((defExt != NULL) ? defExt : _T("bmp")));
    LPTSTR pathname = AllocPathName(pathName, pageName);
    BOOL isExist = IsFileExistAttribute(pathname);
    delete[] pathname;
    if (!isExist)
      return;
  }
}

void KImagePage::ReadyDefaultName(LPCTSTR pathName, LPTSTR pageName, LPCTSTR defExt, int index, int base)
{
  if (m_Image->bitCount <= 8)
    defExt = NULL;

  while (TRUE)
  {
    MakeDefaultImageFilename(pageName, 60, gImageFilenameFormat, index, base);
    // StringCchPrintf(pageName, 60, _T("Image_%.4d."), g_DefaultPageNameIndex++);

    StringCchCat(pageName, 60, ((defExt != NULL) ? defExt : _T("bmp")));
    LPTSTR pathname = AllocPathName(pathName, pageName);
    BOOL isExist = IsFileExistAttribute(pathname);
    delete[] pathname;
    if (!isExist)
      return;
  }
}

void KImagePage::readyImagePathName(LPCTSTR pathName, BOOL asUnique)
{
  if (m_LocalFilename == NULL)
  {
    if (mName == NULL)
    {
      TCHAR defPageName[64];
      ReadyDefaultName(pathName, defPageName, GetDefaultImageFileExt());
      SetName(defPageName);
    }
    m_LocalFilename = AllocString(mName);
  }
  else if (IsFullPath(m_LocalFilename))
  {
    LPCTSTR e = StrRChr(m_LocalFilename, NULL, '\\');
    if (e != NULL)
    {
      int len = (e - m_LocalFilename);
      if ((len != lstrlen(pathName)) || (len != lstrlen(pathName)) ||
        (StrCmpNI(pathName, m_LocalFilename, len) != 0))
      {
        e++;
        LPTSTR filename = AllocString(e);// AllocPathName(pathName, e);
        delete[] m_LocalFilename;
        m_LocalFilename = filename;
      }
      else
      {
        // make it as relative
        e++;
        LPTSTR filename = AllocString(e);
        delete[] m_LocalFilename;
        m_LocalFilename = filename;

      }
    }
  }
  // check image depth
  if ((m_Image != NULL) && (m_Image->bitCount <= 8))
  {
    LPTSTR e = StrRChr(m_LocalFilename, NULL, '.');
    if (e != NULL)
    {
      e++;
      if (CompareStringICNC(e, _T("bmp")) != 0)
      {
        if (lstrlen(e) == 3)
        {
          StrCpy(e, _T("bmp"));
        }
        else
        {
          *e = '\0';
          LPTSTR newName = AllocPathName(m_LocalFilename, _T("bmp"));
          delete[] m_LocalFilename;
          m_LocalFilename = newName;
        }
      }
    }
  }

  if (m_LocalFilename == NULL)
  {
    if (mName != NULL)
      m_LocalFilename = AllocString(mName);
  }

  if (asUnique)
  {
    // check if same filename exist
    readyUniqueFilename(pathName, &m_LocalFilename);
  }
}

LPTSTR KImagePage::readyTempFilename(LPCTSTR pathName)
{
  if (m_LocalFilename != NULL)
  {
    int len = lstrlen(m_LocalFilename) + 10;
    m_TempFilename = new TCHAR[len];
    StringCchCopy(m_TempFilename, len, m_LocalFilename);

    int full_len = lstrlen(pathName) + len;
    LPTSTR fullname = new TCHAR[full_len];
    LPTSTR e = StrRChr(m_TempFilename, NULL, '.');

    if (e == NULL)
      e = m_TempFilename + lstrlen(m_TempFilename);

    LPTSTR ext = StrRChr(m_LocalFilename, NULL, '.');
    while (TRUE) {
      StringCchPrintf(e, 8, _T("_%.8x"), rand());
      if (ext != NULL)
        StringCchCat(m_TempFilename, len, ext);
      MakeFullPathName(fullname, full_len, pathName, m_TempFilename);
      if (!IsFileExist(fullname))
        break;
    }

    delete[] fullname;
  }
  return m_TempFilename;
}

int KImagePage::MoveToWorkingFolder(LPCTSTR pathName)
{
  // check if it is out of working path
  if (m_LocalFilename != NULL)
  {
    if (IsChildFolderOrFile(pathName, m_LocalFilename, FALSE))
    {
      // it is inside
      // change it as relative name
      LPTSTR n = &m_LocalFilename[lstrlen(pathName)];
      ASSERT((n != NULL) && (*n == '\\'));

      {
        n++;
        n = AllocString(n);
        delete[] m_LocalFilename;
        m_LocalFilename = n;
      }
      return 0;
    }

    // copy to work folder
    LPTSTR n = StrRChr(m_LocalFilename, NULL, '\\');
    if (n != NULL)
    {
      n++;
      LPTSTR newname = AllocString(n);
      readyUniqueFilename(pathName, &newname);
      LPTSTR newpath = AllocPathName(pathName, newname);
      CopyFile(m_LocalFilename, newpath, TRUE);
      delete[] m_LocalFilename;
      delete[] newpath;
      m_LocalFilename = newname;
      return 1;
    }
  }
  else
  {
    // 이름을 생성하고 저장한다.
    return StoreImageFile(pathName, NULL, NULL, TRUE);
  }
  return 0;
}

#ifdef USE_XML_STORAGE
HRESULT KImagePage::ReadAttributes(IXmlReader* pReader, int flag)
{
  const WCHAR* pwszPrefix;
  const WCHAR* pwszLocalName;
  const WCHAR* pwszValue;

  HRESULT hr = pReader->MoveToFirstAttribute();
  while (hr != S_FALSE)
  {
    if (!pReader->IsDefault())
    {
      UINT cwchPrefix;
      if (FAILED(hr = pReader->GetPrefix(&pwszPrefix, &cwchPrefix))) {
        TCHAR msg[256];
        StringCchPrintf(msg, 256, _T("Error getting prefix, error is %08.8lx"), hr);
        StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
        return hr;
      }

      if (FAILED(hr = pReader->GetLocalName(&pwszLocalName, NULL))) {
        TCHAR msg[256];
        StringCchPrintf(msg, 256, _T("Error getting local name, error is %08.8lx"), hr);
        StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
        return hr;
      }

      if (FAILED(hr = pReader->GetValue(&pwszValue, NULL))) {
        TCHAR msg[256];
        StringCchPrintf(msg, 256, _T("Error getting value, error is %08.8lx"), hr);
        StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
        return hr;
      }

      if (CompareStringNC(pwszLocalName, tagName) == 0)
      {
        if (mName != NULL)
          delete[] mName;
        mName = AllocString(pwszValue);
      }
      else if (CompareStringNC(pwszLocalName, tagFileName) == 0)
      {
        if (m_LocalFilename != NULL)
          delete[] m_LocalFilename;
        m_LocalFilename = AllocString(pwszValue);
      }
      else if (CompareStringNC(pwszLocalName, tagEcmInfo) == 0)
      {
        if (mpEcmInfo != NULL)
          delete[] mpEcmInfo;
        mpEcmInfo = AllocString(pwszValue);
      }

      /*
      if cwchPrefix > 0)
        wprintf(L"Attr: %s:%s=\"%s\" \n", pwszPrefix, pwszLocalName, pwszValue);
      else
        wprintf(L"Attr: %s=\"%s\" \n", pwszLocalName, pwszValue);
      */
    }

    hr = pReader->MoveToNextAttribute();
  }
  return hr;
}

HRESULT KImagePage::ReadDocument(CComPtr<IXmlReader> pReader, KVoidPtrArray* pShapeList, int flag)
{
  HRESULT hr;

  if (FAILED(hr = ReadAttributes(pReader, flag))) {
    TCHAR msg[256];
    StringCchPrintf(msg, 256, _T("Error writing attributes, error is %08.8lx"), hr);
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
    return hr;
  }
  if (FAILED(hr = pReader->MoveToElement())) {
    TCHAR msg[256];
    StringCchPrintf(msg, 256, _T("Error moving to the element that owns the current attribute node, error is %08.8lx"), hr);
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
    return hr;
  }
  if (pReader->IsEmptyElement())
    return S_OK;

  XmlNodeType nodeType;
  const WCHAR* pwszPrefix;
  const WCHAR* pwszLocalName;
  const WCHAR* pwszValue;
  UINT cwchPrefix;
  MetaKeyNameValue* m = NULL;

  while (S_OK == (hr = pReader->Read(&nodeType))) {
    switch (nodeType) {

    case XmlNodeType_XmlDeclaration:
      if (FAILED(hr = ReadAttributes(pReader, flag))) {
        TCHAR msg[256];
        StringCchPrintf(msg, 256, _T("Error writing attributes, error is %08.8lx"), hr);
        StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
        return hr;
      }
      break;

    case XmlNodeType_Element:
      if (FAILED(hr = pReader->GetPrefix(&pwszPrefix, &cwchPrefix))) {
        TCHAR msg[256];
        StringCchPrintf(msg, 256, _T("Error getting prefix, error is %08.8lx"), hr);
        StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
        return hr;
      }
      if (FAILED(hr = pReader->GetLocalName(&pwszLocalName, NULL))) {
        TCHAR msg[256];
        StringCchPrintf(msg, 256, _T("Error getting local name, error is %08.8lx"), hr);
        StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
        return hr;
      }
      /*
      if (cwchPrefix > 0)
        wprintf(L"Element: %s:%s\n", pwszPrefix, pwszLocalName);
      else
        wprintf(L"Element: %s\n", pwszLocalName);
      */
      if (CompareStringNC(pwszLocalName, tagLine) == 0)
      {
        KShapeLine* child = new KShapeLine();
        hr = child->Read(pReader);
        if (FAILED(hr))
        {
          delete child;
          return hr;
        }
        m_Annotates.AddShape(child);
        if (pShapeList != NULL)
          pShapeList->push_back(child);
      }
      else if (CompareStringNC(pwszLocalName, tagRectangle) == 0)
      {
        KShapeRectangle* child = new KShapeRectangle();
        hr = child->Read(pReader);
        if (FAILED(hr))
        {
          delete child;
          return hr;
        }
        m_Annotates.AddShape(child);
        if (pShapeList != NULL)
          pShapeList->push_back(child);
      }
      else if (CompareStringNC(pwszLocalName, tagEllipse) == 0)
      {
        KShapeEllipse* child = new KShapeEllipse();
        hr = child->Read(pReader);
        if (FAILED(hr))
        {
          delete child;
          return hr;
        }
        m_Annotates.AddShape(child);
        if (pShapeList != NULL)
          pShapeList->push_back(child);
      }
      else if (CompareStringNC(pwszLocalName, tagPolygon) == 0)
      {
        KShapePolygon* child = new KShapePolygon();
        hr = child->Read(pReader);
        if (FAILED(hr))
        {
          delete child;
          return hr;
        }
        m_Annotates.AddShape(child);
        if (pShapeList != NULL)
          pShapeList->push_back(child);
      }
      else if (CompareStringNC(pwszLocalName, tagTextbox) == 0)
      {
        KShapeTextbox* child = new KShapeTextbox();
        hr = child->Read(pReader);
        if (FAILED(hr))
        {
          delete child;
          return hr;
        }
        m_Annotates.AddShape(child);
        if (pShapeList != NULL)
          pShapeList->push_back(child);
      }
      else if (CompareStringNC(pwszLocalName, tagMetaData) == 0)
      {
        m = new MetaKeyNameValue;
        KMetadataInfo::InitMetaKeyNameValue(m);
        hr = ReadMetaData(pReader, m);
        if (FAILED(hr))
        {
          delete m;
          return hr;
        }
        else
        {
          mMetadataContainer.Add(m);
          m = NULL;
        }
      }
      else
      {
        TCHAR msg[256];
        StringCchPrintf(msg, 256, _T("Unknown element %s"), pwszLocalName);
        StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
      }

      if (FAILED(hr = pReader->MoveToElement())) {
        TCHAR msg[256];
        StringCchPrintf(msg, 256, _T("Error moving to the element that owns the current attribute node, error is %08.8lx"), hr);
        StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
        return hr;
      }
      /*
      if (pReader->IsEmptyElement())
        wprintf(L" (empty element)\n");
      */
      break;

    case XmlNodeType_EndElement:
      if (FAILED(hr = pReader->GetPrefix(&pwszPrefix, &cwchPrefix))) {
        TCHAR msg[256];
        StringCchPrintf(msg, 256, _T("Error getting prefix, error is %08.8lx"), hr);
        StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
        return hr;
      }
      if (FAILED(hr = pReader->GetLocalName(&pwszLocalName, NULL))) {
        TCHAR msg[256];
        StringCchPrintf(msg, 256, _T("Error getting local name, error is %08.8lx"), hr);
        StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
        return hr;
      }
      return S_OK;
      break;

    case XmlNodeType_Text:

    case XmlNodeType_Whitespace:
      if (FAILED(hr = pReader->GetValue(&pwszValue, NULL)))
      {
        TCHAR msg[256];
        StringCchPrintf(msg, 256, _T("Error getting value on Text, error is %08.8lx"), hr);
        StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
        return hr;
      }
      //wprintf(L"Whitespace text: >%s<\n", pwszValue);
      break;

    case XmlNodeType_CDATA:
      if (FAILED(hr = pReader->GetValue(&pwszValue, NULL))) {
        TCHAR msg[256];
        StringCchPrintf(msg, 256, _T("Error getting value, error is %08.8lx"), hr);
        StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
        return hr;
      }
      // wprintf(L"CDATA: %s\n", pwszValue);
      break;

    case XmlNodeType_ProcessingInstruction:
      if (FAILED(hr = pReader->GetLocalName(&pwszLocalName, NULL))) {
        TCHAR msg[256];
        StringCchPrintf(msg, 256, _T("Error getting name, error is %08.8lx"), hr);
        StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
        return hr;
      }
      if (FAILED(hr = pReader->GetValue(&pwszValue, NULL))) {
        TCHAR msg[256];
        StringCchPrintf(msg, 256, _T("Error getting value, error is %08.8lx"), hr);
        StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
        return hr;
      }
      //wprintf(L"Processing Instruction name:%S value:%S\n", pwszLocalName, pwszValue);
      break;

    case XmlNodeType_Comment:
      if (FAILED(hr = pReader->GetValue(&pwszValue, NULL))) {
        TCHAR msg[256];
        StringCchPrintf(msg, 256, _T("Error getting value, error is %08.8lx"), hr);
        StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
        return hr;
      }
      //wprintf(L"Comment: %s\n", pwszValue);
      break;
    } // end of switch
  } // end of while
  return S_OK;
}

int KImagePage::ValidateMetadatas(KEcmDocTypeInfo& info)
{
  KMetadataVectorIterator it = mMetadataContainer.mVector.begin();
  while (it != mMetadataContainer.mVector.end())
  {
    MetaKeyNameValue* p = (MetaKeyNameValue*)*it;
    if (!info.IsValidKey(p->key))
    {
      it = mMetadataContainer.mVector.erase(it);
      KMetadataInfo::ClearMetaKeyNameValue(p);
    }
    else
      it++;
  }
  // copy DocTypeOID
  mMetadataContainer.SetDocTypeInfo(info.mDocTypeOid, info.mpDocTypeName);

  return (int)mMetadataContainer.mVector.size();
}

HRESULT KImagePage::StoreDocument(CComPtr<IXmlWriter> pWriter, BOOL selectedOnly)
{
  HRESULT hr;

  if (FAILED(hr = pWriter->WriteStartElement(NULL, tagImage, NULL)))
  {
    TCHAR msg[256];
    StringCchPrintf(msg, 256, _T("Error, Method: WriteStartElement, error is %08.8lx"), hr);
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
    return hr;
  }
  if (FAILED(hr = pWriter->WriteAttributeString(NULL, tagName, NULL,
    mName)))
  {
    TCHAR msg[256];
    StringCchPrintf(msg, 256, _T("Error, Method: WriteAttributeString, error is %08.8lx"), hr);
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
    return hr;
  }
  
  if (FAILED(hr = pWriter->WriteAttributeString(NULL, tagFileName, NULL,
    m_LocalFilename)))
  {
    TCHAR msg[256];
    StringCchPrintf(msg, 256, _T("Error, Method: WriteAttributeString, error is %08.8lx"), hr);
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
    return hr;
  }

  if (mpEcmInfo != NULL)
  {
    if (FAILED(hr = pWriter->WriteAttributeString(NULL, tagEcmInfo, NULL, mpEcmInfo)))
    {
      TCHAR msg[256];
      StringCchPrintf(msg, 256, _T("Error, Method: WriteAttributeString, error is %08.8lx"), hr);
      StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
      return hr;
    }
  }

  hr = StoreMetaData(pWriter);
  if (FAILED(hr))
    return hr;

  if (m_Annotates.GetCount() > 0) {
    hr = m_Annotates.Store(pWriter);
    if (FAILED(hr))
      return hr;
  }
  
  if (FAILED(hr = pWriter->WriteEndElement()))
  {
    TCHAR msg[256];
    StringCchPrintf(msg, 256, _T("Error, Method: WriteEndElement, error is %08.8lx"), hr);
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
    return hr;
  }

  return S_OK;
}
#else

DWORD KImagePage::ReadDocument(KMemoryStream& ms, KVoidPtrArray* pShapeList, int flag)
{
  DWORD blockSize = ms.ReadDword();
  DWORD readed = 0;

  readed += ms.ReadStringBlock(&mName);
  readed += ms.ReadStringBlock(&m_LocalFilename);
  readed += ms.ReadStringBlock(&mpEcmInfo);

  while (readed < blockSize)
  {
    DWORD tag = ms.ReadDword();
    readed += sizeof(DWORD);
    if (tag == KMS_METADATA_GROUP)
    {
      readed += LoadMetaData(ms);
    }
    else if (tag == KMS_ANNOTATE_GROUP)
    {
      m_Annotates.Load(ms);
    }
  }

#ifdef _DEBUG
  if (readed != blockSize)
  {
    TCHAR msg[256];
    StringCchPrintf(msg, 256, _T("Error, blockSize mismatch(%d, %d)"), readed, blockSize);
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_DEBUG);
  }
#endif
  return readed + sizeof(DWORD);
}

HRESULT KImagePage::LoadAnnotates(KMemoryStream& ms, KVoidPtrArray* pShapeList, int flag)
{
  return ((m_Annotates.LoadFromBufferBody(ms, pShapeList) > 0) ? S_OK : S_FALSE);
}

DWORD KImagePage::StoreDocument(KMemoryStream& ms, BOOL selectedOnly)
{
  ms.WriteDword(KMS_IMAGE_ITEM);
  ms.WriteDword(0);
  int startPos = ms.GetPosition();
  DWORD blockSize = 0;

  blockSize += ms.WriteStringBlock(mName);
  blockSize += ms.WriteStringBlock(m_LocalFilename);
  blockSize += ms.WriteStringBlock(mpEcmInfo);

  blockSize += SaveMetaData(ms);

  if (m_Annotates.GetCount() > 0)
  {
    blockSize += m_Annotates.Store(ms);
  }


  ms.UpdateBlockSize(startPos);
  return blockSize + sizeof(DWORD) * 2;
}

#endif // USE_XML_STORAGE

int KImagePage::ImageAutoRename(LPCTSTR pathName, int& index, int& base)
{
  TCHAR defPageName[128];
  LPTSTR currentFileExt = NULL;
  LPTSTR currentFilename = NULL;

  if (m_LocalFilename != NULL)
  {
    currentFileExt = AllocString(GetFileExtension(m_LocalFilename));
    currentFilename = AllocPathName(pathName, m_LocalFilename);
  }
  else
  {
    currentFileExt = AllocString(GetDefaultImageFileExt());
  }

  ReadyDefaultName(pathName, defPageName, currentFileExt, index, base);
  SetName(defPageName);

  // file is exist and it is on working-folder
  if ((currentFilename != NULL) && IsFileExist(currentFilename) &&
    IsChildFolderOrFile(pathName, currentFilename, FALSE))
  {
    LPTSTR newFilename = AllocPathName(pathName, defPageName);
    if (lstrcmpi(currentFilename, newFilename) != 0)
      KMoveFile(currentFilename, newFilename);
    delete[] m_LocalFilename;
    m_LocalFilename = AllocString(defPageName);
  }

  if (currentFilename != NULL)
    delete[] currentFilename;
  if (currentFileExt != NULL)
    delete[] currentFileExt;

  index++;
  return 1;
}

void KImagePage::ClearTempFiles(LPCTSTR workingPath)
{
  if (m_TempFilename != NULL)
  {
    LPTSTR fullName = AllocPathName(workingPath, m_TempFilename);
    if (IsFileExist(fullName))
      KDeleteFile(fullName);
    delete[] fullName;
    delete[] m_TempFilename;
    m_TempFilename = NULL;
  }
}
