/**
* @file KShapeRectangle.h
* @date 2017.05
* @brief KShape for Windows Rectangle class file
*/
    /**
    * @class Rectangle
    * @brief Rectangle 개체 클래스
    */
#include "stdafx.h"
#include "KShapeBase.h"

class KShapeRectangle : public KShapeBase
{
public:
  KShapeRectangle();
  KShapeRectangle(RECT rect);
  KShapeRectangle(RECT rect, DWORD fillcolor, OutlineStyle linestyle);

  ~KShapeRectangle();

  virtual ShapeType GetShapeType() { return ShapeType::ShapeRectangle; }
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

  virtual BOOL GetFillColor(DWORD& color);
  virtual BOOL GetOutlineStyle(OutlineStyle& linestyle);

  virtual BOOL SetFillColor(DWORD& color);
  virtual BOOL SetOutlineStyle(OutlineStyle& linestyle);

#ifdef USE_XML_STORAGE
  HRESULT ReadAttributes(IXmlReader* pReader);
  HRESULT Read(CComPtr<IXmlReader> pReader);

  virtual HRESULT Store(CComPtr<IXmlWriter> pWriter);
#endif // USE_XML_STORAGE

  virtual BOOL LoadFromBufferExt(KMemoryStream& mf);
  virtual BOOL StoreToBufferExt(KMemoryStream& mf);

  RECT Rect;
  OutlineStyle Outline;
  DWORD Fill;

	static BOOL IsOver(RECT r, POINT pt);
};

