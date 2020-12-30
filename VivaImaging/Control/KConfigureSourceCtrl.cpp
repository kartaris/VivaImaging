// KConfigureSourceCtrl.cpp: 구현 파일
//

#include "stdafx.h"
#include "..\VivaImaging.h"

#include "..\Platform\Platform.h"
#include "..\Platform\Utils.h"
#include "..\Platform\Graphics.h"
#include "..\Platform\twain\TwainString.h"
#include "..\Core\KSettings.h"

#include "KConfigureSourceCtrl.h"


// KConfigureSourceCtrl

#define CONFIGURE_LABEL_WIDTH 80
#define CONFIGURE_VALUE_WIDTH 140

#define CFG_LIST_STATIC_ID 20000
#define CFG_LIST_EDIT_ID 22000
#define CFG_LIST_SPIN_ID 23000

TCHAR K_CONFIGURE_SOURCE_CTRL_CLASS[] = _T("KCONFIGURESOURCECTRL");
TCHAR K_CONFIGURE_SOURCE_CTRL[] = _T("KConfigureSourceCtrl");

#define CFGLIST_HORZ_MARGIN 5
#define CFGLIST_VERT_MARGIN 5
#define CFGLIST_ITEM_HEIGHT 10
#define CFGLIST_LINE_HEIGHT 14


IMPLEMENT_DYNAMIC(KConfigureSourceCtrl, CWnd)

KConfigureSourceCtrl::KConfigureSourceCtrl()
{
  mpTwainControl = NULL;
  m_DeviceID = 0;
  mCapCount = 0;
  m_pCapSettings = NULL;
  m_ChildItems = NULL;
  mFont = NULL;
  mScrollMax = 0;
  mScrollPos = 0;
  RegisterWndClass();
}

KConfigureSourceCtrl::~KConfigureSourceCtrl()
{
  if (m_pCapSettings != NULL)
  {
    delete[] m_pCapSettings;
    m_pCapSettings = NULL;
  }
  if (m_ChildItems != NULL)
  {
    delete[] m_ChildItems;
    m_ChildItems = NULL;
  }
}


BEGIN_MESSAGE_MAP(KConfigureSourceCtrl, CWnd)
  ON_WM_VSCROLL()
  ON_WM_CTLCOLOR()
  ON_WM_DESTROY()
END_MESSAGE_MAP()

BOOL KConfigureSourceCtrl::RegisterWndClass()
{
  WNDCLASS windowclass;
  HINSTANCE hInst = AfxGetInstanceHandle();
  if (!(::GetClassInfo(hInst, K_CONFIGURE_SOURCE_CTRL_CLASS, &windowclass)))
  {
    windowclass.style = CS_HREDRAW;
    windowclass.lpfnWndProc = ::DefWindowProc;
    windowclass.cbClsExtra = windowclass.cbWndExtra = 0;
    windowclass.hInstance = hInst;
    windowclass.hIcon = NULL;
    windowclass.hCursor = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
    windowclass.hbrBackground = ::GetSysColorBrush(COLOR_WINDOW);
    windowclass.lpszMenuName = NULL;
    windowclass.lpszClassName = K_CONFIGURE_SOURCE_CTRL_CLASS;
    if (!AfxRegisterClass(&windowclass))
    {
      AfxThrowResourceException();
      return FALSE;
    }
  }
  return TRUE;
}


// KConfigureSourceCtrl 메시지 처리기

BOOL KConfigureSourceCtrl::PreCreateWindow(CREATESTRUCT& cs)
{
  /*
  SIZE total_size, page_size, line_size;
  total_size.cx = cs.cx;
  total_size.cy = 10;
  page_size.cx = cs.cx;
  page_size.cy = cs.cy;
  line_size.cx = cs.cx;
  line_size.cy = DialogY2Pixel(14);


  SetScrollSizes(MM_TEXT, total_size, page_size, line_size);
  */
  // cs.style |= WS_VSCROLL;
  return CWnd::PreCreateWindow(cs);
}

void KConfigureSourceCtrl::OnDraw(CDC* pDC)
{

}

HBRUSH KConfigureSourceCtrl::OnCtlColor(CDC*pDC, CWnd*pWnd, UINT nCtlColor)
{
  UINT id = pWnd->GetDlgCtrlID();
  if ((CTLCOLOR_STATIC == nCtlColor) &&
    (id >= CFG_LIST_STATIC_ID) &&
    (id < CFG_LIST_STATIC_ID + 1000))
  {
    COLORREF color = GetSysColor(COLOR_WINDOW);
    pDC->SetBkColor(color);
    return ::GetSysColorBrush(COLOR_WINDOW);
  }

  return CWnd::OnCtlColor(pDC, pWnd, nCtlColor);
}

