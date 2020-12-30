/**
* @file KImageBase.h
* @date 2019.02
* @copyright k2mobile.co.kr
* @brief 이미지 및 폴더 개체의 베이스 클래스 header file
*/
#pragma once

// flags for GetNumberOfPages
#define IMAGE_ONLY 1
#define SELECTED_ONLY 2
#define FOLDER_ONLY 4
#define EXCLUDE_FOLDED 0x10
#define EXCLUDE_SELECTED 0x20
#define EXCLUDE_CHILD_FOLDER 0x40

#define SELECT_EXCLUSIVE 0x1000

// flags for mFlags
#define IS_FOLDED  1
#define IS_SELECTED 2

#define IS_FILE_NOT_READY 0x010

#define NONE_PAGE 0
#define IMAGE_PAGE 1
#define FOLDER_PAGE 2

#include "../Core/KConstant.h"
#include "../Core/KEcmDocType.h"
#include "../Core/KMetadataInfo.h"
#include "../Platform/KBitmap.h"
#include "../Platform/Utils.h"
#include "../Core/HttpDownloader/KFile.h"
#include "../Core/HttpDownloader/KMemory.h"
#include "../Core/KMemoryStream.h"

#include "Shape/KShapeBase.h"

#include <gdiplus.h>
#include < XmlLite.h>

// permissions for itself
#define SFP_LOOKUP 0x0001 /** 대상 항목의 조회 권한 */
#define SFP_READ 0x0002 /** 대상 항목의 읽기 권한 */
#define SFP_WRITE 0x0004 /** 대상 항목의 쓰기 권한 */
#define SFP_DELETE 0x0008 /** 대상 항목의 삭제 권한 */
#define SFP_SETUP 0x0010 // old 0x000F  /** 대상 항목의 전체 권한 */

// permissions for childs
#define SFP_INSERT_CONTAINER 0x0020 // old 0x0010  /** 대상 항목의 하위 폴더 생성 권한 */
#define SFP_INSERT_ELEMENT 0x0040 // old 0x0020 /** 대상 항목의 하위 문서 생성 권한 */

#define SFP_LOOKUP_ELEMENT 0x0080 /** 대상 항목의 하위 항목 조회 권한 */
#define SFP_READ_ELEMENT 0x0100 /** 대상 항목의 하위 항목 읽기 권한 */
#define SFP_WRITE_ELEMENT 0x0200 /** 대상 항목의 하위 항목 쓰기 권한 */
#define SFP_DELETE_ELEMENT 0x0400 /** 대상 항목의 하위 항목 삭제 권한 */
#define SFP_SETUP_ELEMENT 0x0800 // old 0x0FF0  /** 대상 항목의 하위 항목 전체 권한 */

#define SFP_FULL_SETUP 0x0FFF  /** 대상 항목의 전체 권한 */

typedef enum class tagImageEncodingFormat {
  None,
  Bmp,
  Jpeg,
  Png,
  Tiff
} ImageEncodingFormat;

/*
#define TIFF_ANNOTATE_BLOCK 0x4A60307B
#define TIFF_DOCTYPE_LIST 0x4A60306B
#define TIFF_DOCTYPE_INFO 0x4A60305B
#define TIFF_METADATA_VALUE 0x4A60304B
*/

typedef enum class tagInsertTargetType {
  TARGET_BEFORE,
  TARGET_INSIDE,
  TARGET_NEXT,
} InsertTargetType;

typedef enum class tagMouseState {
  MOUSE_OFF,
  MOUSE_ENTER,
  MOUSE_UP_SCROLL,
  MOUSE_DOWN_SCROLL,
  MOUSE_DRAG_SCROLL,
  MOUSE_DOWN_SELECT,
  MOUSE_CONTEXT_MENU,
  MOUSE_PAGE_DRAG,
  MOUSE_DOWN_FOLDING,
  MOUSE_SCROLLBAR_DRAG, 

  MOUSE_DRAG_CROP_LEFT,
  MOUSE_DRAG_CROP_RIGHT,
  MOUSE_DRAG_CROP_TOP,
  MOUSE_DRAG_CROP_BOTTOM,

	MOUSE_DRAW_SELECT,
	MOUSE_DRAW_SELECT_RESIZE,
	MOUSE_DRAW_SELECT_MOVE,
  MOUSE_DRAW_LINE,
  MOUSE_DRAW_FREEHAND,
  MOUSE_DRAW_FREESHAPE,
  MOUSE_DRAW_RECTANGLE,
  MOUSE_DRAW_ELLIPSE,
  MOUSE_DRAW_TEXTBOX,

  MOUSE_DRAW_SHAPESMART, //PModeAnnotateShape,
  MOUSE_DRAW_SHAPESMART_LAST = MOUSE_DRAW_SHAPESMART + 200,


  MOUSE_SELECT_RECTANGLE,
  MOUSE_SELECT_TRIANGLE,
  MOUSE_SELECT_RHOMBUS,
  MOUSE_SELECT_HEXAGON,
  MOUSE_SELECT_STAR,
  MOUSE_SELECT_ELLIPSE,
  MOUSE_PASTE_COMPLETE,
  MOUSE_SELECT_HANDLE_MOVE,

  MOUSE_STATE_MAX
}MouseState;


