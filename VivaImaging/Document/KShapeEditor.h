#pragma once

#include "KImageBase.h"
#include "KShapeContainer.h"
#include "KDragAction.h"
#include "KShapeProperty.h"

#define R_REDRAW_SHAPE 0x01
#define R_REDRAW_HANDLE 0x02
#define R_REDRAW_THUMB 0x04
#define R_CHANGE_SELECT_TOOL 0x08
#define R_CHANGE_EDITTEXT_TOOL 0x10
#define R_ANNOTATE_CHANGED 0x20

/**
* @brief ShapeEditor Notification event type.
*/
typedef enum tagSHAPE_EVENT_TYPE
{
  SHAPE_EVENT_SELECTED
} SHAPE_EVENT_TYPE;

/**
* @brief ShapeEditor Command type.
*/
typedef enum tagSHAPE_COMMAND_TYPE
{
  SHAPE_ALIGN_LEFT,
  SHAPE_ALIGN_CENTER,
  SHAPE_ALIGN_RIGHT,
  SHAPE_ALIGN_TOP,
  SHAPE_ALIGN_VCENTER,
  SHAPE_ALIGN_BOTTOM,
  SHAPE_ORDER_FORWARD,
  SHAPE_ORDER_BACKWARD,
  SHAPE_ORDER_TOP,
  SHAPE_ORDER_BOTTOM,
  SHAPE_APPLY_PROPERTY,
  SHAPE_COMMAND_MAX
} SHAPE_COMMAND_TYPE;

typedef std::vector<HWND> KHWndVector;
typedef std::vector<HWND>::iterator KHwndIterator;

#define WM_SHAPEEDIT_NOTIFY (WM_USER + 450)

class KShapeEditor : public KShapeContainer
{
public:
  KShapeEditor();
  virtual ~KShapeEditor();

  static void ClearStatic();
  static void StoreDefaultProperty();
  static void LoadDefaultProperty();

  static void RegisterReceiveEvent(HWND hWnd);
  static void SendEventNotify(SHAPE_EVENT_TYPE event, LPVOID param);
  static KHWndVector mNotifyTarget;
  static KShapeProperty mDefault;


  /**
  * @brief 페이지에 포함된 내용 및 대상 개체를 모두 삭제한다.
  */
  void Clear();


  /**
  * @brief 지정한 objectId 목록에 해당하는 개체를 지정한 만큼 이동한다.
  * @param offset : 이동할 양
  * @return int : 이동된 개체의 개수를 리턴한다.
  */
  int MoveObjects(POINT offset);

  /**
  * @brief 지정한 objectId 목록에 해당하는 개체를 지정한 크기로 변경한다.
  * @param offset : 이동할 양
  * @return int : 이동된 개체의 개수를 리턴한다.
  */
  int ResizeObjects(POINT offset);

  /**
  * @brief 지정한 objectId 목록에 해당하는 개체의 특수 핸들을 지정한 위치로 변경한다.
  * @param objectId : 대상 개체의 id 리스트
  * @param type : 이동하는 핸들 종류
  * @param offset : 이동할 양
  * @param keyState : 키보드 상태(Shift, Ctrl, Alt)
  * @return int : 이동된 개체의 개수를 리턴한다.
  */
  int HandleEdit(KIntVector& objectId, EditHandleType type, POINT offset, int keyState);

  /**
  * @brief 선택된 개체의 개수를 리턴한다.
  */
  int GetSelectedObjectsCount(BOOL bAll);

  /**
  * @brief 선택된 개체의 목록을 리턴한다.
  */
  int GetSelectedObjects(KShapeVector& list);

  /**
  * @brief 선택된 개체의 목록을 리턴한다.
  */
  int GetSelectedObjects(KIntVector& list);

  /**
  * @brief 선택된 개체의 최상위 개체를 리턴한다.
  */
  KShapeBase* GetLastSelectedObject();

  int GetSelectedObjectsBound(RECT& rect, KShapeVector* pList);

  /**
  * @brief 지정한 위치에 개체의 핸들, 개체, 가이드라인이 있는지 확인한다.
  * @param pt : 입력 좌표값
  * @param freeIfNotSelected : true이고 대상 개체가 선택되지 않은 상태이면 새로 선택한다.
  * @param editActionData : 선택된 핸들 정보를 담아 리턴할 버퍼
  * @return EditHandleType : 선택된 핸들의 종류
  * @details A. 선택된 개체의 핸들에 해당하는지 검사하고 맞으면 그 핸들 type을 리턴한다.
  * @n B. 모든 개체에 대해 HitOver가 맞으면 MoveObject를 리턴한다.
  */
  EditHandleType GetSelectedObjectsHandle(POINT pt, BOOL freeIfNotSelected, KImageDrawer& drawer, KDragAction& editActionData);

