#include "stdafx.h"
#include "KShapeEditor.h"
#include "../Core/KSettings.h"
#include "../Platform/Graphics.h"

KShapeProperty KShapeEditor::mDefault;
KHWndVector KShapeEditor::mNotifyTarget;

KShapeEditor::KShapeEditor()
{
  mGroupSelectMode = FALSE;
  mButtonDownPt = POINT{ 0,0 };
  mHandleType = EditHandleType::HandleNone;
}

KShapeEditor::~KShapeEditor()
{
	Clear();
}

void KShapeEditor::Clear()
{
	KShapeContainer::Clear();
}

void KShapeEditor::ClearStatic()
{
  mNotifyTarget.clear();
}

void KShapeEditor::StoreDefaultProperty()
{
  mDefault.StoreProperty();
}

void KShapeEditor::LoadDefaultProperty()
{
  mDefault.LoadProperty();
}

void KShapeEditor::RegisterReceiveEvent(HWND hWnd)
{
  KHwndIterator it = mNotifyTarget.begin();
  while (it != mNotifyTarget.end())
  {
    if (*it == hWnd)
      return;
    ++it;
  }
  mNotifyTarget.push_back(hWnd);
}

void KShapeEditor::SendEventNotify(SHAPE_EVENT_TYPE event, LPVOID param)
{
  KHwndIterator it = mNotifyTarget.begin();
  while (it != mNotifyTarget.end())
  {
    HWND hWnd = *it;
    if (IsWindow(hWnd))
      ::SendMessage(hWnd, WM_SHAPEEDIT_NOTIFY, (WPARAM)event, (LPARAM)param);
    ++it;
  }
}

int KShapeEditor::MoveObjects(POINT offset)
{
	int count = 0;
	KShapeIterator it = mShapeList.begin();
	while (it != mShapeList.end())
	{
		KShapeBase* s = *it;
		if (s->IsSelected())
		{
			s->Move(offset);
			++count;
		}
		it++;
	}
	return count;
}

/**
* @brief 지정한 objectId 목록에 해당하는 개체를 지정한 크기로 변경한다.
* @param offset : 이동할 양
* @return int : 이동된 개체의 개수를 리턴한다.
*/
int KShapeEditor::ResizeObjects(POINT offset)
{
	int count = 0;
	KShapeIterator it = mShapeList.begin();
	while (it != mShapeList.end())
	{
		KShapeBase* s = *it;
		if (s->IsSelected())
		{
			s->ResizeByObjectHandle(mHandleType, offset);
			++count;
		}
		it++;
	}
	return count;
}

/**
* @brief 지정한 objectId 목록에 해당하는 개체의 특수 핸들을 지정한 위치로 변경한다.
* @param objectId : 대상 개체의 id 리스트
* @param type : 이동하는 핸들 종류
* @param offset : 이동할 양
* @param keyState : 키보드 상태(Shift, Ctrl, Alt)
* @return int : 이동된 개체의 개수를 리턴한다.
*/
int KShapeEditor::HandleEdit(KIntVector& objectId, EditHandleType type, POINT offset, int keyState)
{
	int count = 0;
	KShapeIterator it = mShapeList.begin();
	while (it != mShapeList.end())
	{
		KShapeBase* s = *it;
		if (DoesListContainValue(objectId, s->ObjectId))
		{
			if (s->HandleEdit(type, offset, keyState))
				++count;
		}
		it++;
	}
	return count;
}

/**
* @brief 선택된 개체의 개수를 리턴한다.
*/
int KShapeEditor::GetSelectedObjectsCount(BOOL bAll)
{
	int count = 0;
	KShapeIterator it = mShapeList.begin();
	while (it != mShapeList.end())
	{
		KShapeBase* s = *it;
		if (s->IsSelected())
		{
			count++;
			if (!bAll && (count >= 2)) // 2 이상이면 더이상 카운트할 필요 없음.
				return count;
		}
		it++;
	}
	return count;
}

/**
* @brief 선택된 개체의 목록을 리턴한다.
*/
int KShapeEditor::GetSelectedObjects(KShapeVector& list)
{
	KShapeIterator it = mShapeList.begin();
	while (it != mShapeList.end())
	{
		KShapeBase* s = *it;
		if (s->IsSelected())
		{
			list.push_back(s);
		}
		it++;
	}
	return (int)list.size();
}

/**
* @brief 선택된 개체의 목록을 리턴한다.
*/
int KShapeEditor::GetSelectedObjects(KIntVector& list)
{
	KShapeIterator it = mShapeList.begin();
	while (it != mShapeList.end())
	{
		KShapeBase* s = *it;
		if (s->IsSelected())
		{
			list.push_back(s->ObjectId);
		}
		it++;
	}
	return (int)list.size();
}

KShapeBase* KShapeEditor::GetLastSelectedObject()
{
  KShapeBase* selected = NULL;
  KShapeIterator it = mShapeList.begin();
  while (it != mShapeList.end())
  {
    KShapeBase* s = *it;
    if (s->IsSelected())
      selected = s;
    it++;
  }
  return selected;
}


/**
* @brief 선택된 개체들의 bounding rect와 개수를 리턴한다.
*/
int KShapeEditor::GetSelectedObjectsBound(RECT& rect, KShapeVector* pList)
{
  int count = 0;
  KShapeIterator it = mShapeList.begin();
  while (it != mShapeList.end())
  {
    KShapeBase* s = *it;
    if (s->IsSelected())
    {
      RECT r;

      if (pList != NULL)
        pList->push_back(s);

      s->GetRectangle(r);
      if (count == 0)
        rect = r;
      else
        OrRect(rect, r);

      ++count;
    }
    ++it;
  }
  return count;
}

