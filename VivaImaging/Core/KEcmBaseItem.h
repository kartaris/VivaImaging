/**
* @file KEcmBaseItem.h
* @date 2019.01
* @author kylee@k2mobile.co.kr
* @brief ECM object base class header file
*/
#pragma once

#include "stdafx.h"
#include "../Core/KConstant.h"
#include "../Core/KMemoryStream.h"

#define ITEM_NEED_UPLOAD 1
#define ITEM_NEED_DOWNLOAD 2

/**
* @class EcmBaseItem
* @brief ECMN base item class definition
*/
class EcmBaseItem
{
public:
  EcmBaseItem(EcmBaseItem* parent, LPCTSTR name);
  virtual ~EcmBaseItem();

  virtual BOOL IsFolder() { return FALSE; }
  inline BOOL IsRoot() { return (mParent == NULL); }

  virtual int GetFolderCount() { return 0; }
  virtual int GetFileCount() { return 0; }

  virtual int StoreToBuffer(KMemoryStream& mf) { return 0; }

  inline void SetParent(EcmBaseItem* parent) {
    mParent = parent; }

  void SetNeedUpload(BOOL upload);
  void SetNeedDownload(BOOL upload);
  virtual void SetItemLoadFlag(int folder_flag, int file_flag) { ; }

  BOOL IsNeed(int needed_flag);
  BOOL IsNeedUpload();
  BOOL IsNeedDownload();

  inline void SetPermission(int p) { mPermission = p; }
  inline UINT GetPermission() { return mPermission; }

  virtual EcmBaseItem* FindFolderItemByOID(LPCTSTR oid) { return NULL; }
  virtual EcmBaseItem* FindFileItemByOID(LPCTSTR oid) { return NULL; }
  virtual EcmBaseItem* FindFolderItemByName(LPCTSTR pathName) { return NULL; }
  virtual EcmBaseItem* FindItemByName(LPCTSTR name) { return NULL; }

  virtual void Dump(int d) { ; }

  HANDLE CreateListData();

  static TCHAR efi_pathDelimeter[];
  static TCHAR efi_itemDelimeter[];
  static TCHAR efi_folderItemKey[];
  static TCHAR efi_fileItemKey[];

  EcmBaseItem* mParent;
  LPTSTR mName;
  UINT mLoadFlag;
  UINT mPermission;
};
