// KPropertyCtrlBase.cpp: 구현 파일
//

#include "stdafx.h"
#include "..\VivaImaging.h"
#include "..\MainFrm.h"

#include "KPropertyCtrlBase.h"
#include "..\Platform\Graphics.h"

#include "KPropertyCtrlRotate.h"
#include "KPropertyCtrlResize.h"
#include "KPropertyCtrlCrop.h"
#include "KPropertyCtrlMonochrome.h"
#include "KPropertyCtrlColorBalance.h"
#include "KPropertyCtrlColorLevel.h"
#include "KPropertyCtrlColorBright.h"
#include "KPropertyCtrlColorHLS.h"
#include "KPropertyCtrlColorHSV.h"
#include "KPropertyCtrlColorReverse.h"
#include "KPropertyCtrlEditImage.h"
#include "KPropertyCtrlAnnotateSelect.h"
#include "KPropertyCtrlAnnotateArrow.h"
#include "KPropertyCtrlAnnotateShape.h"
#include "KPropertyCtrlAnnotateTextbox.h"
// KPropertyCtrlBase

IMPLEMENT_DYNAMIC(KPropertyCtrlBase, CWnd)

KPropertyCtrlBase::KPropertyCtrlBase()
{
  mMinHeight = -1;
  mMaxScrollRange = 0;
  mScrollPos = 0;
  RegisterWndClass();
}

KPropertyCtrlBase::~KPropertyCtrlBase()
{
}


BEGIN_MESSAGE_MAP(KPropertyCtrlBase, CWnd)
END_MESSAGE_MAP()



// KPropertyCtrlBase 메시지 처리기


TCHAR K_PROPERTY_CTRL_CLASS[] = _T("KIMAGINGPROPERTYCTRL");
TCHAR K_PROPERTY_CTRL_WINDOW[] = _T("KImageingPropertyCtrl");

BOOL KPropertyCtrlBase::RegisterWndClass()
{
  WNDCLASS windowclass;
  HINSTANCE hInst = AfxGetInstanceHandle();
  if (!(::GetClassInfo(hInst, K_PROPERTY_CTRL_CLASS, &windowclass)))
  {
    windowclass.style = CS_HREDRAW;
    windowclass.lpfnWndProc = ::DefWindowProc;
    windowclass.cbClsExtra = windowclass.cbWndExtra = 0;
    windowclass.hInstance = hInst;
    windowclass.hIcon = NULL;
    windowclass.hCursor = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
    windowclass.hbrBackground = ::GetSysColorBrush(COLOR_WINDOW);
    windowclass.lpszMenuName = NULL;
    windowclass.lpszClassName = K_PROPERTY_CTRL_CLASS;
    if (!AfxRegisterClass(&windowclass))
    {
      AfxThrowResourceException();
      return FALSE;
    }
  }
  return TRUE;
}

BOOL KPropertyCtrlBase::IsSharedPropertyMode(PropertyMode currMode, PropertyMode newMode)
{
  if (IsSelectShapeMode(currMode) ||
    (currMode == PropertyMode::PModeSelectFill) || (currMode == PropertyMode::PModePasteImage))
  {
    if (IsSelectShapeMode(newMode) ||
      (newMode == PropertyMode::PModeSelectFill) ||
      (newMode == PropertyMode::PModePasteImage))
      return TRUE;
  }
  else if ((currMode == PropertyMode::PModeAnnotateLine) ||
    (currMode == PropertyMode::PModeAnnotateFreehand))
  {
    if ((newMode == PropertyMode::PModeAnnotateLine) ||
      (newMode == PropertyMode::PModeAnnotateFreehand))
      return TRUE;
  }
  else if ((currMode == PropertyMode::PModeAnnotateRectangle) ||
    (currMode == PropertyMode::PModeAnnotatePolygon) ||
    (currMode == PropertyMode::PModeAnnotateEllipse))
  {
    if ((newMode == PropertyMode::PModeAnnotateRectangle) ||
      (newMode == PropertyMode::PModeAnnotatePolygon) ||
      (newMode == PropertyMode::PModeAnnotateEllipse))
      return TRUE;
  }
  else if ((currMode == PropertyMode::PModeAnnotateTextbox) ||
    (currMode == PropertyMode::PModeAnnotateEditText))
  {
    if ((newMode == PropertyMode::PModeAnnotateTextbox) ||
      (newMode == PropertyMode::PModeAnnotateEditText))
      return TRUE;
  }

  return FALSE;
}

