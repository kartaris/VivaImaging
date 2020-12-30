/**
* @file DragAction.cs
* @date 2017.06
* @brief PageBuilder for Windows DragAction class file
*/
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Media;

namespace PageBuilder.Data
{
    /**
    * @enum MouseDragMode
    * @brief 마우스 드래그 모드
    */
    public enum MouseDragMode
    {
        None,
        CreateNewObject, // 새로운 개체 생성 위치 지정 모드
        MoveObject, // 개체 이동 모드
        ResizeObject, // 개체 크기 조정
        MoveGuideline, // 가이드라인 이동
        BlockSelect, // 블럭 지정 선택 모드.
        HandleObject, // 개체 핸들 조정
        CreateHorzGuideline, // 수평 가이드라인 추가
        CreateVertGuideline, // 수직 가이드라인 추가
        MouseDragMax
    };

    /**
    * @class DragAction
    * @brief PageBuilder에서 편집 Action에 따른 개체 외곽선을 Adorner에서 그려주기 위해 전달하는 개체 클래스
    */
    public class DragAction
    {
        public MouseDragMode mouseDragMode;
        public EditHandleType handleType;
        public Graphic targetObject;
        public List<Graphic> extraObjects;
        public int targetIndex; // index of moving guideline
        public Rect originBounds;
        public Point dragAmount;
        public Point resizeRatio;
        public int dragKeyState;
        public GraphicType newObjectType;

        int snapMode;
        Spread currentSnapSpread;

        public const int WITH_XY_SAME_RATIO = 1;
        public const int AS_INTEGER_RATIO = 2;

        public const int MIN_START = -10000;
        public const int MAX_END = 30000;

        /**
        * @brief DragAction class constructor
        */
        public DragAction()
        {
            mouseDragMode = MouseDragMode.None;
            handleType = EditHandleType.None;
            targetObject = null;
            extraObjects = null;
            targetIndex = -1;
            originBounds.X = originBounds.Y = 0;
            originBounds.Width = originBounds.Height = 0;
            dragAmount.X = dragAmount.Y = 0;
            dragKeyState = 0;
            newObjectType = GraphicType.GraphicEmpty;
            snapMode = 0;
            currentSnapSpread = null;
        }

        /**
        * @brief PBDragAction class constructor
        * @param mode : 마우스 드래그 모드
        * @param type : 편집 핸들 타입
        * @param g : 대상 그래픽 개체
        * @param pt : 좌표값
        */
        public DragAction(MouseDragMode mode, EditHandleType type, Graphic g, Point pt)
        {
            mouseDragMode = mode;
            handleType = type;
            targetObject = g;
            extraObjects = null;
            targetIndex = -1;
            originBounds.X = pt.X;
            originBounds.Y = pt.Y;
            originBounds.Width = originBounds.Height = 0;
            dragAmount.X = dragAmount.Y = 0;
            dragKeyState = 0;
            newObjectType = GraphicType.GraphicEmpty;
            snapMode = 0;
            currentSnapSpread = null;
        }

        /**
        * @brief 스냅 활성화된 상태이면 지정한 좌표에 대한 스냅 좌표를 리턴한다.
        * @param pt : 원 좌표.
        * @return Point : 결과 좌표.
        */
        public Point SnapPoint(Point pt)
        {
            if ((mouseDragMode == MouseDragMode.CreateNewObject) &&
                IsSnapMode())
            {
                pt = currentSnapSpread.GetSnappedPoint(pt, snapMode);
            }
            return pt;
        }

        /**
        * @brief 드래그 시작 위치를 설정한다. 스냅 활성화된 상태이면 위치를 스냅 좌표로 바꾼다.
        * @param pt : 시작 위치.
        */
        public Point SetStartPoint(Point pt)
        {
            if ((mouseDragMode == MouseDragMode.CreateNewObject) &&
                IsSnapMode())
            {
                pt = currentSnapSpread.GetSnappedPoint(pt, snapMode);
            }
            originBounds.X = pt.X;
            originBounds.Y = pt.Y;
            originBounds.Width = originBounds.Height = 0;
            return originBounds.TopLeft;
        }

        /**
        * @brief 드래그 원본 크기를 설정한다.
        * @param pt : 드래그 원본 크기.
        */
        public void SetStartSize(Size size)
        {
            originBounds.Size = size;
        }

        /**
        * @brief 스냅 적용을 위한 스프레드와 스냅 모드를 설정한다.
        * @param spread : 스냅 적용할 스프레드 개체.
        * @param mode : 스냅 모드.
        */
        public void SetSnapOption(Spread spread, int mode)
        {
            currentSnapSpread = spread;
            snapMode = mode;
        }

