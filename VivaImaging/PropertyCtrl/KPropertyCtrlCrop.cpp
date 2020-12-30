// KPropertyCtrlBase.cpp: 구현 파일
//

#include "stdafx.h"
#include "..\VivaImaging.h"
#include "..\Core\KConstant.h"
#include "..\Platform\Graphics.h"

#include "KPropertyCtrlCrop.h"
// KPropertyCtrlCrop

IMPLEMENT_DYNAMIC(KPropertyCtrlCrop, CWnd)

KPropertyCtrlCrop::KPropertyCtrlCrop()
  :KPropertyCtrlBase()
{
  mByRatio = 0;
  mLeft = 0;
  mTop = 0;
  mRight = 0;
  mBottom = 0;
  mSourceWidth = 0;
  mSourceHeight = 0;
  mApplyAll = 0;
}

KPropertyCtrlCrop::KPropertyCtrlCrop(CWnd* parent, RECT& rect)
: KPropertyCtrlBase()
{
  Create(WS_CHILD|WS_VISIBLE, rect, parent, 101);
}

KPropertyCtrlCrop::~KPropertyCtrlCrop()
{
}


BEGIN_MESSAGE_MAP(KPropertyCtrlCrop, CWnd)
  ON_WM_CREATE()
  ON_WM_SIZE()
  ON_WM_VSCROLL()
  ON_WM_CTLCOLOR()
END_MESSAGE_MAP()



// KPropertyCtrlCrop 메시지 처리기