void KConfigureSourceCtrl::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
  int delta = mScrollPos;

  switch (nSBCode)
  { /* nSBCode */
  case SB_LINEDOWN:
    delta += DialogY2Pixel(14);
    break;
  case SB_LINEUP:
    delta -= DialogY2Pixel(14);
    break;
  case SB_PAGEUP:
    delta -= mPageSize;
    break;
  case SB_PAGEDOWN:
    delta += mPageSize;
    break;
  case SB_TOP:
    delta = 0;
    break;
  case SB_BOTTOM:
    delta = mScrollMax;
    break;
  case SB_THUMBTRACK:
    delta = (int)nPos;
    break;
  case SB_THUMBPOSITION:
    delta = (int)nPos;
    break;
  case SB_ENDSCROLL:
    break;
  default:
    return;
  } /* nSBCode */

  if (delta < 0)
    delta = 0;
  if (delta > mScrollMax)
    delta = mScrollMax;

  if (delta != mScrollPos)
  {
    int scroll_amount = delta - mScrollPos;
    mScrollPos = delta;
    SetScrollPos(SB_VERT, mScrollPos, TRUE);
    ScrollWindow(0, -scroll_amount);
  }
}

BOOL KConfigureSourceCtrl::OnCommand(WPARAM wParam, LPARAM lParam)
{
  WORD notify_code = HIWORD(wParam);
  if (notify_code == CBN_SELCHANGE)
  {
    WORD ctrl_id = LOWORD(wParam);
    DWORD selectedIndex = ::SendMessage((HWND)lParam, CB_GETCURSEL, 0, 0);
    if (selectedIndex != CB_ERR)
    {
      int cap_index = getCapIndexByHandle((HWND)lParam);
      if (cap_index >= 0)
      {
        ChangeDeviceCaps(m_pCapSettings[cap_index], selectedIndex);
      }
    }
    return TRUE;
  }
  else if (notify_code == EN_CHANGE)
  {
    WORD ctrl_id = LOWORD(wParam);
    TCHAR value[128];
    DWORD len = ::SendMessage((HWND)lParam, WM_GETTEXT, 127, (LPARAM)value);
    if (len > 0)
    {
      int cap_index = getCapIndexByHandle((HWND)lParam);
      LPSTR pValue = MakeUtf8String(value);
      if (cap_index >= 0)
        ChangeDeviceCaps(m_pCapSettings[cap_index], pValue);
      delete[] pValue;
    }
  }
  return 0;
}

void KConfigureSourceCtrl::OnDestroy()
{
  if ((m_ChildItems != NULL) && (mCapCount > 0))
  {
    for (int i = 0; i < (int)mCapCount; i++)
    {
      if (m_ChildItems[i].subctrl != NULL)
      {
        CSpinButtonCtrl* pSpin = (CSpinButtonCtrl*) m_ChildItems[i].subctrl;
        if (pSpin->GetSafeHwnd() != NULL)
        delete pSpin;
      }
    }
  }
}

