#include "stdafx.h"
#include "KImageBase.h"
#include "Shape\KShapeDef.h"

#include "..\Platform\Platform.h"
#include "..\Core\HttpDownloader\KMemory.h"
#include "..\Core\KSettings.h"
#include "..\Core\KConstant.h"

#include <strsafe.h>

UINT KImageBase::gVivaCustomClipboardFormat = 0;
UINT KImageBase::mGlobImageBaseID = 1;

KImageBase::KImageBase(KImageBase* p)
{
  mID = mGlobImageBaseID++;
  mFlags = 0;
  mbModified = PageModifiedFlag::NO_MODIFIED;
  mbModifiedCache = PageModifiedFlag::NO_MODIFIED;
  mParent = p;
  mName = NULL;
  mpParentKeyName = NULL;
  mpEcmInfo = NULL;
  mpFolderNameList = NULL;
  mPermission = SFP_READ | SFP_WRITE | SFP_DELETE;
}

KImageBase::~KImageBase()
{
  if (mName != NULL)
  {
    delete[] mName;
    mName = NULL;
  }
  if (mpParentKeyName != NULL)
  {
    delete[] mpParentKeyName;
    mpParentKeyName = NULL;
  }
  if (mpEcmInfo != NULL)
  {
    delete[] mpEcmInfo;
    mpEcmInfo = NULL;
  }
  if (mpFolderNameList != NULL)
  {
    delete[] mpFolderNameList;
    mpFolderNameList = NULL;
  }
  ClearMetaData(FALSE);
}

void KImageBase::SetName(LPCTSTR name)
{
  if (mName != NULL)
    delete[] mName;

  if (name != NULL)
    mName = AllocString(name);
  else
    mName = NULL;
}

void KImageBase::SetParentKeyName(LPCTSTR str)
{
  if (mpParentKeyName != NULL)
  {
    delete[] mpParentKeyName;
    mpParentKeyName = NULL;
  }
  mpParentKeyName = AllocString(str);
}

// docOID + '|' + fileOID + '|' + storageOID + '|' + permission
// or fileOID + '|' + fullPathIndex + '|' + permission
void KImageBase::SetEcmInfo(LPCTSTR s)
{
  if (mpEcmInfo != NULL)
    delete[] mpEcmInfo;

  LPCTSTR e = StrRChr(s, NULL, '|');
  if (e != NULL)
  {
    mpEcmInfo = AllocString(s, e - s);
    e++;
    int perm = 0;
    if (swscanf_s(e, _T("%x"), &perm) > 0)
      mPermission = perm;

  }
  else
  {
    TCHAR msg[256] = _T("Bad ECM info : no permission found : ");
    if (e != NULL)
      StringCchCat(msg, 256, s);
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG | SYNC_MSG_DEBUG);

    mpEcmInfo = AllocString(s);
  }
}

void KImageBase::SetFolderNameList(LPTSTR nameList)
{
  if (mpFolderNameList != NULL)
    delete[] mpFolderNameList;

  mpFolderNameList = AllocString(nameList);
}

int KImageBase::GetDepth()
{
  int depth = 0;
  if (mParent != NULL)
    depth = mParent->GetDepth() + 1;
  return depth;
}

void KImageBase::SetModified(BOOL bRecurr, PageModifiedFlag modified)
{
  if (modified == PageModifiedFlag::NO_MODIFIED)
    mbModified = PageModifiedFlag::NO_MODIFIED;
  else
    mbModified = (PageModifiedFlag)(mbModified | modified);

#ifdef _DEBUG
  TCHAR msg[32];
  StringCchPrintf(msg, 32, _T("page_id=%d, modified=%d"), mID, mbModified);
  StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
#endif

  mbModifiedCache = (PageModifiedFlag)((int)mbModifiedCache | (int)mbModified);
}

void KImageBase::SetModifiedCache(BOOL bRecurr, PageModifiedFlag modified)
{
  mbModifiedCache = modified;
}

LPTSTR KImageBase::GetImageFolderPath()
{
  KImageBase* p = GetParent();
  if (p != NULL)
  {
    if (p->mParent != NULL) // root item
    {
      LPTSTR path = p->GetImageFolderPath();
      return path;
    }
  }
  return NULL;
}


// 리소스 스트링에서 IDC_METAKEY1 와 매치되어야 함.

// ImageWidth : /ifd/{ushort=256}, VT_UI4
// ImageHeight : /ifd/{ushort=257}, VT_UI4
// BitsPerSample : /ifd/{ushort=258}, VT_UI2
// ImageDescription : /ifd/{ushort=270}, VT_LPSTR
// Make : /ifd/{ushort=271}, manufacture, VT_LPSTR
// Model : /ifd/{ushort=272}, VT_LPSTR

// DocumentName : /ifd/{ushort=269}, VT_LPSTR
// PageName : /ifd/{ushort=285}, VT_LPSTR

// Softare : /ifd/{ushort=305}, VT_LPSTR
// DateTime : /ifd/{ushort=306}, VT_LPSTR
// Artist : /ifd/{ushort=315}, VT_LPSTR
// HostComputer : /ifd/{ushort=316}, VT_LPSTR

// Copyright : /ifd/{ushort=33432},		VT_LPSTR
// DateTimeDigitized : /ifd/{ushort = 36868}, 	VT_LPSTR
// DateTimeOriginal : /ifd/{ushort=36867}, VT_LPSTR
// UserComment : /ifd/{ushort=37510}  LPWSTR
// FileSource : /ifd/{ushort=41728}, VT_BLOB

