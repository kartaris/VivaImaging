#pragma once
#include "stdafx.h"
#include "Shape/KShapeDef.h"
#include "Shape/CurveTracer.h"

#include <gdiplus.h>
using namespace Gdiplus;

/**
* @enum EditHandleType
* @brief 마우스 드래깅 핸들 종류
*/
typedef enum class tagEditHandleType
{
  HandleNone,
  ResizeLeftTop,
  ResizeTop,
  ResizeRightTop,
  ResizeLeft,
  MoveObjectHandle,
  ResizeRight,
  ResizeLeftBottom,
  ResizeBottom,
  ResizeRightBottom,
  MoveHorzGuide,
  MoveVertGuide,
  ObjectHandle1,
  ObjectHandle2,
  ObjectHandle3,
  ObjectHandle4,
  ObjectHandle5,
  ObjectHandle6,
  ObjectHandle7,
  ObjectHandle8,
  ActionHandleMax
} EditHandleType;

// #include "Shape\KShapeBase.h"

typedef struct tagIMAGE_DRAW_OPT
{
  float zoom;
  SIZE displaySize;
  POINT scrollMax;
  POINT scrollOffset;
} IMAGE_DRAW_OPT;


#define WITH_HANDLE 1
#define WITH_GROUP_HANDLE 2

class KImageDrawer
{
public:
  KImageDrawer();
  KImageDrawer(int b_width, int b_height, float z, IMAGE_DRAW_OPT& opt, int cw, int ch);

  virtual ~KImageDrawer();

  KImageDrawer& operator=(const KImageDrawer& src);

  static BOOL IsObjectHandle(EditHandleType a) { return((EditHandleType::ObjectHandle1 <= a) && (a <= EditHandleType::ObjectHandle8)); }

  void Set(int b_width, int b_height, float z, int cw, int ch);
  void SetZoom(float z);
  inline float GetZoom() { return drawOpt.zoom; }

  BOOL Zoom(int dir, LPPOINT zOrg);

  void updateDrawRect();

  void onZoomChanged();

  void CheckScrollOffset();
  BOOL ScrollPage(int dx, int dy, BOOL bScroll = TRUE);

  static int HandleSize() { return handleSize; }
  static float HandlePenWidth() {return penWidth; }

  int GetHandleImageSize();

  int ScreenSizeToImage(int len);

  POINT ScreenToImage(int px, int py);
  void ScreenToImage(POINT& pt);
  void ScreenToImage(CPOINT& pt);
  void ScreenToImages(LP_CPOINT pts, int count);

  POINT ImageToScreen(int px, int py);
  void ImageToScreen(POINT& pt);
  void ImageToScreen(Gdiplus::Point& pt);

  void ScreenToImage(RECT& rect);
  void ImageToScreen(RECT& pt, int flag = 0);

  void DrawBoundingHandle(Graphics& g, POINT& s, POINT& e, int flag);

  void DrawBoundingHandle(Graphics& g, RECT& r, int flag);

  void DrawAdjustHandle(Graphics& g, POINT& p, int flag);

  EditHandleType HitOver(RECT& r, ShapeType mode, POINT& pt);

  BOOL IsOverPoint(int x, int y, POINT& pt);
  BOOL IsOverHandle(int x, int y, POINT& pt);

  EditHandleType HitOverLine(POINT& s, POINT& e, int pen_half_width, POINT& pt);
  EditHandleType HitLineHandle(POINT& s, POINT& e, POINT& pt);
  EditHandleType HitRectangleHandle(RECT& r, POINT& pt);
  EditHandleType HitAdjustHandle(LPPOINT handles, int num, POINT& pt);

  BOOL IsPaintArea(RECT& area);

  int src_width;
  int src_height;
  int client_width;
  int client_height;

  IMAGE_DRAW_OPT drawOpt;
  RECT drawRect;

  RECT PaintRect;

  // cached variable
  float currentZoom;
  int pointImageHalf;
  int handleImageHalf;

  static int handleSize;
  static float penWidth;

  Pen selectingPen;
  SolidBrush selectingBrush;

};
