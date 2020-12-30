#pragma once

#define IMAGE_INPUT_LEVEL 1

#define NOL_CHANGED (WM_USER+260)

// KImageOutputLevelCtrl

class KImageOutputLevelCtrl : public CWnd
{
	DECLARE_DYNAMIC(KImageOutputLevelCtrl)

public:
	KImageOutputLevelCtrl();
	virtual ~KImageOutputLevelCtrl();

  BOOL RegisterWndClass();

  BOOL Create(DWORD windowStyle, RECT& rect, CWnd* parent, int controlId, int mode);

  void drawTrackHandle(CDC& dc, int xp, int yp);
  void drawImageHistogram(CDC& dc, RECT& rect);

  int mMode;
  BYTE mLevel[3];
  BYTE* mpHistogramData;
  CEdit mEditLevel[3];
  WORD mCtrlId;

  int mTrackDrag;
  int mTrackDragXOffset;

  inline LPBYTE GetHistogramBuffer() { return mpHistogramData; }

  void SetLevelValue(int m, BYTE value, BOOL updateEdit);
  void SetCurrentValue(BYTE v1, BYTE v2, BYTE v3);
  void GetCurrentValue(BYTE *v1, BYTE* v2, BYTE* v3);

  virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);


protected:
	DECLARE_MESSAGE_MAP()
public:
  afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
  afx_msg void OnPaint();
  afx_msg void OnSize(UINT nType, int cx, int cy);
  afx_msg void OnMouseMove(UINT nFlags, CPoint point);
  afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
  afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
};