// Artist, FileSource, DateTime, Model, HostComputer // |LastModifier|LastModifierOID|LastModifiedDate 삭제됨
//LPCTSTR gDefaultMetaKeysPredef = _T("/ifd/{ushort=315}|/ifd/{ushort=41728}|/ifd/{ushort=306}|/ifd/{ushort=272}|/ifd/{ushort=316}|/ifd/{ushort=305}");
LPCTSTR gDefaultMetaKeysPredefIfd = _T("/{ushort=315}|/{ushort=306}|/{ushort=272}|/{ushort=316}|/{ushort=305}");
LPCTSTR gDefaultMetaKeysPredefExif = _T("/{ushort=41728}");

// Artist, FileSource, DateTime, Model, HostComputer
//static LPCTSTR pMetaKeysOnCreate = _T("/ifd/{ushort=315}|/ifd/{ushort=41728}|/ifd/{ushort=306}|/ifd/{ushort=272}|/ifd/{ushort=316}"); // 새 이미지 삽입시
static LPCTSTR pMetaKeysOnCreateIfd = _T("/{ushort=315}|/{ushort=306}|/{ushort=272}|/{ushort=316}"); // 새 이미지 삽입시
static LPCTSTR pMetaKeysOnCreateExif = _T("/{ushort=41728}"); // 새 이미지 삽입시

static LPCTSTR pMetaKeysOnSave = _T("LastModifier|LastModifierOID|LastModifiedDate"); // 문서 저장시
static LPCTSTR gDefaultMetaKeysCustom = _T("회원이름|회원전화번호|주소|처리지점|기타|비고|결과");

// ImageWidth, ImageHeight, BitsperSample, ImageDescription, PageName, Software
// 제거: UserComment("|/ifd/{ushort=37510}"
//static LPCTSTR gMetaKeysForImage = _T("/ifd/{ushort=256}|/ifd/{ushort=257}|/ifd/{ushort=258}|/ifd/{ushort=270}|/ifd/{ushort=285}|/ifd/{ushort=305}");
static LPCTSTR gMetaKeysForImage = _T("/{ushort=256}|/{ushort=257}|/{ushort=258}|/{ushort=270}|/{ushort=285}|/{ushort=305}");

LPCTSTR gTiffTagImageWidth = _T("/ifd/{ushort=256}");
LPCTSTR gTiffTagImageHeight = _T("/ifd/{ushort=257}");
LPCTSTR gTiffTagImageBitCount = _T("/ifd/{ushort=258}");
// LPCTSTR gMetaKeyUserComment = _T("/ifd/{ushort=37510}"); // 사용자 메타데이터
// LPCTSTR gTiffTagDocTypeInfo = _T("/ifd/{ushort=37512}");
// LPCTSTR gTiffTagMetadataInfo = _T("/ifd/{ushort=37514}");

LPCTSTR TagIFDPageName = _T("/ifd/{ushort=285}");
LPCTSTR TagIFDSoftware = _T("/ifd/{ushort=305}");

LPCTSTR TagIFDDateTime = _T("/ifd/{ushort=306}");
LPCTSTR TagIFDArtist = _T("/ifd/{ushort=315}");
LPCTSTR TagIFDHostComputer = _T("/ifd/{ushort=316}");

LPCTSTR wangAnnotateKey = _T("/ifd/{ushort=32932");
LPCTSTR TagIFDFileSource = _T("/ifd/{ushort=41728}");

LPCTSTR IFDTagFolderPathInfo = _T("/ifd/{ushort=269}"); // DocumentName
LPCTSTR TagFolderListInfo = _T("/ifd/{ushort=32940}"); // PrevWidowFolder

extern int GetCurrentDateTime(LPTSTR str, int buffLength);
extern LPTSTR GetCurrentUserID();
extern LPTSTR GetCurrentUserOID();

void KImageBase::ClearMetaData(BOOL clearEditOnly)
{
  // ClearKeyNameValueArray(mMetaDatas);

  mDocExtAttributes.Clear(clearEditOnly);

}

static LPCTSTR defaultImageFileExt[] =
{
  _T(""),
  _T("JPG"),
  _T("JPEG"),
  _T("PNG"),
  _T("TIF"),
  _T("TIFF"),
  _T("TIF"),
  _T("TIFF")
};

// static LPCTSTR defaultDocFileExt = _T("XML");

LPCTSTR GetImageFileExt(ImagingSaveFormat saveFormat)
{
  if ((FORMAT_ORIGIN <= saveFormat) && (saveFormat <= TIFF_MULTI_PAGE))
    return defaultImageFileExt[saveFormat-1];
  return defaultImageFileExt[0];
}

LPCTSTR GetDocumentFileExt(ImagingSaveFormat saveFormat)
{
  if (saveFormat == TIF_MULTI_PAGE)
    return defaultImageFileExt[6];
  else if (saveFormat == TIFF_MULTI_PAGE)
    return defaultImageFileExt[7];
  /*
  else
    return defaultDocFileExt;
  */
  return NULL;
}

BOOL HasValidFileExtension(LPCTSTR pathName, BOOL withXml)
{
  LPCTSTR e = StrRChr(pathName, NULL, '.');
  if (e != NULL)
  {
    e++;

    for (int i = 1; i < 6; i++)
    {
      if (lstrcmpi(defaultImageFileExt[i], e) == 0)
        return TRUE;
    }

    /*
    if (withXml &&
      (lstrcmpi(defaultDocFileExt, e) == 0))
      return TRUE;
    */
  }
  return FALSE;
}

LPCTSTR GetDefaultImageFileExt()
{
  return GetImageFileExt(gSaveFormat);
}

ImageEncodingFormat KImageBase::GetEncodingFilename(LPCTSTR filename)
{
  ImageEncodingFormat encode = ImageEncodingFormat::Bmp;

  LPCTSTR ext = GetFileExtension(filename);
  if (ext != NULL)
  {
    if (CompareStringICNC(ext, _T("png")) == 0)
    {
      encode = ImageEncodingFormat::Png;
    }
    else if ((CompareStringICNC(ext, _T("jpg")) == 0) || (CompareStringICNC(ext, _T("jpeg")) == 0))
    {
      encode = ImageEncodingFormat::Jpeg;
    }
    else if ((CompareStringICNC(ext, _T("tif")) == 0) || (CompareStringICNC(ext, _T("tiff")) == 0))
    {
      encode = ImageEncodingFormat::Tiff;
    }
  }
  return encode;
}

