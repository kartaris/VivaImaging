#pragma once
#include "KImageBase.h"
#include "..\Platform\Utils.h"

class KImageFolder : public KImageBase
{
public:
  KImageFolder();
  KImageFolder(KImageBase* p);
  virtual ~KImageFolder();

  virtual void Clear();
  virtual int GetNumberOfPages(int flag);
  virtual int GetType() { return FOLDER_PAGE; }
  virtual void SetModified(BOOL bRecurr, PageModifiedFlag modified);

  virtual LPTSTR GetImageFolderPath();

  BOOL IsLastChild(KImageBase* p);

  BOOL GetEcmInfo(LPTSTR folderOID, LPTSTR* pFullPathIndex);

  // permissions
  inline BOOL IsContainerEditable() { return !!(mPermission & SFP_WRITE_ELEMENT); }

  KImageDocVector m_Childs;

  inline int GetChildCount() { return (int)m_Childs.size(); }

  void ClearThumbImage(BOOL bRecurr);

  void InsertPage(KImageBase* page, KImageBase* afterPage);
  void InsertPage(KImageBase* page, int index);
  BOOL RemovePage(KImageBase* page);
  BOOL NextOrPreviousPage(KImageBase* p[], int s, int flag);

  void CheckReadyFolderPage(KImageDocVector* pPageList);

  KImageFolder* GetPageFromParentKeyName(LPCTSTR parentKeyName, bool CreateIfNotExist);

  KImageBase* GetIndexedPage(int index, int flag);
  int GetIndexOfPage(KImageBase* page, int flag);
  int IsChildOf(KImageBase* page);
  BOOL IsLastOf(KImageBase* page);

  int GetPageList(KImageDocVector& imagePages, int flag);
  int MoveChildTo(KImageFolder& d);

  KImageBase* FindPageByName(LPCTSTR name, int type);

  KImageBase* FindPageByID(UINT id);

  virtual BOOL SetSelectedRecursive(BOOL set);
  int SelectPageRange(int& index, int p1, int p2, int flag);

  int SelectAllPages(BOOL bSelect);

  int DeleteSelectedPages();

  int DetachSelectedPages(KVoidPtrArray& pages);

  KImageBase* DetachPageById(int id);

  int DeletePagesById(KIntVector& pageIdList);

  BOOL InsertPages(int targetIndex, KImageBase* at, InsertTargetType inside, KVoidPtrArray& pages);

  int StorePageIds(int* buff, int count, int flag);
  int RestorePageOrdering(int* pageIds, int count);

  // return number of pages
  virtual int MergeAnnotateObjects();

  virtual int ApplyEffect(ImageEffectJob* effect, BOOL bAll);

#ifdef USE_XML_STORAGE
  HRESULT ReadAttributes(IXmlReader* pReader);
  HRESULT ReadDocument(CComPtr<IXmlReader> pReader, KImageDocVector* newPages, int flag);

  virtual int ValidateMetadatas(KEcmDocTypeInfo& info);

  virtual HRESULT StoreDocument(CComPtr<IXmlWriter> pWriter, BOOL selectedOnly);
#else
  virtual DWORD LoadMetaData(KMemoryStream& ms);
  virtual DWORD SaveMetaData(KMemoryStream& ms);

  DWORD ReadDocument(KMemoryStream& ms, KImageDocVector* newPages, int flag);
  virtual DWORD StoreDocument(KMemoryStream& ms, BOOL selectedOnly);
#endif // USE_XML_STORAGE

  int StoreImageFile(LPCTSTR pathName, KEcmDocTypeInfo* info, LPCTSTR firstFilename, BOOL asTemporaryIfExist);

  virtual int MoveToWorkingFolder(LPCTSTR pathName);
  virtual BOOL RefreshMetaData();

  static int g_DefaultFolderNameIndex;
  static void ReadyDefaultName(LPTSTR pageName);

  virtual int ImageAutoRename(LPCTSTR pathName, int& index, int& base);
  
  virtual void WriteImageName(KMemoryStream& mf);
  virtual int ReadImageName(KMemoryStream& mf);

  LPTSTR GetFolderNameID();
  int WriteFolderNameID(KMemoryStream& mf);
  LPTSTR GetSubFolderNames();
  void ReadyFolderPages(LPTSTR folderList);

  HANDLE BackupPageNames();
  BOOL RestorePageNames(HANDLE h);

  virtual void ClearTempFiles(LPCTSTR workingPath);

  KMetadataInfo* GetMetaDataInfo(int index);

};

