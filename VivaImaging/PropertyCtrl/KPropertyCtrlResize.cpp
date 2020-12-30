// KPropertyCtrlBase.cpp: 구현 파일
//

#include "stdafx.h"
#include "..\VivaImaging.h"
#include "..\Core\KConstant.h"
#include "..\Platform\Graphics.h"

#include "KPropertyCtrlResize.h"
// KPropertyCtrlResize

IMPLEMENT_DYNAMIC(KPropertyCtrlResize, CWnd)

KPropertyCtrlResize::KPropertyCtrlResize()
  :KPropertyCtrlBase()
{
  mWithRatio = 0;
  mWidth = 0;
  mHeight = 0;
  mSourceWidth = 0;
  mSourceHeight = 0;
  mEditBy = 0;
  mPreserveRatio = 1;
  mApplyAll = 0;
}

KPropertyCtrlResize::KPropertyCtrlResize(CWnd* parent, RECT& rect)
: KPropertyCtrlBase()
{
  Create(WS_CHILD|WS_VISIBLE, rect, parent, 101);
}

KPropertyCtrlResize::~KPropertyCtrlResize()
{
}


BEGIN_MESSAGE_MAP(KPropertyCtrlResize, CWnd)
  ON_WM_CREATE()
  ON_WM_SIZE()
  ON_WM_VSCROLL()
  ON_WM_CTLCOLOR()
END_MESSAGE_MAP()



// KPropertyCtrlResize 메시지 처리기

int KPropertyCtrlResize::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
  HWND hWnd = GetSafeHwnd();
  int xm = DialogX2Pixel(PC_X_MARGIN);
  int ym = DialogY2Pixel(PC_Y_MARGIN);
  int height = DialogY2Pixel(PC_ITEM_HEIGHT);
  int right = DialogX2Pixel(PC_X_MARGIN + PC_ITEM_WIDTH);
  int width1 = DialogX2Pixel(50);
  int width2 = DialogX2Pixel(50);
  int x2 = xm + width1 + DialogX2Pixel(3);
  int x3 = x2 + width2 + DialogX2Pixel(3);
  int width3 = DialogX2Pixel(25);
  int yo = DialogY2Pixel(2);
  int dy = ym;

  CString str;

  str.LoadString(AfxGetInstanceHandle(), IDS_BY_RATIO, mResourceLanguage);
  mByRatioCheck.Create(str, WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
    CRect(xm, dy, right, dy + height), this, 301);
  dy += height + ym;

  str.LoadString(AfxGetInstanceHandle(), IDS_WIDTH, mResourceLanguage);
  str += _T(" :");
  mLabel1.Create(str, WS_CHILD | WS_VISIBLE | SS_RIGHT,
    CRect(xm, dy + yo, xm+width1, dy + height), this, 302);
  mEditWidth.Create(WS_CHILD | WS_VISIBLE | WS_BORDER,
    CRect(x2, dy, x2 + width2, dy + height), this, 303);
  mUnit1.Create(_T(""), WS_CHILD | WS_VISIBLE,
    CRect(x3, dy + yo, x3 + width3, dy + height), this, 304);
  dy += height + ym;

  str.LoadString(AfxGetInstanceHandle(), IDS_HEIGHT, mResourceLanguage);
  str += _T(" :");
  mLabel2.Create(str, WS_CHILD | WS_VISIBLE | SS_RIGHT,
    CRect(xm, dy + yo, xm + width1, dy + height), this, 305);
  mEditHeight.Create(WS_CHILD | WS_VISIBLE | WS_BORDER,
    CRect(x2, dy, x2 + width2, dy + height), this, 306);
  mUnit2.Create(_T(""), WS_CHILD | WS_VISIBLE,
    CRect(x3, dy + yo, x3 + width3, dy + height), this, 307);
  dy += height + ym;

  str.LoadString(AfxGetInstanceHandle(), IDS_PRESERVE_XY, mResourceLanguage);
  mPreserveRatioCheck.Create(str, WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
    CRect(xm, dy, right, dy + height), this, 308);
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

  SetValueAsRatio(mWithRatio);
  UpdateValueToControl(3);

  mPreserveRatioCheck.SetCheck(mPreserveRatio ? BST_CHECKED : BST_UNCHECKED);
  mAll.SetCheck(mApplyAll ? BST_CHECKED : BST_UNCHECKED);

  if (gpUiFont != NULL)
  {
    mByRatioCheck.SetFont(gpUiFont);
    mLabel1.SetFont(gpUiFont);
    mEditWidth.SetFont(gpUiFont);
    mUnit1.SetFont(gpUiFont);
    mLabel2.SetFont(gpUiFont);
    mEditHeight.SetFont(gpUiFont);
    mUnit2.SetFont(gpUiFont);
    mPreserveRatioCheck.SetFont(gpUiFont);

    mAll.SetFont(gpUiFont);
    mApply.SetFont(gpUiFont);
  }
  return 0;
}