void KConfigureSourceCtrl::CreateSourceCapsChilds(KTwainControl* twainControl, UINT32 deviceID)
{
  TW_UINT32         i;
  TW_CAPABILITY     CapSupportedCaps;
  pTW_ARRAY_UINT16  pCapSupCaps = 0;
  string            sCapName;
  string            sItemValue;
  RECT rect;
  GetClientRect(&rect);

  mpTwainControl = twainControl;
  m_DeviceID = deviceID;

  // get the supported capabilies
  CapSupportedCaps.Cap = CAP_SUPPORTEDCAPS;
  CapSupportedCaps.hContainer = 0;

  mpTwainControl->GetCAP(CapSupportedCaps);

  if (TWON_ARRAY != CapSupportedCaps.ConType)
  {
    goto cleanup;
  }

  pCapSupCaps = (pTW_ARRAY_UINT16)_DSM_LockMemory(CapSupportedCaps.hContainer);

  if (TWTY_UINT16 != pCapSupCaps->ItemType)
  {
    goto cleanup;
  }

  mCapCount = pCapSupCaps->NumItems;

  if (m_pCapSettings != NULL)
    delete[] m_pCapSettings;
  m_pCapSettings = new CapSetting[mCapCount];
  memset(m_pCapSettings, 0, sizeof(CapSetting) * mCapCount);
  if (m_ChildItems != NULL)
    delete[] m_ChildItems;
  m_ChildItems = new SourceCapsChildItem[mCapCount];
  memset(m_ChildItems, 0, sizeof(SourceCapsChildItem) * mCapCount);

  int y = CFGLIST_VERT_MARGIN;

  for (i = 0; i<mCapCount; i++)
  {
    KVoidPtrArray arg;

    // we are not listing this CAPABILITIES
    if (pCapSupCaps->ItemList[i] != CAP_SUPPORTEDCAPS)
    {
      LPCSTR name = NULL;
      if (TWRC_SUCCESS == mpTwainControl->GetLabel(pCapSupCaps->ItemList[i], sCapName))
      {
        name = (LPSTR)sCapName.c_str();
      }
      else
      {
        name = convertCAP_toString(pCapSupCaps->ItemList[i]);
      }

      m_pCapSettings[i].CapID = pCapSupCaps->ItemList[i];
      m_pCapSettings[i].byChanged = 0;
      m_pCapSettings[i].bReadOnly = FALSE;

      TW_CAPABILITY Cap;
      Cap.Cap = m_pCapSettings[i].CapID;
      Cap.hContainer = 0;
      Cap.ConType = TWON_DONTCARE16;
      UINT selectIndex = 0;

      // We will ignor what Query Supported has reproted about Message Get and get the current anyway.
      TW_UINT16 CondCode = mpTwainControl->GetCAP(Cap);
      if (CondCode == TWCC_SUCCESS)
      {
        pTW_ONEVALUE pCap = NULL;
        if (Cap.hContainer
          && NULL != (pCap = (pTW_ONEVALUE)_DSM_LockMemory(Cap.hContainer)))
        {
          TW_UINT16 type = pCap->ItemType;
          _DSM_UnlockMemory(Cap.hContainer);

          switch (type)
          {
          case TWTY_INT8:
          case TWTY_UINT8:
          case TWTY_INT16:
          case TWTY_UINT16:
          case TWTY_INT32:
          case TWTY_UINT32:
          case TWTY_BOOL:
          {
            TW_UINT32 uCurrentValue;
            getCurrent(&Cap, uCurrentValue);
            if ((Cap.ConType == TWON_ARRAY) || (Cap.ConType == TWON_ENUMERATION))
            {
              TW_UINT32 uVal;
              TW_UINT32 uCount = 0;

              while (GetItem(&Cap, uCount, uVal))
              {
                const char *ar = convertCAP_Item_toString(Cap.Cap, uVal, type);
                if (uCurrentValue == uVal)
                  selectIndex = arg.size();
                arg.push_back(MakeUnicodeString(ar));
                uCount++;
              }
            }
            else
            {
              const char *ar = convertCAP_Item_toString(Cap.Cap, uCurrentValue, type);
              arg.push_back(MakeUnicodeString(ar));
            }
            break;
          }

          case TWTY_STR32:
          case TWTY_STR64:
          case TWTY_STR128:
          case TWTY_STR255:
          {
            string sCurrentValue;
            getCurrent(&Cap, sCurrentValue);
            if ((Cap.ConType == TWON_ARRAY) || (Cap.ConType == TWON_ENUMERATION))
            {
              string sVal;
              TW_UINT32 uCount = 0;

              while (GetItem(&Cap, uCount, sVal))
              {
                if (sCurrentValue.compare(sVal) == 0)
                  selectIndex = arg.size();
                arg.push_back(MakeUnicodeString(sVal.c_str()));
                uCount++;
              }
            }
            else
            {
              arg.push_back(MakeUnicodeString(sCurrentValue.c_str()));
            }
            break;
          }

          case TWTY_FIX32:
          {
            char value[32];
            TW_FIX32 current;
            getCurrent(&Cap, current);

            if ((Cap.ConType == TWON_ARRAY) || (Cap.ConType == TWON_ENUMERATION))
            {
              TW_UINT32 uCount = 0;
              TW_FIX32 fix32;

              while (GetItem(&Cap, uCount, fix32))
              {
                if (current == fix32)
                  selectIndex = arg.size();
                sprintf_s(value, 32, "%d.%d", fix32.Whole, (int)((fix32.Frac / 65536.0 + .0005) * 1000));
                arg.push_back(MakeUnicodeString(value));
                uCount++;
              }
            }
            else
            {
              sprintf_s(value, 32, "%d.%d", current.Whole, (int)((current.Frac / 65536.0 + .0005) * 1000));
              arg.push_back(MakeUnicodeString(value));
            }
            break;
          }

          case TWTY_FRAME:
          {
            char value[64];
            TW_FRAME current_frame;

            getCurrent(&Cap, current_frame);
            if ((Cap.ConType == TWON_ARRAY) || (Cap.ConType == TWON_ENUMERATION))
            {
              TW_UINT32 uCount = 0;
              TW_FRAME frame;

              while (GetItem(&Cap, uCount, frame))
              {
                if ((frame.Left == current_frame.Left) &&
                  (frame.Right == current_frame.Right) &&
                  (frame.Top == current_frame.Top) &&
                  (frame.Bottom == current_frame.Bottom))
                  selectIndex = arg.size();

                sprintf_s(value, 64, "%d.%d  %d.%d  %d.%d  %d.%d",
                  frame.Left.Whole, (int)((frame.Left.Frac / 65536.0 + .0005) * 1000),
                  frame.Right.Whole, (int)((frame.Right.Frac / 65536.0 + .0005) * 1000),
                  frame.Top.Whole, (int)((frame.Top.Frac / 65536.0 + .0005) * 1000),
                  frame.Bottom.Whole, (int)((frame.Bottom.Frac / 65536.0 + .0005) * 1000));
                arg.push_back(MakeUnicodeString(value));
                uCount++;
              }
            }
            else
            {
              sprintf_s(value, 64, "%d.%d  %d.%d  %d.%d  %d.%d",
                current_frame.Left.Whole, (int)((current_frame.Left.Frac / 65536.0 + .0005) * 1000),
                current_frame.Right.Whole, (int)((current_frame.Right.Frac / 65536.0 + .0005) * 1000),
                current_frame.Top.Whole, (int)((current_frame.Top.Frac / 65536.0 + .0005) * 1000),
                current_frame.Bottom.Whole, (int)((current_frame.Bottom.Frac / 65536.0 + .0005) * 1000));
              arg.push_back(MakeUnicodeString(value));
            }
            break;
          }
          default:
          {
            arg.push_back(MakeUnicodeString("?"));
            break;
          }
          }
        }
        else //if(Cap.hContainer)
        {
          //sItemValue = "<<Invalid Container>>";
          arg.push_back(MakeUnicodeString("<<Invalid Container>>"));
        }
      }
      else
      {
        char sError[256];
        sprintf_s(sError, 256, "<<%s>>", convertConditionCode_toString(CondCode));
        arg.push_back(MakeUnicodeString(sError));
      }

      TW_UINT32 nNumItems = 1;
      if (Cap.ConType == TWON_ARRAY
        || Cap.ConType == TWON_ENUMERATION)
      {
        pTW_ARRAY pCap = (pTW_ARRAY)_DSM_LockMemory(Cap.hContainer);
        if (pCap)
        {
          nNumItems = pCap->NumItems;
          _DSM_UnlockMemory(Cap.hContainer);
        }
      }

      LPTSTR pName = MakeUnicodeString(name);
      if (nNumItems != arg.size())
      {
        TCHAR msg[256];
        StringCchPrintf(msg, 256, _T("Argument count mismatch : %s (%d/%d)"), pName, nNumItems, arg.size());
        StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG | SYNC_MSG_DEBUG);
      }
      TW_UINT32 QS = 0;
      mpTwainControl->QuerySupport_CAP(Cap.Cap, QS);

      /*
      if (QS & (TWQC_GET | TWQC_GETDEFAULT | TWQC_GETCURRENT))
      {
        sItemValue += "Get";
      }
      if (QS & TWQC_SET)
      {
        if (0 != sItemValue.length())
          sItemValue += ", ";
        sItemValue += "Set";
      }
      if (QS & TWQC_RESET)
      {
        if (0 != sItemValue.length())
          sItemValue += ", ";
        sItemValue += "Reset";
      }
      */

      if ((Cap.ConType == TWON_ENUMERATION && nNumItems <= 1)
        || !(QS & TWQC_SET))
      {
        m_pCapSettings[i].bReadOnly = TRUE;
      }

      m_ChildItems[i].label = createLabel(pName, y, CFG_LIST_STATIC_ID + i, m_pCapSettings[i].bReadOnly);
      m_ChildItems[i].ctrl_type = 0;
      m_ChildItems[i].ctrl = createControl(pName, y, CFG_LIST_EDIT_ID + i, m_pCapSettings[i].bReadOnly, Cap, arg, selectIndex, rect.right, m_ChildItems[i]);

      if (pName != NULL)
        delete[] pName;

      ClearVoidPtrArray(arg);

      y += CFGLIST_LINE_HEIGHT; // dialog unit
    }
  }

  y += CFGLIST_VERT_MARGIN; // bottom space

  SetScrollbar(y);