#define IsAnnotateMouseMode(s) ((MouseState::MOUSE_DRAW_SELECT <= s) && (s <= MouseState::MOUSE_DRAW_SHAPESMART_LAST))
#define IsAnnotateMouseEditMode(s) ((MouseState::MOUSE_DRAW_SELECT < s) && (s <= MouseState::MOUSE_DRAW_SHAPESMART_LAST))

typedef enum tagPageModifiedFlag
{
  NO_MODIFIED,
  META_MODIFIED,
  IMAGE_MODIFIED,
  ALL_MODIFIED
} PageModifiedFlag;

#define SELECTION_IMAGE_BLOCK 1
#define SELECTION_ANNOTATE 2

class KImageDrawer;
using namespace Gdiplus;

typedef struct tagImageDrawInfo
{
  int src_width;
  int src_height;
  float zoom;
  RECT drawRect;
} ImageDrawInfo;

typedef struct tagEditActionBackupData
{
  UINT targetPageId;
  UINT flag;
  HANDLE metadata;
  HANDLE image;
  HANDLE annotate;
}EditActionBackupData;

typedef struct tagPasteImageActionParam
{
  RECT rect;
  HANDLE image;
}PasteImageActionParam;

// xml reader flag
#define ON_ORIGIN_ORDER 1
#define SELECT_INSERTED_OBJECTS 2

//GetMetaDataSet flag
#define META_FOR_STORE 1
#define META_SKIP_EMPTY 2
#define META_INCLUDE_ANNOTATES 4

// permission flag
#define EDITABLE_TRUECOLOR 0x01


class KImageBase
{
public:
  KImageBase(KImageBase* p);
  virtual ~KImageBase();

  virtual void Clear() { ; }
  virtual int GetNumberOfPages(int flag) { return 0; }
  virtual int GetType() { return NONE_PAGE; }

  static UINT gVivaCustomClipboardFormat;

  inline BOOL IsFolded() { return !!(mFlags & IS_FOLDED); }
  inline BOOL IsSelected() { return !!(mFlags & IS_SELECTED); }
  inline KImageBase* GetParent() { return mParent; }
  inline void SetParent(KImageBase* p) { mParent = p; }

  virtual BOOL SetSelectedRecursive(BOOL set) { return SetSelected(set); }
  inline BOOL SetSelected(BOOL set) {
    BOOL changed = (!!(mFlags & IS_SELECTED) == set);
    if (set) mFlags |= IS_SELECTED;
    else mFlags &= ~IS_SELECTED;
    return changed;
  }

  inline void ToggleFolding() {
    if (mFlags & IS_FOLDED) mFlags &= ~IS_FOLDED;
    else mFlags |= IS_FOLDED;
  }

  inline void AddFlag(int flag) { mFlags |= flag; }

  inline void ClearFlag(int flag) { mFlags &= ~flag; }

  inline UINT GetID() { return mID; }

  // permissions
  virtual inline BOOL IsEditable(int flag = 0) { return !!(mPermission & SFP_WRITE); }

  virtual void ClearThumbImage(BOOL bRecurr) { ; }
  virtual KBitmap* GetThumbImage() { return NULL; }

  void SetName(LPCTSTR name);
  inline LPCTSTR GetName() { return (LPCTSTR)mName; }

  void SetParentKeyName(LPCTSTR str);

  void SetEcmInfo(LPCTSTR e);

  void SetFolderNameList(LPTSTR nameList);
  LPTSTR GetFolderNameList() { return mpFolderNameList; }