void KPropertyCtrlResize::OnSize(UINT nType, int cx, int cy)
{
  UpdateScrollInfo();
}

HBRUSH KPropertyCtrlResize::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
  if (nCtlColor == CTLCOLOR_STATIC)
  {
    HBRUSH br = ::GetSysColorBrush(COLOR_WINDOW);
    pDC->SetBkColor(::GetSysColor(COLOR_WINDOW));
    return br;
  }
  return CWnd::OnCtlColor(pDC, pWnd, nCtlColor);
}

BOOL KPropertyCtrlResize::OnCommand(WPARAM wParam, LPARAM lParam)
{
  WORD notify_code = HIWORD(wParam);
  WORD ctrl_id = LOWORD(wParam);
  if (notify_code == BN_CLICKED)
  {
    if (ctrl_id == 301)
    {
      mWithRatio = mByRatioCheck.GetCheck();
      changeValuesAsRatio(mWithRatio);
      SetValueAsRatio(mWithRatio);
      UpdatePreview();
    }
    else if (ctrl_id == 308)
    {
      mPreserveRatio = mPreserveRatioCheck.GetCheck();
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
      mEditWidth.GetWindowText(str);
      int val = 0;
      if ((str.GetLength() > 0) &&
        (swscanf_s(str.GetBuffer(), _T("%d"), &val) > 0) &&
        (mWidth != val) && (mEditBy != 2))
      {
        mWidth = val;
        if (mPreserveRatio)
        {
          if (mWithRatio)
          {
            mHeight = mWidth;
          }
          else
          {
            float r = (float)mWidth / mSourceWidth;
            mHeight = (int)(mSourceHeight * r);
          }
          mEditBy = 1;
          UpdateValueToControl(2);
          mEditBy = 0;
        }
        UpdatePreview();
      }
    }
    else if (ctrl_id == 306)
    {
      mEditHeight.GetWindowText(str);
      int val = 0;
      if ((str.GetLength() > 0) &&
        (swscanf_s(str.GetBuffer(), _T("%d"), &val) > 0) &&
        (mHeight != val) && (mEditBy != 1))
      {
        mHeight = val;
        if (mPreserveRatio)
        {
          if (mWithRatio)
          {
            mWidth = mHeight;
          }
          else
          {
            float r = (float)mHeight / mSourceHeight;
            mWidth = (int)(mSourceWidth * r);
          }
          mEditBy = 2;
          UpdateValueToControl(1);
          mEditBy = 0;
        }
        UpdatePreview();
      }
    }
  }
  return KPropertyCtrlBase::OnCommand(wParam, lParam);
}

void KPropertyCtrlResize::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
  OnVerticalScroll(nSBCode, nPos);
}

void KPropertyCtrlResize::InitProperty(CVivaImagingDoc* doc)
{
  KImageBase* p = doc->GetImagePage(EXCLUDE_FOLDED, -1);
  if ((p != NULL) && (p->GetType() == IMAGE_PAGE))
  {
    KBitmap* b = ((KImagePage *)p)->GetImage(doc->GetWorkingPath());
    if (b != NULL)
    {
      mSourceWidth = b->width;
      mSourceHeight = b->height;
      if (mWithRatio)
      {
        mWidth = 100;
        mHeight = 100;
      }
      else
      {
        mWidth = mSourceWidth;
        mHeight = mSourceHeight;
      }
      UpdateValueToControl(3);
      UpdatePreview();
    }
  }
}

