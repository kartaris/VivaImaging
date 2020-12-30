// Copyright (C) 2019 K2Mobile
// All rights reserved.


// VivaImagingDoc.h: CVivaImagingDoc 클래스의 인터페이스
//


#pragma once

#include <vector>
#include "Document\KImageFolder.h"
#include "Document\KImagePage.h"
#include "Document\KActionList.h"

#include "Document\KImageCacheManager.h"
#include "Core\KConstant.h"
#include "Core\KEcmDocType.h"
#include "Core\KEcmFolderItem.h"

// AddNewPage flag
#define CLEAR_SELECT 1
#define ADD_SELECT 2
#define READY_META_INFO 4
#define ADD_SELECT_FIRST 8
#define ADD_WITH_FOLDER 0x010
#define ADD_WITH_EMPTY 0x020
#define ADD_ON_TAIL 0x040

// update view flag
#define CHANGE_PAGES 1
#define CHANGE_CURRENT_PAGE 2
#define CHANGE_CURRENT_VIEW 4
#define CHANGE_ALL_VIEW 8

// result of InsertNativeObjectData();
#define ADD_PAGES 1
#define ADD_SHAPES 2


class CVivaImagingDoc : public CDocument
{
protected: // serialization에서만 만들어집니다.
	CVivaImagingDoc();
	DECLARE_DYNCREATE(CVivaImagingDoc)

// 특성입니다.
public:
  KImageFolder m_Pages;

  KImageBase* m_ActivePage;

  KActionList mActionManager;

  // DocType information
  inline LPCTSTR GetTargetFolderOid() { return mDocTypeInfo.mTargetFolderOid; }
  inline LPCTSTR GetDocTypeOid() { return mDocTypeInfo.mDocTypeOid; }
  inline LPCTSTR GetDocTypeName() { return mDocTypeInfo.mpDocTypeName; }

  KEcmDocTypeInfo mDocTypeInfo;

  LPTSTR GetCreatorUserID();
  LPTSTR GetCreatorUserOID();
  LPTSTR GetCreatedDate();

  static LPTSTR GetWorkingBaseFolder();
  static void SetWorkingBaseFolder(LPCTSTR path);

  int ImageAutoRename();

  int GetNumberOfPages(int flag);

  inline LPTSTR GetWorkingPath() { return mWorkingPath; }
  inline KImageBase* GetActivePage() {
    return m_ActivePage;
  }

  ImagingSaveFormat mFileFormat;
  inline ImagingSaveFormat GetCurrentFormat() { return mFileFormat; }
  void SetCurrentFormat(ImagingSaveFormat format);

  TCHAR mServerUploadFolderOid[MAX_OID];
  void SetServerUploadFolder(LPCTSTR folder_oid);
  LPCTSTR GetServerUploadFolder();

  inline BOOL IsNewDocument() { return mIsNewDocument; }

  inline KActionList* GetActionManager() { return &mActionManager; }

  void UpdateExtDocTypeInfo(EcmFolderItem* ecmRoot, KEcmDocTypeInfo* pInfo);

  void insertPage(KImageBase* page, KImageBase* parent = NULL);

  LPTSTR CopyFileToWorkspace(LPCTSTR pathName);

  int AddImagePages(LPCTSTR pFilenames, KIntVector* pPageList, int flag, LPCTSTR deviceName);

  /* 
  * add pages with action
  * return : number of pages
  */
  int AddImagePagesEx(LPCTSTR pFilenames, int length, int flag, LPCTSTR deviceName = NULL);

  // int AddNewPages(KVoidPtrArray& filelist, int insert_flag, LPCTSTR deviceName = NULL);

  int UpdatePages(LPCTSTR pFilenames, int length, int flag, KIntVector& pageList);

  /*
  * return : id of new page
  */
  int AddNewPage(LPCTSTR pFilename, int flag, LPCTSTR deviceName = NULL, LPCTSTR folderName = NULL, LPCTSTR ecmInfo = NULL);

