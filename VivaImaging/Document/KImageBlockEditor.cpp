#include "stdafx.h"
#include "KImageBlockEditor.h"
#include "KShapeEditor.h"

#include "..\Platform\Platform.h"

KImageBlockEditor::KImageBlockEditor()
{
  mMode = PropertyMode::PModeImageView;
	mSelected = false;
  memset(&mRect, 0, sizeof(RECT));
  mpFloatImage = NULL;
}

KImageBlockEditor::~KImageBlockEditor()
{
  if (mpFloatImage != NULL)
  {
    delete mpFloatImage;
    mpFloatImage = NULL;
  }
}

MouseState KImageBlockEditor::OnLButtonDown(PropertyMode mode, int nFlags, CPOINT& cp, KImageDrawer& info, LPRECT ptUpdate)
{
  POINT point = { cp.x, cp.y };
  if (mSelected && // check if over area handle
    (IsSelectShapeMode(mMode) ||// PModeSelectRectangle,,,PModeSelectEllipse,
      (mMode == PropertyMode::PModePasteImage)))
  {
    EditHandleType handle = info.HitRectangleHandle(mRect, point);
    MouseDragMode mode = MouseDragMode::ResizeObject;
    ShapeType shapeType = static_cast<ShapeType>((int)ShapeType::ShapeRectangle + (int)mMode - (int)PropertyMode::PModeSelectRectangle);

    // check inner area of selection
    if (handle == EditHandleType::HandleNone)
    {
      handle = info.HitOver(mRect, shapeType, point);
      if (handle != EditHandleType::HandleNone)
        mode = MouseDragMode::MoveObject;
    }

    if (handle != EditHandleType::HandleNone)
    {
      mDragAction.StartDragAction(mode, handle, point, NULL, shapeType);
      mDragAction.SetOriginBound(mRect);
      return MouseState::MOUSE_SELECT_HANDLE_MOVE;
    }

    // else redraw screen because it will be cleared
    memcpy(ptUpdate, &mRect, sizeof(RECT)); // previous area
  }

	mSelected = false;
	if (IsSelectShapeMode(mode)) // == PModeSelectRectangle)
	{
    ShapeType shapeType = static_cast<ShapeType>((int)ShapeType::ShapeRectangle + (int)mode - (int)PropertyMode::PModeSelectRectangle);
    mDragAction.StartDragAction(MouseDragMode::BlockSelect, EditHandleType::HandleNone, point, NULL, shapeType);
    mMode = mode;
    return (MouseState)((int)MouseState::MOUSE_SELECT_RECTANGLE + (int)mode - (int)PropertyMode::PModeSelectRectangle);
	}

	return MouseState::MOUSE_OFF;
}

int KImageBlockEditor::OnMouseMove(MouseState mouseState, int nFlags, POINT& point, LPRECT ptUpdate)
{
	if ((MouseState::MOUSE_SELECT_RECTANGLE <= mouseState) &&
    (mouseState <= MouseState::MOUSE_SELECT_HANDLE_MOVE))
	{
    if (mDragAction.OnDragging(point, nFlags, ptUpdate))
    {
      return (R_REDRAW_SHAPE | R_REDRAW_HANDLE);
    }
	}

	return 0;
}

int KImageBlockEditor::OnLButtonUp(MouseState mouseState, int nFlags, CPOINT& point, LPRECT ptUpdate)
{
  if ((MouseState::MOUSE_SELECT_RECTANGLE <= mouseState) &&
    (mouseState <= MouseState::MOUSE_SELECT_HANDLE_MOVE))
  {
    POINT pt = { point.x, point.y };
    BOOL changed = mDragAction.OnDragging(pt, nFlags, ptUpdate);
    mDragAction.GetResultRect(&mRect, TRUE);
    if (!changed && (ptUpdate != NULL))
      memcpy(ptUpdate, &mRect, sizeof(RECT));

    // only if valid area selected
    mSelected = ((mRect.left != mRect.right) && (mRect.top != mRect.bottom));
		return (R_REDRAW_SHAPE | R_REDRAW_HANDLE);
	}

	return 0;
}

int KImageBlockEditor::MoveObjects(POINT& p, LPRECT ptUpdate)
{
  if (mSelected)
  {
    if (ptUpdate != NULL)
      memcpy(ptUpdate, &mRect, sizeof(RECT));

    mRect.left += p.x;
    mRect.right += p.x;
    mRect.top += p.y;
    mRect.bottom += p.y;

    if (ptUpdate != NULL)
      AddRect(*ptUpdate, mRect);
		return (R_REDRAW_SHAPE | R_REDRAW_HANDLE);
  }
  return 0;
}

int KImageBlockEditor::Select(RECT& rect)
{
  memcpy(&mRect, &rect, sizeof(RECT));
  mSelected = TRUE;
  mMode = PropertyMode::PModeSelectRectangle;
  return 1;
}