int KPropertyCtrlCrop::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
  HWND hWnd = GetSafeHwnd();
  int xm = DialogX2Pixel(PC_X_MARGIN);
  int ym = DialogY2Pixel(PC_Y_MARGIN);
  int height = DialogY2Pixel(PC_ITEM_HEIGHT);
  int right = DialogX2Pixel(PC_X_MARGIN + PC_ITEM_WIDTH);
  int width1 = DialogX2Pixel(50);
  int width2 = DialogX2Pixel(50);
  int dy = ym;
  int x2 = xm + width1 + DialogX2Pixel(3);
  int x3 = x2 + width2 + DialogX2Pixel(3);
  int width3 = DialogX2Pixel(25);
  int yo = DialogY2Pixel(2);
  CString str;

  str.LoadString(AfxGetInstanceHandle(), IDS_BY_RATIO, mResourceLanguage);
  mByRatioCheck.Create(str, WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
    CRect(xm, dy, right, dy + height), this, 301);
  dy += height + ym;

  str.LoadString(AfxGetInstanceHandle(), IDS_LEFT, mResourceLanguage);
  str += _T(" :");
  mLabel1.Create(str, WS_CHILD | WS_VISIBLE | SS_RIGHT,
    CRect(xm, dy + yo, xm + width1, dy + height), this, 302);
  mEditLeft.Create(WS_CHILD | WS_VISIBLE | WS_BORDER,
    CRect(x2, dy, x2 + width2, dy + height), this, 303);
  mUnit1.Create(_T(""), WS_CHILD | WS_VISIBLE,
    CRect(x3, dy + yo, x3 + width3, dy + height), this, 304);
  dy += height + ym;

  str.LoadString(AfxGetInstanceHandle(), IDS_TOP, mResourceLanguage);
  str += _T(" :");
  mLabel2.Create(str, WS_CHILD | WS_VISIBLE | SS_RIGHT,
    CRect(xm, dy + yo, xm + width1, dy + height), this, 305);
  mEditTop.Create(WS_CHILD | WS_VISIBLE | WS_BORDER,
    CRect(x2, dy, x2 + width2, dy + height), this, 306);
  mUnit2.Create(_T(""), WS_CHILD | WS_VISIBLE,
    CRect(x3, dy + yo, x3 + width3, dy + height), this, 307);
  dy += height + ym;

  str.LoadString(AfxGetInstanceHandle(), IDS_RIGHT, mResourceLanguage);
  str += _T(" :");
  mLabel3.Create(str, WS_CHILD | WS_VISIBLE | SS_RIGHT,
    CRect(xm, dy + yo, xm + width1, dy + height), this, 308);
  mEditRight.Create(WS_CHILD | WS_VISIBLE | WS_BORDER,
    CRect(x2, dy, x2 + width2, dy + height), this, 309);
  mUnit3.Create(_T(""), WS_CHILD | WS_VISIBLE,
    CRect(x3, dy + yo, x3 + width3, dy + height), this, 310);
  dy += height + ym;

  str.LoadString(AfxGetInstanceHandle(), IDS_BOTTOM, mResourceLanguage);
  str += _T(" :");
  mLabel4.Create(str, WS_CHILD | WS_VISIBLE | SS_RIGHT,
    CRect(xm, dy + yo, xm + width1, dy + height), this, 311);
  mEditBottom.Create(WS_CHILD | WS_VISIBLE | WS_BORDER,
    CRect(x2, dy, x2 + width2, dy + height), this, 312);
  mUnit4.Create(_T(""), WS_CHILD | WS_VISIBLE,
    CRect(x3, dy, x3 + width3, dy + height), this, 313);
  dy += height + ym * 2;

  str.LoadString(AfxGetInstanceHandle(), IDS_APPLY_ALL, mResourceLanguage);
  mAll.Create(str, WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
    CRect(xm, dy, right, dy + height), this, 101);
  dy += height + ym;

  height = DialogY2Pixel(PC_APPLY_HEIGHT);
  right = DialogX2Pixel(PC_X_MARGIN + PC_APPLY_WIDTH);

  str.LoadString(AfxGetInstanceHandle(), IDS_APPLY_NOW, mResourceLanguage);
  mApply.Create(str, WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
    CRect(xm, dy, right, dy + height), this, 102);

  mMinHeight = dy + height + ym;

  SetValueAsRatio(mByRatio);
  UpdateValueToControl(0x1F);

  if (gpUiFont != NULL)
  {
    mByRatioCheck.SetFont(gpUiFont);

    mLabel1.SetFont(gpUiFont);
    mEditLeft.SetFont(gpUiFont);
    mUnit1.SetFont(gpUiFont);
    mLabel2.SetFont(gpUiFont);
    mEditTop.SetFont(gpUiFont);
    mUnit2.SetFont(gpUiFont);
    mLabel3.SetFont(gpUiFont);
    mEditRight.SetFont(gpUiFont);
    mUnit3.SetFont(gpUiFont);
    mLabel4.SetFont(gpUiFont);
    mEditBottom.SetFont(gpUiFont);
    mUnit4.SetFont(gpUiFont);

    mAll.SetFont(gpUiFont);
    mApply.SetFont(gpUiFont);
  }
  return 0;
}

void KPropertyCtrlCrop::OnSize(UINT nType, int cx, int cy)
{
  UpdateScrollInfo();
}

HBRUSH KPropertyCtrlCrop::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
  if (nCtlColor == CTLCOLOR_STATIC)
  {
    HBRUSH br = ::GetSysColorBrush(COLOR_WINDOW);
    pDC->SetBkColor(::GetSysColor(COLOR_WINDOW));
    return br;
  }
  return CWnd::OnCtlColor(pDC, pWnd, nCtlColor);
}

