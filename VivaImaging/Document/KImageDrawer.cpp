#include "stdafx.h"
#include "KImageDrawer.h"

#include "Shape\mathmatics.h"
#include "..\Platform\Platform.h"
#include "..\Platform\Graphics.h"

int KImageDrawer::handleSize = 0;
float KImageDrawer::penWidth = 0.0;

#define HANDLE_OUTER_PEN_COLOR 0xFF303030
#define HANDLE_INNER_COLOR 0xFFF0F0F0
#define HANDLE_GROUP_COLOR 0xFFF0A0A0
#define HANDLE_ADJUST_COLOR 0xFFF0A020

#define SELECT_BORDER_PEN_COLOR 0xFF6eB2E9
#define SELECT_INNER_FILL_COLOR 0x406eB2E9

KImageDrawer::KImageDrawer()
  :selectingPen(SELECT_BORDER_PEN_COLOR, 2), selectingBrush(SELECT_INNER_FILL_COLOR)
{
  src_width = 0;
  src_height = 0;
  client_width = 0;
  client_height = 0;
  memset(&drawRect, 0, sizeof(RECT));
  memset(&PaintRect, 0, sizeof(RECT));
  memset(&drawOpt, 0, sizeof(IMAGE_DRAW_OPT));
  drawOpt.zoom = 1.0;

  handleSize = DialogX2Pixel(7);
  penWidth = (float)DialogX2Pixel(7) / 10;

  selectingPen.SetDashStyle(DashStyleDash);
}

KImageDrawer::KImageDrawer(int b_width, int b_height, float z, IMAGE_DRAW_OPT& opt, int cw, int ch)
  :selectingPen(SELECT_BORDER_PEN_COLOR, 2), selectingBrush(SELECT_INNER_FILL_COLOR)
{
  src_width = b_width;
  src_height = b_height;
  client_height = ch;
  client_width = cw;
  drawOpt.zoom = z;
  memset(&drawRect, 0, sizeof(RECT));
  memset(&PaintRect, 0, sizeof(RECT));
  memcpy(&drawOpt, &opt, sizeof(IMAGE_DRAW_OPT));
  handleSize = DialogX2Pixel(7);
  penWidth = (float)DialogX2Pixel(7) / 10;

  selectingPen.SetDashStyle(DashStyleDash);

  if (drawOpt.zoom > 0.0)
  {
    currentZoom = drawOpt.zoom;
  }
  else
  {
    float rx = (float)client_width / b_width;
    float ry = (float)client_height / b_height;

    if (rx < ry)
      ry = rx;
    currentZoom = ry;
  }
  onZoomChanged();
  updateDrawRect();
}

KImageDrawer::~KImageDrawer()
{
}

KImageDrawer& KImageDrawer::operator=(const KImageDrawer& src)
{
  src_width = src.src_width;
  src_height = src.src_height;
  client_height = src.client_height;
  client_width = src.client_width;
  memcpy(&drawOpt, &src.drawOpt, sizeof(IMAGE_DRAW_OPT));
  handleSize = src.handleSize;
  penWidth = src.penWidth;
  currentZoom = src.currentZoom;
  return *this;
}

void KImageDrawer::Set(int b_width, int b_height, float z, int cw, int ch)
{
  src_width = b_width;
  src_height = b_height;
  client_width = cw;
  client_height = ch;
  drawOpt.zoom = z;

  if (drawOpt.zoom > 0.0)
  {
    currentZoom = z;
  }
  else
  {
    if ((client_width > 0) && (b_width > 0) &&
      (client_height > 0) && (b_height > 0))
    {
      float rx = (float)client_width / b_width;
      float ry = (float)client_height / b_height;

      if (rx < ry)
        ry = rx;
      currentZoom = ry;
    }
    else
    {
      currentZoom = 1.0;
    }
  }
  onZoomChanged();
  updateDrawRect();
}

