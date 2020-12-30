#pragma once

#include "../Platform/KBitmap.h"
#include "../VivaImagingDoc.h"

// KThumbListView

typedef struct tagPageDragTarget
{
  int pageIndex;
  KImageBase* targetPage;
  InsertTargetType inside; // 0 : before, 1 : inside, 2 : next
} PageDragTarget;

#define TIMER_MOUSE_SCROLL 1002
#define TIMER_THUMB_REFRESH 1004

class KThumbListView : public CWnd
{
	DECLARE_DYNAMIC(KThumbListView)

public:
	KThumbListView();
	virtual ~KThumbListView();

  static int m_MarginX;
  static int m_MarginY;
  static int m_MarginInnerX;
  static int m_MarginInnerY;
  static int m_MarginDepth;
  static int m_folder_height;

  static void ClearStatic();

  BOOL RegisterWndClass();
  BOOL Create(DWORD windowStyle, RECT& rect, CWnd* parent, int controlId);
  
  void drawFolderIconImage(CDC& dc, RECT& d, KImageBase* p, BOOL hasFocus, BOOL isActive);
  void checkDraggingScroll(int dy, int bottom);
  void checkForNeedUpdateThumb(BOOL bRender);
  void getPageRect(int p, BOOL asInsert, RECT& rect);
  void invalidatePage(PageDragTarget& target);
  void invalidatePage(int p, BOOL asInsert);
  int pageFromPoint(POINT& point);
  PageDragTarget pageInsertFromPoint(POINT& point);

  CVivaImagingDoc* m_pDocument;
  int m_ScrollOffset;
  int m_ScrollMax;
  int m_bScrollbarActive;
  int m_bScrollbarDragOffset;
  int m_thumb_width;

  static KBitmap* m_pScrollUpIcon;
  static KBitmap* m_pScrollDownIcon;
  static KBitmap* m_pCheckIcon;
  static KBitmap* m_pFoldingNormIcon;
  static KBitmap* m_pFoldingFocusIcon;
  static KBitmap* m_pFolderIcon;

  MouseState m_MouseState;
  UINT_PTR m_MouseScrollTimer;
  POINT m_MousePoint;

  int m_LastButtonDownIndex;
  int m_LastClickSelect;
  PageDragTarget m_MovePageTarget;
  int m_foldingOverIndex;
  int m_foldingDownIndex;

  UINT_PTR m_RefreshTimer;

  int calcThumbHeight(KImageBase* p);

  void RecalcScrollRange();
  void RecalcScrollRange(int cx, int cy);
  BOOL GetActiveScrollbarRect(RECT& client, RECT& scrollbar);

  void ScrollPage(int n);
  void GetScrollIconRect(RECT& rect, BOOL onUp);

  BOOL IsFolderFoldFocused(int index);
  BOOL GetFoldingIconRect(int index, RECT& rect);
  int IsOverFoldingIcon(int index, POINT& point);

  void UpdateThumbView(KIntVector& pageIdList, int flag);
  void UpdateThumbView(BOOL updateAll, int index, int flag);

  void OnActivePageChanged(int flag);
  BOOL IsVisiblePage(int page_index);
  void ScrollToVisiblePage(int page_index);

  BOOL MoveSelectedPages(PageDragTarget& target);

protected:
	DECLARE_MESSAGE_MAP()
public:
  afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
  afx_msg void OnPaint();
  afx_msg void OnSize(UINT nType, int cx, int cy);
  afx_msg void OnMouseMove(UINT nFlags, CPoint point);
  afx_msg void OnMouseLeave();
  afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
  afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
  afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
  afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
  afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
  afx_msg void OnTimer(UINT_PTR timerEvent);
  afx_msg void OnDropFiles(HDROP hDropInfo);
  afx_msg void OnEditSelectAll();
  afx_msg void OnUpdateEditSelectAll(CCmdUI* pCmdUI);
  afx_msg void OnEditCut();
  afx_msg void OnUpdateEditCut(CCmdUI* pCmdUI);
  afx_msg void OnEditCopy();
  afx_msg void OnUpdateEditCopy(CCmdUI* pCmdUI);
  afx_msg void OnEditPaste();
  afx_msg void OnUpdateEditPaste(CCmdUI* pCmdUI);
  afx_msg void OnEditDelete();
  afx_msg void OnUpdateEditDelete(CCmdUI* pCmdUI);
};


