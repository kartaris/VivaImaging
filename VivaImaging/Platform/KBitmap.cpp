#include "stdafx.h"
#include "KBitmap.h"
#include "wincodecsdk.h"
#include "propvarutil.h"

#include "..\Core\KSettings.h"
#include "..\Document\KImageBlockEditor.h"
#include "crypting.h"
#include "KColor.h"

#include <math.h>

#define USE_IIMAGING // WIC API

// #define USE_TOPDOWN_KBITMAP

#ifdef USE_IIMAGING
#include <wincodec.h>
#endif


#ifdef USE_CXIMAGE
#include "K2Image/CxImage/ximage.h"
#include "K2Image/CxImage/ximabmp.h"
#include "K2Image/CxImage/ximajpg.h"
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

KBitmap::KBitmap()
{
	hbitmap = NULL;
	bitImage = NULL;
	paletteNumber = 0;
	palette = NULL;
	bpp = 0;
	lineBufferLength = 0;

	width = 0;
	height = 0;
	bitCount = 0;
	hasAlpha = false;
  horzDPI = 0;
  vertDPI = 0;
}

KBitmap::KBitmap(KBitmap& bm)
{
	hbitmap = NULL;
	bitImage = NULL;
	palette = NULL;
	bpp = 0;
	lineBufferLength = 0;

	*this = bm;
}

KBitmap::KBitmap(int w, int h, short colorDepth, bool bAlpha, RGBQUAD* defpalette, int pnumber)
{
	hbitmap = NULL;
	bitImage = NULL;
	palette = NULL;

	_CreateBuffer(w, h, colorDepth, bAlpha, defpalette, pnumber);
}

KBitmap::KBitmap(HANDLE hdib)
{
  PBITMAPINFOHEADER bi = (PBITMAPINFOHEADER)GlobalLock(hdib);
  if (bi != NULL)
    LoadImageFromDIB(bi, GlobalSize(hdib));
  GlobalUnlock(hdib);
}

KBitmap::KBitmap(PBITMAPINFOHEADER bi, UINT buffSize)
{
  LoadImageFromDIB(bi, buffSize);
  /*
  width = bi->biWidth;
  height = abs(bi->biHeight);
  bitCount = bi->biBitCount;
  hasAlpha = (bi->biBitCount == 32);

  PBITMAPV5HEADER bi5 = (PBITMAPV5HEADER)bi;

  LPBYTE b = (LPBYTE)bi;
  b += bi->biSize; // sizeof(BITMAPINFO);
  paletteNumber = 0;
  if (bi->biBitCount == 4)
    paletteNumber = 16;
  if (bi->biBitCount == 8)
    paletteNumber = 256;

  if (paletteNumber > 0)
  {
    palette = new RGBQUAD[paletteNumber];
    memcpy(palette, b, sizeof(RGBQUAD) * paletteNumber);
    b += sizeof(RGBQUAD) * paletteNumber;
  }
  if (bi->biBitCount == 32)
  {
    //b += sizeof(COLORREF) * 3;
  }
  bpp = bi->biBitCount / 8;
  lineBufferLength = ((bitCount * width + 31) / 32) * 4;
  int buffsize = lineBufferLength * height;



  HDC hdc = GetDC(GetDesktopWindow());
  hbitmap = ::CreateDIBSection(hdc, (BITMAPINFO*)bi, DIB_RGB_COLORS, (void **)&bitImage, NULL, 0);
  ReleaseDC(GetDesktopWindow(), hdc);

  memcpy(bitImage, b, buffsize);
  */
}

KBitmap::~KBitmap()
{
	Clear();
}

RGBQUAD Windows2Palette[2] = 
{
  { 0,0,0,0 },
  { 255,255,255,0 },
};

RGBQUAD Windows16Palette[16] =
{
  { 0,0,0,0 },
  { 128,0,0,0 },
  { 0,128,0,0 },
  { 128,128,0,0 },
  { 0,0,128,0 },
  { 128,0,128,0 },
  { 0,128,128,0 },
  { 192,192,192,0 },
  { 128,128,128,0 },
  { 255,0,0,0 },
  { 0,255,0,0 },
  { 255,255,0,0 },
  { 0,0,255,0 },
  { 255,0,255,0 },
  { 0,255,255,0 },
  { 255,255,255,0 }
};


RGBQUAD Windows256Palette[256] =
{
  { 0,0,0,0 },
  { 128,0,0,0 },
  { 0,128,0,0 },
  { 128,128,0,0 },
  { 0,0,128,0 },
  { 128,0,128,0 },
  { 0,128,128,0 },
  { 192,192,192,0 },
  { 192,220,192,0 },
  { 166,202,240,0 },
  { 4,4,4,0 },
  { 8,8,8,0 },
  { 12,12,12,0 },
  { 17,17,17,0 },
  { 22,22,22,0 },
  { 28,28,28,0 },
  { 34,34,34,0 },
  { 41,41,41,0 },
  { 85,85,85,0 },
  { 77,77,77,0 },
  { 66,66,66,0 },
  { 57,57,57,0 },
  { 255,124,128,0 },
  { 255,80,80,0 },
  { 214,0,147,0 },
  { 204,236,255,0 },
  { 239,214,198,0 },
  { 231,231,214,0 },
  { 173,169,144,0 },
  { 51,0,0,0 },
  { 102,0,0,0 },
  { 153,0,0,0 },
  { 204,0,0,0 },
  { 0,51,0,0 },
  { 51,51,0,0 },
  { 102,51,0,0 },
  { 153,51,0,0 },
  { 204,51,0,0 },
  { 255,51,0,0 },
  { 0,102,0,0 },
  { 51,102,0,0 },
  { 102,102,0,0 },
  { 153,102,0,0 },
  { 204,102,0,0 },
  { 255,102,0,0 },
  { 0,153,0,0 },
  { 51,153,0,0 },
  { 102,153,0,0 },
  { 153,153,0,0 },
  { 204,153,0,0 },
  { 255,153,0,0 },
  { 0,204,0,0 },
  { 51,204,0,0 },
  { 102,204,0,0 },
  { 153,204,0,0 },
  { 204,204,0,0 },
  { 255,204,0,0 },
  { 102,255,0,0 },
  { 153,255,0,0 },
  { 204,255,0,0 },
  { 0,0,51,0 },
  { 51,0,51,0 },
  { 102,0,51,0 },
  { 153,0,51,0 },
  { 204,0,51,0 },
  { 255,0,51,0 },
  { 0,51,51,0 },
  { 51,51,51,0 },
  { 102,51,51,0 },
  { 153,51,51,0 },
  { 204,51,51,0 },
  { 255,51,51,0 },
  { 0,102,51,0 },
  { 51,102,51,0 },
  { 102,102,51,0 },
  { 153,102,51,0 },
  { 204,102,51,0 },
  { 255,102,51,0 },
  { 0,153,51,0 },
  { 51,153,51,0 },
  { 102,153,51,0 },
  { 153,153,51,0 },
  { 204,153,51,0 },
  { 255,153,51,0 },
  { 0,204,51,0 },
  { 51,204,51,0 },
  { 102,204,51,0 },
  { 153,204,51,0 },
  { 204,204,51,0 },
  { 255,204,51,0 },
  { 51,255,51,0 },
  { 102,255,51,0 },
  { 153,255,51,0 },
  { 204,255,51,0 },
  { 255,255,51,0 },
  { 0,0,102,0 },
  { 51,0,102,0 },
  { 102,0,102,0 },
  { 153,0,102,0 },
  { 204,0,102,0 },
  { 255,0,102,0 },
  { 0,51,102,0 },
  { 51,51,102,0 },
  { 102,51,102,0 },
  { 153,51,102,0 },
  { 204,51,102,0 },
  { 255,51,102,0 },
  { 0,102,102,0 },
  { 51,102,102,0 },
  { 102,102,102,0 },
  { 153,102,102,0 },
  { 204,102,102,0 },
  { 0,153,102,0 },
  { 51,153,102,0 },
  { 102,153,102,0 },
  { 153,153,102,0 },
  { 204,153,102,0 },
  { 255,153,102,0 },
  { 0,204,102,0 },
  { 51,204,102,0 },
  { 153,204,102,0 },
  { 204,204,102,0 },
  { 255,204,102,0 },
  { 0,255,102,0 },
  { 51,255,102,0 },
  { 153,255,102,0 },
  { 204,255,102,0 },
  { 255,0,204,0 },
  { 204,0,255,0 },
  { 0,153,153,0 },
  { 153,51,153,0 },
  { 153,0,153,0 },
  { 204,0,153,0 },
  { 0,0,153,0 },
  { 51,51,153,0 },
  { 102,0,153,0 },
  { 204,51,153,0 },
  { 255,0,153,0 },
  { 0,102,153,0 },
  { 51,102,153,0 },
  { 102,51,153,0 },
  { 153,102,153,0 },
  { 204,102,153,0 },
  { 255,51,153,0 },
  { 51,153,153,0 },
  { 102,153,153,0 },
  { 153,153,153,0 },
  { 204,153,153,0 },
  { 255,153,153,0 },
  { 0,204,153,0 },
  { 51,204,153,0 },
  { 102,204,102,0 },
  { 153,204,153,0 },
  { 204,204,153,0 },
  { 255,204,153,0 },
  { 0,255,153,0 },
  { 51,255,153,0 },
  { 102,204,153,0 },
  { 153,255,153,0 },
  { 204,255,153,0 },
  { 255,255,153,0 },
  { 0,0,204,0 },
  { 51,0,153,0 },
  { 102,0,204,0 },
  { 153,0,204,0 },
  { 204,0,204,0 },
  { 0,51,153,0 },
  { 51,51,204,0 },
  { 102,51,204,0 },
  { 153,51,204,0 },
  { 204,51,204,0 },
  { 255,51,204,0 },
  { 0,102,204,0 },
  { 51,102,204,0 },
  { 102,102,153,0 },
  { 153,102,204,0 },
  { 204,102,204,0 },
  { 255,102,153,0 },
  { 0,153,204,0 },
  { 51,153,204,0 },
  { 102,153,204,0 },
  { 153,153,204,0 },
  { 204,153,204,0 },
  { 255,153,204,0 },
  { 0,204,204,0 },
  { 51,204,204,0 },
  { 102,204,204,0 },
  { 153,204,204,0 },
  { 204,204,204,0 },
  { 255,204,204,0 },
  { 0,255,204,0 },
  { 51,255,204,0 },
  { 102,255,153,0 },
  { 153,255,204,0 },
  { 204,255,204,0 },
  { 255,255,204,0 },
  { 51,0,204,0 },
  { 102,0,255,0 },
  { 153,0,255,0 },
  { 0,51,204,0 },
  { 51,51,255,0 },
  { 102,51,255,0 },
  { 153,51,255,0 },
  { 204,51,255,0 },
  { 255,51,255,0 },
  { 0,102,255,0 },
  { 51,102,255,0 },
  { 102,102,204,0 },
  { 153,102,255,0 },
  { 204,102,255,0 },
  { 255,102,204,0 },
  { 0,153,255,0 },
  { 51,153,255,0 },
  { 102,153,255,0 },
  { 153,153,255,0 },
  { 204,153,255,0 },
  { 255,153,255,0 },
  { 0,204,255,0 },
  { 51,204,255,0 },
  { 102,204,255,0 },
  { 153,204,255,0 },
  { 204,204,255,0 },
  { 255,204,255,0 },
  { 51,255,255,0 },
  { 102,255,204,0 },
  { 153,255,255,0 },
  { 204,255,255,0 },
  { 255,102,102,0 },
  { 102,255,102,0 },
  { 255,255,102,0 },
  { 102,102,255,0 },
  { 255,102,255,0 },
  { 102,255,255,0 },
  { 165,0,33,0 },
  { 95,95,95,0 },
  { 119,119,119,0 },
  { 134,134,134,0 },
  { 150,150,150,0 },
  { 203,203,203,0 },
  { 178,178,178,0 },
  { 215,215,215,0 },
  { 221,221,221,0 },
  { 227,227,227,0 },
  { 234,234,234,0 },
  { 241,241,241,0 },
  { 248,248,248,0 },
  { 255,251,240,0 },
  { 160,160,164,0 },
  { 128,128,128,0 },
  { 255,0,0,0 },
  { 0,255,0,0 },
  { 255,255,0,0 },
  { 0,0,255,0 },
  { 255,0,255,0 },
  { 0,255,255,0 },
  { 255,255,255,0 }
};

void KBitmap::_swap(KBitmap& bm)
{
	int v = width;
	width = bm.width;
	bm.width = v;

	v = height;
	height = bm.height;
	bm.height = v;

	v = bitCount;
	bitCount = bm.bitCount;
	bm.bitCount = v;

	bool a = hasAlpha;
	hasAlpha = bm.hasAlpha;
	bm.hasAlpha = a;

	HBITMAP h = hbitmap;
	hbitmap = bm.hbitmap;
	bm.hbitmap = h;

	BYTE* i = bitImage;
	bitImage = bm.bitImage;
	bm.bitImage = i;

	v = lineBufferLength;
	lineBufferLength = bm.lineBufferLength;
	bm.lineBufferLength = v;

	v = bpp;
	bpp = bm.bpp;
	bm.bpp = v;

  RGBQUAD* r = palette;
	palette = bm.palette;
	bm.palette = r;

	WORD n = paletteNumber;
	paletteNumber = bm.paletteNumber;
	bm.paletteNumber = n;
}

const KBitmap& KBitmap::operator=(const KBitmap& img)
{
	if(this != &img)
	{
    Clear();
    CreateBitmap(img.width, img.height, img.bitCount, img.hasAlpha,
      img.palette, img.paletteNumber, img.bitImage);
	}
	return *this;
}

void KBitmap::Clear()
{
	if(hbitmap)
	{
		// Win32 system will free bitmap image buffer.
		DeleteObject(hbitmap);
		hbitmap = NULL;
	}

	/* this buffer will be deleted automatically on WIN32
	if(bitImage)
	{
		HCMemFree(bitImage);
		bitImage = NULL;
	}
	*/

	/*
	if(pbminfo)
	{
		HCMemFree(pbminfo);
		pbminfo = NULL;
	}
	*/
	if(palette != NULL)
	{
		delete[] palette;
		palette = NULL;
	}
	lineBufferLength = 0;
	paletteNumber = 0;

	width = 0;
	height = 0;
}

bool KBitmap::_SetBitCount(short bits)
{
	bitCount = bits;
	hasAlpha = (bitCount >= 32);

  if(bitCount <= 1)
		bitCount = 1;
  else if (bitCount <= 4)
		bitCount = 4;
  else if (bitCount <= 8)
		bitCount = 8;
  else if(bitCount <= 24)
		bitCount = 24;
  else
		bitCount = 32;

	switch(bitCount)
	{
		case 32:
			paletteNumber = 0;
			bpp = 4;
			break;
		case 24:
			paletteNumber = 0;
			bpp = 3;
			break;
		case 8:
			paletteNumber = 256;
			bpp = 1;
			break;
		case 4:
			paletteNumber = 16;
			bpp = 0;
			break;
		case 1:
			paletteNumber = 2;
			bpp = 0;
			break;
		default:
			TRACE("Bad bit count on CreateBuffer\n");
			return false;
	}
	return true;
}

bool KBitmap::_CreateBuffer(int w, int h, short bits, bool bAlpha, RGBQUAD* palettes, int pnumber)
{
	width = w;
	height = h;
	hasAlpha = bAlpha;

	if( !_SetBitCount(bits) )
		return false;

  if (bits == 8)
  {
    if (palettes == NULL)
    {
      palettes = Windows256Palette;
      pnumber = 256;
    }
  }
  else if (bits == 4)
  {
    if (palettes == NULL)
    {
      palettes = Windows16Palette;
      pnumber = 16;
    }
  }
  if (bits == 1)
  {
    if (palettes == NULL)
    {
      palettes = Windows2Palette;
      pnumber = 2;
    }
  }

	CreateBitmap(width, height, bitCount, hasAlpha, palettes, pnumber, NULL);
	return true;
}

BOOL KBitmap::ValidateRect(RECT& rect)
{
  if (rect.left < 0)
    rect.left = 0;
  if (rect.top < 0)
    rect.top = 0;
  if (rect.right > width)
    rect.right = width;
  if (rect.bottom > height)
    rect.bottom = height;
  return ((rect.left < rect.right) && (rect.top < rect.bottom));
}

bool KBitmap::CreateBitmap(int w, int h, short bits, bool bAlpha, RGBQUAD* defpalette, short pNumber, BYTE* buff)
{
	width = w;
	height = abs(h);
	hasAlpha = bAlpha;
  if (!_SetBitCount(bits))
    return false;

	int buffsize = sizeof(BITMAPINFOHEADER);
	BITMAPINFO* pbminfo;
	HDC hdc;

	if(pNumber > 0)
		buffsize += pNumber * sizeof(RGBQUAD);
	else if(hasAlpha)
		buffsize += sizeof(RGBQUAD) * 4;
	else
		buffsize += sizeof(RGBQUAD);

	pbminfo = (BITMAPINFO*) malloc(buffsize);
	pbminfo->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	pbminfo->bmiHeader.biWidth = w;
#ifdef USE_TOPDOWN_KBITMAP
  pbminfo->bmiHeader.biHeight = (LONG)height; // top-down DIB
#else
  pbminfo->bmiHeader.biHeight = (LONG)-height; // bottum-up DIB
#endif
	pbminfo->bmiHeader.biBitCount = bits;
	pbminfo->bmiHeader.biPlanes = 1;
	pbminfo->bmiHeader.biCompression = hasAlpha ? BI_BITFIELDS : BI_RGB;
	pbminfo->bmiHeader.biSizeImage = 0;
	pbminfo->bmiHeader.biXPelsPerMeter = (long)(96 * INCHES_PER_METER);
	pbminfo->bmiHeader.biYPelsPerMeter = (long)(96 * INCHES_PER_METER);
	pbminfo->bmiHeader.biClrUsed = pNumber;
	pbminfo->bmiHeader.biClrImportant = 0;
	lineBufferLength = ((bits * width + 31) / 32) * 4;

	if(hasAlpha)
	{
		int *pMask = (int*)&(pbminfo->bmiColors[0]);
		*pMask++ = 0x00FF0000;
		*pMask++ = 0x0000FF00;
		*pMask++ = 0x000000FF;
		*pMask++ = 0xFF000000;
	}
  else if (pNumber > 0)
  {
    palette = new RGBQUAD[pNumber];
    memcpy(palette, defpalette, sizeof(RGBQUAD) * pNumber);
    memcpy(pbminfo->bmiColors, defpalette, sizeof(RGBQUAD) * pNumber);
    paletteNumber = pNumber;
  }

	hdc = GetDC(GetDesktopWindow());
	hbitmap = ::CreateDIBSection(hdc, pbminfo, DIB_RGB_COLORS, (void **)&bitImage, NULL, 0);
	ReleaseDC(GetDesktopWindow(), hdc);
	free(pbminfo);

  if (hbitmap != NULL)
  {
    if (buff == NULL)
      memset(bitImage, 0, lineBufferLength * height);
    else
      memcpy(bitImage, buff, lineBufferLength * height);
  }
	return(hbitmap != NULL);
}

/*
void KBitmap::SetBitmap(int w, int h, short bits, bool bAlpha, RGBQUAD* defpalette, short pNumber, BYTE* buff)
{
	_CreateBuffer(w, h, bits, bAlpha, defpalette, pNumber);

	if(buff != NULL)
		memcpy(bitImage, buff, lineBufferLength * height);
}
*/

RGBQUAD KBitmap::paletteColor(int i)
{
	if((i >= 0) && (i < paletteNumber))
		return palette[i];

  RGBQUAD rgb = { 0, 0, 0, 0 };
	return rgb;
}

void KBitmap::setPaletteColor(int i, RGBQUAD color)
{
	if((i >= 0) && (i < paletteNumber))
		palette[i] = color;
}

void KBitmap::setAlphaBuffer(bool bAlpha)
{
	hasAlpha = bAlpha;
}