void KImageDrawer::SetZoom(float z)
{
  drawOpt.zoom = z;

  onZoomChanged();
  // check scroll range
  drawOpt.displaySize.cx = (int)(currentZoom * src_width);
  drawOpt.displaySize.cy = (int)(currentZoom * src_height);
  CheckScrollOffset();
  updateDrawRect();
}

BOOL KImageDrawer::Zoom(int dir, LPPOINT zOrg)
{
  POINT baseImagePos = { 0, 0 };
  if (zOrg != NULL)
  {
    baseImagePos = *zOrg;
    ScreenToImage(baseImagePos);
  }

  if (dir != 0)
  {
    float diff = (currentZoom / 4);
    if (dir < 0)
      diff = (-1) * diff;
    //drawOpt.zoom += diff;
    float newzoom = currentZoom + diff;

    if (newzoom < 0.1)
      newzoom = (float)0.1;
    else if ((0.95 < newzoom) && (newzoom < 1.1))
      newzoom = (float)1.0;
    else if (newzoom > 10)
      newzoom = (float)10;

    if (newzoom != drawOpt.zoom)
    {
      drawOpt.zoom = newzoom;

      onZoomChanged();

      // check scroll range
      drawOpt.displaySize.cx = (int)(drawOpt.zoom * src_width);
      drawOpt.displaySize.cy = (int)(drawOpt.zoom * src_height);

      if (zOrg != NULL)
      {
        POINT curr = baseImagePos;
        ImageToScreen(curr);

        // 차이만큼 화면 스크롤 한다.
        drawOpt.scrollOffset.x += (curr.x - zOrg->x);
        if (drawOpt.scrollOffset.x < 0)
          drawOpt.scrollOffset.x = 0;
        drawOpt.scrollOffset.y += (curr.y - zOrg->y);
        if (drawOpt.scrollOffset.y < 0)
          drawOpt.scrollOffset.y = 0;
      }
      updateDrawRect();
      return TRUE;
    }
  }
  return FALSE;
}

void KImageDrawer::updateDrawRect()
{
  int dx = (int)(src_width * currentZoom);
  int dy = (int)(src_height * drawOpt.zoom);

  drawRect.left = (dx < client_width) ? (client_width - dx) / 2 : -drawOpt.scrollOffset.x;
  drawRect.top = (dy < client_height) ? (client_height - dy) / 2 : -drawOpt.scrollOffset.y;
  drawRect.right = drawRect.left + dx;
  drawRect.bottom = drawRect.top + dy;
}

void KImageDrawer::onZoomChanged()
{
  if (drawOpt.zoom > 0.0)
  {
    currentZoom = drawOpt.zoom;
  }
  else if ((client_width > 0) && (client_height > 0) && (src_width > 0) && (src_height > 0))
  {

    float xr = (float)client_width / src_width;
    currentZoom = (float)client_height / src_height;
    if (xr < currentZoom)
      currentZoom = xr;
  }
  else
  {
    currentZoom = 1.0;
  }
  handleImageHalf = ScreenSizeToImage(handleSize) / 2;
  pointImageHalf = handleImageHalf / 4;
}

void KImageDrawer::CheckScrollOffset()
{
  drawOpt.scrollMax.x = (client_width < drawOpt.displaySize.cx) ? drawOpt.displaySize.cx - client_width : 0;
  if (drawOpt.scrollOffset.x > drawOpt.scrollMax.x)
    drawOpt.scrollOffset.x = drawOpt.scrollMax.x;
  drawOpt.scrollMax.y = (client_height < drawOpt.displaySize.cy) ? drawOpt.displaySize.cy - client_height : 0;
  if (drawOpt.scrollOffset.y > drawOpt.scrollMax.y)
    drawOpt.scrollOffset.y = drawOpt.scrollMax.y;
}