/**
* @brief 지정한 위치에 개체의 핸들, 개체, 가이드라인이 있는지 확인한다.
* @param pt : 입력 좌표값
* @param freeIfNotSelected : true이고 대상 개체가 선택되지 않은 상태이면 새로 선택한다.
* @param editActionData : 선택된 핸들 정보를 담아 리턴할 버퍼
* @return EditHandleType : 선택된 핸들의 종류
* @details A. 선택된 개체의 핸들에 해당하는지 검사하고 맞으면 그 핸들 type을 리턴한다.
* @n B. 모든 개체에 대해 HitOver가 맞으면 MoveObject를 리턴한다.
*/
EditHandleType KShapeEditor::GetSelectedObjectsHandle(POINT pt, BOOL freeIfNotSelected, KImageDrawer& drawer, KDragAction& editActionData)
{
  if (mDragAction.mbGroupSelected)
  {
    KShapeVector list;
    RECT rect;
    if (GetSelectedObjectsBound(rect, &list) > 1)
    {
      EditHandleType a = drawer.HitRectangleHandle(rect, pt);
      if (a != EditHandleType::HandleNone)
      {
        mDragAction.StartDragAction(MouseDragMode::ResizeGroupObject, a, pt, list, rect);
        return a;
      }
    }
  }
  else
  {
    // check if selected objects handle
    KShapeIterator it = mShapeList.begin();
    while (it != mShapeList.end())
    {
      KShapeBase* s = *it;
      if (s->IsVisible() && !s->IsLocked() && s->IsSelected())
      {
        EditHandleType a = s->HitObjectsHandle(drawer, pt);
        if (a != EditHandleType::HandleNone)
        {
          MouseDragMode m = KImageDrawer::IsObjectHandle(a) ? MouseDragMode::HandleObject : MouseDragMode::ResizeObject;
          editActionData.StartDragAction(m, a, pt, s, ShapeType::ShapeNone);

          TCHAR msg[128];
          StringCchPrintf(msg, 128, _T("StartDrag mode=%d, handle=%d\n"), m, a);
          OutputDebugString(msg);

          return a;
        }
      }
      it++;
    }
  }

  // check if selected objects over
  KShapeIterator it = mShapeList.begin();
  while (it != mShapeList.end())
  {
    KShapeBase* s = *it;
    if (s->IsVisible() && !s->IsLocked() && s->IsSelected())
    {
      if (s->HitOver(drawer, pt) > EditHandleType::HandleNone)
      {
        editActionData.StartDragAction(MouseDragMode::MoveObject, EditHandleType::MoveObjectHandle, pt, s, ShapeType::ShapeNone);
        return EditHandleType::MoveObjectHandle;
      }
    }
    it++;
  }

	// check inside of objects
  KShapeBase* s = FindOverObject(pt, drawer);
  if (s != NULL)
  {
    // 대상 개체가 선택되지 않은 상태이면 선택 해제하고 선택한다.
    if (s->IsSelected() == false)
    {
      if (freeIfNotSelected)
        FreeAllSelection(NULL);
      s->SetShapeFlag(true, KSHAPE_SELECTED);
      OnSelectionChanged(s);
    }

    editActionData.StartDragAction(MouseDragMode::MoveObject, EditHandleType::MoveObjectHandle, pt, s, ShapeType::ShapeNone);
    return EditHandleType::MoveObjectHandle;
  }

	// check horizontal guidelines
#if 0
	if (H_GuideLines != null)
	{
		int index = 0;
		foreach(double y in H_GuideLines)
		{
			if (Math.Abs(y - pt.Y) <= GUIDELINE_PICK_DISTANCE)
			{
        editActionData.StartDragAction(MoveGuideline, MoveHorzGuide, pt, s, index);
        //editActionData.targetIndex = index;
				//editActionData.handleType = EditHandleType.MoveHorzGuide;
				return EditHandleType.MoveHorzGuide;
			}
			++index;
		}
	}

	// check vertical guidelines
	if (V_GuideLines != null)
	{
		int index = 0;
		foreach(double x in V_GuideLines)
		{
			if (Math.Abs(x - pt.X) < GUIDELINE_PICK_DISTANCE)
			{
        editActionData.StartDragAction(MoveGuideline, MoveVertGuide, pt, s, index);
        //editActionData.targetIndex = index;
				//editActionData.handleType = EditHandleType.MoveVertGuide;
				return EditHandleType.MoveVertGuide;
			}
			++index;
		}
	}
#endif
	return EditHandleType::HandleNone;
}

BOOL KShapeEditor::IsOverSelectedObject(POINT pt, KImageDrawer& drawer)
{
  // check if selected objects over
  KShapeIterator it = mShapeList.begin();
  while (it != mShapeList.end())
  {
    KShapeBase* s = *it;
    if (s->IsVisible() && !s->IsLocked() && s->IsSelected())
    {
      if (s->HitOver(drawer, pt) > EditHandleType::HandleNone)
      {
        return TRUE;
      }
    }
    it++;
  }
  return FALSE;
}

KShapeBase* KShapeEditor::FindOverObject(POINT pt, KImageDrawer& drawer)
{
  // check if selected objects over
  KShapeIterator it = mShapeList.begin();
  while (it != mShapeList.end())
  {
    KShapeBase* s = *it;
    if (s->IsVisible() && !s->IsLocked() && s->IsSelected())
    {
      if (s->HitOver(drawer, pt) > EditHandleType::HandleNone)
      {
        return s;
      }
    }
    it++;
  }
  return NULL;
}

/**
* @brief 선택된 개체들을 모두 해제한다.
* @details A. 전체 개체 리스트에서 각 개체의 플래그에 선택 플래그가 있으면 해제하고 해제된 개체 개수를 리턴한다.
*/
int KShapeEditor::FreeAllSelection(LPRECT ptRect)
{
	int count = 0;
	KShapeIterator it = mShapeList.begin();
	while (it != mShapeList.end())
	{
		KShapeBase* s = *it++;
		if (s->IsSelected())
		{
			if (ptRect != NULL)
			{
				RECT rect = s->GetBounds(NULL);
				if (count == 0)
				{
					memcpy(ptRect, &rect, sizeof(RECT));
				}
				else
				{
					AddRect(*ptRect, rect);
				}
			}

			s->ClearSelected();
			count++;
		}
	}
  if (count > 0)
    OnSelectionChanged(NULL);

	return count;
}

int KShapeEditor::SelectAll(LPRECT ptRect)
{
	int count = 0;
	KShapeIterator it = mShapeList.begin();
  KShapeBase* last = NULL;
	while (it != mShapeList.end())
	{
		KShapeBase* s = *it++;
    if (s->IsVisible() && !s->IsLocked())
    {
      if (!s->IsSelected())
      {
        s->SetSelected();
        if (ptRect != NULL)
        {
          RECT rect = s->GetBounds(NULL);
          if (count == 0)
          {
            memcpy(ptRect, &rect, sizeof(RECT));
          }
          else
          {
            AddRect(*ptRect, rect);
          }
        }
        last = s;
        count++;
      }
    }
	}
  if (count > 0)
    OnSelectionChanged(last);
  return count;
}

int KShapeEditor::SetSelected(KVoidPtrArray& shapeList)
{
  FreeAllSelection(NULL);
  KVoidPtrIterator it = shapeList.begin();
  while (it != shapeList.end())
  {
    KShapeBase* s = (KShapeBase*)*it++;
    s->SetSelected();
  }
  return shapeList.size();
}

int KShapeEditor::AddSelectionBy(POINT& pt, BOOL freeIfOut)
{
  int count = 0;
  KShapeRevIterator it = mShapeList.rbegin();
  KShapeBase* last = NULL;

  while (it != mShapeList.rend())
  {
    KShapeBase* s = *it++;
    if (s->IsVisible() && !s->IsLocked())
    {
      if (s->HitOver(pt))
      {
        s->SetShapeFlag(true, KSHAPE_SELECTED);
        count++;
        break;
      }
    }
  }
  if (count > 0)
    OnSelectionChanged(last);
  return count;
}