cleanup:
  if (pCapSupCaps)
  {
    _DSM_UnlockMemory(CapSupportedCaps.hContainer);
  }
  if (CapSupportedCaps.hContainer)
  {
    _DSM_Free(CapSupportedCaps.hContainer);
  }

}

HWND KConfigureSourceCtrl::createLabel(LPCTSTR name, int dy, int ctrlId, BOOL bReadOnly)
{
  HINSTANCE instance = AfxGetInstanceHandle();
  int x = DialogX2Pixel(CFGLIST_HORZ_MARGIN);
  int y = DialogY2Pixel(dy);
  int width = DialogX2Pixel(CONFIGURE_LABEL_WIDTH);
  int height = DialogX2Pixel(10);

  DWORD wstyle = WS_CHILD | WS_VISIBLE;
  HWND h = ::CreateWindow(_T("STATIC"), name, wstyle|SS_LEFT, x, y, width, height, GetSafeHwnd(), (HMENU)ctrlId, instance, NULL);

  if (mFont != NULL)
    ::SendMessage(h, WM_SETFONT, (WPARAM)mFont->GetSafeHandle(), NULL);
  return h;
}

HWND KConfigureSourceCtrl::createControl(PCTSTR name, int dy, int ctrlId, BOOL bReadOnly, TW_CAPABILITY& Cap, KVoidPtrArray& arg, 
  UINT selectedIndex, int right, SourceCapsChildItem& item)
{
  HINSTANCE instance = AfxGetInstanceHandle();
  int x = DialogX2Pixel(CONFIGURE_LABEL_WIDTH + CFGLIST_HORZ_MARGIN * 2);
  int y = DialogY2Pixel(dy);
  int width = right - DialogX2Pixel(CFGLIST_HORZ_MARGIN) - x;
  int height = DialogX2Pixel(CFGLIST_ITEM_HEIGHT);
  HWND h = NULL;
  DWORD wstyle = WS_CHILD | WS_VISIBLE;

  if ((Cap.ConType == TWON_ARRAY) ||
    (Cap.ConType == TWON_ENUMERATION))
  {
    h = ::CreateWindow(_T("COMBOBOX"), name, wstyle| WS_VSCROLL | CBS_DROPDOWNLIST, x, y, width, height, GetSafeHwnd(), (HMENU)ctrlId, instance, NULL);
    if (mFont != NULL)
      ::SendMessage(h, WM_SETFONT, (WPARAM)mFont->GetSafeHandle(), NULL);

    KVoidPtrIterator it = arg.begin();
    while (it != arg.end())
    {
      LPTSTR str = (LPTSTR)*it++;
      ::SendMessage(h, CB_ADDSTRING, 0, (LPARAM)str);
    }

    ::SendMessage(h, CB_SETCURSEL, selectedIndex, (LPARAM)NULL);
  }
  else if (Cap.ConType == TWON_RANGE)
  {
    TCHAR value[32] = _T("");
    h = ::CreateWindow(_T("EDIT"), name, wstyle | WS_BORDER, x, y, width, height, GetSafeHwnd(), (HMENU)ctrlId, instance, NULL);
    if (mFont != NULL)
      ::SendMessage(h, WM_SETFONT, (WPARAM)mFont->GetSafeHandle(), NULL);

    RECT rect;
    rect.left = x + width;
    rect.top = y;
    rect.right = rect.left + DialogX2Pixel(10);
    rect.bottom = rect.top + height;

    CSpinButtonCtrl *pSpin = new CSpinButtonCtrl();
    pSpin->Create(wstyle | UDS_ALIGNRIGHT, rect, this, ctrlId + 1000);

    pSpin->SetBuddy(CWnd::FromHandle(h));
    if (mFont != NULL)
    pSpin->SetFont(mFont);

    item.subctrl = pSpin;

    pTW_RANGE pCapPT = (pTW_RANGE)_DSM_LockMemory(Cap.hContainer);
    if (pCapPT->ItemType <= TWTY_FIX32)
    {
      pSpin->SetRange32(pCapPT->MinValue, pCapPT->MaxValue);
      //pCapPT->StepSize;
      //pCapPT->DefaultValue;
      StringCchPrintf(value, 32, _T("%d"), pCapPT->CurrentValue);
    }
    _DSM_UnlockMemory(Cap.hContainer);

    ::SendMessage(h, WM_SETTEXT, NULL, (LPARAM)value);
  }
  else if (Cap.ConType == TWON_ONEVALUE)
  {
    h = ::CreateWindow(_T("EDIT"), name, wstyle | WS_BORDER, x, y, width, height, GetSafeHwnd(), (HMENU)ctrlId, instance, NULL);
    if (mFont != NULL)
      ::SendMessage(h, WM_SETFONT, (WPARAM)mFont->GetSafeHandle(), NULL);

    if (arg.size() > 0)
    {
      KVoidPtrIterator it = arg.begin();
      LPTSTR p = (LPTSTR)*it;
      ::SendMessage(h, WM_SETTEXT, NULL, (LPARAM)p);
    }
    else
    {
      TW_UINT32 val;
      getCurrent(&Cap, val);
      TCHAR value[32] = _T("");
      StringCchPrintf(value, 32, _T("%d"), val);
      ::SendMessage(h, WM_SETTEXT, NULL, (LPARAM)value);
    }
  }
  return h;
}