BOOL KImageDrawer::ScrollPage(int dx, int dy, BOOL bScroll)
{
  int nx = drawOpt.scrollOffset.x + dx;
  int ny = drawOpt.scrollOffset.y + dy;
  if (nx < 0)
    nx = 0;
  if (nx > drawOpt.scrollMax.x)
    nx = drawOpt.scrollMax.x;
  if (ny < 0)
    ny = 0;
  if (ny > drawOpt.scrollMax.y)
    ny = drawOpt.scrollMax.y;

  if ((nx != drawOpt.scrollOffset.x) || (ny != drawOpt.scrollOffset.y))
  {
    if (bScroll)
    {
      drawOpt.scrollOffset.x = nx;
      drawOpt.scrollOffset.y = ny;
    }
    return TRUE;
  }
  return FALSE;
}


int KImageDrawer::GetHandleImageSize()
{
  return ScreenSizeToImage(handleSize + 2);
}

int KImageDrawer::ScreenSizeToImage(int len)
{
  int n = (int)(len / currentZoom);
  return n;
}

POINT KImageDrawer::ScreenToImage(int px, int py)
{
  POINT img = { 0, 0 };

  if (drawOpt.zoom > 0.0)
  {
    int dx = (int)(src_width * drawOpt.zoom);
    int dy = (int)(src_height * drawOpt.zoom);

    int left = (dx < client_width) ? (client_width - dx) / 2 : 0;
    int top = (dy < client_height) ? (client_height - dy) / 2 : 0;

    img.x = (int)((px + drawOpt.scrollOffset.x - left) / drawOpt.zoom);
    img.y = (int)((py + drawOpt.scrollOffset.y - top) / drawOpt.zoom);
  }
  else
  {
    int dw = (int)(currentZoom * src_width);
    int dh = (int)(currentZoom * src_height);
    if (dw != client_width)
    {
      img.x = (client_width - dw) / 2;
    }
    if (dh != client_height)
    {
      img.y = (client_height - dh) / 2;
    }
    img.x = (int)((px - img.x) / currentZoom);
    img.y = (int)((py - img.y) / currentZoom);
  }
  return img;
}

void KImageDrawer::ScreenToImage(POINT& pt)
{
  pt = ScreenToImage(pt.x, pt.y);
}

void KImageDrawer::ScreenToImage(CPOINT& pt)
{
  POINT point = ScreenToImage(pt.x, pt.y);
  pt.x = point.x;
  pt.y = point.y;
}

void KImageDrawer::ScreenToImages(LP_CPOINT pts, int count)
{
  for (int i = 0; i < count; i++)
  {
    POINT pt = ScreenToImage(pts->x, pts->y);
    pts->x = pt.x;
    pts->y = pt.y;
    pts++;
  }
}

void KImageDrawer::ScreenToImage(RECT& sr)
{
  if (drawOpt.zoom > 0.0)
  {
    int dx = (int)(src_width * drawOpt.zoom);
    int dy = (int)(src_height * drawOpt.zoom);

    int left = (dx < client_width) ? (client_width - dx) / 2 : 0;
    int top = (dy < client_height) ? (client_height - dy) / 2 : 0;

    sr.left = (int)((sr.left + drawOpt.scrollOffset.x - left) / drawOpt.zoom);
    sr.top = (int)((sr.top + drawOpt.scrollOffset.y - top) / drawOpt.zoom);
    sr.right = (int)((sr.right + drawOpt.scrollOffset.x - left) / drawOpt.zoom);
    sr.bottom = (int)((sr.bottom + drawOpt.scrollOffset.y - top) / drawOpt.zoom);
  }
  else
  {
    int dw = (int)(currentZoom * src_width);
    int dh = (int)(currentZoom * src_height);
    int ox = 0;
    int oy = 0;

    if (dw != client_width)
      ox = (client_width - dw) / 2;
    if (dh != client_height)
      oy = (client_height - dh) / 2;
    sr.left = (int)((sr.left - ox) / currentZoom);
    sr.top = (int)((sr.top - oy) / currentZoom);
    sr.right = (int)((sr.right - ox) / currentZoom);
    sr.bottom = (int)((sr.bottom - oy) / currentZoom);
  }
}

