#include "stdafx.h"
#include "KDragAction.h"

#include "..\Platform\Platform.h"
#include "Shape\KShapePolygon.h"

KDragAction::KDragAction()
{
  mouseDragMode = MouseDragMode::DragNone;
  handleType = EditHandleType::HandleNone;
  targetObject = NULL;
  //extraObjects = null;
  targetIndex = -1;
  originBounds.left = 0;
  originBounds.top = 0;
  originBounds.right = 0;
  originBounds.bottom = 0;
  dragAmount.x = dragAmount.y = 0;
  dragStart.x = dragStart.y = 0;
  dragKeyState = 0;
  newObjectType = ShapeType::ShapeNone;
  snapMode = 0;
  //currentSnapSpread = null;
  mbGroupSelected = FALSE;
  mGroupBounds = RECT{ 0,0,0,0 };
  resizeRatio = POINT{ 0, 0 };
  resultAmount = POINT{ 0, 0 };
}

KDragAction::~KDragAction()
{
}

void KDragAction::StartDragAction(MouseDragMode mode, EditHandleType type, POINT pt, KShapeBase* s, ShapeType shape)
{
  mouseDragMode = mode;
  handleType = type;
  targetObject = s;
  extraObjects.clear();
  targetIndex = -1;

  if (s != NULL)
  {
    s->GetRectangle(originBounds);
  }
  else
  {
    originBounds.left = pt.x;
    originBounds.top = pt.y;
    originBounds.right = pt.x;
    originBounds.bottom = pt.y;
  }
  dragStart = pt;
  dragAmount.x = dragAmount.y = 0;
  resultAmount.x = resultAmount.y = 0;
  dragKeyState = 0;
  newObjectType = shape;
  snapMode = 0;
  //currentSnapSpread = null;

  if ((mouseDragMode == MouseDragMode::ResizeObject) ||
    (mouseDragMode == MouseDragMode::HandleObject) ||
    (mouseDragMode == MouseDragMode::MoveObject))
  {
    if (targetObject != NULL)
    {
      targetObject->SetShapeFlag(FALSE, KSHAPE_VISIBLE);
      targetObject->GetRectangle(originBounds);
    }
  }
}

void KDragAction::StartDragAction(MouseDragMode mode, EditHandleType type, POINT pt, KShapeVector& list, RECT& boundRect)
{
  StartDragAction(mode, type, pt, NULL, ShapeType::ShapeNone);
  AddExtraObjects(list, TRUE);
  originBounds = boundRect;
}

/**
* @brief 드래그 시작 위치를 설정한다. 스냅 활성화된 상태이면 위치를 스냅 좌표로 바꾼다.
* @param pt : 시작 위치.
*/
POINT KDragAction::SetStartPoint(POINT pt)
{
  /*
  if ((mouseDragMode == MouseDragMode.CreateNewObject) &&
    IsSnapMode())
  {
    pt = currentSnapSpread.GetSnappedPoint(pt, snapMode);
  }
  */
  dragStart = pt;
  return pt;
}

/**
* @brief 드래그 원본 크기를 설정한다.
* @param size : 드래그 원본 크기.
*/

void KDragAction::SetStartSize(SIZE& size)
{
  originBounds.right = originBounds.left + size.cx;
  originBounds.bottom = originBounds.top + size.cy;
}

/**
* @brief 스냅 적용을 위한 스프레드와 스냅 모드를 설정한다.
* @param spread : 스냅 적용할 스프레드 개체.
* @param mode : 스냅 모드.
*/
void KDragAction::SetSnapOption(int mode)
{
  //currentSnapSpread = spread;
  snapMode = mode;
}

/**
* @brief 대상 개체를 추가한다.
* @param objects : DragAction의 대상이 되는 개체 목록.
*/
void KDragAction::AddExtraObjects(KShapeVector& objects, BOOL clearPrevious)
{
  ShapeType resizeShapeType = ShapeType::ShapeNone;
  if ((mouseDragMode == MouseDragMode::ResizeObject) && (targetObject != NULL))
    resizeShapeType = targetObject->GetResizeType();

  if (clearPrevious)
    extraObjects.clear();

  KShapeIterator it = objects.begin();
  while (it != objects.end())
  {
    KShapeBase* s = *it++;
    if ((targetObject != s) &&
      // Resize style이 동일한 개체만 추가한다
      ((resizeShapeType == ShapeType::ShapeNone) || (resizeShapeType == s->GetResizeType())))
    {
      extraObjects.push_back(s);
      s->SetShapeFlag(FALSE, KSHAPE_VISIBLE);
    }
  }
}

