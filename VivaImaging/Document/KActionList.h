/**
* @file KActionList.h
* @date 2019.02
* @copyright k2mobile.co.kr
* @brief KActionList class(액션 리스트를 괸리하는 클래스) header file
*/
#pragma once

#include "KEditAction.h"

class KActionList
{
public:
  KActionList();
  ~KActionList();

  void Clear();

  /**
  * @brief return TRUE if it can undo
  */
  BOOL CanUndo();

  /**
  * @brief return TRUE if it can redo
  */
  BOOL CanRedo();

  int GetUndoCount();
  KEditAction* GetAction(int index);

  /**
  * @brief save new action item
  */
  void SaveAction(EditActionType type, LPBYTE pParam, int p_length, LPBYTE pBackup, int b_length);
  void SaveAction(EditActionType type, HANDLE pHandle, HANDLE bHandle);
  void SaveAction(KEditAction* a);

  /**
  * @brief Undo를 실행하고 그 액션을 리턴한다.
  */
  KEditAction* Undo();
  /**
  * @brief Redo를 실행하고 그 액션을 리턴한다.
  */
  KEditAction* Redo();

  /**
  * @brief 이 ActionList에서 사용하는 메모리 크기를 체크해서 한도를 넘으면 오래된 항목은 삭제한다.
  */
  void CheckBuferLimit();
  void Dump();

  /**
  * @brief 이 Action 항목을 저장하는 List
  */
  KActionVector mActionList;
  /**
  * @brief 이 ActionList에서 current action pointer
  */
  UINT mActionIndex;

  /**
  * @brief 이 ActionList에서 사용할 메모리 크기 한도.
  */
  __int64 mMaxBufferSize;

};

