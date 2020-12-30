#pragma once
#include "stdafx.h"

#include "KImageBase.h"
#include "Shape\KShapeBase.h"


#define WITH_XY_SAME_RATIO 1
#define AS_INTEGER_RATIO 2
#define MIN_START -10000
#define MAX_END 30000

class KDragAction
{
public:
  KDragAction();
  virtual ~KDragAction();

  void StartDragAction(MouseDragMode mode, EditHandleType type, POINT pt, KShapeBase* g, ShapeType shape);
 
  void StartDragAction(MouseDragMode mode, EditHandleType type, POINT pt, KShapeVector& list, RECT& boundRect);

  inline void SetOriginBound(RECT& r) { memcpy(&originBounds, &r, sizeof(RECT)); }

  inline BOOL IsActive() { return(mouseDragMode != MouseDragMode::DragNone); }
  /**
  * @brief 드래그 시작 위치를 설정한다. 스냅 활성화된 상태이면 위치를 스냅 좌표로 바꾼다.
  * @param pt : 시작 위치.
  */
  POINT SetStartPoint(POINT pt);

  /**
  * @brief 드래그 원본 크기를 설정한다.
  * @param size : 드래그 원본 크기.
  */
  void SetStartSize(SIZE& size);

  /**
  * @brief 스냅 적용을 위한 스프레드와 스냅 모드를 설정한다.
  * @param spread : 스냅 적용할 스프레드 개체.
  * @param mode : 스냅 모드.
  */
  void SetSnapOption(int mode);

  /**
  * @brief 대상 개체를 추가한다.
  * @param objects : DragAction의 대상이 되는 개체 목록.
  */
  void AddExtraObjects(KShapeVector& objects, BOOL clearPrevious);

  /**
  * @brief 마우스 드래그 상태값을 클리어 한다.
  */
  void Clear();

  /**
* @brief 그룹 선택 좌표를 설정한다.
*/
  BOOL mbGroupSelected;
  RECT mGroupBounds;

  void SetSelectionBounds(BOOL bGroup, RECT& rect);

  /**
  * @brief 마우스 드래그 이벤트 발생할 때 호출되는 함수
  * @param pt : 마우스 좌표.
  * @param keyState : x-y with same ratio, 정수배 확대 플래그
  * @param ptUpdate : 화면 업데이트할 영역
  * @details A. 핸들의 종류에 따라 이동된 좌표값을 업데이트 한다.
  * @return bool : 드래그 값이 변경되었으면 true를 리턴한다.
  */
  BOOL OnDragging(POINT& pt, int keyState, LPRECT ptUpdate);

  BOOL OnDragging(LP_CPOINT points, int count, int keyState, LPRECT ptUpdate);

  BOOL OnDragCancel(LPRECT pUpdate);

  BOOL OnDragEnd(LPRECT pUpdate);

  void GetUpdateRect(RECT& rect);


  /**
  * @brief 마우스 드래그 액션에 따라 rubber rectangle을 화면에 출력한다.
  * @details A. target 개체에 대한 사각형을 출력한다.
  * @n B. extraObjects에 대한 사각형을 출력한다.
  */
  void OnRender(Graphics& g, KImageDrawer& info);

  /**
  * @brief 핸들의 이동량을 키보드 제어에 따라 계산한다.
  */
  POINT GetResultAmount(RECT& originRect);

  void GetResultRect(LPRECT r, BOOL bClear);

  inline KShapeBase* DetachObject() {
    KShapeBase* s = targetObject; targetObject = NULL; return s;
  }

  /**
  * @brief 원본 사각형 좌표에 mouse-mode, handle-type 이동된 좌표를 적용하여 결과 Rect를 리턴한다.
  */
  RECT MakeResizedRect(RECT& originRect);

  RECT MakeResizedRect(RECT& originRect, POINT& amount);

  /**
  * @brief 원본 직선 좌표에 mouse-mode, handle-type 이동된 좌표를 적용하여 결과를 원본 시작점-끝점에 저장하여 리턴한다.
  */
  void MakeResizedRect(POINT& osp, POINT& oep);
  void MakeResizedRect(POINT& osp, POINT& oep, POINT& amount);

  /**
  * @brief 폴리곤 점의 위치를 드래그에 따라 이동한다.
  * @param p : 대상 점 좌표
  * @param newBound : 드래그된 바운드 영역
  */
  void MakeResizedPoint(POINT& p, RECT& orgBound, RECT& newBound);

  void OnRenderRubberRect(Graphics& g, RECT& originRect, KImageDrawer& info);

  MouseDragMode mouseDragMode;
  EditHandleType handleType;
  KShapeBase* targetObject;
  KShapeVector extraObjects;
  int targetIndex; // index of moving guideline
  RECT originBounds;
  POINT dragStart;
  POINT dragAmount; // drag 이동값
  POINT resultAmount; // shift-ctrl 적용한 drag에 따른 최종 이동값.
  POINT resizeRatio;
  // POINT dragOffset;

  int dragKeyState;
  ShapeType newObjectType;

  int snapMode;
  // Spread currentSnapSpread;
};
