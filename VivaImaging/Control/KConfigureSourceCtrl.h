#pragma once

#include "..\Platform\twain\KTwainControl.h"
#include "..\Platform\Utils.h"

// KConfigureSourceCtrl

typedef struct tagSourceCapsChildItem
{
  HWND label;
  HWND ctrl;
  LPVOID subctrl;
  int ctrl_type;
}SourceCapsChildItem;

class KConfigureSourceCtrl : public CWnd
{
	DECLARE_DYNAMIC(KConfigureSourceCtrl)

public:
	KConfigureSourceCtrl();
	virtual ~KConfigureSourceCtrl();

  BOOL RegisterWndClass();
  void CreateSourceCapsChilds(KTwainControl* twainControl, UINT32 deviceID);
  inline void SetFont(CFont* pFont) { mFont = pFont; }


private:
  KTwainControl* mpTwainControl;
  UINT32 m_DeviceID;
  UINT32 mCapCount;
  CapSetting * m_pCapSettings;
  SourceCapsChildItem* m_ChildItems;
  CFont* mFont;
  int mScrollMax;
  int mScrollPos;
  int mPageSize;

  HWND createLabel(LPCTSTR name, int dy, int ctrlId, BOOL bReadOnly);
  HWND createControl(PCTSTR name, int dy, int ctrlId, BOOL bReadOnly, TW_CAPABILITY& Cap, KVoidPtrArray& arg,
    UINT selectedIndex, int right, SourceCapsChildItem& item);
  void SetScrollbar(int dialogHeight);

  int getCapIndexByHandle(HWND ctrlhWnd);
  BOOL ChangeDeviceCaps(CapSetting& setting, int selectedIndex);
  BOOL ChangeDeviceCaps(CapSetting& setting, LPCSTR pValue);

protected:
	DECLARE_MESSAGE_MAP()
public:
  afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
  afx_msg HBRUSH OnCtlColor(CDC*pDC, CWnd*pWnd, UINT nCtlColor);
  afx_msg void OnDraw(CDC* pDC);
  afx_msg void OnDestroy();
  virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
  virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
};