void KDragAction::Clear()
{
  mouseDragMode = MouseDragMode::DragNone;
  handleType = EditHandleType::HandleNone;
  targetObject = NULL;
  extraObjects.clear();
  targetIndex = -1;
  originBounds.left = 0;
  originBounds.top = 0;
  originBounds.right = 0;
  originBounds.bottom = 0;
  dragAmount.x = dragAmount.y = 0;
  dragStart.x = dragStart.y = 0;
  dragKeyState = 0;
  newObjectType = ShapeType::ShapeNone;
  snapMode = 0;
  mbGroupSelected = FALSE;
}

void KDragAction::SetSelectionBounds(BOOL bGroup, RECT& rect)
{
  mbGroupSelected = bGroup;
  mGroupBounds = rect;
}

BOOL KDragAction::OnDragging(POINT& pt, int keyState, LPRECT ptUpdate)
{
  long dy = 0;
  long dx = 0;

  // 개체 영역을 설정하거나 크기 리사이즈할 때에는 이동하는 점 위치를 스냅 적용한다.
#ifdef _SUPPORT_SNAP_TO_
  if (((mouseDragMode == CreateNewObject) || (mouseDragMode == ResizeObject)) &&
    IsSnapMode())
  {
    pt = currentSnapSpread.GetSnappedPoint(pt, snapMode);
  }
#endif

  BOOL isResizeMode = (mouseDragMode == MouseDragMode::ResizeGroupObject) || (mouseDragMode == MouseDragMode::ResizeObject);

  if (!isResizeMode ||
    ((handleType != EditHandleType::ResizeLeft) && (handleType != EditHandleType::ResizeRight)))
  {
    dy = pt.y - dragStart.y;
  }
  if (!isResizeMode ||
    ((handleType != EditHandleType::ResizeTop) && (handleType != EditHandleType::ResizeBottom)))
  {
    dx = pt.x - dragStart.x;
  }

  RECT rect;
  GetUpdateRect(rect); // current area
  dragKeyState = keyState;

  if ((mouseDragMode == MouseDragMode::CreateNewObject) && (newObjectType == ShapeType::ShapePolygon))
  {
    if (targetObject != NULL)
      ((KShapePolygon*)targetObject)->AddPoints(&pt, 1);
  }

  if ((dragAmount.x != dx) || (dragAmount.y != dy))
  {
    dragAmount.x = dx;
    dragAmount.y = dy;

    if (mouseDragMode == MouseDragMode::ResizeObject)
    {
      if (originBounds.right > originBounds.left)
        resizeRatio.x = dragAmount.x / (originBounds.right - originBounds.left);

      if (originBounds.bottom > originBounds.top)
        resizeRatio.y = dragAmount.x / (originBounds.bottom - originBounds.top);
    }
    /*
    CString str;
    str.Format(_T("(%d,%d)"), dragAmount.x, dragAmount.y);
    StoreLogHistory(_T(__FUNCTION__), str, SYNC_MSG_DEBUG);
    */

    if (ptUpdate != NULL)
    {
      GetUpdateRect(*ptUpdate);

      AddRect(*ptUpdate, rect);
      
      if ((mouseDragMode == MouseDragMode::HandleObject) &&
        (targetObject != NULL))
      {
        targetObject->GetDragBounds(this, 0, rect);
        AddRect(*ptUpdate, rect);
      }
    }

    resultAmount = GetResultAmount(originBounds);
    return TRUE; // it needs to update screen
  }
  return FALSE;
}

BOOL KDragAction::OnDragging(LP_CPOINT points, int count, int keyState, LPRECT ptUpdate)
{
  long dy = 0;
  long dx = 0;

  if ((mouseDragMode == MouseDragMode::CreateNewObject) && (newObjectType == ShapeType::ShapePolygon))
  {
    if (targetObject != NULL)
    {
      RECT rect;
      GetUpdateRect(rect); // current area

      ((KShapePolygon*)targetObject)->AddPoints(points, count);

      if (ptUpdate != NULL)
      {
        GetUpdateRect(*ptUpdate);

        AddRect(*ptUpdate, rect);
      }
      return TRUE;
    }
  }
  else
  {
    POINT pt = { points[count - 1].x, points[count - 1].y };
    return OnDragging(pt, keyState, ptUpdate);
  }

  return FALSE;
}

