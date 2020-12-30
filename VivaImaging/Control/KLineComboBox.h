#pragma once

// KLineComboBox
#include "..\Platform\KColor.h"
#include "..\Platform\KBitmap.h"


#define K_LINE_STYLE 1
#define K_LINE_STYLE_WITH_NULL 2
#define K_LINE_WIDTH 3
#define K_LINE_START_ARROW 4
#define K_LINE_END_ARROW 5

class KLineComboBox : public CComboBox
{
	DECLARE_DYNAMIC(KLineComboBox)

public:
	KLineComboBox();
	virtual ~KLineComboBox();

  BOOL Create(DWORD windowStyle, RECT& rect, CWnd* parent, int controlId, int style);

  virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
  virtual int CompareItem(LPCOMPAREITEMSTRUCT lpCompareItemStruct);
  virtual void MeasureItem(LPMEASUREITEMSTRUCT lpmis);

  int GetLineStyle();
  int GetLineWidth();

  static int GetLineWidth(int index);
  static int GetLineStyle(int index);
  static int FromLineStyle(int style);
  static int FromLineWidth(int width);

  int mStyle;

protected:
	DECLARE_MESSAGE_MAP()
public:

};