  /*
  * return : id of new page
  */
  int AddNewPage(PBITMAPINFOHEADER pDIB, int flag, LPCTSTR deviceName = NULL);

  KImageFolder* addNewFolder(LPCTSTR name);

  /*
  * return : id of new page
  */
  int AddNewFolder(LPCTSTR name, int flag, LPCTSTR ecmInfo);

  BOOL SetDocTypeInfo(KEcmDocTypeInfo& columnInfos);

  KImageBase* GetRootImagePage();

  KImageBase* GetImagePage(int flag, int index = -1);

  BOOL OnAccessCacheImage(KImagePage* p);

  BOOL GetCurrentPageInfo(int index, int& w, int& h, int& c, LPCTSTR* pName);

  void ClearThumbImage(BOOL clearAll, int index);

  void SetActivePage(KImageBase* page);
  int GetActivePageIndex();
  BOOL SetActivePageIndex(int index);

  //void ReadyDefaultName(int type, LPTSTR pageName, LPCTSTR defExt);

  BOOL SelectPageRange(int p1, int p2, int flag);
  BOOL ToggleSelectPage(int p);
  BOOL SelectPage(int p, BOOL bSelect);
  BOOL SelectAllPages(BOOL bSelect);
  HANDLE DeleteSelectedPages();

// 작업입니다.
  //BOOL DeletePage();
  BOOL MovePreviousPage();
  BOOL MoveNextPage();

  BOOL GotoPreviousPage();
  BOOL CanGotoPreviousPage();
  BOOL GotoNextPage();
  BOOL CanGotoNextPage();

  BOOL IsImagePage();

  BOOL SetMetaData(KImageBase* page, LPCTSTR key, LPCTSTR str);
  int PropertyChange(LPCTSTR key, int value);
  int PropertyChange(LPCTSTR key, LPCTSTR str);

  int DetachSelectedPages(KVoidPtrArray& pages);

  BOOL InsertPages(int targetIndex, InsertTargetType inside, KImageBase* at, KVoidPtrArray& pages);

  BOOL MoveSelectedPages(int targetPageIndex, InsertTargetType insideFolder, KImageBase* target_page);

  int ShapeEditCommand(int channel, int* params, void* extra, RECT& rect);

  // Image editing
  int ApplyEffect(ImageEffectJob* effect);

  int OnLButtonUp(MouseState mouseState, int nFlags, CPOINT& point, KImageDrawer& info, LPRECT ptUpdate);

  int OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags, PropertyMode mode, KImageDrawer& info, LPRECT ptUpdate);

  int OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags, PropertyMode mode, KImageDrawer& info, LPRECT ptUpdate);

  int DeleteSelection(LPRECT ptUpdate);

  int EndFloatingImagePaste(KImagePage* p, BOOL bDone, LPRECT rect);

  // int StoreToFile(KImageBase* page);

  BOOL ReadyWorkingPath();

  BOOL ReadyFileSave(LPCTSTR lpszPathName);

  //inline ImagingSaveFormat GetLastSavedFormat() { return mSavedFormat; }
  inline LPCTSTR GetDownloadedBodyDoc() { return mpDownloadedBodyDocName; }

public:
  int Undo();
  BOOL CanUndo();

  int undoDeletePage(HANDLE param, HANDLE backup);
  int undoInsertPage(HANDLE param, HANDLE backup);
  int undoMovePage(HANDLE param, HANDLE backup);
  int undoEditName(HANDLE param, HANDLE backup);
  int undoAutoRename(HANDLE param, HANDLE backup);

  int undoImageData(HANDLE param, HANDLE backup);
  int undoEditMetaData(HANDLE param, HANDLE backup);
  int undoEditDocType(HANDLE param, HANDLE backup);
  int undoAnnotateEdit(HANDLE param, HANDLE backup);

  int Redo();
  BOOL CanRedo();

  int redoDeletePage(HANDLE param, HANDLE backup);
  int redoInsertPage(HANDLE param, HANDLE backup);
  int redoMovePage(HANDLE param, HANDLE backup);
  int redoEditMetaData(HANDLE param, HANDLE backup);
  int redoEditDocType(HANDLE param, HANDLE backup);
  int redoEditName(HANDLE param, HANDLE backup);
  int redoAutoRename(HANDLE param, HANDLE backup);

  int deletePagesById(HANDLE param);

  BOOL UpdateRenderImage(KImagePage* p, ImageEffectJob* effect, KBitmap* cacheImage);


