/**
* @file KEcmFileItem.h
* @date 2019.01
* @author kylee@k2mobile.co.kr
* @brief ECM File object class header file
*/
#pragma once

#include "stdafx.h"
#include "KConstant.h"
#include "KMetadataInfo.h"
#include "KEcmBaseItem.h"
#include "../Platform/Utils.h"

/**
* @class EcmFileItem
* @brief ECM file item class definition
*/
class EcmFileItem : public EcmBaseItem
{
public:
  EcmFileItem(EcmBaseItem* parent, LPCTSTR name, LPCTSTR doc_oid, LPCTSTR file_oid, LPCTSTR storage_oid);
  virtual ~EcmFileItem();

  virtual int GetFolderCount() { return 0; }
  virtual int GetFileCount() { return 1; }

  virtual void SetItemLoadFlag(int folder_flag, int file_flag) { mLoadFlag |= file_flag; }

  virtual void Dump(int d);

  void SetFileInfo(__int64 fileSize, SYSTEMTIME& systime, int permissions);

  virtual void SetEcmInfo(LPCTSTR e);
  BOOL GetEcmInfo(LPTSTR docOID, LPTSTR fileOID, LPTSTR storageOID);

  LPTSTR MakeItemInfo();
  virtual int StoreToBuffer(KMemoryStream& mf);

  void SetMetadataInfo(KMetadataInfo* pInfo) { pMetadata = pInfo; }

  KMetadataInfo* pMetadata;
  //TCHAR extDocTypeOID[MAX_OID];

  // EcmFolderItem* parent;
  TCHAR docOID[MAX_OID];
  TCHAR fileOID[MAX_OID];
  TCHAR storageOID[MAX_STGOID];
};


