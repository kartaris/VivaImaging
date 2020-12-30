/**
* @file Graphics.h
* @date 2015.02
* @brief Graphic specific functions header file
*/
#ifndef _DESTINY_GRAPHICS_SPECFIC_MODULE_H_
#define _DESTINY_GRAPHICS_SPECFIC_MODULE_H_

#define KWND_CENTER 1
#define KWND_PARENT_CENTER 2
#define KWND_TOPMOST 4

/**
* @brief 지정한 윈도우를 화면상에서 맨 위로 올리고 화면의 가운데로 이동한다.
* @param hwnd : 윈도우 핸들
* @param toCenter : 윈도우를 화면 가운데로 이동한다
*/
void MakeTopActive(HWND hwnd, int toCenter, HWND parent);
/**
* @brief 윈도우를 화면의 가운데로 이동하기 위한 위치를 계산한다.
* @param hCurrWnd : 부모 윈도우 핸들
* @param hWnd : 대상 윈도우 핸들
* @param rect : 윈도우를 화면 가운데로 이동한 위치
*/
BOOL GetCenterWindow(HWND hCurrWnd, HWND hWnd, LPRECT rect);
/**
* @brief 윈도우를 화면의 가운데로 이동하고 윈도우를 화면의 맨 위로 올린다.
* @param hCurrWnd : 부모 윈도우 핸들
* @param hWnd : 대상 윈도우 핸들
* @param toCenter : 플래그(KWND_CENTER, KWND_TOPMOST)
*/
BOOL MakeCenterWindow(HWND hCurrWnd, HWND hWnd, int toCenter = KWND_CENTER);

/**
* @brief 이미지를 지정한 영역의 가운데에 그린다.
* @param hDC : Device Context
* @param area : 화면 영역
* @param hbm : 이미지 핸들
* @param rop : raster operation
*/
void DrawImageCenter(HDC hDC, RECT &area, HBITMAP hbm, int rop);
/**
* @brief 이미지의 지정 프레임을 지정한 영역에 그린다. 이미지는 정사각형의 여러 프레임이 세로로 배열되어 있다.
* @param hDC : Device Context
* @param rect : 화면 영역
* @param hbm : 이미지 핸들
* @param frame : 출력할 프레임 인덱스
* @param total : 전체 프레임 개수
*/
void DrawImageFrame(HDC hDC, RECT& rect, HBITMAP hbm, int frame, int total);

#define USE_GDIPLUS_TRANSPARENT_IMAGE
#define USE_GDIPLUS_HBITMAP

#ifdef USE_GDIPLUS_TRANSPARENT_IMAGE
#include <gdiplus.h>
using namespace Gdiplus;

#ifdef USE_GDIPLUS_HBITMAP
/**
* @brief PNG 형식의 이미지 리소스를 윈도우 Bitmap 핸들로 로드하여 온다.
* @param resource_id : PNG 리소스 ID
*/
HBITMAP LoadTransparentImage(UINT resource_id);
/**
* @brief PNG 형식의 이미지를 지정한 영역에 그린다.
* @param hDC : Device Context
* @param rect : 화면 영역
* @param hbm : 이미지 핸들
* @param asGray : TRUE이면 Source Alpha값을 50%로 그린다.
*/
void DrawTransparentImage(HDC hDC, RECT rect, HBITMAP hbm, BOOL asGray = FALSE);
/**
* @brief PNG 형식의 이미지의 지정 프레임을 지정한 영역에 그린다. 이미지는 동일한 높이의 여러 프레임이 세로로 배열되어 있다.
* @param hDC : Device Context
* @param rect : 화면 영역
* @param hbm : 이미지 핸들
* @param frame : 그릴 프레임
* @param total : 전체 프레임
* @param asGray : TRUE이면 Source Alpha값을 50%로 그린다.
*/
void DrawTransparentImageFrame(HDC hDC, RECT rect, HBITMAP hbm, int frame, int total, BOOL asGray = FALSE);
/**
* @brief PNG 형식의 이미지의 지정 프레임을 지정한 영역에 그린다. 이미지는 동일한 높이의 여러 프레임이 가로로 배열되어 있다.
* @param hDC : Device Context
* @param rect : 화면 영역
* @param hbm : 이미지 핸들
* @param frame : 그릴 프레임
* @param total : 전체 프레임
* @param asGray : TRUE이면 Source Alpha값을 50%로 그린다.
*/
void DrawTransparentImageHorz(HDC hDC, RECT rect, HBITMAP hbm, int frame, int total, BOOL asGray = FALSE);
#else
Image *LoadTransparentImage(UINT resource_id);
void DrawTransparentImage(HDC hDC, RECT& sub, Image *image);
void DrawTransparentImageFrame(HDC hDC, RECT& sub, Image *image, int frame, int total);
#endif

#endif // USE_GDIPLUS_TRANSPARENT_IMAGE

/**
* @brief 툴팁 윈도우를 생성한다.
* @param hDlg : 부모 윈도우 핸들
* @param hItem : 컨트롤 윈도우 핸들
* @param pszText : 표시할 텍스트
* @param pRect : 툴팁 영역
*/
HWND CreateToolTipWnd(HWND hDlg, HWND hItem, LPCTSTR pszText, LPRECT pRect = NULL);


int CharacterHeightToPointSize(int charHeight);
int PointSizeToCharacterHeight(int pointSize);

/**
* @brief 다이얼로그 유닛값을 화면의 픽셀값으로 변환한다.
* @param d : 입출력 값.
*/
void DialogUnit2Pixel(POINT &d);
/**
* @brief 다이얼로그 유닛 y값을 화면의 픽셀값으로 변환하여 리턴한다.
* @param d : 입력 값.
*/
int DialogY2Pixel(int d);
/**
* @brief 다이얼로그 유닛 x값을 화면의 픽셀값으로 변환하여 리턴한다.
* @param d : 입력 값.
*/
int DialogX2Pixel(int d);

