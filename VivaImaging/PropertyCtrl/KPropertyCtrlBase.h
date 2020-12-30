#pragma once

#include "..\Document\KImageProperty.h"
#include "..\Document\KImageBase.h"
#include "..\Document\KImageDrawer.h"
#include "..\Document\Shape\KShapeBase.h"
#include "..\VivaImagingDoc.h"

#define PC_X_MARGIN 8
#define PC_Y_MARGIN 6
#define PC_ITEM_HEIGHT 16
#define PC_ITEM_WIDTH 160
#define PC_BUTTON_HEIGHT 18

#define PC_APPLY_HEIGHT 20
#define PC_APPLY_WIDTH 100 // apply button width

// Mouse Hit Test
#define HT_CROP_TOP 0x01
#define HT_CROP_BOTTOM 0x02
#define HT_CROP_LEFT 0x010
#define HT_CROP_RIGHT 0x020

// KPropertyCtrlBase

class KPropertyCtrlBase : public CWnd
{
	DECLARE_DYNAMIC(KPropertyCtrlBase)

public:
	KPropertyCtrlBase();
	virtual ~KPropertyCtrlBase();

  BOOL RegisterWndClass();

  static BOOL IsSharedPropertyMode(PropertyMode currMode, PropertyMode newMode);

  static KPropertyCtrlBase* CreateControl(PropertyMode mode, CWnd* parent, RECT& rect);
  static void DrawPropertyCtrlPreview(CDC* pDC, KImageDrawer& info, ImageEffectJob* effect);

  static int UseMouseAction(PropertyMode mode);
  static int MouseHitTest(POINT& dpoint, KImageDrawer& info, ImageEffectJob* effect);
  static int MouseDragging(MouseState state, POINT& point, KImageDrawer& info, ImageEffectJob* effect);

  BOOL Create(DWORD windowStyle, RECT& rect, CWnd* parent, int controlId);

  void UpdatePreview(ImageEffectJob* effect);
  void ApplyProperty(ImageEffectJob* effect);
  void CloseProperty(ImageEffectJob* effect);
  void FreeImageEffectJob(ImageEffectJob* effect);

  virtual PropertyMode GetCtrlMode() { return PropertyMode::PModeMax; }
  virtual BOOL OnPropertyValueChanged(MouseState state, ImageEffectJob* effect) { return FALSE; }

  virtual void InitProperty(CVivaImagingDoc* doc) { ; }
  virtual void OnCloseProperty() { ; }
  virtual void UpdateShapeStyle(void *shape) { ; }

  virtual BOOL GetLineStyle(OutlineStyle& linestyle, BYTE& startArrow, BYTE& endArrow) { return FALSE; }
  virtual BOOL GetShapeStyle(DWORD& fillcolor, OutlineStyle& linestyle) { return FALSE; }
  virtual BOOL GetFillColor(DWORD& fillcolor) { return FALSE; }

  /*

  static DWORD gDefaultFillColor;
  static OutlineStyle gDefaultLineStyle;
  static int gDefaultStartArrow;
  static int gDefaultEndArrow;
  static DWORD gDefaultTextColor;
  static CString gDefaultFacename;
  static int gDefaultFontSize;
  static int gDefaultFontAttribute;

  static DWORD GetDefaultFill();
  static void SetDefaultFill(DWORD fill);
  static void GetDefaultLineStyle(OutlineStyle& outline);
  static void SetDefaultLineStyle(OutlineStyle& outline);

  static void SetDefaultLineColor(DWORD color);
  static void SetDefaultLineStyle(int style);
  static void SetDefaultLineWidth(int width);

  static int GetDefaultStartArrow();
  static int GetDefaultEndArrow();
  static void SetDefaultStartArrow(int a);
  static void SetDefaultEndArrow(int a);

  static LPCTSTR GetDefaultFacename();
  static void SetDefaultFacename(LPCTSTR name);
  static int GetDefaultFontSize();
  static void SetDefaultFontSize(int pt);
  static DWORD GetDefaultTextColor();
  static void SetDefaultTextColor(DWORD fill);
  static int ToggleDefaultFontStyle(int attr);

  static void GetDefaultFontStyle(LPTSTR facename, int& pointSize, DWORD& attr);
  */

  static PropertyGroupMode GetPropertyGroupMode(PropertyMode mode);

  int mMinHeight;
  int mMaxScrollRange;
  int mScrollPos;


  void UpdateScrollInfo();
  int GetVScrollPos(UINT nSBCode, UINT nPos);
  int OnVerticalScroll(UINT nSBCode, UINT nPos);

  void adjustControlYPosition(CWnd* ctrl, int dy, int height);
  int RearrangeControlYPosition(UINT* arrange_control_ids, int count, int dy, int height);

protected:
	DECLARE_MESSAGE_MAP()
};