  /**
  * @brief 지정 위치에 선택된 개체가 있으면 TRUE를 리턴한다.
  */
  BOOL IsOverSelectedObject(POINT pt, KImageDrawer& drawer);

  /**
  * @brief 지정 위치에 있는 개체를 리턴한다.
  */
  KShapeBase* FindOverObject(POINT pt, KImageDrawer& drawer);

  /**
  * @brief 선택된 개체들을 모두 해제한다.
  * @details A. 전체 개체 리스트에서 각 개체의 플래그에 선택 플래그가 있으면 해제하고 해제된 개체 개수를 리턴한다.
  */
  int FreeAllSelection(LPRECT ptRect);

  /**
  * @brief 모드 개체들을 선택한다.
  * @details A. 모드 개체들을 선택하고 선택된 개체 개수를 리턴한다.
  */
  int SelectAll(LPRECT ptRect);

  int SetSelected(KVoidPtrArray& shapeList);

  int AddSelectionBy(POINT& pt, BOOL freeIfOut);

  /**
  * @brief 지정한 영역 내의 개체들을 모두 선택한다.
  * @details A. 전체 개체 리스트에서 각 개체의 레이어가 visible이고, 잠금 상태가 아니면,
  * @n      - 개체의 bound가 지정한 사각형 내부이면 선택 플래그를 셋트하고, 바깥이고 선택된 상태이면 선택을 해제한다.
  * @n B. 선택이 바뀐 개체의 개수를 리턴한다.
  */
  int AddSelectionBy(RECT& rect, BOOL freeIfOut);

  /**
  * @brief 선택된 개체를 포함하는 사각 영역을 계산한다.
  */
  int GetSelectionBound(RECT& area);

  /**
  * @brief 선택된 개체의 다음 또는 이전 개체를 선택한다.
  */
  int SelectNextObject(KShapeVector& selectedItems, BOOL bPrevious);

  /**
  * @brief 지정한 개체의 순서를 맨 위로 올린다.
  * @param targets : 대상 개체들의 objectID 리스트.
  * @details A. 스프레드 개체중
  * @n      - 대상 개체 리스트에 속하고  selected_top이 음이면 이 개체의 순서를 selected_top으로 설정한다.
  * @n      - 속하지 않고 selected_top이 유효하면 unselected_top에 지금의 순서를 넣는다.
  * @n      - unselected_top은 selected_top 하위 개체중 선택 안된 첫번째 개체임.
  * @n B. unselected_top이 유효하면,
  * @n      - 대상 개체들을 임시 버퍼 target_items로 이동한다.
  * @n      - target_items의 개체들을 GraphicList의 뒤에 추가한다.
  * @n      - target_items를 해제하고, 그 개수를 리턴한다.
  */
  int BringToTop(KShapeVector& targets);

  /**
  * @brief 지정한 개체의 순서를 맨 아래로 내린다.
  * @param targets : 대상 개체들의 objectID 리스트.
  * @details A. 스프레드 개체중
  * @n      - 대상 개체 리스트에 속하고  target_order가 유효하면( >= 0) 이 개체의 id를 moving_items에 넣는다.
  * @n      - 속하지 않으며 target_order가 음이면 현재의 순서를 target_order로 한다.
  * @n B. 현재의 순서를 유지하도록 임시 리스트에 대상 개체를 옮긴다.(targets의 순서는 무작위임)
  * @n C. order를 하나 올리고 그것이 0보다 크면,
  *       - 대상 개체들을 order 아래로 이동한다.
  */
  int SendToBottom(KShapeVector& targets);

  /**
  * @brief 지정한 개체의 순서를 위로 올린다.
  * @param targets : 대상 개체들의 objectID 리스트.
  * @details A. 스프레드 개체중
  * @n      - 대상 개체 리스트에 속하면 임시로  moving_items에 이동한다.
  * @n      - 속하지 않으며 moving_item에 들어있는 개체들을 이 개체 다음에 삽입한다.
  * @n B. moving_item에 남아있는 개체들을 리스트 뒤에 삽입한다.
  */
  int BringForward(KShapeVector& targets);

  /**
  * @brief 지정한 개체의 순서를 아래로 내린다.
  * @param targets : 대상 개체들의 objectID 리스트.
  * @details A. 스프레드 개체중 역순으로
  * @n      - 대상 개체 리스트에 속하면 임시로  moving_items에 이동한다.
  * @n      - 속하지 않으며 moving_item에 들어있는 개체들을 이 개체 앞에 삽입한다.
  * @n B. moving_item에 남아있는 개체들을 리스트 맨 앞에 삽입한다.
  */
  int SendBackward(KShapeVector& targets);