BOOL KPropertyCtrlCrop::OnCommand(WPARAM wParam, LPARAM lParam)
{
  WORD notify_code = HIWORD(wParam);
  WORD ctrl_id = LOWORD(wParam);
  if (notify_code == BN_CLICKED)
  {
    if (ctrl_id == 301)
    {
      mByRatio = mByRatioCheck.GetCheck();
      SetValueAsRatio(mByRatio);
      changeValuesAsRatio(mByRatio);
      UpdatePreview();
    }
    else if (ctrl_id == 101)
    {
      mApplyAll = mAll.GetCheck();
    }
    else if (ctrl_id == 102)
    {
      ApplyEffect();
    }
    return 1;
  }
  else if (notify_code == EN_CHANGE)
  {
    CString str;

    if (ctrl_id == 303)
    {
      mEditLeft.GetWindowText(str);
      int val = 0;
      if ((str.GetLength() > 0) &&
        (swscanf_s(str.GetBuffer(), _T("%d"), &val) > 0))
      {
        mLeft = val;
        UpdatePreview();
      }
    }
    else if (ctrl_id == 306)
    {
      mEditTop.GetWindowText(str);
      int val = 0;
      if ((str.GetLength() > 0) &&
        (swscanf_s(str.GetBuffer(), _T("%d"), &val) > 0))
      {
        mTop = val;
        UpdatePreview();
      }
    }
    else if (ctrl_id == 309)
    {
      mEditRight.GetWindowText(str);
      int val = 0;
      if ((str.GetLength() > 0) &&
        (swscanf_s(str.GetBuffer(), _T("%d"), &val) > 0))
      {
        mRight = val;
        UpdatePreview();
      }
    }
    else if (ctrl_id == 312)
    {
      mEditBottom.GetWindowText(str);
      int val = 0;
      if ((str.GetLength() > 0) &&
        (swscanf_s(str.GetBuffer(), _T("%d"), &val) > 0))
      {
        mBottom = val;
        UpdatePreview();
      }
    }
  }
  return KPropertyCtrlBase::OnCommand(wParam, lParam);
}

void KPropertyCtrlCrop::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
  OnVerticalScroll(nSBCode, nPos);
}

void KPropertyCtrlCrop::InitProperty(CVivaImagingDoc* doc)
{
  KImageBase* p = doc->GetImagePage(EXCLUDE_FOLDED, -1);
  if ((p != NULL) && (p->GetType() == IMAGE_PAGE))
  {
    KBitmap* b = ((KImagePage *)p)->GetImage(doc->GetWorkingPath());
    if (b != NULL)
    {
      mSourceWidth = b->width;
      mSourceHeight = b->height;
      // now reset parameters
      mLeft = 0;
      mTop = 0;
      mRight = 0;
      mBottom = 0;
      mApplyAll = 0;
      UpdateValueToControl(0x1F);
    }
  }
}

void KPropertyCtrlCrop::UpdateValueToControl(int mask)
{
  CString str;

  if (mask & 1)
  {
    str.Format(_T("%d"), mLeft);
    mEditLeft.SetWindowTextW(str);
  }
  if (mask & 2)
  {
    str.Format(_T("%d"), mTop);
    mEditTop.SetWindowTextW(str);
  }
  if (mask & 4)
  {
    str.Format(_T("%d"), mRight);
    mEditRight.SetWindowTextW(str);
  }
  if (mask & 8)
  {
    str.Format(_T("%d"), mBottom);
    mEditBottom.SetWindowTextW(str);
  }
  if (mask & 16)
  {
    mAll.SetCheck(mApplyAll ? BST_CHECKED : BST_UNCHECKED);
  }
}

void KPropertyCtrlCrop::changeValuesAsRatio(BOOL asRatio)
{
  if ((mSourceWidth == 0) || (mSourceHeight == 0))
    return;

  if (asRatio)
  {
    mLeft = (int)((float)mLeft * 100 / mSourceWidth + 0.5);
    mTop = (int)((float)mTop * 100 / mSourceHeight + 0.5);
    mRight = (int)((float)mRight * 100 / mSourceWidth + 0.5);
    mBottom = (int)((float)mBottom * 100 / mSourceHeight + 0.5);
  }
  else
  {
    mLeft = (int)((float)mLeft * mSourceWidth / 100 + 0.5);
    mTop = (int)((float)mTop * mSourceHeight / 100 + 0.5);
    mRight = (int)((float)mRight * mSourceWidth / 100 + 0.5);
    mBottom = (int)((float)mBottom * mSourceHeight / 100 + 0.5);
  }
  UpdateValueToControl(0x0F);
}