KPropertyCtrlBase* KPropertyCtrlBase::CreateControl(PropertyMode mode, CWnd* parent, RECT& rect)
{
  KPropertyCtrlBase* ctrl = NULL;

  if (mode == PropertyMode::PModeRotate)
    ctrl = new KPropertyCtrlRotate(parent, rect);
  else if (mode == PropertyMode::PModeResize)
    ctrl = new KPropertyCtrlResize(parent, rect);
  else if (mode == PropertyMode::PModeCrop)
    ctrl = new KPropertyCtrlCrop(parent, rect);
  else if (mode == PropertyMode::PModeMonochrome)
    ctrl = new KPropertyCtrlMonochrome(parent, rect);
  else if (mode == PropertyMode::PModeColorBalance)
    ctrl = new KPropertyCtrlColorBalance(parent, rect);
  else if (mode == PropertyMode::PModeColorLevel)
    ctrl = new KPropertyCtrlColorLevel(parent, rect);
  else if (mode == PropertyMode::PModeColorBright)
    ctrl = new KPropertyCtrlColorBright(parent, rect);
  else if (mode == PropertyMode::PModeColorHLS)
    ctrl = new KPropertyCtrlColorHLS(parent, rect);
  else if (mode == PropertyMode::PModeColorHSV)
    ctrl = new KPropertyCtrlColorHSV(parent, rect);
  else if (mode == PropertyMode::PModeColorReverse)
    ctrl = new KPropertyCtrlColorReverse(parent, rect);
  else if (IsSelectShapeMode(mode) ||
    (mode == PropertyMode::PModeSelectFill))
    ctrl = new KPropertyCtrlEditImage(parent, rect);
  else if (mode == PropertyMode::PModeAnnotateSelect)
    ctrl = new KPropertyCtrlAnnotateSelect(parent, rect);
  else if ((mode == PropertyMode::PModeAnnotateLine) ||
    (mode == PropertyMode::PModeAnnotateFreehand))
    ctrl = new KPropertyCtrlAnnotateArrow(parent, rect);
  else if ((mode == PropertyMode::PModeAnnotateRectangle) ||
    (mode == PropertyMode::PModeAnnotatePolygon) ||
    (mode == PropertyMode::PModeAnnotateEllipse))
    ctrl = new KPropertyCtrlAnnotateShape(parent, rect);
  else if ((mode == PropertyMode::PModeAnnotateTextbox) || (mode == PropertyMode::PModeAnnotateEditText))
    ctrl = new KPropertyCtrlAnnotateTextbox(parent, rect);

  return ctrl;
}

BOOL KPropertyCtrlBase::Create(DWORD windowStyle, RECT& rect, CWnd* parent, int controlId)
{
  return CWnd::Create(K_PROPERTY_CTRL_CLASS, K_PROPERTY_CTRL_WINDOW, windowStyle, rect, parent, controlId);
}

void KPropertyCtrlBase::UpdatePreview(ImageEffectJob* effect)
{
  CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
  pFrame->UpdatePreview(effect);
}

void KPropertyCtrlBase::DrawPropertyCtrlPreview(CDC* pDC, KImageDrawer& info, ImageEffectJob* effect)
{
  if (effect->job == PropertyMode::PModeResize)
    KPropertyCtrlResize::DrawPropertyCtrlPreview(pDC, info, effect);
  else if (effect->job == PropertyMode::PModeCrop)
    KPropertyCtrlCrop::DrawPropertyCtrlPreview(pDC, info, effect);
}

int KPropertyCtrlBase::UseMouseAction(PropertyMode mode)
{
  return ((PropertyMode::PModeCrop == mode) || (PropertyMode::PModeResize == mode));
}

int KPropertyCtrlBase::MouseHitTest(POINT& dpoint, KImageDrawer& info, ImageEffectJob* effect)
{
  if (effect->job == PropertyMode::PModeCrop)
    return KPropertyCtrlCrop::MouseHitTest(dpoint, info, effect);
  else if (effect->job == PropertyMode::PModeResize)
    return KPropertyCtrlResize::MouseHitTest(dpoint, info, effect);
  return 0;
}

int KPropertyCtrlBase::MouseDragging(MouseState state, POINT& point, KImageDrawer& info, ImageEffectJob* effect)
{
  if (effect->job == PropertyMode::PModeCrop)
    return KPropertyCtrlCrop::MouseDragging(state, point, info, effect);
  else if (effect->job == PropertyMode::PModeResize)
    return KPropertyCtrlResize::MouseDragging(state, point, info, effect);
  return 0;
}

