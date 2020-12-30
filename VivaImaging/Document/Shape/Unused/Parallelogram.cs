/**
* @file Parallelogram.cs
* @date 2017.06
* @brief PageBuilder for Windows Parallelogram class file
*/
using PageBuilder.Data;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media;
using System.Windows.Shapes;

namespace PageBuilder.Data
{
    /**
    * @class Parallelogram 
    * @brief 평행사변형 개체 클래스
    */
    public class Parallelogram : Graphic
    {
        /**
         * Relative movement of Edge (0 to 1, 0 for rectangle)
         */
        double Handle;

        /**
        * @brief Parallelogram class constructor
        */
        public Parallelogram() : base()
        {
        }

        /**
        * @brief Parallelogram class constructor
        * @param rect : 개체의 좌표
        * @param layerId : 개체의 소속 레이어 ID
        */
        public Parallelogram(Rect rect, int layerId) : base(rect, layerId)
        {
            Handle = 0.2;
        }

        /**
        * @brief 개체의 종류를 리턴하는 가상함수
        */
        public override GraphicType ObjectType()
        {
            return GraphicType.GraphicParallelogram;
        }

        public void SetHandle(double handle)
        {
            if (handle < 0)
                handle = 0;
            if (handle > 1)
                handle = 1;
            Handle = handle;
            ClearPathGeometry();
        }

        /**
        * @brief 개체가 선택된 상태의 핸들을 출력하는 가상 함수.
        * @param drawingContext : 대상 Context
        */
        public override void RenderObjectHandle(DrawingContext drawingContext)
        {
            base.RenderObjectHandle(drawingContext);

            double move = Width * Handle;
            Point edge = new Point(Right() - move, Bottom());
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
            double move = Width * Handle;
            Point edge = new Point(Right() - move, Bottom());

            double handle_half = SELECTED_HANDLE_SIZE / 2;
            if ((Math.Abs(pt.X - edge.X) <= handle_half) &&
                (Math.Abs(pt.Y - edge.Y) <= handle_half))
                return EditHandleType.ObjectHandle1;

            return base.HitTestObjectsHandle(pt);
        }

        public override void OnRenderRubber(DrawingContext drawingContext, Brush brush, Pen pen, MouseDragMode mode, EditHandleType handleType, Point dragAmount, int keyState)
        {
            double edge_pt;
            Rect bound;

            if (handleType == EditHandleType.ObjectHandle1)
            {
                edge_pt = Width - Width * Handle;
                double handle = 1 - (edge_pt + dragAmount.X) / Width;
                if (handle < 0)
                    handle = 0;
                if (handle > 1)
                    handle = 1;

                string str = string.Format("new handle = {0}", handle);
                Console.WriteLine(str);

                edge_pt = Width * handle;
                bound = GetBounds();
            }
            else
            {
                bound = DragAction.MakeResizedRect(GetBounds(), mode, handleType, dragAmount);
                edge_pt = bound.Width * Handle;
            }

            StreamGeometry streamGeometry = new StreamGeometry();
            using (StreamGeometryContext geometryContext = streamGeometry.Open())
            {
                PointCollection points = new PointCollection();
                geometryContext.BeginFigure(new Point(bound.Left, bound.Bottom), true, true);
                points.Add(new Point(bound.Right - edge_pt, bound.Bottom));
                points.Add(new Point(bound.Right, bound.Top));
                points.Add(new Point(bound.Left + edge_pt, bound.Top));
                geometryContext.PolyLineTo(points, true, true);
            }

            drawingContext.DrawGeometry(brush, pen, streamGeometry);
        }

        public override Point GetResultRubber(EditHandleType handleType, Point dragAmount)
        {
            Point result = new Point(0, 0);
            double edge_pt = Width - Width * Handle;
            result.X = 1 - (edge_pt + dragAmount.X) / Width;

            if (result.X < 0)
                result.X = 0;
            if (result.X > 1)
                result.X = 1;
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
            if (handleType == EditHandleType.ObjectHandle1)
            {
                if (Handle != handle.X)
                {
                    Handle = handle.X;
                    ClearPathGeometry();
                    return true;
                }
            }
            return false;
        }

        public override void CreateDrawing(Canvas dc)
        {
            if (pathGeom == null)
            {
                PathFigure pathFigure = new PathFigure();

                pathFigure.StartPoint = new Point(X, Bottom());
                pathFigure.Segments.Add(new LineSegment(BottomLeft(), true));
                double move = Width * Handle;
                pathFigure.Segments.Add(new LineSegment(new Point(Right() - move, Bottom()), true));
                pathFigure.Segments.Add(new LineSegment(new Point(Right(), Y), true));
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
