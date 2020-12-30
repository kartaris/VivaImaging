/**
* @file Triangle.cs
* @date 2017.05
* @brief PageBuilder for Windows Triangle class file
*/
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Media;
using System.Windows.Shapes;
using System.Windows.Controls;

namespace PageBuilder.Data
{
    /**
    * @class Triangle
    * @brief Triangle 개체 클래스
    */
    public class Triangle : Graphic
    {

        public enum DirectionType
        {
            Top,
            Left,
            Right,
            Bottom
        };

        /**
         * Edge direction
         */
        public DirectionType Direction;
        /**
         * Relative movement of Edge (0 to 1)
         */
        public double Handle;

        /**
        * @brief Triangle class constructor
        */
        public Triangle() : base()
        {
        }

        /**
        * @brief Triangle class constructor
        * @param rect : 개체의 좌표
        * @param layerId : 개체의 소속 레이어 ID
        */
        public Triangle(Rect rect, int layerId) : base (rect, layerId)
        {
            Direction = DirectionType.Top;
            Handle = 0.5;
        }

        /**
        * @brief 개체의 종류를 리턴하는 가상함수
        */
        public override GraphicType ObjectType()
        {
            return GraphicType.GraphicTriangle;
        }

        public void SetDirection(DirectionType type)
        {
            Direction = type;
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

        Point getEdgePoint()
        {
            Point edge;

            if (Direction == DirectionType.Top)
            {
                double edge_pt = Handle * Width;
                edge = new Point((X + edge_pt), Y);
            }
            else if (Direction == DirectionType.Right)
            {
                double edge_pt = Handle * Height;
                edge = new Point(Right(), (Y + edge_pt));
            }
            else if (Direction == DirectionType.Left)
            {
                double edge_pt = Handle * Height;
                edge = new Point(X, (Y + edge_pt));
            }
            else // if (Direction == DirectionType.Bottom)
            {
                double edge_pt = Handle * Width;
                edge = new Point((X + edge_pt), Bottom());
            }
            return edge;
        }

        /**
        * @brief 개체가 선택된 상태의 핸들을 출력하는 가상 함수.
        * @param drawingContext : 대상 Context
        * @details A. base.RenderObjectHandle()으로 기본 핸들을 출력한다.
        * @n B. 꼭지점 핸들의 위치를 계산하고 RenderSpecialHandle()으로 출력한다.
        */
        public override void RenderObjectHandle(DrawingContext drawingContext)
        {
            base.RenderObjectHandle(drawingContext);

            Point edge = getEdgePoint();
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

        /**
        * @brief 개체가 선택된 상태의 핸들에 마우스 HitTest를 행하는 가상 함수.
        * @param pt : 대상 좌표
        * @details A. 꼭지점 핸들 좌표를 계산하고,지정한 좌표가 핸들의 좌표 위에 있으면 ObjectHandle1 을 리턴한다.
        * @n B. base.HitTestObjectsHandle() 을 호출하여 기본 핸들의 마우스 HitTest를 행한다.
        */
        public override EditHandleType HitTestObjectsHandle(Point pt)
        {
            Point edge = getEdgePoint();

            double handle_half = SELECTED_HANDLE_SIZE / 2;
            if ((Math.Abs(pt.X - edge.X) <= handle_half) &&
                (Math.Abs(pt.Y - edge.Y) <= handle_half))
            {
                // origin = edge;
                return EditHandleType.ObjectHandle1;
            }
            return base.HitTestObjectsHandle(pt);
        }

        /**
        * @brief 핸들의 좌표 이동에 따른 변경된 개체 외곽선을 출력하는 가상 함수.
        * @param drawingContext : 출력할 Context
        * @param brush : 출력할 brush
        * @param pen : 출력할 pen
        * @param handleType : 이동중인 핸들 종류
        * @param dragAmount : 핸들을 이동한 양
        * @param keyState : Shift/Ctrl 키 누름에 의한 WITH_XY_SAME_RATIO 플래그
        * @details A. 현재 핸들의 위치를 계산하고, 이를 dragAmount 만큼 이동시킨 좌표에 대한 핸들값을 계산한다.
        * @n B. 핸들값의 최대 최소 범위를 설정하고, 결과 위치를 계산한다.
        * @n C. 새로운 핸들값에 대한 도형을 geometryContext으로 생성하여 context에 출력한다.
        */
        public override void OnRenderRubber(DrawingContext drawingContext, Brush brush, Pen pen, MouseDragMode mode, EditHandleType handleType, Point dragAmount, int keyState)
        {
            StreamGeometry streamGeometry = new StreamGeometry();
            double edge_pt;
            Rect bound = GetBounds();

            if (handleType == EditHandleType.ObjectHandle1)
            {
                if ((Direction == DirectionType.Top) || (Direction == DirectionType.Bottom))
                {
                    edge_pt = Handle * Width;
                    double handle = (edge_pt + dragAmount.X) / Width;
                    if (handle < 0)
                        handle = 0;
                    if (handle > 1)
                        handle = 1;

                    string str = string.Format("new handle = {0}", handle);
                    Console.WriteLine(str);
                    edge_pt = handle * Width;
                }
                else
                {
                    edge_pt = Handle * Height;
                    double handle = (edge_pt + dragAmount.Y) / Height;
                    if (handle < 0)
                        handle = 0;
                    if (handle > 1)
                        handle = 1;

                    string str = string.Format("new handle = {0}", handle);
                    Console.WriteLine(str);
                    edge_pt = handle * Height;
                }
            }
            else
            {
                bound = DragAction.MakeResizedRect(GetBounds(), mode, handleType, dragAmount);

                if ((Direction == DirectionType.Top) || (Direction == DirectionType.Bottom))
                    edge_pt = Handle * bound.Width;
                else
                    edge_pt = Handle * bound.Height;
            }

            using (StreamGeometryContext geometryContext = streamGeometry.Open())
            {
                PointCollection points = new PointCollection();
                if (Direction == DirectionType.Top)
                {
                    geometryContext.BeginFigure(new Point(bound.Left + edge_pt, bound.Top), true, true);
                    points.Add(bound.BottomLeft);
                    points.Add(bound.BottomRight);
                }
                else if (Direction == DirectionType.Right)
                {
                    geometryContext.BeginFigure(new Point(bound.Right, (bound.Top + edge_pt)), true, true);
                    points.Add(bound.BottomLeft);
                    points.Add(bound.TopLeft);
                }
                else if (Direction == DirectionType.Left)
                {
                    geometryContext.BeginFigure(new Point(bound.Left, (bound.Top + edge_pt)), true, true);
                    points.Add(bound.TopRight);
                    points.Add(bound.BottomRight);
                }
                else // if (Direction == DirectionType.Bottom)
                {
                    geometryContext.BeginFigure(new Point((bound.Left + edge_pt), bound.Bottom), true, true);
                    points.Add(bound.TopLeft);
                    points.Add(bound.TopRight);
                }
                geometryContext.PolyLineTo(points, true, true);
            }

            drawingContext.DrawGeometry(brush, pen, streamGeometry);
        }

        public override Point GetResultRubber(EditHandleType handleType, Point dragAmount)
        {
            Point result = new Point(0, 0);
            if ((Direction == DirectionType.Top) || (Direction == DirectionType.Bottom))
            {
                double edge_pt = Handle * Width;
                result.X = (edge_pt + dragAmount.X) / Width;
            }
            else
            {
                double edge_pt = Handle * Height;
                result.X = (edge_pt + dragAmount.Y) / Height;
            }
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
                if ((Direction == DirectionType.Top) || (Direction == DirectionType.Bottom))
                {
                    if (Handle != handle.X)
                    {
                        Handle = handle.X;
                        ClearPathGeometry();
                        return true;
                    }
                }
                else
                {
                    if (Handle != handle.Y)
                    {
                        Handle = handle.Y;
                        ClearPathGeometry();
                        return true;
                    }
                }
            }
            return false;
        }

        /**
        * @brief 개체의 화면 출력을 위해 StackPanel에 Geometry를 생성하는 가상 함수.
        * @param dc : 대상 Panel
        * @details A. 삼각형을 구성하는 PathFigure를 생성하고 선분을 추가하고, close 한다.
        * @n B. PathGeometry 개체를 생성하고 Figures에 추가한다.
        * @n B. Path 개체를 생성하고 채우기 및 외곽선 속성을 설정하고 Data로 PathGeometry를 설정한다.
        * @n C. StackPanel의 child로 추가한다.
        * @n D. base 클래스의 CreateDrawing()을 호출하여 선택 핸들을 출력한다.
        */
        public override void CreateDrawing(Canvas dc)
        {
            if (pathGeom == null)
            {
                PathFigure pf = new PathFigure();

                if (Direction == DirectionType.Top)
                {
                    double edge_pt = Handle * Width;
                    pf.StartPoint = new Point((X + edge_pt), Y);
                    pf.Segments.Add(new LineSegment(BottomLeft(), true));
                    pf.Segments.Add(new LineSegment(BottomRight(), true));
                }
                else if (Direction == DirectionType.Right)
                {
                    double edge_pt = Handle * Height;
                    pf.StartPoint = new Point(Right(), (Y + edge_pt));
                    pf.Segments.Add(new LineSegment(BottomLeft(), true));
                    pf.Segments.Add(new LineSegment(TopLeft(), true));
                }
                else if (Direction == DirectionType.Left)
                {
                    double edge_pt = Handle * Height;
                    pf.StartPoint = new Point(X, (Y + edge_pt));
                    pf.Segments.Add(new LineSegment(TopRight(), true));
                    pf.Segments.Add(new LineSegment(BottomRight(), true));
                }
                else // if (Direction == DirectionType.Bottom)
                {
                    double edge_pt = Handle * Width;
                    pf.StartPoint = new Point((X + edge_pt), Bottom());
                    pf.Segments.Add(new LineSegment(TopLeft(), true));
                    pf.Segments.Add(new LineSegment(TopRight(), true));
                }

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
