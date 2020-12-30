#include "stdafx.h"
#include "KButtonPropItem.h"
#include "../MainFrm.h"

KButtonPropItem::KButtonPropItem(const CString& strName, const COleVariant& varValue, LPCTSTR lpszDescr, DWORD_PTR dwData,
  LPCTSTR lpszEditMask, LPCTSTR lpszEditTemplate, LPCTSTR lpszValidChars)
  : CMFCPropertyGridProperty(strName, varValue, lpszDescr, dwData, lpszEditMask, lpszEditTemplate, lpszValidChars)
{

}

KButtonPropItem::~KButtonPropItem()
{

}

BOOL KButtonPropItem::HasButton() const
{
  return TRUE;
}

void KButtonPropItem::OnClickButton(CPoint point)
{
  CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
  pFrame->OnChangeDocType();
}