KBitmap* KBitmap::CloneBitmap(RECT& rect, PropertyMode mode)
{
  if ((rect.left >= rect.right) || (rect.top >= rect.bottom))
    return NULL;

  int d_depth = bitCount;
  if (mode > PropertyMode::PModeCopyRectangle)
    d_depth = 32;
  KBitmap* b = new KBitmap(rect.right - rect.left, rect.bottom - rect.top, d_depth, (d_depth == 32), palette, paletteNumber);

  if (mode == PropertyMode::PModeCopyRectangle)
    copyBlock(*b, rect.left, rect.top, rect.right, rect.bottom);
  else
    b->drawImage(0, 0, this, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, 0);

  // set alpha as 0 out of ellipse
  if (d_depth == 32)
  {
    if (mode == PropertyMode::PModeCopyEllipse)
    {
      b->SetEllipseAlphaMask(0, 0, rect.right - rect.left, rect.bottom - rect.top, 0xFF, 0);
    }
    else
    {
      KBitmap mask;
      mask.CreateBitmap(rect.right - rect.left, rect.bottom - rect.top, 24, FALSE, NULL, 0, NULL);
      HANDLE h = mask.handle();

      HDC hdc = GetDC(GetDesktopWindow());
      HDC memdc = CreateCompatibleDC(hdc);
      HANDLE oldh = SelectObject(memdc, h);
      Graphics g(memdc);

      Point* p = new Point[10];
      RECT br = { 0, 0, rect.right - rect.left, rect.bottom - rect.top };
      ShapeType type = (ShapeType)((int)mode - (int)PropertyMode::PModeCopyRectangle + (int)ShapeType::ShapeRectangle);
      int pnum = KShapeBase::ReadyShapePolygon(type, p, 10, br);
      SolidBrush brush(0xFFFFFFFF);
      g.FillPolygon(&brush, p, pnum);
      delete[] p;

      SelectObject(memdc, oldh);
      DeleteDC(memdc);
      ReleaseDC(GetDesktopWindow(), hdc);

      b->SetAlphaMask(0, 0, mask, 0xFF, 0);
    }
  }
  return b;
}

void KBitmap::SetEllipseAlphaMask(int left, int top, int right, int bottom, BYTE inAlpha, BYTE outAlpha)
{
  if (bpp < 4)
  {
    StoreLogHistory(_T(__FUNCTION__), _T("mask available only alpha image "), SYNC_MSG_LOG | SYNC_MSG_DEBUG);
    return;
  }
  int cx = (left + right) / 2;
  int cy = (top + bottom) / 2;
  double a_sq = (right - left) / 2;
  double b_sq = (bottom - top) / 2;
  a_sq = a_sq * a_sq;
  b_sq = b_sq * b_sq;

  for (int y = 0; y < height; y++)
  {
    BYTE* s = _GetScanline(y);
    for (int x = 0; x < width; x++)
    {
      // check inside of ellipse
      int rx = abs(x - cx);
      int ry = abs(y - cy);

      double cr = sqrt((double)rx * rx / a_sq + (double)ry * ry / b_sq);
      if (cr > 1.0)
        memset(s, 0, 3); // clear color for transparent pixel
      s += 3; // RGBA

      *s = (cr > 1.0) ? outAlpha : inAlpha;
      s++;
    }
  }
}

void KBitmap::SetAlphaMask(int left, int top, KBitmap& mask, BYTE inAlpha, BYTE outAlpha)
{
  int xe = width - left;
  int ye = height - top;
  if (xe > mask.width)
    xe = mask.width;
  if (ye > mask.height)
    ye = mask.height;

  if (bpp < 4)
  {
    StoreLogHistory(_T(__FUNCTION__), _T("mask available only alpha image "), SYNC_MSG_LOG | SYNC_MSG_DEBUG);
    return;
  }

  for (int y = top; y < ye; y++)
  {
    BYTE* d = _GetScanline(y);
    BYTE* s = mask._GetScanline(y - top);
    if (left > 0)
      d += left * bpp;
    for (int x = left; x < xe; x++)
    {
      // check inside of ellipse
      if (*s < 128)
        memset(d, 0, 3);
      d += 3;
      if (*s < 128)
        *d = outAlpha;
      else
        *d = inAlpha;
      d++;
      s += mask.bpp;
    }
  }
}

void KBitmap::drawImage(int xp, int yp, KBitmap* b, int x, int y, int w, int h, int mode)
{
	HDC hdc = GetDC(GetDesktopWindow());
	HDC memDC;
	HDC srcDC;
	HANDLE oldmem;
	HANDLE oldsrc;

	memDC = CreateCompatibleDC(hdc);
	srcDC = CreateCompatibleDC(hdc);
	oldmem = SelectObject(memDC, handle());
	oldsrc = SelectObject(srcDC, b->handle());

	BitBlt(memDC, xp, yp, w, h, srcDC, x, y, SRCCOPY);

	SelectObject(memDC, oldmem);
	SelectObject(srcDC, oldsrc);

	DeleteDC(memDC);
	DeleteDC(srcDC);

	ReleaseDC(GetDesktopWindow(), hdc);
}

void KBitmap::drawImage(int xp, int yp, int dw, int dh, KBitmap* b, int x, int y, int w, int h)
{
	if((w == dw) && (h == dh))
		drawImage(xp, yp, b, x, y, w, h, 0);
	else
	{
		HDC hdc = GetDC(GetDesktopWindow());
		HDC memDC;
		HDC srcDC;
		HANDLE oldmem;
		HANDLE oldsrc;

		memDC = CreateCompatibleDC(hdc);
		srcDC = CreateCompatibleDC(hdc);
		oldmem = SelectObject(memDC, handle());
		oldsrc = SelectObject(srcDC, b->handle());

		SetStretchBltMode(memDC, COLORONCOLOR);
		StretchBlt(memDC, xp, yp, dw, dh, srcDC, x, y, w, h, SRCCOPY);

		SelectObject(memDC, oldmem);
		SelectObject(srcDC, oldsrc);

		DeleteDC(memDC);
		DeleteDC(srcDC);

		ReleaseDC(GetDesktopWindow(), hdc);
	}
}

static void _alphaBlendPixel(LPBYTE d, LPBYTE s, BYTE alpha)
{
	unsigned short p;
	int i;

	for(i = 0; i < 3 ; i++)
	{
		p = (unsigned short) *d * (255 - alpha) + (unsigned short) *s * alpha;
		*d = (BYTE) (p >> 8);
		d++;
		s++;
	}
}

// alpha is 0 - 255, 0 : no draw, 128 : 50%, 255 : OPAQUE 
void KBitmap::drawAlpha(int xp, int yp, KBitmap* b, int x, int y, int w, int h, BYTE alpha)
{
	int yc, xc, i;
	LPBYTE d, s;
	unsigned short p;
	BYTE a;

  if ((bpp < 3) || (b->bpp < 3))
  {
    if (bitCount == b->bitCount)
    {
      copyFromBlock(xp, yp, *b, x, y, w, h);
      return;
    }
    if (bpp < 3)
      return;

    // conver it to 24-bit image
    if (b->bpp < 3)
      b->ConverToColor(FALSE);
  }

	if(alpha > 0)
	{
		if(yp + h > height)
			h = height - yp;
		if(xp + w > width)
			w = width - xp;

		for(yc = 0; yc < h; yc++)
		{
			d = _GetScanline(yp + yc) + xp * bpp;
			s = b->_GetScanline(y + yc) + x * b->bpp;

			if((alpha == 255) && (bpp == b->bpp) && (bpp < 4))
			{
				memcpy(d, s, w * bpp);
			}
			else
			{
				a = alpha;
				for(xc = 0; xc < w; xc++)
				{
					// _alphaBlendPixel(d, s, alpha);
					if(b->bpp == 4) // has alpha channel
					{
						double da = (double)((unsigned int) alpha * s[3]);
						a = (BYTE) sqrt(da);
					}
					if(a > 0)
					{
						for(i = 0; i < 3 ; i++)
						{
							p = (unsigned short) *d * (255 - a) + (unsigned short) *s * a;
							*d = (BYTE) (p >> 8);
							d++;
							s++;
						}
						d += (bpp - 3);
						s += (b->bpp - 3);
					}
					else
					{
						d += bpp;
						s += b->bpp;
					}
				}
			}
		}
	}
}

void KBitmap::fill(COLORREF color)
{
	if((bitCount >= 24) && (bitImage != NULL))
	{
		int x, y;
		BYTE* line_buff;
		BYTE* p;
		BYTE b, g, r, a;

		a = (BYTE)(color >> 24);
		b = (BYTE)(color >> 16);
		g = (BYTE)(color >> 8);
		r = (BYTE)(color);

		line_buff = bitImage;
		for(y = 0; y < height; y++)
		{
			p = line_buff;
			for(x = 0; x < width; x++)
			{
				*(p++) = r;
				*(p++) = g;
				*(p++) = b;
				if(bpp == 4)
					*(p++) = a;
			}
			line_buff += lineBufferLength;
		}
	}
  else if (bitCount == 8)
  {
		int x, y;
		BYTE* line_buff;
		BYTE* p;
		BYTE r;

		r = (BYTE)(color);
		line_buff = bitImage;
		for(y = 0; y < height; y++)
		{
			p = line_buff;
			for(x = 0; x < width; x++)
			{
				*(p++) = r;
			}
			line_buff += lineBufferLength;
		}
  }
}

LPBYTE KBitmap::_pixelBuffer(int x, int y)
{
	LPBYTE b = bitImage;

	if(b)
	{
		if(y < (height - 1))
			b += lineBufferLength * (height - 1 - y);
    if (x > 0)
    {
      if (bpp > 0)
        b += bpp * x;
      else
        b += (x * bitCount) / 8;
    }
	}
	return b;
}

bool KBitmap::rotateCopy(KBitmap& bm, int direction)
{
	int x, y;
	LPBYTE sb, db;

	if(direction & 1)
	{
		if((width != bm.height) || (height != bm.width))
			return false;
	}
	else
	{
		if((width != bm.width) || (height != bm.height))
			return false;
	}

	if(direction == 1)
	{
		for(y=0 ; y<height ; y++)
		{
			sb = _pixelBuffer(0, y);
			db = bm._pixelBuffer(bm.width-y-1, 0);

			for(x=0 ; x<width ; x++)
			{
				memcpy(db, sb, bpp);
				db -= bm.lineBufferLength;
				sb += bpp;
			}
		}
	}
	else if(direction == 3)
	{
		for(y=0 ; y<height ; y++)
		{
			sb = _pixelBuffer(0, y);
			db = bm._pixelBuffer(y, bm.height-1);

			for(x=0 ; x<width ; x++)
			{
				memcpy(db, sb, bpp);
				db += bm.lineBufferLength;
				sb += bpp;
			}
		}
	}
	else if(direction == 2)
	{
		for(y=0 ; y<height ; y++)
		{
			sb = _pixelBuffer(0, y);
			db = bm._pixelBuffer(bm.width-1, bm.height-1-y);

			for(x=0 ; x<width ; x++)
			{
				memcpy(db, sb, bpp);
				db -= bpp;
				sb += bpp;
			}
		}
	}
	else
	{
		for(y=0 ; y<height ; y++)
		{
			sb = _pixelBuffer(0, y);
			db = bm._pixelBuffer(0, y);
			memcpy(db, sb, bpp*width);
		}
	}
	return true;
}

bool KBitmap::rotate(int direction)
{
	if(hbitmap != NULL)
	{
		KBitmap bm(height, width, bitCount);

		if(bm.hbitmap != NULL)
		{
			rotateCopy(bm, direction);
			_swap(bm);
			return true;
		}
	}
	return false;
}

bool KBitmap::resize(int cx, int cy, bool preserve_xy_ratio)
{
	if((cx != width) || (cy != height))
	{
		KBitmap* nb;
		nb = new KBitmap(cx, cy, bitCount, hasAlpha);
		if((nb != NULL) && nb->handle())
		{
			HDC memDC;
			HDC srcDC;
			HGDIOBJ osbm, onbm;
			HDC dc;

			dc = GetDC(GetDesktopWindow());

			memDC = CreateCompatibleDC(dc);
			srcDC = CreateCompatibleDC(dc);

			osbm = SelectObject(srcDC, handle());
			onbm = SelectObject(memDC, nb->handle());

			SetStretchBltMode(memDC, COLORONCOLOR);
			StretchBlt(memDC, 0, 0, cx, cy, srcDC, 0, 0, width, height, SRCCOPY);

			SelectObject(srcDC, osbm);
			SelectObject(memDC, onbm);

			ReleaseDC(GetDesktopWindow(), dc);

			_swap(*nb);
			delete nb;
			return true;
		}
	}
	return false;
}

bool KBitmap::crop(RECT& rect)
{
	int cx, cy;

	cx = width - rect.left - rect.right;
	cy = height - rect.top - rect.bottom;
	if((cx > 0) && (cy > 0))
	{
		KBitmap* nb;

		nb = new KBitmap(cx, cy, bitCount, hasAlpha);
		if(nb != NULL)
		{
			HDC dc;
			HDC memDC;
			HDC srcDC;
			HGDIOBJ osbm, onbm;

			dc = GetDC(GetDesktopWindow());
			memDC = CreateCompatibleDC(dc);
			srcDC = CreateCompatibleDC(dc);

			osbm = SelectObject(srcDC, handle());
			onbm = SelectObject(memDC, nb->handle());

			SetStretchBltMode(memDC, COLORONCOLOR);
			BitBlt(memDC, 0, 0, cx, cy, srcDC, rect.left, rect.top, SRCCOPY);

			SelectObject(srcDC, osbm);
			SelectObject(memDC, onbm);

			ReleaseDC(GetDesktopWindow(), dc);

			_swap(*nb);
			delete nb;
			return true;
		}
	}
	else
	{
		Clear();
	}
	return false;
}

bool KBitmap::SetAllAlpha(BYTE a)
{
	int y, x;
	BYTE* lineBuffer;
	BYTE* sBuffer;

	if((bitCount == 32) && (bitImage != NULL))
	{
		sBuffer = bitImage + 3;
		for(y = 0; y < height; y++)
		{
			lineBuffer = sBuffer;
			for(x = 0; x < width; x++)
			{
				*(lineBuffer) = a;
				lineBuffer += 4;
			}
			sBuffer += lineBufferLength;
		}
		return true;
	}
	return false;
}

bool KBitmap::copyAlphaChannel(KBitmap* mask)
{
	if( (bitCount == 32) && (mask->bitCount >= 8) &&
		(width == mask->width) && (height == mask->height) )
	{
		int y, x, alpha_bpp;
		BYTE* lineBuffer;
		BYTE* alphaBuffer;
		BYTE* sBuffer;
		BYTE* dBuffer;

		sBuffer = bitImage + 3;
		dBuffer = mask->bitImage;
		alpha_bpp = (mask->bitCount / 8);

		for(y = 0; y < height; y++)
		{
			lineBuffer = sBuffer;
			alphaBuffer = dBuffer;

			for(x = 0; x < width; x++)
			{
#ifdef WIN32_DESKTOP // 잔상 남는 문제 때문에...
				if(*alphaBuffer == 0)
					memset(lineBuffer-3, 0, 3);
#endif
				*(lineBuffer) = *(alphaBuffer);
				lineBuffer += 4;
				alphaBuffer += alpha_bpp;
			}

			sBuffer += lineBufferLength;
			dBuffer += mask->lineBufferLength;
		}

		hasAlpha = true;
		return true;
	}
	else
		TRACE("copyAlphaChannel fail:bitCount=%d, mask->bitCount=%d\n", bitCount, mask->bitCount);
	return false;
}

bool KBitmap::MergeImage(KBitmap& img)
{
	if(bitCount == img.bitCount)
	{
		int y, x, w, h;
		BYTE* s;
		BYTE* d;
		BYTE* sBuffer;
		BYTE* dBuffer;

		// lineBuffer is r,g,b,a
		sBuffer = bitImage;
		dBuffer = img.bitImage;
		w = width;
		if(w > img.width)
			w = img.width;
		h = height;
		if(h > img.height)
			h = img.height;

		for(y = 0; y < h; y++)
		{
			s = sBuffer;
			d = dBuffer;

			for(x = 0; x < w; x++)
			{
				*s = (*d & *s);
				s++;
				d++;
			}

			sBuffer += lineBufferLength;
			dBuffer += img.lineBufferLength;
		}
		return true;
	}
	return false;
}

#define PIE_VALUE 3.141592

BOOL KBitmap::ReadyEffectMaskImage(TRANSITION_EFFECT_TYPE effectType, int maxStep, int effectStep)
{
  if (effectType == TE_CIRCULAR_SECTOR)
  {
    int x, y;
		BYTE* s;
    int hx = width / 2;
    double a;
    double base_a = PIE_VALUE * effectStep / (2 * maxStep);

		for(y = 0; y < height; y++)
		{
      s = bitImage + y * lineBufferLength;
			for(x = 0; x < width; x++)
			{
        int dx = x - hx;
        int dy = height - y;

        if (dy != 0)
        {
          a = atan((double)dy / dx);
        }
        else
        {
          a = (dx >= 0) ? 0.0 : 3.0;
        }

				*s = (base_a > a) ? 255 : 0;
        s++;
			}
		}
  }

  return TRUE;
}

/*
void AddContrastBright(HSV_C& hsv, char contrast, char brightness)
{
	int v;

	v = hsv.Sat + (hsv.Sat * contrast / 100);
	if(v > 100)
		v = 100;
	if(v < 0)
		v = 0;
	hsv.Sat = v;

	v = hsv.Val + (hsv.Val * brightness / 100);
	if(v > 100)
		v = 100;
	if(v < 0)
		v = 0;
	hsv.Val = v;
}

bool KBitmap::SetContrastBrightness(char contrast, char brightness)
{
	if(bitCount >= 24)
	{
		int y, x;
		BYTE* lineBuffer;
		BYTE* sBuffer;
		RGB_C rgb;
		HSV_C hsv;

		// lineBuffer is r,g,b,a
		sBuffer = bitImage;

		for(y = 0; y < height; y++)
		{
			lineBuffer = sBuffer;

			for(x = 0; x < width; x++)
			{
				rgb.Blu = lineBuffer[0];
				rgb.Grn = lineBuffer[1];
				rgb.Red = lineBuffer[2];

				hsv = ConvertRGB2HSV(rgb);
				AddContrastBright(hsv, contrast, brightness);
				rgb = ConvertHSV2RGB(hsv);

				lineBuffer[0] = rgb.Blu;
				lineBuffer[1] = rgb.Grn;
				lineBuffer[2] = rgb.Red;

				lineBuffer += bpp;
			}
			sBuffer += lineBufferLength;
		}
		return true;
	}
	return false;
}

bool KBitmap::SetGrayscale(bool bw)
{
	if(bitCount >= 24)
	{
		int y, x;
		BYTE* lineBuffer;
		BYTE* sBuffer;
		RGB_C rgb;
		BYTE g;

		sBuffer = bitImage;

		for(y = 0; y < height; y++)
		{
			lineBuffer = sBuffer;

			for(x = 0; x < width; x++)
			{
				rgb.Blu = lineBuffer[0];
				rgb.Grn = lineBuffer[1];
				rgb.Red = lineBuffer[2];

				g = GetGrayLevel(rgb.Red, rgb.Grn, rgb.Blu);
				if(bw)
					g = (g >= 128) ? 255 : 0;

				lineBuffer[0] = g;
				lineBuffer[1] = g;
				lineBuffer[2] = g;

				lineBuffer += bpp;
			}
			sBuffer += lineBufferLength;
		}
		return true;
	}
	return false;
}

bool KBitmap::setAlphaValue(BYTE alpha)
{
	if(bitCount == 32)
	{
		int y, x;
		BYTE* lineBuffer;
		BYTE* sBuffer;

		sBuffer = bitImage;

		for(y = 0; y < height; y++)
		{
			lineBuffer = sBuffer;
			for(x = 0; x < width; x++)
			{
				lineBuffer[3] = (BYTE)(((unsigned) lineBuffer[3] * alpha) >> 8);
				lineBuffer += bpp;
			}
			sBuffer += lineBufferLength;
		}
		return true;
	}
	return false;
}

*/

#if defined(USE_CXIMAGE)

void Copy24BitAlphaTo32Image(LPBYTE d, LPBYTE s, LPBYTE a, int sbpp, int biWidth, int biHeight, int flag)
{
	int		x, y, so, sw;
	LPBYTE org = s;

	sw = ((sbpp == 24) ? 3 : 4);
	so = WIDTHBYTES((DWORD) biWidth * sbpp) - (biWidth * sw);

	// 알파버퍼는 바이트 단위.
	// ao = WIDTHBYTES((DWORD) biWidth * 8) - biWidth;

	for(y=0 ; y<biHeight ; y++)
	{
		if(flag & 1) // flip horizontal
			s = org + biWidth * (biHeight - y - 1) * sw;

		for(x=0 ; x<biWidth ; x++)
		{
			if((a != NULL) && (*a == 0))
				memset(d, 0, 3);
			else
				memcpy(d, s, 3);
			d += 3;
			if(a != NULL)
				*(d++) = *(a++);
			else
				*(d++) = 0xFF;
			s += sw;
		}
		if(so > 0)
			s += so;
	}
}

