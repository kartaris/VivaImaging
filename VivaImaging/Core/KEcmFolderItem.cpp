#include "stdafx.h"

#include "KConstant.h"
#include "KEcmFolderItem.h"
#include "../Platform/Platform.h"
#include "../Core/KSettings.h"

#include <strsafe.h>
#include <Shlwapi.h>

EcmFolderItem::EcmFolderItem(EcmBaseItem* parent, LPCTSTR name, LPCTSTR folder_oid, LPCTSTR folder_index)
  :EcmBaseItem(parent, name)
{
  if (folder_oid != NULL)
    StringCchCopy(folderOID, MAX_OID, folder_oid);
  folderFullPathIndex = AllocString(folder_index);
}

EcmFolderItem::~EcmFolderItem()
{
  if (folderFullPathIndex != NULL)
  {
    delete[] folderFullPathIndex;
    folderFullPathIndex = NULL;
  }

  ClearChilds();
}

void EcmFolderItem::ClearChilds()
{
  KVoidPtrIterator it = mChildList.begin();
  while (it != mChildList.end())
  {
    EcmBaseItem* item = (EcmBaseItem*)*(it++);
    delete item;
  }
  mChildList.clear();
}

void EcmFolderItem::AddChild(EcmBaseItem* item)
{
  item->SetParent(this);
  mChildList.push_back(item);
}

int EcmFolderItem::GetFolderCount()
{
  int count = 1;
  KVoidPtrIterator it = mChildList.begin();
  while (it != mChildList.end())
  {
    EcmBaseItem* item = (EcmBaseItem*)*(it++);
    count += item->GetFolderCount();
  }
  return count;
}

int EcmFolderItem::GetFileCount()
{
  int count = 0;
  KVoidPtrIterator it = mChildList.begin();
  while (it != mChildList.end())
  {
    EcmBaseItem* item = (EcmBaseItem*)*(it++);
    count += item->GetFileCount();
  }
  return count;
}

void EcmFolderItem::SetFolderInfo(SYSTEMTIME& systime, int permissions)
{
  mPermission = permissions;
}


// folderOID + '|' + fullPathIndex + '|' + permission
void EcmFolderItem::SetEcmInfo(LPCTSTR s)
{
  LPCTSTR e = StrChr(s, '|');
  if (e != NULL)
  {
    int len = e - s;
    if (len >= MAX_OID)
      len = MAX_OID - 1;
    StringCchCopyN(folderOID, MAX_OID, e, len);
    s = e + 1;

    e = StrChr(s, '|');
    if (e != NULL)
    {
      len = e - s;

      if (folderFullPathIndex != NULL)
        delete[] folderFullPathIndex;

      folderFullPathIndex = AllocString(s, len);

      s = e + 1;
      // permission
      int perm = 0;
      if (swscanf_s(s, _T("%x"), &perm) > 0)
        mPermission = perm;
    }
  }
}

int EcmFolderItem::GetPathName(LPTSTR buff, int buffSize)
{
  int length = 0;
  if ((mParent != NULL) && !mParent->IsRoot())
  {
    length = ((EcmFolderItem*)mParent)->GetPathName(buff, buffSize) + 1;
    if ((buff != NULL) && (lstrlen(buff) > 0))
      StringCchCat(buff, buffSize, _T("\\"));
  }
  length += lstrlen(mName);
  if ((buff != NULL) && (mName != NULL))
    StringCchCat(buff, buffSize, mName);
  return length;
}

EcmBaseItem* EcmFolderItem::FindFolderItemByOID(LPCTSTR oid)
{
  if (lstrcmp(folderOID, oid) == 0)
    return this;

  KVoidPtrIterator it = mChildList.begin();
  while (it != mChildList.end())
  {
    EcmBaseItem* item = (EcmBaseItem*)*(it++);
    if (item->IsFolder())
    {
      if (lstrcmp(((EcmFolderItem *)item)->folderOID, oid) == 0)
        return item;

      item = ((EcmFolderItem *)item)->FindFolderItemByOID(oid);
      if (item != NULL)
        return item;
    }
  }
  return NULL;
}

EcmBaseItem* EcmFolderItem::FindFileItemByOID(LPCTSTR oid)
{
  KVoidPtrIterator it = mChildList.begin();
  while (it != mChildList.end())
  {
    EcmBaseItem* item = (EcmBaseItem*)*(it++);
    if (item->IsFolder() && (lstrcmp(((EcmFolderItem *)item)->folderOID, oid) == 0))
      return item;
  }
  return NULL;
}