BOOL KDragAction::OnDragCancel(LPRECT pUpdate)
{
  if (mouseDragMode == MouseDragMode::CreateNewObject)
  {
    if (pUpdate != NULL)
      GetUpdateRect(*pUpdate);

    if (targetObject != NULL)
    {
      if (pUpdate != NULL)
      {
        RECT rect = targetObject->GetBounds(NULL);
        AddRect(*pUpdate, rect);
      }
      delete targetObject;
    }
    targetObject = NULL;
    mouseDragMode = MouseDragMode::DragNone;
    return TRUE;
  }
  else if ((mouseDragMode == MouseDragMode::ResizeObject) ||
    (mouseDragMode == MouseDragMode::ResizeGroupObject) ||
    (mouseDragMode == MouseDragMode::MoveObject) ||
    (mouseDragMode == MouseDragMode::HandleObject))
  {
    if (pUpdate != NULL)
    {
      GetUpdateRect(*pUpdate);

      if (targetObject != NULL)
      {
        RECT rect = targetObject->GetBounds(NULL);
        AddRect(*pUpdate, rect);
        targetObject->SetShapeFlag(TRUE, KSHAPE_VISIBLE);
      }

      if (extraObjects.size() > 0)
      {
        KShapeIterator it = extraObjects.begin();
        while (it != extraObjects.end())
        {
          KShapeBase* s = *it++;
          RECT rect = s->GetBounds(NULL);
          AddRect(*pUpdate, rect);
          s->SetShapeFlag(TRUE, KSHAPE_VISIBLE);
        }
        extraObjects.clear();
      }
    }
    targetObject = NULL;
    mouseDragMode = MouseDragMode::DragNone;
    return TRUE;
  }
  return FALSE;
}

BOOL KDragAction::OnDragEnd(LPRECT pUpdate)
{
  if (mouseDragMode == MouseDragMode::CreateNewObject)
  {
    if (newObjectType == ShapeType::ShapeLine)
    {
      POINT sp = { originBounds.left, originBounds.top };
      POINT ep = { originBounds.right, originBounds.bottom };

      if ((resultAmount.x == 0) && (resultAmount.y == 0))
        resultAmount.x = 2;

      MakeResizedRect(sp, ep, resultAmount);
      targetObject->SetEndPoint(ep, TRUE);

      if (pUpdate != NULL)
        *pUpdate = targetObject->GetBounds(NULL);
    }
    else if (newObjectType == ShapeType::ShapePolygon)
    {
      if (pUpdate != NULL)
      {
        // straighten & make curve
        ((KShapePolygon*)targetObject)->FlattenPolygon();

        *pUpdate = targetObject->GetBounds(NULL);
      }
    }
    else
    {
      RECT rect;
      GetResultRect(&rect, FALSE);

      // check minimum size
      if ((rect.right - rect.left) < 2)
        rect.right = rect.left + 2;
      if ((rect.bottom - rect.top) < 2)
        rect.bottom = rect.top + 2;

      targetObject->SetRectangle(rect);

      if (pUpdate != NULL)
        memcpy(pUpdate, &rect, sizeof(RECT));
    }
    return TRUE;
  }
  else if ((mouseDragMode == MouseDragMode::ResizeObject) ||
    (mouseDragMode == MouseDragMode::ResizeGroupObject) || (mouseDragMode == MouseDragMode::MoveObject))
  {
    GetResultRect(pUpdate, FALSE);

    // 타겟 개체를 기준으로 drag 값을 구한다.
    /*
    RECT originRect;
    targetObject->GetRectangle(originRect);
    resultAmount = GetResultAmount(originRect);
    */

    resultAmount = GetResultAmount(originBounds);

    if (targetObject != NULL)
    {
      targetObject->OnDragEnd(this, resultAmount);
      *pUpdate = targetObject->GetBounds(NULL);

      targetObject->SetShapeFlag(TRUE, KSHAPE_VISIBLE);
    }

    if (extraObjects.size() > 0)
    {
      KShapeIterator it = extraObjects.begin();
      while (it != extraObjects.end())
      {
        KShapeBase* s = *it++;
        s->OnDragEnd(this, resultAmount);
        s->SetShapeFlag(TRUE, KSHAPE_VISIBLE);
      }
    }

    if ((resultAmount.x == 0) && (resultAmount.y == 0))
      mouseDragMode = MouseDragMode::DragNone;
    return TRUE;
  }
  else if (mouseDragMode == MouseDragMode::HandleObject)
  {
    resultAmount = GetResultAmount(originBounds);

    if (targetObject != NULL)
    {
      targetObject->OnDragEnd(this, resultAmount);
      *pUpdate = targetObject->GetBounds(NULL);
      targetObject->SetShapeFlag(TRUE, KSHAPE_VISIBLE);
    }
    return TRUE;
  }

  else if (mouseDragMode == MouseDragMode::BlockSelect)
  {
    GetResultRect(pUpdate, TRUE);
    return TRUE;
  }
  return FALSE;
}