void KImageBase::SetMetaData(LPCTSTR key, LPCTSTR str, EcmExtDocTypeColumnType type, MetaDataCategory column, LPCTSTR name, BOOL byEdit)
{
  TCHAR msg[256];
  StringCchPrintf(msg, 256, _T("%s=%s"), key, str);
  StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG | SYNC_MSG_DEBUG);

    if (mDocExtAttributes.Set(key, str, name, type, column, TRUE) && byEdit)
      SetModified(FALSE, PageModifiedFlag::META_MODIFIED);
}

void KImageBase::SetMetaData(LPCTSTR key, LPBYTE buff, int len, WORD valueType, MetaDataCategory column)
{
  TCHAR msg[256];
  StringCchPrintf(msg, 256, _T("%s=%d-byte"), key, len);
  StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG | SYNC_MSG_DEBUG);

  mDocExtAttributes.Set(key, buff, len, valueType, NULL, EcmExtDocTypeColumnType::Binary, column, TRUE);
}

BOOL KImageBase::SetMetaDataValue(LPCTSTR key, LPCTSTR str)
{
  return mDocExtAttributes.Set(key, str, NULL, EcmExtDocTypeColumnType::None, MetaDataCategory::None, FALSE);
}

MetaKeyNameValue* KImageBase::FindMetaData(LPCTSTR key)
{
  return mDocExtAttributes.Find(key);
}

BOOL KImageBase::RemoveMetaData(LPCTSTR key)
{
  return mDocExtAttributes.Remove(key);
}

LPCTSTR KImageBase::GetMetaData(LPCTSTR key)
{
#ifdef _DEBUG
  if (CompareStringNC(key, _T("Modified")) == 0)
  {
    static LPCTSTR booleanStr[] = { _T("False"), _T("True") };
    return booleanStr[((int)mbModified ? 1 : 0)];
  }
#endif

  if (CompareStringNC(key, TagIFDPageName) == 0) // PageName
  {
    return GetName();
  }
  else if (CompareStringNC(key, TagIFDSoftware) == 0) // Softare
  {
    return _T("DestinyImaging v1.0");
  }

  PMetaKeyNameValue p = FindMetaData(key);
  return ((p != NULL) ? p->value : NULL);
}

LPBYTE KImageBase::GetMetaDataBinary(LPCTSTR key, int& len, MetaDataCategory& category, WORD& vtype)
{
  return NULL;
}

void KImageBase::GetMetaDataSet(KVoidPtrArray& list, KEcmDocTypeInfo* info, int flag)
{
  if (flag & META_FOR_STORE)
  {
    appendMetaDatas(list, flag);

    // property tab에는 나타나지 않지만 파일 저장시에 속성으로 저장되는 tag
    appendMetaDataSet(list, gMetaKeysForImage, MetaDataCategory::Ifd, flag);
  }
  else
  {
    appendMetaDataSet(list, gDefaultMetaKeysPredefIfd, MetaDataCategory::Ifd, flag);
    appendMetaDataSet(list, gDefaultMetaKeysPredefExif, MetaDataCategory::Exif, flag);
  }

#ifdef _DEBUG
  if (!(flag & META_FOR_STORE))
  {
    static LPCTSTR gDebugMetaKeysCustom = _T("Modified");
    appendMetaDataSet(list, gDebugMetaKeysCustom, MetaDataCategory::None, flag);
  }
#endif

}

void KImageBase::GetExtDocTypeInfoSet(KVoidPtrArray& list, int flag, KEcmDocTypeInfo& columns)
{
  appendExtDocTypeInfo(list, columns, flag);
}

void KImageBase::InitMetaDatas(BOOL onStart)
{
  if (onStart)
  {
    InitMetaDataSet(pMetaKeysOnCreateIfd, TRUE, MetaDataCategory::Ifd);
    InitMetaDataSet(pMetaKeysOnCreateExif, TRUE, MetaDataCategory::Exif);
  }
  else
  {
    InitMetaDataSet(pMetaKeysOnSave, FALSE, MetaDataCategory::None);
  }
}

void KImageBase::InitMetaDataSet(LPCTSTR pKeys, BOOL skipIfExist, MetaDataCategory category)
{
  while ((pKeys != NULL) && (*pKeys != '\0'))
  {
    LPCTSTR e = StrChr(pKeys, DA_VALUE_SEPARATOR);
    LPTSTR key;

    if (e != NULL)
      key = AllocString(pKeys, e - pKeys);
    else
      key = AllocString(pKeys);

    pKeys = (e != NULL) ? (e + 1) : NULL;

    // 시작시에는 존재하는 태그(파일에서 읽은)는 스킵
    if (skipIfExist && (FindMetaData(key) != NULL))
    {
      delete[] key;
      continue;
    }

    // if exist clear it
    RemoveMetaData(key);

    LPTSTR value = MakeDefaultMetaDataValue(key);
    if (value != NULL)
    {
      InsertMetaData(key, value, NULL, EcmExtDocTypeColumnType::None, category);
      delete[] value;
    }
    delete[] key;
  }
#ifdef FIXED_CUSTOM_META_TAGS
#endif
}

void KImageBase::InsertMetaData(LPTSTR key, LPTSTR value, LPTSTR name,
  EcmExtDocTypeColumnType type, MetaDataCategory category)
{
  mDocExtAttributes.Set(key, value, name, type, category, TRUE);
}

