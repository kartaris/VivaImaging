#pragma once

#include "stdafx.h"
#include "KImageBase.h"
#include "KImageproperty.h"
#include "KShapeEditor.h"
#include "KImageBlockEditor.h"

// effect preview image를 백그라운드 쓰레드에서 생성하기.
#define _RENDER_BY_BG_THREAD

class KImagePage : public KImageBase
{
public:
  KImagePage();
  KImagePage(KImageBase* p);
  KImagePage(KImageBase* p, LPCTSTR pFilename);
  KImagePage(KImageBase* p, PBITMAPINFOHEADER pDIB);

  virtual ~KImagePage();

  virtual inline KBitmap* GetThumbImage() {
    return m_ThumbImage;
  };

  static int g_DefaultPageNameIndex;

  KBitmap* GetImage(LPCTSTR folderPath, ImageEffectJob* effect = NULL, BOOL originImage = FALSE);
  inline LPCTSTR GetLocalFilename() { return m_LocalFilename; }

  virtual void Clear();
  virtual int GetNumberOfPages(int flag) { return 1; }
  virtual int GetType() { return IMAGE_PAGE; }
  virtual void SetModified(BOOL bRecurr, PageModifiedFlag modified);

  // permissions
  virtual BOOL IsEditable(int flag = 0);

  BOOL IsNeedRenderThumbImage(LPTSTR folderPath, int cx, int cy);
  BOOL RenderThumbImage(CDC* pDC, LPTSTR folderPath, int cx, int cy);
  virtual void ClearThumbImage(BOOL bRecurr);

  inline BOOL HasTemporaryEffect() { return mHasEffect; }
  BOOL hasDifferentFolder(LPCTSTR workingPath);

  void ClearCacheImage(LPCTSTR workingPath);
  void ClearPreviewImage();

  SIZE GetImageSizeRatio();

  BOOL GetEcmInfo(LPTSTR docOID, LPTSTR fileOID, LPTSTR storageOID);

  void ClearEditImage();

  BOOL SetPathName(LPTSTR pathName);

  // BOOL SaveImageFile(LPCTSTR pathName);

  virtual int StoreImageFile(LPCTSTR pathName, KEcmDocTypeInfo* info, LPCTSTR firstFilename, BOOL asTemporaryIfExist);

  BOOL StoreImageFileEx(LPCTSTR filename, KVoidPtrArray* pMetaData);
  BOOL StoreImageFileExInternal(LPCTSTR filename, KVoidPtrArray* pMetaData);
  BOOL StoreImageWithFrameEncoder(IWICImagingFactory* pImageFactory, IWICBitmapEncoder* piEncoder, 
    ImageEncodingFormat encode, KVoidPtrArray* pMetaData);
  BOOL StoreMetadataToWriterCascaded(IWICImagingFactory* pFactory, IWICMetadataQueryWriter* piQWriter,
    ImageEncodingFormat encode, KVoidPtrArray* pMetaData);
  BOOL StoreMetadataToQWriter(IWICImagingFactory* pFactory, IWICMetadataQueryWriter* piQWriter,
    ImageEncodingFormat encode, KVoidPtrArray* pMetaData);

  BOOL StoreMetadataToWriterEx(IWICMetadataQueryWriter* piQWriter, ImageEncodingFormat encode, KVoidPtrArray* pMetaData, MetaDataCategory category);
  BOOL StoreExtraDataToWriter(IWICBitmapFrameEncode* piFrameEncode, LPBYTE pExtraData, int len);

  virtual void GetMetaDataSet(KVoidPtrArray& list, KEcmDocTypeInfo* info, int flag);

  LPBYTE GetMetaDataBinary(LPCTSTR key, int& len, MetaDataCategory& category, WORD& vtype);

#if 0
  BOOL StoreMetaTagToFrameEncoder(IWICBitmapFrameEncode *piFrameEncode, LPCTSTR folderList);
  BOOL StoreAnnotatesToFrameEncoder(IWICBitmapFrameEncode *piFrameEncode, KEcmDocTypeInfo* pInfo);
#endif

  void SetImageDPI(UINT xDpi, UINT yDpi);

  int StoreExtraDataBlock(KMemoryStream& mf, KEcmDocTypeInfo* pInfo);
  int LoadExtraDataBlock(KMemoryStream& mf, KEcmDocTypeInfo* pInfo);

  BOOL SetWICMetadataValue(UINT ifd_id, PROPVARIANT& value, KEcmDocTypeInfo* docTypeInfo);

  virtual int MergeAnnotateObjects();

  BOOL LoadMetaDataFromMetadataReader(void* pReader, KEcmDocTypeInfo* docTypeInfo);

  BOOL LoadMetaDataFromFrameDecoder(IWICBitmapFrameDecode *pIFrameDecoder, KEcmDocTypeInfo* docTypeInfo);
  BOOL LoadImageFromFrameDecoder(IWICImagingFactory* pImageFactory, IWICBitmapFrameDecode *pIBitmapFrame, KEcmDocTypeInfo* docTypeInfo);

  BOOL IsNeedRenderEffect(ImageEffectJob* effect);

  BOOL IsNeedRenderImageEffect(ImageEffectJob* effect);
  BOOL SetImageEffect(ImageEffectJob* effect);
  BOOL SetImageEffectCache(ImageEffectJob* effect, KBitmap* cacheImage);

