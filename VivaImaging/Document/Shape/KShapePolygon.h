/**
* @file KShapePolygon.h
* @date 2017.05
* @brief KShape for polygon class file
*/

#include "stdafx.h"
#include "KShapeBase.h"

#define PRESSURE_SENSITIVE_WIDTH_BASE 100

class KShapePolygon : public KShapeBase
{
public:
  /**
  * @brief Line class constructor
  * @param sp : 개체의 시작점 좌표
  * @param ep : 개체의 끝점 좌표
  */
  KShapePolygon();
  KShapePolygon(LP_CPOINT pts, int point_num, BOOL closed);
  KShapePolygon(LP_CPOINT pts, int point_num, DWORD fillcolor, OutlineStyle linestyle, BOOL closed);
  virtual ~KShapePolygon();

  virtual ShapeType GetShapeType() { return ShapeType::ShapePolygon; }
  virtual ShapeType GetResizeType() { return ShapeType::ShapeRectangle; }

  void dumpPoints();

  BOOL Move(POINT& offset);
  EditHandleType HitObjectsHandle(KImageDrawer& drawer, POINT& pt);
  EditHandleType HitOver(KImageDrawer& drawer, POINT& pt);

  virtual BOOL HitOver(POINT& pt);

  virtual BOOL Move(POINT offset);
  virtual BOOL ResizeByObjectHandle(EditHandleType type, POINT offset);

  virtual void GetRectangle(RECT& rect);
  virtual void GetNormalRectangle(RECT& rect);
  virtual RECT GetBounds(KImageDrawer* drawer);

  void readyGraphicsPath(GraphicsPath& path, KImageDrawer& info, KDragAction* drag);

  virtual void Draw(Graphics& g, KImageDrawer& info, int flag);

  int getPathSegment(int s, GraphicsPath& path, KImageDrawer& info, float& penWidth, KDragAction* drag);
  void drawPathShape(Graphics& g, KImageDrawer& info, GraphicsPath* path, int flag, float prs = 1.0);


  virtual void OnRenderRubber(Graphics& g, KDragAction* drag, KImageDrawer& info, int flag);
  virtual void OnDragEnd(KDragAction* drag, POINT& amount);
  virtual void GetDragBounds(RECT& bound, KDragAction* drag);
  
  virtual BOOL GetFillColor(DWORD& color);
  virtual BOOL GetOutlineStyle(OutlineStyle& linestyle);

  virtual BOOL SetFillColor(DWORD& color);
  virtual BOOL SetOutlineStyle(OutlineStyle& linestyle);

#ifdef USE_XML_STORAGE
  int parsePointValues(LPCTSTR szValue);
  HRESULT ReadAttributes(IXmlReader* pReader);
  HRESULT Read(CComPtr<IXmlReader> pReader);

  virtual HRESULT Store(CComPtr<IXmlWriter> pWriter);
#endif

  virtual BOOL LoadFromBufferExt(KMemoryStream& mf);
  virtual BOOL StoreToBufferExt(KMemoryStream& mf);

 // void AddPoint(POINT& pt);
  BOOL AddPoints(LPPOINT points, int number);
  BOOL AddPoints(LP_CPOINT points, int count);

  BOOL FlattenPolygon();
  BOOL MakeCurvePolygon();

  RECT CalcRectangle();

  BOOL IsPressureSensitiveDrawing();

  RECT Rect;
  /** 좌표 */
  LP_CPOINT Points;
  int allocNumber;
  BOOL Closed;
  /** 좌표 개수 */
  int PointNumber;
  /** 개체의 선 정보 */
  OutlineStyle Outline;

  DWORD Fill;

  /** 개체의 화살표 정보 */
  BYTE StartArrow;
  BYTE EndArrow;


};