HANDLE KImageBase::BackupMetaKeyValue(LPCTSTR key)
{
  PMetaKeyNameValue p = mDocExtAttributes.Find(key);
  return StoreMetaKeyValue(GetID(), key, (p != NULL) ? p->value : NULL);
}

HANDLE KImageBase::StoreMetaKeyValue(UINT pid, LPCTSTR key, LPCTSTR str)
{
  int length = sizeof(UINT);
  length += lstrlen(key) + 1;
  length += lstrlen(str) + 1;

  HANDLE h = KMemoryAlloc(length * sizeof(TCHAR));
  if (h != NULL)
  {
    LPTSTR buff = (LPTSTR)KMemoryLock(h);
    *((UINT*)buff) = pid;
    buff += sizeof(UINT) / sizeof(TCHAR);

    if (key != NULL)
      StringCchCopy(buff, length, key);
    else
      buff[0] = '\0';
    int slen = lstrlen(buff);
    buff[slen++] = DA_VALUE_SEPARATOR;

    if (str != NULL)
      StringCchCopy(&buff[slen], length - slen, str);
    else
      buff[slen] = '\0';

    KMemoryUnlock(h);
  }
  return h;
}

int KImageBase::RestoreDocExtAttributes(LPTSTR key)
{
  int count = 0;
  // 포함된 항목만 restore한다.
  // ClearMetaData(TRUE);

  while ((key != NULL) && (*key != '\0'))
  {
    LPTSTR ikey;
    LPTSTR value = NULL;
    LPTSTR typestr = NULL;
    int type = 0;
    LPTSTR e = StrChr(key, DA_PARAMETER_SEPARATOR);
    if (e != NULL)
      ikey = AllocString(key, e - key);
    else
      ikey = AllocString(key);

    value = StrChr(ikey, DA_VALUE_SEPARATOR);
    if (value != NULL)
    {
      *(value++) = '\0';
    }
    SetMetaDataValue(ikey, value);

    delete[] ikey;

    if (e != NULL)
      key = e + 1;
    else
      key = NULL;
    count++;
  }
  return count;
}

/*
HANDLE KImageBase::BackupDocExtAttributes()
{
  int length = sizeof(UINT); // page id number
  KMetadataVectorIterator it = mDocExtAttributes.mVector.begin();
  while (it != mDocExtAttributes.mVector.end())
  {
    PMetaKeyNameValue p = (PMetaKeyNameValue)*(it++);

    length += lstrlen(p->key) + 1; // DA_VALUE_SEPARATOR
    length += lstrlen(p->name) + 1; // DA_VALUE_SEPARATOR
    length += lstrlen(p->value) + 1; // DA_VALUE_SEPARATOR
    length += 1 + 1; // for type, DA_PARAMETER_SEPARATOR
  }
  length += 2;

  HANDLE h = KMemoryAlloc(length * sizeof(TCHAR));
  if (h != NULL)
  {
    LPTSTR buff = (LPTSTR)KMemoryLock(h);

    *((UINT*)buff) = GetID(); // page id
    buff += sizeof(UINT) / sizeof(TCHAR);

    it = mDocExtAttributes.mVector.begin();
    while (it != mDocExtAttributes.mVector.end())
    {
      PMetaKeyNameValue p = (PMetaKeyNameValue)*(it++);
      TCHAR str[4];

      if (p->key != NULL)
      {
        StringCchCopy(buff, length, p->key);
        int slen = lstrlen(p->key);
        buff += slen;
        length -= slen;
      }
      *(buff++) = DA_VALUE_SEPARATOR;

      if (p->name != NULL)
      {
        StringCchCopy(buff, length, p->name);
        int slen = lstrlen(p->name);
        buff += slen;
        length -= slen;
      }
      *(buff++) = DA_VALUE_SEPARATOR;

      if (p->value != NULL)
      {
        StringCchCopy(buff, length, p->value);
        int slen = lstrlen(p->value);
        buff += slen;
        length -= slen;
      }
      *(buff++) = DA_VALUE_SEPARATOR;

      StringCchPrintf(str, 4, _T("%d"), p->type);
      if (lstrlen(str) > 0)
      {
        StringCchCopy(buff, length, str);
        int slen = lstrlen(str);
        buff += slen;
        length -= slen;
      }
      *(buff++) = DA_PARAMETER_SEPARATOR;
    }
    KMemoryUnlock(h);
  }
  return h;
}

int KImageBase::RestoreDocExtAttributes(LPTSTR key)
{
  int count = 0;
  ClearMetaData(TRUE);

  while ((key != NULL) && (*key != '\0'))
  {
    LPTSTR ikey;
    LPTSTR value = NULL;
    LPTSTR typestr = NULL;
    int type = 0;
    LPTSTR e = StrChr(key, DA_PARAMETER_SEPARATOR);
    if (e != NULL)
      ikey = AllocString(key, e - key);
    else
      ikey = AllocString(key);

    LPTSTR name = StrChr(ikey, DA_VALUE_SEPARATOR);
    if (name != NULL)
    {
      *(name++) = '\0';
      value = StrChr(name, DA_VALUE_SEPARATOR);
      if (value != NULL)
      {
        *(value++) = '\0';
        typestr = StrChr(value, DA_VALUE_SEPARATOR);
        if (typestr != NULL)
        {
          *(typestr++) = '\0';

          swscanf_s(typestr, _T("%d"), &type);
          InsertMetaData(ikey, value, name, (EcmExtDocTypeColumnType)type);
        }
      }
    }
    delete[] ikey;

    if (e != NULL)
      key = e + 1;
    else
      key = NULL;
    count++;
  }
  return count;
}
*/