/**
* @brief 지정한 영역 내의 개체들을 모두 선택한다.
* @details A. 전체 개체 리스트에서 각 개체의 레이어가 visible이고, 잠금 상태가 아니면,
* @n      - 개체의 bound가 지정한 사각형 내부이면 선택 플래그를 셋트하고, 바깥이고 선택된 상태이면 선택을 해제한다.
* @n B. 선택이 바뀐 개체의 개수를 리턴한다.
*/
int KShapeEditor::AddSelectionBy(RECT& rect, BOOL freeIfOut)
{
	int count = 0;
	KShapeIterator it = mShapeList.begin();
  KShapeBase* last = NULL;

	while (it != mShapeList.end())
	{
		KShapeBase* s = *it++;
		if (s->IsVisible() && !s->IsLocked())
		{
			if (RectContains(rect, s->GetBounds(NULL)))
			{
				s->SetShapeFlag(true, KSHAPE_SELECTED);
				count++;
        last = s;
			}
			else if (freeIfOut && s->IsSelected())
			{
				s->ClearSelected();
				count++;
			}
		}
		else if (s->IsSelected())
		{
			s->ClearSelected();
			count++;
		}
	}
  if (count > 0)
    OnSelectionChanged(last);
  return count;
}

int KShapeEditor::GetSelectionBound(RECT& area)
{
  int count = 0;
  KShapeIterator it = mShapeList.begin();
  while (it != mShapeList.end())
  {
    KShapeBase* s = *it++;
    if (s->IsVisible() && !s->IsLocked() && s->IsSelected())
    {
      RECT rect = s->GetBounds(NULL);
      if (count == 0)
        memcpy(&area, &rect, sizeof(RECT));
      else
        AddRect(area, rect);
      count++;
    }
  }
  return count;
}

int KShapeEditor::SelectNextObject(KShapeVector& selectedItems ,BOOL bNext)
{
  int count = 0;
  KShapeBase* last_candidate = NULL;
  KShapeBase* first_candidate = NULL;
  KShapeBase* next_candidate = NULL;
  int selected_found = 0;

  KShapeIterator it = mShapeList.begin();
  while (it != mShapeList.end())
  {
    KShapeBase* s = *it++;
    if (s->IsVisible() && !s->IsLocked())
    {
      if (s->IsSelected())
      {
        s->ClearSelected();
        selected_found++;
      }
      else
      {
        last_candidate = s;
        if (first_candidate == NULL)
          first_candidate = s;
        if (selected_found)
          next_candidate = s;
      }

      if (selected_found)
      {
        if (bNext && (next_candidate != NULL))
        {
          next_candidate->SetSelected();
          selectedItems.push_back(next_candidate);
          return 1;
        }
        if (!bNext && (selected_found == 1) && (last_candidate != NULL))
        {
          last_candidate->SetSelected();
          selectedItems.push_back(last_candidate);
          return 1;
        }
        selected_found++;
      }
    }
  }

  if (bNext && (first_candidate != NULL))
  {
    first_candidate->SetSelected();
    selectedItems.push_back(first_candidate);
    return 1;
  }
  if (!bNext && (last_candidate != NULL))
  {
    last_candidate->SetSelected();
    selectedItems.push_back(last_candidate);
    return 1;
  }
  return 0;
}


/**
* @brief 지정한 개체의 순서를 맨 위로 올린다.
* @param targets : 대상 개체들의 objectID 리스트.
* @details A. 스프레드 개체중
* @n      - 대상 개체 리스트에 속하고  selected_top이 음이면 이 개체의 순서를 selected_top으로 설정한다.
* @n      - 속하지 않고 selected_top이 유효하면 unselected_top에 지금의 순서를 넣는다.
* @n      - unselected_top은 selected_top 하위 개체중 선택 안된 첫번째 개체임.
* @n B. unselected_top이 유효하면,
* @n      - 대상 개체들을 임시 버퍼 target_items로 이동한다.
* @n      - target_items의 개체들을 GraphicList의 뒤에 추가한다.
* @n      - target_items를 해제하고, 그 개수를 리턴한다.
*/
int KShapeEditor::BringToTop(KShapeVector& targets)
{
	int n = 0;
	int selected_top = -1;
	int unselected_top = -1;

	// 선택된 개체중 첫번째 것과 그 아래중 선택 안된 개체의 order를 찾는다.
	KShapeIterator it = mShapeList.begin();
	while (it != mShapeList.end())
	{
		KShapeBase* s = *it;
		if (DoesListContainValue(targets, s))
		{
			if (selected_top < 0)
				selected_top = n;
		}
		else
		{
			if (selected_top >= 0)
				unselected_top = n;
		}
		++n;
		++it;
	}

	n = 0;
	if (unselected_top > 0)
	{
		// 대상 개체를 일단 임시 버퍼로 이동
		KShapeVector target_items;

		KShapeIterator it = mShapeList.begin();
		while (it != mShapeList.end())
		{
			KShapeBase* s = *it;
			if (DoesListContainValue(targets, s))
			{
				it = mShapeList.erase(it);
				target_items.push_back(s);
			}
			else
			{
				++it;
			}
		}

		// 다시 뒤에 추가
		it = target_items.begin();
		while (it != target_items.end())
		{
			KShapeBase* s = *it++;
			mShapeList.push_back(s);
		}
		n = target_items.size();
		target_items.clear();
	}
	return n;
}

/**
* @brief 지정한 개체의 순서를 맨 아래로 내린다.
* @param targets : 대상 개체들의 objectID 리스트.
* @details A. 스프레드 개체중
* @n      - 대상 개체 리스트에 속하고  target_order가 유효하면( >= 0) 이 개체의 id를 moving_items에 넣는다.
* @n      - 속하지 않으며 target_order가 음이면 현재의 순서를 target_order로 한다.
* @n B. 현재의 순서를 유지하도록 임시 리스트에 대상 개체를 옮긴다.(targets의 순서는 무작위임)
* @n C. order를 하나 올리고 그것이 0보다 크면,
*       - 대상 개체들을 order 아래로 이동한다.
*/
int KShapeEditor::SendToBottom(KShapeVector& targets)
{
	int target_order = -1;
	int n = 0;
	KShapeVector moving_items;

	// 들어갈 order와 이동될 개체들 목록을 준비한다.
	KShapeIterator it = mShapeList.begin();
	while (it != mShapeList.end())
	{
		KShapeBase* s = *it++;
		if (DoesListContainValue(targets, s))
		{
			if (target_order >= 0)
				moving_items.push_back(s);
		}
		else
		{
			if (target_order < 0)
				target_order = n;
		}
		n++;
	}

	n = 0;
	if ((target_order >= 0) && (moving_items.size() > 0))
	{
    int index = 0;
		KShapeIterator ait = moving_items.begin();
		while (ait != moving_items.end())
		{
      KShapeBase* s = *ait++;
			DetachChildObject(s);
			if (s != NULL)
			{
				it = mShapeList.begin();
				if (target_order > 0)
					it += target_order;
				mShapeList.insert(it, s);
				target_order++;
				n++;
			}
			else
			{
				TCHAR msg[120] = _T("Cannot found object on BringToTop : ");
				int slen = lstrlen(msg);
				StringCchPrintf(&msg[slen], 120 - slen, _T("%d"), index);
				StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG | SYNC_MSG_DEBUG);
			}
      index++;
		}
		moving_items.clear();
	}
	return n; // number of items which are moved
}