void KDragAction::GetUpdateRect(RECT& rect)
{
  GetResultRect(&rect, FALSE);
  if (targetObject != NULL)
  {
    RECT b = targetObject->GetBounds(rect, NULL);

#ifdef _DEBUG
    TCHAR msg[128];
    StringCchPrintf(msg, 128, _T("GetBounds(%d,%d,%d,%d)\n"), b.left, b.top, b.right - b.left, b.bottom - b.top);
    OutputDebugString(msg);
#endif

    AddRect(rect, b);
    int pen_width = targetObject->GetPenWidth() / 2 + 1;
    AddRect(rect, pen_width, pen_width);
  }

  if ((mouseDragMode == MouseDragMode::ResizeObject) || (mouseDragMode == MouseDragMode::MoveObject))
  {
    if (extraObjects.size() > 0)
    {
      KShapeIterator it = extraObjects.begin();
      while (it != extraObjects.end())
      {
        KShapeBase* s = *it++;

        RECT r;
        s->GetDragBounds(r, this);
        AddRect(rect, r);
      }
    }
  }
  else if (mouseDragMode == MouseDragMode::CreateNewObject)
  {
    if ((newObjectType == ShapeType::ShapePolygon) && (targetObject != NULL))
    {
      RECT r;
      r = targetObject->GetBounds(NULL);
      AddRect(rect, r);
    }
  }
}

void KDragAction::OnRender(Graphics& g, KImageDrawer& info)
{
#if 0 // shape's special handle
  if ((EditHandleType.ObjectHandle1 <= handleType) && (handleType <= EditHandleType.ObjectHandle4))
  {
    targetObject.OnRenderRubber(drawingContext, brush, pen, mouseDragMode, handleType, dragAmount, dragKeyState);
  }
  else if (mouseDragMode != DragNone)
#else
  if (mouseDragMode != MouseDragMode::DragNone)
#endif
  {
    if (targetObject != NULL)
    {
      /*
      if (mouseDragMode == MouseDragMode::HandleObject)
      {
        targetObject->OnDragObjectHandle(handleType, dragAmount, dragKeyState);
      }
      */
      targetObject->OnRenderRubber(g, this, info, 0);
    }
    else
    {
      OnRenderRubberRect(g, originBounds, info);
    }

    int flag = mbGroupSelected ? DRAW_NO_HANDLE : 0;

    if (extraObjects.size() > 0)
    {
      KShapeIterator it = extraObjects.begin();
      while(it != extraObjects.end())
      {
        KShapeBase* s = *it++;
        s->OnRenderRubber(g, this, info, flag);
      }
    }
  }

  if (mbGroupSelected)
  {
    RECT rect = MakeResizedRect(mGroupBounds, resultAmount);

    info.ImageToScreen(rect);
    info.DrawBoundingHandle(g, rect, WITH_GROUP_HANDLE);
  }
}