HANDLE KImageBase::BackupName()
{
  int length = sizeof(UINT); // page id number
  length += (lstrlen(mName) + 1) * sizeof(TCHAR);

  HANDLE h = KMemoryAlloc(length);
  if (h != NULL)
  {
    LPTSTR buff = (LPTSTR)KMemoryLock(h);

    *((UINT*)buff) = GetID(); // page id
    buff += sizeof(UINT) / sizeof(TCHAR);

    if (mName != NULL)
      StringCchCopy(buff, length, mName);
    else
      *buff = '\0';
    KMemoryUnlock(h);
  }
  return h;
}

HANDLE KImageBase::AllocStringHandle(LPCTSTR str)
{
  int length = 0;
  length += lstrlen(str) + 1;

  HANDLE h = KMemoryAlloc(length * sizeof(TCHAR));
  if (h != NULL)
  {
    LPTSTR buff = (LPTSTR)KMemoryLock(h);
    if (str != NULL)
      StringCchCopy(buff, length, str);
    else
      buff[0] = '\0';
    KMemoryUnlock(h);
  }
  return h;
}

#define EXIF_MATA_TAGS

void KImageBase::SetAcquiredDevice(LPCTSTR deviceName)
{
  // ECM 확장속성을 사용하는 경우에는 넣지 않는다.
#ifdef FIXED_CUSTOM_META_TAGS
  LPCTSTR key = _T("/ifd/{ushort=272}"); // _T("InputDeviceName");
  RemoveMetaData(key);
  InsertMetaData(AllocString(key), AllocString(deviceName));

  // acquired date
  key = _T("/ifd/{ushort=36868}");
  RemoveMetaData(key);

  TCHAR datetime[64];
  GetCurrentDateTime(datetime, 64);
  InsertMetaData(AllocString(key), AllocString(datetime));
#endif
}

LPTSTR KImageBase::MakeDefaultMetaDataValue(LPCTSTR key)
{
  // "Creator|CreatorOID|CreatedDate|LastModifier|LastModifierOID|LastModifiedDate|InputDeviceName|InputComputerName|InputComputerMacAddress"
  LPTSTR value = NULL;

#ifdef EXIF_MATA_TAGS
  // Artist, FileSource, DateTime, Model, HostComputer
  // static LPCTSTR pMetaKeysOnCreate = _T("/ifd/{ushort=315}|/ifd/{ushort=41728}|/ifd/{ushort=306}| /ifd/{ushort=272}|/ifd/{ushort=316}"); // 새 이미지 삽입시
  // ImageWidth, ImageHeight, BitsperSample, ImageDescription, Software, UserComment
  // static LPCTSTR gMetaKeysForImage = _T("/ifd/{ushort=256}| /ifd/{ushort=257}|/ifd/{ushort=258}|/ifd/{ushort=270}|/ifd/{ushort=305}|/ifd/{ushort=37510}");
  
  if (CompareStringNC(key, TagIFDArtist) == 0) // 315, Artist
    value = AllocString(GetCurrentUserID());
  else if (CompareStringNC(key, TagIFDFileSource) == 0) // 41728, FileSource
    value = AllocString(GetCurrentUserOID());
  else if (CompareStringNC(key, TagIFDDateTime) == 0) // 306, DatTime
  {
    TCHAR buff[32];
    GetCurrentDateTime(buff, 32);
    value = AllocString(buff);
  }
  else if (CompareStringNC(key, TagIFDHostComputer) == 0) // 316, HostComputer
  {
    TCHAR buff[256];
    DWORD size = 256;
    GetComputerNameEx(ComputerNameNetBIOS, buff, &size);
    StringCchCat(buff, 256, _T(", "));
    size = lstrlen(buff);
    if (size < 240)
    {
      IP_ADAPTER_INFO AdapterInfo[4];       // Allocate information for up to 16 NICs
      DWORD dwBufLen = sizeof(AdapterInfo);  // Save memory size of buffer

      DWORD dwStatus = GetAdaptersInfo(AdapterInfo, &dwBufLen);
      if (dwStatus == ERROR_SUCCESS)
      {
        for (UINT i = 0; i < AdapterInfo[0].AddressLength; i++)
          StringCchPrintf(&buff[size + i * 2], 3, _T("%.2X"), AdapterInfo[0].Address[i]);
      }
    }
    value = AllocString(buff);
  }

  if (CompareStringNC(key, _T("LastModifier")) == 0)
  {
    value = AllocString(GetCurrentUserID());
  }
  else if (CompareStringNC(key, _T("LastModifierOID")) == 0)
  {
    value = AllocString(GetCurrentUserOID());
  }
  else if (CompareStringNC(key, _T("LastModifiedDate")) == 0)
  {
    TCHAR buff[32];
    GetCurrentDateTime(buff, 32);
    value = AllocString(buff);
  }

#else
  if ((CompareStringNC(key, _T("Writer")) == 0) || (CompareStringNC(key, _T("LastModifier")) == 0))
    value = AllocString(GetCurrentUserID());
  else if ((CompareStringNC(key, _T("CreatorOID")) == 0) || (CompareStringNC(key, _T("LastModifierOID")) == 0))
    value = AllocString(GetCurrentUserOID());
  // else if (CompareStringNC(key, _T("Caption")) == 0)
  else if ((CompareStringNC(key, _T("CreatedDate")) == 0) || (CompareStringNC(key, _T("LastModifiedDate")) == 0))
  {
    TCHAR buff[32];
    GetCurrentDateTime(buff, 32);
    value = AllocString(buff);
  }
  else if (CompareStringNC(key, _T("InputComputerName")) == 0)
  {
    TCHAR buff[256];
    DWORD size = 256;
    GetComputerNameEx(ComputerNameNetBIOS, buff, &size);
    value = AllocString(buff);
  }
  else if (CompareStringNC(key, _T("InputComputerMacAddress")) == 0)
  {
    IP_ADAPTER_INFO AdapterInfo[4];       // Allocate information for up to 16 NICs
    DWORD dwBufLen = sizeof(AdapterInfo);  // Save memory size of buffer

    DWORD dwStatus = GetAdaptersInfo(AdapterInfo, &dwBufLen);
    if (dwStatus == ERROR_SUCCESS)
    {
      TCHAR buff[32] = _T("");
      for (UINT i = 0; i <  AdapterInfo[0].AddressLength; i++)
        StringCchPrintf(&buff[i * 2], 3, _T("%.2X"), AdapterInfo[0].Address[i]);
      value = AllocString(buff);
    }
  }
#endif

  return value;
}