void KConfigureSourceCtrl::SetScrollbar(int dialogHeight)
{
  RECT rect;
  GetClientRect(&rect);

  mPageSize = rect.bottom - rect.top;
  mScrollMax = DialogY2Pixel(dialogHeight) - mPageSize;
  if (mScrollMax < 0)
    mScrollMax = 0;
  if (mScrollPos > mScrollMax)
    mScrollPos = mScrollMax;

  SCROLLINFO si = { sizeof(SCROLLINFO) };
  si.fMask = SIF_ALL;
  si.nMin = 0;
  si.nMax = mScrollMax + mPageSize;
  si.nPos = mScrollPos;
  si.nPage = mPageSize;
  SetScrollInfo(SB_VERT, &si, TRUE);

  /*
  SIZE total_size, page_size, line_size;
  total_size.cx = rect.right - rect.left;
  total_size.cy = ;
  page_size.cx = rect.right - rect.left;
  page_size.cy = rect.bottom - rect.top;
  line_size.cx = rect.right - rect.left;
  line_size.cy = DialogY2Pixel(14);
  SetScrollSizes(MM_TEXT, total_size, page_size, line_size);

  */
}

int KConfigureSourceCtrl::getCapIndexByHandle(HWND ctrlhWnd)
{
  for (UINT32 i = 0; i < mCapCount; i++)
  {
    if (m_ChildItems[i].ctrl == ctrlhWnd)
      return i;
  }
  return -1;
}