        /**
        * @brief 대상 개체를 추가한다.
        * @param objects : DragAction의 대상이 되는 개체 목록.
        */
        public void AddExtraObjects(List<Graphic> objects)
        {
            if (objects != null)
            {
                if (extraObjects == null)
                    extraObjects = new List<Graphic>();
                foreach (Graphic g in objects)
                {
                    if ((targetObject == null) || (g.ObjectId != targetObject.ObjectId))
                        extraObjects.Add(g);
                }
            }
            else
            {
                extraObjects = null;
            }
        }

        /**
        * @brief 마우스 드래그 이벤트 발생할 때 호출되는 함수
        * @param pt : 마우스 좌표.
        * @param keyState : x-y with same ratio, 정수배 확대 플래그
        * @details A. 핸들의 종류에 따라 이동된 좌표값을 업데이트 한다.
        * @return bool : 드래그 값이 변경되었으면 true를 리턴한다.
        */
        public bool OnDragging(Point pt, int keyState)
        {
            double dy = 0;
            double dx = 0;

            // 개체 영역을 설정하거나 크기 리사이즈할 때에는 이동하는 점 위치를 스냅 적용한다.
            if (((mouseDragMode == MouseDragMode.CreateNewObject) || (mouseDragMode == MouseDragMode.ResizeObject)) &&
                IsSnapMode())
            {
                pt = currentSnapSpread.GetSnappedPoint(pt, snapMode);
            }


            if ((mouseDragMode != MouseDragMode.ResizeObject) ||
                ((handleType != EditHandleType.ResizeLeft) && (handleType != EditHandleType.ResizeRight)))
            {
                dy = pt.Y - originBounds.Y;
            }
            if ((mouseDragMode != MouseDragMode.ResizeObject) ||
                ((handleType != EditHandleType.ResizeTop) && (handleType != EditHandleType.ResizeBottom)))
            {
                dx = pt.X - originBounds.X;
            }

            dragKeyState = keyState;
            if ((dragAmount.X != dx) || (dragAmount.Y != dy))
            {
                dragAmount.X = dx;
                dragAmount.Y = dy;

                if (mouseDragMode == MouseDragMode.ResizeObject)
                {
                    if (originBounds.Width > 0)
                        resizeRatio.X = dragAmount.X / originBounds.Width;

                    if (originBounds.Height > 0)
                        resizeRatio.Y = dragAmount.X / originBounds.Height;
                }

                string str = string.Format("DragAction:OnDragging({0}, {1})", dragAmount.X, dragAmount.Y);
                Console.WriteLine(str);

                return true;
            }
            return false;
        }

        /**
        * @brief 마우스 드래그 액션을 클리어한다.
        */
        public void ClearDragAction()
        {
            mouseDragMode = MouseDragMode.None;
            handleType = EditHandleType.None;
            targetObject = null;
            originBounds.X = originBounds.Y = 0;
            dragKeyState = 0;
            originBounds.Width = originBounds.Height = 0;
            dragAmount.X = dragAmount.Y = 0;
            if (extraObjects != null)
                extraObjects.Clear();

            currentSnapSpread = null;
            snapMode = 0;
        }

        /**
        * @brief 마우스 드래그 액션에 따라 rubber rectangle을 화면에 출력한다.
        * @details A. target 개체에 대한 사각형을 출력한다.
        * @n B. extraObjects에 대한 사각형을 출력한다.
        */
        public void OnRender(DrawingContext drawingContext, Brush brush, Pen pen)
        {
            if ((EditHandleType.ObjectHandle1 <= handleType) && (handleType <= EditHandleType.ObjectHandle4))
            {
                targetObject.OnRenderRubber(drawingContext, brush, pen, mouseDragMode, handleType, dragAmount, dragKeyState);
            }
            else
            {
                if (targetObject != null)
                {
                    targetObject.OnRenderRubber(drawingContext, brush, pen, mouseDragMode, handleType, dragAmount, dragKeyState);
                    // OnRenderRubberObject(drawingContext, brush, pen, targetObject);
                }
                else
                {
                    OnRenderRubberRect(drawingContext, brush, pen, originBounds);
                }

                if (extraObjects != null)
                {
                    foreach (Graphic g in extraObjects)
                    {
                        OnRenderRubberObject(drawingContext, brush, pen, g);
                    }
                }
            }
        }

        bool IsSnapMode()
        {
            return ((currentSnapSpread != null) &&
                (snapMode != 0) && 
                ((mouseDragMode == MouseDragMode.CreateNewObject) ||
                    (mouseDragMode == MouseDragMode.MoveObject) ||
                    (mouseDragMode == MouseDragMode.ResizeObject)));
        }

