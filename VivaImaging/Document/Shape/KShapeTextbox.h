/**
* @file KShapeTextbox.h
* @date 2017.05
* @brief KShape for Textbox class file
*/
    /**
    * @class Rectangle
    * @brief Rectangle 개체 클래스
    */
#include "stdafx.h"
#include "KShapeBase.h"

class KShapeTextbox : public KShapeBase
{
public:
  KShapeTextbox();
  KShapeTextbox(RECT rect);
  KShapeTextbox(RECT rect, DWORD textcolor, LPCTSTR facename, int pointSize, DWORD attribute);

  ~KShapeTextbox();

  virtual ShapeType GetShapeType() { return ShapeType::ShapeTextbox; }
  virtual ShapeType GetResizeType() { return ShapeType::ShapeRectangle; }

  virtual BOOL Move(POINT& offset);
  virtual EditHandleType HitObjectsHandle(KImageDrawer& drawer, POINT& pt);
  virtual EditHandleType HitOver(KImageDrawer& drawer, POINT& pt);

  virtual BOOL HitOver(POINT& pt);

  virtual void GetRectangle(RECT& rect);
  virtual void GetNormalRectangle(RECT& rect);
  virtual RECT GetBounds(KImageDrawer* drawer);

  virtual void Draw(Graphics& g, KImageDrawer& info, int flag);

  void drawShape(Graphics& g, KImageDrawer& info, RECT& r, int flag);

  virtual BOOL SetRectangle(RECT& rect);

  virtual void OnRenderRubber(Graphics& g, KDragAction* drag, KImageDrawer& info, int flag);
  virtual void OnDragEnd(KDragAction* drag, POINT& amount);
  virtual void GetDragBounds(RECT& bound, KDragAction* drag);

  virtual BOOL GetTextColor(DWORD& color);
  virtual BOOL SetTextColor(DWORD& color);
  virtual BOOL GetFont(LPTSTR faceName, int& pointSize, DWORD& attribute);
  virtual BOOL SetFont(LPCTSTR faceName, int pointSize, DWORD attribute);

#ifdef USE_XML_STORAGE
  HRESULT ReadAttributes(IXmlReader* pReader);
  HRESULT Read(CComPtr<IXmlReader> pReader);

  virtual HRESULT Store(CComPtr<IXmlWriter> pWriter);
#endif // USE_XML_STORAGE

  virtual BOOL LoadFromBufferExt(KMemoryStream& mf);
  virtual BOOL StoreToBufferExt(KMemoryStream& mf);

  BOOL SetText(LPCTSTR text);
  LPCTSTR GetText() { return TextBuffer; }

  RECT Rect;
  OutlineStyle Outline;
  DWORD Fill;
  DWORD TextColor;
  LPTSTR TextBuffer;
  TCHAR FontFaceName[LF_FACESIZE];
  int FontPointSize;
  DWORD FontAttribute;
  //LOGFONT LogFont;
};