static void reverseStrip(BYTE* bitImage, int height, int lineBufferLength)
{
	int y;
	BYTE* buff;
	BYTE* d1;
	BYTE* d2;
	int half = (height / 2);

	buff = (BYTE*) malloc(lineBufferLength);
	for(y = 0; y < half; y++)
	{
		d1 = bitImage + y * lineBufferLength;
		d2 = bitImage + (height - y - 1) * lineBufferLength;
		memcpy(buff, d1, lineBufferLength);
		memcpy(d1, d2, lineBufferLength);
		memcpy(d2, buff, lineBufferLength);
	}
	free(buff);
}

#endif

HBITMAP KBitmap::LoadFromPNGResource(int resid)
{
  HINSTANCE instance = AfxGetInstanceHandle();
  HRSRC hResource = ::FindResource(instance, MAKEINTRESOURCE(resid), _T("PNG"));
  if (!hResource)
    return NULL;

  DWORD imageSize = ::SizeofResource(instance, hResource);
  if (!imageSize)
    return NULL;

  const void* pResourceData = ::LockResource(::LoadResource(instance, hResource));
  if (!pResourceData)
    return NULL;

  HGLOBAL buffer = ::GlobalAlloc(GMEM_MOVEABLE, imageSize);
  if (buffer != NULL)
  {
    void* pBuffer = ::GlobalLock(buffer);
    if (pBuffer)
    {
      CopyMemory(pBuffer, pResourceData, imageSize);

#if 0
      IStream* pStream = NULL;
      if (::CreateStreamOnHGlobal(buffer, FALSE, &pStream) == S_OK)
      {
        Bitmap *bitmap = Bitmap::FromStream(pStream);
        Color bgcolor(0, 0, 0, 0);
        bitmap->GetHBITMAP(bgcolor, &hbitmap);

        pStream->Release();
        delete bitmap;
      }
#else
      LoadImageFromBuffer((LPBYTE)pBuffer, imageSize, 0);
#endif

      ::GlobalUnlock(buffer);
    }
    ::GlobalFree(buffer);
  }
  return hbitmap;
}

HBITMAP KBitmap::LoadImageFromBuffer(LPBYTE buff, int size, DWORD flag, DWORD frameIndex)
{
	HRESULT hr = S_FALSE;
	return loadImageFromBufferCx(buff, size, flag, frameIndex);
}

HBITMAP KBitmap::LoadImageFromFile(const WCHAR* filename, DWORD flag, DWORD frameIndex)
{
	return loadImageFromFileCx(filename, flag, frameIndex);
}

HBITMAP KBitmap::loadImageFromFileCx(const WCHAR* filename, DWORD flag, DWORD frameIndex)
{
	LPBYTE buff;
	DWORD access, share, create;
	HANDLE h;
  HBITMAP bitmap = NULL;

	access = GENERIC_READ;
	share = FILE_SHARE_READ;
	create = OPEN_EXISTING;

	h = CreateFile((LPCWSTR)filename, access, share, NULL, create, 0, 0);
	if(h != INVALID_HANDLE_VALUE)
	{
		DWORD sizeHigh;
		DWORD size;

		size = GetFileSize(h, &sizeHigh);
		if(size > 0)
		{
			buff = (LPBYTE)malloc(size);
			if(buff != NULL)
			{
				DWORD nReaded;
				
				ReadFile(h, buff, size, &nReaded, NULL);
				if(nReaded == size)
				{
					bitmap = loadImageFromBufferCx(buff, size, flag, frameIndex);

					/***
					BYTE* d = buff;

					for(DWORD i = 0; i < size; i++)
					{
						TRACE("0x%.2x,", *d++);
						if((i & 0x000F) == 15)
							TRACE("\n");
					}
					***/
				}
				free(buff);
			}
		}
		CloseHandle(h);
	}
	return(bitmap);
}

HBITMAP KBitmap::LoadImageFromStream(IStream* pStream, DWORD flag, DWORD frameIndex)
{
  IWICImagingFactory* pImageFactory = 0;

  K_CoInitializeEx(0, COINIT_MULTITHREADED);
  if (SUCCEEDED(CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER,
    IID_IWICImagingFactory, (void**)&pImageFactory)))
  {
    IWICBitmapDecoder *pDecoder = NULL;
    if ((pStream != NULL) &&
      SUCCEEDED(pImageFactory->CreateDecoderFromStream(pStream, NULL, WICDecodeMetadataCacheOnDemand, &pDecoder)))
    {
      UINT uiFrameCount = 0;
      if (SUCCEEDED(pDecoder->GetFrameCount(&uiFrameCount)))
      {
        IWICBitmapFrameDecode *pIBitmapFrame;
        if (frameIndex >= uiFrameCount)
          frameIndex = uiFrameCount - 1;

        if (SUCCEEDED(pDecoder->GetFrame(frameIndex, &pIBitmapFrame)))
        {
          LoadImageFromFrameDecoder(pImageFactory, pIBitmapFrame);
        }
      }
      pDecoder->Release();
    }
    pImageFactory->Release();
  }
  K_CoUninitialize();
  // decoded image is bottom-up
#ifdef USE_TOPDOWN_KBITMAP
  Flip(FLIP_VERTICAL);
#endif
  return(hbitmap);
}

HBITMAP KBitmap::LoadImageFromFrameDecoder(IWICImagingFactory* pImageFactory, IWICBitmapFrameDecode *pIBitmapFrame)
{
  IWICFormatConverter *pConverter = NULL;
  WICPixelFormatGUID PixelFormat;
  UINT cx, cy;

  pIBitmapFrame->GetPixelFormat(&PixelFormat);
  pIBitmapFrame->GetSize(&cx, &cy);
  double xDpi, yDpi;
  pIBitmapFrame->GetResolution(&xDpi, &yDpi);

  if (PixelFormat == GUID_WICPixelFormat24bppRGB)
  {
    pImageFactory->CreateFormatConverter(&pConverter);

    pConverter->Initialize(
      pIBitmapFrame,
      GUID_WICPixelFormat24bppBGR,
      WICBitmapDitherTypeNone,
      NULL,
      0.f,
      WICBitmapPaletteTypeCustom);

    pConverter->GetPixelFormat(&PixelFormat);
  }
  else if ((PixelFormat == GUID_WICPixelFormat24bppBGR) ||
    (PixelFormat == GUID_WICPixelFormat8bppIndexed) ||
    (PixelFormat == GUID_WICPixelFormat4bppIndexed) ||
    (PixelFormat == GUID_WICPixelFormat1bppIndexed))
  {
  }
  else if (PixelFormat != GUID_WICPixelFormat32bppPBGRA)
  {
    pImageFactory->CreateFormatConverter(&pConverter);

    pConverter->Initialize(
      pIBitmapFrame,
      GUID_WICPixelFormat32bppPBGRA,
      WICBitmapDitherTypeNone,
      NULL,
      0.f,
      WICBitmapPaletteTypeCustom);

    pConverter->GetPixelFormat(&PixelFormat);
  }

  if ((PixelFormat == GUID_WICPixelFormat32bppPBGRA) ||
    (PixelFormat == GUID_WICPixelFormat24bppBGR) ||
    (PixelFormat == GUID_WICPixelFormat8bppIndexed) ||
    (PixelFormat == GUID_WICPixelFormat4bppIndexed) ||
    (PixelFormat == GUID_WICPixelFormat1bppIndexed))
  {
    LPBITMAPINFO bi;
    int p = 4;
    if (PixelFormat == GUID_WICPixelFormat8bppIndexed)
      p = 256;
    else if (PixelFormat == GUID_WICPixelFormat4bppIndexed)
      p = 16;
    else if (PixelFormat == GUID_WICPixelFormat1bppIndexed)
      p = 2;

    bi = (LPBITMAPINFO)malloc(p * sizeof(RGBQUAD) + sizeof(BITMAPINFO));
    if (bi != NULL)
    {
      bi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
      bi->bmiHeader.biWidth = cx;
#ifdef USE_TOPDOWN_KBITMAP
      bi->bmiHeader.biHeight = (LONG)cy; // top-down DIB
#else
      bi->bmiHeader.biHeight = 0 - (LONG)cy; // bottom-up DIB
#endif
      bi->bmiHeader.biPlanes = 1;

      if (PixelFormat == GUID_WICPixelFormat32bppPBGRA)
        bi->bmiHeader.biBitCount = 32;
      else if (PixelFormat == GUID_WICPixelFormat24bppBGR)
        bi->bmiHeader.biBitCount = 24;
      else if (PixelFormat == GUID_WICPixelFormat8bppIndexed)
        bi->bmiHeader.biBitCount = 8;
      else if (PixelFormat == GUID_WICPixelFormat4bppIndexed)
        bi->bmiHeader.biBitCount = 4;
      else if (PixelFormat == GUID_WICPixelFormat1bppIndexed)
        bi->bmiHeader.biBitCount = 1;
      bi->bmiHeader.biCompression = (bi->bmiHeader.biBitCount == 32) ? BI_BITFIELDS : BI_RGB;
      bi->bmiHeader.biSizeImage = 0;
      bi->bmiHeader.biXPelsPerMeter = (LONG)(xDpi * INCHES_PER_METER);
      bi->bmiHeader.biYPelsPerMeter = (LONG)(yDpi * INCHES_PER_METER);
      bi->bmiHeader.biClrUsed = 0;
      bi->bmiHeader.biClrImportant = 0;
      bitCount = bi->bmiHeader.biBitCount;
      if (bitCount == 32)
      {
        int *pMask = (int*)&(bi->bmiColors[0]);
        *pMask++ = 0x00FF0000;
        *pMask++ = 0x0000FF00;
        *pMask++ = 0x000000FF;
        *pMask++ = 0xFF000000;
      }
      else if (bi->bmiHeader.biBitCount == 24)
      {
        int *pMask = (int*)&(bi->bmiColors[0]);
        *pMask++ = 0x00FF0000;
        *pMask++ = 0x0000FF00;
        *pMask++ = 0x000000FF;
      }

      paletteNumber = 0;
      palette = NULL;
      if (bitCount <= 8)
      {
        IWICPalette* pPalette = NULL;
        UINT actualColors = 0;
        UINT paletteCount = 0;

        if (bitCount == 8)
          paletteNumber = 256;
        else if (bitCount == 4)
          paletteNumber = 16;
        else if (bitCount == 1)
          paletteNumber = 2;

        palette = new RGBQUAD[paletteNumber];

        pImageFactory->CreatePalette(&pPalette);
        if (pConverter != NULL)
          pConverter->CopyPalette(pPalette);
        else
          pIBitmapFrame->CopyPalette(pPalette);

        pPalette->GetColorCount(&paletteCount);
        if (paletteCount > 0)
        {
          WICColor* pPaletteColors = new WICColor[paletteCount];
          pPalette->GetColors(paletteCount, pPaletteColors, &actualColors);
          for (UINT i = 0; i < paletteCount; i++)
          {
            palette[i].rgbRed = GetRValue(pPaletteColors[i]);
            palette[i].rgbGreen = GetGValue(pPaletteColors[i]);
            palette[i].rgbBlue = GetBValue(pPaletteColors[i]);
          }
          delete[] pPaletteColors;
        }
        pPalette->Release();

        if (paletteNumber >(short)paletteCount)
          memset(bi->bmiColors, 0, sizeof(RGBQUAD) * paletteCount);
        memcpy(bi->bmiColors, palette, sizeof(RGBQUAD) * paletteCount);
      }

      lineBufferLength = WIDTHBYTES((DWORD)bi->bmiHeader.biWidth * bi->bmiHeader.biBitCount);
      bi->bmiHeader.biSizeImage = lineBufferLength * bi->bmiHeader.biHeight;

      HDC hdc = GetDC(GetDesktopWindow());
      hbitmap = ::CreateDIBSection(hdc, bi, DIB_RGB_COLORS, (void **)&bitImage, NULL, 0);
      ReleaseDC(GetDesktopWindow(), hdc);

      if (pConverter != NULL)
        pConverter->CopyPixels(NULL, lineBufferLength, bi->bmiHeader.biSizeImage, bitImage);
      else
        pIBitmapFrame->CopyPixels(NULL, lineBufferLength, bi->bmiHeader.biSizeImage, bitImage);

      width = cx;
      height = cy;
      bpp = bi->bmiHeader.biBitCount / 8;
      hasAlpha = (bitCount == 32);
      // lineBufferLength = width * 4;
      horzDPI = (UINT)xDpi;
      vertDPI = (UINT)yDpi;

      free(bi);
    }
  }

  if (pConverter != NULL)
    pConverter->Release();
  return hbitmap;
}

HBITMAP KBitmap::loadImageFromBufferCx(LPBYTE buff, int size, DWORD flag, DWORD frameIndex)
{
#if defined(USE_IIMAGING)
  IWICImagingFactory* pImageFactory = 0;

  K_CoInitializeEx(0, COINIT_MULTITHREADED);
  if (SUCCEEDED(CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER,
    IID_IWICImagingFactory, (void**)&pImageFactory)))
  {
    IWICBitmapDecoder *pDecoder = NULL;
    IWICStream *pStream;

    if (SUCCEEDED(pImageFactory->CreateStream(&pStream)))
      pStream->InitializeFromMemory(buff, size);

    if ((pStream != NULL) &&
      SUCCEEDED(pImageFactory->CreateDecoderFromStream(pStream, NULL, WICDecodeMetadataCacheOnDemand, &pDecoder)))
    {
      UINT uiFrameCount = 0;
      if (SUCCEEDED(pDecoder->GetFrameCount(&uiFrameCount)))
      {
        IWICBitmapFrameDecode *pIBitmapFrame;
        if (frameIndex >= uiFrameCount)
          frameIndex = uiFrameCount - 1;

        if (SUCCEEDED(pDecoder->GetFrame(frameIndex, &pIBitmapFrame)))
        {
          LoadImageFromFrameDecoder(pImageFactory, pIBitmapFrame);

          pIBitmapFrame->Release();
        }
      }
      /*
      if(SUCCEEDED(pImageFactory->CreateImageFromBuffer(buff, size, BufferDisposalFlagNone, &pImage))
        && SUCCEEDED(pImage->GetImageInfo(&imageInfo)))
      {
        CreateBITMAPfromImage(pImageFactory, pImage, imageInfo, pDevice, palette, pal_num);

        pImage->Release();
      }
      else
        TRACE("ImageFactory loading failure\n");
      */
      pDecoder->Release();
    }

    if (pStream != NULL)
      pStream->Release();
    pImageFactory->Release();
  }
  K_CoUninitialize();
  // decoded image is bottom-up
#ifdef USE_TOPDOWN_KBITMAP
  Flip(FLIP_VERTICAL);
#endif
  return(hbitmap);
#elif defined(USE_CXIMAGE)
	CxImage img;

	if( img.Decode(buff, size, CXIMAGE_FORMAT_UNKNOWN) )
	{
		LPBITMAPINFO bi;
		int p, numberOfColors;
		HDC hdc;
		HBITMAP hbitmap = NULL;

		numberOfColors = img.GetNumColors();
		if(numberOfColors > 0)
		{
			if(numberOfColors <= 2)
				numberOfColors = 2;
			else if(numberOfColors <= 16)
				numberOfColors = 16;
			else
				numberOfColors = 256;
		}
		p = numberOfColors;
		if(p < 4)
			p = 4;
		bi = (LPBITMAPINFO)malloc(p * sizeof(RGBQUAD) + sizeof(BITMAPINFO));
		if(bi != NULL)
		{
			bi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
			bi->bmiHeader.biWidth = img.GetWidth();
			bi->bmiHeader.biHeight = img.GetHeight();
			bi->bmiHeader.biPlanes = 1;
			bi->bmiHeader.biBitCount = img.GetBpp();
			bi->bmiHeader.biCompression = BI_RGB;
			bi->bmiHeader.biSizeImage = 0;
			bi->bmiHeader.biXPelsPerMeter = img.GetXDPI() * INCHES_PER_METER; 
			bi->bmiHeader.biYPelsPerMeter = img.GetYDPI() * INCHES_PER_METER; 
			bi->bmiHeader.biClrUsed = img.GetNumColors();
			bi->bmiHeader.biClrImportant = 0; 
			if(numberOfColors > 0)
			{
				memcpy(bi->bmiColors, img.GetPalette(), numberOfColors * sizeof(RGBQUAD));
				for(int i = 0; i < numberOfColors; i++)
				{
					bi->bmiColors[i].rgbRed = GetRValue(palette[i]);
					bi->bmiColors[i].rgbGreen = GetGValue(palette[i]);
					bi->bmiColors[i].rgbBlue = GetBValue(palette[i]);
				}
			}
			else if((bi->bmiHeader.biBitCount == 24) && img.AlphaIsValid())
			{
				bi->bmiHeader.biBitCount = 32;

				int *pMask = (int*)&(bi->bmiColors[0]);
				*pMask++ = 0x00FF0000;
				*pMask++ = 0x0000FF00;
				*pMask++ = 0x000000FF;
				*pMask++ = 0xFF000000;
			}

      lineBufferLength = WIDTHBYTES ((DWORD) bi->bmiHeader.biWidth * bi->bmiHeader.biBitCount);
			bi->bmiHeader.biSizeImage = lineBufferLength * bi->bmiHeader.biHeight;

			hdc = GetDC(GetDesktopWindow());

			// hbitmap = K2P_CreateDIBitmap(hdc, bi, NULL, imageBuff, bi->bmiHeader.biSizeImage);
			hbitmap = ::CreateDIBSection(hdc, bi, DIB_RGB_COLORS, (void **)&bitImage, NULL, 0);

			ReleaseDC(GetDesktopWindow(), hdc);

			// imageBuff = (LPBYTE)malloc(bi->bmiHeader.biSizeImage);
			if(bitImage != NULL)
			{
				width = bi->bmiHeader.biWidth;
				height = bi->bmiHeader.biHeight;
				lineBufferLength = bi->bmiHeader.biSizeImage / bi->bmiHeader.biHeight;
				_SetBitCount(bi->bmiHeader.biBitCount);

				memset(bitImage, 0, bi->bmiHeader.biSizeImage);
				if( (img.GetBpp() == 24) && img.AlphaIsValid() )
				{
					Copy24BitAlphaTo32Image((LPBYTE) bitImage, img.GetBits(), img.AlphaGetPointer() /*GetAlphaBits()*/, img.GetBpp(), bi->bmiHeader.biWidth, bi->bmiHeader.biHeight, 0);
					/*
					if(pAlpha != NULL)
						*pAlpha = true;
					*/
				}
				else if(img.GetBpp() == 32)
				{
					if( img.AlphaIsValid() )
						Copy24BitAlphaTo32Image((LPBYTE) bitImage, img.GetBits(), img.AlphaGetPointer() /*GetAlphaBits()*/, img.GetBpp(), bi->bmiHeader.biWidth, bi->bmiHeader.biHeight, 0);
					else
						memcpy(bitImage, img.GetBits(), bi->bmiHeader.biSizeImage);
					/*
					if(pAlpha != NULL)
						*pAlpha = true;
					*/
				}
				else if(bi->bmiHeader.biSizeImage == (int) (img.GetEffWidth() * bi->bmiHeader.biHeight))
				{
					if(img.IsTransparent())
					{
						int index = img.GetTransIndex();

						// need to fix.
						if((index > 0) && (index < (int)bi->bmiHeader.biClrUsed))
						{
							bi->bmiColors[index].rgbRed = 255;
							bi->bmiColors[index].rgbGreen = 128;
							bi->bmiColors[index].rgbBlue = 128;
						}
					}
					memcpy(bitImage, img.GetBits(), bi->bmiHeader.biSizeImage);
				}

#ifndef IMAGE_STRIP_REVERSED
				reverseStrip(bitImage, height, lineBufferLength);
#endif

			}
			free(bi);
		}
		
		return(hbitmap);
	}
#endif
	return NULL;
}