POINT KImageDrawer::ImageToScreen(int px, int py)
{
  POINT srn = { 0, 0 };
  if (drawOpt.zoom > 0.0)
  {
    int dx = (int)(src_width * drawOpt.zoom);
    int dy = (int)(src_height * drawOpt.zoom);

    int left = (dx < client_width) ? (client_width - dx) / 2 : 0;
    int top = (dy < client_height) ? (client_height - dy) / 2 : 0;

    int ox = (dx > client_width) ? (int)(drawOpt.scrollOffset.x / drawOpt.zoom) : 0;
    int oy = (dy > client_height) ? (int)(drawOpt.scrollOffset.y / drawOpt.zoom) : 0;

    srn.x = (int)((px - ox) * drawOpt.zoom) + left;
    srn.y = (int)((py - oy) * drawOpt.zoom) + top;
  }
  else
  {
    int dw = (int)(currentZoom * src_width);
    int dh = (int)(currentZoom * src_height);
    int ox = 0;
    int oy = 0;
    if (dw != client_width)
      ox = (client_width - dw) / 2;
    if (dh != client_height)
      oy = (client_height - dh) / 2;
    srn.x = ox + (int)(px * currentZoom);
    srn.y = oy + (int)(py * currentZoom);
  }
  return srn;
}

void KImageDrawer::ImageToScreen(POINT& pt)
{
  pt = ImageToScreen(pt.x, pt.y);
}

void KImageDrawer::ImageToScreen(Gdiplus::Point& pt)
{
  if (drawOpt.zoom > 0.0)
  {
    int dx = (int)(src_width * drawOpt.zoom);
    int dy = (int)(src_height * drawOpt.zoom);

    int left = (dx < client_width) ? (client_width - dx) / 2 : 0;
    int top = (dy < client_height) ? (client_height - dy) / 2 : 0;

    int ox = (dx > client_width) ? (int)(drawOpt.scrollOffset.x / drawOpt.zoom) : 0;
    int oy = (dy > client_height) ? (int)(drawOpt.scrollOffset.y / drawOpt.zoom) : 0;

    pt.X = (int)((pt.X - ox) * drawOpt.zoom) + left;
    pt.Y = (int)((pt.Y - oy) * drawOpt.zoom) + top;
  }
  else
  {
    int dw = (int)(currentZoom * src_width);
    int dh = (int)(currentZoom * src_height);
    int ox = 0;
    int oy = 0;
    if (dw != client_width)
      ox = (client_width - dw) / 2;
    if (dh != client_height)
      oy = (client_height - dh) / 2;
    pt.X = ox + (int)(pt.X * currentZoom);
    pt.Y = oy + (int)(pt.Y * currentZoom);
  }
}

void KImageDrawer::ImageToScreen(RECT& r, int flag)
{
  if (drawOpt.zoom > 0.0)
  {
    int dx = (int)(src_width * drawOpt.zoom);
    int dy = (int)(src_height * drawOpt.zoom);

    int left = (dx < client_width) ? (client_width - dx) / 2 : 0;
    int top = (dy < client_height) ? (client_height - dy) / 2 : 0;

    int ox = (dx > client_width) ? (int)(drawOpt.scrollOffset.x / drawOpt.zoom) : 0;
    int oy = (dy > client_height) ? (int)(drawOpt.scrollOffset.y / drawOpt.zoom) : 0;

    r.left = (int)((r.left - ox) * drawOpt.zoom) + left;
    r.top = (int)((r.top - oy) * drawOpt.zoom) + top;
    r.right = (int)((r.right - ox) * drawOpt.zoom) + left;
    r.bottom = (int)((r.bottom - oy) * drawOpt.zoom) + top;
  }
  else
  {
    int dw = (int)(currentZoom * src_width);
    int dh = (int)(currentZoom * src_height);
    int ox = 0;
    int oy = 0;
    if (dw != client_width)
      ox = (client_width - dw) / 2;
    if (dh != client_height)
      oy = (client_height - dh) / 2;
    r.left = ox + (int)(r.left * currentZoom);
    r.top = oy + (int)(r.top * currentZoom);
    r.right = ox + (int)(r.right * currentZoom);
    r.bottom = oy + (int)(r.bottom * currentZoom);
  }

  if (flag & WITH_HANDLE)
  {
    int hw = (int)((handleSize + penWidth) / 2 + 0.5);
    r.left -= hw;
    r.top -= hw;
    r.right += hw + 1;
    r.bottom += hw + 1;
  }
}