void KPropertyCtrlBase::ApplyProperty(ImageEffectJob* effect)
{
  CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
  pFrame->ApplyProperty(effect);
}

void KPropertyCtrlBase::CloseProperty(ImageEffectJob* effect)
{
  CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
  pFrame->CloseProperty(effect);
}

void KPropertyCtrlBase::FreeImageEffectJob(ImageEffectJob* effect)
{
  if (effect->channel & ImageEffectFontStyle)
  {
    LPCTSTR facename = (LPCTSTR)effect->extra;
    delete[] facename;
  }
}

PropertyGroupMode KPropertyCtrlBase::GetPropertyGroupMode(PropertyMode mode)
{
  if (mode < PropertyMode::PModeAnnotateSelect)
    return PropertyGroupMode::PModeImage;
  else if (mode < PropertyMode::PModeSelectRectangle)
    return PropertyGroupMode::PModeAnnotate;
  else
    return PropertyGroupMode::PModeBlockEdit;
}
void KPropertyCtrlBase::UpdateScrollInfo()
{
  RECT rect;
  GetClientRect(&rect);

  if ((mMinHeight > 0) && ((rect.bottom - rect.top) < mMinHeight))
  {
    mMaxScrollRange = mMinHeight - (rect.bottom - rect.top);
  }
  else
  {
    mMaxScrollRange = 0;
  }
  if (mScrollPos > mMaxScrollRange)
  {
    int nPos = mMaxScrollRange;
    OnVerticalScroll(SB_THUMBTRACK, nPos);
  }
  SetScrollRange(SB_VERT, 0, mMaxScrollRange);
}

int KPropertyCtrlBase::GetVScrollPos(UINT nSBCode, UINT nPos)
{
  int pos = mScrollPos;

  switch (nSBCode)
  {
  case SB_LINEDOWN:
    pos += DialogY2Pixel(20);
    break;
  case SB_LINEUP:
    pos -= DialogY2Pixel(20);
    break;
  case SB_PAGEUP:
    pos -= DialogY2Pixel(100);
    break;
  case SB_PAGEDOWN:
    pos += DialogY2Pixel(100);
    break;
  case SB_TOP:
    pos = 0;
    break;
  case SB_BOTTOM:
    pos = mMaxScrollRange;
    break;
  case SB_THUMBTRACK:
    pos = (int)nPos;
    break;
  case SB_THUMBPOSITION:
    pos = (int)nPos;
    break;
  case SB_ENDSCROLL:
    break;
  default:
    return pos;
  }
  if (pos < 0)
    pos = 0;
  if (pos > mMaxScrollRange)
    pos = mMaxScrollRange;
  return pos;
}

int KPropertyCtrlBase::OnVerticalScroll(UINT nSBCode, UINT nPos)
{
  int pos = GetVScrollPos(nSBCode, nPos);
  if (pos != mScrollPos)
  {
    ScrollWindowEx(0, mScrollPos - pos, NULL, NULL, NULL, NULL, 
      SW_ERASE | SW_INVALIDATE | SW_SCROLLCHILDREN);
    mScrollPos = pos;
    SetScrollPos(SB_VERT, mScrollPos, TRUE);
  }
  return mScrollPos;
}


void KPropertyCtrlBase::adjustControlYPosition(CWnd* ctrl, int dy, int height)
{
  RECT rect;

  ctrl->GetWindowRect(&rect);
  ScreenToClient(&rect);
  rect.top = dy;
  rect.bottom = dy + height;
  ctrl->MoveWindow(&rect);
}

int KPropertyCtrlBase::RearrangeControlYPosition(UINT* arrange_control_ids, int count, int dy, int height)
{
  CWnd* ctrl;

  ctrl = (CWnd*)GetDlgItem(arrange_control_ids[0]);
  if (ctrl != NULL)
  {
    RECT rect;
    ctrl->GetWindowRect(&rect);
    int comboHeight = rect.bottom - rect.top;
    if (height < comboHeight)
    {
      height = comboHeight;
      int ym = DialogY2Pixel(PC_Y_MARGIN);

      for (int i = 0; i < count; i++)
      {
        if (arrange_control_ids[i] > 0)
        {
          ctrl = (CWnd*)GetDlgItem(arrange_control_ids[i]);
          adjustControlYPosition(ctrl, dy, height);
        }
        else // line feed
        {
          dy += height + ym;
        }
      }
      return dy;
    }
  }
  return -1;
}

