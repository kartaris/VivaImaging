#pragma once

// KImageButton
#include "..\Platform\KColor.h"
#include "..\Platform\KBitmap.h"


typedef struct tagBitmapCacheData
{
  int resid;
  HBITMAP bm;
}BitmapCacheData;

typedef std::vector<BitmapCacheData*> KBitmapCacheVector;
typedef std::vector<BitmapCacheData*>::iterator KBitmapCacheIterator;

class KImageButton : public CButton
{
	DECLARE_DYNAMIC(KImageButton)

public:
	KImageButton();
	virtual ~KImageButton();

  BOOL Create(DWORD windowStyle, RECT& rect, CWnd* parent, int controlId, int resid, int index);
  void SetSelected(BOOL bSelected);
  inline BOOL IsSelected() { return mBSelected; }

  virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

  int mResourceId;
  int mImageIndex;
  BOOL mBSelected;

  static KBitmapCacheVector gBitmapCache;
  static HBITMAP getCacheResource(int resourceId);
  static void ClearStatic();


protected:
	DECLARE_MESSAGE_MAP()
public:

};