HBITMAP KBitmap::LoadImageFromRGBABuffer(int w, int h, short colorDepth, RGBQUAD* pal, int pNumber, bool bAlpha, LPBYTE imageBuffer)
{
  LPBITMAPINFO bi;
  int p;
  HDC hdc;

  width = w;
  height = abs(h);
  bitCount = colorDepth;
  hasAlpha = bAlpha;
  paletteNumber = pNumber;
  horzDPI = 96;
  vertDPI = 96;
  if (paletteNumber > 0)
  {
    palette = new RGBQUAD[paletteNumber];
    memcpy(palette, pal, sizeof(RGBQUAD) * paletteNumber);
  }

  p = pNumber;
  if (p < 4)
    p = 4;
  bi = (LPBITMAPINFO)malloc(p * sizeof(RGBQUAD) + sizeof(BITMAPINFO));
  if (bi != NULL)
  {
    bi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi->bmiHeader.biWidth = w;
    bi->bmiHeader.biHeight = h;
    bi->bmiHeader.biPlanes = 1;
    bi->bmiHeader.biBitCount = 32;
    bi->bmiHeader.biCompression = BI_RGB;
    bi->bmiHeader.biSizeImage = 0;
    bi->bmiHeader.biXPelsPerMeter = 96 * INCHES_PER_METER;
    bi->bmiHeader.biYPelsPerMeter = 96 * INCHES_PER_METER;
    bi->bmiHeader.biClrUsed = pNumber;
    bi->bmiHeader.biClrImportant = 0;
    if (pNumber > 0)
    {
      memcpy(bi->bmiColors, palette, pNumber * sizeof(COLORREF));
      /*
      for (int i = 0; i < pNumber; i++)
      {
        bi->bmiColors[i].rgbRed = GetRValue(palette[i]);
        bi->bmiColors[i].rgbGreen = GetGValue(palette[i]);
        bi->bmiColors[i].rgbBlue = GetBValue(palette[i]);
      }
      */
    }
    else if (bi->bmiHeader.biBitCount == 32)
    {
      bi->bmiHeader.biBitCount = 32;
      bi->bmiHeader.biCompression = BI_BITFIELDS;

      DWORD* pMask = (DWORD*)&(bi->bmiColors[0]);
      *pMask++ = 0x00FF0000;
      *pMask++ = 0x0000FF00;
      *pMask++ = 0x000000FF;
      *pMask++ = 0xFF000000;
    }

    lineBufferLength = WIDTHBYTES((DWORD)bi->bmiHeader.biWidth * bi->bmiHeader.biBitCount);
    bi->bmiHeader.biSizeImage = lineBufferLength * abs(bi->bmiHeader.biHeight);

    hdc = GetDC(GetDesktopWindow());
    hbitmap = ::CreateDIBSection(hdc, bi, DIB_RGB_COLORS, (void **)&bitImage, NULL, 0);
    ReleaseDC(GetDesktopWindow(), hdc);

    if (bitImage != NULL)
    {
      memcpy(bitImage, imageBuffer, bi->bmiHeader.biSizeImage);
    }
    free(bi);
  }

  return(hbitmap);
}

HBITMAP KBitmap::LoadImageFromDIB(PBITMAPINFOHEADER pDIB, UINT buffSize)
{
  LPBITMAPINFO bi = (LPBITMAPINFO)pDIB;
  LPBITMAPV5HEADER biv5 = (LPBITMAPV5HEADER)pDIB;

  lineBufferLength = WIDTHBYTES((DWORD)bi->bmiHeader.biWidth * bi->bmiHeader.biBitCount);
  bi->bmiHeader.biSizeImage = lineBufferLength * abs(bi->bmiHeader.biHeight);

#ifndef USE_TOPDOWN_KBITMAP
  BOOL needFlip = FALSE;
  if (bi->bmiHeader.biHeight > 0)
  {
    bi->bmiHeader.biHeight = -bi->bmiHeader.biHeight;
    needFlip = TRUE;
  }
#endif

  HDC hdc = GetDC(GetDesktopWindow());
  hbitmap = ::CreateDIBSection(hdc, bi, DIB_RGB_COLORS, (void **)&bitImage, NULL, 0);
  ReleaseDC(GetDesktopWindow(), hdc);

  BITMAP bm;
  GetObject(hbitmap, sizeof(BITMAP), &bm);

  paletteNumber = 0;
  palette = NULL;
  switch (pDIB->biBitCount)
  {
  case 1:
    paletteNumber = 2;
    break;
  case 8:
    paletteNumber = 256;
    break;
  case 24:
  default:
    break;
  }

  LPBYTE imageBuffer = (LPBYTE)pDIB + bi->bmiHeader.biSize;

  if (paletteNumber > 0)
  {
    palette = new RGBQUAD[paletteNumber];
    memcpy(palette, imageBuffer, sizeof(RGBQUAD) * paletteNumber);
    imageBuffer += sizeof(RGBQUAD) * paletteNumber;
  }

  if (bi->bmiHeader.biCompression == BI_BITFIELDS)
    imageBuffer += sizeof(RGBQUAD) * 3;

  if (buffSize > 0)
  {
    imageBuffer = (LPBYTE)pDIB + buffSize - bi->bmiHeader.biSizeImage;
  }

  if (bitImage != NULL)
  {
    memcpy(bitImage, imageBuffer, bi->bmiHeader.biSizeImage);

    width = bi->bmiHeader.biWidth;
    height = abs(bi->bmiHeader.biHeight);
    bitCount = bi->bmiHeader.biBitCount;
    bpp = bitCount / 8;

    hasAlpha = (bi->bmiHeader.biBitCount == 32);
    lineBufferLength = bi->bmiHeader.biSizeImage / height;

    horzDPI = (UINT)PerMeter2PerInch(bi->bmiHeader.biXPelsPerMeter);
    vertDPI = (UINT)PerMeter2PerInch(bi->bmiHeader.biYPelsPerMeter);

#ifndef USE_TOPDOWN_KBITMAP
    if (needFlip)
      Flip(FLIP_VERTICAL);
#endif
  }
  return(hbitmap);
}

HBITMAP KBitmap::LoadBMPFile(LPCTSTR filename)
{
  HANDLE h = CreateFile(filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, 0);
  if (h != INVALID_HANDLE_VALUE)
  {
    DWORD nReaded;
    BITMAPFILEHEADER bfh;
    BITMAPV5HEADER bih;
    DWORD pos = 0;

    ReadFile(h, &bfh, sizeof(BITMAPFILEHEADER), &nReaded, NULL);
    if (nReaded < sizeof(BITMAPFILEHEADER))
      goto close_read;
    pos += nReaded;

    memset(&bih, 0, sizeof(BITMAPV5HEADER));
    ReadFile(h, &bih, sizeof(BITMAPINFOHEADER), &nReaded, NULL);
    if (nReaded < sizeof(BITMAPINFOHEADER))
      goto close_read;
    pos += nReaded;
    if (bih.bV5Size > sizeof(BITMAPINFOHEADER))
    {
      DWORD adv = bih.bV5Size - sizeof(BITMAPINFOHEADER);
      ReadFile(h, &bih + sizeof(BITMAPINFOHEADER), adv, &nReaded, NULL);
      pos += adv;
    }

    width = bih.bV5Width;
    height = bih.bV5Height;
    bitCount = bih.bV5BitCount;
    hasAlpha = (bitCount == 32);
    lineBufferLength = WIDTHBYTES((DWORD)width * bitCount);
    palette = NULL;

    horzDPI = (UINT)PerMeter2PerInch(bih.bV5XPelsPerMeter);
    vertDPI = (UINT)PerMeter2PerInch(bih.bV5YPelsPerMeter);

    if (pos < bfh.bfOffBits)
    {
      SetFilePointer(h, bfh.bfOffBits - pos, 0, FILE_CURRENT);
    }

    HDC hdc = GetDC(GetDesktopWindow());
    hbitmap = ::CreateDIBSection(hdc, (LPBITMAPINFO)&bih, DIB_RGB_COLORS, (void **)&bitImage, NULL, 0);
    ReleaseDC(GetDesktopWindow(), hdc);

    nReaded = lineBufferLength * height;
    ReadFile(h, bitImage, nReaded, &nReaded, NULL);
    int err = GetLastError();

close_read:
    CloseHandle(h);
  }
  return hbitmap;
}

HBITMAP KBitmap::RefreshBitmapPalette()
{
  if (paletteNumber > 0)
  {
    HDC hdc = CreateCompatibleDC(NULL);
    HBITMAP oldbm = (HBITMAP)::SelectObject(hdc, hbitmap);
    int n = SetDIBColorTable(hdc, 0, paletteNumber, palette);
    //hbitmap = ::CreateDIBSection(hdc, bi, DIB_RGB_COLORS, (void **)&bitImage, NULL, 0);
    ::SelectObject(hdc, oldbm);
    DeleteDC(hdc);
  }
  return hbitmap;
#if 0
  LPBITMAPINFO bi;
  int p;
  HDC hdc;

  p = paletteNumber;
  if (p < 4)
    p = 4;
  bi = (LPBITMAPINFO)malloc(p * sizeof(RGBQUAD) + sizeof(BITMAPINFO));
  if (bi != NULL)
  {
    bi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi->bmiHeader.biWidth = width;
    bi->bmiHeader.biHeight = height;
    bi->bmiHeader.biPlanes = 1;
    bi->bmiHeader.biBitCount = bitCount;
    bi->bmiHeader.biCompression = BI_RGB;
    bi->bmiHeader.biSizeImage = 0;
    bi->bmiHeader.biXPelsPerMeter = 96 * INCHES_PER_METER;
    bi->bmiHeader.biYPelsPerMeter = 96 * INCHES_PER_METER;
    bi->bmiHeader.biClrUsed = paletteNumber;
    bi->bmiHeader.biClrImportant = 0;
    if (paletteNumber > 0)
    {
      memcpy(bi->bmiColors, palette, pNumber * sizeof(COLORREF));
    }
    else if (bi->bmiHeader.biBitCount == 32)
    {
      bi->bmiHeader.biCompression = BI_BITFIELDS;

      DWORD* pMask = (DWORD*)&(bi->bmiColors[0]);
      *pMask++ = 0x00FF0000;
      *pMask++ = 0x0000FF00;
      *pMask++ = 0x000000FF;
      *pMask++ = 0xFF000000;
    }

    lineBufferLength = WIDTHBYTES((DWORD)bi->bmiHeader.biWidth * bi->bmiHeader.biBitCount);
    bi->bmiHeader.biSizeImage = lineBufferLength * abs(bi->bmiHeader.biHeight);

    hdc = GetDC(GetDesktopWindow());
    hbitmap = ::CreateDIBSection(hdc, bi, DIB_RGB_COLORS, (void **)&bitImage, NULL, 0);
    ReleaseDC(GetDesktopWindow(), hdc);

    /*
    if (bitImage != NULL)
    {
      memcpy(bitImage, imageBuffer, bi->bmiHeader.biSizeImage);
    }
    */
    free(bi);
  }
#endif
}

void KBitmap::SetImageDPI(UINT xDpi, UINT yDpi)
{
  horzDPI = (UINT)xDpi;
  vertDPI = (UINT)yDpi;
}

LPBYTE KBitmap::getTopDownImageBuffer()
{
  int h = abs(height);
  UINT cbBufferSize = lineBufferLength * h;
  LPBYTE buff = new BYTE[cbBufferSize];
  LPBYTE d = buff + lineBufferLength * (h - 1);
  LPBYTE s = bitImage;

  for (int y = 0; y < height; y++)
  {
    memcpy(d, s, lineBufferLength);
    d -= lineBufferLength;
    s += lineBufferLength;
  }
  return buff;
}

BOOL KBitmap::StoreBMPFile(LPCTSTR filename, int dpi)
{
  BOOL rtn = FALSE;
  HANDLE h = CreateFile(filename, GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, 0, 0);
  if (h != INVALID_HANDLE_VALUE)
  {
    DWORD nSize;
    BITMAPFILEHEADER bfh;
    BITMAPINFOHEADER bih;
    DWORD pos = 0;

    memset(&bfh, 0, sizeof(BITMAPFILEHEADER));
    bfh.bfType = 19778;
    bfh.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + (lineBufferLength * height);
    bfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

    WriteFile(h, &bfh, sizeof(BITMAPFILEHEADER), &nSize, NULL);
    if (nSize < sizeof(BITMAPFILEHEADER))
      goto close_read;
    pos += nSize;

    if (dpi <= 0)
      dpi = 96;

    memset(&bih, 0, sizeof(BITMAPINFOHEADER));
    bih.biSize = sizeof(BITMAPINFOHEADER);
    bih.biWidth = width;
    bih.biHeight = height;
    bih.biPlanes = 1;
    bih.biBitCount = bitCount;
    bih.biCompression = BI_RGB;
    bih.biSizeImage = lineBufferLength * height;
    bih.biXPelsPerMeter = (int)(100 * dpi / 2.54 + 0.5);
    bih.biYPelsPerMeter = (int)(100 * dpi / 2.54 + 0.5);

    WriteFile(h, &bih, sizeof(BITMAPINFOHEADER), &nSize, NULL);
    if (nSize < sizeof(BITMAPINFOHEADER))
      goto close_read;
    pos += nSize;

    // dummy
    /*
    nSize = 0;
    WriteFile(h, &nSize, 2, &nSize, NULL);
    pos += nSize;
    */

    WriteFile(h, bitImage, lineBufferLength * height, &nSize, NULL);

    pos += nSize;

  close_read:
    CloseHandle(h);
  }
  return rtn;
}

#if 0
BOOL KBitmap::StoreImage(LPCTSTR filename, KVoidPtrArray* pMetaData, LPBYTE pExtraData, int len)
{
  BOOL r = FALSE;
  LPTSTR filename_tmp = (LPTSTR)filename;

  if (IsFileExist(filename) || (mGlobalOption & GO_USECRYPTION))
  {
    filename_tmp = AllocAddString(filename, '_', _T("tmp"));
  }

  if (StoreImageInternal(filename_tmp, pMetaData, pExtraData, len))
  {
    if (mGlobalOption & GO_USECRYPTION)
    {
      KEncryptFile(filename_tmp, filename);
#ifndef _DEBUG
      KDeleteFile(filename_tmp);
#endif
    }
    else if(filename != filename_tmp)
    {
      DeleteFile(filename);
      KMoveFile(filename_tmp, filename);
    }
    r = TRUE;
  }
  if (filename != filename_tmp)
    delete[] filename_tmp;
  return r;
}

BOOL KBitmap::StoreImageInternal(LPCTSTR filename, KVoidPtrArray* pMetaData, LPBYTE pExtraData, int len)
{
#if defined(USE_IIMAGING)
  IWICImagingFactory* pImageFactory = 0;

  HRESULT hr = K_CoInitializeEx(0, COINIT_MULTITHREADED);
  if (SUCCEEDED(CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER,
    IID_IWICImagingFactory, (void**)&pImageFactory)))
  {

    IWICStream *piFileStream = NULL;
    IWICBitmapEncoder *piEncoder = NULL;

    // Create a file stream.
    hr = pImageFactory->CreateStream(&piFileStream);
 
    // Initialize our new file stream.
    if (SUCCEEDED(hr))
    {
      hr = piFileStream->InitializeFromFilename(filename, GENERIC_WRITE);
    }

    BOOL isExtraDataEnable = FALSE;

    // Create the encoder.
    if (SUCCEEDED(hr))
    {
      LPCTSTR ext = StrRChr(filename, NULL, '.');
      if (ext != NULL)
      {
        ext++;
        if (CompareStringICNC(ext, _T("png")) == 0)
          hr = pImageFactory->CreateEncoder(GUID_ContainerFormatPng, NULL, &piEncoder);
        else if (CompareStringICNC(ext, _T("bmp")) == 0)
          hr = pImageFactory->CreateEncoder(GUID_ContainerFormatBmp, NULL, &piEncoder);
        else if ((CompareStringICNC(ext, _T("jpg")) == 0) || (CompareStringICNC(ext, _T("jpeg")) == 0))
          hr = pImageFactory->CreateEncoder(GUID_ContainerFormatJpeg, NULL, &piEncoder);
        else if (CompareStringICNC(ext, _T("tiff")) == 0)
        {
          hr = pImageFactory->CreateEncoder(GUID_ContainerFormatTiff, NULL, &piEncoder);
          isExtraDataEnable = TRUE;
        }
      }
      
      if (piEncoder == NULL)
      {
        hr = pImageFactory->CreateEncoder(GUID_ContainerFormatBmp, NULL, &piEncoder);
      }
    }

    // Initialize the encoder
    if (SUCCEEDED(hr))
    {
      hr = piEncoder->Initialize(piFileStream, WICBitmapEncoderNoCache);

      if (!isExtraDataEnable) // skip if not Tiff Encoder
      {
        pExtraData = NULL;
        len = 0;
      }

      StoreImageWithFrameEncoder(pImageFactory, piEncoder, pMetaData, pExtraData, len);

      piEncoder->Commit();
      piFileStream->Commit(STGC_DEFAULT);
    }

    if (piEncoder != NULL)
      piEncoder->Release();
    if (piFileStream != NULL)
      piFileStream->Release();
    pImageFactory->Release();
  }
  K_CoUninitialize();

#ifdef _DEBUG
  TCHAR msg[300] = _T("Save image : ");
  StringCchCat(msg, 300, filename);
  StringCchCat(msg, 300, (SUCCEEDED(hr) ? _T(" Ok") : _T(" Fail")));
  StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG | SYNC_MSG_DEBUG);
#endif
  return TRUE;
#endif
}

BOOL KBitmap::StoreImageWithFrameEncoder(IWICImagingFactory* pImageFactory, IWICBitmapEncoder *piEncoder, KVoidPtrArray* pMetaData, LPBYTE pExtraData, int len)
{
  IWICBitmapFrameEncode *piFrameEncode = NULL;
  WICPixelFormatGUID pixelFormat = { 0 };
  UINT count = 0;
  double dpiX = horzDPI;
  double dpiY = vertDPI;

  HRESULT hr = piEncoder->CreateNewFrame(&piFrameEncode, NULL);

  if (SUCCEEDED(hr))
  {
    IWICPalette* pPalette = NULL;

    // Initialize the encoder.
    hr = piFrameEncode->Initialize(NULL);

    hr = piFrameEncode->SetSize(width, height);
    hr = piFrameEncode->SetResolution(dpiX, dpiY);

    if (bitCount == 24)
      pixelFormat = GUID_WICPixelFormat24bppBGR;
    else if (bitCount == 8)
      pixelFormat = GUID_WICPixelFormat8bppIndexed;
    else if (bitCount == 4)
      pixelFormat = GUID_WICPixelFormat4bppIndexed;
    else if (bitCount == 1)
      pixelFormat = GUID_WICPixelFormat1bppIndexed;
    else if (bitCount == 32)
      pixelFormat = GUID_WICPixelFormat32bppPBGRA;

    hr = piFrameEncode->SetPixelFormat(&pixelFormat);

    if (bitCount <= 8)
    {
      pImageFactory->CreatePalette(&pPalette);

      WICColor* pPaletteColors = new WICColor[paletteNumber];
      for (int i = 0; i < paletteNumber; i++)
      {
        pPaletteColors[i] = RGB(palette[i].rgbRed, palette[i].rgbGreen, palette[i].rgbBlue);
      }

      pPalette->InitializeCustom(pPaletteColors, paletteNumber);
      delete[] pPaletteColors;
      piFrameEncode->SetPalette(pPalette);
    }

    if (pMetaData != NULL)
    {
      IWICMetadataQueryWriter* piFrameQWriter = NULL;
      HRESULT h = piFrameEncode->GetMetadataQueryWriter(&piFrameQWriter);
      if (SUCCEEDED(h))
      {
        StoreMetadataToWriter(piFrameQWriter, pMetaData);
      }
      else
      {
        TCHAR msg[] = _T("Failure on GetMetadataQueryWriter()");
        StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
      }
      if (piFrameQWriter)
        piFrameQWriter->Release();
    }
    if (pExtraData != NULL)
    {
      StoreExtraDataToWriter(piFrameEncode, pExtraData, len);
    }

    UINT cbBufferSize = lineBufferLength * abs(height);

#ifdef USE_TOPDOWN_KBITMAP
    LPBYTE buff = getTopDownImageBuffer();
    hr = piFrameEncode->WritePixels(abs(height), lineBufferLength, cbBufferSize, buff);
    delete[] buff;
#else
    hr = piFrameEncode->WritePixels(height, lineBufferLength, cbBufferSize, bitImage);
#endif

    // Commit the frame.
    if (SUCCEEDED(hr))
      hr = piFrameEncode->Commit();
    if (pPalette)
      pPalette->Release();
  }

  if (piFrameEncode)
    piFrameEncode->Release();
  return SUCCEEDED(hr);
}
#endif 