/**
* @brief 핸들의 이동량을 키보드 제어에 따라 계산한다.
*/
POINT KDragAction::GetResultAmount(RECT& originRect)
{
  POINT amount = dragAmount;

  // snapped amount
#ifdef _SUPPORT_SNAP_TO_
  if ((MouseDragMode.MoveObject == mouseDragMode) && IsSnapMode())
  {
    Rect rect;
    targetObject.GetRectangle(rect);
    rect = MakeResizedRect(rect, mouseDragMode, handleType, amount);

    Point snap_amount = currentSnapSpread.GetSnappedMovement(rect, snapMode);

    string str = string.Format("snap move ({0}, {1})", snap_amount.X, snap_amount.Y);
    Console.WriteLine(str);

    amount.X += snap_amount.X;
    amount.Y += snap_amount.Y;
  }
#endif

#if 0
  if ((EditHandleType.ObjectHandle1 <= handleType) && (handleType <= EditHandleType.ObjectHandle4))
  {
    amount = targetObject.GetResultRubber(handleType, dragAmount);
  }
  else if ((EditHandleType.MoveHorzGuide <= handleType) && (handleType <= EditHandleType.MoveVertGuide))
  {
    ;
  }
  else
#endif
  {
    if ((dragKeyState & WITH_XY_SAME_RATIO) == WITH_XY_SAME_RATIO)
    {
      long ax = abs(amount.x);
      long ay = abs(amount.y);
      if (ax < ay)
        ax = ay;
      amount.x = amount.y = ax;

      if (dragAmount.x < 0)
        amount.x = -amount.x;
      if (dragAmount.y < 0)
        amount.y = -amount.y;
    }

    if ((dragKeyState & AS_INTEGER_RATIO) == AS_INTEGER_RATIO)
    {
      if (mouseDragMode == MouseDragMode::MoveObject) // 이동할 때에는 한쪽 방향으로만 이동
      {
        if (abs(amount.x) >= abs(amount.y))
          amount.y = 0;
        else
          amount.x = 0;
      }
      else if (mouseDragMode == MouseDragMode::ResizeObject)
      {
        if (originRect.right != originRect.left)
        {
          if (amount.x > 0)
          {
            double r = abs(amount.x) / (originRect.right - originRect.left);
            amount.x = (originRect.right - originRect.left) * ((int)(r + 0.5));
          }
          else if (amount.x < 0)
          {
            double r = abs(amount.x) / (originRect.right - originRect.left);
            if (r > 0.25)
              amount.x = -(originRect.right - originRect.left) / 2;
            else
              amount.x = 0;
          }
        }

        if (originRect.bottom != originRect.top)
        {
          if (amount.y > 0)
          {
            double r = abs(amount.y) / (originRect.bottom - originRect.top);
            amount.y = (originRect.bottom - originRect.top) * ((int)(r + 0.5));
          }
          else if (amount.y < 0)
          {
            double r = abs(amount.y) / (originRect.bottom - originRect.top);
            if (r > 0.25)
              amount.y = -(originRect.bottom - originRect.top) / 2;
            else
              amount.y = 0;
          }
        }
      }
    }
  }

  return amount;
}

void KDragAction::GetResultRect(LPRECT r, BOOL bClear)
{
  RECT nr = MakeResizedRect(originBounds, resultAmount);
  memcpy(r, &nr, sizeof(RECT));

  if (bClear)
    Clear();
}


/**
* @brief 원본 사각형 좌표에 mouse-mode, handle-type 이동된 좌표를 적용하여 결과 RECT를 리턴한다.
*/
RECT KDragAction::MakeResizedRect(RECT& originRect)
{
  return MakeResizedRect(originRect, resultAmount);
}

