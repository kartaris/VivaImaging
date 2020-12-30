// Copyright (C) 2019 K2Mobile
// All rights reserved.

#include "stdafx.h"

#include "PropertiesWnd.h"
#include "Resource.h"
#include "MainFrm.h"
#include "VivaImaging.h"
#include "Core\KConstant.h"
#include "Document\KShapeEditor.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// CResourceViewBar

CPropertiesWnd::CPropertiesWnd()
{
  mMode = PropertyMode::PModeImageView;
  mPropertyCtrl = NULL;

	m_nComboHeight = 0;
  mpHeightProp = NULL;
  mpWidthProp = NULL;
  mpColorProp = NULL;
  mpNameProp = NULL;
  mpDocTypeNameProp = NULL;

  mpMetaDataInfoProp = NULL;
  mpMetaDataPropArray = NULL;
  mMetaDataPropArrayCount = 0;

  mpExtDocTypeInfoProp = NULL;
  mExtDocTypeInfoCount = 0;
  mpExtDocTypeInfoPropArray = NULL;

  mLastDocTypeOid[0] = '\0';
}

CPropertiesWnd::~CPropertiesWnd()
{
  if (mpMetaDataPropArray != NULL)
  {
    delete[] mpMetaDataPropArray;
    mpMetaDataPropArray = NULL;
  }
  if (mpExtDocTypeInfoPropArray != NULL)
  {
    delete[] mpExtDocTypeInfoPropArray;
    mpExtDocTypeInfoPropArray = NULL;
  }
  if (mPropertyCtrl != NULL)
  {
    delete mPropertyCtrl;
    mPropertyCtrl = NULL;
  }
  ClearKeyNameValueArray(mMetaDataPropArray);
  ClearKeyNameValueArray(mExtDocTypeInfoArray);
}