BOOL KBitmap::StoreImageWithFrameEncoder(IWICImagingFactory* pImageFactory, IWICBitmapFrameEncode* piFrameEncode)
{
  WICPixelFormatGUID pixelFormat = { 0 };

  // Initialize the encoder.
  HRESULT hr = piFrameEncode->Initialize(NULL);

  double dpiX = horzDPI;
  double dpiY = vertDPI;
  hr = piFrameEncode->SetSize(width, height);
  hr = piFrameEncode->SetResolution(dpiX, dpiY);

  if (bitCount == 24)
    pixelFormat = GUID_WICPixelFormat24bppBGR;
  else if (bitCount == 8)
    pixelFormat = GUID_WICPixelFormat8bppIndexed;
  else if (bitCount == 4)
    pixelFormat = GUID_WICPixelFormat4bppIndexed;
  else if (bitCount == 1)
    pixelFormat = GUID_WICPixelFormat1bppIndexed;
  else if (bitCount == 32)
    pixelFormat = GUID_WICPixelFormat32bppPBGRA;

  hr = piFrameEncode->SetPixelFormat(&pixelFormat);

  if (bitCount <= 8)
  {
    IWICPalette* pPalette = NULL;
    pImageFactory->CreatePalette(&pPalette);
    if (pPalette)
    {
      WICColor* pPaletteColors = new WICColor[paletteNumber];
      if (pPaletteColors != NULL)
      {
        for (int i = 0; i < paletteNumber; i++)
        {
          pPaletteColors[i] = RGB(palette[i].rgbRed, palette[i].rgbGreen, palette[i].rgbBlue);
        }

        pPalette->InitializeCustom(pPaletteColors, paletteNumber);
        delete[] pPaletteColors;
        piFrameEncode->SetPalette(pPalette);
      }
      pPalette->Release();
    }
  }

  UINT cbBufferSize = lineBufferLength * abs(height);

#ifdef USE_TOPDOWN_KBITMAP
  LPBYTE buff = getTopDownImageBuffer();
  hr = piFrameEncode->WritePixels(abs(height), lineBufferLength, cbBufferSize, buff);
  delete[] buff;
#else
  hr = piFrameEncode->WritePixels(height, lineBufferLength, cbBufferSize, bitImage);
#endif

  return SUCCEEDED(hr);
}

HBITMAP KBitmap::CompatibleBitmap()
{
  HDC hdc = GetDC(GetDesktopWindow());
  HDC dstDC;
  HDC srcDC;
  HANDLE olddst;
  HANDLE oldsrc;

  dstDC = CreateCompatibleDC(hdc);
  srcDC = CreateCompatibleDC(hdc);
  HBITMAP hbm = ::CreateCompatibleBitmap(dstDC, width, height);
  
  olddst = SelectObject(dstDC, hbm);
  oldsrc = SelectObject(srcDC, handle());

  BitBlt(dstDC, 0, 0, width, height, srcDC, 0, 0, SRCCOPY);

  SelectObject(dstDC, olddst);
  SelectObject(srcDC, oldsrc);

  DeleteDC(dstDC);
  DeleteDC(srcDC);

  ReleaseDC(GetDesktopWindow(), hdc);
  return hbm;
}

HANDLE KBitmap::GetDIB()
{
  HANDLE h;
  LPBYTE buff;
  if (bitCount == 32)
  {
    int headersize = sizeof(BITMAPV5HEADER);
    int buffsize = headersize + lineBufferLength * height;
    h = GlobalAlloc(GHND | GMEM_SHARE, buffsize);
    buff = (LPBYTE)GlobalLock(h);
    BITMAPV5HEADER* bih = (BITMAPV5HEADER*)buff;

    memset(buff, 0, sizeof(BITMAPV5HEADER));

    bih->bV5Size = headersize;// sizeof(BITMAPINFOHEADER);
    bih->bV5Width = width;
    bih->bV5Height = -height;
    bih->bV5Planes = 1;
    bih->bV5BitCount = bitCount;
    bih->bV5Compression = BI_BITFIELDS;
    bih->bV5SizeImage = lineBufferLength * height;
    bih->bV5XPelsPerMeter = 2834;
    bih->bV5YPelsPerMeter = 2834;
    bih->bV5RedMask = 0x00FF0000;
    bih->bV5GreenMask = 0x0000FF00;
    bih->bV5BlueMask = 0x000000FF;
    bih->bV5AlphaMask = 0xFF000000;
    buff += headersize;
  }
  else
  {
    int headersize = sizeof(BITMAPINFOHEADER);
    int buffsize = headersize + lineBufferLength * height + paletteNumber * sizeof(RGBQUAD);
    h = GlobalAlloc(GHND | GMEM_SHARE, buffsize);
    buff = (LPBYTE)GlobalLock(h);
    BITMAPINFOHEADER* bih = (BITMAPINFOHEADER*)buff;

    memset(buff, 0, sizeof(BITMAPINFOHEADER));

    bih->biSize = headersize;// sizeof(BITMAPINFOHEADER);
    bih->biWidth = width;
    bih->biHeight = -height;
    bih->biPlanes = 1;
    bih->biBitCount = bitCount;
    bih->biCompression = BI_RGB;
    bih->biSizeImage = lineBufferLength * height;
    bih->biXPelsPerMeter = 2834;
    bih->biYPelsPerMeter = 2834;
    buff += headersize;

    if (paletteNumber > 0)
    {
      memcpy(buff, palette, paletteNumber * sizeof(RGBQUAD));
      buff += paletteNumber * sizeof(RGBQUAD);
    }

  }

  memcpy(buff, bitImage, lineBufferLength * height);
  GlobalUnlock(h);
  return h;
}

void KBitmap::AlphaBlend(CDC& dc, int xp, int yp, int alpha)
{
  if (hbitmap == NULL)
    return;

  CDC memdc;
  memdc.CreateCompatibleDC(&dc);
  HBITMAP old_bitmap = (HBITMAP)memdc.SelectObject(hbitmap);

  BLENDFUNCTION bf;
  bf.BlendOp = AC_SRC_OVER;
  bf.BlendFlags = 0;
  bf.SourceConstantAlpha = (BYTE)alpha;
  bf.AlphaFormat = AC_SRC_ALPHA;

  ::AlphaBlend(dc, xp, yp, width, height, memdc, 0, 0, width, height, bf);

  memdc.SelectObject(old_bitmap);
  memdc.DeleteDC();
}

void KBitmap::AlphaBlend(CDC& dc, RECT& rect, RECT& src, int alpha)
{
  if (hbitmap == NULL)
    return;

  CDC memdc;
  memdc.CreateCompatibleDC(&dc);
  HBITMAP old_bitmap = (HBITMAP)memdc.SelectObject(hbitmap);

  BLENDFUNCTION bf;
  bf.BlendOp = AC_SRC_OVER;
  bf.BlendFlags = 0;
  bf.SourceConstantAlpha = (BYTE)alpha;
  bf.AlphaFormat = AC_SRC_ALPHA;

  ::AlphaBlend(dc, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top,
    memdc, src.left, src.top, src.right - src.left, src.bottom - src.top, bf);

  memdc.SelectObject(old_bitmap);
  memdc.DeleteDC();
}

void KBitmap::Blit(CDC& dc, RECT& rect, float zoom, RECT& src, BLIT_MODE mode)
{
  if (hbitmap == NULL)
    return;

  CDC memdc;
  memdc.CreateCompatibleDC(&dc);
  HBITMAP old_bitmap = (HBITMAP)memdc.SelectObject(hbitmap);

  if (zoom == 1.0)
  {
    dc.BitBlt(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top,
      &memdc, src.left, src.top, SRCCOPY);
  }
  else
  {
    dc.SetStretchBltMode(COLORONCOLOR);
    dc.StretchBlt(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top,
      &memdc, src.left, src.top, src.right - src.left, src.bottom - src.top, SRCCOPY);
  }
  memdc.SelectObject(old_bitmap);
  memdc.DeleteDC();
}

void KBitmap::Blit(CDC& dc, RECT& rect, BLIT_MODE mode)
{
  if (hbitmap == NULL)
    return;

  CDC memdc;
  memdc.CreateCompatibleDC(&dc);
  HBITMAP old_bitmap = (HBITMAP)memdc.SelectObject(hbitmap);

  if (mode == BLIT_FITTO)
  {
    float xr = (float)(rect.right - rect.left) / width;
    float yr = (float)(rect.bottom - rect.top) / height;
    if (xr < yr)
      yr = xr;

    int dw = (int)(yr * width);
    int dh = (int)(yr * height);
    if (dw != (rect.right - rect.left))
    {
      rect.left += (rect.right - rect.left - dw) / 2;
      rect.right = rect.left + dw;
    }
    if (dh != (rect.bottom - rect.top))
    {
      rect.top += (rect.bottom - rect.top - dh) / 2;
      rect.bottom = rect.top + dh;
    }
  }
  else if (mode == BLIT_AS_CENTER)
  {
    int dw = width;
    int dh = height;
    if (dw != (rect.right - rect.left))
    {
      rect.left += (rect.right - rect.left - dw) / 2;
      rect.right = rect.left + dw;
    }
    if (dh != (rect.bottom - rect.top))
    {
      rect.top += (rect.bottom - rect.top - dh) / 2;
      rect.bottom = rect.top + dh;
    }
  }

  if ((width == (rect.right - rect.left)) && (height == (rect.bottom - rect.top)))
  {
    dc.BitBlt(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top,
      &memdc, 0, 0, SRCCOPY);
  }
  else
  {
    dc.SetStretchBltMode(COLORONCOLOR);
    dc.StretchBlt(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top,
      &memdc, 0, 0, width, height, SRCCOPY);
  }
  memdc.SelectObject(old_bitmap);
  memdc.DeleteDC();
}

static BYTE bit_mask_1b_array[] = { 0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01 };
static BYTE bit_shift_1b_array[] = { 7, 6, 5, 4, 3, 2, 1, 0 };
static BYTE bit_mask_4b_array[] = { 0xC0, 0x30, 0x0C, 0x03 };
static BYTE bit_shift_4b_array[] = { 6, 4, 2, 0 };


static BYTE bit_mask[8] = { 0x80, 0x40,0x20,0x10,0x08,0x04, 0x02, 0x01 };
static BYTE valid_bit_mask[8] = { 0x80, 0xC0,0xE0,0xF0,0xF8,0xFC, 0xFE, 0xFF };

/*
 * channel 0x01=Red, 0x02=Green, 0x04=Blue, 0x07=all
*/
void KBitmap::GetHistogram(BYTE* histogram, int channel)
{
  UINT* pValues = new UINT[256];
  memset(histogram, 0, 256);

  if (pValues != NULL)
  {
    memset(pValues, 0, sizeof(int) * 256);
    BYTE bit_mask = 0xFF;
    short bit_shift = 0;

    for (int y = 0; y < height; y++)
    {
      LPBYTE s = _GetScanline(y);
      BYTE p;

      for (int x = 0; x < width; x++)
      {
        if (bpp < 3)
        {
          if (bitCount == 1)
          {
            bit_mask = bit_mask_1b_array[x % 0x07];
            bit_shift = bit_shift_1b_array[x % 0x07];
          }
          else if (bitCount == 4)
          {
            bit_mask = bit_mask_4b_array[x % 0x03];
            bit_shift = bit_shift_4b_array[x % 0x03];
          }

          p = (*s & bit_mask);
          if (bit_shift > 0)
            p = p >> bit_shift;

          if (bitCount == 1)
          {
            if ((x % 0x07) == 0x07)
              s++;
          }
          else if (bitCount == 4)
          {
            if ((x % 0x03) == 0x03)
              s++;
          }
          else
          {
            s++;
          }

          if (p < paletteNumber)
          {
            BYTE v;
            if (channel == 0x01)
              v = palette[p].rgbRed;
            else if (channel == 0x02)
              v = palette[p].rgbGreen;
            else if (channel == 0x04)
              v = palette[p].rgbBlue;
            else // if (channel == 0x07)
              v = (BYTE)(((int)palette[p].rgbRed + (int)palette[p].rgbGreen + (int)palette[p].rgbBlue) / 3);

            if (pValues[v] < 0xFFFFFFFF)
              pValues[v]++;
          }
        }
        else
        {
          if (bpp == 4)
            s++; // alpha

          BYTE v;
          if (channel == 0x01)
            v = s[2];
          else if (channel == 0x02)
            v = s[1];
          else if (channel == 0x04)
            v = s[0];
          else // if (channel == 0x07)
            v = (BYTE)(((int)s[0] + (int)s[1] + (int)s[2]) / 3);

          if (pValues[v] < 0xFFFFFFFF)
            pValues[v]++;
          s += 3;
        }
      }
    }

    // find max
    UINT max = 0;
    for (int x = 0; x < 256; x++)
    {
      if (max < pValues[x])
        max = pValues[x];
    }

    for (int x = 0; x < 256; x++)
    {
      BYTE v = (int)((double)255 * pValues[x] / max) & 0x0FF;
      histogram[x] = v;
    }
    delete[] pValues;
  }
}

void KBitmap::Flip(FLIP_DIRECTION dir)
{
  if ((bitImage == NULL) || (lineBufferLength == 0))
  return;

  BYTE* s;
  BYTE* d;
  if (dir == FLIP_VERTICAL)
  {
    LPBYTE t = new BYTE[lineBufferLength];

    for (int y = 0; y < height / 2; y++)
    {
      int dy = height - y - 1;
      if (y != dy)
      {
        s = _GetScanline(y);
        d = _GetScanline(dy);

        memcpy(t, s, lineBufferLength);
        memcpy(s, d, lineBufferLength);
        memcpy(d, t, lineBufferLength);
      }
    }
    delete[] t;
  }
  else if (dir == FLIP_HORIZONTAL)
  {
    BYTE t[4];
    for (int y = 0; y < height; y++)
    {
      s = _GetScanline(y);
      d = s + bpp * (width - 1);
      for (int x = 0; x < width / 2; x++)
      {
        memcpy(t, s, bpp);
        memcpy(s, d, bpp);
        memcpy(d, t, bpp);
        s += bpp;
        d -= bpp;
      }
    }
  }
}

BOOL KBitmap::FlipStep(FLIP_DIRECTION dir, int& y, LPBYTE t)
{
  BYTE* s;
  BYTE* d;

  if (dir == FLIP_VERTICAL)
  {
    if (y < height / 2)
    {
      int dy = height - y - 1;
      if (y != dy)
      {
        s = _GetScanline(y);
        d = _GetScanline(dy);

        memcpy(t, s, lineBufferLength);
        memcpy(s, d, lineBufferLength);
        memcpy(d, t, lineBufferLength);
      }
      y++;
      return (y * 100 / (height / 2));
    }
    else
    {
      return DONE_PROGRESS_STEP;
    }
  }
  else if (dir == FLIP_HORIZONTAL)
  {
    BYTE t[4];
    if (y < height)
    {
      s = _GetScanline(y);
      d = s + bpp * (width - 1);
      for (int x = 0; x < width / 2; x++)
      {
        memcpy(t, s, bpp);
        memcpy(s, d, bpp);
        memcpy(d, t, bpp);
        s += bpp;
        d -= bpp;
      }
      y++;
      return (100 * y / height);
    }
    return DONE_PROGRESS_STEP;
  }
  return -1;
}

void KBitmap::Rotate(RotateMode m)
{
  if (m == RotateMode::ROTATE_90CW) // ROTATE_90CCW B0-2A-43-86-6D-32
  {
    KBitmap bm(height, width, bitCount, (bpp == 4), palette, paletteNumber);

    if (bitCount >= 8)
    {
      for (int y = 0; y < height; y++)
      {
        LPBYTE s = _GetScanline(y);
        LPBYTE d = bm._GetScanline(0);

        d += (height - 1 - y) * bpp;
        for (int x = 0; x < width; x++)
        {
          memcpy(d, s, bpp);
          s += bpp;
          d += bm.lineBufferLength;
        }
      }
    }
    else
    {
      for (int y = 0; y < bm.height; y++)
      {
        LPBYTE d = bm._GetScanline(y);
        for (int x = 0; x < height; x++)
        {
          if ((x & 0x07) == 0)
            *d = 0;

          LPBYTE s = _GetScanline(x);
          s += (width - y - 1) / 8;
          int bit_offset = (width - y - 1) & 0x007;

          if (*s & bit_mask[bit_offset])
            *d |= bit_mask[x & 0x07];

          if ((x & 0x07) == 0)
            d++;
        }
      }
    }
    _swap(bm);
  }
  else if (m == RotateMode::ROTATE_90CCW)
  {
    KBitmap bm(height, width, bitCount, (bpp == 4), palette, paletteNumber);

    if (bitCount >= 8)
    {
      for (int y = 0; y < height; y++)
      {
        LPBYTE s = _GetScanline(y);
        LPBYTE d = bm._GetScanline(bm.height - 1);
        d += y * bpp;
        for (int x = 0; x < width; x++)
        {
          memcpy(d, s, bpp);
          s += bpp;
          d -= bm.lineBufferLength;
        }
      }
    }
    else
    {
      for (int y = 0; y < bm.height; y++)
      {
        LPBYTE d = bm._GetScanline(y);
        for (int x = 0; x < height; x++)
        {
          if ((x & 0x07) == 0)
            *d = 0;

          LPBYTE s = _GetScanline(x);
          s += y / 8;
          int bit_offset =(y & 0x007);

          if (*s & bit_mask[bit_offset])
            *d |= bit_mask[x & 0x07];

          if ((x & 0x07) == 0)
            d++;
        }
      }
    }
    _swap(bm);
  }
  else if (m == RotateMode::ROTATE_180)
  {
    KBitmap bm(width, height, bitCount, (bpp == 4), palette, paletteNumber);
    for (int y = 0; y < height; y++)
    {
      LPBYTE s = _GetScanline(y);
      LPBYTE d = bm._GetScanline(height - 1 - y);
      d += (width - 1) * bpp;

      for (int x = 0; x < width; x++)
      {
        memcpy(d, s, bpp);
        s += bpp;
        d -= bpp;
      }
    }
    _swap(bm);
  }
}

int KBitmap::RotateStep(RotateMode m, int& y, KBitmap* bm)
{
  if (m == RotateMode::ROTATE_90CW) // ROTATE_90CCW B0-2A-43-86-6D-32
  {
    if (bitCount >= 8)
    {
      if (y < height)
      {
        LPBYTE s = _GetScanline(y);
        LPBYTE d = bm->_GetScanline(0);

        d += (height - 1 - y) * bpp;
        for (int x = 0; x < width; x++)
        {
          memcpy(d, s, bpp);
          s += bpp;
          d += bm->lineBufferLength;
        }
        y++;
      }
      if (y == height)
      {
        _swap(*bm);
        return DONE_PROGRESS_STEP;
      }
      return(y * 100 / height);
    }
    else
    {
      if (y < bm->height)
      {
        LPBYTE d = bm->_GetScanline(y);
        for (int x = 0; x < height; x++)
        {
          if ((x & 0x07) == 0)
            *d = 0;

          LPBYTE s = _GetScanline(x);
          s += (width - y - 1) / 8;
          int bit_offset = (width - y - 1) & 0x007;

          if (*s & bit_mask[bit_offset])
            *d |= bit_mask[x & 0x07];

          if ((x & 0x07) == 0)
            d++;
        }
        y++;
      }
      if (y == bm->height)
      {
        _swap(*bm);
        return DONE_PROGRESS_STEP;
      }
      return(y * 100 / bm->height);
    }
  }
  else if (m == RotateMode::ROTATE_90CCW)
  {
    //KBitmap bm(height, width, bitCount, (bpp == 4), palette, paletteNumber);

    if (bitCount >= 8)
    {
      if (y < height)
      {
        LPBYTE s = _GetScanline(y);
        LPBYTE d = bm->_GetScanline(bm->height - 1);
        d += y * bpp;
        for (int x = 0; x < width; x++)
        {
          memcpy(d, s, bpp);
          s += bpp;
          d -= bm->lineBufferLength;
        }
        y++;
      }
      if (y == height)
      {
        _swap(*bm);
        return DONE_PROGRESS_STEP;
      }
      return(y * 100 / height);
    }
    else
    {
      if (y < bm->height)
      {
        LPBYTE d = bm->_GetScanline(y);
        for (int x = 0; x < height; x++)
        {
          if ((x & 0x07) == 0)
            *d = 0;

          LPBYTE s = _GetScanline(x);
          s += y / 8;
          int bit_offset = (y & 0x007);

          if (*s & bit_mask[bit_offset])
            *d |= bit_mask[x & 0x07];

          if ((x & 0x07) == 0)
            d++;
        }
        y++;
      }
      if (y == bm->height)
      {
        _swap(*bm);
        return DONE_PROGRESS_STEP;
      }
      return(y * 100 / height);
    }
  }
  else if (m == RotateMode::ROTATE_180)
  {
    // KBitmap bm(width, height, bitCount, (bpp == 4), palette, paletteNumber);
    if (y < height)
    {
      LPBYTE s = _GetScanline(y);
      LPBYTE d = bm->_GetScanline(height - 1 - y);
      d += (width - 1) * bpp;

      for (int x = 0; x < width; x++)
      {
        memcpy(d, s, bpp);
        s += bpp;
        d -= bpp;
      }
      y++;
    }
    if (y == height)
    {
      _swap(*bm);
      return DONE_PROGRESS_STEP;
    }
    return(y * 100 / height);
  }
  return -1;
}