  BOOL renderImage(LPCTSTR folderPath, ImageEffectJob* effect);
  unsigned __int64 GetImageCacheSize();

  BOOL GetImageSelection(int* param);
  BOOL GetUpdateRect(ImageEffectJob* effect, RECT& rect);
  BOOL CloseProperty(ImageEffectJob* effect, RECT& rect);;

  BOOL CloseToolMode(PropertyMode mode, PropertyMode nextMode, LPRECT updateRect);

  static BOOL IsPropertyNeedImageCacheUpdate(PropertyMode mode);
  static BOOL IsPropertyNeedImageOverlayUpdate(PropertyMode mode);
  static BOOL IsPropertyNeedSceenErase(ImageEffectJob* effect);

  HANDLE SaveAnnotateData();
  int RestoreAnnotateData(HANDLE h);

  int SetShapeSelected(KVoidPtrArray& shapeList);

  virtual MouseState OnLButtonDown(PropertyMode mode, int nFlags, CPOINT& point, KImageDrawer& info, LPRECT ptUpdate);
  virtual int OnLButtonUp(MouseState mouseState, int nFlags, CPOINT& point, KImageDrawer& info, LPRECT ptUpdate);
  virtual int OnMouseMove(MouseState mouseState, int nFlags, POINT& point, KImageDrawer& info, LPRECT ptUpdate);
  virtual int OnMouseMoves(MouseState mouseState, int nFlags, LP_CPOINT points, int count, KImageDrawer& info, LPRECT ptUpdate);
  virtual int OnRButtonDown(PropertyMode mode, int nFlags, POINT& point, KImageDrawer& info, LPRECT ptUpdate);
  virtual int OnRButtonUp(MouseState mouseState, int nFlags, POINT& point, KImageDrawer& info, LPRECT ptUpdate);

  virtual int OnKeyDown(UINT nChar, UINT nFlags, PropertyMode mode, KImageDrawer& info, LPRECT ptUpdate);
  virtual int OnKeyUp(UINT nChar, UINT nFlags, PropertyMode mode, KImageDrawer& info, LPRECT ptUpdate);

  virtual int SelectAll(PropertyMode mode, KImageDrawer& drawer, LPRECT updateRect);
  virtual int HasSelection(int type);
  virtual HANDLE CopyToClipboard(PropertyMode mode, BOOL bCut, int& clipboard_data_format);
  virtual int PasteClipboardData(HANDLE h);
  virtual int DeleteSelection(LPRECT updateRect);

  virtual void DrawEditing(Graphics& g, KImageDrawer& info, PropertyMode mode, int flag);

  virtual int ApplyEffect(ImageEffectJob* effect, BOOL bAll);

  BOOL restoreImageData(HANDLE h_dib);

  BOOL StartFloatingImagePaste(PBITMAPINFOHEADER bi, UINT buffSize, LPRECT updateRect);
  BOOL HasFloatingImage();
  BOOL GetFloatingImagePasteParam(PasteImageActionParam* p);
  BOOL EndFloatingImagePaste(BOOL bDone, LPRECT updateRect);

  int ShapeEditCommand(int channel, int* params, void* extra, RECT& rect);

  void OnSelectionChanged(KShapeBase* shape);
  KShapeBase* IsNeedTextboxEditText();
  
  void MakeDefaultImageFilename(LPTSTR pageName, int length, LPCTSTR formatStr, int index, int base);

  void ReadyDefaultName(LPCTSTR pathName, LPTSTR pageName, LPCTSTR defExt);
  void ReadyDefaultName(LPCTSTR pathName, LPTSTR pageName, LPCTSTR defExt, int index, int base);

  void readyImagePathName(LPCTSTR pathName, BOOL asUnique);
  LPTSTR readyTempFilename(LPCTSTR pathName);

  virtual int MoveToWorkingFolder(LPCTSTR pathName);

#ifdef USE_XML_STORAGE
  HRESULT ReadAttributes(IXmlReader* pReader, int flag);
  HRESULT ReadDocument(CComPtr<IXmlReader> pReader, KVoidPtrArray* pShapeList, int flag);

  virtual int ValidateMetadatas(KEcmDocTypeInfo& info);

  virtual HRESULT StoreDocument(CComPtr<IXmlWriter> pWriter, BOOL selectedOnly);
#else
  DWORD ReadDocument(KMemoryStream& ms, KVoidPtrArray* pShapeList, int flag);
  HRESULT LoadAnnotates(KMemoryStream& ms, KVoidPtrArray* pShapeList, int flag);
  virtual DWORD StoreDocument(KMemoryStream& ms, BOOL selectedOnly);
#endif

  virtual int ImageAutoRename(LPCTSTR pathName, int& index, int& base);

  inline void SetMultiPageIndex(int idx) { m_MultiPageIndex = idx; }

  virtual void ClearTempFiles(LPCTSTR workingPath);

private:
  LPTSTR m_LocalFilename;
  LPTSTR m_TempFilename;
  DWORD m_MultiPageIndex; // index of MultiPageImage(start from 1)
  LPTSTR m_Tag;
  BOOL mHasEffect;
  KBitmap* m_Image;
  KBitmap* m_CacheImage;
  KBitmap* m_ThumbImage;

  KShapeEditor m_Annotates;

  KImageBlockEditor m_BlockEditor;

  ImageEffectJob* mpPreviewEffect;

};