/**
* @brief 지정한 개체의 순서를 위로 올린다.
* @param targets : 대상 개체들의 objectID 리스트.
* @details A. 스프레드 개체중
* @n      - 대상 개체 리스트에 속하면 임시로  moving_items에 이동한다.
* @n      - 속하지 않으며 moving_item에 들어있는 개체들을 이 개체 다음에 삽입한다.
* @n B. moving_item에 남아있는 개체들을 리스트 뒤에 삽입한다.
*/
int KShapeEditor::BringForward(KShapeVector& targets)
{
	KShapeVector moving_items;
	int n = 0;
  int selected_first = -1;

	// 선택된 개체이면 moving_item으로 옮기고,
	KShapeIterator it = mShapeList.begin();
	while (it != mShapeList.end())
	{
		KShapeBase* s = *it;
		if (DoesListContainValue(targets, s))
		{
			moving_items.push_back(s);
			it = mShapeList.erase(it);
      if (selected_first < 0)
        selected_first = n;
		}
		else
		{
      ++it;
    }
    n++;
	}

  // 이동 항목은 selected_first 앞에 붙인다.
  n = 0;
  if (moving_items.size() > 0)
  {
    if (selected_first < (int)mShapeList.size())
      selected_first++;

    KShapeIterator it = mShapeList.begin();
    if (selected_first > 0)
      it += selected_first;

    KShapeIterator cit = moving_items.begin();
    while (cit != moving_items.end())
    {
      KShapeBase* c = *cit++;
      it = mShapeList.insert(it, c);
    }

		n = moving_items.size();
		moving_items.clear();
	}
	return n;
}

/**
* @brief 지정한 개체의 순서를 아래로 내린다.
* @param targets : 대상 개체들의 objectID 리스트.
* @details A. 스프레드 개체중 역순으로
* @n      - 대상 개체 리스트에 속하면 임시로  moving_items에 이동한다.
* @n      - 속하지 않으며 moving_item에 들어있는 개체들을 이 개체 앞에 삽입한다.
* @n B. moving_item에 남아있는 개체들을 리스트 맨 앞에 삽입한다.
*/
int KShapeEditor::SendBackward(KShapeVector& targets)
{
  KShapeVector moving_items;
  int n = 0;
  int selected_first = -1;

  // 선택된 개체이면 moving_item으로 옮기고,
  KShapeIterator it = mShapeList.begin();
  while (it != mShapeList.end())
  {
    KShapeBase* s = *it;
    if (DoesListContainValue(targets, s))
    {
      moving_items.push_back(s);
      it = mShapeList.erase(it);
      if (selected_first < 0)
        selected_first = n;
    }
    else
    {
      ++it;
    }
    n++;
  }

  // 이동 항목은 selected_first 다음에 붙인다.
  n = 0;
  if (moving_items.size() > 0)
  {
    if (selected_first > 0)
      selected_first--;

    KShapeIterator it = mShapeList.begin();
    if (selected_first > 0)
      it += selected_first;

    KShapeIterator cit = moving_items.begin();
    while (cit != moving_items.end())
    {
      KShapeBase* c = *cit++;
      it = mShapeList.insert(it, c);
    }

    n = moving_items.size();
    moving_items.clear();
  }
	return n;
}

/**
* @brief 지정한 개체를 왼쪽 정렬한다.
* @param targets : 대상 개체들의 objectID 리스트.
* @details A. 대상 개체가 하나이면 스프레드 왼쪽 테두리에 붙도록 X좌표를 맞춘다.
* @n B. 대상 개체가 하나 이상이면,
* @n      - 가장 왼쪽인 좌표를 찾고
* @n      - 대상 개체들을 가장 왼쪽 좌표로 이동한다.
*/
int KShapeEditor::AlignLeft(KShapeVector& targets)
{
  RECT rect;
  if (targets.size() == 1)
	{
		KShapeBase* c = targets.at(0);
		if (c != NULL)
		{
      c->GetNormalRectangle(rect);
			POINT p = { -rect.left, 0 };
			c->Move(p);
		}
		return 1;
	}
	else if (targets.size() > 1)
	{
		// find left-most position
		int count = 0;
		int x = 0;

		KShapeIterator it = targets.begin();
		while (it != targets.end())
		{
      KShapeBase* c = *it++;
			if (c != NULL)
			{
				c->GetNormalRectangle(rect);
				if (count == 0)
					x = rect.left;
				else if (rect.left < x)
					x = rect.left;
				++count;
			}
		}

		// move items
		it = targets.begin();
		while (it != targets.end())
		{
      KShapeBase* c = *it++;
			if (c != NULL)
			{
				c->GetNormalRectangle(rect);
				POINT p = { x - rect.left, 0 };
				c->Move(p);
			}
		}
		return count;
	}
	else
	{
		return 0;
	}
}

/**
* @brief 지정한 개체를 가운데 정렬한다.
* @param targets : 대상 개체들의 objectID 리스트.
* @details A. 대상 개체가 하나이면 스프레드의 중앙으로 X 좌표를 맞춘다.
* @n B. 대상 개체가 하나 이상이면,
* @n      - 가장 왼쪽, 가장 오른쪽인 좌표를 찾고 이의 가운데 점을 구한다.
* @n      - 대상 개체들의 중심을 이 가운데 좌표로 이동한다.
*/
int KShapeEditor::AlignCenter(KShapeVector& targets)
{
  RECT rect;

	if (targets.size() == 1)
	{
		KShapeBase* c = targets.at(0);
		if (c != NULL)
		{
			c->GetNormalRectangle(rect);
			POINT p = { (Width - (rect.right - rect.left)) / 2 - rect.left, 0 };
			c->Move(p);
		}
		return 1;
	}
	else if (targets.size() > 1)
	{
		// find left-most right-mode position
		int count = 0;
		int xs = 0;
		int xe = 0;

		KShapeIterator it = targets.begin();
		while (it != targets.end())
		{
      KShapeBase* c = *it++;
			if (c != NULL)
			{
				c->GetNormalRectangle(rect);
				if (count == 0)
				{
					xs = rect.left;
					xe = rect.right;
				}
				else
				{
					if (xs > rect.left)
						xs = rect.left;
					if (xe < rect.right)
						xe = rect.right;
				}
        count++;
      }
		}

		// move items center to (xs + xe) / 2
		int cx = (xs + xe) / 2;
		it = targets.begin();
		while (it != targets.end())
		{
      KShapeBase* c = *it++;
			if (c != NULL)
			{
        c->GetNormalRectangle(rect);
        POINT p = { (cx - (rect.right - rect.left) / 2) - rect.left, 0 };
				c->Move(p);
			}
		}
#if 0
		foreach(int id in targets)
		{
			Graphic c = GetObject(id);
			if (c != null)
			{
				if (count == 0)
				{
					xs = c.X;
					xe = c.Right();
				}
				else
				{
					if (xs > c.X)
						xs = c.X;
					if (xe < c.Right())
						xe = c.Right();
				}
				count++;
			}
		}

		// move items center to (xs + xe) / 2
		double cx = (xs + xe) / 2;
		foreach(int id in targets)
		{
			Graphic c = GetObject(id);
			if (c != null)
			{
				c.X = cx - c.Width / 2;
				c.ClearPathGeometry();
			}
		}
#endif
		return count;
	}
	else
	{
		return 0;
	}
}