void KBitmap::Crop(int asRatio, int left, int top, int right, int bottom)
{
  // check valid size
  if (asRatio)
  {
    left = (int)((float)width * left / 100 + 0.5);
    top = (int)((float)height * top / 100 + 0.5);
    right = width - (int)((float)width * right / 100 + 0.5);
    bottom = height - (int)((float)height * bottom / 100 + 0.5);
  }
  else
  {
    right = width - right;
    bottom = height - bottom;
  }

  if (left < 0)
    left = 0;
  if (top < 0)
    top = 0;
  if (right > width)
    right = width;
  if (bottom > height)
    bottom = height;
  if (left >= right)
    right = left + 1;
  if (top >= bottom)
    bottom = top + 1;

  if ((left >= width) || (top >= height))
    return;
  if ((left == 0) && (right == width) && (top == 0) && (bottom == height))
    return;

  Crop(left, top, right, bottom);
}

static void bitShiftCopy(LPBYTE d, LPBYTE s, int shift_bit, int copyLength, BYTE last_mask)
{
  BYTE mask_off_head[8] = { 0, 0xFE, 0xFC, 0xF8, 0xF0, 0xE0, 0xC0, 0x80 };
  BYTE next_shift_value[8] = { 0, 7, 6, 5, 0xF0, 0xE0, 0xC0, 0x80 };
  BYTE p_value = 0;
  BYTE mask = mask_off_head[shift_bit];
  BYTE next_shift = 8 - shift_bit;
  BYTE db;
  for (int x = 0; x < copyLength; x++)
  {
    db = ((*s << shift_bit) & mask);
    s++;
    db |= (*s >> next_shift) & ~mask;

    if (x == (copyLength - 1))
      *d = (db & last_mask) | (*d & ~last_mask);
    else
      *d = db;
    d++;
  }
}

void KBitmap::copyBlock(KBitmap& bm, int left, int top, int right, int bottom)
{
  LPBYTE s, d;
  int y;

  int copyLength = (bm.width * bitCount + 7) / 8;
  int shift_bit = (left * bitCount & 0x007);
  BYTE last_mask = valid_bit_mask[(bm.width * bitCount) & 0x007];

#ifndef USE_TOPDOWN_KBITMAP
  top = height - bottom;
#endif

  for (y = 0; y < bm.height; y++)
  {
    s = _pixelBuffer(left, y + top);
    d = bm._pixelBuffer(0, y);

    if ((bitCount > 8) || (shift_bit == 0))
      memcpy(d, s, copyLength);
    else
      bitShiftCopy(d, s, shift_bit, copyLength, last_mask);
  }
}

// static BYTE valid_bit_mask[8] = {0xFF, 0x80, 0xC0,0xE0,0xF0,0xF8,0xFC, 0xFE };

static void bitShiftCopyFrom(LPBYTE d, LPBYTE s, int shift_bit, int copyLength, BYTE last_mask)
{
  BYTE shift_off_head[8] = { 0xFF, 0x7F, 0x3F, 0x1F, 0x0F, 0x07, 0x03, 0x01 };
  //BYTE next_shift_value[8] = { 0, 7, 6, 5, 0xF0, 0xE0, 0xC0, 0x80 };
  BYTE p_value = 0;
  BYTE mask = shift_off_head[shift_bit];
  BYTE next_shift = 8 - shift_bit;
  BYTE db;
  BYTE next_bit = 0;

  for (int x = 0; x < copyLength; x++)
  {
    if (shift_bit > 0)
      db = ((*s >> shift_bit) & mask) | next_bit;
    else
      db = *s;

    if (shift_bit > 0)
      next_bit = (*s << next_shift) & ~mask;

    BYTE valid_mask = 0xFF;
    if (x == 0)
      valid_mask &= mask;
    if (x == (copyLength - 1))
      valid_mask &= last_mask;

    if (valid_mask == 0xFF)
      *d = db;
    else
      *d = (*d & ~valid_mask) | (db & valid_mask);

    d++;
    s++;
  }
}

//static BYTE valid_bit_mask[8] = { 0x80, 0xC0,0xE0,0xF0,0xF8,0xFC, 0xFE, 0xFF };

void KBitmap::copyFromBlock(int xp, int yp, KBitmap& bm, int sx, int sy, int w, int h)
{
  LPBYTE s, d;
  int yo = 0;
  int copy_width = bm.width;
  if ((xp + bm.width) > width)
    copy_width -= (xp + bm.width) - width;
  int copyLength = (xp + w - 1) * bitCount / 8 - xp * bitCount / 8 + 1;
  int shift_bit = (xp * bitCount & 0x007);
  short last_bit = ((xp + copy_width) * bitCount) & 0x007;
  BYTE last_mask = 0xFF;
  if (last_bit > 0)
    last_mask = valid_bit_mask[last_bit - 1];
  if ((xp + copy_width) >= width)
    last_mask = 0xFF;

  int copy_height = h;// abs(bm.height);
  if (yp + copy_height > height)
  {
    yo = yp + copy_height - height;
    copy_height -= yo;
  }
#ifndef USE_TOPDOWN_KBITMAP
  yp = height - (yp + copy_height);
#endif

  for (int y = 0 ; y < copy_height; y++)
  {
    s = bm._pixelBuffer(0, yo + y);
    d = _pixelBuffer(xp, yp + y);

    if (bitCount > 8) // || (shift_bit == 0))
      memcpy(d, s, copyLength);
    else
      bitShiftCopyFrom(d, s, shift_bit, copyLength, last_mask);
  }
}

void KBitmap::Crop(int left, int top, int right, int bottom)
{
  KBitmap bm(right - left, bottom - top, bitCount, (bitCount >= 32), palette, paletteNumber);
  copyBlock(bm, left, top, right, bottom);
  _swap(bm);
}

void KBitmap::Resize(int byRatio, int w, int h)
{
  if (byRatio)
  {
    w = width * w / 100;
    h = height * h / 100;
  }
  resize(w, h, 0);
}

void KBitmap::Reverse(ReverseMode mode)
{
  if (mode == ReverseMode::REVERSE_NONE)
    return;
  /*
  if ((reverseMode >= 2) && (bpp < 3)) // only 24-bit or more
    return;
    */
  if (bitCount <= 8)
  {
    // change palettes
    if (palette != NULL)
    {
      for (int x = 0; x < paletteNumber; x++)
      {
        if (mode == ReverseMode::REVERSE_VALUE) // Value reverse
        {
          RGB_C rgb;
          rgb.Blu = palette[x].rgbBlue;
          rgb.Grn = palette[x].rgbGreen;
          rgb.Red = palette[x].rgbRed;
          HSV_C hsv = ConvertRGB2HSV(rgb);
          hsv.Val = (100 - hsv.Val);
          rgb = ConvertHSV2RGB(hsv);
          palette[x].rgbBlue = rgb.Blu;
          palette[x].rgbGreen = rgb.Grn;
          palette[x].rgbRed = rgb.Red;
        }
        else if (mode == ReverseMode::REVERSE_LIGHTNESS) // Lightness reverse
        {
          RGB_C rgb;
          rgb.Blu = palette[x].rgbBlue;
          rgb.Grn = palette[x].rgbGreen;
          rgb.Red = palette[x].rgbRed;
          HLS_C hls = ConvertRGB2HLS(rgb);
          hls.Lit = (100 - hls.Lit);
          rgb = ConvertHLS2RGB(hls);
          palette[x].rgbBlue = rgb.Blu;
          palette[x].rgbGreen = rgb.Grn;
          palette[x].rgbRed = rgb.Red;
        }
        else if (mode == ReverseMode::REVERSE_RGB) // rgb reverse
        {
          palette[x].rgbBlue = 255 - palette[x].rgbBlue;
          palette[x].rgbGreen = 255 - palette[x].rgbGreen;
          palette[x].rgbRed = 255 - palette[x].rgbRed;
        }
      }
    }

    // we have to create bitmap newly
    /*
    KBitmap bm;
    bm.CreateBitmap(width, height, bitCount, FALSE, palette, paletteNumber, bitImage);
    _swap(bm);
    */
    RefreshBitmapPalette();
    return;
  }

  for (int y = 0; y < height; y++)
  {
    LPBYTE s = _GetScanline(y);
    for (int x = 0; x < width; x++)
    {
      if (bpp == 4)
        s++; // alpha

      if (mode == ReverseMode::REVERSE_VALUE) // Value reverse
      {
        RGB_C rgb;
        rgb.Blu = s[0];
        rgb.Grn = s[1];
        rgb.Red = s[2];
        HSV_C hsv =ConvertRGB2HSV(rgb);
        hsv.Val = (100 - hsv.Val);
        rgb = ConvertHSV2RGB(hsv);
        *s++ = rgb.Blu;
        *s++ = rgb.Grn;
        *s++ = rgb.Red;
      }
      else if (mode == ReverseMode::REVERSE_LIGHTNESS) // Lightness reverse
      {
        RGB_C rgb;
        rgb.Blu = s[0];
        rgb.Grn = s[1];
        rgb.Red = s[2];
        HLS_C hls = ConvertRGB2HLS(rgb);
        hls.Lit = (100 - hls.Lit);
        rgb = ConvertHLS2RGB(hls);
        *s++ = rgb.Blu;
        *s++ = rgb.Grn;
        *s++ = rgb.Red;
      }
      else if (mode == ReverseMode::REVERSE_RGB) // rgb reverse
      {
        for (int p = 0; p < bpp; p++)
        {
          *s = (255 - *s);
          s++;
        }
      }
    }
  }
}

int KBitmap::ReverseStep(ReverseMode mode, int& y, LPBYTE buffer)
{
  if (mode == ReverseMode::REVERSE_NONE)
    return -1;

  if (bitCount <= 8)
  {
    // change palettes
    if (palette != NULL)
    {
      for (int x = 0; x < paletteNumber; x++)
      {
        if (mode == ReverseMode::REVERSE_VALUE) // Value reverse
        {
          RGB_C rgb;
          rgb.Blu = palette[x].rgbBlue;
          rgb.Grn = palette[x].rgbGreen;
          rgb.Red = palette[x].rgbRed;
          HSV_C hsv = ConvertRGB2HSV(rgb);
          hsv.Val = (100 - hsv.Val);
          rgb = ConvertHSV2RGB(hsv);
          palette[x].rgbBlue = rgb.Blu;
          palette[x].rgbGreen = rgb.Grn;
          palette[x].rgbRed = rgb.Red;
        }
        else if (mode == ReverseMode::REVERSE_LIGHTNESS) // Lightness reverse
        {
          RGB_C rgb;
          rgb.Blu = palette[x].rgbBlue;
          rgb.Grn = palette[x].rgbGreen;
          rgb.Red = palette[x].rgbRed;
          HLS_C hls = ConvertRGB2HLS(rgb);
          hls.Lit = (100 - hls.Lit);
          rgb = ConvertHLS2RGB(hls);
          palette[x].rgbBlue = rgb.Blu;
          palette[x].rgbGreen = rgb.Grn;
          palette[x].rgbRed = rgb.Red;
        }
        else if (mode == ReverseMode::REVERSE_RGB) // rgb reverse
        {
          palette[x].rgbBlue = 255 - palette[x].rgbBlue;
          palette[x].rgbGreen = 255 - palette[x].rgbGreen;
          palette[x].rgbRed = 255 - palette[x].rgbRed;
        }
      }
    }

    RefreshBitmapPalette();
    return DONE_PROGRESS_STEP;
  }

  if (y < height)
  {
    LPBYTE s = _GetScanline(y);
    for (int x = 0; x < width; x++)
    {
      if (bpp == 4)
        s++; // alpha

      if (mode == ReverseMode::REVERSE_VALUE) // Value reverse
      {
        RGB_C rgb;
        rgb.Blu = s[0];
        rgb.Grn = s[1];
        rgb.Red = s[2];
        HSV_C hsv = ConvertRGB2HSV(rgb);
        hsv.Val = (100 - hsv.Val);
        rgb = ConvertHSV2RGB(hsv);
        *s++ = rgb.Blu;
        *s++ = rgb.Grn;
        *s++ = rgb.Red;
      }
      else if (mode == ReverseMode::REVERSE_LIGHTNESS) // Lightness reverse
      {
        RGB_C rgb;
        rgb.Blu = s[0];
        rgb.Grn = s[1];
        rgb.Red = s[2];
        HLS_C hls = ConvertRGB2HLS(rgb);
        hls.Lit = (100 - hls.Lit);
        rgb = ConvertHLS2RGB(hls);
        *s++ = rgb.Blu;
        *s++ = rgb.Grn;
        *s++ = rgb.Red;
      }
      else if (mode == ReverseMode::REVERSE_RGB) // rgb reverse
      {
        for (int p = 0; p < bpp; p++)
        {
          *s = (255 - *s);
          s++;
        }
      }
    }
    y++;
  }
  if (y == height)
    return DONE_PROGRESS_STEP;
  return (y * 100 / height);
}

void KBitmap::converToColor(KBitmap& bm)
{
  BYTE bit_mask = 0xFF;
  short bit_shift = 0;
  for (int y = 0; y < height; y++)
  {
    LPBYTE s = _GetScanline(y);
    LPBYTE d = bm._GetScanline(y);
    BYTE c;
    BYTE dx = 1;

    for (int x = 0; x < width; x++)
    {
      if (bitCount == 1)
      {
        dx = x & 0x07;
        bit_mask = bit_mask_1b_array[dx];
        bit_shift = bit_shift_1b_array[dx];
      }
      else if (bitCount == 4)
      {
        dx = x & 0x03;
        bit_mask = bit_mask_4b_array[dx];
        bit_shift = bit_shift_4b_array[dx];
      }

      c = (*s & bit_mask);
      if (bit_shift > 0)
        c = c >> bit_shift;

      if (bitCount == 1)
      {
        if (dx == 0x07)
          s++;
      }
      else if (bitCount == 4)
      {
        if (dx == 0x03)
          s++;
      }
      else
      {
        s++;
      }

      if (bm.bpp == 4)
        *(d++) = 0xFF;
      if (c < paletteNumber)
      {
        *(d++) = palette[c].rgbBlue;
        *(d++) = palette[c].rgbGreen;
        *(d++) = palette[c].rgbRed;
      }
      else
      {
        memset(d, 0x00, 3);
        d += 3;
      }
    }
  }
}

void KBitmap::ConverToColor(BOOL withAlpha)
{
  KBitmap bm(width, height, (withAlpha ? 32 : 24), withAlpha);
  converToColor(bm);
  _swap(bm);
}

/* gamma : 10 ~ 500 */
void KBitmap::Monochrome(int asGrayscale, int gamma)
{
  if (bitCount == 1)
    return;

#if 1
  double dinvgamma = (double)gamma / 100;
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

  KBitmap bm(width, height, (asGrayscale ? 8 : 1), FALSE, palettes, pnumber);

  for (int y = 0; y < height; y++)
  {
    LPBYTE s = _GetScanline(y);
    LPBYTE d = bm._GetScanline(y);
    BYTE g = 0;

    *d = 0;
    for (int x = 0; x < width; x++)
    {
      if (bpp == 4)
        s++; // alpha

      // BGR format
      if (bpp >= 3)
      {
        g = (BYTE)((int)(0.299 * s[2]) + (int)(0.587 * s[1]) + (int)(0.114 * s[0]));
        s += 3;
      }
      else if (bpp == 1)
      {
        g = *s++;
        g = (BYTE)((int)(0.299 * palette[g].rgbRed) + (int)(0.587 * palette[g].rgbGreen) + (int)(0.114 * palette[g].rgbBlue));
      }

      if (gamma != 100)
      {
        g = (BYTE)max(0, min(255, (int)(pow((double)(unsigned int)g, dinvgamma) / dMax)));
      }

      if (asGrayscale)
      {
        *d = g;
        d++;
      }
      else
      {
        if (g >= 128)
        {
          *d |= bit_mask[x & 0x07];
        }

        if (((x & 0x07) == 0x07) && ((x + 1) < width))
        {
          d++;
          *d = 0;
        }
      }
    }
  }
  _swap(bm);
#else
  BYTE cTable[256];

  if (gamma < 10)
    gamma = 10;
  if (gamma > 500)
    gamma = 500;
  if (gamma != 100)
  {
    /**
    * \param gamma can be from 0.1 to 5.
    * \return true if everything is ok
    * \sa GammaRGB
    */

    double dinvgamma = (double)gamma / 100;
    double dMax = pow(255.0, dinvgamma) / 255.0;

    for (int i = 0; i<256; i++) {
      cTable[i] = (BYTE)max(0, min(255, (int)(pow((double)i, dinvgamma) / dMax)));
    }
  }

  for (int y = 0; y < height; y++)
  {
    LPBYTE s = _GetScanline(y);

    for (int x = 0; x < width; x++)
    {
      if (bpp == 4)
        s++; // alpha

      // BGR format
      BYTE g = (BYTE)((int)(0.299 * s[2]) + (int)(0.587 * s[1]) + (int)(0.114 * s[0]));
      if (gamma != 100)
        g = cTable[g];

      if (asGrayscale)
      {
        s[0] = g;
        s[1] = g;
        s[2] = g;
      }
      else
      {
        if (g >= 128)
        {
          s[0] = 0xFF;
          s[1] = 0xFF;
          s[2] = 0xFF;
        }
        else
        {
          s[0] = 0;
          s[1] = 0;
          s[2] = 0;
        }
      }
      s += 3;
    }
  }
#endif
}

BOOL KBitmap::MonochromeStep(int asGrayscale, int gamma, int& y, KBitmap* bm)
{
  if ((palette != NULL) && (bm->paletteNumber == paletteNumber))
  {
    if (gamma != 100)
    {
      double dinvgamma = (double)gamma / 100;
      double dMax = pow(255.0, dinvgamma) / 255.0;
      BYTE g;

      for (int x = 0; x < paletteNumber; x++)
      {
        g = (BYTE)((int)(0.299 * palette[x].rgbRed) + (int)(0.587 * palette[x].rgbGreen) + (int)(0.114 * palette[x].rgbBlue));
        if (gamma != 100)
        {
          g = (BYTE)max(0, min(255, (int)(pow((double)(unsigned int)g, dinvgamma) / dMax)));
        }

        palette[x].rgbBlue = g;
        palette[x].rgbGreen = g;
        palette[x].rgbRed = g;
      }

      RefreshBitmapPalette();
    }
    return DONE_PROGRESS_STEP;
  }

  if (y < height)
  {
    double dinvgamma = (double)gamma / 100;
    double dMax = pow(255.0, dinvgamma) / 255.0;

    LPBYTE s = _GetScanline(y);
    LPBYTE d = bm->_GetScanline(y);
    BYTE g;

    *d = 0;
    for (int x = 0; x < width; x++)
    {
      if (bpp == 4)
        s++; // alpha

      // BGR format
      if (bpp >= 3)
      {
        g = (BYTE)((int)(0.299 * s[2]) + (int)(0.587 * s[1]) + (int)(0.114 * s[0]));
        s += 3;
      }
      else if (bpp == 1)
      {
        g = *s++;
        g = (BYTE)((int)(0.299 * palette[g].rgbRed) + (int)(0.587 * palette[g].rgbGreen) + (int)(0.114 * palette[g].rgbBlue));
      }

      if (gamma != 100)
      {
        g = (BYTE)max(0, min(255, (int)(pow((double)(unsigned int)g, dinvgamma) / dMax)));
      }

      if (asGrayscale)
      {
        *d = g;
        d++;
      }
      else
      {
        if (g >= 128)
        {
          *d |= bit_mask[x & 0x07];
        }

        if (((x & 0x07) == 0x07) && ((x + 1) < width))
        {
          d++;
          *d = 0;
        }
      }
    }
    y++;
  }

  if (y == height)
  {
    _swap(*bm);
    return DONE_PROGRESS_STEP;
  }
  return (y * 100 / height);
}

