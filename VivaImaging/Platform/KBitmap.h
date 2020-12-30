#ifndef K_BITMAP_H
#define K_BITMAP_H

#include "..\Document\KImageProperty.h"
#include "../Platform/Utils.h"

class KBitmap;

#define KDIB_WITH_META	0x0001

#ifndef WIDTHBYTES
#define WIDTHBYTES(i)           ((unsigned)((i+31)&(~31))/8)  /* ULONG aligned ! */
#endif

typedef enum tagTRANSITION_EFFECT_TYPE {
  TE_NONE,
  TE_SCROLL,
  TE_CIRCULAR_SECTOR,
  // TE_CIRCULAR_SECTOR_2,
  TE_CONCENTRIC_CIRCLR,
  TE_CLOCK, // clockwize
  //TE_CLOCK_2, // counter-clockwize
  TE_PENTAGON,
  TE_HEXAGON,
  TE_OCTAGON,
  TE_STAR,
  TE_ZOOM_CENTER,
  TE_ZOOM_LT,
  TE_ZOOM_RT,
  TE_ZOOM_LB,
  TE_ZOOM_RB,
  TE_HORZ_SPAN,
  TE_VERT_SPAN,
  TE_RANDOM
}TRANSITION_EFFECT_TYPE;

typedef enum tagFLIP_DIRECTION {
  FLIP_HORIZONTAL,
  FLIP_VERTICAL
}FLIP_DIRECTION;

typedef enum tagBLIT_MODE {
  BLIT_FITTO,
  BLIT_STRETCH,
  BLIT_AS_CENTER
}BLIT_MODE;

// return value of ApplyEffect
#define EFFECT_DONE 1
#define EFFECT_CHANGE_IMAGE 2
#define EFFECT_CHANGE_SIZE 4

#define INCHES_PER_METER (10000 / 254)
#define PerMeter2PerInch(x) (LONG)((float)x * 254 / 10000 + 0.5)

#define DONE_PROGRESS_STEP 200

class KBitmap
{
public:
	KBitmap();
	KBitmap(KBitmap& bm);
	KBitmap(int width, int height, short colorDepth=32, bool hasAlpha=true, RGBQUAD* palette = NULL, int pnum = 0);
  KBitmap(HANDLE hdib);
  KBitmap(PBITMAPINFOHEADER bi, UINT buffSize);
	~KBitmap();

	int width;
	int height;
	short bitCount;
	bool hasAlpha;

	inline int GetWidth() const { return width; }
	inline int GetHeight() const { return height; }

	const KBitmap& operator=(const KBitmap& img);

  BOOL ValidateRect(RECT& rect);

	// void SetBitmap(int w, int h, short bits, bool bAlpha, RGBQUAD* defpalette, short pNumber, BYTE* buff);
	bool CreateBitmap(int w, int h, short bits, bool hasAlpha, RGBQUAD* palette, short paletteNumber, BYTE* buff);

	void setAlphaBuffer(bool bAlpha);

  KBitmap* CloneBitmap(RECT& rect, PropertyMode mode);
  void SetEllipseAlphaMask(int left, int top, int right, int bottom, BYTE inAlpha, BYTE outAlpha);
  void SetAlphaMask(int left, int top, KBitmap& mask, BYTE inAlpha, BYTE outAlpha);

	void drawImage(int xp, int yp, KBitmap* b, int x, int y, int w, int h, int mode);
	void drawImage(int xp, int yp, int dw, int dh, KBitmap* b, int x, int y, int w, int h);

	void drawAlpha(int xp, int yp, KBitmap* b, int x, int y, int w, int h, BYTE alpha);
	void fill(COLORREF color);

	// 0 : no rotate, 1 : 90, 2 : 180, 3 : 270 degree clock-wize
	bool rotate(int direction);
	bool resize(int cx, int cy, bool preserve_xy_ratio);
	bool crop(RECT& rect);
	bool rotateCopy(KBitmap& bm, int direction);
	bool SetAllAlpha(BYTE a);
	bool copyAlphaChannel(KBitmap* mask);
	bool MergeImage(KBitmap& img);