void KPropertyCtrlResize::UpdateValueToControl(int mask)
{
  CString str;
  if (mask & 1)
  {
    str.Format(_T("%d"), mWidth);
    mEditWidth.SetWindowTextW(str);
  }
  if (mask & 2)
  {
    str.Format(_T("%d"), mHeight);
    mEditHeight.SetWindowTextW(str);
  }
}

void KPropertyCtrlResize::changeValuesAsRatio(BOOL asRatio)
{
  if ((mSourceWidth == 0) || (mSourceHeight == 0))
    return;

  if (asRatio)
  {
    mWidth = (int)((float)mWidth * 100 / mSourceWidth + 0.5);
    mHeight = (int)((float)mHeight * 100 / mSourceHeight + 0.5);
  }
  else
  {
    mWidth = (int)((float)mWidth * mSourceWidth / 100 + 0.5);
    mHeight = (int)((float)mHeight * mSourceHeight / 100 + 0.5);
  }
  UpdateValueToControl(3);
}

void KPropertyCtrlResize::SetValueAsRatio(BOOL asRatio)
{
  CString str;

  int sid = (asRatio ? IDS_UNIT_PERCENT : IDS_UNIT_PIXEL);
  str.LoadString(AfxGetInstanceHandle(), sid, mResourceLanguage);

  mUnit1.SetWindowText(str);
  mUnit2.SetWindowText(str);
}

void KPropertyCtrlResize::readyEffectJob(ImageEffectJob& effect)
{
  memset(&effect, 0, sizeof(ImageEffectJob));
  effect.job = PropertyMode::PModeResize;
  effect.param[0] = mWithRatio;
  effect.param[1] = mWidth;
  effect.param[2] = mHeight;
  effect.param[3] = mPreserveRatio;

  mApplyAll = (mAll.GetCheck() == BST_CHECKED);
  effect.applyAll = mApplyAll;
}

void KPropertyCtrlResize::UpdatePreview()
{
  ImageEffectJob effect;
  readyEffectJob(effect);
  KPropertyCtrlBase::UpdatePreview(&effect);
}

void KPropertyCtrlResize::ApplyEffect()
{
  ImageEffectJob effect;
  readyEffectJob(effect);
  KPropertyCtrlBase::ApplyProperty(&effect);
}

void KPropertyCtrlResize::DrawPropertyCtrlPreview(CDC* pDC, KImageDrawer& info, ImageEffectJob* effect)
{
  RECT rect;
  int n_width;
  int n_height;

  if (effect->param[0] == 1)
  {
    n_width = info.src_width * effect->param[1] / 100;
    n_height = info.src_height * effect->param[2] / 100;
  }
  else
  {
    n_width = (int)(effect->param[1]);
    n_height = (int)(effect->param[2]);
  }

  rect.left = (info.src_width - n_width) / 2;
  rect.right = rect.left + n_width;
  rect.top = (info.src_height - n_height) / 2;
  rect.bottom = rect.top + n_height;
  info.ImageToScreen(rect);

  CPen pen;
  pen.CreatePen(PS_DOT, 1, 0x00FFFFFF);
  CPen* old_pen = (CPen*)pDC->SelectObject(&pen);
  CBrush* old_brush = (CBrush*)pDC->SelectStockObject(NULL_BRUSH);
  int old_rop = pDC->SetROP2(R2_XORPEN);
  pDC->Rectangle(&rect);

  pDC->SetROP2(old_rop);
  pDC->SelectObject(old_brush);
  pDC->SelectObject(old_pen);
}