BOOL KConfigureSourceCtrl::ChangeDeviceCaps(CapSetting& setting, int selectedIndex)
{
  BOOL bChanged = FALSE;
  TW_CAPABILITY Cap;
  Cap.Cap = setting.CapID;
  Cap.hContainer = 0;
  Cap.ConType = TWON_DONTCARE16;

  LPCSTR name = NULL;
  string sCapName;
  LPTSTR capName = NULL;
  TCHAR msg[256] = _T("");

  /*
  mpTwainControl->ConnectDSM();
  if (!mpTwainControl->LoadDataSource(m_DeviceID))
  {
    mpTwainControl->DisconnectDSM();
    return FALSE;
  }
  */
  if (mpTwainControl->GetState() < 4)
  {
    StoreLogHistory(_T(__FUNCTION__), _T("Device not ready to change config"), SYNC_MSG_LOG | SYNC_MSG_DEBUG);
    return FALSE;
  }

  if (TWRC_SUCCESS == mpTwainControl->GetLabel(setting.CapID, sCapName))
  {
    name = (LPSTR)sCapName.c_str();
  }
  else
  {
    name = convertCAP_toString(setting.CapID);
  }
  capName = MakeUnicodeString(name);

  // We will ignor what Query Supported has reproted about Message Get and get the current anyway.
  TW_UINT16 CondCode = mpTwainControl->GetCAP(Cap);
  if (CondCode != TWCC_SUCCESS)
  {
    delete[] capName;
    return FALSE;
  }

  if (CAP_XFERCOUNT == Cap.Cap)
  {
    TW_UINT32 Val;
    getCurrent(&Cap, Val);
    Val = (TW_INT16)Val == -1 ? 1 : -1;// flop
    mpTwainControl->SetCapabilityOneValue(Cap.Cap, Val, TWTY_INT16);
    bChanged = TRUE;
  }
  else if (Cap.ConType == TWON_ENUMERATION)
  {
    pTW_ONEVALUE pCap = NULL;
    if (Cap.hContainer
      && NULL != (pCap = (pTW_ONEVALUE)_DSM_LockMemory(Cap.hContainer)))
    {
      TW_UINT16 type = pCap->ItemType;
      _DSM_UnlockMemory(Cap.hContainer);

      switch (type)
      {
      case TWTY_INT8:
      case TWTY_UINT8:
      case TWTY_INT16:
      case TWTY_UINT16:
      case TWTY_INT32:
      case TWTY_UINT32:
      case TWTY_BOOL:
      {
        if ((Cap.ConType == TWON_ARRAY) || (Cap.ConType == TWON_ENUMERATION))
        {
          TW_UINT32 uVal;
          TW_UINT32 uCount = 0;
          TW_UINT32 uCurrentValue = 0;
          const char *arg_name = NULL;

          while (GetItem(&Cap, uCount, uVal))
          {
            if (uCount == selectedIndex)
            {
              arg_name = convertCAP_Item_toString(Cap.Cap, uVal, type);
              uCurrentValue = uVal;
              break;
            }
            uCount++;
          }
          if (arg_name != NULL)
          {
            if (TWRC_SUCCESS == mpTwainControl->SetCapabilityOneValue(Cap.Cap, (TW_UINT16)selectedIndex, type))
            {
              bChanged = TRUE;
            }
            else
            {
              TCHAR msg[256];
              LPTSTR aname = MakeUnicodeString(arg_name);
              StringCchPrintf(msg, 256, _T("SetCapability fail %s as %s"), capName, aname);
              StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG | SYNC_MSG_DEBUG);
              delete[] aname;
            }
          }
          else
          {
            TCHAR msg[256];
            StringCchPrintf(msg, 256, _T("SetCapability fail %s out of range(%d/%d)"), capName, selectedIndex, uCount);
            StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG | SYNC_MSG_DEBUG);
          }
        }
        break;
      }

      case TWTY_FIX32:
      {
        char value[32];
        TW_FIX32 selectedValue;
        BOOL hasValid = FALSE;

        if ((Cap.ConType == TWON_ARRAY) || (Cap.ConType == TWON_ENUMERATION))
        {
          TW_UINT32 uCount = 0;
          TW_FIX32 fix32;

          while (GetItem(&Cap, uCount, fix32))
          {
            if (uCount == selectedIndex)
            {
              sprintf_s(value, 32, "%d.%d", fix32.Whole, (int)((fix32.Frac / 65536.0 + .0005) * 1000));
              selectedValue = fix32;
              hasValid = TRUE;
              break;
            }
            uCount++;
          }

          if (hasValid)
          {
            if (TWRC_SUCCESS == mpTwainControl->SetCapabilityOneValue(Cap.Cap, &selectedValue))
            {
              bChanged = TRUE;
            }
            else
            {
              TCHAR msg[256];
              LPTSTR aname = MakeUnicodeString(value);
              StringCchPrintf(msg, 256, _T("SetCapability fail %s as %s"), capName, aname);
              StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG | SYNC_MSG_DEBUG);
              delete[] aname;
            }
          }
          else
          {
            TCHAR msg[256];
            StringCchPrintf(msg, 256, _T("SetCapability fail %s out of range(%d/%d)"), capName, selectedIndex, uCount);
            StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG | SYNC_MSG_DEBUG);
          }
        }
        break;
      }

      case TWTY_STR32:
      case TWTY_STR64:
      case TWTY_STR128:
      case TWTY_STR255:
      case TWTY_FRAME:
      default:
      {
        StringCchPrintf(msg, 256, _T("Unsupported data types for ENUM type"));
        break;
      }
      }
    }
    else
    {
      StringCchPrintf(msg, 256, _T("Access Fail on Cap Container"));
    }
  }
  else if (Cap.ConType == TWON_ONEVALUE)
  {
    pTW_ONEVALUE pCapPT = (pTW_ONEVALUE)_DSM_LockMemory(Cap.hContainer);
    switch (pCapPT->ItemType)
    {
    case TWTY_BOOL:
    {
      // Not displaying a dialog just try to set the opposit value
      TW_INT16  twrc = mpTwainControl->SetCapabilityOneValue(Cap.Cap, (TW_UINT16)pCapPT->Item ? FALSE : TRUE, TWTY_BOOL);
      if (twrc == TWRC_SUCCESS || twrc == TWRC_CHECKSTATUS)
      {
        bChanged = true;
      }
    }
    break;

    default:
      StringCchPrintf(msg, 256, _T("ONEVALUE only have BOOL type"));
      break;
    }
    _DSM_UnlockMemory(Cap.hContainer);

  }
  else
  {
    StringCchPrintf(msg, 256, _T("Canot modify values"), capName);
  }

  if (!bChanged)
  {
    if (lstrlen(msg) == 0)
      StringCchPrintf(msg, 256, _T("Unhandled "));
    StringCchCat(msg, 256, _T(" : "));
    StringCchCat(msg, 256, capName);

    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG | SYNC_MSG_DEBUG);
  }
  delete[] capName;
  return bChanged;
}