void DialogUnit2Pixel(HWND hWnd, POINT &d);

int DialogY2Pixel(HWND hWnd, int d);

int DialogX2Pixel(HWND hWnd, int d);

// return 0/1/2(16px, 24px, 32px) proper icon size
int GetHiDPIImageButtonSize(HWND hWnd);

/**
* @brief 대상 윈도우 핸들을 윈도우 타스크바에서 숨김 또는 보임 처리한다.
* @param hWnd : 대상 윈도우 핸들
* @param bShow : TRUE이면 보임, 아니면 숨김
*/
void ShowHideFromTaskBar(HWND hWnd, BOOL bShow);

#define FORMAT_LINE_START_ELLIPSE 1
#define FORMAT_LINE_END_ELLIPSE 2
#define FORMAT_LINE_ELLIPSE 3
/**
* @brief 문자열을 여러줄로 포매팅한다.
* @param dc : Device Context
* @param str : 원본 및 결과 스트링
* @param width : 포매팅할 폭
* @param max_line : 최대 줄 수. 이를 넘는 부분은 생략한다.
* @param flag : 플래그(FORMAT_LINE_START_ELLIPSE)
*/
int FormatLineString(CDC &dc, CString &str, int width, int max_line, int flag);

void DlgMoveToOffset(CWnd* dlg, int itemId, int xoffset, int yoffset, BOOL bRepaint);

void DlgMoveToOffset(CWnd* dlg, CWnd* child, int xoffset, int yoffset, BOOL bRepaint);

// ROW_REPAINT flag
#define REPAINT_BELOW 1
#define REFRESH_ROWCOUNT 2

/**
* @brief 다이얼로그의 컨트롤을 지정한 만큼 이동한다. 
* 다이얼로그의 크기를 리사이즈될때 이에 따라 컨트롤의 위치와 크기를 변경할 때 사용된다.
* @param dlg : 다이얼로그 윈도우 핸들
* @param xp : 다이얼로그의 right에서 컨트롤의 left까지의 값. 0보다 작으면 이동하지 않는다.
* @param yp : 다이얼로그의 bottom에서 컨트롤의 top까지의 값. 0보다 작으면 이동하지 않는다.
* @param xe : 다이얼로그의 right에서 컨트롤의 right까지의 값. 0보다 작으면 이동하지 않는다.
* @param ye : 다이얼로그의 bottom에서 컨트롤의 bottom까지의 값. 0보다 작으면 이동하지 않는다.
* @param client : 다이얼로그의 클라이언트 영역 좌표
* @param child : 컨트롤 핸들
* @param bRepaint : TRUE이면 Repaint를 한다.
*/
void DlgMoveToOffset(CWnd* dlg, int xp, int yp, int xe, int ye, RECT& client, CWnd* child, BOOL bRepaint);

#define DLGPOS_R_R 0x01
#define DLGPOS_R_L 0x02
#define DLGPOS_B_B 0x10
#define DLGPOS_B_T 0x20

/**
* @brief 다이얼로그의 컨트롤의 마진값을 저장한다. 
* 다이얼로그의 크기를 리사이즈될때 이에 따라 컨트롤의 위치와 크기를 변경할 때 사용된다.
* @param dlg : 다이얼로그 윈도우 핸들
* @param child : 컨트롤 핸들
* @param client : 다이얼로그의 클라이언트 영역 좌표
* @param pt : 마진값을 저장할 버퍼
* @param dir : 마진값의 종류
* @details DLGPOS_R_R : 컨트롤의 오른쪽에서 클라이언트 영역 오른쪽까지.
* @n DLGPOS_R_L : 컨트롤의 왼쪽에서 클라이언트 영역 오른쪽까지.
* @n DLGPOS_B_B : 컨트롤의 아래에서 클라이언트 영역 아래까지.
* @n DLGPOS_B_T : 컨트롤의 위에서 클라이언트 영역 아래까지.
*/
void DlgStoreButtonPos(CWnd* dlg, CWnd* child, RECT& client, POINT& pt, int dir);

/**
* @brief 지정한 영역이 현재 디스플레이 영역에 일부분이라도 보이는 좌표인지 확인한다. 
* 충돌처리창 등에서 이전의 다이얼로그 좌표값이 유효한지 확인하여 각 모니터 영역과 조금(60픽셀)이라도 겹치면
* 유효한 것으로 판단하고, 아니면 화면의 가운데 영역으로 이동한다.
* @param rect : 화면 좌표.
* @details 시스템의 각 모니터의 영역 좌표를 받아서 이 부분이 rect 영역과 60픽셀 이상 겹치면 유효한 것으로 판단한다.
*/
BOOL IsValidWindowPosition(RECT &rect);


/**
* @brief 화면의 DPI값을 리턴한다. 
*/
int GetScreenDpi(HWND hWnd = NULL);

extern CFont* gpUiFont;
extern int gDlgFontBaseWidth;
extern int gDlgFontBaseHeight;
extern TCHAR gDefaultFacename[LF_FACESIZE];

#define NO_LINE_WIDTH 8
#define NO_LINE_STYLE 5
#define NO_LINE_ARROW_STYLE 4


void DrawLineArrow(CDC& dc, int cx, int cy, int width, int arrow_dir, int style, COLORREF color);

void SetDlgItemFont(CWnd* dlg, int* dlg_ctrl_ids, int count, CFont* pFont);

#endif