/*


*/

void KBitmap::ColorLevel(int channel, BYTE* param)
{
  if ((bpp < 3) && (paletteNumber > 0))
  {
    for (int x = 0; x < paletteNumber; x++)
    {
      int cmask = 0x01;
      BYTE* s = &(palette[x].rgbBlue);

      for (int c = 0; c < 3; c++)
      {
        if (cmask & channel)
        {
          int p = c * 5;
          // input control
          if (s[0] <= param[p])
          {
            s[0] = 0;
          }
          else if (s[0] >= param[p + 2])
          {
            s[0] = 255;
          }
          else
          {
            if (s[0] < param[p + 1])
            {
              BYTE n = s[0] - param[p];
              BYTE d = param[p + 1] - param[p];
              s[0] = ((int)n * 128 / d) & 0x0FF;
            }
            else
            {
              BYTE n = s[0] - param[p + 1];
              BYTE d = param[p + 2] - param[p + 1];
              s[0] = ((int)n * 128 / d + 128) & 0x0FF;
            }
          }

          // output control
          BYTE n = param[p + 4] - param[p + 3];
          if (n != 255)
          {
            /*
            if (s[0] <= out_start)
              s[0] = 0;
            else if (s[0] < out_end)
              s[0] = (BYTE)((int)(s[0] - out_start) * 255 / n);
            else
              s[0] = 0xFF;
            */
            int v = s[0] * n / 255 + param[p + 3];
            if (v > 255)
              v = 255;
            s[0] = (BYTE)v;
          }
        }
        cmask = cmask << 1;
        if (c == 0)
          s = &(palette[x].rgbGreen);
        else if (c == 1)
          s = &(palette[x].rgbRed);
      }
    }
    RefreshBitmapPalette();
    return;
  }

  for (int y = 0; y < height; y++)
  {
    LPBYTE s = _GetScanline(y);

    for (int x = 0; x < width; x++)
    {
      if (bpp == 4)
        s++; // alpha

      // BGR format
      int cmask = 0x01;
      for (int c = 0; c < 3; c++)
      {
        if (cmask & channel)
        {
          int p = c * 5;
          // input control
          if (s[0] <= param[p])
          {
            s[0] = 0;
          }
          else if (s[0] >= param[p + 2])
          {
            s[0] = 255;
          }
          else
          {
            if (s[0] < param[p+1])
            {
              BYTE n = s[0] - param[p];
              BYTE d = param[p+1] - param[p];
              s[0] = ((int)n * 128 / d) & 0x0FF;
            }
            else
            {
              BYTE n = s[0] - param[p+1];
              BYTE d = param[p+2] - param[p+1];
              s[0] = ((int)n * 128 / d + 128) & 0x0FF;
            }
          }

          // output control
          BYTE n = param[p+4] - param[p+3];
          if (n != 255)
          {
            /*
            if (s[0] <= out_start)
              s[0] = 0;
            else if (s[0] < out_end)
              s[0] = (BYTE)((int)(s[0] - out_start) * 255 / n);
            else
              s[0] = 0xFF;
            */
            int v = s[0] * n / 255 + param[p+3];
            if (v > 255)
              v = 255;
            s[0] = (BYTE)v;
          }
        }
        cmask = cmask << 1;
        s++;
      }
    }
  }
}

BOOL KBitmap::ColorLevelStep(int channel, BYTE* param, int& y)
{
  if (bpp < 3)
  {
    for (int x = 0; x < paletteNumber; x++)
    {
      int cmask = 0x01;
      BYTE* s = &(palette[x].rgbBlue);

      for (int c = 0; c < 3; c++)
      {
        if (cmask & channel)
        {
          int p = c * 5;
          // input control
          if (s[0] <= param[p])
          {
            s[0] = 0;
          }
          else if (s[0] >= param[p + 2])
          {
            s[0] = 255;
          }
          else
          {
            if (s[0] < param[p + 1])
            {
              BYTE n = s[0] - param[p];
              BYTE d = param[p + 1] - param[p];
              s[0] = ((int)n * 128 / d) & 0x0FF;
            }
            else
            {
              BYTE n = s[0] - param[p + 1];
              BYTE d = param[p + 2] - param[p + 1];
              s[0] = ((int)n * 128 / d + 128) & 0x0FF;
            }
          }

          // output control
          BYTE n = param[p + 4] - param[p + 3];
          if (n != 255)
          {
            /*
            if (s[0] <= out_start)
              s[0] = 0;
            else if (s[0] < out_end)
              s[0] = (BYTE)((int)(s[0] - out_start) * 255 / n);
            else
              s[0] = 0xFF;
            */
            int v = s[0] * n / 255 + param[p + 3];
            if (v > 255)
              v = 255;
            s[0] = (BYTE)v;
          }
        }
        cmask = cmask << 1;
        if (c == 0)
          s = &(palette[x].rgbGreen);
        else if (c == 1)
          s = &(palette[x].rgbRed);
      }
    }
    RefreshBitmapPalette();
    return DONE_PROGRESS_STEP;
  }

  if (y < height)
  {
    LPBYTE s = _GetScanline(y);

    for (int x = 0; x < width; x++)
    {
      if (bpp == 4)
        s++; // alpha

      // BGR format
      int cmask = 0x01;
      for (int c = 0; c < 3; c++)
      {
        if (cmask & channel)
        {
          int p = c * 5;
          // input control
          if (s[0] <= param[p])
          {
            s[0] = 0;
          }
          else if (s[0] >= param[p + 2])
          {
            s[0] = 255;
          }
          else
          {
            if (s[0] < param[p + 1])
            {
              BYTE n = s[0] - param[p];
              BYTE d = param[p + 1] - param[p];
              s[0] = ((int)n * 128 / d) & 0x0FF;
            }
            else
            {
              BYTE n = s[0] - param[p + 1];
              BYTE d = param[p + 2] - param[p + 1];
              s[0] = ((int)n * 128 / d + 128) & 0x0FF;
            }
          }

          // output control
          BYTE n = param[p + 4] - param[p + 3];
          if (n != 255)
          {
            /*
            if (s[0] <= out_start)
              s[0] = 0;
            else if (s[0] < out_end)
              s[0] = (BYTE)((int)(s[0] - out_start) * 255 / n);
            else
              s[0] = 0xFF;
            */
            int v = s[0] * n / 255 + param[p + 3];
            if (v > 255)
              v = 255;
            s[0] = (BYTE)v;
          }
        }
        cmask = cmask << 1;
        s++;
      }
    }
    y++;
  }
  if (y == height)
    return DONE_PROGRESS_STEP;
  return (y * 100 / height);
}

/*
* red : -100 ~ 100
* green : -100 ~ 100
* blue : -100 ~ 100
*/
void KBitmap::ColorBalance(int redp, int greenp, int bluep)
{
  if (bpp < 1)
    return;
  if ((redp == 0) && (greenp == 0) && (bluep == 0))
    return;

  int bluev = 255 * bluep / 100;
  int greenv = 255 * greenp / 100;
  int redv = 255 * redp / 100;

  BYTE rTable[256];
  BYTE gTable[256];
  BYTE bTable[256];

  for (int i = 0; i < 256; i++)
  {
    rTable[i] = (BYTE)max(0, min(255, i + redv));
    gTable[i] = (BYTE)max(0, min(255, i + greenv));
    bTable[i] = (BYTE)max(0, min(255, i + bluev));
  }
  convertTable(rTable, gTable, bTable);
}

BOOL KBitmap::ColorBalanceStep(int redp, int greenp, int bluep, int& y, BYTE* rgbTable)
{
  if ((palette != NULL) && (paletteNumber > 0))
  {
    for (int i = 0; i < paletteNumber; i++)
    {
      palette[i].rgbRed = rgbTable[palette[i].rgbRed];
      palette[i].rgbGreen = rgbTable[256+palette[i].rgbGreen];
      palette[i].rgbBlue = rgbTable[512+palette[i].rgbBlue];
    }

    // create bitmap newly
    RefreshBitmapPalette();
    return DONE_PROGRESS_STEP;
  }
  else
  {
    if (y < height)
    {
      LPBYTE s = _GetScanline(y);

      for (int x = 0; x < width; x++)
      {
        if (bpp == 4)
          s++; // alpha

        // BGR format
        *s++ = rgbTable[512+*s];
        *s++ = rgbTable[256+*s];
        *s++ = rgbTable[*s];
      }
      y++;
    }

    if (y == height)
      return DONE_PROGRESS_STEP;
    return (y * 100 / height);
  }
}

/*
* bright : -100 ~ 100
* contrast : -100 ~ 100
*/
void KBitmap::ColorBrightContrast(int bright, int contrast)
{
  if (bpp < 1)
    return;
  if ((bright == 0) && (contrast == 0))
    return;

  float c = (100 + contrast) / 100.0f;
  bright = bright * 255 / 100 + 128;

  BYTE cTable[256];
  for (int i = 0; i<256; i++) {
    cTable[i] = (BYTE)max(0, min(255, (int)((i - 128)*c + bright + 0.5f)));
  }

  convertTable(cTable);
}

BOOL KBitmap::ColorBrightContrastStep(int bright, int contrast, int& y, BYTE* cTable)
{
  if ((palette != NULL) && (paletteNumber > 0))
  {
    for (int i = 0; i < paletteNumber; i++)
    {
      palette[i].rgbBlue = cTable[palette[i].rgbBlue];
      palette[i].rgbGreen = cTable[palette[i].rgbGreen];
      palette[i].rgbRed = cTable[palette[i].rgbRed];
    }

    // create bitmap newly
    RefreshBitmapPalette();
    return DONE_PROGRESS_STEP;
  }
  else
  {
    if (y < height)
    {
      LPBYTE s = _GetScanline(y);

      for (int x = 0; x < width; x++)
      {
        if (bpp == 4)
          s++; // alpha

        // BGR format
        for (int c = 0; c < 3; c++)
        {
          *s = cTable[*s];
          s++;
        }
      }
      y++;
    }
    if (y == height)
      return DONE_PROGRESS_STEP;
    return (y * 100 / height);
  }
}

void KBitmap::convertTable(BYTE* cTable)
{
  if ((palette != NULL) && (paletteNumber > 0))
  {
    for (int i = 0; i < paletteNumber; i++)
    {
      palette[i].rgbBlue = cTable[palette[i].rgbBlue];
      palette[i].rgbGreen = cTable[palette[i].rgbGreen];
      palette[i].rgbRed = cTable[palette[i].rgbRed];
    }

    // create bitmap newly
    RefreshBitmapPalette();
  }
  else
  {
    for (int y = 0; y < height; y++)
    {
      LPBYTE s = _GetScanline(y);

      for (int x = 0; x < width; x++)
      {
        if (bpp == 4)
          s++; // alpha

        // BGR format
        for (int c = 0; c < 3; c++)
        {
          *s = cTable[*s];
          s++;
        }
      }
    }
  }
}

void KBitmap::convertTable(BYTE* rTable, BYTE* gTable, BYTE* bTable)
{
  if ((palette != NULL) && (paletteNumber > 0))
  {
    for (int i = 0; i < paletteNumber; i++)
    {
      palette[i].rgbBlue = bTable[palette[i].rgbBlue];
      palette[i].rgbGreen = gTable[palette[i].rgbGreen];
      palette[i].rgbRed = rTable[palette[i].rgbRed];
    }

    // create bitmap newly
    RefreshBitmapPalette();
  }
  else
  {
    for (int y = 0; y < height; y++)
    {
      LPBYTE s = _GetScanline(y);

      for (int x = 0; x < width; x++)
      {
        if (bpp == 4)
          s++; // alpha

        // BGR format
        *s++ = bTable[*s];
        *s++ = gTable[*s];
        *s++ = rTable[*s];
      }
    }
  }
}

void KBitmap::ColorHLS(int hue, int light, int sat)
{
  if ((hue == 0) && (sat == 0) && (light == 0))
    return;
  if (bpp < 3)
  {
    for (int i = 0; i < paletteNumber; i++)
    {
      RGB_C rgb;
      rgb.Blu = palette[i].rgbBlue;
      rgb.Grn = palette[i].rgbGreen;
      rgb.Red = palette[i].rgbRed;

      HLS_C hls = ConvertRGB2HLS(rgb);
      hls.Hue = max(0, min(359, (hue + hls.Hue)));
      hls.Lit = max(0, min(100, (light + hls.Lit)));
      hls.Sat = max(0, min(100, (sat + hls.Sat)));
      rgb = ConvertHLS2RGB(hls);

      palette[i].rgbRed = rgb.Red;
      palette[i].rgbGreen = rgb.Grn;
      palette[i].rgbBlue = rgb.Blu;
    }
    RefreshBitmapPalette();
    return;
  }

  for (int y = 0; y < height; y++)
  {
    LPBYTE s = _GetScanline(y);

    for (int x = 0; x < width; x++)
    {
      if (bpp == 4)
        s++; // alpha

      RGB_C rgb;
      rgb.Blu = s[0];
      rgb.Grn = s[1];
      rgb.Red = s[2];
      HLS_C hls = ConvertRGB2HLS(rgb);
      hls.Hue = max(0, min(359, (hue + hls.Hue)));
      hls.Lit = max(0, min(100, (light + hls.Lit)));
      hls.Sat = max(0, min(100, (sat + hls.Sat)));
      rgb = ConvertHLS2RGB(hls);
      // BGR format
      *s++ = rgb.Blu;
      *s++ = rgb.Grn;
      *s++ = rgb.Red;
    }
  }
}


BOOL KBitmap::ColorHLSStep(int hue, int light, int sat, int& y)
{
  if ((hue == 0) && (sat == 0) && (light == 0))
    return DONE_PROGRESS_STEP;

  if (bpp < 3)
  {
    for (int i = 0; i < paletteNumber; i++)
    {
      RGB_C rgb;
      rgb.Blu = palette[i].rgbBlue;
      rgb.Grn = palette[i].rgbGreen;
      rgb.Red = palette[i].rgbRed;

      HLS_C hls = ConvertRGB2HLS(rgb);
      hls.Hue = max(0, min(359, (hue + hls.Hue)));
      hls.Lit = max(0, min(100, (light + hls.Lit)));
      hls.Sat = max(0, min(100, (sat + hls.Sat)));
      rgb = ConvertHLS2RGB(hls);

      palette[i].rgbRed = rgb.Red;
      palette[i].rgbGreen = rgb.Grn;
      palette[i].rgbBlue = rgb.Blu;
    }
    RefreshBitmapPalette();
    return DONE_PROGRESS_STEP;
  }

  if (y < height)
  {
    LPBYTE s = _GetScanline(y);

    for (int x = 0; x < width; x++)
    {
      if (bpp == 4)
        s++; // alpha

      RGB_C rgb;
      rgb.Blu = s[0];
      rgb.Grn = s[1];
      rgb.Red = s[2];
      HLS_C hls = ConvertRGB2HLS(rgb);
      hls.Hue = max(0, min(359, (hue + hls.Hue)));
      hls.Lit = max(0, min(100, (light + hls.Lit)));
      hls.Sat = max(0, min(100, (sat + hls.Sat)));
      rgb = ConvertHLS2RGB(hls);
      // BGR format
      *s++ = rgb.Blu;
      *s++ = rgb.Grn;
      *s++ = rgb.Red;
    }
    y++;
  }
  if (y == height)
    return DONE_PROGRESS_STEP;
  return (y * 100 / height);
}

void KBitmap::ColorHSV(int hue, int sat, int value)
{
  if (bpp < 3)
    return;
  if ((hue == 0) && (sat == 0) && (value == 0))
    return;
  LPBYTE end_of_buff = bitImage + height * lineBufferLength;
  for (int y = 0; y < height; y++)
  {
    LPBYTE s = _GetScanline(y);

    for (int x = 0; x < width; x++)
    {
      if (bpp == 4)
        s++; // alpha

      ASSERT((s+3) <= end_of_buff);
      RGB_C rgb;
      rgb.Blu = s[0];
      rgb.Grn = s[1];
      rgb.Red = s[2];
      HSV_C hsv = ConvertRGB2HSV(rgb); //  *((RGB_C *)s));
      hsv.Hue = max(0, min(359, (hue + hsv.Hue)));
      hsv.Sat = max(0, min(100, (sat + hsv.Sat)));
      hsv.Val = max(0, min(100, (value + hsv.Val)));
      rgb = ConvertHSV2RGB(hsv);
      // BGR format
      *s++ = rgb.Blu;
      *s++ = rgb.Grn;
      *s++ = rgb.Red;
    }
  }
}

BOOL KBitmap::ColorHSVStep(int hue, int sat, int value, int& y)
{
  if (y < height)
  {
    LPBYTE s = _GetScanline(y);

    for (int x = 0; x < width; x++)
    {
      if (bpp == 4)
        s++; // alpha

      RGB_C rgb;
      rgb.Blu = s[0];
      rgb.Grn = s[1];
      rgb.Red = s[2];
      HSV_C hsv = ConvertRGB2HSV(rgb); //  *((RGB_C *)s));
      hsv.Hue = max(0, min(359, (hue + hsv.Hue)));
      hsv.Sat = max(0, min(100, (sat + hsv.Sat)));
      hsv.Val = max(0, min(100, (value + hsv.Val)));
      rgb = ConvertHSV2RGB(hsv);
      // BGR format
      *s++ = rgb.Blu;
      *s++ = rgb.Grn;
      *s++ = rgb.Red;
    }
    y++;
  }
  if (y == height)
    return DONE_PROGRESS_STEP;
  return (y * 100 / height);
}

BOOL KBitmap::FillRectangle(RECT& rect, DWORD argb)
{
  if (rect.right > width)
    rect.right = width;
  if (rect.bottom > height)
    rect.bottom = height;
  if (rect.left < 0)
    rect.left = 0;
  if (rect.top < 0)
    rect.top = 0;
  if ((rect.left >= width) || (rect.bottom >= height) ||
    (rect.right <= 0) || (rect.bottom <= 0))
    return FALSE;

  BYTE a = (argb >> 24) & 0xFF;
  if (a == 0)
    return FALSE;
  BYTE r = (argb >> 16) & 0xFF;
  BYTE g = (argb >> 8) & 0xFF;
  BYTE b = argb & 0xFF;

  for (int y = rect.top; y <= rect.bottom; y++)
  {
    LPBYTE s = _GetScanline(y);
    s += (rect.left * bpp);
    for (int x = rect.left; x <= rect.right; x++)
    {
      if (bpp == 4)
        s++; // alpha

      if (a == 0xFF)
      {
        *s++ = b;
        *s++ = g;
        *s++ = r;
      }
      else
      {
        *s++ = (b * a + *s * (255 - a)) / 255;
        *s++ = (g * a + *s * (255 - a)) / 255;
        *s++ = (r * a + *s * (255 - a)) / 255;
      }
    }
  }
  return TRUE;
}