void KImageBlockEditor::DrawPreview(Graphics& g, KImageDrawer& info)
{
  if (mMode == PropertyMode::PModeImageView)
    return;
  if (mDragAction.IsActive())
  {
    mDragAction.OnRender(g, info);
  }
	else if (mSelected)
	{
    RECT r = mRect;
    info.ImageToScreen(r);
    NormalizeRect(r);

    if (mMode == PropertyMode::PModeSelectRectangle)
    {
      g.DrawRectangle(&info.selectingPen, r.left, r.top, r.right - r.left, r.bottom - r.top);
    }
    else if (mMode == PropertyMode::PModeSelectTriangle) {
      Point* p = new Point[3];
      KShapeBase::ReadyShapePolygon(ShapeType::ShapeTriangle, p, 3, r);
      g.DrawPolygon(&info.selectingPen, p, 3);
      delete[] p;
    }
    else if (mMode == PropertyMode::PModeSelectRhombus) {
      Point* p = new Point[4];
      KShapeBase::ReadyShapePolygon(ShapeType::ShapeRhombus, p, 4, r);
      g.DrawPolygon(&info.selectingPen, p, 4);
      delete[] p;
    }
    else if (mMode == PropertyMode::PModeSelectPentagon) {
      Point* p = new Point[5];
      KShapeBase::ReadyShapePolygon(ShapeType::ShapePentagon, p, 5, r);
      g.DrawPolygon(&info.selectingPen, p, 5);
      delete[] p;
    }
    else if (mMode == PropertyMode::PModeSelectStar) {
      Point* p = new Point[10];
      KShapeBase::ReadyShapePolygon(ShapeType::ShapeStar, p, 10, r);
      g.DrawPolygon(&info.selectingPen, p, 10);
      delete[] p;
    }
    else if (mMode == PropertyMode::PModeSelectEllipse)
    {
      g.DrawEllipse(&info.selectingPen, r.left, r.top, r.right - r.left, r.bottom - r.top);
    }
    else if (mMode == PropertyMode::PModePasteImage)
    {
      HDC hdc = g.GetHDC();
      CDC* pdc = CDC::FromHandle(hdc);
      if (mpFloatImage->bitCount == 32)
      {
        RECT srect = { 0, 0, mpFloatImage->width, mpFloatImage->height };
        mpFloatImage->AlphaBlend(*pdc, r, srect, 255);
      }
      else
      {
        mpFloatImage->Blit(*pdc, r, BLIT_STRETCH);
      }
      g.ReleaseHDC(hdc);

      /*
      Rect gRect(r.left, r.top, r.right - r.left, r.bottom - r.top);
      Bitmap* b = Bitmap::FromHBITMAP((HBITMAP)mpFloatImage->handle(), NULL);
      g.DrawImage(b, gRect);
      delete b;
      */
      g.DrawRectangle(&info.selectingPen, r.left, r.top, r.right - r.left, r.bottom - r.top);
    }

    info.DrawBoundingHandle(g, r, 0);
	}
}

BOOL KImageBlockEditor::SetEndPoint(POINT& point, BOOL bFinish)
{
	if ((mRect.right != point.x) || (mRect.bottom != point.y) || bFinish)
	{
		mRect.right = point.x;
		mRect.bottom = point.y;
		if (bFinish)
			NormalizeRect(mRect);
		return TRUE;
	}
	return FALSE;
}

BOOL KImageBlockEditor::GetImageSelection(int* param)
{
  if (mMode > PropertyMode::PModeImageView)
  {
    param[0] = mRect.left;
    param[1] = mRect.top;
    param[2] = mRect.right;
    param[3] = mRect.bottom;
    return TRUE;
  }
  return FALSE;
}

void KImageBlockEditor::GetUpdateRect(RECT& rect, int *param)
{
  rect.left = param[0];
  rect.top = param[1];
  rect.right = param[2];
  rect.bottom = param[3];
}

BOOL KImageBlockEditor::GetUpdateRect(RECT& rect, int *param, BOOL bClose)
{
  if (mMode > PropertyMode::PModeImageView)
  {
    rect.left = param[0];
    rect.top = param[1];
    rect.right = param[2];
    rect.bottom = param[3];
    if (bClose)
      mMode = PropertyMode::PModeImageView;
    return TRUE;
  }
  return FALSE;
}

BOOL KImageBlockEditor::StartFloatingImagePaste(int bodyWidth, int bodyHeight, PBITMAPINFOHEADER bi, UINT buffSize, LPRECT updateRect)
{
  if (mpFloatImage != NULL)
  {
    delete mpFloatImage;
    mpFloatImage = NULL;
  }
  mpFloatImage = new KBitmap(bi, buffSize);
  mRect.left = (bodyWidth - bi->biWidth) / 2;
  mRect.right = mRect.left + bi->biWidth;
  int height = abs(bi->biHeight);
  mRect.top = (bodyHeight - height) / 2;
  mRect.bottom = mRect.top + height;
  mMode = PropertyMode::PModePasteImage;
  mSelected = TRUE;
  if (updateRect != NULL)
    *updateRect = mRect;
  return TRUE;
}

BOOL KImageBlockEditor::CloseBlockEditor()
{
  if (mpFloatImage != NULL)
  {
    delete mpFloatImage;
    mpFloatImage = NULL;
  }
  mMode = PropertyMode::PModeImageView;
  mSelected = false;
  memset(&mRect, 0, sizeof(RECT));
  return TRUE;
}