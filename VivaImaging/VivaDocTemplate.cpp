// VivaDocTemplate.cpp: KVivaDocTemplate 클래스의 구현
// Copyright c 2019 K2Mobile


#include "stdafx.h"
//#include "MainFrm.h"
#include "Resource.h"
#include "VivaImaging.h"
#include "VivaDocTemplate.h"

KVivaDocTemplate::KVivaDocTemplate(UINT nIDResource, CRuntimeClass* pDocClass,
  CRuntimeClass* pFrameClass, CRuntimeClass* pViewClass)
  : CSingleDocTemplate(nIDResource, pDocClass,pFrameClass, pViewClass)
{

}

BOOL KVivaDocTemplate::GetDocString(CString& rString, enum CDocTemplate::DocStringIndex i) // get one of the info strings
{
  CString strTemp, strLeft, strRight;
  int nFindPos;
  AfxExtractSubString(strTemp, m_strDocStrings, (int)i);

  if (i == CDocTemplate::filterExt) {
    nFindPos = strTemp.Find(';');
    if (-1 != nFindPos) {
      //string contains two extensions
      strLeft = strTemp.Left(nFindPos + 1);
      strRight = _T("*") + strTemp.Right(lstrlen((LPCTSTR)strTemp) - nFindPos - 1);
      strTemp = strLeft + strRight;
    }
  }
  rString = strTemp;
  return TRUE;
}

CDocTemplate::Confidence KVivaDocTemplate::MatchDocType(LPCTSTR pszPathName, CDocument*& rpDocMatch)
{
  //ASSERT(pszPathName != NULL);
  rpDocMatch = NULL;

  // go through all documents
  POSITION pos = GetFirstDocPosition();
  while (pos != NULL)
  {
    CDocument* pDoc = GetNextDoc(pos);
    if (pDoc->GetPathName() == pszPathName) {
      // already open
      rpDocMatch = pDoc;
      return yesAlreadyOpen;
    }
  }  // end while

  // see if it matches either suffix
  CString strFilterExt;
  if (GetDocString(strFilterExt, CDocTemplate::filterExt) &&
    !strFilterExt.IsEmpty())
  {
    // see if extension matches
    ASSERT(strFilterExt[0] == '.');
    CString ext1, ext2;
    int nDot = CString(pszPathName).ReverseFind('.');
    LPCTSTR pszDot = (nDot < 0) ? NULL : pszPathName + nDot;

    int nSemi = strFilterExt.Find(';');
    if (-1 != nSemi) {
      // string contains two extensions
      ext1 = strFilterExt.Left(nSemi);
      ext2 = strFilterExt.Mid(nSemi + 2);
      // check for a match against either extension
      if (nDot >= 0 && (lstrcmpi(pszPathName + nDot, ext1) == 0
        || lstrcmpi(pszPathName + nDot, ext2) == 0))
        return yesAttemptNative; // extension matches
    }
    else
    { // string contains a single extension
      if (nDot >= 0 && (lstrcmpi(pszPathName + nDot,
        strFilterExt) == 0))
        return yesAttemptNative;  // extension matches
    }
  }
  return yesAttemptForeign;
}