/**
* @brief 지정한 개체를 오른쪽 정렬한다.
* @param targets : 대상 개체들의 objectID 리스트.
* @details A. 대상 개체가 하나이면 스프레드의 오른쪽 테두리에 붙도록 X 좌표를 맞춘다.
* @n B. 대상 개체가 하나 이상이면,
* @n      - 가장 오른쪽인 좌표를 찾고,
* @n      - 대상 개체들의 오른쪽을 이 좌표로 이동한다.
*/
int KShapeEditor::AlignRight(KShapeVector& targets)
{
  RECT rect;
  if (targets.size() == 1)
	{
		KShapeBase* c = targets.at(0);
		if (c != NULL)
		{
      c->GetNormalRectangle(rect);
      POINT p = { (Width - (rect.right - rect.left)) - rect.left, 0 };
			c->Move(p);
		}
		return 1;
	}
	else if (targets.size() > 1)
	{
		// find right-mode position
		int count = 0;
		int xe = 0;

		KShapeIterator it = targets.begin();
		while (it != targets.end())
		{
      KShapeBase* c = *it++;
			if (c != NULL)
			{
        c->GetNormalRectangle(rect);
        if (count == 0)
				{
					xe = rect.right;
				}
				else
				{
					if (xe < rect.right)
						xe = rect.right;
				}
        count++;
      }
		}
		// move items center to (xs + xe) / 2
		it = targets.begin();
		while (it != targets.end())
		{
      KShapeBase* c = *it++;
			if (c != NULL)
			{
        c->GetNormalRectangle(rect);
        POINT p = { (xe - rect.right), 0 };
				c->Move(p);
			}
		}
    return count;
  }
  return 0;
#if 0
	if (targets.Count() == 1)
	{
		Graphic c = GetObject(targets[0]);
		if (c != null)
		{
			c.X = (SpreadWidth - c.Width);
		}
		return 1;
	}
	else if (targets.Count() > 1)
	{
		// find right-most position
		int count = 0;
		double x = 0;

		foreach(int id in targets)
		{
			Graphic c = GetObject(id);
			if (c != null)
			{
				if (count == 0)
					x = c.Right();
				else if (x < c.Right())
					x = c.Right();
				count++;
			}
		}

		// move items
		foreach(int id in targets)
		{
			Graphic c = GetObject(id);
			if (c != null)
			{
				c.X = x - c.Width;
				c.ClearPathGeometry();
			}
		}
		return count;
	}
	else
	{
		return 0;
	}
#endif
}

/**
* @brief 지정한 개체를 위쪽 정렬한다.
* @param targets : 대상 개체들의 objectID 리스트.
* @details A. 대상 개체가 하나이면 스프레드 위쪽 테두리에 붙도록 Y좌표를 맞춘다.
* @n B. 대상 개체가 하나 이상이면,
* @n      - 가장 위쪽인 좌표를 찾고
* @n      - 대상 개체들을 가장 위쪽 좌표로 이동한다.
*/
int KShapeEditor::AlignTop(KShapeVector& targets)
{
  RECT rect;
  if (targets.size() == 1)
	{
		KShapeBase* c = targets.at(0);
		if (c != NULL)
		{
      c->GetNormalRectangle(rect);
      POINT p = { 0, (Height - (rect.bottom - rect.top)) - rect.top };
			c->Move(p);
		}
		return 1;
	}
	else if (targets.size() > 1)
	{
		// find top-most position
		int count = 0;
		int y = 0;

		KShapeIterator it = targets.begin();
		while (it != targets.end())
		{
      KShapeBase* c = *it++;
			if (c != NULL)
			{
        c->GetNormalRectangle(rect);
        int c_top = rect.top;
				if ((count == 0) || (c_top < y))
					y = c_top;
				count++;
			}
		}

		// move items
		it = targets.begin();
		while (it != targets.end())
		{
      KShapeBase* c = *it++;
			if (c != NULL)
			{
        c->GetNormalRectangle(rect);
        POINT p = { 0, y - rect.top };
				c->Move(p);
			}
		}
		return count;
	}
	else
	{
		return 0;
	}
}

/**
* @brief 지정한 개체를 수직 가운데 정렬한다.
* @param targets : 대상 개체들의 objectID 리스트.
* @details A. 대상 개체가 하나이면 스프레드의 중앙으로 Y 좌표를 맞춘다.
* @n B. 대상 개체가 하나 이상이면,
*       - 가장 위쪽, 가장 아래인 좌표를 찾고 이의 가운데 점을 구한다.
*       - 대상 개체들의 중심을 이 가운데 좌표로 이동한다.
*/
int KShapeEditor::AlignVCenter(KShapeVector& targets)
{
  RECT rect;
  if (targets.size() == 1)
	{
		KShapeBase* c = targets.at(0);
		if (c != NULL)
		{
      c->GetNormalRectangle(rect);
			POINT p = { 0, (Height - (rect.bottom - rect.top)) / 2 - rect.top };
			c->Move(p);
		}
		return 1;
	}
	else if (targets.size() > 1)
	{
		// find top-most bottom-mode position
		int count = 0;
		int ys = 0;
		int ye = 0;

		KShapeIterator it = targets.begin();
		while (it != targets.end())
		{
      KShapeBase* c = *it++;
			if (c != NULL)
			{
        c->GetNormalRectangle(rect);
        if (count == 0)
				{
					ys = rect.top;
					ye = rect.bottom;
				}
				else
				{
					if (ys > rect.top)
						ys = rect.top;
					if (ye < rect.bottom)
						ye = rect.bottom;
				}
				count++;
			}
		}

		// move items center to (ys + ye) / 2
		int cy = (ys + ye) / 2;
		it = targets.begin();
		while (it != targets.end())
		{
      KShapeBase* c = *it++;
			if (c != NULL)
			{
        c->GetNormalRectangle(rect);
        POINT p = { 0, (cy - (rect.bottom - rect.top) / 2) - rect.top };
				c->Move(p);
			}
		}
		return count;
	}
	else
	{
		return 0;
	}
}

/**
* @brief 지정한 개체를 아래쪽 정렬한다.
* @param targets : 대상 개체들의 objectID 리스트.
* @details A. 대상 개체가 하나이면 스프레드의 아래쪽 테두리에 붙도록 Y 좌표를 맞춘다.
* @n B. 대상 개체가 하나 이상이면,
*       - 가장 아래쪽인 좌표를 찾고,
*       - 대상 개체들의 아래쪽을 이 좌표로 이동한다.
*/
int KShapeEditor::AlignBottom(KShapeVector& targets)
{
  RECT rect;
  if (targets.size() == 1)
	{
		KShapeBase* c = targets.at(0);
		if (c != NULL)
		{
      c->GetNormalRectangle(rect);
      POINT p = { 0, Height - rect.bottom };
			c->Move(p);
		}
		return 1;
	}
	else if (targets.size() > 1)
	{
		// find bottom-most position
		int count = 0;
		int y = 0;

		KShapeIterator it = targets.begin();
		while (it != targets.end())
		{
      KShapeBase* c = *it++;
			if (c != NULL)
			{
        c->GetNormalRectangle(rect);
        if (count == 0)
					y = rect.bottom;
				else if (y < rect.bottom)
					y = rect.bottom;
				count++;
			}
		}

		// move items
		it = targets.begin();
		while (it != targets.end())
		{
      KShapeBase* c = *it++;
			if (c != NULL)
			{
        c->GetNormalRectangle(rect);
        POINT p = { 0, y - rect.bottom };
				c->Move(p);
			}
		}
		return count;
	}
	else
	{
		return 0;
	}
}