/**
* @brief 원본 사각형 좌표에 mouse-mode, handle-type 이동된 좌표를 적용하여 결과 RECT를 리턴한다.
*/
RECT KDragAction::MakeResizedRect(RECT& originRect, POINT& amount)
{
  POINT s, e;

  s.x = originRect.left;
  e.x = originRect.right;
  s.y = originRect.top;
  e.y = originRect.bottom;

  if (mouseDragMode == MouseDragMode::ResizeObject)
  {
    if ((handleType == EditHandleType::ResizeLeftTop) || (handleType == EditHandleType::ResizeLeft) || (handleType == EditHandleType::ResizeLeftBottom))
    {
      s.x += amount.x;
    }
    else if ((handleType == EditHandleType::ResizeRightTop) || (handleType == EditHandleType::ResizeRight) || (handleType == EditHandleType::ResizeRightBottom))
    {
      e.x += amount.x;
    }
    if ((handleType == EditHandleType::ResizeLeftTop) || (handleType == EditHandleType::ResizeTop) || (handleType == EditHandleType::ResizeRightTop))
    {
      s.y += amount.y;
    }
    else if ((handleType == EditHandleType::ResizeLeftBottom) || (handleType == EditHandleType::ResizeBottom) || (handleType == EditHandleType::ResizeRightBottom))
    {
      e.y += amount.y;
    }
  }
  else if (mouseDragMode == MouseDragMode::ResizeGroupObject)
  {
    if ((handleType == EditHandleType::ResizeRightTop) || (handleType == EditHandleType::ResizeRight) || (handleType == EditHandleType::ResizeRightBottom))
    {
      float r = (float)amount.x / (originBounds.right - originBounds.left);
      s.x = (int)((s.x - originBounds.left) * r + s.x);
      e.x = (int)((e.x - originBounds.left) * r + e.x);
    }
    else if ((handleType == EditHandleType::ResizeLeftTop) || (handleType == EditHandleType::ResizeLeft) || (handleType == EditHandleType::ResizeLeftBottom))
    {
      float r = (float)(originBounds.right - originBounds.left + amount.x) / (originBounds.right - originBounds.left);
      s.x = (int)((s.x - originBounds.left) * r + originBounds.left);
      e.x = (int)((e.x - originBounds.left) * r + originBounds.left);
    }

    if ((handleType == EditHandleType::ResizeLeftBottom) || (handleType == EditHandleType::ResizeBottom) || (handleType == EditHandleType::ResizeRightBottom))
    {
      float r = (float)amount.y / (originBounds.bottom - originBounds.top);
      s.y = (int)((s.y - originBounds.top) * r + s.y);
      e.y = (int)((e.y - originBounds.top) * r + e.y);
    }
    else if ((handleType == EditHandleType::ResizeLeftTop) || (handleType == EditHandleType::ResizeTop) || (handleType == EditHandleType::ResizeRightTop))
    {
      float r = (float)amount.x / (originBounds.right - originBounds.left);
      s.x = (int)((originBounds.right - s.x) * r + s.x);
      e.x = (int)((originBounds.right - e.x) * r + e.x);
    }
  }
  else if (mouseDragMode == MouseDragMode::MoveObject)
  {
    s.x += amount.x;
    s.y += amount.y;
    e.x += amount.x;
    e.y += amount.y;
  }
  else if ((mouseDragMode == MouseDragMode::CreateNewObject) || (mouseDragMode == MouseDragMode::BlockSelect))
  {
    e.x += amount.x;
    e.y += amount.y;
  }
#if 0
  else if (mode == CreateHorzGuideline)
  {
    s.Y += amount.Y;
    e.Y += amount.Y;
  }
  else if (mode == CreateVertGuideline)
  {
    s.X += amount.X;
    e.X += amount.X;
  }
  else if (mode == MoveGuideline)
  {
    if (MoveHorzGuide == type)
    {
      s.Y += amount.Y;
      e.Y += amount.Y;
    }
    else
    {
      s.X += amount.X;
      e.X += amount.X;
    }
  }
#endif
  RECT rect;
  rect.left = min(s.x, e.x);
  rect.top = min(s.y, e.y);
  rect.right = max(s.x, e.x);
  rect.bottom = max(s.y, e.y);
  return rect;
}

void KDragAction::MakeResizedRect(POINT& osp, POINT& oep)
{
  MakeResizedRect(osp, oep, resultAmount);
}

void KDragAction::MakeResizedRect(POINT& osp, POINT& oep, POINT& amount)
{
  if ( ((mouseDragMode == MouseDragMode::ResizeObject) && (handleType == EditHandleType::ResizeLeftTop)) ||
    (mouseDragMode == MouseDragMode::MoveObject))
  {
    osp.x += amount.x;
    osp.y += amount.y;
  }

  if (((mouseDragMode == MouseDragMode::ResizeObject) && (handleType == EditHandleType::ResizeRightBottom)) ||
    (mouseDragMode == MouseDragMode::MoveObject) ||
    (mouseDragMode == MouseDragMode::CreateNewObject))
  {
    oep.x += amount.x;
    oep.y += amount.y;
  }
}

