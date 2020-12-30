#include "stdafx.h"
#include "KBgTaskEffectRender.h"

#include "KConstant.h"
#include "KSettings.h"

#include "ThreadPool.h"
#include <strsafe.h>
#include <Shlwapi.h>

#include "../Platform/Platform.h"

KBgTaskEffectRender::KBgTaskEffectRender(CWnd* owner, BgTaskType task, LPCTSTR arg)
  : KBgTask(owner, task, arg)
{
  mpPreviewEffect = NULL;
  m_pPage = NULL;
}

KBgTaskEffectRender::~KBgTaskEffectRender(void)
{
  if (mpPreviewEffect != NULL)
    delete mpPreviewEffect;
  if (m_CacheImage != NULL)
    delete m_CacheImage;
}

BOOL KBgTaskEffectRender::SetEffectRender(KImagePage* p, ImageEffectJob* effect)
{
  m_pPage = p;
  KBitmap* bm = m_pPage->GetImage(NULL, NULL, TRUE);
  if (bm != NULL)
  {
    m_CacheImage = new KBitmap(*bm);
    if (mpPreviewEffect == NULL)
      mpPreviewEffect = new ImageEffectJob;
    memcpy(mpPreviewEffect, effect, sizeof(ImageEffectJob));
  }
  return (bm != NULL);
}

void KBgTaskEffectRender::Do()
{
  if (m_CacheImage != NULL)
  {
    //m_CacheImage->ApplyEffect(mpPreviewEffect);

    BOOL need_spare = FALSE;
    LPVOID spare = NULL;

    // 1. ready temporary buffer
    switch (mpPreviewEffect->job)
    {
    case PropertyMode::PModeRotate:
      if ((mpPreviewEffect->param[0] == (int)RotateMode::ROTATE_90CW) ||
        (mpPreviewEffect->param[0] == (int)RotateMode::ROTATE_90CCW))
        spare = new KBitmap(m_CacheImage->height, m_CacheImage->width, m_CacheImage->bitCount,
        (m_CacheImage->bpp == 4), m_CacheImage->palette, m_CacheImage->paletteNumber);

      else if (mpPreviewEffect->param[0] == (int)RotateMode::ROTATE_180)
        spare = new KBitmap(m_CacheImage->width, m_CacheImage->height, m_CacheImage->bitCount,
        (m_CacheImage->bpp == 4), m_CacheImage->palette, m_CacheImage->paletteNumber);

      else if (mpPreviewEffect->param[0] == (int)RotateMode::FLIP_VERT)
        spare = new BYTE[m_CacheImage->lineBufferLength];

      break;

    case PropertyMode::PModeMonochrome:
    {
      int asGrayscale = mpPreviewEffect->param[0];
      double dinvgamma = (double)mpPreviewEffect->param[1] / 100;
      double dMax = pow(255.0, dinvgamma) / 255.0;
      RGBQUAD palettes[256];
      int pnumber = 0;
      if (asGrayscale)
      {
        pnumber = 256;
        for (int i = 0; i < 256; i++)
        {
          palettes[i].rgbBlue = i;
          palettes[i].rgbGreen = i;
          palettes[i].rgbRed = i;
          palettes[i].rgbReserved = 0;
        }
      }
      else
      {
        pnumber = 2;
        for (int i = 0; i < 2; i++)
        {
          palettes[i].rgbBlue = (i == 0) ? 0 : 255;
          palettes[i].rgbGreen = (i == 0) ? 0 : 255;
          palettes[i].rgbRed = (i == 0) ? 0 : 255;
          palettes[i].rgbReserved = 0;
        }
      }
      spare = new KBitmap(m_CacheImage->width, m_CacheImage->height, (asGrayscale ? 8 : 1), FALSE, palettes, pnumber);
      break;
    }

    case PropertyMode::PModeColorLevel:
      break;

    case PropertyMode::PModeColorBalance:
    {
      int bluev = 255 * mpPreviewEffect->param[2] / 100;
      int greenv = 255 * mpPreviewEffect->param[1] / 100;
      int redv = 255 * mpPreviewEffect->param[0] / 100;

      BYTE* cTable = new BYTE[256*3];

      for (int i = 0; i < 256; i++)
      {
        cTable[i] = (BYTE)max(0, min(255, i + redv));
        cTable[i+256] = (BYTE)max(0, min(255, i + greenv));
        cTable[i+512] = (BYTE)max(0, min(255, i + bluev));
      }
      spare = cTable;
      break;
    }

    case PropertyMode::PModeColorBright:
    {
      float c = (100 + mpPreviewEffect->param[1]) / 100.0f;
      int bright = mpPreviewEffect->param[0] * 255 / 100 + 128;

      BYTE* cTable = new BYTE[256];
      for (int i = 0; i < 256; i++) {
        cTable[i] = (BYTE)max(0, min(255, (int)((i - 128)*c + bright + 0.5f)));
      }
      spare = cTable;
      break;
    }

    }

    // 2. execute effect loop
    int step = 0;

    while (IsContinue())
    {
      int progress = m_CacheImage->ApplyEffectStep(mpPreviewEffect, step, spare);

      if ((progress < 0) || (progress > 100))
        break;
      SetProgressed(progress);
    }

    if (IsContinue())
    {
      SetDone();
#ifdef _DEBUG
      SendLogMessage(_T(__FUNCTION__), _T("Complete preview rendering"), SYNC_MSG_LOG);
#endif
    }
#ifdef _DEBUG
    else
    {
      TCHAR msg[128];
      StringCchPrintf(msg, 128, _T("Stop preview rendering %d"), step);
      SendLogMessage(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
    }
#endif

    // 1. clear temporary buffer
    switch (mpPreviewEffect->job)
    {
    case PropertyMode::PModeRotate:
      if ((mpPreviewEffect->param[0] == (int)RotateMode::ROTATE_90CW) ||
        (mpPreviewEffect->param[0] == (int)RotateMode::ROTATE_90CCW) ||
        (mpPreviewEffect->param[0] == (int)RotateMode::ROTATE_180))
      {
        KBitmap* bm = (KBitmap*)spare;
        delete bm;
      }
      else if (mpPreviewEffect->param[0] == (int)RotateMode::FLIP_VERT)
      {
        LPBYTE buff = (LPBYTE)spare;
        delete[] buff;
      }
      break;

    case PropertyMode::PModeMonochrome:
    {
      KBitmap* bm = (KBitmap*)spare;
      delete bm;
    }
    break;

    case PropertyMode::PModeColorBalance:
    case PropertyMode::PModeColorBright:
    {
      LPBYTE buff = (LPBYTE)spare;
      delete[] buff;
    }

    }

  }
}