int KShapeEditor::ApplyProperty(KShapeVector& targets, int channel, DWORD fill, OutlineStyle& lineStyle, 
  LPCTSTR facename, DWORD fontColor, int fontSize, int attr)
{
  int count = 0;
  KShapeIterator it = targets.begin();

  while (it != targets.end())
  {
    KShapeBase* c = *it++;
    if (c != NULL)
    {
      BOOL changed = FALSE;

      if ((channel & ImageEffectFillStyle) && c->SetFillColor(fill))
        changed = TRUE;
      
      if ((channel & ImageEffectLineStyle) && c->SetOutlineStyle(lineStyle))
        changed = TRUE;

      if (channel & ImageEffectFontStyle)
      {
        if (c->SetTextColor(fontColor))
          changed = TRUE;
        if (c->SetFont(facename, fontSize, attr))
          changed = TRUE;
      }

      if (changed)
        count++;
    }
  }
  return count;
}

int KShapeEditor::DeleteSelection(LPRECT updateRect)
{
  int count = 0;
  KShapeIterator it = mShapeList.begin();
  while (it != mShapeList.end())
  {
    KShapeBase* s = *it;
    if (s->IsVisible() && !s->IsLocked() && s->IsSelected())
    {
      RECT rect;
      s->GetNormalRectangle(rect);
      AddRect(*updateRect, rect);
      count++;

      it = mShapeList.erase(it);
      delete s;
    }
    else
    {
      ++it;
    }
  }
  return count;
}

HANDLE KShapeEditor::GetSelectedClipboardData(BOOL bCut)
{
  HANDLE h = NULL;
#ifdef USE_XML_STORAGE
  CComPtr<IStream> pOutStream;
  CComPtr<IXmlWriter> pWriter;
  HRESULT hr;

  hr = CreateStreamOnHGlobal(NULL, FALSE, &pOutStream);
  if (FAILED(hr))
  {
    TCHAR msg[256] = _T("Error creating MemStream for writing");
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
    return FALSE;
  }
  if (FAILED(hr = CreateXmlWriter(__uuidof(IXmlWriter), (void**)&pWriter, NULL)))
  {
    TCHAR msg[256];
    StringCchPrintf(msg, 256, _T("Error creating xml writer, error is %08.8lx"), hr);
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
    return FALSE;
  }

  if (FAILED(hr = pWriter->SetOutput(pOutStream)))
  {
    TCHAR msg[256];
    StringCchPrintf(msg, 256, _T("Error setting output for writer, error is %08.8lx"), hr);
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
    return FALSE;
  }

  if (FAILED(hr = pWriter->WriteStartDocument(XmlStandalone_Omit)))
  {
    TCHAR msg[256];
    StringCchPrintf(msg, 256, _T("Error, Method: WriteStartDocument, error is %08.8lx"), hr);
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
    return FALSE;
  }

  if (FAILED(hr = pWriter->WriteStartElement(NULL, tagAnnotates, NULL)))
  {
    TCHAR msg[256];
    StringCchPrintf(msg, 256, _T("Error, Method: WriteStartElement, error is %08.8lx"), hr);
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
    return FALSE;
  }

  int index = 0;
  KShapeIterator it = mShapeList.begin();
  while (it != mShapeList.end())
  {
    KShapeBase* s = *it;
    if (s->IsVisible() && !s->IsLocked() && s->IsSelected())
    {
      s->SetChildIndex(index);
      // s->Store(mf);
      s->Store(pWriter);

      if (bCut)
      {
        it = mShapeList.erase(it);
        delete s;
      }
      else
      {
        ++it;
      }
    }
    else
    {
      ++it;
    }
    ++index;
  }

  if (FAILED(hr = pWriter->WriteEndDocument()))
  {
    TCHAR msg[256];
    StringCchPrintf(msg, 256, _T("Error, Method: WriteEndDocument, error is %08.8lx"), hr);
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
    return FALSE;
  }
  if (FAILED(hr = pWriter->Flush()))
  {
    TCHAR msg[256];
    StringCchPrintf(msg, 256, _T("Error, Method: Flush, error is %08.8lx"), hr);
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
    return FALSE;
  }

  if (FAILED(hr = GetHGlobalFromStream(pOutStream, &h)))
  {
    TCHAR msg[256];
    StringCchPrintf(msg, 256, _T("Error, GetHGlobalFromStream, error is %08.8lx"), hr);
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
  }
  else
  {
    void* p = GlobalLock(h);
    SIZE_T size = GlobalSize(h);
    GlobalUnlock(h);
  }

  /*
  ULARGE_INTEGER endPosition;
  LARGE_INTEGER sp = { 0, 0 };
  pOutStream->Seek(sp, STREAM_SEEK_END, &endPosition);
  pOutStream->Seek(sp, STREAM_SEEK_SET, NULL);

  h = GlobalAlloc(0, (UINT)endPosition.LowPart);
  void* p = GlobalLock(h);

  ULONG written = 0;
  pOutStream->Write(p, endPosition.LowPart, &written);
  GlobalUnlock(h);
  */
  /*
  pOutStream->Seek(sp, STREAM_SEEK_END, &endPosition);
  pOutStream->Seek(sp, STREAM_SEEK_SET, NULL);

  h = GlobalAlloc(0, (UINT)endPosition.LowPart);
  void* p = GlobalLock(h);
  CComPtr<IStream> pStream = SHCreateMemStream((const BYTE *)p, (UINT)endPosition.LowPart);

  ULARGE_INTEGER ep;
  pOutStream->CopyTo(pStream, endPosition, NULL, &ep);
  //pStream->Release();
  GlobalUnlock(h);
  */
#else

  KShapeContainer selectedItems;
  KMemoryStream mf;

  GetSelectedItems(selectedItems, bCut);
  selectedItems.StoreToBuffer(mf);

  if (!bCut)
    selectedItems.ClearList();

  h = GlobalAlloc(0, (UINT)mf.Size());
  if (h != NULL)
  {
    void* p = GlobalLock(h);
    if (p != NULL)
    {
      memcpy(p, mf.Data(), mf.Size());
      GlobalUnlock(h);
    }
  }

#endif // USE_XML_STORAGE
  return h;
}