BOOL KConfigureSourceCtrl::ChangeDeviceCaps(CapSetting& setting, LPCSTR pValue)
{
  BOOL bChanged = FALSE;
  TW_CAPABILITY Cap;
  Cap.Cap = setting.CapID;
  Cap.hContainer = 0;
  Cap.ConType = TWON_DONTCARE16;

  LPCSTR name = NULL;
  string sCapName;
  LPTSTR capName = NULL;
  TCHAR msg[256] = _T("");

  /*
  mpTwainControl->ConnectDSM();
  if (!mpTwainControl->LoadDataSource(m_DeviceID))
  {
  mpTwainControl->DisconnectDSM();
  return FALSE;
  }
  */
  if (mpTwainControl->GetState() < 4)
  {
    StoreLogHistory(_T(__FUNCTION__), _T("Device not ready to change config"), SYNC_MSG_LOG | SYNC_MSG_DEBUG);
    return FALSE;
  }

  if (TWRC_SUCCESS == mpTwainControl->GetLabel(setting.CapID, sCapName))
  {
    name = (LPSTR)sCapName.c_str();
  }
  else
  {
    name = convertCAP_toString(setting.CapID);
  }
  capName = MakeUnicodeString(name);

  // We will ignor what Query Supported has reproted about Message Get and get the current anyway.
  TW_UINT16 CondCode = mpTwainControl->GetCAP(Cap);
  if (CondCode != TWCC_SUCCESS)
  {
    delete[] capName;
    return FALSE;
  }

  if (CAP_XFERCOUNT == Cap.Cap)
  {
    TW_UINT32 Val;
    getCurrent(&Cap, Val);
    Val = (TW_INT16)Val == -1 ? 1 : -1;// flop
    mpTwainControl->SetCapabilityOneValue(Cap.Cap, Val, TWTY_INT16);
    bChanged = TRUE;
  }
  else if (Cap.ConType == TWON_ONEVALUE)
  {
    pTW_ONEVALUE pCapPT = (pTW_ONEVALUE)_DSM_LockMemory(Cap.hContainer);
    switch (pCapPT->ItemType)
    {
    case TWTY_FIX32:
    {
      float fv;
      sscanf_s(pValue, "%f", &fv);
      TW_FIX32 fix32 = FloatToFIX32(fv);
      TW_INT16  twrc = mpTwainControl->SetCapabilityOneValue(Cap.Cap, &fix32);
      if (twrc == TWRC_SUCCESS || twrc == TWRC_CHECKSTATUS)
      {
        bChanged = true;
      }

    }
    case TWTY_FRAME:
    {
      TW_FRAME frame;
      float left, right, top, bottom;

      sscanf_s(pValue, "%f  %f  %f  %f", &left, &right, &top, &bottom);

      frame.Left = FloatToFIX32(left);
      frame.Right = FloatToFIX32(right);
      frame.Top = FloatToFIX32(top);
      frame.Bottom = FloatToFIX32(bottom);

      TW_INT16  twrc = mpTwainControl->SetCapabilityOneValue(Cap.Cap, &frame);
      if (twrc == TWRC_SUCCESS || twrc == TWRC_CHECKSTATUS)
      {
        bChanged = true;
      }
    }

    default:
      StringCchPrintf(msg, 256, _T("ONEVALUE only have FIX or FRAME type"));
      break;
    }
    _DSM_UnlockMemory(Cap.hContainer);

  }
  else
  {
    StringCchPrintf(msg, 256, _T("Canot modify values as string"), capName);
  }

  if (!bChanged)
  {
    if (lstrlen(msg) == 0)
      StringCchPrintf(msg, 256, _T("Unhandled "));
    StringCchCat(msg, 256, _T(" : "));
    StringCchCat(msg, 256, capName);

    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG | SYNC_MSG_DEBUG);
  }
  delete[] capName;
  return bChanged;
}
