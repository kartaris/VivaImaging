/**
* @file KEditAction.h
* @date 2019.02
* @copyright k2mobile.co.kr
* @brief KEditAction class(편집 액션을 담당하는 클래스) header file
*/
#pragma once

typedef enum tagEditActionType{
  EA_InsertPage,
  EA_DeletePage,
  EA_InsertFolder,
  EA_DeleteFolder,
  EA_MovePage,
  EA_ModifyMetaData,

  /*
  PModeImageView = 0,
  PModeRotate,
  PModeResize,
  PModeCrop,

  PModeEditImage,

  PModeMonochrome,
  PModeColorBalance,
  PModeColorLevel,
  PModeColorBright,
  PModeColorHLS,
  PModeColorHSV,
  PModeColorReverse,

  PModeAnnotateSelect,
  PModeAnnotateLine,
  PModeAnnotateFreehand,
  PModeAnnotateRectangle,
  PModeAnnotateEllipse,
  PModeAnnotatePolygon,
  PModeAnnotateTextbox,
  PModeAnnotateEditText,

  */

  EA_ImageEffect,
  EA_Rotate,
  EA_Resize,
  EA_Crop,
  EA_PModeEditImage, // not-used
  EA_Monochrome,
  EA_ColorBalance,
  EA_ColorLevel,
  EA_ColorBright,
  EA_ColorHLS,
  EA_ColorHSV,
  EA_ColorReverse,

  EA_SelectRectangle,
  EA_SelectTriangle,
  EA_SelectRhombus,
  EA_SelectPentagon,
  EA_SelectStar,
  EA_SelectEllipse,
  EA_SelectFill,
  EA_CopyRectangle,
  EA_CopyTriangle,
  EA_CopyRhombus,
  EA_CopyPentagon,
  EA_CopyStar,
  EA_CopyEllipse,
  EA_PasteImage,

  EA_AnnotateAddLine,
  EA_AnnotateAddFreehand,
  EA_AnnotateAddFreeshape,
  EA_AnnotateAddRectangle,
  EA_AnnotateAddEllipse,
  EA_AnnotateAddTextbox,
  EA_AnnotateAddShapeSmart,
  EA_AnnotateDelete,
  EA_AnnotateMove,
  EA_AnnotateResize,
  EA_AnnotateEdit, // EditText/ChangeProperty
  EA_AnnotateAlign,
  EA_AnnotateOrder,
  EA_PasteAnnotates,
  EA_AnnotateProperty, // end of Annotate action

  EA_AutoRename,
  EA_ModifyName,
  EA_ModifyDocType,
  EA_Max
} EditActionType;

class KEditAction
{
public:
  KEditAction(EditActionType type, HGLOBAL pParam = NULL, HGLOBAL pBackup = NULL);
  ~KEditAction();

  static HGLOBAL CopyActionBuffer(HGLOBAL h);
  static HGLOBAL CopyActionBuffer(LPBYTE buff, UINT length);

  void StoreParamData(LPBYTE buff, UINT length);
  void StoreBackupData(LPBYTE buff, UINT length);

  __int64 GetMemoryUsed();

  // DEBUG information
  void ToString(LPTSTR msg, int length);

  // undo listbox
  inline EditActionType GetType() { return mType; }

  EditActionType mType;
  HGLOBAL mParam;
  HGLOBAL mBackup;
};

#include <vector>

typedef std::vector<KEditAction*> KActionVector;
typedef std::vector<KEditAction*>::iterator KActionIterator;

extern void ClearActionVector(KActionVector& list);