  /**
  * @brief 지정한 개체를 왼쪽 정렬한다.
  * @param targets : 대상 개체들의 objectID 리스트.
  * @details A. 대상 개체가 하나이면 스프레드 왼쪽 테두리에 붙도록 X좌표를 맞춘다.
  * @n B. 대상 개체가 하나 이상이면,
  * @n      - 가장 왼쪽인 좌표를 찾고
  * @n      - 대상 개체들을 가장 왼쪽 좌표로 이동한다.
  */
  int AlignLeft(KShapeVector& targets);

  /**
  * @brief 지정한 개체를 가운데 정렬한다.
  * @param targets : 대상 개체들의 objectID 리스트.
  * @details A. 대상 개체가 하나이면 스프레드의 중앙으로 X 좌표를 맞춘다.
  * @n B. 대상 개체가 하나 이상이면,
  * @n      - 가장 왼쪽, 가장 오른쪽인 좌표를 찾고 이의 가운데 점을 구한다.
  * @n      - 대상 개체들의 중심을 이 가운데 좌표로 이동한다.
  */
  int AlignCenter(KShapeVector& targets);

  /**
  * @brief 지정한 개체를 오른쪽 정렬한다.
  * @param targets : 대상 개체들의 objectID 리스트.
  * @details A. 대상 개체가 하나이면 스프레드의 오른쪽 테두리에 붙도록 X 좌표를 맞춘다.
  * @n B. 대상 개체가 하나 이상이면,
  * @n      - 가장 오른쪽인 좌표를 찾고,
  * @n      - 대상 개체들의 오른쪽을 이 좌표로 이동한다.
  */
  int AlignRight(KShapeVector& targets);

  /**
  * @brief 지정한 개체를 위쪽 정렬한다.
  * @param targets : 대상 개체들의 objectID 리스트.
  * @details A. 대상 개체가 하나이면 스프레드 위쪽 테두리에 붙도록 Y좌표를 맞춘다.
  * @n B. 대상 개체가 하나 이상이면,
  * @n      - 가장 위쪽인 좌표를 찾고
  * @n      - 대상 개체들을 가장 위쪽 좌표로 이동한다.
  */
  int AlignTop(KShapeVector& targets);

  /**
  * @brief 지정한 개체를 수직 가운데 정렬한다.
  * @param targets : 대상 개체들의 objectID 리스트.
  * @details A. 대상 개체가 하나이면 스프레드의 중앙으로 Y 좌표를 맞춘다.
  * @n B. 대상 개체가 하나 이상이면,
  *       - 가장 위쪽, 가장 아래인 좌표를 찾고 이의 가운데 점을 구한다.
  *       - 대상 개체들의 중심을 이 가운데 좌표로 이동한다.
  */
  int AlignVCenter(KShapeVector& targets);

  /**
  * @brief 지정한 개체를 아래쪽 정렬한다.
  * @param targets : 대상 개체들의 objectID 리스트.
  * @details A. 대상 개체가 하나이면 스프레드의 아래쪽 테두리에 붙도록 Y 좌표를 맞춘다.
  * @n B. 대상 개체가 하나 이상이면,
  *       - 가장 아래쪽인 좌표를 찾고,
  *       - 대상 개체들의 아래쪽을 이 좌표로 이동한다.
  */
  int AlignBottom(KShapeVector& targets);

  int ApplyProperty(KShapeVector& targets, int channel, DWORD fill, OutlineStyle& lineStyle, LPCTSTR facename, DWORD fontColor, int fontSize, int attr);

  int DeleteSelection(LPRECT updateRect);
  HANDLE GetSelectedClipboardData(BOOL bCut);

  inline void SetGroupSelectMode(BOOL bSet) { mGroupSelectMode = bSet; }
  inline BOOL IsGroupSelectMode() {return mGroupSelectMode; }

  inline BOOL IsDragginState() { return (mDragAction.mouseDragMode > MouseDragMode::DragNone); }

  MouseState OnLButtonDown(PropertyMode mode, int nFlags, CPOINT& point, KImageDrawer& drawer, LPRECT ptUpdate);
  int OnMouseMove(MouseState mouseState, int nFlags, POINT& point, LPRECT ptUpdate);
  int OnMouseMoves(MouseState mouseState, int nFlags, LP_CPOINT points, int count, LPRECT ptUpdate);
  int OnLButtonUp(MouseState mouseState, int nFlags, CPOINT& point, LPRECT ptUpdate);
  int OnRButtonDown(PropertyMode mode, int nFlags, POINT& point, KImageDrawer& info, LPRECT ptUpdate);

  BOOL OnDragCancel(LPRECT pUpdate);

  void OnSelectionChanged(KShapeBase* shape);
  KShapeBase* IsNeedTextboxEditText();

  virtual void Draw(Graphics& g, KImageDrawer& info, int flag);

  int MakeCurvePolygon(KShapeVector& objs, RECT& area);

  POINT mButtonDownPt;
	EditHandleType mHandleType;
  KDragAction mDragAction;
  BOOL mGroupSelectMode;
};
