/**
* @file KEcmFolderItem.h
* @date 2019.01
* @author kylee@k2mobile.co.kr
* @brief ECM Folder object class header file
*/
#pragma once

#include "stdafx.h"
#include "../Core/KConstant.h"
#include "../Platform/Utils.h"
#include "KEcmBaseItem.h"
#include "KEcmFileItem.h"

/**
* @class EcmFolderItem
* @brief ECM folder item class definition
*/
class EcmFolderItem : public EcmBaseItem
{
public:
  EcmFolderItem(EcmBaseItem* parent, LPCTSTR name, LPCTSTR folder_oid, LPCTSTR folder_index);
  virtual ~EcmFolderItem();

  virtual BOOL IsFolder() { return TRUE; }

  virtual int GetFolderCount();
  virtual int GetFileCount();

  virtual int StoreToBuffer(KMemoryStream& mf);

  void ClearChilds();
  void AddChild(EcmBaseItem* item);

  void SetFolderInfo(SYSTEMTIME& systime, int permissions);
  virtual void SetEcmInfo(LPCTSTR e);

  int GetPathName(LPTSTR buff, int buffSize);

  EcmBaseItem* FindFolderItemByOID(LPCTSTR oid);
  EcmBaseItem* FindFileItemByOID(LPCTSTR oid);
  EcmBaseItem* FindFolderItemByName(LPCTSTR pathName);
  EcmBaseItem* FindItemByName(LPCTSTR name);

  virtual void SetItemLoadFlag(int folder_flag, int file_flag);

  int GetLoadNeededFolder(int needed_flag);
  int GetLoadNeededFile(int needed_flag);

  EcmBaseItem* GetNextItem(int flag);

  void Dump(int d);


  TCHAR folderOID[MAX_OID];
  LPTSTR folderFullPathIndex;

  KVoidPtrArray mChildList;

};

