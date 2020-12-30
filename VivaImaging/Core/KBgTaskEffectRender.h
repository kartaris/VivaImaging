/**
* @file KBgTaskEffectRender.h
* @date 2019.01
* @author kylee@k2mobile.co.kr
* @brief Image Effect Reder background task class header file
*/
#pragma once

#include "stdafx.h"
#include "KBgTask.h"
#include "KEcmBaseItem.h"

#include "../Document/KImagePage.h"

#include "../Platform/Utils.h"

#define R_WARN_SERVER_BUSY -2
#define R_FAIL_SCAN_SERVER_FOLDERS -3

class KBgTaskEffectRender : public KBgTask
{
public:
  KBgTaskEffectRender(CWnd* owner, BgTaskType task, LPCTSTR arg);
  virtual ~KBgTaskEffectRender(void);

  virtual void Do();

  BOOL SetEffectRender(KImagePage* p, ImageEffectJob* effect);
  void DetachImage() { m_CacheImage = NULL; }

  KImagePage* m_pPage;
  ImageEffectJob* mpPreviewEffect;
  KBitmap* m_CacheImage;

};

