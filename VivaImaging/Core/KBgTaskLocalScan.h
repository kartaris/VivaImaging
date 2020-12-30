/**
* @file KBgTaskLocalScan.h
* @date 2019.01
* @author kylee@k2mobile.co.kr
* @brief Local folder scanning background task class header file
*/
#pragma once

#include "stdafx.h"
#include "KBgTask.h"
#include "../Platform/Utils.h"

/**
* @class KBgTaskLocalScan
* @brief Local Folder/File scan class definition
*/
class KBgTaskLocalScan : public KBgTask
{
public:
  KBgTaskLocalScan(CWnd* owner, BgTaskType task, LPCTSTR arg);
  virtual ~KBgTaskLocalScan(void);

  KVoidPtrArray mFileList;

  virtual void Do();

private:
  int scanFolder(KVoidPtrArray& childs, LPCTSTR path, int depthLimit);


};