  virtual void SetModified(BOOL bRecurr, PageModifiedFlag modified);
  BOOL IsModified() { return (mbModified != PageModifiedFlag::NO_MODIFIED); }

  virtual void SetModifiedCache(BOOL bRecurr, PageModifiedFlag modified);
  BOOL IsModifiedCache() { return (mbModifiedCache != PageModifiedFlag::NO_MODIFIED); }

  virtual LPTSTR GetImageFolderPath();

  ImageEncodingFormat GetEncodingFilename(LPCTSTR filename);

  void ClearMetaData(BOOL clearEditOnly);
  void SetMetaData(LPCTSTR key, LPCTSTR str, EcmExtDocTypeColumnType type,
    MetaDataCategory column, LPCTSTR name, BOOL byEdit);
  void SetMetaData(LPCTSTR key, LPBYTE buff, int len, WORD valueType, MetaDataCategory column);
  BOOL SetMetaDataValue(LPCTSTR key, LPCTSTR str);

  MetaKeyNameValue* FindMetaData(LPCTSTR key);
  BOOL RemoveMetaData(LPCTSTR key);
  virtual LPCTSTR GetMetaData(LPCTSTR key);
  virtual LPBYTE GetMetaDataBinary(LPCTSTR key, int& len, MetaDataCategory& category, WORD& vtype);

  virtual void GetMetaDataSet(KVoidPtrArray& list, KEcmDocTypeInfo* info, int flag);
  void GetExtDocTypeInfoSet(KVoidPtrArray& list, int flag, KEcmDocTypeInfo& columns);

  void InitMetaDatas(BOOL onStart);
  void InitMetaDataSet(LPCTSTR pKeys, BOOL skipIfExist, MetaDataCategory category);
  void InsertMetaData(LPTSTR key, LPTSTR value, LPTSTR name, EcmExtDocTypeColumnType type,
    MetaDataCategory category);

  HANDLE BackupMetaKeyValue(LPCTSTR key);

  static HANDLE StoreMetaKeyValue(UINT pid, LPCTSTR key, LPCTSTR str);
  //HANDLE BackupDocExtAttributes();
  int RestoreDocExtAttributes(LPTSTR key);
  inline KMetadataInfo& GeDocExtAttributes() { return mDocExtAttributes; }
  inline void SetDocExtAttributes(KMetadataInfo& info) { mDocExtAttributes = info; }

  HANDLE BackupName();
  static HANDLE AllocStringHandle(LPCTSTR str);

  void SetAcquiredDevice(LPCTSTR deviceName);

  LPTSTR MakeDefaultMetaDataValue(LPCTSTR key);

#ifdef USE_XML_STORAGE
  HRESULT ReadMetaAttributes(IXmlReader* pReader, MetaKeyNameValue* p);
  HRESULT ReadMetaData(CComPtr<IXmlReader> pReader, MetaKeyNameValue* p);

  HRESULT StoreMetaData(CComPtr<IXmlWriter> pWriter);
#else
  virtual DWORD LoadMetaData(KMemoryStream& ms);
  virtual DWORD SaveMetaData(KMemoryStream& ms);
#endif

  virtual int MergeAnnotateObjects() { return 0; }

  int GetDepth();

  virtual MouseState OnLButtonDown(PropertyMode mode, int nFlags, CPOINT& point, KImageDrawer& info, LPRECT ptUpdate) { return MouseState::MOUSE_OFF; }
  virtual int OnLButtonUp(MouseState mouseState, int nFlags, CPOINT& point, KImageDrawer& info, LPRECT ptUpdate) { return 0; }
  virtual int OnMouseMove(MouseState mouseState, int nFlags, POINT& point, KImageDrawer& info, LPRECT ptUpdate) { return 0; }
  virtual int OnMouseMoves(MouseState mouseState, int nFlags, LP_CPOINT points, int count, KImageDrawer& info, LPRECT ptUpdate) { return 0; }
  virtual int OnRButtonDown(PropertyMode mode, int nFlags, POINT& point, KImageDrawer& info, LPRECT ptUpdate) { return 0; }
  virtual int OnRButtonUp(MouseState mouseState, int nFlags, POINT& point, KImageDrawer& info, LPRECT ptUpdate) { return 0; }

  virtual int OnKeyDown(UINT nChar, UINT nFlags, PropertyMode mode, KImageDrawer& info, LPRECT ptUpdate) { return 0; }
  virtual int OnKeyUp(UINT nChar, UINT nFlags, PropertyMode mode, KImageDrawer& info, LPRECT ptUpdate) { return 0; }