void KImageBase::appendMetaData(KVoidPtrArray& list, LPTSTR key, LPCTSTR name,
  EcmExtDocTypeColumnType type, MetaDataCategory category, LPCTSTR str, int flag)
{
  if (str == NULL)
    str = GetMetaData(key);

  MetaKeyNameValue* n = new MetaKeyNameValue;
  memset(n, 0, sizeof(MetaKeyNameValue));
  n->key = key;
  n->name = (name != NULL) ? AllocString(name) : NULL;
  n->value = (str != NULL) ? AllocString(str) : NULL;
  n->type = type;
  n->category = category;
  list.push_back(n);
}

void KImageBase::appendMetaData(KVoidPtrArray& list, LPTSTR key, LPCTSTR name, LPBYTE buff, int len, MetaDataCategory category, WORD vtype, int flag)
{
  MetaKeyNameValue* n = new MetaKeyNameValue;
  memset(n, 0, sizeof(MetaKeyNameValue));
  n->key = key;
  n->name = (name != NULL) ? AllocString(name) : NULL;
  if (len > 0)
  {
    n->value = (LPTSTR)new BYTE[len];
    memcpy(n->value, buff, len);
  }
  n->len = len;
  n->type = EcmExtDocTypeColumnType::Binary;
  n->category = category;
  n->vtype = vtype;
  list.push_back(n);
}

void KImageBase::appendExtDocTypeInfo(KVoidPtrArray& list, KEcmDocTypeInfo& columns, int flag)
{
  KEcmExtDocTypeColumnConstIterator it = columns.mArray.begin();
  while (it != columns.mArray.end())
  {
    EcmExtDocTypeColumn* col = *(it++);

    LPCTSTR str = GetMetaData(col->key);

    if ((str != NULL) || !(flag & META_SKIP_EMPTY))
    {
      LPTSTR key = AllocString(col->key);
      appendMetaData(list, key, col->name, col->type, MetaDataCategory::ExtDocType, str, flag);
    }
  }
}

void KImageBase::appendMetaDatas(KVoidPtrArray& list, int flag)
{
  KMetadataVectorIterator it = mDocExtAttributes.mVector.begin();
  while (it != mDocExtAttributes.mVector.end())
  {
    PMetaKeyNameValue p = (PMetaKeyNameValue) * (it++);

    // 확장속성은 스킵
    if (KMetadataInfo::IsDocTypeMetadata(p))
      continue;

    PMetaKeyNameValue n = KMetadataInfo::CopyMetaKeyNameValue(p);
    if (n != NULL)
      list.push_back(n);
  }
}

void KImageBase::appendMetaDataSet(KVoidPtrArray& list, LPCTSTR pKeys, MetaDataCategory category, int flag)
{
  while ((pKeys != NULL) && (*pKeys != '\0'))
  {
    LPCTSTR e = StrChr(pKeys, DA_VALUE_SEPARATOR);
    LPTSTR key;

    if (e != NULL)
      key = AllocString(pKeys, e - pKeys);
    else
      key = AllocString(pKeys);

    LPCTSTR str = GetMetaData(key);
    if (str != NULL)
    {
      appendMetaData(list, key, NULL, EcmExtDocTypeColumnType::None, category, str, flag);
      key = NULL;
    }
    else
    {
      int len = 0;
      WORD vtype = 0;
      LPBYTE buff = GetMetaDataBinary(key, len, category, vtype);
      if (buff != NULL) 
      {
        appendMetaData(list, key, NULL, buff, len, category, vtype, flag);
        key = NULL;
      }
      else if (!(flag & META_SKIP_EMPTY))
      {
        appendMetaData(list, key, NULL, EcmExtDocTypeColumnType::None, category, str, flag);
        key = NULL;
      }
    }
    if (key != NULL)
      delete[] key;
    pKeys = (e != NULL) ? (e + 1) : NULL;
  }
}

LPTSTR KImageBase::getCustomMetaString(KEcmDocTypeInfo& columns, LPTSTR buff, int flag)
{
  KEcmExtDocTypeColumnConstIterator it = columns.mArray.begin();
  while (it != columns.mArray.end())
  {
    EcmExtDocTypeColumn* col = *(it++);

    LPCTSTR str = GetMetaData(col->key);
    if ((str != NULL) || !(flag & META_SKIP_EMPTY))
    {
      int len = lstrlen(buff) + lstrlen(col->key) + lstrlen(str) + 4;
      LPTSTR n = new TCHAR[len];
      if (buff != NULL)
      {
        StringCchCopy(n, len, buff);
        StringCchCat(n, len, _T("\n"));
      }
      else
      {
        n[0] = '\0';
      }

      StringCchCat(n, len, col->key);
      StringCchCat(n, len, _T("\b"));
      if (str != NULL)
        StringCchCat(n, len, str);
      if (buff != NULL)
        delete[] buff;
      buff = n;
    }
  }
  return buff;
}