        public Point GetResultAmount()
        {
            if ((mouseDragMode == MouseDragMode.MoveGuideline) || // 가이드라인 이동
                ((mouseDragMode == MouseDragMode.ResizeObject) &&
                    targetObject.IsLineStyleRubber()))
            {
                return dragAmount;
            }

            return GetResultAmount(targetObject.GetBounds());
        }

        /**
        * @brief 핸들의 이동량을 키보드 제어에 따라 계산한다.
        */
        public Point GetResultAmount(Rect originRect)
        {
            Point amount = dragAmount;

            // snapped amount
            if ((MouseDragMode.MoveObject == mouseDragMode) && IsSnapMode())
            {
                Rect rect = MakeResizedRect(targetObject.GetBounds(), mouseDragMode, handleType, amount);

                Point snap_amount = currentSnapSpread.GetSnappedMovement(rect, snapMode);

                string str = string.Format("snap move ({0}, {1})", snap_amount.X, snap_amount.Y);
                Console.WriteLine(str);

                amount.X += snap_amount.X;
                amount.Y += snap_amount.Y;
            }

            if ((EditHandleType.ObjectHandle1 <= handleType) && (handleType <= EditHandleType.ObjectHandle4))
            {
                amount = targetObject.GetResultRubber(handleType, dragAmount);
            }
            else if ((EditHandleType.MoveHorzGuide <= handleType) && (handleType <= EditHandleType.MoveVertGuide))
            {
                ;
            }
            else
            {
                if ((dragKeyState & WITH_XY_SAME_RATIO) == WITH_XY_SAME_RATIO)
                {
                    double ax = Math.Abs(amount.X);
                    double ay = Math.Abs(amount.Y);
                    if (ax < ay)
                        ax = ay;
                    amount.X = amount.Y = ax;

                    if (dragAmount.X < 0)
                        amount.X = -amount.X;
                    if (dragAmount.Y < 0)
                        amount.Y = -amount.Y;
                }

                if ((dragKeyState & AS_INTEGER_RATIO) == AS_INTEGER_RATIO)
                {
                    if (amount.X > 0)
                    {
                        double r = Math.Abs(amount.X) / originRect.Width;
                        amount.X = originRect.Width * ((int)(r + 0.5));
                    }
                    else if (amount.X < 0)
                    {
                        double r = Math.Abs(amount.X) / originRect.Width;
                        if (r > 0.25)
                            amount.X = -originRect.Width / 2;
                        else
                            amount.X = 0;
                    }
                    if (amount.Y > 0)
                    {
                        double r = Math.Abs(amount.Y) / originRect.Height;
                        amount.Y = originRect.Height * ((int)(r + 0.5));
                    }
                    else if (amount.Y < 0)
                    {
                        double r = Math.Abs(amount.Y) / originRect.Height;
                        if (r > 0.25)
                            amount.Y = -originRect.Height / 2;
                        else
                            amount.Y = 0;
                    }
                }
            }

            return amount;
        }

        /**
        * @brief 원본 사각형 좌표에 변화된 dragAmount를 적용하여 결과 Rect를 리턴한다.
        */
        Rect makeResizedRect(Rect originRect)
        {
            Point amount = GetResultAmount(originRect);
            return MakeResizedRect(originRect, mouseDragMode, handleType, amount);
        }