BEGIN_MESSAGE_MAP(CPropertiesWnd, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_COMMAND(ID_EXPAND_ALL, OnExpandAllProperties)
	ON_UPDATE_COMMAND_UI(ID_EXPAND_ALL, OnUpdateExpandAllProperties)
	ON_COMMAND(ID_SORTPROPERTIES, OnSortProperties)
	ON_UPDATE_COMMAND_UI(ID_SORTPROPERTIES, OnUpdateSortProperties)
	ON_COMMAND(ID_PROPERTIES1, OnProperties1)
	ON_UPDATE_COMMAND_UI(ID_PROPERTIES1, OnUpdateProperties1)
	ON_COMMAND(ID_PROPERTIES2, OnProperties2)
	ON_UPDATE_COMMAND_UI(ID_PROPERTIES2, OnUpdateProperties2)
	ON_WM_SETFOCUS()
	ON_WM_SETTINGCHANGE()
  ON_REGISTERED_MESSAGE(AFX_WM_PROPERTY_CHANGED, OnPropertyChanged)
  ON_MESSAGE(WM_SHAPEEDIT_NOTIFY, OnShapeEditNotify)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CResourceViewBar 메시지 처리기

void CPropertiesWnd::SetPropertyMode(PropertyMode mode)
{
  if (mMode != mode)
  {
    // shared property mode 
    if (!KPropertyCtrlBase::IsSharedPropertyMode(mMode, mode))
    {
      if (mMode == PropertyMode::PModeImageView)
      {
        m_wndToolBar.ShowWindow(SW_HIDE);
        m_wndPropList.ShowWindow(SW_HIDE);
      }
      else
      {
        if (mPropertyCtrl != NULL)
        {
          mPropertyCtrl->OnCloseProperty();
          delete mPropertyCtrl;
          mPropertyCtrl = NULL;
        }
      }

      if (mode == PropertyMode::PModeImageView)
      {
        m_wndToolBar.ShowWindow(SW_SHOW);
        m_wndPropList.ShowWindow(SW_SHOW);
      }
      else //if (mode != PModeImageView)
      {
        if (mPropertyCtrl != NULL)
        {
          mPropertyCtrl->OnCloseProperty();
          delete mPropertyCtrl;
        }

        CRect rectClient;
        GetClientRect(rectClient);
        rectClient.top += m_nComboHeight + 1;
        mPropertyCtrl = KPropertyCtrlBase::CreateControl(mode, this, rectClient);
        if (mPropertyCtrl != NULL)
          mPropertyCtrl->ShowWindow(SW_SHOW);
      }
    }
    mMode = mode;
    ReadyComobString();
    AdjustLayout();
  }
}

void CPropertiesWnd::InitProperty(CVivaImagingDoc* doc)
{
  if (mPropertyCtrl != NULL)
  {
    mPropertyCtrl->InitProperty(doc);
  }
  else
  {
    UpdatePageMetadata(doc);
  }
}

void CPropertiesWnd::ReadyComobString()
{
  m_wndObjectCombo.ResetContent();

  CString str;
  int sel = 0;

  str.LoadString(AfxGetInstanceHandle(), IDS_PROPERTYWND_VIEWMODE, mResourceLanguage);
  m_wndObjectCombo.AddString(str);
  if (mMode > PropertyMode::PModeImageView)
  {
    str.LoadString(AfxGetInstanceHandle(), IDS_PROPERTYWND_VIEWMODE + (int)mMode, mResourceLanguage);
    m_wndObjectCombo.AddString(str);
    sel = 1;
  }
  m_wndObjectCombo.SetCurSel(sel);
}

void CPropertiesWnd::AdjustLayout()
{
	if (GetSafeHwnd () == nullptr || (AfxGetMainWnd() != nullptr && AfxGetMainWnd()->IsIconic()))
	{
		return;
	}

	CRect rectClient;
	GetClientRect(rectClient);

  if (mMode == PropertyMode::PModeImageView)
  {
    int cyTlb = m_wndToolBar.CalcFixedLayout(FALSE, TRUE).cy;

    m_wndObjectCombo.SetWindowPos(nullptr, rectClient.left, rectClient.top, rectClient.Width(), m_nComboHeight, SWP_NOACTIVATE | SWP_NOZORDER);
    m_wndToolBar.SetWindowPos(nullptr, rectClient.left, rectClient.top + m_nComboHeight, rectClient.Width(), cyTlb, SWP_NOACTIVATE | SWP_NOZORDER);
    m_wndPropList.SetWindowPos(nullptr, rectClient.left, rectClient.top + m_nComboHeight + cyTlb, rectClient.Width(), rectClient.Height() - (m_nComboHeight + cyTlb), SWP_NOACTIVATE | SWP_NOZORDER);
  }
  else
  {
    m_wndObjectCombo.SetWindowPos(nullptr, rectClient.left, rectClient.top, rectClient.Width(), m_nComboHeight, SWP_NOACTIVATE | SWP_NOZORDER);
    rectClient.top += m_nComboHeight + 1;
    if (mPropertyCtrl != NULL)
      mPropertyCtrl->SetWindowPos(nullptr, rectClient.left, rectClient.top, rectClient.Width(), rectClient.Height(), SWP_NOACTIVATE | SWP_NOZORDER);
  }
}

int CPropertiesWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	// 콤보 상자를 만듭니다.
	const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_BORDER | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

	if (!m_wndObjectCombo.Create(dwViewStyle, rectDummy, this, 201))
	{
		TRACE0("속성 콤보 상자를 만들지 못했습니다. \n");
		return -1;      // 만들지 못했습니다.
	}

  ReadyComobString();

	CRect rectCombo;
	m_wndObjectCombo.GetWindowRect (&rectCombo);

	m_nComboHeight = rectCombo.Height();

	if (!m_wndPropList.Create(WS_VISIBLE | WS_CHILD, rectDummy, this, 202))
	{
		TRACE0("속성 표를 만들지 못했습니다. \n");
		return -1;      // 만들지 못했습니다.
	}

	InitPropList();

	m_wndToolBar.Create(this, AFX_DEFAULT_TOOLBAR_STYLE, IDR_PROPERTIES);
	m_wndToolBar.LoadToolBar(IDR_PROPERTIES, 0, 0, TRUE /* 잠금 */);
	m_wndToolBar.CleanUpLockedImages();
	m_wndToolBar.LoadBitmap(theApp.m_bHiColorIcons ? IDB_PROPERTIES_HC : IDR_PROPERTIES, 0, 0, TRUE /* 잠금 */);

	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() | CBRS_TOOLTIPS | CBRS_FLYBY);
	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() & ~(CBRS_GRIPPER | CBRS_SIZE_DYNAMIC | CBRS_BORDER_TOP | CBRS_BORDER_BOTTOM | CBRS_BORDER_LEFT | CBRS_BORDER_RIGHT));
	m_wndToolBar.SetOwner(this);

	// 모든 명령은 부모 프레임이 아닌 이 컨트롤을 통해 라우팅됩니다.
	m_wndToolBar.SetRouteCommandsViaFrame(FALSE);

	AdjustLayout();

  // Register to receive notification from ShapeEditor
  KShapeEditor::RegisterReceiveEvent(GetSafeHwnd());

	return 0;
}

