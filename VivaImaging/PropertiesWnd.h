// Copyright (C) 2019 K2Mobile
// All rights reserved.

#pragma once

#include "Control\KPropertyGridProperty.h"
#include "PropertyCtrl\KPropertyCtrlBase.h"
#include "PropertyCtrl/KButtonPropItem.h"

class CPropertiesToolBar : public CMFCToolBar
{
public:
	virtual void OnUpdateCmdUI(CFrameWnd* /*pTarget*/, BOOL bDisableIfNoHndler)
	{
		CMFCToolBar::OnUpdateCmdUI((CFrameWnd*) GetOwner(), bDisableIfNoHndler);
	}

	virtual BOOL AllowShowOnList() const { return FALSE; }
};


#define PROPERTY_HEIGHT 100
#define PROPERTY_WIDTH 101
#define PROPERTY_COLORDEPTH 102
#define PROPERTY_NAME 103
#define PROPERTY_DOCTYPE_BTN 105

#define PROPERTY_META_DATAID  200
#define PROPERTY_META_USERID  201
#define PROPERTY_META_CREATED_DATE 202
#define PROPERTY_META_MODIFIED_DATE 203
#define PROPERTY_META_UPLOADED_DATE 204
#define PROPERTY_META_DEVICE_NAME 205
#define PROPERTY_META_COMPUTER_NAME 206

#define PROPERTY_META_CUSTOM1 301
#define PROPERTY_META_CUSTOM2 302
#define PROPERTY_META_CUSTOM3 303
#define PROPERTY_META_CUSTOM4 304

class CPropertiesWnd : public CDockablePane
{
// 생성입니다.
public:
	CPropertiesWnd();

  void SetPropertyMode(PropertyMode mode);
  inline PropertyMode GetPropertyMode() { return mMode; }
  void InitProperty(CVivaImagingDoc* doc);

  void ReadyComobString();
  void AdjustLayout();

  inline KPropertyCtrlBase* GetPropertyCtrl() { return mPropertyCtrl; }

// 특성입니다.
public:
	void SetVSDotNetLook(BOOL bSet)
	{
		m_wndPropList.SetVSDotNetLook(bSet);
		m_wndPropList.SetGroupNameFullWidth(bSet);
	}

  void UpdatePageMetadata(CVivaImagingDoc* doc);

  virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

  BOOL OnPropertyValueChanged(PropertyMode mode, MouseState state, ImageEffectJob* effect);

  CMFCPropertyGridCtrl* GetPropertyGridCtrl() {
    return &m_wndPropList;
  }

protected:
	CFont m_fntPropList;

  PropertyMode mMode;
  KPropertyCtrlBase* mPropertyCtrl;

	CComboBox m_wndObjectCombo;
	CPropertiesToolBar m_wndToolBar;
	CMFCPropertyGridCtrl m_wndPropList;

  CMFCPropertyGridProperty* mpHeightProp;
  CMFCPropertyGridProperty* mpWidthProp;
  CMFCPropertyGridProperty* mpColorProp;
  CMFCPropertyGridProperty* mpNameProp;
  CMFCPropertyGridProperty* mpDocTypeNameProp;

  CMFCPropertyGridProperty* mpMetaDataInfoProp;
  CMFCPropertyGridProperty* mpExtDocTypeInfoProp;

  TCHAR mLastDocTypeOid[MAX_OID];

  LPCTSTR GetPropertyMetaDataKey(int idx);
  LPTSTR translateMetaKeyName(LPCTSTR key);
  LPTSTR makeMetaKeyHelp(LPCTSTR key);

// 구현입니다.
public:
	virtual ~CPropertiesWnd();

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
  afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnExpandAllProperties();
	afx_msg void OnUpdateExpandAllProperties(CCmdUI* pCmdUI);
	afx_msg void OnSortProperties();
	afx_msg void OnUpdateSortProperties(CCmdUI* pCmdUI);
	afx_msg void OnProperties1();
	afx_msg void OnUpdateProperties1(CCmdUI* pCmdUI);
	afx_msg void OnProperties2();
	afx_msg void OnUpdateProperties2(CCmdUI* pCmdUI);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
  afx_msg LRESULT OnPropertyChanged(WPARAM wParam, LPARAM lParam);
  afx_msg LRESULT OnShapeEditNotify(WPARAM wparam, LPARAM lparam);

	DECLARE_MESSAGE_MAP()

	void InitPropList();
	void SetPropListFont();

	int m_nComboHeight;
  KVoidPtrArray mExtDocTypeInfoArray;
  int mExtDocTypeInfoCount;
  CMFCPropertyGridProperty** mpExtDocTypeInfoPropArray;

  KVoidPtrArray mMetaDataPropArray;
  int mMetaDataPropArrayCount;
  CMFCPropertyGridProperty** mpMetaDataPropArray;
};

