/**
* @file KShapeLine.h
* @date 2017.05
* @brief KShape for Line class file
*/

#include "stdafx.h"
#include "KShapeBase.h"

class KShapeLine : public KShapeBase
{
public:

  /**
  * @brief Line class constructor
  * @param sp : 개체의 시작점 좌표
  * @param ep : 개체의 끝점 좌표
  */
  KShapeLine();
  KShapeLine(POINT sp, POINT ep);
  KShapeLine(POINT sp, POINT ep, OutlineStyle linestyle, BYTE start, BYTE end);
  virtual ~KShapeLine();

  virtual ShapeType GetShapeType() { return ShapeType::ShapeLine; }
  virtual ShapeType GetResizeType() { return ShapeType::ShapeLine; }

	virtual EditHandleType HitObjectsHandle(KImageDrawer& drawer, POINT& pt);
  virtual EditHandleType HitOver(KImageDrawer& drawer, POINT& pt);

  virtual BOOL HitOver(POINT& pt);

  virtual void GetRectangle(RECT& rect);
  virtual void GetNormalRectangle(RECT& rect);
  virtual RECT GetBounds(KImageDrawer* drawer);

  virtual BOOL Move(POINT& offset);
  virtual BOOL ResizeByObjectHandle(EditHandleType type, POINT offset);

  virtual void Draw(Graphics& g, KImageDrawer& info, int flag);

  void drawShape(Graphics& g, KImageDrawer& info, POINT& sp, POINT& ep, int flag);

  virtual BOOL SetEndPoint(POINT& point, BOOL bFinish);

  virtual void OnRenderRubber(Graphics& g, KDragAction* drag, KImageDrawer& info, int flag);
  virtual void OnDragEnd(KDragAction* drag, POINT& amount);
  virtual void GetDragBounds(RECT& bound, KDragAction* drag);

  virtual BOOL GetOutlineStyle(OutlineStyle& linestyle);
  virtual BOOL SetOutlineStyle(OutlineStyle& linestyle);

#ifdef USE_XML_STORAGE
  HRESULT ReadAttributes(IXmlReader* pReader);
  HRESULT Read(CComPtr<IXmlReader> pReader);

  virtual HRESULT Store(CComPtr<IXmlWriter> pWriter);
#endif // USE_XML_STORAGE

  virtual BOOL LoadFromBufferExt(KMemoryStream& mf);
  virtual BOOL StoreToBufferExt(KMemoryStream& mf);

  /** 시작점 좌표 */
  POINT StartPoint;
  /** 끝점 좌표 */
  POINT EndPoint;
  /** 개체의 선 정보 */
  OutlineStyle Outline;
  /** 개체의 화살표 정보 */
  BYTE StartArrow;
  BYTE EndArrow;
};