LRESULT CPropertiesWnd::OnShapeEditNotify(WPARAM wparam, LPARAM lparam)
{
  if (wparam == SHAPE_EVENT_SELECTED)
  {
    KPropertyCtrlBase* prop = GetPropertyCtrl();
    if (prop != NULL)
    {
      prop->UpdateShapeStyle((void *)lparam);
    }
  }
  return 0;
}

void CPropertiesWnd::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

  CRect rectCombo;
  m_wndObjectCombo.GetWindowRect(&rectCombo);

  m_nComboHeight = rectCombo.Height();

	AdjustLayout();
}

void CPropertiesWnd::OnExpandAllProperties()
{
	m_wndPropList.ExpandAll();
}

void CPropertiesWnd::OnUpdateExpandAllProperties(CCmdUI* /* pCmdUI */)
{
}

void CPropertiesWnd::OnSortProperties()
{
	m_wndPropList.SetAlphabeticMode(!m_wndPropList.IsAlphabeticMode());
}

void CPropertiesWnd::OnUpdateSortProperties(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_wndPropList.IsAlphabeticMode());
}

void CPropertiesWnd::OnProperties1()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
}

void CPropertiesWnd::OnUpdateProperties1(CCmdUI* /*pCmdUI*/)
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
}

void CPropertiesWnd::OnProperties2()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
}

void CPropertiesWnd::OnUpdateProperties2(CCmdUI* /*pCmdUI*/)
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
}

BOOL CPropertiesWnd::OnCommand(WPARAM wParam, LPARAM lParam)
{
  WORD notify_code = HIWORD(wParam);
  WORD ctrl_id = LOWORD(wParam);
  if ((notify_code == CBN_SELCHANGE) && (ctrl_id == 201))
  {
    int sel = m_wndObjectCombo.GetCurSel();
    if ((sel == 0) && (mMode > PropertyMode::PModeImageView))
    {
      // change to view mode
      CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
      if (pFrame)
        pFrame->SetViewImageMode(PropertyMode::PModeImageView);
      return 1;
    }
  }
  return CWnd::OnCommand(wParam, lParam);
}

LPTSTR CPropertiesWnd::translateMetaKeyName(LPCTSTR key)
{
  // check if pre-defined key
  LPCTSTR pKeys = gDefaultMetaKeysPredefIfd;
  int index = 0;

  while ((pKeys != NULL) && (*pKeys != '\0'))
  {
    LPCTSTR e = StrChr(pKeys, DA_VALUE_SEPARATOR);
    size_t key_len = 0;

    if (e != NULL)
      key_len = e - pKeys;
    else
      key_len = lstrlen(pKeys);
    if ((key_len == lstrlen(key)) && (StrCmpN(pKeys, key, key_len) == 0))
    {
      CString keystr;
      keystr.LoadString(AfxGetInstanceHandle(), IDS_METAKEY1 + index, mResourceLanguage);
      return AllocString(keystr);
    }
    index++;
    if (e != NULL)
      pKeys = e + 1;
    else
      pKeys = NULL;
  }

  // predef key가 아니면 그대로 사용한다.
  return NULL;
}

