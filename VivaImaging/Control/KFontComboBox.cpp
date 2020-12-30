// KFontComboBox.cpp: 구현 파일
//

#include "stdafx.h"
#include "..\VivaImaging.h"
#include "..\Core\KConstant.h"
#include "..\Core\KSettings.h"
#include "KFontComboBox.h"
#include "..\Platform\Platform.h"
#include "..\Platform\Graphics.h"

// KFontComboBox

IMPLEMENT_DYNAMIC(KFontComboBox, CComboBox)

KFontComboBox::KFontComboBox()
{
}

KFontComboBox::~KFontComboBox()
{
}


BEGIN_MESSAGE_MAP(KFontComboBox, CWnd)
END_MESSAGE_MAP()



// KFontComboBox 메시지 처리기

BOOL KFontComboBox::Create(DWORD windowStyle, RECT& rect, CWnd* parent, int controlId)
{
  return CComboBox::Create(windowStyle, rect, parent, controlId);
}


int CALLBACK enumFontFamiliesProc(
  const LOGFONT    *lpelfe,
  const TEXTMETRIC *lpntme,
  DWORD      FontType,
  LPARAM     lParam
)
{
  if ((TRUETYPE_FONTTYPE & FontType) == 0)
    lpelfe = NULL;
  KFontComboBox* p = (KFontComboBox*)lParam;
  return p->AddFontFamily(lpelfe);
}

int gFontSizes[] = { 8, 9, 10, 12, 14, 16, 18, 20, 24, 28, 32, 40, 64, 80 };


int KFontComboBox::AddFontFamily(const LOGFONT* lf)
{
  if (lf != NULL)
  {
    LPCTSTR facename = lf->lfFaceName;
    if (facename[0] == '@')
      facename++;
    if (FindString(0, facename) == CB_ERR)
      AddString(lf->lfFaceName);
  }
  return 1;
}

int KFontComboBox::InitItems(int type)
{
  int count = 0;
  if (type == FONT_COMBO_FACENAME)
  {
    CDC* hdc = GetDC();
    LOGFONT lf;
    memset(&lf, 0, sizeof(LOGFONT));
    lf.lfCharSet = DEFAULT_CHARSET;
    // lf.lfOutPrecision = OUT_TT_ONLY_PRECIS;
    EnumFontFamiliesEx(hdc->GetSafeHdc(), &lf, enumFontFamiliesProc, (LPARAM)this, 0);

    count = GetCount();
    TCHAR msg[120];
    StringCchPrintf(msg, 120, _T("Enum %d fonts"), count);
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG | SYNC_MSG_DEBUG);
  }
  else if (type == FONT_COMBO_FONTSIZE)
  {
    count = sizeof(gFontSizes) / sizeof(int);
    for (int i = 0; i < count; i++)
    {
      CString str;
      str.Format(_T("%d"), gFontSizes[i]);
      AddString(str);
    }
  }
  return count;
}

int KFontComboBox::SetSelectFaceName(LPCTSTR faceName)
{
  int index = FindString(0, faceName);
  if (index != CB_ERR)
    SetCurSel(index);
  return index;
}

int KFontComboBox::SetSelectFontSize(int ptSize)
{
  int count = sizeof(gFontSizes) / sizeof(int);
  for (int i = 0; i < count; i++)
  {
    if (gFontSizes[i] >= ptSize)
    {
      SetCurSel(i);
      return i;
    }
  }
  return -1;
}

int KFontComboBox::SetSelectFontHeight(int ftHeight)
{
  int ptSize = 0;
  if (ftHeight < 0)
  {
    ptSize = CharacterHeightToPointSize(ftHeight);
  }
  return SetSelectFontSize(ptSize);
}

LPTSTR KFontComboBox::GetSelectedFaceName()
{
  LPTSTR facename = NULL;
  int index = GetCurSel();
  if (index >= 0)
  {
    CString str;
    GetLBText(index, str);
    facename = AllocString(str);
  }
  return facename;
}

int KFontComboBox::GetSelectedFontSize()
{
  int index = GetCurSel();
  if (index >= 0)
    return GetItemFontSize(index);
  return -1;
}

// character height( < 0)
int KFontComboBox::GetSelectedFontHeight()
{
  int fontSize = GetSelectedFontSize();
  if (fontSize > 0)
  {
    fontSize = PointSizeToCharacterHeight(fontSize);
  }
  else
    fontSize = 0;
  return fontSize;
}

int KFontComboBox::GetItemFontSize(int index)
{
  int count = sizeof(gFontSizes) / sizeof(int);
  if ((index >= 0) && (index < count))
  {
    return gFontSizes[index];
  }
  return gFontSizes[0];
}