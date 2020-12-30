#pragma once


#include "Shape/KShapeBase.h"
#include "Shape/KShapeLine.h"
#include "Shape/KShapePolygon.h"
#include "Shape/KShapeRectangle.h"
#include "Shape/KShapeEllipse.h"
#include "Shape/KShapeTextbox.h"
#include "Shape/KShapeSmart.h"

#include "KImageBase.h"
#include "KImageDrawer.h"
#include < XmlLite.h>

class KShapeContainer
{
public:
  KShapeContainer();
  virtual ~KShapeContainer();

  /**
  * @brief 페이지에 포함된 내용 및 대상 개체를 모두 삭제한다.
  */
  virtual void Clear();

  void ClearList();

  /**
  * @brief 대상 개체를 현재의 페이지에 추가한다.
  * @param s : 대상 개체
  */
  void AddShape(KShapeBase* s);

  /**
  * @brief 지정한 index에 해당하는 개체를 찾아 리턴한다.
  * @param index : 찾을 대상 개체의 index
  */
  KShapeBase* GetObjectByIndex(int index);

  /**
  * @brief 지정한 object_id에 해당하는 개체를 찾아 리턴한다.
  * @param object_id : 찾을 대상 개체의 id
  */
  KShapeBase* GetObject(int object_id);

  int RemoveObjectsById(KIntVector& objectId);
  inline int GetCount() { return (int)mShapeList.size(); }

  /**
  * @brief 지정한 id의 개체를 스프레드 Graphic 목록에서 제거하고 개체를 리턴한다.
  * @param child_id : 개체의 ID
  * @return Graphic : 스프레드에서 제거된 개체
  */
  KShapeBase* DetachChildObject(int child_id);

  KShapeBase* DetachChildObject(KShapeBase* s);

  void SetSize(int w, int h);

  virtual void Draw(Graphics& g, KImageDrawer& info, int flag);

  int GetObjectsBound(KShapeVector& objs, RECT& area);

  int GetSelectedItems(KShapeContainer& container, BOOL bRemove);

#ifdef USE_XML_STORAGE
  HRESULT Store(CComPtr<IXmlWriter> pWriter);
#else
  DWORD Load(KMemoryStream& ms);
  DWORD Store(KMemoryStream& ms);
#endif // USE_XML_STORAGE

  DWORD StoreToBuffer(KMemoryStream& mf);
  DWORD LoadFromBuffer(KMemoryStream& mf);
  DWORD LoadFromBufferBody(KMemoryStream& mf, KVoidPtrArray* pShapeList = NULL);

  KShapeVector mShapeList;

  int Width;
  int Height;

};