private:
  void clearDocument();

  TCHAR mCreatorID[MAX_USERNAME];
  TCHAR mCreatorOID[MAX_OID];
  TCHAR mCreatedDate[MAX_DATETIME];

  BOOL mIsNewDocument;
  BOOL mIsTempFolder;
  LPTSTR mWorkingPath;
  //ImagingSaveFormat mSavedFormat;
  LPTSTR mpDownloadedBodyDocName; // body xml file

  void setWorkingPath(LPCTSTR path);

  BOOL readyDefaultProject();

  KImageCacheManager mCacheManager;

      // 재정의입니다.
public:

  HRESULT ReadAttributes(IXmlReader* pReader);

  //virtual BOOL IsModified();
  virtual void SetModifiedFlag(BOOL bModified = TRUE);

  virtual BOOL OnNewDocument();
  virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);

  BOOL OpenDocumentFolder(LPCTSTR path);

  virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
  virtual void OnCloseDocument();

  virtual void SetTitle(LPCTSTR lpszTitle);

  ImagingSaveFormat OnFileSave();
  //afx_msg void OnUpdateFileSave(CCmdUI* pCmdUI);
  ImagingSaveFormat OnFileSaveAs(LPCTSTR name);

  static UINT GetImageFrameCount(LPCTSTR lpszPathName);

#ifdef USE_XML_STORAGE
  static HRESULT ReadCheckAttributes(IXmlReader* pReader);
  static BOOL IsImagingXMLDocFile(LPCTSTR lpszPathName);


  BOOL OnOpenXmlDocument(LPCTSTR lpszPathName, CString& errmsg);

  BOOL OnSaveXML(LPCTSTR lpszPathName, CString& errmsg);
  LPSTR StoreDocTypeInfoToString();
#endif // USE_XML_STORAGE

  BOOL InsertImageFile(LPCTSTR lpszPathName, KImageDocVector* pPageList, BOOL insertAsSubFolder, CString& errmsg,
    LPCTSTR folderName = NULL);

  BOOL OnSaveAsTiff(LPCTSTR lpszPathName, CString& errmsg);
  BOOL SaveImageAsTiff(LPCTSTR lpszPathName, KImageDocVector& imagePages, CString& errmsg);

  BOOL StoreFrame(IWICImagingFactory* pImageFactory, IWICBitmapEncoder *piEncoder, ImageEncodingFormat encode,
    KImagePage* image, LPCTSTR folderList, KEcmDocTypeInfo* pInfo);

  HANDLE CopyToEditActionBackup(BOOL applyAll);

  HANDLE CopyToClipboard(BOOL allSelected);

  HANDLE SaveDocTypeMetaData();
  BOOL LoadDocTypeMetaData(HANDLE h);

  void checkReadyFolderPage(KImageDocVector* pPageList, KImageDocVector& newPageList);

  int InsertPageData(HANDLE b, int flag);

  void ClearModifiedFlag();

  BOOL IsEditable();
  BOOL IsChangableToEdit();
  BOOL IsFolderEditable();

	virtual void Serialize(CArchive& ar);
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

// 구현입니다.
public:
	virtual ~CVivaImagingDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
  afx_msg void OnFileClose();

  // 생성된 메시지 맵 함수
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// 검색 처리기에 대한 검색 콘텐츠를 설정하는 도우미 함수
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
};

extern int GetCurrentDateTime(LPTSTR str, int buffLength);
extern LPTSTR GetCurrentUserID();
extern LPTSTR GetCurrentUserOID();