void KImageDrawer::DrawBoundingHandle(Graphics& g, POINT& s, POINT& e, int flag)
{
  int hw = handleSize / 2;
  int iw = hw - 1;
  Pen pen(HANDLE_OUTER_PEN_COLOR, penWidth);
  SolidBrush brush(HANDLE_INNER_COLOR);

  g.FillEllipse(&brush, s.x - iw, s.y - iw, handleSize - 2, handleSize - 2);
  g.DrawEllipse(&pen, s.x - hw, s.y - hw, handleSize, handleSize);

  g.FillEllipse(&brush, e.x - iw, e.y - iw, handleSize - 2, handleSize - 2);
  g.DrawEllipse(&pen, e.x - hw, e.y - hw, handleSize, handleSize);
}

void KImageDrawer::DrawBoundingHandle(Graphics& g, RECT& r, int flag)
{
  int hw = handleSize / 2;
  int iw = hw - 1;
  Pen pen(HANDLE_OUTER_PEN_COLOR, penWidth);
  // SolidBrush brush(HANDLE_INNER_COLOR);
  SolidBrush brush((flag & WITH_GROUP_HANDLE) ? HANDLE_GROUP_COLOR : HANDLE_INNER_COLOR);

  g.FillEllipse(&brush, r.left - iw, r.top - iw, handleSize - 2, handleSize - 2);
  g.DrawEllipse(&pen, r.left - hw, r.top - hw, handleSize, handleSize);

  g.FillEllipse(&brush, r.left - iw, r.bottom - iw, handleSize - 2, handleSize - 2);
  g.DrawEllipse(&pen, r.left - hw, r.bottom - hw, handleSize, handleSize);

  g.FillEllipse(&brush, r.right - iw, r.top - iw, handleSize - 2, handleSize - 2);
  g.DrawEllipse(&pen, r.right - hw, r.top - hw, handleSize, handleSize);

  g.FillEllipse(&brush, r.right - iw, r.bottom - iw, handleSize - 2, handleSize - 2);
  g.DrawEllipse(&pen, r.right - hw, r.bottom - hw, handleSize, handleSize);

  int c = (r.top + r.bottom) / 2;
  g.FillEllipse(&brush, r.left - iw, c - iw, handleSize - 2, handleSize - 2);
  g.DrawEllipse(&pen, r.left - hw, c - hw, handleSize, handleSize);

  g.FillEllipse(&brush, r.right - iw, c - iw, handleSize - 2, handleSize - 2);
  g.DrawEllipse(&pen, r.right - hw, c - hw, handleSize, handleSize);

  c = (r.left + r.right) / 2;
  g.FillEllipse(&brush, c - iw, r.top - iw, handleSize - 2, handleSize - 2);
  g.DrawEllipse(&pen, c - hw, r.top - hw, handleSize, handleSize);

  g.FillEllipse(&brush, c - iw, r.bottom - iw, handleSize - 2, handleSize - 2);
  g.DrawEllipse(&pen, c - hw, r.bottom - hw, handleSize, handleSize);

}

void KImageDrawer::DrawAdjustHandle(Graphics& g, POINT& p, int flag)
{
  int hw = handleSize / 2;
  int iw = hw - 1;

  Pen pen(HANDLE_OUTER_PEN_COLOR, penWidth);
  // SolidBrush brush(HANDLE_INNER_COLOR);
  SolidBrush brush(HANDLE_ADJUST_COLOR);

  g.FillEllipse(&brush, p.x  - iw, p.y - iw, handleSize - 2, handleSize - 2);
  g.DrawEllipse(&pen, p.x - hw, p.y - hw, handleSize, handleSize);
}


