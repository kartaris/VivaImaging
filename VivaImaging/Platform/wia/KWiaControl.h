#pragma once

#include <Wia.h>
#include "..\Utils.h"
#include "..\twain\KTwainControl.h"

#define WM_KWIA_MSG (WM_USER + 410)

class KWiaControl
{
public:
  KWiaControl();
  ~KWiaControl();

  BOOL Init(HWND hWnd);
  void CheckReadyManager();
  void ReleaseManager();

  void GetDataSourceList(KImageCaptureDeviceVector& vector);

  int ScanImage(HWND hWnd, BSTR deviceIDS, LPTSTR workingPath);


  IWiaDevMgr2* m_WiaDevMgr;
  HWND mParentWnd;

};

