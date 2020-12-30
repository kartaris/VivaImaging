// KPropertyGridProperty.cpp: 구현 파일
//

#include "stdafx.h"
#include "KPropertyGridProperty.h"


// KPropertyGridProperty

KPropertyGridProperty::KPropertyGridProperty(HWND hWnd, UINT id, const CString& strName,
  const _variant_t& varValue,
  LPCTSTR lpszDescr,
  DWORD_PTR dwData)
: CMFCPropertyGridProperty(strName, varValue, lpszDescr, dwData, NULL, NULL, NULL)
{
  mParent = hWnd;
  mID = id;
}

KPropertyGridProperty::~KPropertyGridProperty()
{
}


// KPropertyGridProperty 메시지 처리기

BOOL KPropertyGridProperty::OnEndEdit()
{
  if (mParent != NULL)
    PostMessage(mParent, WMSG_PROPERTY_CHANGED, mID, 0);
  return 0;
}