LPTSTR KImageBase::getSeparatMetaString(LPCTSTR pKeys, LPTSTR buff, int flag)
{
  while ((pKeys != NULL) && (*pKeys != '\0'))
  {
    LPCTSTR e = StrChr(pKeys, DA_VALUE_SEPARATOR);
    LPTSTR key;

    if (e != NULL)
      key = AllocString(pKeys, e - pKeys);
    else
      key = AllocString(pKeys);

    LPCTSTR str = GetMetaData(key);
    if ((str != NULL) || !(flag & META_SKIP_EMPTY))
    {
      int len = lstrlen(buff) + lstrlen(key) + lstrlen(str) + 4;
      LPTSTR n = new TCHAR[len];
      if (buff != NULL)
      {
        StringCchCopy(n, len, buff);
        StringCchCat(n, len, _T("\n"));
      }
      else
      {
        n[0] = '\0';
      }

      StringCchCat(n, len, key);
      StringCchCat(n, len, _T("\b"));
      if (str != NULL)
        StringCchCat(n, len, str);
      if (buff != NULL)
        delete[] buff;
      buff = n;
    }

    if (key != NULL)
      delete[] key;

    pKeys = (e != NULL) ? (e + 1) : NULL;
  }
  return buff;
}

BOOL KImageBase::RefreshMetaData()
{
  // InitMetaDatas()으로 업데이트 됨.
#if 0
  // update LastModifier|LastModifierOID|LastModifiedDate
  SetMetaData(_T("LastModifier"), GetCurrentUserID());
  SetMetaData(_T("LastModifierOID"), GetCurrentUserOID());
  TCHAR buff[32];
  GetCurrentDateTime(buff, 32);
  SetMetaData(_T("LastModifiedDate"), buff);
#endif

  return TRUE;
}