void KPropertyCtrlCrop::SetValueAsRatio(BOOL asRatio)
{
  CString str;

  int sid = (asRatio ? IDS_UNIT_PERCENT : IDS_UNIT_PIXEL);
  str.LoadString(AfxGetInstanceHandle(), sid, mResourceLanguage);

  mUnit1.SetWindowText(str);
  mUnit2.SetWindowText(str);
  mUnit3.SetWindowText(str);
  mUnit4.SetWindowText(str);
}

void KPropertyCtrlCrop::readyEffectJob(ImageEffectJob& effect)
{
  memset(&effect, 0, sizeof(ImageEffectJob));
  effect.job = PropertyMode::PModeCrop;
  effect.param[0] = mByRatio;
  effect.param[1] = mLeft;
  effect.param[2] = mTop;
  effect.param[3] = mRight;
  effect.param[4] = mBottom;

  mApplyAll = (mAll.GetCheck() == BST_CHECKED);
  effect.applyAll = mApplyAll;
}

void KPropertyCtrlCrop::ApplyEffect()
{
  ImageEffectJob effect;
  readyEffectJob(effect);
  KPropertyCtrlBase::ApplyProperty(&effect);
}

void KPropertyCtrlCrop::UpdatePreview()
{
  ImageEffectJob effect;
  readyEffectJob(effect);
  KPropertyCtrlBase::UpdatePreview(&effect);
}

void KPropertyCtrlCrop::DrawPropertyCtrlPreview(CDC* pDC, KImageDrawer& info, ImageEffectJob* effect)
{
  RECT rect;

  if (effect->param[0] == 1)
  {
    rect.left = (int)((float)info.src_width * effect->param[1] / 100 + 0.5);
    rect.top = (int)((float)info.src_height * effect->param[2] / 100 + 0.5);
    rect.right = (int)((float)info.src_width - info.src_width * effect->param[3] / 100 + 0.5);
    rect.bottom = (int)((float)info.src_height - info.src_height * effect->param[4] / 100 + 0.5);
  }
  else
  {
    rect.left = (int)effect->param[1];
    rect.top = (int)effect->param[2];
    rect.right = info.src_width - (int)effect->param[3];
    rect.bottom = info.src_height - (int)effect->param[4];
  }

  info.ImageToScreen(rect);

  CPen pen;
  pen.CreatePen(PS_DASH, 1, 0x00FFFFFF);
  CPen* old_pen = (CPen*)pDC->SelectObject(&pen);
  CBrush* old_brush = (CBrush*)pDC->SelectStockObject(NULL_BRUSH);
  int old_rop = pDC->SetROP2(R2_XORPEN);
  pDC->Rectangle(&rect);

  pDC->SetROP2(old_rop);
  pDC->SelectObject(old_brush);
  pDC->SelectObject(old_pen);
}

