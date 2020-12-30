// Copyright (C) 2019 K2Mobile
// All rights reserved.

// VivaImagingView.h: CVivaImagingView 클래스의 인터페이스
//

#pragma once

#include "Document\KImageDrawer.h"

#define USE_POINTER_EVENT

typedef struct tagWmPointerInfo
{
  int x;
  int y;
  int type;
  int pointerId;
  int pressure;
}WmPointerInfo;

class CVivaImagingView : public CView
{
protected: // serialization에서만 만들어집니다.
	CVivaImagingView();
	DECLARE_DYNCREATE(CVivaImagingView)

// 특성입니다.
public:
	CVivaImagingDoc* GetDocument() const;

// 작업입니다.
public:

// 재정의입니다.
public:
	virtual void OnDraw(CDC* pDC);  // 이 뷰를 그리기 위해 재정의되었습니다.
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
  virtual void OnPrint(CDC *pDC, CPrintInfo *pInfo);

// 구현입니다.
public:
	virtual ~CVivaImagingView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

  void DoSaveToServer(ImagingSaveFormat saveFormat);

  void DoFileSaveAs(LPCTSTR serverFolder, LPCTSTR name, ImagingSaveFormat format, UINT flag);

  void DoFileOpenServerFolder();

  void DoFileOpenServerFile();

  BOOL SetActivePageIndex(int pageIndex);

  void UpdateThumbView(KIntVector& pageIdList, int flag);

  void UpdateThumbView(BOOL updateAll, int index, int flag);

  void OnActivePageChange(int flag);
  void OnActivePageChanged(int flag);
  void OnPageSizeChanged(UINT flag);

  int InsertNewImagePages(LPCTSTR filenames);
  void ClearImageMode(BOOL bUpdate, PropertyMode nextMode);
  void SetImageMode(PropertyMode mode);
  inline PropertyMode GetImageMode() { return mMode; }

  void UpdatePreview(ImageEffectJob* effect);

  void FillSelectedArea(ImageEffectJob* effect);
  void CopySelectedArea(ImageEffectJob* effect);
  int InsertImageDropFiles(HDROP hdrop);
  void DoEditPaste();

  void PasteClipboardData(HANDLE b);
  void PasteClipboardImage(HANDLE h);
  void ShapeEditCommand(int channel, int* params, void* extra);

  void ApplyEffect(ImageEffectJob* effect);
  void CloseProperty(ImageEffectJob* effect);
  void EndFloatingImagePaste(BOOL bDone);

  int PropertyChange(LPCTSTR key, LPCTSTR str);

  void StartPreviewImageRender(KImagePage* p, ImageEffectJob* effect);

  void OpenAndUploadLocalFile(LPCTSTR fileName, LPCTSTR folderOID);
  void OpenLocalFolder(LPCTSTR pathName);
  void OpenServerFolder(LPCTSTR serverFolderOID);
  void OpenServerFile(LPCTSTR serverFileOID);
  void OnStartUpload(LPCTSTR folderOidIndex, LPCTSTR filename, ImagingSaveFormat saveFormat, UINT flag);

  void OnTaskEnd(KBgTask* task);

  KVoidPtrArray mBgTasks;
  void StopAllTask();
  void AddTask(KBgTask* task);
  void RemoveTask(KBgTask* task);
  void ClearSameTask(BgTaskType type, KImagePage* targetPage);

  KShapeBase* mpActiveShape;
  CEdit* mpEditWnd;
  BOOL StartTextEditing(KShapeBase* a);

  KImageBase* mpActivePageBackup;
  KImageDrawer mBackupDrawer;


protected:

  BOOL ZoomScreen(int dir, LPPOINT base = NULL);
  void UpdateActionResult(int r);
  void CheckScrollOffset();

  BOOL ScrollPage(int dx, int dy);
  void CheckAutoScrolling(CRect& client, POINT& point, BOOL bScroll);

  PropertyMode mMode;
  PropertyMode mToggleEditMode;
  // IMAGE_DRAW_OPT mDrawOption;
  KImageDrawer mDrawer;
  MouseState m_MouseState;
  POINT m_MousePoint;
  UINT_PTR m_MouseScrollTimer;

  ImageEffectJob* mpPreviewEffect;

  int mToolCursor;

#ifdef USE_POINTER_EVENT
  WmPointerInfo m_LastPointer;
  POINT m_GesturePoint;

  void InitTouchHandle();
  BOOL AddTouchHandle(UINT pointerId);
  BOOL RemoveTouchHandle(UINT pointerId);
  BOOL IsSingleTouch(UINT pointerId);
#endif

  void OnEditLButtonDown(UINT nFlags, CPoint point, int pressure);
  void OnEditMouseMove(UINT nFlags, CPoint point, int pressure);
  void OnEditMouseMoves(UINT nFlags, LP_CPOINT pts, int count);
  void OnEditLButtonUp(UINT nFlags, CPoint point, int pressure);
  void OnEditLButtonCancel();

