#include "stdafx.h"

#include "KConstant.h"
#include "KEcmFileItem.h"
#include "KEcmFolderItem.h"

#include "../Platform/Platform.h"
#include "../Core/KSettings.h"

#include <strsafe.h>
#include <Shlwapi.h>

EcmFileItem::EcmFileItem(EcmBaseItem* parent, LPCTSTR name, LPCTSTR doc_oid, LPCTSTR file_oid, LPCTSTR storage_oid)
  :EcmBaseItem(parent, name)
{
  parent = NULL;
  pMetadata = NULL;

  if (doc_oid != NULL)
    StringCchCopy(docOID, MAX_OID, doc_oid);
  else
    docOID[0] = '\0';
  if (file_oid != NULL)
    StringCchCopy(fileOID, MAX_OID, file_oid);
  else
    fileOID[0] = '\0';
  if (storage_oid != NULL)
    StringCchCopy(storageOID, MAX_STGOID, storage_oid);
  else
    storageOID[0] = '\0';
  /*
  if (docType_oid != NULL)
    StringCchCopy(extDocTypeOID, MAX_OID, docType_oid);
  else
    extDocTypeOID[0] = '\0';
    */
  }

EcmFileItem::~EcmFileItem()
{

}

void EcmFileItem::SetFileInfo(__int64 fileSize, SYSTEMTIME& systime, int permissions)
{
  mPermission = permissions;
}

// docOID + '|' + fileOID + '|' + storageOID + '|' + permission
void EcmFileItem::SetEcmInfo(LPCTSTR s)
{
  LPCTSTR e = StrChr(s, '|');
  if (e != NULL)
  {
    int len = e - s;
    if (len >= MAX_OID)
      len = MAX_OID - 1;
    StringCchCopyN(docOID, MAX_OID, e, len);
    s = e + 1;

    e = StrChr(s, '|');
    if (e != NULL)
    {
      len = e - s;
      if (len >= MAX_OID)
        len = MAX_OID - 1;
      StringCchCopyN(fileOID, MAX_OID, e, s - e);
      s = e + 1;

      e = StrChr(s, '|');
      if (e != NULL)
      {
        len = e - s;
        if (len >= MAX_STGOID)
          len = MAX_STGOID - 1;
        StringCchCopyN(storageOID, MAX_STGOID, e, s - e);
        s = e + 1;

        // permission
        int perm = 0;
        if (swscanf_s(s, _T("%x"), &perm) > 0)
          mPermission = perm;
      }
    }
  }
}

BOOL EcmFileItem::GetEcmInfo(LPTSTR pdocOID, LPTSTR pfileOID, LPTSTR pstorageOID)
{
  if ((pdocOID != NULL) && (lstrlen(docOID) > 0))
    StringCchCopy(pdocOID, MAX_OID, docOID);

  if ((pfileOID != NULL) && (lstrlen(fileOID) > 0))
    StringCchCopy(pfileOID, MAX_OID, fileOID);

  if ((pstorageOID != NULL) && (lstrlen(fileOID) > 0))
    StringCchCopy(pstorageOID, MAX_STGOID, storageOID);
  return TRUE;
}

LPTSTR EcmFileItem::MakeItemInfo()
{
  int buffSize = 0;
  LPTSTR buff = NULL;

  if ((mParent != NULL) && !mParent->IsRoot())
    buffSize = ((EcmFolderItem*)mParent)->GetPathName(NULL, 0) + 1;
  buffSize += lstrlen(mName) + 2;

  // fileOID, storageOID
  buffSize += lstrlen(fileOID) + lstrlen(storageOID) + 12;

  buff = new TCHAR[buffSize];
  buff[0] = '\0';

  if ((mParent != NULL) && !mParent->IsRoot())
  {
    ((EcmFolderItem*)mParent)->GetPathName(buff, buffSize);
    if (lstrlen(buff) > 0)
      StringCchCat(buff, buffSize, _T("\\"));
  }
  StringCchCat(buff, buffSize, mName);

  // fileOID, storageOID
  StringCchCat(buff, buffSize, _T("|"));
  StringCchCat(buff, buffSize, fileOID);
  StringCchCat(buff, buffSize, _T("|"));
  StringCchCat(buff, buffSize, storageOID);

  TCHAR str[12];
  StringCchPrintf(str, 12, _T("%x"), mPermission);
  StringCchCat(buff, buffSize, _T("|"));
  StringCchCat(buff, buffSize, str);

  return buff;
}

int EcmFileItem::StoreToBuffer(KMemoryStream& mf)
{
  int len = 0;
  int lastPos = mf.GetPosition();

  // store with relative pathname
  if ((mParent != NULL) && !mParent->IsRoot())
  {
    len = ((EcmFolderItem*)mParent)->GetPathName(NULL, 0) + 1;
    LPTSTR path = new TCHAR[len];
    path[0] = '\0';
    ((EcmFolderItem*)mParent)->GetPathName(path, len);

    mf.WriteString(path);
    delete[] path;

    mf.WriteString(efi_pathDelimeter);
  }

  mf.WriteString(mName);

  mf.WriteString(efi_itemDelimeter);
  mf.WriteString(efi_fileItemKey);

  mf.WriteString(efi_itemDelimeter);
  mf.WriteString(fileOID);
  mf.WriteString(efi_itemDelimeter);
  mf.WriteString(storageOID);

  TCHAR str[16];
  StringCchPrintf(str, 16, _T("%x"), mPermission);
  mf.WriteString(efi_itemDelimeter);
  mf.Write(str, (lstrlen(str) + 1) * sizeof(TCHAR)); // with null terminator

  return (mf.GetPosition() - lastPos);
}

void EcmFileItem::Dump(int d)
{
#ifdef _DEBUG
  TCHAR str[256];
  for (int i = 0; i < d; i++)
    str[i] = ' ';
  str[d] = '\0';

  if (mName != NULL)
    StringCchCat(str, 256, mName);
  StringCchCat(str, 256, _T(", docOID:"));
  StringCchCat(str, 256, docOID);
  StringCchCat(str, 256, _T(", fileOID:"));
  StringCchCat(str, 256, fileOID);
  StringCchCat(str, 256, _T(", storageOID:"));
  StringCchCat(str, 256, storageOID);
  StringCchCat(str, 256, _T(", permission:"));

  TCHAR pstr[16];
  StringCchPrintf(pstr, 16, _T("%x"), mPermission);
  StringCchCat(str, 256, pstr);

  StoreLogHistory(_T(__FUNCTION__), str, SYNC_MSG_LOG);
#endif
}
