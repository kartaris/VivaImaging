/**
* @file Trapezoid.cs
* @date 2017.06
* @brief PageBuilder for Windows Trapezoid class file
*/
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media;
using System.Windows.Shapes;
using PageBuilder.Data;

namespace PageBuilder.Data
{
    /**
    * @class Trapezoid 
    * @brief 사다리꼴 도형 개체 클래스
    */
    public class Trapezoid : Graphic
    {
        /**
         * Relative movement of Left Edge (0 to 0.5)
         */
        public double LeftHandle;
        /**
         * Relative movement of Right Edge (0 to 0.5)
         */
        public double RightHandle;

        /**
        * @brief Trapezoid  class constructor
        */
        public Trapezoid() : base()
        {
        }

        /**
        * @brief Trapezoid  class constructor
        * @param rect : 개체의 좌표
        * @param layerId : 개체의 소속 레이어 ID
        */
        public Trapezoid(Rect rect, int layerId) : base(rect, layerId)
        {
            LeftHandle = 0.2;
            RightHandle = 0.2;
        }

        /**
        * @brief 개체의 종류를 리턴하는 가상함수
        */
        public override GraphicType ObjectType()
        {
            return GraphicType.GraphicTrapezoid;
        }

        public void SetHandle(double left, double right)
        {
            if (left < 0)
                left = 0;
            if (left > 0.5)
                left = 0.5;
            LeftHandle = left;

            if (right < 0)
                right = 0;
            if (right > 0.5)
                right = 0.5;
            RightHandle = right;
            ClearPathGeometry();
        }

        /**
        * @brief 개체가 선택된 상태의 핸들을 출력하는 가상 함수.
        * @param drawingContext : 대상 Context
        */
        public override void RenderObjectHandle(DrawingContext drawingContext)
        {
            base.RenderObjectHandle(drawingContext);

            Point edge;

            double move = Width * RightHandle;
            edge = new Point(Right() - move, Y);
            RenderSpecialHandle(drawingContext, edge);

            move = Width * LeftHandle;
            edge = new Point(X + move, Y);
            RenderSpecialHandle(drawingContext, edge);
        }

        /**
        * @brief 지정한 좌표에 개체가 위치하는지 체크하는 가상 함수.
        * @param pt : 지정한 좌표
        * @return bool : 위치가 해당되면 true를 리턴한다.
        */
        public override bool HitTestOver(Point pt)
        {
            return pathGeom.FillContains(pt, 2, ToleranceType.Absolute);
        }

        public override EditHandleType HitTestObjectsHandle(Point pt)
        {
            Point edge;

            double move = Width * LeftHandle;
            edge = new Point(X + move, Y);

            double handle_half = SELECTED_HANDLE_SIZE / 2;
            if ((Math.Abs(pt.X - edge.X) <= handle_half) &&
                (Math.Abs(pt.Y - edge.Y) <= handle_half))
                return EditHandleType.ObjectHandle1;

            move = Width * RightHandle;
            edge = new Point(Right() - move, Y);

            if ((Math.Abs(pt.X - edge.X) <= handle_half) &&
                (Math.Abs(pt.Y - edge.Y) <= handle_half))
                return EditHandleType.ObjectHandle2;

            return base.HitTestObjectsHandle(pt);
        }