  virtual int SelectAll(PropertyMode mode, KImageDrawer& drawer, LPRECT updateRect) { return 0; }
  virtual int HasSelection(int type) { return 0; }
  virtual HANDLE CopyToClipboard(PropertyMode mode, BOOL bCut, int& clipboard_data_format) { return 0; }
  virtual int PasteClipboardData(HANDLE h) { return 0; }
  virtual int DeleteSelection(LPRECT updateRect) { return 0; }

  virtual void DrawEditing(Graphics& g, KImageDrawer& info, PropertyMode mode, int flag) { return; }

  virtual int ApplyEffect(ImageEffectJob* effect, BOOL bAll) { return 0; }

  virtual int MoveToWorkingFolder(LPCTSTR pathName) { return 0; }

  virtual int StoreImageFile(LPCTSTR pathName, KEcmDocTypeInfo* info, LPCTSTR firstFilename, BOOL asTemporaryIfExist) { return 0; }

  virtual int ValidateMetadatas(KEcmDocTypeInfo& info) { return 0;}

#ifdef USE_XML_STORAGE
  virtual HRESULT StoreDocument(CComPtr<IXmlWriter> pWriter, BOOL selectedOnly) { return -2; }
#else
  virtual DWORD StoreDocument(KMemoryStream& ms, BOOL selectedOnly) { return 0; }
#endif

  virtual BOOL RefreshMetaData();

  virtual int ImageAutoRename(LPCTSTR pathName, int& index, int& base) { return 0; }

  virtual void WriteImageName(KMemoryFile& mf);
  virtual int ReadImageName(KMemoryFile& mf);

  virtual void ClearTempFiles(LPCTSTR workingPath) { ; }

  static UINT mGlobImageBaseID;

  UINT mID;
  int mFlags;
  UINT mPermission;

  PageModifiedFlag mbModified;
  PageModifiedFlag mbModifiedCache;
  KImageBase* mParent;
  LPTSTR mName;

  KMetadataInfo mDocExtAttributes;

  LPTSTR mpParentKeyName;
  LPTSTR mpEcmInfo;
  LPTSTR mpFolderNameList;

protected:
  void appendMetaData(KVoidPtrArray& list, LPTSTR key, LPCTSTR name, EcmExtDocTypeColumnType type, MetaDataCategory category, LPCTSTR str, int flag);
  void appendMetaData(KVoidPtrArray& list, LPTSTR key, LPCTSTR name, LPBYTE buff, int len, MetaDataCategory category, WORD vtype, int flag);

  void appendExtDocTypeInfo(KVoidPtrArray& list, KEcmDocTypeInfo& columns, int flag);
  void appendMetaDataSet(KVoidPtrArray& list, LPCTSTR pKeys, MetaDataCategory category, int flag);
  void appendMetaDatas(KVoidPtrArray& list, int flag);

  LPTSTR getCustomMetaString(KEcmDocTypeInfo& columns, LPTSTR buff, int flag);

  LPTSTR getSeparatMetaString(LPCTSTR pKeys, LPTSTR buff, int flag);
};

#include <vector>

typedef std::vector<KImageBase*> KImageDocVector;
typedef std::vector<KImageBase*>::iterator KImageDocIterator;
typedef std::vector<KImageBase*>::reverse_iterator KImageDocRevIterator;

extern LPCTSTR gDefaultMetaKeysPredefIfd;
extern LPCTSTR gDefaultMetaKeysPredefExif;
extern LPCTSTR IFDTagFolderPathInfo;
extern LPCTSTR TagFolderListInfo;

extern LPCTSTR gTiffTagImageWidth;
extern LPCTSTR gTiffTagImageHeight;
extern LPCTSTR gTiffTagImageBitCount;
//extern LPCTSTR gMetaKeyUserComment;
//extern LPCTSTR gTiffTagDocTypeInfo;
//extern LPCTSTR gTiffTagMetadataInfo;

extern LPCTSTR GetImageFileExt(ImagingSaveFormat saveFormat);
extern LPCTSTR GetDocumentFileExt(ImagingSaveFormat saveFormat);
extern BOOL HasValidFileExtension(LPCTSTR pathName, BOOL withXml);
extern LPCTSTR GetDefaultImageFileExt();