EcmBaseItem* EcmFolderItem::FindFolderItemByName(LPCTSTR pathName)
{
  int len = lstrlen(pathName);
  LPCTSTR e = StrChr(pathName, '\\');
  if (e != NULL)
  {
    len = e - pathName;
    e++;
  }

  KVoidPtrIterator it = mChildList.begin();
  while (it != mChildList.end())
  {
    EcmBaseItem* item = (EcmBaseItem*)*(it++);
    if (item->IsFolder())
    {
      if (wcsncmp(item->mName, pathName, len) == 0)
      {
        if (e != NULL)
          return ((EcmFolderItem*)item)->FindFolderItemByName(e);
        return item;
      }
    }
  }
  return NULL;
}

EcmBaseItem* EcmFolderItem::FindItemByName(LPCTSTR name)
{
  KVoidPtrIterator it = mChildList.begin();
  while (it != mChildList.end())
  {
    EcmBaseItem* item = (EcmBaseItem*)*(it++);
    if (lstrcmp(item->mName, name) == 0)
    {
      return item;
    }
  }
  return NULL;
}

void EcmFolderItem::SetItemLoadFlag(int folder_flag, int file_flag)
{
  mLoadFlag |= folder_flag;
  KVoidPtrIterator it = mChildList.begin();
  while (it != mChildList.end())
  {
    EcmFolderItem* item = (EcmFolderItem*)*(it++);
    if (item->IsFolder())
      item->SetItemLoadFlag(folder_flag, file_flag);
    else
      item->mLoadFlag |= file_flag;
  }
}

int EcmFolderItem::GetLoadNeededFolder(int needed_flag)
{
  int count = 0;
  KVoidPtrIterator it = mChildList.begin();
  while (it != mChildList.end())
  {
    EcmFolderItem* item = (EcmFolderItem*)*(it++);
    if (item->IsFolder() && item->IsNeed(needed_flag))
      count++;
  }
  return count;
}

int EcmFolderItem::GetLoadNeededFile(int needed_flag)
{
  int count = 0;
  KVoidPtrIterator it = mChildList.begin();
  while (it != mChildList.end())
  {
    EcmFolderItem* item = (EcmFolderItem*)*(it++);
    if (!item->IsFolder() && item->IsNeed(needed_flag))
      count++;
  }
  return count;
}

EcmBaseItem* EcmFolderItem::GetNextItem(int needed_flag)
{
  KVoidPtrIterator it = mChildList.begin();
  while (it != mChildList.end())
  {
    EcmBaseItem* item = (EcmBaseItem*)*(it++);
    if (item->IsNeed(needed_flag))
      return item;

    if (item->IsFolder())
    {
      item = ((EcmFolderItem*)item)->GetNextItem(needed_flag);
      if (item != NULL)
        return item;
    }
  }
  return NULL;
}

int EcmFolderItem::StoreToBuffer(KMemoryStream& mf)
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
  mf.WriteString(efi_folderItemKey);

  mf.WriteString(efi_itemDelimeter);
  mf.WriteString(folderOID);
  mf.WriteString(efi_itemDelimeter);
  mf.WriteString(folderFullPathIndex);

  TCHAR str[16];
  StringCchPrintf(str, 16, _T("%x"), mPermission);
  mf.WriteString(efi_itemDelimeter);
  mf.Write(str, (lstrlen(str) + 1) * sizeof(TCHAR)); // with null terminator

  KVoidPtrIterator it = mChildList.begin();
  while (it != mChildList.end())
  {
    EcmBaseItem* item = (EcmBaseItem*)*(it++);
    item->StoreToBuffer(mf);
  }
  return (mf.GetPosition() - lastPos);
}

void EcmFolderItem::Dump(int d)
{
#ifdef _DEBUG
  TCHAR str[256];
  for (int i = 0; i < d; i++)
    str[i] = ' ';
  str[d] = '\0';

  if (mName != NULL)
    StringCchCat(str, 256, mName);
  StringCchCat(str, 256, _T(", folderOID:"));
  StringCchCat(str, 256, folderOID);
  StringCchCat(str, 256, _T(", permission:"));

  TCHAR pstr[16];
  StringCchPrintf(pstr, 16, _T("%x"), mPermission);
  StringCchCat(str, 256, pstr);

  StoreLogHistory(_T(__FUNCTION__), str, SYNC_MSG_LOG);

  d++;
  KVoidPtrIterator it = mChildList.begin();
  while (it != mChildList.end())
  {
    EcmBaseItem* item = (EcmBaseItem*)*(it++);
    item->Dump(d);
  }
#endif
}
