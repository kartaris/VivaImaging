#pragma once

// flags for GetNumberOfPages

#include "../Platform/KBitmap.h"
#include "KImageBase.h"
#include "Shape/KShapeBase.h"
#include "KDragAction.h"

class KBitmap;

class KImageBlockEditor
{
public:
  KImageBlockEditor();
  virtual ~KImageBlockEditor();

  BOOL SetEndPoint(POINT& point, BOOL bFinish);

  MouseState OnLButtonDown(PropertyMode mode, int nFlags, CPOINT& point, KImageDrawer& info, LPRECT ptUpdate);
  int OnMouseMove(MouseState mouseState, int nFlags, POINT& point, LPRECT ptUpdate);
  int OnLButtonUp(MouseState mouseState, int nFlags, CPOINT& point, LPRECT ptUpdate);

  int MoveObjects(POINT& p, LPRECT ptUpdate);
  int Select(RECT& rect);

  void DrawPreview(Graphics& g, KImageDrawer& info);

  BOOL GetImageSelection(int* param);
  static void GetUpdateRect(RECT& rect, int *param);
  BOOL GetUpdateRect(RECT& rect, int *param, BOOL bClose);

  BOOL StartFloatingImagePaste(int bodyWidth, int bodyHeight, PBITMAPINFOHEADER bi, UINT buffSize, LPRECT updateRect);
  BOOL CloseBlockEditor();

  PropertyMode mMode;
  BOOL mSelected;
  RECT mRect;
  KBitmap* mpFloatImage;
  KDragAction mDragAction;
};