LPTSTR CPropertiesWnd::makeMetaKeyHelp(LPCTSTR key)
{
  CString str;
  //str.LoadString(IDS_METAKEY_HELP_TAIL);
  str.LoadString(AfxGetInstanceHandle(), IDS_METAKEY_HELP_TAIL, mResourceLanguage);

  int len = lstrlen(key) + str.GetLength() + 2;
  LPTSTR buff = new TCHAR[len];

  if (mResourceLanguage == 949)
  {
    StringCchCopy(buff, len, key);
    StringCchCat(buff, len, _T(" "));
    StringCchCat(buff, len, str);
  }
  else
  {
    StringCchCopy(buff, len, str);
    StringCchCat(buff, len, _T(" "));
    StringCchCat(buff, len, key);
  }
  return buff;
}

void CPropertiesWnd::UpdatePageMetadata(CVivaImagingDoc* doc)
{
  KImageBase* p = doc->GetImagePage(EXCLUDE_FOLDED, -1);

  BOOL isImagePage = (p != NULL) && (p->GetType() == IMAGE_PAGE);
  if (isImagePage)
  {
    KBitmap* b = ((KImagePage*)p)->GetImage(doc->GetWorkingPath());
    if (b != NULL)
    {
      if (mpHeightProp != NULL)
      {
        mpHeightProp->SetValue(_variant_t((long)b->height, VT_I4));
      }
      if (mpWidthProp != NULL)
      {
        mpWidthProp->SetValue(_variant_t((long)b->width, VT_I4));
      }
      if (mpColorProp != NULL)
      {
        /*
        static LPTSTR colorSpaceName[] = {
          _T("흑백"),
          _T("256 컬러"),
          _T("24비트 컬러"),
          _T("32비트 컬러") };
        */
        CString name;


        int n = 0;
        if (b->bitCount == 1)
          n = 0;
        else if (b->bitCount == 4)
          n = 1;
        else if (b->bitCount == 8)
          n = 2;
        else if (b->bitCount == 24)
          n = 3;
        else if (b->bitCount == 32)
          n = 4;
        name.LoadString(AfxGetInstanceHandle(), IDS_PROPERTY_COLORSPACE1+n, mResourceLanguage);
        // mpColorProp->SetValue(_variant_t(colorSpaceName[n]));
        mpColorProp->SetValue(name);
      }

      // Current docType
      if (mpDocTypeNameProp != NULL)
      {
        LPCTSTR docTypeName = doc->GetDocTypeName();
        mpDocTypeNameProp->SetValue((docTypeName != NULL) ? docTypeName : _T("None"));
        mpDocTypeNameProp->Enable(doc->IsEditable());
      }
    }
  }

  if (mpHeightProp != NULL)
    mpHeightProp->Show(isImagePage);
  if (mpWidthProp != NULL)
    mpWidthProp->Show(isImagePage);
  if (mpColorProp != NULL)
    mpColorProp->Show(isImagePage);

  if (mpNameProp != NULL)
  {
    mpNameProp->SetValue(_variant_t((p != NULL) ? p->GetName() : _T("")));
    mpNameProp->Enable(doc->IsEditable());
  }

  // MetaData
  ClearKeyNameValueArray(mMetaDataPropArray);

  for (int i = 0; i < mMetaDataPropArrayCount; i++)
    mpMetaDataInfoProp->RemoveSubItem(mpMetaDataPropArray[i]);
  if (mpMetaDataPropArray != NULL)
    delete[] mpMetaDataPropArray;
  mMetaDataPropArrayCount = 0;
  mpMetaDataPropArray = NULL;

  if (p != NULL)
  {
    p->GetMetaDataSet(mMetaDataPropArray, NULL, META_SKIP_EMPTY);
    mMetaDataPropArrayCount = mMetaDataPropArray.size();
    if (mMetaDataPropArrayCount > 0)
    {
      mpMetaDataPropArray = new CMFCPropertyGridProperty*[mMetaDataPropArrayCount];

      KVoidPtrIterator it = mMetaDataPropArray.begin();
      int i = 0;
      while (it != mMetaDataPropArray.end())
      {
        MetaKeyNameValue* p = (MetaKeyNameValue*)*(it++);

        if (p->name == NULL)
          p->name = translateMetaKeyName(p->key);

        LPTSTR name = (p->name != NULL) ? p->name : p->key;
        LPTSTR helpstr = makeMetaKeyHelp(name);

        mpMetaDataPropArray[i] = new CMFCPropertyGridProperty(name, p->value, helpstr, PROPERTY_META_DATAID + i);
        mpMetaDataPropArray[i]->Enable(FALSE);
        mpMetaDataInfoProp->AddSubItem(mpMetaDataPropArray[i]);
        i++;

        if (helpstr != NULL)
          delete[] helpstr;
      }
    }
    else
    {
      mpMetaDataPropArray = NULL;
    }
  }

  // ExtDocTypeInfo
  if (mpDocTypeNameProp != NULL)
    mpDocTypeNameProp->Show(isImagePage);

  if (mpExtDocTypeInfoProp != NULL)
  {
    ClearKeyNameValueArray(mExtDocTypeInfoArray);

    // 키-이름-값 정보를 mExtDocTypeInfoArray에 작성
    if (p != NULL)
      p->GetExtDocTypeInfoSet(mExtDocTypeInfoArray, 0, doc->mDocTypeInfo);

    // 문서유형이 바뀌었는지 체크
    BOOL changed = FALSE;
    LPCTSTR docTypeOid = doc->GetDocTypeOid();
    if ((lstrlen(mLastDocTypeOid) == 0) ||
      (lstrcmp(mLastDocTypeOid, docTypeOid) != 0))
      changed = TRUE;
    if ((mpExtDocTypeInfoPropArray != NULL) &&
      (mExtDocTypeInfoCount != (int)mExtDocTypeInfoArray.size()))
      changed = TRUE;

    if (changed)
    {
      for (int i = 0; i < mExtDocTypeInfoCount; i++)
        mpExtDocTypeInfoProp->RemoveSubItem(mpExtDocTypeInfoPropArray[i]);

      delete[] mpExtDocTypeInfoPropArray;
      mpExtDocTypeInfoPropArray = NULL;
      mExtDocTypeInfoCount = 0;

      mpExtDocTypeInfoProp->Expand(FALSE);
    }

    if (mpExtDocTypeInfoPropArray == NULL)
    {
      int i = 0;

      size_t propCount = mExtDocTypeInfoArray.size();
      if (propCount > 0)
      {
        if (mpExtDocTypeInfoPropArray == NULL)
          mpExtDocTypeInfoPropArray = new CMFCPropertyGridProperty*[propCount];
        memset(mpExtDocTypeInfoPropArray, 0, sizeof(UINT_PTR)* propCount);

        KVoidPtrIterator it = mExtDocTypeInfoArray.begin();
        while (it != mExtDocTypeInfoArray.end())
        {
          MetaKeyNameValue* p = (MetaKeyNameValue*)*it;

          if (p->name == NULL)
            p->name = translateMetaKeyName(p->key);

          LPTSTR name = (p->name != NULL) ? p->name : p->key;
          LPTSTR helpstr = makeMetaKeyHelp(name);

          mpExtDocTypeInfoPropArray[i] = new CMFCPropertyGridProperty(name, p->value, helpstr, PROPERTY_META_CUSTOM1 + i);

          if (helpstr != NULL)
            delete[] helpstr;

          ++i;
          ++it;
        }
        mExtDocTypeInfoCount = propCount;
      }

      for (int i = 0; i < mExtDocTypeInfoCount; i++)
        mpExtDocTypeInfoProp->AddSubItem(mpExtDocTypeInfoPropArray[i]);

      if (!mpExtDocTypeInfoProp->IsExpanded())
        mpExtDocTypeInfoProp->Expand();
    }
    else if (mExtDocTypeInfoCount == (int)mExtDocTypeInfoArray.size())
    {
      int i = 0;
      KVoidPtrIterator it = mExtDocTypeInfoArray.begin();
      while (it != mExtDocTypeInfoArray.end())
      {
        MetaKeyNameValue* p = (MetaKeyNameValue*)*it;

        mpExtDocTypeInfoPropArray[i]->SetValue(p->value);
        mpExtDocTypeInfoPropArray[i]->Show(TRUE);
        ++i;
        ++it;
      }
    }
    else if ((mExtDocTypeInfoArray.size() == 0) && (mExtDocTypeInfoCount > 0))
    {
      for (int i = 0; i < mExtDocTypeInfoCount; i++)
        mpExtDocTypeInfoPropArray[i]->Show(FALSE);
    }

    StringCchCopy(mLastDocTypeOid, MAX_OID, docTypeOid);

    mpExtDocTypeInfoProp->Redraw();
  }
}