	bool SetContrastBrightness(char contrast, char brightness);
	bool SetGrayscale(bool bw);

	bool setAlphaValue(BYTE alpha);

  BOOL ReadyEffectMaskImage(TRANSITION_EFFECT_TYPE effectType, int maxStep, int effectStep);

  #ifdef WIN32_CXIMAGE
	bool CreateFromBitmap(KHBITMAP h, KPainter* p);
	bool CreateAlphaFromBitmap(KHBITMAP h, KPainter* p);
	bool CreateDIBitmap(KDC dc, KBITMAPINFO* bmi, LPBYTE buff, int length);
  #endif // SUPPORT_CREATE_FROM_BITMAP

	HBITMAP hbitmap;
	BYTE* bitImage;

	int lineBufferLength;
	short bpp;
	RGBQUAD* palette;
	short paletteNumber;
  UINT horzDPI;
  UINT vertDPI;

	inline HBITMAP handle() { return hbitmap; }
	inline bool isValid() const { return (hbitmap != NULL); }
	inline int GetPaletteNumber() { return paletteNumber; };
	inline void setNumberOfPalettes(int count) { paletteNumber = count; }
	inline BYTE* GetImageBuffer() { return bitImage; };
	inline RGBQUAD* GetPalette() { return palette; }

  RGBQUAD paletteColor(int i);
	void setPaletteColor(int i, RGBQUAD color);
	//HBITMAP LoadImageData(LPBYTE buff, int size, HDC dc, bool* pAlpha, DWORD* palette, int pal_num);

  HBITMAP LoadFromPNGResource(int resid);

	HBITMAP LoadImageFromBuffer(LPBYTE buff, int size, DWORD flag = 0, DWORD idx = 0);
	HBITMAP LoadImageFromFile(const WCHAR* name, DWORD flag = 0, DWORD idx = 0);
  HBITMAP LoadImageFromStream(IStream* pStream, DWORD flag = 0, DWORD idx = 0);

  HBITMAP LoadImageFromFrameDecoder(IWICImagingFactory* pImageFactory, IWICBitmapFrameDecode *pIBitmapFrame);

  HBITMAP LoadImageFromRGBABuffer(int w, int h, short colorDepth, RGBQUAD* pal, int pNumber, bool bAlpha, LPBYTE imageBuffer);

  HBITMAP LoadImageFromDIB(PBITMAPINFOHEADER pDIB, UINT buffSize = 0);

  HBITMAP LoadBMPFile(LPCTSTR filename);

  HBITMAP RefreshBitmapPalette();

  void SetImageDPI(UINT xDpi, UINT yDpi);
  LPBYTE getTopDownImageBuffer();

  BOOL StoreBMPFile(LPCTSTR filename, int dpi = 0);

#if 0
  BOOL StoreImage(LPCTSTR filename, KVoidPtrArray* pMetaData, LPBYTE pExtraData, int len);
  BOOL StoreImageInternal(LPCTSTR filename, KVoidPtrArray* pMetaData, LPBYTE pExtraData, int len);
  BOOL StoreImageWithFrameEncoder(IWICImagingFactory* pImageFactory, IWICBitmapEncoder *piEncoder, 
    KVoidPtrArray* pMetaData);
  BOOL StoreMetadataToWriter(IWICMetadataQueryWriter* piFrameQWriter, KVoidPtrArray* pMetaData);
  BOOL StoreExtraDataToWriter(IWICBitmapFrameEncode* piFrameEncode, LPBYTE pExtraData, int len);
#endif

  BOOL StoreImageWithFrameEncoder(IWICImagingFactory* pImageFactory, IWICBitmapFrameEncode* piFrameEncode);

  HBITMAP CompatibleBitmap();
  HANDLE GetDIB();

  void AlphaBlend(CDC& dc, int xp, int yp, int alpha);
  void AlphaBlend(CDC& dc, RECT& rect, RECT& src, int alpha);

