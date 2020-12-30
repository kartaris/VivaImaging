#pragma once

// KFontComboBox

#define FONT_COMBO_FACENAME 1
#define FONT_COMBO_FONTSIZE 2

class KFontComboBox : public CComboBox
{
	DECLARE_DYNAMIC(KFontComboBox)

public:
	KFontComboBox();
	virtual ~KFontComboBox();

  BOOL Create(DWORD windowStyle, RECT& rect, CWnd* parent, int controlId);

  int InitItems(int type);

  int AddFontFamily(const LOGFONT* lf);

  int SetSelectFaceName(LPCTSTR faceName);
  int SetSelectFontSize(int ptSize);
  int SetSelectFontHeight(int ftHeight);

  LPTSTR GetSelectedFaceName();
  int GetSelectedFontSize();
  int GetSelectedFontHeight();

  int GetItemFontSize(int index);

protected:
	DECLARE_MESSAGE_MAP()
public:

};


