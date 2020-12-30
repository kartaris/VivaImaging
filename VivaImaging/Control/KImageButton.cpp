// KImageButton.cpp: 구현 파일
//

#include "stdafx.h"
#include "..\VivaImaging.h"
#include "..\Core\KConstant.h"
#include "..\Document\Shape\KShapeBase.h"
#include "KImageButton.h"

#include "..\Platform\Graphics.h"

// KImageButton

IMPLEMENT_DYNAMIC(KImageButton, CButton)

KImageButton::KImageButton()
{
  mResourceId = -1;
  mImageIndex = -1;
  mBSelected = FALSE;
}

KImageButton::~KImageButton()
{
}


BEGIN_MESSAGE_MAP(KImageButton, CWnd)
END_MESSAGE_MAP()



// KImageButton 메시지 처리기

BOOL KImageButton::Create(DWORD windowStyle, RECT& rect, CWnd* parent, int controlId, int resid, int index)
{
  mResourceId = resid;
  mImageIndex = index;
  windowStyle |= BS_PUSHBUTTON | BS_OWNERDRAW;

  return CButton::Create(_T("O"), windowStyle, rect, parent, controlId);
}

void KImageButton::SetSelected(BOOL bSelected)
{
  if (mBSelected != bSelected)
  {
    mBSelected = bSelected;
    Invalidate();
  }
}

KBitmapCacheVector KImageButton::gBitmapCache;

HBITMAP KImageButton::getCacheResource(int resourceId)
{
  KBitmapCacheIterator it = gBitmapCache.begin();
  while (it != gBitmapCache.end())
  {
    BitmapCacheData* d = *it;
    if (d->resid == resourceId)
      return d->bm;
    it++;
  }

  BitmapCacheData* d = new BitmapCacheData;
  if (d != NULL)
  {
    d->resid = resourceId;
    d->bm = LoadTransparentImage(resourceId);
    if (d->bm != NULL)
    {
      gBitmapCache.push_back(d);
      return d->bm;
    }
    delete d;
  }
  return NULL;
}

void KImageButton::ClearStatic()
{
  KBitmapCacheIterator it = gBitmapCache.begin();
  while (it != gBitmapCache.end())
  {
    BitmapCacheData* d = *it;
    if (d->bm != NULL)
    {
      DeleteObject(d->bm);
      d->bm = NULL;
      delete d;
    }
    it++;
  }
  gBitmapCache.clear();
}

void KImageButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
  CDC dc;

  dc.Attach(lpDrawItemStruct->hDC);
  COLORREF crOldTextColor = dc.GetTextColor();
  COLORREF crOldBkColor = dc.GetBkColor();

  // If this item is selected, set the background color 
  // and the text color to appropriate values. Erase
  // the rect by filling it with the background color.
  if (//(lpDrawItemStruct->itemAction & ODA_SELECT) ||
    (lpDrawItemStruct->itemState  & (ODS_SELECTED)))
  {
    crOldTextColor = ::GetSysColor(COLOR_HIGHLIGHTTEXT);
    crOldBkColor=::GetSysColor(COLOR_HIGHLIGHT);
  }
  else if (mBSelected)
  {
    crOldTextColor = ::GetSysColor(COLOR_HIGHLIGHTTEXT);
    crOldBkColor = ::GetSysColor(COLOR_BTNSHADOW); // COLOR_GRAYTEXT);
  }

  dc.FillSolidRect(&lpDrawItemStruct->rcItem, crOldBkColor);

  if ((mResourceId > 0) && (mImageIndex >= 0))
  {
    HBITMAP h = getCacheResource(mResourceId);
    if (h != NULL)
    {
      BITMAP bm;
      GetObject(h, sizeof(BITMAP), &bm);

      RECT rect;

      rect.left = (lpDrawItemStruct->rcItem.right - lpDrawItemStruct->rcItem.left - bm.bmHeight) / 2;
      rect.top = (lpDrawItemStruct->rcItem.bottom - lpDrawItemStruct->rcItem.top - bm.bmHeight) / 2;

      if (lpDrawItemStruct->itemState  & ODS_SELECTED)
      {
        rect.left++;
        rect.top++;
      }

      rect.right = rect.left + bm.bmHeight;
      rect.bottom = rect.top + bm.bmHeight;
      int total = bm.bmWidth / bm.bmHeight;

      DrawTransparentImageHorz(dc.GetSafeHdc(), rect, h, mImageIndex, total, FALSE);
    }
  }

  if (lpDrawItemStruct->itemState & ODS_FOCUS)       // If the button is focused
  {
    RECT rect = lpDrawItemStruct->rcItem;
    // Draw a focus rect which indicates the user 
    // that the button is focused
    /*
    int iChange = 2;
    rect.top += iChange;
    rect.left += iChange;
    rect.right -= iChange;
    rect.bottom -= iChange;
    */
    dc.DrawFocusRect(&rect);
  }

  dc.SetTextColor(crOldTextColor);
  dc.SetBkColor(crOldBkColor);

  dc.Detach();
}

