// VivaDocTemplate.h: KVivaDocTemplate 헤더
// Copyright (C) 2019 K2Mobile
// All rights reserved.

#pragma once

#include "afxwinappex.h"

class KVivaDocTemplate : public CSingleDocTemplate
{
public:
  KVivaDocTemplate(UINT nIDResource, CRuntimeClass* pDocClass,
    CRuntimeClass* pFrameClass, CRuntimeClass* pViewClass);

  virtual BOOL GetDocString(CString& rString,
    enum CDocTemplate::DocStringIndex index); // get one of the info strings

  virtual CDocTemplate::Confidence MatchDocType(LPCTSTR lpszPathName,
    CDocument*& rpDocMatch);

};