  void Blit(CDC& dc, RECT& rect, float zoom, RECT& src, BLIT_MODE mode);
  void Blit(CDC& dc, RECT& rect, BLIT_MODE mode);


  void GetHistogram(BYTE* histogram, int channel);

  void Flip(FLIP_DIRECTION dir);
  int FlipStep(FLIP_DIRECTION dir, int& step, LPBYTE t);

  void Rotate(RotateMode m);
  int RotateStep(RotateMode m, int& y, KBitmap* bm);

  void copyBlock(KBitmap& bm, int left, int top, int right, int bottom);
  void copyFromBlock(int xp, int yp, KBitmap& bm, int x, int y, int w, int h);

  void Crop(int asRatio, int left, int top, int right, int bottom);
  void Crop(int left, int top, int right, int bottom);
  void Resize(int byRatio, int w, int h);

  void Reverse(ReverseMode mode);
  int ReverseStep(ReverseMode mode, int& y, LPBYTE buffer);

  void converToColor(KBitmap& bm);
  void ConverToColor(BOOL withAlpha);

  void Monochrome(int asGrayscale, int gamma);
  int MonochromeStep(int asGrayscale, int gamma, int& y, KBitmap* bm);

  void ColorLevel(int channel, BYTE* param);
  int ColorLevelStep(int channel, BYTE* param, int& y);

  void ColorBalance(int redp, int greenp, int bluep);
  int ColorBalanceStep(int redp, int greenp, int bluep, int& y, BYTE* rgbTable);

  void ColorBrightContrast(int bright, int contrast);
  int ColorBrightContrastStep(int bright, int contrast, int& y, BYTE* cTable);

  void ColorHLS(int hue, int light, int sat);
  int ColorHLSStep(int hue, int light, int sat, int& y);

  void ColorHSV(int hue, int sat, int value);
  int ColorHSVStep(int hue, int sat, int value, int& y);

  BOOL FillRectangle(RECT& rect, DWORD argb);
  BOOL FillEllipse(RECT& rect, DWORD argb);
  BOOL FillMask(RECT& rect, KBitmap& mask, DWORD argb);

  BOOL IsNeedRenderEffect(ImageEffectJob* effect);

  static void GetEffectInfoString(ImageEffectJob* effect, LPTSTR str, int length);

  // BgTask를 사용하지 않는 simple effect
  static BOOL IsSimpleImageEffect(ImageEffectJob* effect);

  int ApplyEffect(ImageEffectJob* effect);

  int ApplyEffectStep(ImageEffectJob* effect, int& step, LPVOID buffer);


  void FillShapeRectangle(PropertyMode mode, RECT& rect, DWORD color);

	void Dump();

  void Clear();

  inline BYTE* _GetScanline(int line) { return(bitImage + line * lineBufferLength); };

private:
	void _swap(KBitmap& bm);
	bool _SetBitCount(short bits);

	bool _CreateBuffer(int w, int h, short bits, bool hasAlpha, RGBQUAD* palettes, int pnumber);

	void _BitmaskConvert(BYTE *src, WORD redmask, WORD greenmask, WORD bluemask, BYTE bpp);
	LPBYTE _pixelBuffer(int x, int y);

	HBITMAP loadImageFromFileCx(const WCHAR* filename, DWORD flag, DWORD frameIndex);
  HBITMAP loadImageFromBufferCx(LPBYTE buff, int size, DWORD flag, DWORD frameIndex);
  void convertTable(BYTE* cTable);
  void convertTable(BYTE* rTable, BYTE* gTable, BYTE* bTable);
};


LPTSTR GetRotateName(RotateMode m);
LPTSTR GetReverseName(ReverseMode m);
LPTSTR GetBlockModeName(PropertyMode mode);

extern void Copy24BitAlphaTo32Image(LPBYTE d, LPBYTE s, LPBYTE a, int sbpp, int biWidth, int biHeight, int flag);

#endif // K_DIB_BITMAP_H