#ifdef USE_XML_STORAGE
HRESULT KImageBase::ReadMetaAttributes(IXmlReader* pReader, MetaKeyNameValue* p)
{
  const WCHAR* pwszPrefix = NULL;
  const WCHAR* pwszLocalName = NULL;
  const WCHAR* pwszValue = NULL;

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
      /*
      if (cwchPrefix > 0)
        wprintf(L"Attr: %s:%s=\"%s\" \n", pwszPrefix, pwszLocalName, pwszValue);
      else
        wprintf(L"Attr: %s=\"%s\" \n", pwszLocalName, pwszValue);
      */
    }

    if (CompareStringNC(pwszLocalName, tagExtDocTypeColumnKey) == 0)
    {
      p->key = AllocString(pwszValue);
    }
    else if (CompareStringNC(pwszLocalName, tagExtDocTypeColumnName) == 0)
    {
      p->name = AllocString(pwszValue);
    }
    else if (CompareStringNC(pwszLocalName, tagExtDocTypeColumnType) == 0)
    {
      p->type = KEcmDocTypeInfo::TypeFromString(pwszValue);
    }

    hr = pReader->MoveToNextAttribute();
  }
  return hr;
}

 HRESULT KImageBase::ReadMetaData(CComPtr<IXmlReader> pReader, MetaKeyNameValue* p)
{
  HRESULT hr;
  if (FAILED(hr = ReadMetaAttributes(pReader, p))) {
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

  while (S_OK == (hr = pReader->Read(&nodeType)))
  {
    switch (nodeType)
    {
    case XmlNodeType_Element:
      if (FAILED(hr = pReader->GetPrefix(&pwszPrefix, &cwchPrefix)))
      {
        TCHAR msg[256];
        StringCchPrintf(msg, 256, _T("Error getting prefix on Element, error is %08.8lx"), hr);
        StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
        return hr;
      }
      if (FAILED(hr = pReader->GetLocalName(&pwszLocalName, NULL)))
      {
        TCHAR msg[256];
        StringCchPrintf(msg, 256, _T("Error getting local name on Element, error is %08.8lx"), hr);
        StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
        return hr;
      }
      /*
      if (cwchPrefix > 0)
      wprintf(L"Element: %s:%s\n", pwszPrefix, pwszLocalName);
      else
      wprintf(L"Element: %s\n", pwszLocalName);
      */

      if (FAILED(hr = pReader->MoveToElement()))
      {
        TCHAR msg[256];
        StringCchPrintf(msg, 256, _T("Error moving to the element that owns the current attribute node, error is %08.8lx"), hr);
        StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
        return hr;
      }
      break;

    case XmlNodeType_EndElement:
      if (FAILED(hr = pReader->GetPrefix(&pwszPrefix, &cwchPrefix)))
      {
        TCHAR msg[256];
        StringCchPrintf(msg, 256, _T("Error getting prefix on EndElement, error is %08.8lx"), hr);
        StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
        return hr;
      }
      if (FAILED(hr = pReader->GetLocalName(&pwszLocalName, NULL)))
      {
        TCHAR msg[256];
        StringCchPrintf(msg, 256, _T("Error getting local name on EndElement, error is %08.8lx"), hr);
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
      if (p->value != NULL)
        delete[] p->value;
      p->value = AllocString(pwszValue);
      //wprintf(L"Whitespace text: >%s<\n", pwszValue);
      break;

    case XmlNodeType_CDATA:
      if (FAILED(hr = pReader->GetValue(&pwszValue, NULL)))
      {
        TCHAR msg[256];
        StringCchPrintf(msg, 256, _T("Error getting value on CDATA, error is %08.8lx"), hr);
        StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
        return hr;
      }
      //wprintf(L"CDATA: %s\n", pwszValue);
      break;

    case XmlNodeType_ProcessingInstruction:
      if (FAILED(hr = pReader->GetLocalName(&pwszLocalName, NULL)))
      {
        TCHAR msg[256];
        StringCchPrintf(msg, 256, _T("Error getting name on ProcssingInstruction, error is %08.8lx"), hr);
        StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
        return hr;
      }
      if (FAILED(hr = pReader->GetValue(&pwszValue, NULL)))
      {
        TCHAR msg[256];
        StringCchPrintf(msg, 256, _T("Error getting value on ProcessingInstruction, error is %08.8lx"), hr);
        StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
        return hr;
      }
      //wprintf(L"Processing Instruction name:%S value:%S\n", pwszLocalName, pwszValue);
      break;

    case XmlNodeType_Comment:
      if (FAILED(hr = pReader->GetValue(&pwszValue, NULL)))
      {
        TCHAR msg[256];
        StringCchPrintf(msg, 256, _T("Error getting value on Comment, error is %08.8lx"), hr);
        StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
        return hr;
      }
      //wprintf(L"Comment: %s\n", pwszValue);
      break;
    } // end of switch
  } // end of while
  return S_OK;
}

HRESULT KImageBase::StoreMetaData(CComPtr<IXmlWriter> pWriter)
{
  KMetadataVectorIterator it = mDocExtAttributes.mVector.begin();
  while (it != mDocExtAttributes.mVector.end())
  {
    MetaKeyNameValue* p = (MetaKeyNameValue*)*it;

    if ((p->value != NULL) && (lstrlen(p->value) > 0))
    {
      HRESULT hr;

      if (FAILED(hr = pWriter->WriteStartElement(NULL, tagMetaData, NULL)))
      {
        TCHAR msg[256];
        StringCchPrintf(msg, 256, _T("Error, Method: WriteStartElement, error is %08.8lx"), hr);
        StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
        return hr;
      }

      if (FAILED(hr = pWriter->WriteAttributeString(NULL, tagExtDocTypeColumnKey, NULL,
        p->key)))
      {
        TCHAR msg[256];
        StringCchPrintf(msg, 256, _T("Error, Method: WriteAttributeString, error is %08.8lx"), hr);
        StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
        return hr;
      }

      if (FAILED(hr = pWriter->WriteAttributeString(NULL, tagExtDocTypeColumnName, NULL,
        p->name)))
      {
        TCHAR msg[256];
        StringCchPrintf(msg, 256, _T("Error, Method: WriteAttributeString, error is %08.8lx"), hr);
        StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
        return hr;
      }

      LPCTSTR typestr = KEcmDocTypeInfo::GetValueTypeName(p->type);

      if (FAILED(hr = pWriter->WriteAttributeString(NULL, tagExtDocTypeColumnType, NULL,
        typestr)))
      {
        TCHAR msg[256];
        StringCchPrintf(msg, 256, _T("Error, Method: WriteAttributeString, error is %08.8lx"), hr);
        StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
        return hr;
      }

      if (FAILED(hr = pWriter->WriteString(p->value)))
      {
        TCHAR msg[256];
        StringCchPrintf(msg, 256, _T("Error, Method: WriteString, error is %08.8lx"), hr);
        StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
        return hr;
      }
      if (FAILED(hr = pWriter->WriteEndElement()))
      {
        TCHAR msg[256];
        StringCchPrintf(msg, 256, _T("Error, Method: WriteEndElement, error is %08.8lx"), hr);
        StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
        return hr;
      }
    }
    ++it;
  }
  return S_OK;
}
#else

DWORD KImageBase::LoadMetaData(KMemoryStream& ms)
{
  return mDocExtAttributes.LoadFromBuffer(ms);

  /*
  DWORD blockSize = ms.ReadDword();
  DWORD readed = 0;

  while (blockSize > readed)
  {
    DWORD tag = ms.ReadDword();
    if (tag == KMS_NAMEVALUE_ITEM)
    {
      MetaKeyNameValue* p = new MetaKeyNameValue;
      DWORD size = LoadMetaValue(ms, p);
      if (size > 0)
      {
        mDocExtAttributes.mVector.push_back(p);
      }
      else
      {
        delete p;
        return 0;
      }
      readed += size + sizeof(DWORD);
    }
    else
    {
      TCHAR msg[256];
      StringCchPrintf(msg, 256, _T("Unknown tag 0X%x"), tag);
      StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
      return 0;
    }
  }
  return readed + sizeof(DWORD);
  */
}

DWORD KImageBase::SaveMetaData(KMemoryStream& ms)
{
  return mDocExtAttributes.StoreToBuffer(ms);
  /*
  ms.WriteDword(KMS_METADATA_GROUP);
  ms.WriteDword(0);
  int startPos = ms.GetPosition();
  DWORD blockSize = sizeof(DWORD) * 2;

  KMetadataVectorIterator it = mDocExtAttributes.mVector.begin();
  while (it != mDocExtAttributes.mVector.end())
  {
    MetaKeyNameValue* p = (MetaKeyNameValue*)*it;

    if ((p->value != NULL) && (lstrlen(p->value) > 0))
    {
      blockSize += StoreMetaValue(ms, p);
    }
  }

  DWORD bs = ms.UpdateBlockSize(startPos) + sizeof(DWORD) * 2;
#ifdef _DEBUG
  if (bs != blockSize)
  {
    TCHAR msg[256];
    StringCchPrintf(msg, 256, _T("Mismatch block size %d, %d"), bs, blockSize);
    OutputDebugString(msg);
  }
#endif
  return blockSize;
  */
}

#endif // USE_XML_STORAGE

void KImageBase::WriteImageName(KMemoryFile& mf)
{
  mf.WriteDword(GetID());
  LPCTSTR name = GetName();
  UINT length = lstrlen(name);
  mf.WriteDword(length);
  mf.Write(name, sizeof(TCHAR) * length);
}

int KImageBase::ReadImageName(KMemoryFile& mf)
{
  UINT pid = mf.ReadDword();
  UINT length = mf.ReadDword();

  if (length > 0)
  {
    TCHAR* namestr = new TCHAR[length + 1];
    mf.Read((void*)namestr, length * sizeof(TCHAR));
    namestr[length] = '\0';
    SetName(namestr);
    delete[] namestr;
  }
  else
  {
    SetName(NULL);
  }
  return 1;
}