int KPropertyCtrlCrop::MouseHitTest(POINT& point, KImageDrawer& info, ImageEffectJob* effect)
{
  RECT rect;

  if (effect->param[0] == 1)
  {
    rect.left = (int)((float)info.src_width * effect->param[1] / 100 + 0.5);
    rect.top = (int)((float)info.src_height * effect->param[2] / 100 + 0.5);
    rect.right = info.src_width - (int)((float)info.src_width * effect->param[3] / 100 + 0.5);
    rect.bottom = info.src_height - (int)((float)info.src_height * effect->param[4] / 100 + 0.5);
  }
  else
  {
    rect.left = effect->param[1];
    rect.top = effect->param[2];
    rect.right = info.src_width - effect->param[3];
    rect.bottom = info.src_height - effect->param[4];
  }
  int margin_px = (int)(8 / info.currentZoom); // cursor margin in image coordinate
  if (((rect.left - margin_px) < point.x) && (point.x < (rect.right + margin_px)))
  {
    if (((rect.top - margin_px) < point.y) && (point.y < (rect.top + margin_px)))
      return HT_CROP_TOP;
    if (((rect.bottom - margin_px) < point.y) && (point.y < (rect.bottom + margin_px)))
      return HT_CROP_BOTTOM; //  IDC_SIZENS;
  }
  if (((rect.top - margin_px) < point.y) && (point.y < (rect.bottom + margin_px)))
  {
    if (((rect.left - margin_px) < point.x) && (point.x < (rect.left + margin_px)))
      return HT_CROP_LEFT; // IDC_SIZEWE;
    if (((rect.right - margin_px) < point.x) && (point.x < (rect.right + margin_px)))
      return HT_CROP_RIGHT;
  }
  return 0;
}

int KPropertyCtrlCrop::MouseDragging(MouseState state, POINT& point, KImageDrawer& info, ImageEffectJob* effect)
{
  RECT imgarea = { 0, 0, info.src_width, info.src_height };
  if (state == MouseState::MOUSE_DRAG_CROP_LEFT)
  {
    int v = point.x - imgarea.left;
    if (v < 0)
      v = 0;
    if (v > imgarea.right)
      v = imgarea.right;

    if (effect->param[0] == 1)
      v = (int)((float) v * 100 / info.src_width + 0.5);

    if (v != effect->param[1])
    {
      effect->param[1] = v;
      return 1;
    }
  }
  else if (state == MouseState::MOUSE_DRAG_CROP_RIGHT)
  {
    int v = imgarea.right - point.x;
    if (v < 0)
      v = 0;
    if (v > imgarea.right)
      v = imgarea.right;

    if (effect->param[0] == 1)
      v = (int)((float)v * 100 / info.src_width + 0.5);
    if (v != effect->param[3])
    {
      effect->param[3] = v;
      return 1;
    }
  }
  else if (state == MouseState::MOUSE_DRAG_CROP_TOP)
  {
    int v = point.y - imgarea.top;
    if (v < 0)
      v = 0;
    if (v > imgarea.bottom)
      v = imgarea.bottom;

    if (effect->param[0] == 1)
      v = (int)((float)v * 100 / info.src_height + 0.5);
    if (v != effect->param[2])
    {
      effect->param[2] = v;
      return 1;
    }
  }
  else if (state == MouseState::MOUSE_DRAG_CROP_BOTTOM)
  {
    int v = imgarea.bottom - point.y;
    if (v < 0)
      v = 0;
    if (v > imgarea.bottom)
      v = imgarea.bottom;

    if (effect->param[0] == 1)
      v = (int)((float)v * 100 / info.src_height + 0.5);
    if (v != effect->param[4])
    {
      effect->param[4] = v;
      return 1;
    }
  }
  return 0;
}

BOOL KPropertyCtrlCrop::OnPropertyValueChanged(MouseState state, ImageEffectJob* effect)
{
  CString str;

  if ((state == MouseState::MOUSE_DRAG_CROP_LEFT) && (mLeft != effect->param[1]))
  {
    mLeft = effect->param[1];
    UpdateValueToControl(1);
  }
  else if ((state == MouseState::MOUSE_DRAG_CROP_RIGHT) && (mRight != effect->param[3]))
  {
    mRight = effect->param[3];
    UpdateValueToControl(4);
  }
  else if ((state == MouseState::MOUSE_DRAG_CROP_TOP) && (mTop != effect->param[2]))
  {
    mTop = effect->param[2];
    UpdateValueToControl(2);
  }
  else if ((state == MouseState::MOUSE_DRAG_CROP_BOTTOM) && (mBottom != effect->param[4]))
  {
    mBottom = effect->param[4];
    UpdateValueToControl(8);
  }
  return TRUE;
}