EditHandleType KImageDrawer::HitOver(RECT& r, ShapeType mode, POINT& pt)
{
  if ((r.left <= pt.x) && (r.right >= pt.x) && (r.top <= pt.y) && (r.bottom >= pt.y))
  {
    if (mode == ShapeType::ShapeEllipse)
    {
      int cx = (r.left + r.right) / 2;
      int cy = (r.top + r.bottom) / 2;
      double a_sq = (r.right - r.left) / 2;
      double b_sq = (r.bottom - r.top) / 2;
      a_sq = a_sq * a_sq;
      b_sq = b_sq * b_sq;

      // check inside of ellipse
      int rx = abs(pt.x - cx);
      int ry = abs(pt.y - cy);

      double cr = sqrt((double)rx * rx / a_sq + (double)ry * ry / b_sq);
      return((cr <= 1.0) ? EditHandleType::MoveObjectHandle : EditHandleType::HandleNone);
    }
    return EditHandleType::MoveObjectHandle;
  }
  return EditHandleType::HandleNone;
}

BOOL KImageDrawer::IsOverPoint(int x, int y, POINT& pt)
{
  return ((x - pointImageHalf <= pt.x) && (x + pointImageHalf >= pt.x) &&
    (y - pointImageHalf <= pt.y) && (y + pointImageHalf >= pt.y));
}

BOOL KImageDrawer::IsOverHandle(int x, int y, POINT& pt)
{
  return ((x - handleImageHalf <= pt.x) && (x + handleImageHalf >= pt.x) &&
    (y - handleImageHalf <= pt.y) && (y + handleImageHalf >= pt.y));
}

EditHandleType KImageDrawer::HitOverLine(POINT& s, POINT& e, int pen_half_width, POINT& pt)
{
  if (PointIsonLine(&s, &e, &pt, handleImageHalf + pen_half_width))
    return EditHandleType::MoveObjectHandle;
  return EditHandleType::HandleNone;
}

EditHandleType KImageDrawer::HitLineHandle(POINT& s, POINT& e, POINT& pt)
{
  if (IsOverHandle(s.x, s.y, pt))
    return EditHandleType::ResizeLeftTop;
  if (IsOverHandle(e.x, e.y, pt))
    return EditHandleType::ResizeRightBottom;

  return EditHandleType::HandleNone;
}

EditHandleType KImageDrawer::HitRectangleHandle(RECT& r, POINT& pt)
{
  if (IsOverHandle(r.left, r.top, pt))
    return EditHandleType::ResizeLeftTop;
  else if (IsOverHandle(r.left, r.bottom, pt))
    return EditHandleType::ResizeLeftBottom;
  else if (IsOverHandle(r.right, r.top, pt))
    return EditHandleType::ResizeRightTop;
  else if (IsOverHandle(r.right, r.bottom, pt))
    return EditHandleType::ResizeRightBottom;

  int c = (r.top + r.bottom) / 2;
  if (IsOverHandle(r.left, c, pt))
    return EditHandleType::ResizeLeft;
  else if (IsOverHandle(r.right, c, pt))
    return EditHandleType::ResizeRight;

  c = (r.left + r.right) / 2;
  if (IsOverHandle(c, r.top, pt))
    return EditHandleType::ResizeTop;
  else if (IsOverHandle(c, r.bottom, pt))
    return EditHandleType::ResizeBottom;

  return EditHandleType::HandleNone;
}

EditHandleType KImageDrawer::HitAdjustHandle(LPPOINT handles, int num, POINT& pt)
{
  for (int i = 0; i < num; i++)
  {
    if (IsOverHandle(handles[i].x, handles[i].y, pt))
      return (EditHandleType)((int)EditHandleType::ObjectHandle1 + i);
  }
  return EditHandleType::HandleNone;
}

BOOL KImageDrawer::IsPaintArea(RECT& area)
{
  if (PaintRect.left < PaintRect.right)
  {
    return ((PaintRect.left <= area.right) && (area.left <= PaintRect.right) &&
      (PaintRect.top <= area.bottom) && (area.top <= PaintRect.bottom));
  }
  return TRUE;
}
