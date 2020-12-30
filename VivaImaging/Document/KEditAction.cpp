#include "stdafx.h"
#include "KEditAction.h"
#include "KImageBase.h"
#include "../Core/HttpDownloader/KMemory.h"
#include "../Platform/Platform.h"

KEditAction::KEditAction(EditActionType type, HGLOBAL  pParam, HGLOBAL pBackup)
  : mType(type), mParam(pParam), mBackup(pBackup)
{
}

KEditAction::~KEditAction()
{
  if (mParam != NULL)
  {
    if (mType == EA_PasteImage)
    {
      PasteImageActionParam* pr = (PasteImageActionParam*)KMemoryLock(mParam);
      if (pr->image != NULL)
        GlobalFree(pr->image); // this is GlobalAlloc memory
    }
    KMemoryFree(mParam);
    mParam = NULL;
  }
  if (mBackup != NULL)
  {
    if ((EA_ImageEffect <= mType) && (mType <= EA_AnnotateProperty))
    {
      int count = KMemorySize(mBackup) / sizeof(EditActionBackupData);
      EditActionBackupData* ed = (EditActionBackupData*)KMemoryLock(mBackup);
      if (ed != NULL)
      {
        for (int i = 0; i < count; i++)
        {
          if (ed->image != NULL)
            GlobalFree(ed->image);
          if (ed->annotate != NULL)
            KMemoryFree(ed->annotate);
          ed++;
        }
      }
    }

    KMemoryFree(mBackup);
    mBackup = NULL;
  }
}

HGLOBAL KEditAction::CopyActionBuffer(HGLOBAL h)
{
  LPVOID sp = GlobalLock(h);
  UINT length = KMemorySize(h);

  HGLOBAL nh = KMemoryAlloc(length);
  LPVOID np = KMemoryLock(nh);
  memcpy(np, sp, length);

  KMemoryUnlock(nh);
  GlobalUnlock(h);
  return nh;
}

HGLOBAL KEditAction::CopyActionBuffer(LPBYTE buff, UINT length)
{
  HGLOBAL nh = KMemoryAlloc(length);
  LPVOID np = KMemoryLock(nh);
  memcpy(np, buff, length);

  KMemoryUnlock(nh);
  return nh;
}

void KEditAction::StoreParamData(LPBYTE buff, UINT length)
{
  if (mParam != NULL)
    KMemoryFree(mParam);
  mParam = CopyActionBuffer(buff, length);
}

void KEditAction::StoreBackupData(LPBYTE buff, UINT length)
{
  if (mBackup != NULL)
    KMemoryFree(mBackup);
  mBackup = CopyActionBuffer(buff, length);
}

__int64 KEditAction::GetMemoryUsed()
{
  __int64 size = 0;

  if (mParam != NULL)
  {
    if (mType == EA_PasteImage)
    {
      PasteImageActionParam* pr = (PasteImageActionParam*)KMemoryLock(mParam);
      if (pr->image != NULL)
        size += GlobalSize(pr->image); // this is GlobalAlloc memory
    }
    size += KMemorySize(mParam);
  }
  if (mBackup != NULL)
  {
    if ((EA_ImageEffect <= mType) && (mType <= EA_AnnotateProperty))
    {
      int count = KMemorySize(mBackup) / sizeof(EditActionBackupData);
      EditActionBackupData* ed = (EditActionBackupData*)KMemoryLock(mBackup);
      if (ed != NULL)
      {
        for (int i = 0; i < count; i++)
        {
          if (ed->image != NULL)
            size += GlobalSize(ed->image);
          if (ed->annotate != NULL)
            size += KMemorySize(ed->annotate);
          ed++;
        }
      }
    }

    size += KMemorySize(mBackup);
  }

  return size;
}

void KEditAction::ToString(LPTSTR buff, int length)
{
  static LPTSTR actionName[] = {
    _T("Insert Page"),
    _T("Delete Page"),
    _T("Insert Folder"),
    _T("Delete Folder"),
    _T("Move Page"),
    _T("Modify MetaData"),

    _T("Image Effect"),
    _T("Rotate"),
    _T("Resize"),
    _T("Crop"),

    _T("EditImage"),
    _T("Monochrome"),
    _T("ColorBalance"),
    _T("ColorLevel"),
    _T("ColorBright"),
    _T("ColorHLS"),
    _T("ColorHSV"),
    _T("ColorReverse"),

    _T("Select Rectangle"),
    _T("Select Triangle"),
    _T("Select Rhombus"),
    _T("Select Pentagon"),
    _T("Select Star"),
    _T("Select Ellipse"),
    _T("Select Fill"),
    _T("Copy Rectangle area"),
    _T("Copy Triangle area"),
    _T("Copy Rhombus area"),
    _T("Copy Pentagon area"),
    _T("Copy Star area"),
    _T("Copy Ellipse area"),
    _T("Paste Image"),

    _T("Add Line Annotate"),
    _T("Add Freehand Annotate"),
    _T("Add Freeshape Annotate"),
    _T("Add Rectangle Annotate"),
    _T("Add Ellipse Annotate"),
    _T("Add Textbox Annotate"),
    _T("Delete Annotate"),
    _T("Move Annotate"),
    _T("Resize Annotate"),
    _T("Edit Annotate"),
    _T("Align Annotate"),
    _T("Order Annotate"),
    _T("Change Annotate Property"),
    _T("Paste Annotates"),

    _T("Auto Rename"),
    _T("Edit Name"),
    _T("Edit DocType"),

    _T("EA_UnknownAction")
  };

  int count_name = sizeof(actionName) / sizeof(LPTSTR);
  int idx = (int)mType;

  if ((idx >= 0) && (idx < count_name))
  {
    StringCchCopy(buff, length, actionName[idx]);
  }
  else
  {
    StringCchCopy(buff, length, actionName[count_name-1]);
  }

#ifdef _DEBUG
  TCHAR sizeStr[16];
  FormatFileSize(sizeStr, 16, GetMemoryUsed());
  StringCchCat(buff, length, _T("-"));
  StringCchCat(buff, length, sizeStr);
#endif
}

void ClearActionVector(KActionVector& list)
{
  KActionIterator it = list.begin();
  while (it != list.end())
  {
    KEditAction* a = *it++;
    delete a;
  }
  list.clear();
}