LPCTSTR CPropertiesWnd::GetPropertyMetaDataKey(int idx)
{
  if ((idx >= 0) && (idx < (int)mExtDocTypeInfoArray.size()))
  {
    KVoidPtrIterator it = mExtDocTypeInfoArray.begin();
    if ((idx > 0) && (idx < (int)mExtDocTypeInfoArray.size()))
      it += idx;
    if (it != mExtDocTypeInfoArray.end())
    {
      MetaKeyNameValue* p = (MetaKeyNameValue*)*it;
      return p->key;
    }
  }
  return NULL;
}

void CPropertiesWnd::InitPropList()
{
	SetPropListFont();

	m_wndPropList.EnableHeaderCtrl(FALSE);
	m_wndPropList.EnableDescriptionArea();
	m_wndPropList.SetVSDotNetLook();
	m_wndPropList.MarkModifiedProperties();

  CString name;
  CString tooltipstr;

  name.LoadString(AfxGetInstanceHandle(), IDS_PROPERTY_INFO, mResourceLanguage);
  mpMetaDataInfoProp = new CMFCPropertyGridProperty(name);

  name.LoadString(AfxGetInstanceHandle(), IDS_PROPERTY_HEIGHT, mResourceLanguage);
  tooltipstr.LoadString(AfxGetInstanceHandle(), IDS_PROPERTY_HEIGHT_TOOLTIP, mResourceLanguage);
  mpHeightProp = new CMFCPropertyGridProperty(name, (_variant_t)250l, tooltipstr, 100);
  mpHeightProp->EnableSpinControl(FALSE, 0, 20000);
  mpHeightProp->Enable(FALSE);
  mpMetaDataInfoProp->AddSubItem(mpHeightProp);

  name.LoadString(AfxGetInstanceHandle(), IDS_PROPERTY_WIDTH, mResourceLanguage);
  tooltipstr.LoadString(AfxGetInstanceHandle(), IDS_PROPERTY_WIDTH_TOOLTIP, mResourceLanguage);
  mpWidthProp = new CMFCPropertyGridProperty(name, (_variant_t)250l, tooltipstr, 101);
  mpWidthProp->EnableSpinControl(FALSE, 0, 20000);
  mpWidthProp->Enable(FALSE);
  mpMetaDataInfoProp->AddSubItem(mpWidthProp);

  name.LoadString(AfxGetInstanceHandle(), IDS_PROPERTY_COLOR, mResourceLanguage);
  tooltipstr.LoadString(AfxGetInstanceHandle(), IDS_PROPERTY_COLOR_TOOLTIP, mResourceLanguage);
  mpColorProp = new CMFCPropertyGridProperty(name, _T(""), tooltipstr, 102);
  mpColorProp->Enable(FALSE);
  mpMetaDataInfoProp->AddSubItem(mpColorProp);

  name.LoadString(AfxGetInstanceHandle(), IDS_PROPERTY_NAME, mResourceLanguage);
  tooltipstr.LoadString(AfxGetInstanceHandle(), IDS_PROPERTY_NAME_TOOLTIP, mResourceLanguage);
  mpNameProp = new CMFCPropertyGridProperty(name, _T(""), tooltipstr, PROPERTY_NAME);
  mpMetaDataInfoProp->AddSubItem(mpNameProp);

  m_wndPropList.AddProperty(mpMetaDataInfoProp);

  name.LoadString(AfxGetInstanceHandle(), IDS_PROPERTY_METADATA, mResourceLanguage);
  mpExtDocTypeInfoProp = new CMFCPropertyGridProperty(name);

  name.LoadString(AfxGetInstanceHandle(), IDS_PROPERTY_DOCTYPE, mResourceLanguage);
  tooltipstr.LoadString(AfxGetInstanceHandle(), IDS_PROPERTY_DOCTYPE_TOOLTIP, mResourceLanguage);
  mpDocTypeNameProp = new KButtonPropItem(name, _T("None"), tooltipstr, PROPERTY_DOCTYPE_BTN);
  mpExtDocTypeInfoProp->AddSubItem(mpDocTypeNameProp);

  m_wndPropList.AddProperty(mpExtDocTypeInfoProp);

#if 0
	CMFCPropertyGridProperty* pGroup1 = new CMFCPropertyGridProperty(_T("모양"));

	pGroup1->AddSubItem(new CMFCPropertyGridProperty(_T("3D 모양"), (_variant_t) false, _T("창에 굵은 글꼴이 아닌 글꼴을 지정하고, 컨트롤에 3D 테두리를 지정합니다.")));

	CMFCPropertyGridProperty* pProp = new CMFCPropertyGridProperty(_T("테두리"), _T("Dialog Frame"), _T("None, Thin, Resizable 또는 Dialog Frame 중 하나를 지정합니다."));
	pProp->AddOption(_T("None"));
	pProp->AddOption(_T("Thin"));
	pProp->AddOption(_T("Resizable"));
	pProp->AddOption(_T("Dialog Frame"));
	pProp->AllowEdit(FALSE);

	pGroup1->AddSubItem(pProp);
	pGroup1->AddSubItem(new CMFCPropertyGridProperty(_T("캡션"), (_variant_t) _T("정보"), _T("창의 제목 표시줄에 표시되는 텍스트를 지정합니다.")));

	m_wndPropList.AddProperty(pGroup1);

	CMFCPropertyGridProperty* pSize = new CMFCPropertyGridProperty(_T("창 크기"), 0, TRUE);

	pProp = new CMFCPropertyGridProperty(_T("높이"), (_variant_t) 250l, _T("창의 높이를 지정합니다."));
	pProp->EnableSpinControl(TRUE, 50, 300);
	pSize->AddSubItem(pProp);

	pProp = new CMFCPropertyGridProperty( _T("너비"), (_variant_t) 150l, _T("창의 너비를 지정합니다."));
	pProp->EnableSpinControl(TRUE, 50, 200);
	pSize->AddSubItem(pProp);

	m_wndPropList.AddProperty(pSize);

	CMFCPropertyGridProperty* pGroup2 = new CMFCPropertyGridProperty(_T("글꼴"));

	LOGFONT lf;
	CFont* font = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	font->GetLogFont(&lf);

	_tcscpy_s(lf.lfFaceName, _T("맑은 고딕"));

	pGroup2->AddSubItem(new CMFCPropertyGridFontProperty(_T("글꼴"), lf, CF_EFFECTS | CF_SCREENFONTS, _T("창의 기본 글꼴을 지정합니다.")));
	pGroup2->AddSubItem(new CMFCPropertyGridProperty(_T("시스템 글꼴을 사용합니다."), (_variant_t) true, _T("창에서 MS Shell Dlg 글꼴을 사용하도록 지정합니다.")));

	m_wndPropList.AddProperty(pGroup2);

	CMFCPropertyGridProperty* pGroup3 = new CMFCPropertyGridProperty(_T("기타"));
	pProp = new CMFCPropertyGridProperty(_T("(이름)"), _T("응용 프로그램"));
	pProp->Enable(FALSE);
	pGroup3->AddSubItem(pProp);

	CMFCPropertyGridColorProperty* pColorProp = new CMFCPropertyGridColorProperty(_T("창 색상"), RGB(210, 192, 254), nullptr, _T("창의 기본 색상을 지정합니다."));
	pColorProp->EnableOtherButton(_T("기타..."));
	pColorProp->EnableAutomaticButton(_T("기본값"), ::GetSysColor(COLOR_3DFACE));
	pGroup3->AddSubItem(pColorProp);

	static const TCHAR szFilter[] = _T("아이콘 파일(*.ico)|*.ico|모든 파일(*.*)|*.*||");
	pGroup3->AddSubItem(new CMFCPropertyGridFileProperty(_T("아이콘"), TRUE, _T(""), _T("ico"), 0, szFilter, _T("창 아이콘을 지정합니다.")));

	pGroup3->AddSubItem(new CMFCPropertyGridFileProperty(_T("폴더"), _T("c:\\")));

	m_wndPropList.AddProperty(pGroup3);

	CMFCPropertyGridProperty* pGroup4 = new CMFCPropertyGridProperty(_T("계층"));

	CMFCPropertyGridProperty* pGroup41 = new CMFCPropertyGridProperty(_T("첫번째 하위 수준"));
	pGroup4->AddSubItem(pGroup41);

	CMFCPropertyGridProperty* pGroup411 = new CMFCPropertyGridProperty(_T("두 번째 하위 수준"));
	pGroup41->AddSubItem(pGroup411);

	pGroup411->AddSubItem(new CMFCPropertyGridProperty(_T("항목 1"), (_variant_t) _T("값 1"), _T("설명입니다.")));
	pGroup411->AddSubItem(new CMFCPropertyGridProperty(_T("항목 2"), (_variant_t) _T("값 2"), _T("설명입니다.")));
	pGroup411->AddSubItem(new CMFCPropertyGridProperty(_T("항목 3"), (_variant_t) _T("값 3"), _T("설명입니다.")));

	pGroup4->Expand(FALSE);
	m_wndPropList.AddProperty(pGroup4);
#endif
}