MouseState KShapeEditor::OnLButtonDown(PropertyMode mode, int nFlags, CPOINT& cp, KImageDrawer& drawer, LPRECT ptUpdate)
{
  MouseState s = MouseState::MOUSE_OFF;
  POINT point = { cp.x, cp.y };
	
  if (ptUpdate != NULL)
    GetSelectionBound(*ptUpdate);

  // check if over the object or object handle
	if (mode == PropertyMode::PModeAnnotateSelect)
	{
    // 타겟이 선택되지 않은 상태이고 Shift 누른 상태이면 기존 선택을 해제한다. 
    BOOL freeIfNotSelected = !(nFlags & MK_SHIFT);

    EditHandleType handle = GetSelectedObjectsHandle(point, freeIfNotSelected, drawer, mDragAction);

    if (handle != EditHandleType::HandleNone)
    {
      // 선택된 개체 다시 그리도록 영역 설정.
      if (ptUpdate != NULL)
      {
        RECT rect;
        if (GetSelectionBound(rect))
          AddRect(*ptUpdate, rect);
      }

      // 선택된 개체들을 DragAction에 등록하고 편집 모드로 설정한다.
      KShapeVector list;
      GetSelectedObjects(list);
      mDragAction.AddExtraObjects(list, TRUE);

      if (handle == EditHandleType::MoveObjectHandle)
      {
        return MouseState::MOUSE_DRAW_SELECT_MOVE;
      }
      else if ((handle >= EditHandleType::ResizeLeftTop) && (handle <= EditHandleType::ActionHandleMax))
      {
        return MouseState::MOUSE_DRAW_SELECT_RESIZE;
      }
    }
	}

  if ((mode != PropertyMode::PModeAnnotateSelect) || !(nFlags & MK_SHIFT))
  {
    FreeAllSelection(ptUpdate);
  }

  KShapeBase* createdShape = NULL;

	if (mode == PropertyMode::PModeAnnotateSelect)
	{
		s = MouseState::MOUSE_DRAW_SELECT;
	}
	else if (mode == PropertyMode::PModeAnnotateLine)
	{
		OutlineStyle linestyle = { (COLORREF)0xFF000000, 5, 0 };
		BYTE startArrow = 0;
		BYTE endArrow = 0;
    mDefault.GetLineStyle(linestyle, startArrow, endArrow);

    createdShape = new KShapeLine(point, point, linestyle, startArrow, endArrow);
		s = MouseState::MOUSE_DRAW_LINE;
	}
	else if (mode == PropertyMode::PModeAnnotateFreehand)
	{
    DWORD fillcolor = 0xFFFFFFFF;
    OutlineStyle linestyle = { (COLORREF)0xFF000000, 1, 0 };
    BYTE startArrow = 0;
    BYTE endArrow = 0;
    mDefault.GetLineStyle(linestyle, startArrow, endArrow);

    createdShape = new KShapePolygon(&cp, 1, fillcolor, linestyle, FALSE);
		s = MouseState::MOUSE_DRAW_FREEHAND;
	}
	else if (mode == PropertyMode::PModeAnnotateRectangle)
	{
		RECT rect = { point.x, point.y, point.x, point.y };
		DWORD fillcolor = 0xFFFFFFFF;
		OutlineStyle linestyle = { (COLORREF)0xFF000000, 1, 0 };
    mDefault.GetShapeStyle(fillcolor, linestyle);

    createdShape = new KShapeRectangle(rect, fillcolor, linestyle);
		s = MouseState::MOUSE_DRAW_RECTANGLE;
	}
	else if (mode == PropertyMode::PModeAnnotateEllipse)
	{
		RECT rect = { point.x, point.y, point.x, point.y };
		DWORD fillcolor = 0xFFFFFFFF;
		OutlineStyle linestyle = { (COLORREF)0xFF000000, 1, 0 };
    mDefault.GetShapeStyle(fillcolor, linestyle);

    createdShape = new KShapeEllipse(rect, fillcolor, linestyle);
		s = MouseState::MOUSE_DRAW_ELLIPSE;
	}
  else if (mode == PropertyMode::PModeAnnotatePolygon)
  {
    DWORD fillcolor = 0xFFFFFFFF;
    OutlineStyle linestyle = { (COLORREF)0xFF000000, 1, 0 };
    mDefault.GetShapeStyle(fillcolor, linestyle);

    createdShape = new KShapePolygon(&cp, 1, fillcolor, linestyle, TRUE);
    s = MouseState::MOUSE_DRAW_FREESHAPE;
  }
  else if (mode == PropertyMode::PModeAnnotateTextbox)
	{
		RECT rect = { point.x, point.y, point.x, point.y };
    DWORD textcolor = 0;
    TCHAR facename[LF_FACESIZE];
    int pointSize = 0;
    DWORD attr = 0;

    mDefault.GetTextStyle(textcolor, facename, LF_FACESIZE, pointSize, attr);
    //textcolor = KPropertyCtrlBase::GetDefaultTextColor();
    //KPropertyCtrlBase::GetDefaultFontStyle(facename, pointSize, attr);

    createdShape = new KShapeTextbox(rect, textcolor, facename, pointSize, attr);
		s = MouseState::MOUSE_DRAW_TEXTBOX;
	}
  else if (IsAnnotateShapeMode(mode))
  {
    RECT rect = { point.x, point.y, point.x, point.y };
    DWORD fillcolor = 0xFFFFFFFF;
    OutlineStyle linestyle = { (COLORREF)0xFF000000, 1, 0 };
    mDefault.GetShapeStyle(fillcolor, linestyle);

    ShapeType shapeID;
    int index = (int)mode - (int)PropertyMode::PModeAnnotateShape;
    if (index < 32)
      shapeID = KShapeSmartBaseObject::GetShapeID(index, 0);
    else if ((32 <= index) && (index < 32 + 27))
      shapeID = KShapeSmartBaseObject::GetShapeID(index - 32, 1);
    else if ((59 <= index) && (index < 59 + 28))
      shapeID = KShapeSmartBaseObject::GetShapeID(index - 59, 2);
    else if ((87 <= index) && (index < 87 + 16))
      shapeID = KShapeSmartBaseObject::GetShapeID(index - 87, 3); // flowchart
    else if ((103 <= index) && (index < 103 + 27))
      shapeID = KShapeSmartBaseObject::GetShapeID(index - 103, 4); // callout
    else
      shapeID = ShapeType::Rectangle;

    createdShape = KShapeSmartBaseObject::CreateShapeObject(shapeID,
      rect, fillcolor, linestyle, NULL);
    s = static_cast<MouseState>((int)MouseState::MOUSE_DRAW_SHAPESMART + (int)mode - (int)PropertyMode::PModeAnnotateShape);

    // create all shapes for test
#ifdef _TEST_SHAPES
    for (int s = (int)ShapeType::Rectangle; s <= (int)ShapeType::ButtonHome; s++)
    {
      int dx = (s - (int)ShapeType::Rectangle) % 16;
      int dy = (s - (int)ShapeType::Rectangle) /16;
      RECT rect = { dx*100, dy*100, dx*100+90, dy*100+90 };

      KShapeBase* sp = KShapeSmartBaseObject::CreateShapeObject((ShapeType)s, rect, fillcolor, linestyle, NULL);
      if (sp != NULL)
        AddShape(sp);
    }
#endif
  }

  if (s == MouseState::MOUSE_DRAW_SELECT)
  {
    mDragAction.StartDragAction(MouseDragMode::BlockSelect, EditHandleType::HandleNone, point, NULL, ShapeType::ShapeNone);
  }
  else if (createdShape != NULL)
  {
    mDragAction.StartDragAction(MouseDragMode::CreateNewObject, EditHandleType::HandleNone, point, createdShape,
      createdShape->GetShapeType());
  }

	if ((ptUpdate != NULL) && (createdShape != NULL))
	{
		RECT rect = createdShape->GetBounds(NULL);
		AddRect(*ptUpdate, rect);
	}
	return s;
}