        public override void OnRenderRubber(DrawingContext drawingContext, Brush brush, Pen pen, MouseDragMode mode, EditHandleType handleType, Point dragAmount, int keyState)
        {
            double leftHandle = LeftHandle;
            double rightHandle = RightHandle;
            Rect bound;

            if (handleType == EditHandleType.ObjectHandle1)
            {
                double edge_pt = Width * LeftHandle;
                leftHandle = (edge_pt + dragAmount.X) / Width;
                if (leftHandle < 0)
                    leftHandle = 0;
                if (leftHandle > 0.5)
                    leftHandle = 0.5;

                if ((keyState & DragAction.WITH_XY_SAME_RATIO) == 0) //DragAction.WITH_XY_SAME_RATIO)
                {
                    rightHandle = leftHandle;
                }

                string str = string.Format("new handle = {0}", leftHandle);
                Console.WriteLine(str);
                bound = GetBounds();
            }
            else if (handleType == EditHandleType.ObjectHandle2)
            {
                double edge_pt = Width - Width * RightHandle;
                rightHandle = 1 - (edge_pt + dragAmount.X) / Width;
                if (rightHandle < 0)
                    rightHandle = 0;
                if (rightHandle > 0.5)
                    rightHandle = 0.5;

                if ((keyState & DragAction.WITH_XY_SAME_RATIO) == 0) //DragAction.WITH_XY_SAME_RATIO)
                {
                    leftHandle = rightHandle;
                }

                string str = string.Format("new handle = {0}", rightHandle);
                Console.WriteLine(str);
                bound = GetBounds();
            }
            else
            {
                bound = DragAction.MakeResizedRect(GetBounds(), mode, handleType, dragAmount);
            }

            double edge_left = bound.Width * leftHandle;
            double edge_right = bound.Width * rightHandle;

            StreamGeometry streamGeometry = new StreamGeometry();
            using (StreamGeometryContext geometryContext = streamGeometry.Open())
            {
                PointCollection points = new PointCollection();
                geometryContext.BeginFigure(new Point(bound.Left, bound.Bottom), true, true);
                points.Add(bound.BottomRight);
                points.Add(new Point(bound.Right - edge_right, bound.Top));
                points.Add(new Point(bound.Left + edge_left, bound.Top));
                geometryContext.PolyLineTo(points, true, true);
            }

            drawingContext.DrawGeometry(brush, pen, streamGeometry);
        }

        public override Point GetResultRubber(EditHandleType handleType, Point dragAmount)
        {
            Point result = new Point(0, 0);
            double edge_pt;
            if (handleType == EditHandleType.ObjectHandle1)
            {
                edge_pt = Width * LeftHandle;
                result.X = (edge_pt + dragAmount.X) / Width;
            }
            else
            {
                edge_pt = Width - Width * RightHandle;
                result.X = 1 - (edge_pt + dragAmount.X) / Width;
            }

            if (result.X < 0)
                result.X = 0;
            if (result.X > 0.5)
                result.X = 0.5;
            return result;
        }

        /**
        * @brief 개체별 특수 핸들을 드롭하고 결과값을 Handle에 적용하는 가상 함수.
        * @param handleType : 드래깅하는 핸들의 종류
        * @param handle : 드래깅에 의해 변동된 handle값
        * @param keyState : Shift/Ctrl 키 상태값
        * @return bool : 값이 정상적으로 설정되었으면 true를 리턴한다.
        */
        public override bool HandleEdit(EditHandleType handleType, Point handle, int keyState)
        {
            bool changed = false;
            if (handleType == EditHandleType.ObjectHandle1)
            {
                if (LeftHandle != handle.X)
                {
                    LeftHandle = handle.X;
                    changed = true;
                }

                if ((keyState & DragAction.WITH_XY_SAME_RATIO) == 0) //DragAction.WITH_XY_SAME_RATIO)
                {
                    if (RightHandle != handle.X)
                    {
                        RightHandle = handle.X;
                        changed = true;
                    }
                }

            }
            else if (handleType == EditHandleType.ObjectHandle2)
            {
                if (RightHandle != handle.X)
                {
                    RightHandle = handle.X;
                    changed = true;
                }
                if ((keyState & DragAction.WITH_XY_SAME_RATIO) == 0) //DragAction.WITH_XY_SAME_RATIO)
                {
                    if (LeftHandle != handle.X)
                    {
                        LeftHandle = handle.X;
                        changed = true;
                    }
                }
            }

            if (changed)
                ClearPathGeometry();

            return changed;
        }

        public override void CreateDrawing(Canvas dc)
        {
            if (pathGeom == null)
            {
                PathFigure pathFigure = new PathFigure();

                pathFigure.StartPoint = BottomLeft();
                pathFigure.Segments.Add(new LineSegment(BottomRight(), true));
                double move = Width * RightHandle;
                pathFigure.Segments.Add(new LineSegment(new Point(Right() - move, Y), true));
                move = Width * LeftHandle;
                pathFigure.Segments.Add(new LineSegment(new Point(X + move, Y), true));
                pathFigure.IsClosed = true;

                pathGeom = new PathGeometry();
                ((PathGeometry)pathGeom).Figures.Add(pathFigure);
            }

            Path path = new Path();
            GetFillStroke(ref path);
            path.Data = pathGeom;

            dc.Children.Add(path);

            // draw label textbox
            CreateDrawingText(dc);
        }

    }
}