int KPropertyCtrlResize::MouseHitTest(POINT& point, KImageDrawer& info, ImageEffectJob* effect)
{
  RECT rect;
  int n_width;
  int n_height;

  if (effect->param[0] == 1)
  {
    n_width = info.src_width * effect->param[1] / 100;
    n_height = info.src_height * effect->param[2] / 100;
  }
  else
  {
    n_width = (int)(effect->param[1]);
    n_height = (int)(effect->param[2]);
  }

  rect.left = (info.src_width - n_width) / 2;
  rect.right = rect.left + n_width;
  rect.top = (info.src_height - n_height) / 2;
  rect.bottom = rect.top + n_height;

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

int KPropertyCtrlResize::MouseDragging(MouseState state, POINT& point, KImageDrawer& info, ImageEffectJob* effect)
{
  int n_width;
  int n_height;

  if (effect->param[0] == 1)
  {
    n_width = info.src_width * effect->param[1] / 100;
    n_height = info.src_height * effect->param[2] / 100;
  }
  else
  {
    n_width = (int)(effect->param[1]);
    n_height = (int)(effect->param[2]);
  }

  RECT imgarea = { 0, 0, info.src_width, info.src_height };
  if ((state == MouseState::MOUSE_DRAG_CROP_LEFT) || (state == MouseState::MOUSE_DRAG_CROP_RIGHT))
  {
    int v = (state == MouseState::MOUSE_DRAG_CROP_LEFT) ? (point.x - imgarea.left) : (imgarea.right - point.x);
    int changed_width = info.src_width - 2 * v;
    
    TCHAR msg[64];
    StringCchPrintf(msg, 64, _T("width=%d"), changed_width);
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);

    if (effect->param[0] == 1)
      changed_width = (int)((float)changed_width * 100 / info.src_width + 0.5);

    if (changed_width != effect->param[1])
    {
      effect->param[1] = changed_width;

      if (effect->param[3] == 1) // same ratio, change height value also
      {
        if (effect->param[0] == 1)
        {
          effect->param[2] = changed_width;
        }
        else
        {
          float r = (float)changed_width / info.src_width;
          effect->param[2] = (int)((float)r * info.src_height + 0.5);
        }
      }
      return 1;
    }
  }
  else if ((state == MouseState::MOUSE_DRAG_CROP_TOP) || (state == MouseState::MOUSE_DRAG_CROP_BOTTOM))
  {
    int v = (state == MouseState::MOUSE_DRAG_CROP_TOP) ? (point.y - imgarea.top) : (imgarea.bottom - point.y);
    int changed_height = info.src_height - 2 * v;

    TCHAR msg[64];
    StringCchPrintf(msg, 64, _T("changed_height=%d"), changed_height);
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);

    if (effect->param[0] == 1)
      changed_height = (int)((float)changed_height * 100 / info.src_height + 0.5);

    if (changed_height != effect->param[2])
    {
      effect->param[2] = changed_height;

      if (effect->param[3] == 1) // same ratio, change width value also
      {
        if (effect->param[0] == 1)
        {
          effect->param[1] = changed_height;
        }
        else
        {
          float r = (float)changed_height / info.src_height;
          effect->param[1] = (int)((float)r * info.src_width + 0.5);
        }
      }
      return 1;
    }
  }

  return 0;
}

BOOL KPropertyCtrlResize::OnPropertyValueChanged(MouseState state, ImageEffectJob* effect)
{
  if (((state == MouseState::MOUSE_DRAG_CROP_LEFT) || (state == MouseState::MOUSE_DRAG_CROP_RIGHT)) &&
    (mWidth != effect->param[1]))
  {
    int mask = 1;
    mWidth = effect->param[1];

    if (mPreserveRatio)
    {
      float r = (float)mWidth / mSourceWidth;
      mHeight = (int)(r * mSourceHeight + 0.5);
      mask = 3;
    }
    UpdateValueToControl(mask);
  }
  else if (((state == MouseState::MOUSE_DRAG_CROP_TOP) || (state == MouseState::MOUSE_DRAG_CROP_BOTTOM)) &&
    (mHeight != effect->param[2]))
  {
    int mask = 2;
    mHeight = effect->param[2];

    if (mPreserveRatio)
    {
      float r = (float)mHeight / mSourceHeight;
      mWidth = (int)(r * mSourceWidth + 0.5);
      mask = 3;
    }
    UpdateValueToControl(mask);
  }
  return TRUE;
}
