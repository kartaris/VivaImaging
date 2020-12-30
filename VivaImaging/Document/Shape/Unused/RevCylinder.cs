/**
* @file RevCylinder.cs
* @date 2017.06
* @brief PageBuilder for Windows RevCylinder class file
*/
using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media;
using System.Windows.Shapes;

namespace PageBuilder.Data
{
    /**
    * @class Cylinder 
    * @brief 옆으로 누운 역-실린더 개체 클래스
    */
    public class RevCylinder : Cylinder
    {
        /**
        * @brief RevCylinder class constructor
        */
        public RevCylinder() : base()
        {
        }

        /**
        * @brief RevCylinder class constructor
        * @param rect : 개체의 좌표
        * @param layerId : 개체의 소속 레이어 ID
        */
        public RevCylinder(Rect rect, int layerId) : base(rect, layerId)
        {
            Line.Color = Color.FromArgb(0x00, 0, 0, 0);
            Handle = 0.25;
        }

        /**
        * @brief 개체의 종류를 리턴하는 가상함수
        */
        public override GraphicType ObjectType()
        {
            return GraphicType.GraphicRevCylinder;
        }

        /**
        * @brief 개체의 특수 핸들의 위치츷 리턴하는 가상 함수.
        * @param h : 핸들의 종류
        */
        public override Point CalculateHandlePosition(EditHandleType h)
        {
            double move = Width * Handle;
            return new Point(Right() - move, Y + Height / 2);
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

        /**
        * @brief 핸들의 좌표 이동에 따른 변경된 개체 외곽선을 출력하는 가상 함수.
        * @param drawingContext : 출력할 Context
        * @param brush : 출력할 brush
        * @param pen : 출력할 pen
        * @param mode : 마우스 드래그 모드
        * @param handleType : 이동중인 핸들 종류
        * @param dragAmount : 핸들을 이동한 양
        * @param keyState : Shift/Ctrl 키 누름에 의한 WITH_XY_SAME_RATIO 플래그
        * @details A. 현재 핸들의 위치를 계산하고, 이를 dragAmount 만큼 이동시킨 좌표에 대한 핸들값을 계산한다.
        * @n B. 핸들값의 최대 최소 범위를 설정하고, 결과 위치를 계산한다.
        * @n C. 새로운 핸들값에 대한 도형을 geometryContext으로 생성하여 context에 출력한다.
        */
        public override void OnRenderRubber(DrawingContext drawingContext, Brush brush, Pen pen, MouseDragMode mode, EditHandleType handleType, Point dragAmount, int keyState)
        {
            double move;
            Rect bound;

            if (handleType == EditHandleType.ObjectHandle1)
            {
                double edge_pt = Width - Width * Handle;
                double handle = 1 - (edge_pt + dragAmount.X) / Width;

                if (handle < 0)
                    handle = 0;
                if (handle > 0.5)
                    handle = 0.5;

                string str = string.Format("new handle = {0}", handle);
                Console.WriteLine(str);
                move = Width * handle;
                bound = GetBounds();
            }
            else
            {
                bound = DragAction.MakeResizedRect(GetBounds(), mode, handleType, dragAmount);
                move = bound.Width * Handle;
            }

            double dx = move / 1.74;
            double dy = (bound.Height / 2) / 1.74;

            StreamGeometry streamGeometry = new StreamGeometry();
            using (StreamGeometryContext geometryContext = streamGeometry.Open())
            {
                geometryContext.BeginFigure(new Point(bound.Left, bound.Bottom), true, true);

                geometryContext.LineTo(new Point(bound.Right, bound.Bottom), true, true);

                geometryContext.BezierTo(
                    new Point(bound.Right - dx, bound.Bottom),
                    new Point(bound.Right - move, bound.Top + bound.Height / 2 + dy),
                    new Point(bound.Right - move, bound.Top + bound.Height / 2), true, true);

                geometryContext.BezierTo(
                    new Point(bound.Right - move, bound.Top + bound.Height / 2 - dy),
                    new Point(bound.Right - dx, bound.Top),
                    new Point(bound.Right, bound.Top), true, true);

                geometryContext.LineTo(new Point(bound.Left, bound.Top), true, true);

                geometryContext.BezierTo(
                    new Point(bound.Left + dx, bound.Top),
                    new Point(bound.Left + move, bound.Top + bound.Height / 2 - dy),
                    new Point(bound.Left + move, bound.Top + bound.Height / 2), true, true);
                // pf.Segments.Add(new LineSegment(new Point(Bounds.Left + move, Bounds.Top + Bounds.Height / 2), true));

                geometryContext.BezierTo(
                    new Point(bound.Left + move, bound.Top + bound.Height / 2 + dy),
                    new Point(bound.Left + dx, bound.Bottom),
                    new Point(bound.Left, bound.Bottom), true, true);
            }
            drawingContext.DrawGeometry(brush, pen, streamGeometry);
        }

        /**
        * @brief 개체의 화면 출력을 위해 StackPanel에 Geometry를 생성하는 가상 함수.
        * @param dc : 대상 Panel
        * @details A. RectangleGeometry를 생성하여 좌표를 설정한다.
        * @n B. Path 개체를 생성하고 채우기 및 외곽선 속성을 설정하고 Data로 RectangleGeometry를 설정한다.
        * @n C. StackPanel의 child로 추가한다.
        * @n D. base 클래스의 CreateDrawing()을 호출하여 선택 핸들을 출력한다.
        */
        public override void CreateDrawing(Canvas dc)
        {
            if (pathGeom == null)
            {
                PathFigure pf = new PathFigure();

                double move = Width * Handle;
                pf.StartPoint = BottomLeft();
                pf.Segments.Add(new LineSegment(BottomRight(), true));
                double dx = move / 1.74;
                double dy = (Height / 2) / 1.74;
                pf.Segments.Add(new BezierSegment(
                    new Point(Right() - dx, Bottom()),
                    new Point(Right() - move, Y + Height / 2 + dy),
                    new Point(Right() - move, Y + Height / 2), true));
                // pf.Segments.Add(new LineSegment(new Point(Bounds.Right - move, Bounds.Top + Bounds.Height / 2), true));

                pf.Segments.Add(new BezierSegment(
                    new Point(Right() - move, Y + Height / 2 - dy),
                    new Point(Right() - dx, Y),
                    new Point(Right(), Y), true));
                // pf.Segments.Add(new LineSegment(new Point(Bounds.Right, Bounds.Top), true));

                pf.Segments.Add(new LineSegment(TopLeft(), true));

                pf.Segments.Add(new BezierSegment(
                    new Point(X + dx, Y),
                    new Point(X + move, Y + Height / 2 - dy),
                    new Point(X + move, Y + Height / 2), true));
                // pf.Segments.Add(new LineSegment(new Point(Bounds.Left + move, Bounds.Top + Bounds.Height / 2), true));

                pf.Segments.Add(new BezierSegment(
                    new Point(X + move, Y + Height / 2 + dy),
                    new Point(X + dx, Bottom()),
                    new Point(X, Bottom()), true));

                pf.IsClosed = true;

                pathGeom = new PathGeometry();
                ((PathGeometry)pathGeom).Figures.Add(pf);
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