BOOL KBitmap::FillEllipse(RECT& ellipse, DWORD argb)
{
  RECT rect;
  memcpy(&rect, &ellipse, sizeof(RECT));

  if (rect.right > width)
    rect.right = width;
  if (rect.bottom > height)
    rect.bottom = height;
  if (rect.left < 0)
    rect.left = 0;
  if (rect.top < 0)
    rect.top = 0;
  if ((rect.left >= width) || (rect.bottom >= height) ||
    (rect.right <= 0) || (rect.bottom <= 0))
    return FALSE;

  BYTE a = (argb >> 24) & 0xFF;
  if (a == 0)
    return FALSE;
  BYTE r = (argb >> 16) & 0xFF;
  BYTE g = (argb >> 8) & 0xFF;
  BYTE b = argb & 0xFF;

  int cx = (ellipse.left + ellipse.right) / 2;
  int cy = (ellipse.top + ellipse.bottom) / 2;
  double a_sq = (ellipse.right - ellipse.left) / 2;
  double b_sq = (ellipse.bottom - ellipse.top) / 2;
  a_sq = a_sq * a_sq;
  b_sq = b_sq * b_sq;
  for (int y = rect.top; y <= rect.bottom; y++)
  {
    LPBYTE s = _GetScanline(y);
    s += (rect.left * bpp);
    for (int x = rect.left; x <= rect.right; x++)
    {
      // check inside of ellipse
      int rx = abs(x - cx);
      int ry = abs(y - cy);

      double cr = sqrt((double)rx * rx / a_sq + (double)ry * ry / b_sq);
      if (cr > 1.0)
      {
        s += bpp;
        continue;
      }

      if (bpp == 4)
        s++; // alpha

      if (a == 0xFF)
      {
        *s++ = b;
        *s++ = g;
        *s++ = r;
      }
      else
      {
        *s++ = (b * a + *s * (255 - a)) / 255;
        *s++ = (g * a + *s * (255 - a)) / 255;
        *s++ = (r * a + *s * (255 - a)) / 255;
      }
    }
  }
  return TRUE;
}

BOOL KBitmap::FillMask(RECT& rect, KBitmap& mask, DWORD argb)
{
  BYTE a = (argb >> 24) & 0xFF;
  if (a == 0)
    return FALSE;
  BYTE r = (argb >> 16) & 0xFF;
  BYTE g = (argb >> 8) & 0xFF;
  BYTE b = argb & 0xFF;

  int ys = (rect.top < 0) ? 0 : rect.top;
  int ye = (rect.bottom < height) ? rect.bottom : height;
  int oy = (rect.top < 0) ? abs(rect.top) : 0;
  int xs = (rect.left < 0) ? 0 : rect.left;
  int xe = (rect.right < width) ? rect.right : width;
  int ox = (rect.left < 0) ? abs(rect.left) : 0;

  for (int y = ys; y < ye; y++)
  {
    LPBYTE d = _GetScanline(y);
    LPBYTE s = mask._GetScanline(oy + y - ys);
    s += ox * mask.bpp;
    d += xs * bpp;
    for (int x = xs; x < xe; x++)
    {
      if (*s == 255)
      {
        if (bpp == 4)
          d++; // alpha

        if (a == 0xFF)
        {
          *d++ = b;
          *d++ = g;
          *d++ = r;
        }
        else
        {
          *d++ = (b * a + *d * (255 - a)) / 255;
          *d++ = (g * a + *d * (255 - a)) / 255;
          *d++ = (r * a + *d * (255 - a)) / 255;
        }
      }
      else
      {
        d += bpp;
      }

      s += mask.bpp;
    }
  }
  return TRUE;
}


static LPTSTR gStrPropertyNames[] = 
{
  _T("PModeImageView"),
  _T("PModeRotate"),
  _T("PModeResize"),
  _T("PModeCrop"),

  _T("PModeEditImage"),

  _T("PModeMonochrome"),
  _T("PModeColorBalance"),
  _T("PModeColorLevel"),
  _T("PModeColorBright"),
  _T("PModeColorHLS"),
  _T("PModeColorHSV"),
  _T("PModeColorReverse"),

  _T("PModeAnnotateSelect"),
  _T("PModeAnnotateLine"),
  _T("PModeAnnotateFreehand"),
  _T("PModeAnnotateRectangle"),
  _T("PModeAnnotateEllipse"),
  _T("PModeAnnotatePolygon"),
  _T("PModeAnnotateTextbox"),

  _T("PModeSelectRectangle"),
  _T("PModeSelectTriangle"),
  _T("PModeSelectRhombus"),
  _T("PModeSelectPentagon"),
  _T("PModeSelectStar"),
  _T("PModeSelectEllipse"),
  _T("PModeSelectFill"),
  _T("PModeCopyRectangle"),
  _T("PModeCopyTriangle"),
  _T("PModeCopyRhombus"),
  _T("PModeCopyPentagon"),
  _T("PModeCopyStar"),
  _T("PModeCopyEllipse"),
  _T("PModePasteImage"),

  _T("PModePasteShapeSmart"),
  _T("PModePasteShapeSmart"),
  _T("PModePasteShapeSmart"),
  _T("PModePasteShapeSmart"),
  _T("PModePasteShapeSmart"),

  _T("PModeMax")
};

LPTSTR GetEffectName(PropertyMode m)
{
  int str_count = sizeof(gStrPropertyNames) / sizeof(LPTSTR);
#ifdef _DEBUG
  ASSERT(str_count == (int)PropertyMode::PModeMax);
#endif

  if ((int)m < str_count) // PropertyMode::PModeMax)
    return gStrPropertyNames[(int)m];
  else
    return gStrPropertyNames[str_count - 1];
}


BOOL KBitmap::IsNeedRenderEffect(ImageEffectJob* effect)
{
  BOOL r = FALSE;

  if ((effect != NULL) && (effect->job > PropertyMode::PModeImageView))
  {
    switch (effect->job)
    {
    case PropertyMode::PModeRotate:
      r = (effect->param[0] > (int)RotateMode::ROTATE_NONE);
      break;

    case PropertyMode::PModeCrop:
      r = (effect->param[1] > 0) || (effect->param[2] > 0) || (effect->param[3] > 0) || (effect->param[4] > 0);
      break;

    case PropertyMode::PModeResize:
      if (effect->param[0] == 0)
        r = (effect->param[1] != width) || (effect->param[2] != height);
      else
        r = (effect->param[1] != 100) || (effect->param[2] != 100);
      break;

    case PropertyMode::PModeColorReverse:
      r = (effect->param[0] > 0);
      break;

    case PropertyMode::PModeMonochrome:
      r = (bitCount > 1); // (effect->param[0] == 0) ? (bitCount > 1) : (bitCount >= 8);
      break;

    case PropertyMode::PModeColorLevel:
      r = (bitCount > 1);
      break;

    case PropertyMode::PModeColorBalance:
      r = ((effect->param[0] != 0) || (effect->param[1] != 0) || (effect->param[2] != 0));
      break;

    case PropertyMode::PModeColorBright:
      r = ((effect->param[0] != 0) || (effect->param[1] != 0));
      break;

    case PropertyMode::PModeColorHLS:
      r = ((effect->param[0] != 0) || (effect->param[1] != 0) || (effect->param[2] != 0));
      break;

    case PropertyMode::PModeColorHSV:
      r = ((effect->param[0] != 0) || (effect->param[1] != 0) || (effect->param[2] != 0));
      break;
    }
  }
  return r;
}

LPTSTR effectRotateNames[] = {
  _T("None"),
  _T("ROTATE 90CW"),
  _T("ROTATE 90CCW"),
  _T("ROTATE 180"),
  _T("FLIP HORZ"),
  _T("FLIP VERT")
};

LPTSTR effectInvalid = _T("Invalid");

LPTSTR GetRotateName(RotateMode m)
{
  if ((RotateMode::ROTATE_NONE <= m) & (m <= RotateMode::FLIP_VERT))
    return effectRotateNames[(int)m];
  return effectInvalid;
}

LPTSTR effectReverseNames[] = {
  _T("None"),
  _T("Reverse RGB"),
  _T("Reverse Value"),
  _T("Reverse Lightness"),
};

LPTSTR GetReverseName(ReverseMode m)
{
  if ((ReverseMode::REVERSE_NONE <= m) && (m <= ReverseMode::REVERSE_LIGHTNESS))
    return effectReverseNames[(int)m];
  return effectInvalid;
}

LPTSTR effectBlockModeNames[] = {
  _T("Rectangle"),
  _T("Triangle"),
  _T("Rhombus"),
  _T("Pentagon"),
  _T("Star"),
  _T("Ellipse")
};

LPTSTR GetBlockModeName(PropertyMode mode)
{
  if ((PropertyMode::PModeSelectRectangle <= mode) && (mode <= PropertyMode::PModeSelectEllipse))
    return effectBlockModeNames[(int)mode - (int)PropertyMode::PModeSelectRectangle];
  return effectInvalid;
}

void KBitmap::GetEffectInfoString(ImageEffectJob* effect, LPTSTR str, int length)
{
  switch (effect->job)
  {
  case PropertyMode::PModeRotate:
    StringCchPrintf(str, length, _T("Rotate: %s"), GetRotateName((RotateMode)effect->param[0]));
    break;

  case PropertyMode::PModeCrop:
    StringCchPrintf(str, length, _T("Crop : abs=%d, ltrb(%d,%d,%d,%d)"), effect->param[0],
      effect->param[1], effect->param[2], effect->param[3], effect->param[4]);
    break;

  case PropertyMode::PModeResize:
    StringCchPrintf(str, length, _T("Resize : abs=%d, size(%d,%d)"), effect->param[0],
      effect->param[1], effect->param[2]);
    break;

  case PropertyMode::PModeColorReverse:
    StringCchPrintf(str, length, _T("Reverse : %s"), GetReverseName((ReverseMode)effect->param[0]));
    break;

  case PropertyMode::PModeMonochrome:
    if (effect->param[0] == 1)
      StringCchPrintf(str, length, _T("Monochrome : Grascale, g=%d"), effect->param[0]);
    else
      StringCchCopy(str, length, _T("Monochrome : B&W"));
    break;

  case PropertyMode::PModeColorLevel:
    StringCchPrintf(str, length, _T("ColorLevel : channel=%d"), effect->channel);
    {
      BYTE* pv = (BYTE*)effect->param;
      int cmask = 0x01;
      for (int c = 0; c < 3; c++)
      {
        if (cmask & effect->channel)
        {
          TCHAR val[120];
          StringCchPrintf(val, 120, _T(",Channel=%d, Input(%d,%d,%d), Output(%d,%d)"), cmask, pv[0], pv[1], pv[2], pv[3], pv[4]);
          StringCchCat(str, length, val);
        }
        cmask <<= 1;
      }
    }
    break;

  case PropertyMode::PModeColorBalance:
    StringCchPrintf(str, length, _T("ColorBalance : R=%d, G=%d, B=%d"), effect->param[0], effect->param[1], effect->param[2]);
    break;

  case PropertyMode::PModeColorBright:
    StringCchPrintf(str, length, _T("BrightContrast : (%d, %d)"), effect->param[0], effect->param[1]);
    break;

  case PropertyMode::PModeColorHLS:
    StringCchPrintf(str, length, _T("ColorHLS : (%d, %d, %d)"), effect->param[0], effect->param[1], effect->param[2]);
    break;

  case PropertyMode::PModeColorHSV:
    StringCchPrintf(str, length, _T("ColorHSV : (%d, %d, %d)"), effect->param[0], effect->param[1], effect->param[2]);
    break;

  }
}

BOOL KBitmap::IsSimpleImageEffect(ImageEffectJob* effect)
{
  switch (effect->job)
  {
  case PropertyMode::PModeRotate:
  case PropertyMode::PModeColorReverse:
  case PropertyMode::PModeMonochrome:
  case PropertyMode::PModeColorLevel:
  case PropertyMode::PModeColorBalance:
  case PropertyMode::PModeColorBright:
  case PropertyMode::PModeColorHLS:
  case PropertyMode::PModeColorHSV:
    return FALSE;

  case PropertyMode::PModeCrop:
  case PropertyMode::PModeResize:
    return TRUE;
  }
  return TRUE;
}


int KBitmap::ApplyEffect(ImageEffectJob* effect)
{
  if ((bitImage == NULL) || (lineBufferLength == 0) || (bitCount == 0))
  {
    StoreLogHistory(_T(__FUNCTION__), _T("invalid bitmap"), SYNC_MSG_LOG | SYNC_MSG_DEBUG);
    return 0;
  }

  DWORD start_tick = GetTickCount();
  int r = 0;

  switch (effect->job)
  {
  case PropertyMode::PModeRotate:
  {
    RotateMode rotateMode = (RotateMode)effect->param[0];
    switch (rotateMode)
    {
    case RotateMode::ROTATE_90CW:
    case RotateMode::ROTATE_90CCW:
    case RotateMode::ROTATE_180:
      Rotate((RotateMode)effect->param[0]);
      r = EFFECT_CHANGE_IMAGE | EFFECT_CHANGE_SIZE;
      break;

    case RotateMode::FLIP_HORZ:
      Flip(FLIP_HORIZONTAL);
      r = EFFECT_CHANGE_IMAGE;
      break;

    case RotateMode::FLIP_VERT:
      Flip(FLIP_VERTICAL);
      r = EFFECT_CHANGE_IMAGE;
      break;
    }
  }
  break;

  case PropertyMode::PModeCrop:
    Crop(effect->param[0], effect->param[1], effect->param[2], effect->param[3], effect->param[4]);
    r = EFFECT_CHANGE_IMAGE | EFFECT_CHANGE_SIZE;
    break;

  case PropertyMode::PModeResize:
    Resize(effect->param[0], effect->param[1], effect->param[2]);
    r = EFFECT_CHANGE_IMAGE | EFFECT_CHANGE_SIZE;
    break;

  case PropertyMode::PModeColorReverse:
    Reverse((ReverseMode)effect->param[0]);
    r = EFFECT_CHANGE_IMAGE;
    break;

  case PropertyMode::PModeMonochrome:
    Monochrome(effect->param[0], effect->param[1]);
    r = EFFECT_CHANGE_IMAGE;
    break;

  case PropertyMode::PModeColorLevel:
    ColorLevel(effect->channel, (BYTE *)effect->param);
    r = EFFECT_CHANGE_IMAGE;
    break;

  case PropertyMode::PModeColorBalance:
    ColorBalance(effect->param[0], effect->param[1], effect->param[2]);
    r = EFFECT_CHANGE_IMAGE;
    break;

  case PropertyMode::PModeColorBright:
    ColorBrightContrast(effect->param[0], effect->param[1]);
    r = EFFECT_CHANGE_IMAGE;
    break;

  case PropertyMode::PModeColorHLS:
    ColorHLS(effect->param[0], effect->param[1], effect->param[2]);
    r = EFFECT_CHANGE_IMAGE;
    break;

  case PropertyMode::PModeColorHSV:
    ColorHSV(effect->param[0], effect->param[1], effect->param[2]);
    r = EFFECT_CHANGE_IMAGE;
    break;

  case PropertyMode::PModeSelectRectangle:
  case PropertyMode::PModeSelectTriangle:
  case PropertyMode::PModeSelectRhombus:
  case PropertyMode::PModeSelectPentagon:
  case PropertyMode::PModeSelectStar:
  case PropertyMode::PModeSelectEllipse:
  {
    RECT rect;
    KImageBlockEditor::GetUpdateRect(rect, effect->param);

    FillShapeRectangle(effect->job, rect, (DWORD)effect->param[5]);
    r = EFFECT_CHANGE_IMAGE;
    break;
  }

  case PropertyMode::PModeCopyRectangle:
  case PropertyMode::PModeCopyTriangle:
  case PropertyMode::PModeCopyRhombus:
  case PropertyMode::PModeCopyPentagon:
  case PropertyMode::PModeCopyStar:
  case PropertyMode::PModeCopyEllipse:
  {
    RECT rect;
    KImageBlockEditor::GetUpdateRect(rect, effect->param);

    if (ValidateRect(rect))
    {
      effect->pResult = CloneBitmap(rect, effect->job);
    }
    else
    {
      StoreLogHistory(_T(__FUNCTION__), _T("Invalid rect on CopyRectangle"), SYNC_MSG_LOG | SYNC_MSG_DEBUG);
    }
    break;
  }
  }

  TCHAR msg[256];
  GetEffectInfoString(effect, msg, 256);

#ifdef _DEBUG
  DWORD end_tick = GetTickCount();
  DWORD ellapsed = (start_tick < end_tick) ? (end_tick - start_tick) : (0xFFFFFFFF - start_tick + end_tick);
  TCHAR es[64];
  StringCchPrintf(es, 64, _T(", ellapsed=%d"), ellapsed);
  StringCchCat(msg, 256, es);
#endif

  StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);

  return r;
}

int KBitmap::ApplyEffectStep(ImageEffectJob* effect, int& step, LPVOID buffer)
{
  if ((bitImage == NULL) || (lineBufferLength == 0) || (bitCount == 0))
  {
    StoreLogHistory(_T(__FUNCTION__), _T("invalid bitmap"), SYNC_MSG_LOG | SYNC_MSG_DEBUG);
    return -1;
  }
  int progress = -1;

  switch (effect->job)
  {
  case PropertyMode::PModeRotate:
  {
    switch ((RotateMode)effect->param[0])
    {
    case RotateMode::ROTATE_90CW:
    case RotateMode::ROTATE_90CCW:
    case RotateMode::ROTATE_180:
      progress = RotateStep((RotateMode)effect->param[0], step, (KBitmap*)buffer);
      break;

    case RotateMode::FLIP_HORZ:
      progress = FlipStep(FLIP_HORIZONTAL, step, (LPBYTE)buffer);
      break;

    case RotateMode::FLIP_VERT:
      progress = FlipStep(FLIP_VERTICAL, step, (LPBYTE)buffer);
      break;
    }
  }
  break;

  case PropertyMode::PModeColorReverse:
    progress = ReverseStep((ReverseMode)effect->param[0], step, (LPBYTE)buffer);
    break;

  case PropertyMode::PModeMonochrome:
    progress = MonochromeStep(effect->param[0], effect->param[1], step, (KBitmap*)buffer);
    break;

  case PropertyMode::PModeColorLevel:
    progress = ColorLevelStep(effect->channel, (BYTE *)effect->param, step);
    break;

  case PropertyMode::PModeColorBalance:
    progress = ColorBalanceStep(effect->param[0], effect->param[1], effect->param[2], step, (LPBYTE)buffer);
    break;

  case PropertyMode::PModeColorBright:
    progress = ColorBrightContrastStep(effect->param[0], effect->param[1], step, (LPBYTE)buffer);
    break;

  case PropertyMode::PModeColorHLS:
    progress = ColorHLSStep(effect->param[0], effect->param[1], effect->param[2], step);
    break;

  case PropertyMode::PModeColorHSV:
    progress = ColorHSVStep(effect->param[0], effect->param[1], effect->param[2], step);
    break;
  }

  return progress;
}

void KBitmap::FillShapeRectangle(PropertyMode mode, RECT& rect, DWORD color)
{
  if (mode == PropertyMode::PModeSelectRectangle)
  {
    FillRectangle(rect, color);
  }
  else if (mode == PropertyMode::PModeSelectEllipse)
  {
    FillEllipse(rect, color);
  }
  else if ((PropertyMode::PModeSelectTriangle <= mode) && (mode <= PropertyMode::PModeSelectStar))
  {
    KBitmap mask;
    mask.CreateBitmap(rect.right - rect.left, rect.bottom - rect.top, 24, FALSE, NULL, 0, NULL);
    HANDLE h = mask.handle();

    HDC hdc = GetDC(GetDesktopWindow());
    HDC memdc = CreateCompatibleDC(hdc);
    HANDLE oldh = SelectObject(memdc, h);
    Graphics g(memdc);

    Point* p = new Point[10];
    RECT br = { 0, 0, rect.right - rect.left, rect.bottom - rect.top };
    ShapeType type = static_cast<ShapeType>((int)mode - (int)PropertyMode::PModeSelectTriangle + (int)ShapeType::ShapeTriangle);
    int pnum = KShapeBase::ReadyShapePolygon(type, p, 10, br);
    SolidBrush brush(0xFFFFFFFF);
    g.FillPolygon(&brush, p, pnum);
    delete[] p;

    FillMask(rect, mask, color);

    SelectObject(memdc, oldh);
    DeleteDC(memdc);
    ReleaseDC(GetDesktopWindow(), hdc);
  }

}

void KBitmap::Dump()
{
	int y, x;
	BYTE* d;
  TCHAR str[128];

  StringCchPrintf(str, 128, _T("width = %d, height = %d, bpp = %d, bitCount = %d, lineBufferLength = %d, hasAlpha = %d\n"),
    width, height, bpp, bitCount, lineBufferLength, hasAlpha);
  OutputDebugString(str);

	for(y = 0; y < height; y++)
	{
		d = _GetScanline(y);

		for(x = 0; x < lineBufferLength; x++)
		{
      StringCchPrintf(str, 128, _T("0x%.2x,"), *d++);
      OutputDebugString(str);
		}
    OutputDebugString(_T("\n"));
	}
}