int KShapeEditor::OnMouseMove(MouseState mouseState, int nFlags, POINT& point, LPRECT ptUpdate)
{
  if (mouseState > MouseState::MOUSE_OFF)
  {
    if (mDragAction.OnDragging(point, nFlags, ptUpdate))
      return (R_REDRAW_SHAPE | R_REDRAW_HANDLE);
  }
	return 0;
}

int KShapeEditor::OnMouseMoves(MouseState mouseState, int nFlags, LP_CPOINT points, int count, LPRECT ptUpdate)
{
  if (mouseState > MouseState::MOUSE_OFF)
  {
    if (mDragAction.OnDragging(points, count, nFlags, ptUpdate))
      return (R_REDRAW_SHAPE | R_REDRAW_HANDLE);
  }
  return 0;
}

int KShapeEditor::OnLButtonUp(MouseState mouseState, int nFlags, CPOINT& point, LPRECT ptUpdate)
{
  if (mouseState > MouseState::MOUSE_OFF)
  {
    RECT rect;
    BOOL bUpdated = mDragAction.OnDragging(&point, 1, nFlags, &rect);
    int rtn = 0;

    if (mDragAction.OnDragEnd(ptUpdate))
    {
      RECT resultRect;
      memcpy(&resultRect, ptUpdate, sizeof(RECT));

      if (bUpdated && (ptUpdate != NULL))
        AddRect(*ptUpdate, rect);

      if (mouseState == MouseState::MOUSE_DRAW_SELECT)
      {
        POINT pt = { point.x, point.y };
        if ((resultRect.left == resultRect.right) && (resultRect.top == resultRect.bottom))
          AddSelectionBy(pt, false);
        else
          AddSelectionBy(resultRect, false);

        // selected object area
        if (GetSelectionBound(rect) > 0)
        {
          TCHAR msg[64];
          StringCchPrintf(msg, 64, _T("%d items are selected"), GetSelectedObjectsCount(TRUE));
          StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);

          if (ptUpdate != NULL)
            AddRect(*ptUpdate, rect);
        }

        rtn = (R_REDRAW_SHAPE | R_REDRAW_HANDLE);
      }
      else
      {
        KShapeBase* target = mDragAction.DetachObject();
        if (target != NULL)
        {
          int shape_flag = KSHAPE_VISIBLE;
          if (mGlobalOption & GO_AUTOCHANGESELECT)
            shape_flag |= KSHAPE_SELECTED;
          target->SetShapeFlag(TRUE, shape_flag);

          if (mDragAction.mouseDragMode == MouseDragMode::CreateNewObject)
            AddShape(target);

          rtn = (R_REDRAW_SHAPE | R_REDRAW_HANDLE | R_REDRAW_THUMB | R_ANNOTATE_CHANGED);
          // R_CHANGE_SELECT : shape 그리기가 완성되면 선택툴로 전환한다.
          if (mGlobalOption & GO_AUTOCHANGESELECT)
            rtn |= R_CHANGE_SELECT_TOOL;

          // IsNeedTextboxEditText();
          // 드래그 이동에서는 텍스트 모드로 변화 안한다.
          if ((mDragAction.mouseDragMode == MouseDragMode::DragNone) && (target->GetShapeType() == ShapeType::ShapeTextbox))
            rtn |= R_CHANGE_EDITTEXT_TOOL;
        }
        else
        {
          if (mDragAction.mouseDragMode != MouseDragMode::DragNone)
            rtn = (R_REDRAW_SHAPE | R_REDRAW_HANDLE);
        }
      }
    }

    SetGroupSelectMode(FALSE);
    mDragAction.Clear();
    mouseState = MouseState::MOUSE_OFF;
    return rtn;
  }
	return 0;
}

int KShapeEditor::OnRButtonDown(PropertyMode mode, int nFlags, POINT& point, KImageDrawer& drawer, LPRECT ptUpdate)
{
  int r = 0;
  // 지점에 선택된 개체가 있으면 그대로 리턴한다.
  if (IsOverSelectedObject(point, drawer))
    return 0;

  // SHIFT가 아니면 해제한다.
  if (nFlags & MK_SHIFT)
  {
    FreeAllSelection(ptUpdate);
    r = R_REDRAW_SHAPE | R_REDRAW_HANDLE;
  }

  // 새로 선택한다.
  KShapeBase* s = FindOverObject(point, drawer);
  if (s != NULL)
  {
    s->SetShapeFlag(true, KSHAPE_SELECTED);
    OnSelectionChanged(s);

    RECT rect = s->GetBounds(&drawer);
    AddRect(*ptUpdate, rect);
    r = R_REDRAW_SHAPE | R_REDRAW_HANDLE;
  }
  return r;
}

BOOL KShapeEditor::OnDragCancel(LPRECT pUpdate)
{
  // if (mDragAction.mouseDragMode == MouseDragMode::CreateNewObject)
  return mDragAction.OnDragCancel(pUpdate);
}

void KShapeEditor::Draw(Graphics& g, KImageDrawer& info, int flag)
{
  if (IsGroupSelectMode())
    flag |= DRAW_NO_HANDLE;

	KShapeContainer::Draw(g, info, flag);

  mDragAction.OnRender(g, info);
}

void KShapeEditor::OnSelectionChanged(KShapeBase* shape)
{
  if (shape == NULL)
    shape = GetLastSelectedObject();
  SendEventNotify(SHAPE_EVENT_SELECTED, shape);
  /*
  CMainFrame *pMain = (CMainFrame*)AfxGetMainWnd();
  KPropertyCtrlBase* prop = pMain->GetPropertyCtrl();
  if (prop != NULL)
  {
    if (shape == NULL)
      shape = GetLastSelectedObject();
    prop->UpdateShapeStyle(shape);
  }
  */
}

KShapeBase* KShapeEditor::IsNeedTextboxEditText()
{
  KShapeVector list;
  GetSelectedObjects(list);
  if (list.size() == 1)
  {
    KShapeBase* s = list.at(0);
    if (s->GetShapeType() == ShapeType::ShapeTextbox)
      return s;
  }
  return NULL;
}

int KShapeEditor::MakeCurvePolygon(KShapeVector& objs, RECT& area)
{
  int count = 0;
  KShapeIterator it = objs.begin();
  while (it != objs.end())
  {
    KShapeBase* s = *it++;
    if (s->IsVisible() && (s->GetShapeType() == ShapeType::ShapePolygon))
    {
      if (((KShapePolygon*)s)->MakeCurvePolygon())
      {
        RECT rect = s->GetBounds(NULL);
        if (count == 0)
          memcpy(&area, &rect, sizeof(RECT));
        else
          AddRect(area, rect);
      }
      count++;
    }
  }
  return count;
}