        /**
        * @brief 원본 사각형 좌표에 mouse-mode, handle-type 이동된 좌표를 적용하여 결과 Rect를 리턴한다.
        */
        public static Rect MakeResizedRect(Rect originRect, MouseDragMode mode, EditHandleType type, Point amount)
        {
            Point s = originRect.TopLeft;
            Point e = originRect.BottomRight;

            if (mode == MouseDragMode.ResizeObject)
            {
                if ((type == EditHandleType.ResizeLeftTop) || (type == EditHandleType.ResizeLeft) || (type == EditHandleType.ResizeLeftBottom))
                {
                    s.X += amount.X;
                }
                else if ((type == EditHandleType.ResizeRightTop) || (type == EditHandleType.ResizeRight) || (type == EditHandleType.ResizeRightBottom))
                {
                    e.X += amount.X;
                }
                if ((type == EditHandleType.ResizeLeftTop) || (type == EditHandleType.ResizeTop) || (type == EditHandleType.ResizeRightTop))
                {
                    s.Y += amount.Y;
                }
                else if ((type == EditHandleType.ResizeLeftBottom) || (type == EditHandleType.ResizeBottom) || (type == EditHandleType.ResizeRightBottom))
                {
                    e.Y += amount.Y;
                }
            }
            else if (mode == MouseDragMode.MoveObject)
            {
                s.X += amount.X;
                s.Y += amount.Y;
                e.X += amount.X;
                e.Y += amount.Y;
            }
            else if ((mode == MouseDragMode.CreateNewObject) || (mode == MouseDragMode.BlockSelect))
            {
                e.X += amount.X;
                e.Y += amount.Y;
            }
            else if (mode == MouseDragMode.CreateHorzGuideline)
            {
                s.Y += amount.Y;
                e.Y += amount.Y;
            }
            else if (mode == MouseDragMode.CreateVertGuideline)
            {
                s.X += amount.X;
                e.X += amount.X;
            }
            else if (mode == MouseDragMode.MoveGuideline)
            {
                if (EditHandleType.MoveHorzGuide == type)
                {
                    s.Y += amount.Y;
                    e.Y += amount.Y;
                }
                else
                {
                    s.X += amount.X;
                    e.X += amount.X;
                }
            }

            return new Rect(Math.Min(s.X, e.X), Math.Min(s.Y, e.Y), Math.Abs(s.X - e.X), Math.Abs(s.Y - e.Y));
        }

        void OnRenderRubberObject(DrawingContext drawingContext, Brush brush, Pen pen, Graphic g)
        {
            if (g.IsLineStyleRubber())
            {
                Point sp = g.GetStartPoint();
                Point ep = g.GetEndPoint();

                if ((mouseDragMode == MouseDragMode.MoveObject) ||
                    ((mouseDragMode == MouseDragMode.ResizeObject) && (handleType == EditHandleType.ResizeLeftTop)))
                {
                    sp.X += dragAmount.X;
                    sp.Y += dragAmount.Y;
                }

                if ((mouseDragMode == MouseDragMode.MoveObject) ||
                    ((mouseDragMode == MouseDragMode.ResizeObject) && (handleType == EditHandleType.ResizeRightBottom)))
                {
                    ep.X += dragAmount.X;
                    ep.Y += dragAmount.Y;
                }
                drawingContext.DrawLine(pen, sp, ep);
            }
            else
            {
                OnRenderRubberRect(drawingContext, brush, pen, g.GetBounds());
            }
        }

    /**
    * @brief 출력할 DrawingContext에 변경된 좌표를 계산하여 지정한 brusn, pen으로 출력한다.
    * @param drawingContext : 출력할 target device
    * @param brush : 출력에 사용할 brush
    * @param pen : 출력에 사용할 pen
    * @param originRect : 원본 좌표
    */
    void OnRenderRubberRect(DrawingContext drawingContext, Brush brush, Pen pen, Rect originRect)
        {
            if ((mouseDragMode == MouseDragMode.CreateNewObject) &&
                ((newObjectType == GraphicType.GraphicLine) ||
                (newObjectType == GraphicType.GraphicBezierPath)))
            {
                Point ex = new Point(originRect.Left + dragAmount.X, originRect.Top + dragAmount.Y);
                drawingContext.DrawLine(pen, originRect.TopLeft, ex);
                return;
            }

            Rect rect = makeResizedRect(originRect);

            string str = string.Format("Adorner size : {0}, {1}, {2}, {3}", rect.X, rect.Y, rect.Right, rect.Bottom);
            Console.WriteLine(str);

            if (MouseDragMode.CreateHorzGuideline == mouseDragMode)
            {
                Point ps = new Point(MIN_START, rect.Y);
                Point pe = new Point(MAX_END, rect.Y);
                drawingContext.DrawLine(pen, ps, pe);
            }
            else if (MouseDragMode.CreateVertGuideline == mouseDragMode)
            {
                Point ps = new Point(rect.X, MIN_START);
                Point pe = new Point(rect.X, MAX_END);
                drawingContext.DrawLine(pen, ps, pe);
            }
            else if (MouseDragMode.MoveGuideline == mouseDragMode)
            {
                if (EditHandleType.MoveHorzGuide == handleType)
                {
                    Point ps = new Point(MIN_START, rect.Y);
                    Point pe = new Point(MAX_END, rect.Y);
                    drawingContext.DrawLine(pen, ps, pe);
                }
                else // if (EditHandleType.MoveVertGuide == handleType))
                {
                    Point ps = new Point(rect.X, MIN_START);
                    Point pe = new Point(rect.X, MAX_END);
                    drawingContext.DrawLine(pen, ps, pe);
                }
            }
            else
            {
                drawingContext.DrawRectangle(brush, pen, rect);
            }
        }

    }
}