  public:
    afx_msg void OnFileServerSetDocType();
    afx_msg void OnUpdateFileServerSetDocType(CCmdUI* pCmdUI);

// 생성된 메시지 맵 함수
protected:
  afx_msg void OnMouseMove(UINT nFlags, CPoint point);
  afx_msg void OnMouseLeave();
  afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
  afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
  afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
  afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
  afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
  afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
  afx_msg BOOL OnSetCursor(CWnd* pWnd,UINT nHitTest, UINT message);
  afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
  afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
  afx_msg void OnTimer(UINT_PTR timerEvent);
  afx_msg void OnSize(UINT nType, int cx, int cy);
  afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
  afx_msg void OnDropFiles(HDROP hDropInfo);
  afx_msg void OnUpdateFileNew(CCmdUI* pCmdUI);
  afx_msg void OnFileSave();
  afx_msg void OnUpdateFileSave(CCmdUI* pCmdUI);
  afx_msg void OnFileSaveAs();
  afx_msg void OnUpdateFileSaveAs(CCmdUI* pCmdUI);
  afx_msg void OnFilePrintPreview();
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
  afx_msg void OnEditUndo();
  afx_msg void OnUpdateEditUndo(CCmdUI* pCmdUI);
  afx_msg void OnEditRedo();
  afx_msg void OnUpdateEditRedo(CCmdUI* pCmdUI);
  afx_msg void OnEditAlignLeft();
  afx_msg void OnUpdateEditAlignLeft(CCmdUI* pCmdUI);
  afx_msg void OnEditAlignCenter();
  afx_msg void OnUpdateEditAlignCenter(CCmdUI* pCmdUI);
  afx_msg void OnEditAlignRight();
  afx_msg void OnUpdateEditAlignRight(CCmdUI* pCmdUI);
  afx_msg void OnEditAlignTop();
  afx_msg void OnUpdateEditAlignTop(CCmdUI* pCmdUI);
  afx_msg void OnEditAlignVCenter();
  afx_msg void OnUpdateEditAlignVCenter(CCmdUI* pCmdUI);
  afx_msg void OnEditAlignBottom();
  afx_msg void OnUpdateEditAlignBottom(CCmdUI* pCmdUI);
  afx_msg void OnEditOrderTop();
  afx_msg void OnUpdateEditOrderTop(CCmdUI* pCmdUI);
  afx_msg void OnEditOrderBottom();
  afx_msg void OnUpdateEditOrderBottom(CCmdUI* pCmdUI);
  afx_msg void OnEditOrderForward();
  afx_msg void OnUpdateEditOrderForward(CCmdUI* pCmdUI);
  afx_msg void OnEditOrderBackward();
  afx_msg void OnUpdateEditOrderBackward(CCmdUI* pCmdUI);

