#pragma once
#include <afxpropertygridctrl.h>


class KButtonPropItem : public CMFCPropertyGridProperty
{
public:
  KButtonPropItem(const CString& strName, const COleVariant& varValue, LPCTSTR lpszDescr = NULL, DWORD_PTR dwData = 0,
    LPCTSTR lpszEditMask = NULL, LPCTSTR lpszEditTemplate = NULL, LPCTSTR lpszValidChars = NULL);

  virtual ~KButtonPropItem();

  virtual BOOL HasButton() const;

  virtual void OnClickButton(CPoint point);

};

