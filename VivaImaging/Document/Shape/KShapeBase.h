/**
* @file KShapeBase.h
* @date 2017.05
* @brief KShape for base class file
*/
#pragma once

#include <vector>
#include "KShapeDef.h"
#include "CurveTracer.h"
#include "../KImageDrawer.h"
#include "../../Core/KMemoryStream.h"

#include < XmlLite.h>

typedef struct tagShapeHeader {
  int type;
  UINT size;
} ShapeHeader;

// Draw() flag
#define DRAW_WHITE_BACKGROUND 1
#define DRAW_NO_HANDLE 2

class KDragAction;

class KShapeBase
{
public:
  KShapeBase();
  KShapeBase(KShapeBase* p);
  virtual ~KShapeBase();

  virtual ShapeType GetShapeType() { return ShapeType::ShapeNone; }
  virtual ShapeType GetResizeType() { return ShapeType::ShapeNone; }

  virtual BOOL Move(POINT& offset) { return FALSE; }
  virtual BOOL ResizeByObjectHandle(EditHandleType type, POINT& offset) { return FALSE; }

  virtual BOOL HandleEdit(EditHandleType handleType, POINT& handle, int keyState) { return FALSE; }
  virtual EditHandleType HitObjectsHandle(KImageDrawer& drawer, POINT& pt) { return EditHandleType::HandleNone; }
  virtual EditHandleType HitOver(KImageDrawer& drawer, POINT& pt) { return EditHandleType::HandleNone; }

  virtual BOOL HitOver(POINT& pt) { return FALSE; }

  /**
  * @brief 개체의 좌표를 리턴한다.
  */
  virtual void GetRectangle(RECT& rect) { ; }
  virtual void GetNormalRectangle(RECT& rect) { ; }

  /**
  * @brief 개체의 외곽 좌표를 리턴한다.(외곽선두께, 화살표 크기를 포함)
  */
  virtual RECT GetBounds(KImageDrawer* drawer);
  virtual RECT GetBounds(RECT& rect, KImageDrawer* drawer);

  void AddToBounds(RECT& rect, KImageDrawer* drawer, int penHalfWidth);

  virtual int GetPenWidth();

  virtual void Draw(Graphics& g, KImageDrawer& info, int flag) { ; }

  virtual BOOL SetRectangle(RECT& rect) { return FALSE; }
  virtual BOOL SetEndPoint(POINT& point, BOOL bFinish) { return FALSE; }

  virtual void OnRenderRubber(Graphics& g, KDragAction* drag, KImageDrawer& info, int flag) { ; }
  virtual void GetDragBounds(KDragAction* drag, int flag, RECT& rect) { ; }
  virtual void OnDragEnd(KDragAction* drag, POINT& amount) { ; }
  virtual void GetDragBounds(RECT& bound, KDragAction* drag) { ; }

  virtual BOOL GetFillColor(DWORD& color) { return FALSE; }
  virtual BOOL GetOutlineStyle(OutlineStyle& linestyle) { return FALSE; }
  virtual BOOL GetTextColor(DWORD& color) { return FALSE; }
  virtual BOOL GetFont(LPTSTR faceName, int& pointSize, DWORD& attribute) { return FALSE; }

  virtual BOOL SetFillColor(DWORD& color) { return FALSE; }
  virtual BOOL SetOutlineStyle(OutlineStyle& linestyle) { return FALSE; }
  virtual BOOL SetTextColor(DWORD& color) { return FALSE; }
  virtual BOOL SetFont(LPCTSTR faceName, int pointSize, DWORD attribute) { return FALSE; }

  void SetShapeFlag(BOOL bSet, int flag);

  void ClearSelected() { ObjectFlag &= ~KSHAPE_SELECTED; }
  void SetSelected() { ObjectFlag |= KSHAPE_SELECTED; }

  BOOL IsSelected() { return ((ObjectFlag & KSHAPE_SELECTED) == KSHAPE_SELECTED); }
  BOOL IsVisible() { return ((ObjectFlag & KSHAPE_VISIBLE) == KSHAPE_VISIBLE); }
  BOOL IsLocked() { return ((ObjectFlag & KSHAPE_LOCKED) == KSHAPE_LOCKED); }

  inline void SetChildIndex(int index) { childIndex = index; }

  static KShapeBase* createShape(ShapeType shape_type);

  static int ReadyShapePolygon(ShapeType type, Point* p, int count, RECT& r);

  virtual BOOL HitTest(KImageDrawer& info);

#ifdef USE_XML_STORAGE
  HRESULT ReadFill(DWORD& Fill, CComPtr<IXmlReader> pReader);
  HRESULT ReadOutline(OutlineStyle& outline, CComPtr<IXmlReader> pReader);
  HRESULT ReadFontInfo(LPTSTR faceName, int& pointSize, DWORD& fontAttribute, CComPtr<IXmlReader> pReader);

  virtual HRESULT Store(CComPtr<IXmlWriter> pWriter) { return -2; }

  HRESULT StoreStartEndPoint(POINT& sp, POINT& ep, CComPtr<IXmlWriter> pWriter);
  HRESULT StoreRect(RECT& rect, CComPtr<IXmlWriter> pWriter);
  HRESULT StoreFill(DWORD fillColor, CComPtr<IXmlWriter> pWriter);
  HRESULT StoreOutline(OutlineStyle& outline, CComPtr<IXmlWriter> pWriter);
  HRESULT StoreTextColor(DWORD color, CComPtr<IXmlWriter> pWriter);
  HRESULT StoreFontInfo(LPCTSTR faceName, int pointSize, int fontAttribute, CComPtr<IXmlWriter> pWriter);
#endif // USE_XML_STORAGE

  static KShapeBase* LoadFromBuffer(KMemoryStream& mf, int* readed);
  virtual BOOL LoadFromBufferExt(KMemoryStream& mf);

  int StoreToBuffer(KMemoryStream& mf);
  virtual BOOL StoreToBufferExt(KMemoryStream& mf);

  UINT childIndex;
  UINT ObjectId;
  UINT ObjectFlag;

  static UINT gObjectID;
};

#include <vector>

typedef std::vector<KShapeBase*> KShapeVector;
typedef std::vector<KShapeBase*>::iterator KShapeIterator;
typedef std::vector<KShapeBase*>::reverse_iterator KShapeRevIterator;

extern BOOL DoesListContainValue(KShapeVector& objectId, KShapeBase* s);

extern BOOL RectContains(RECT& rect, RECT subrect);
extern void AddRect(RECT& dst, RECT& src);
extern void AddRect(RECT& dst, int cx, int cy);
extern void OrRect(RECT& dst, RECT& src);
extern void ExpandRect(RECT& dst, int cx, int cy);

extern BYTE GetAlphaValue(COLORREF color);
extern void NormalizeRect(RECT& rect);

/*
typedef struct tagDragAction
{
  MouseDragMode mouseDragMode;
  EditHandleType handleType;
  KShapeBase* targetObject;
  KShapeVector extraObjects;
  int targetIndex; // index of moving guideline
  RECT originBounds;
  POINT dragAmount;
  POINT resizeRatio;
  int dragKeyState;
  int newObjectType;

  int snapMode;
} DragAction;
*/

#define WITH_XY_SAME_RATIO 1
#define AS_INTEGER_RATIO 2

#define MIN_START -10000
#define MAX_END 30000