  afx_msg void OnInsertFolder();
  afx_msg void OnUpdateInsertFolder(CCmdUI* pCmdUI);
  afx_msg void OnInsertScanImage();
  afx_msg void OnUpdateInsertScanImage(CCmdUI* pCmdUI);
  afx_msg void OnInsertFileImage();
  afx_msg void OnUpdateInsertFileImage(CCmdUI* pCmdUI);
  afx_msg void OnMoveUpImage();
  afx_msg void OnUpdateMoveUpImage(CCmdUI* pCmdUI);
  afx_msg void OnMoveDownImage();
  afx_msg void OnUpdateMoveDownImage(CCmdUI* pCmdUI);
  afx_msg void OnMenuPageUp();
  afx_msg void OnUpdateMenuPageUp(CCmdUI* pCmdUI);
  afx_msg void OnMenuPageDown();
  afx_msg void OnUpdateMenuPageDown(CCmdUI* pCmdUI);
  afx_msg void OnViewZoom();
  afx_msg void OnUpdateViewZoom(CCmdUI* pCmdUI);
  afx_msg void OnViewZoomIn();
  afx_msg void OnUpdateViewZoomIn(CCmdUI* pCmdUI);
  afx_msg void OnViewZoomOut();
  afx_msg void OnUpdateViewZoomOut(CCmdUI* pCmdUI);
  afx_msg void OnViewReal();
  afx_msg void OnUpdateViewReal(CCmdUI* pCmdUI);
  afx_msg void OnViewFitTo();
  afx_msg void OnUpdateViewFitTo(CCmdUI* pCmdUI);
  afx_msg void OnViewZoom400();
  afx_msg void OnUpdateViewZoom400(CCmdUI* pCmdUI);
  afx_msg void OnViewZoom200();
  afx_msg void OnUpdateViewZoom200(CCmdUI* pCmdUI);
  afx_msg void OnViewZoom100();
  afx_msg void OnUpdateViewZoom100(CCmdUI* pCmdUI);
  afx_msg void OnViewZoom75();
  afx_msg void OnUpdateViewZoom75(CCmdUI* pCmdUI);
  afx_msg void OnViewZoom50();
  afx_msg void OnUpdateViewZoom50(CCmdUI* pCmdUI);
  afx_msg void OnViewZoom30();
  afx_msg void OnUpdateViewZoom30(CCmdUI* pCmdUI);
  afx_msg void OnImageRotate();
  afx_msg void OnUpdateImageRotate(CCmdUI* pCmdUI);
  afx_msg void OnImageResize();
  afx_msg void OnUpdateImageResize(CCmdUI* pCmdUI);
  afx_msg void OnImageCrop();
  afx_msg void OnUpdateImageCrop(CCmdUI* pCmdUI);
  afx_msg void OnImageMonochrome();
  afx_msg void OnUpdateImageMonochrome(CCmdUI* pCmdUI);
  afx_msg void OnImageColorBalance();
  afx_msg void OnUpdateImageColorBalance(CCmdUI* pCmdUI);
  afx_msg void OnImageColorLevel();
  afx_msg void OnUpdateImageColorLevel(CCmdUI* pCmdUI);
  afx_msg void OnImageColorHLS();
  afx_msg void OnUpdateImageColorHLS(CCmdUI* pCmdUI);
  afx_msg void OnImageColorHSV();
  afx_msg void OnUpdateImageColorHSV(CCmdUI* pCmdUI);
  afx_msg void OnImageColorBright();
  afx_msg void OnUpdateImageColorBright(CCmdUI* pCmdUI);
  afx_msg void OnImageColorReverse();
  afx_msg void OnUpdateImageColorReverse(CCmdUI* pCmdUI);
  afx_msg void OnImageSelectRect();
  afx_msg void OnUpdateImageSelectRect(CCmdUI* pCmdUI);
  afx_msg void OnImageSelectTriangle();
  afx_msg void OnUpdateImageSelectTriangle(CCmdUI* pCmdUI);
  afx_msg void OnImageSelectRhombus();
  afx_msg void OnUpdateImageSelectRhombus(CCmdUI* pCmdUI);
  afx_msg void OnImageSelectPentagon();
  afx_msg void OnUpdateImageSelectPentagon(CCmdUI* pCmdUI);
  afx_msg void OnImageSelectStar();
  afx_msg void OnUpdateImageSelectStar(CCmdUI* pCmdUI);
  afx_msg void OnImageSelectEllipse();
  afx_msg void OnUpdateImageSelectEllipse(CCmdUI* pCmdUI);
  afx_msg void OnImageSelectFill();
  afx_msg void OnUpdateImageSelectFill(CCmdUI* pCmdUI);
  afx_msg void OnImageAutoNaming();
  afx_msg void OnUpdateImageAutoNaming(CCmdUI* pCmdUI);
  afx_msg void OnImageAnnotateSelect();
  afx_msg void OnUpdateImageAnnotateSelect(CCmdUI* pCmdUI);
  afx_msg void OnImageAnnotateLine();
  afx_msg void OnUpdateImageAnnotateLine(CCmdUI* pCmdUI);
  afx_msg void OnImageAnnotateFreehand();
  afx_msg void OnUpdateImageAnnotateFreehand(CCmdUI* pCmdUI);
  afx_msg void OnImageAnnotateRectangle();
  afx_msg void OnUpdateImageAnnotateRectangle(CCmdUI* pCmdUI);
  afx_msg void OnImageAnnotateEllipse();
  afx_msg void OnUpdateImageAnnotateEllipse(CCmdUI* pCmdUI);
  afx_msg void OnImageAnnotatePolygon();
  afx_msg void OnUpdateImageAnnotatePolygon(CCmdUI* pCmdUI);
  afx_msg void OnImageAnnotateTextbox();
  afx_msg void OnUpdateImageAnnotateTextbox(CCmdUI* pCmdUI);
  afx_msg void OnImageAnnotateShapes();
  afx_msg void OnUpdateImageAnnotateShapes(CCmdUI* pCmdUI);

#ifdef USE_POINTER_EVENT
  afx_msg LRESULT OnWmPointerEnter(WPARAM wparam, LPARAM lparam);
  afx_msg LRESULT OnWmPointerLeave(WPARAM wparam, LPARAM lparam);
  afx_msg LRESULT OnWmPointerUp(WPARAM wparam, LPARAM lparam);
  afx_msg LRESULT OnWmPointerDown(WPARAM wparam, LPARAM lparam);
  afx_msg LRESULT OnWmPointerUpdate(WPARAM wparam, LPARAM lparam);
  afx_msg LRESULT OnWmGesture(WPARAM wparam, LPARAM lparam);
#endif

  afx_msg LRESULT OnScanImageDone(WPARAM wParam, LPARAM lParam);
  afx_msg LRESULT OnThreadCallResult(WPARAM wParam, LPARAM lParam);
  afx_msg LRESULT OnBgTaskMessage(WPARAM wParam, LPARAM lParam);
  DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // VivaImagingView.cpp의 디버그 버전
inline CVivaImagingDoc* CVivaImagingView::GetDocument() const
   { return reinterpret_cast<CVivaImagingDoc*>(m_pDocument); }
#endif