void CPropertiesWnd::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);
	m_wndPropList.SetFocus();
}

void CPropertiesWnd::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
{
	CDockablePane::OnSettingChange(uFlags, lpszSection);
	SetPropListFont();
}

void CPropertiesWnd::SetPropListFont()
{
	::DeleteObject(m_fntPropList.Detach());

	LOGFONT lf;
	afxGlobalData.fontRegular.GetLogFont(&lf);

	NONCLIENTMETRICS info;
	info.cbSize = sizeof(info);

	afxGlobalData.GetNonClientMetrics(info);

	lf.lfHeight = info.lfMenuFont.lfHeight;
	lf.lfWeight = info.lfMenuFont.lfWeight;
	lf.lfItalic = info.lfMenuFont.lfItalic;

	m_fntPropList.CreateFontIndirect(&lf);

	m_wndPropList.SetFont(&m_fntPropList);
	m_wndObjectCombo.SetFont(&m_fntPropList);
}

LRESULT CPropertiesWnd::OnPropertyChanged(WPARAM wParam, LPARAM lParam)
{
  CMFCPropertyGridProperty *pProp = (CMFCPropertyGridProperty*)lParam;
  if (pProp != NULL)
  {
    if (pProp->GetData() == PROPERTY_NAME)
    {
      _variant_t val = mpNameProp->GetValue();
      mpNameProp->SetOriginalValue(val);
      mpNameProp->ResetOriginalValue();

      // mpNameProp->SetModified
      CString str = val.bstrVal;
      CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
      pFrame->OnPropertyChanged(_T("Name"), str.GetBuffer());
    }
    else if (pProp->GetData() >= PROPERTY_META_CUSTOM1)
    {
      int idx = (int)(pProp->GetData() - PROPERTY_META_CUSTOM1);
      LPCTSTR key = GetPropertyMetaDataKey(idx);
      if (key != NULL)
      {
        _variant_t val = mpExtDocTypeInfoPropArray[idx]->GetValue();
        CString str = val.bstrVal;
        CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
        pFrame->OnPropertyChanged(key, str.GetBuffer());
      }
      else
      {
        TCHAR msg[256];
        StringCchPrintf(msg, 256, _T("Cannot found Property Key for item-id=%d"), (int)pProp->GetData());
      }
    }
  }
  return 0;
}

BOOL CPropertiesWnd::OnPropertyValueChanged(PropertyMode mode, MouseState state, ImageEffectJob* effect)
{
  if ((mPropertyCtrl != NULL) && (mPropertyCtrl->GetCtrlMode() == mode))
  {
    mPropertyCtrl->OnPropertyValueChanged(state, effect);
    return TRUE;
  }
  return FALSE;
}