void KDragAction::MakeResizedPoint(POINT& p, RECT& orgBound, RECT& newBound)
{
  p.x -= orgBound.left;
  if ((orgBound.right != orgBound.left) &&
    ((orgBound.right - orgBound.left) != (newBound.right - newBound.left)))
    p.x = p.x * (newBound.right - newBound.left) / (orgBound.right - orgBound.left);
  p.x += newBound.left;

  p.y -= orgBound.top;
  if ((orgBound.bottom != orgBound.top) &&
    ((orgBound.bottom - orgBound.top) != (newBound.bottom - newBound.top)))
    p.y = p.y * (newBound.bottom - newBound.top) / (orgBound.bottom - orgBound.top);
  p.y += newBound.top;
}


/**
* @brief 출력할 Graphics에 변경된 좌표를 계산하여 지정한 brusn, pen으로 출력한다.
* @n 영역을 지정할 때 사용된다.
* @param Graphics : 출력할 target device
* @param brush : 출력에 사용할 brush
* @param pen : 출력에 사용할 pen
* @param originRect : 원본 좌표
*/
void KDragAction::OnRenderRubberRect(Graphics& g, RECT& originRect, KImageDrawer& info)
{
  if ((mouseDragMode == MouseDragMode::CreateNewObject) &&
    ((newObjectType == ShapeType::ShapeLine) ||
    (newObjectType == ShapeType::ShapePolygon)))
  {
    POINT ps = info.ImageToScreen(originRect.left, originRect.top);
    POINT pe = info.ImageToScreen(originRect.left + dragAmount.x, originRect.top + dragAmount.y);

    g.DrawLine(&info.selectingPen, Point(ps.x, ps.y), Point(pe.x, pe.y));
    return;
  }

  RECT r = MakeResizedRect(originRect, resultAmount);

  /*
  CString str;
  str.Format(_T("Adorner rect : %d, %d, %d, %d"), r.left, r.top, r.right, r.bottom);
  StoreLogHistory(_T(__FUNCTION__), str, SYNC_MSG_DEBUG);
  */

  info.ImageToScreen(r);

#ifdef _SUPPORT_GUIDELINE_
  if (CreateHorzGuideline == mouseDragMode)
  {
    Point ps = { MIN_START, rect.top };
    Point pe = { MAX_END, rect.top };
    g.DrawLine(&pen, ps, pe);
  }
  else if (CreateVertGuideline == mouseDragMode)
  {
    Point ps = { rect.left, MIN_START };
    Point pe = { rect.left, MAX_END };
    g.DrawLine(&pen, ps, pe);
  }
  else if (MoveGuideline == mouseDragMode)
  {
    if (MoveHorzGuide == handleType)
    {
      Point ps = new Point(MIN_START, rect.Y);
      Point pe = new Point(MAX_END, rect.Y);
      g.DrawLine(&pen, ps, pe);
    }
    else // if (EditHandleType.MoveVertGuide == handleType))
    {
      Point ps = new Point(rect.X, MIN_START);
      Point pe = new Point(rect.X, MAX_END);
      g.DrawLine(&pen, ps, pe);
    }
  }
  else
#endif
  {
    if (newObjectType == ShapeType::ShapeEllipse)
    {
      g.FillEllipse(&info.selectingBrush, r.left, r.top, r.right - r.left, r.bottom - r.top);
      g.DrawEllipse(&info.selectingPen, r.left, r.top, r.right - r.left, r.bottom - r.top);
    }
    else if ((ShapeType::ShapeTriangle <= newObjectType) && (newObjectType <= ShapeType::ShapeStar))
    {
      Point* p = new Point[10];
      int pnum = KShapeBase::ReadyShapePolygon(newObjectType, p, 10, r);
      g.FillPolygon(&info.selectingBrush, p, pnum);
      g.DrawPolygon(&info.selectingPen, p, pnum);
      delete[] p;
    }
    else
    {
      g.FillRectangle(&info.selectingBrush, r.left, r.top, r.right - r.left, r.bottom - r.top);
      g.DrawRectangle(&info.selectingPen, r.left, r.top, r.right - r.left, r.bottom - r.top);
    }
  }
}
