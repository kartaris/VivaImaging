/**
* @file Pentagon.cs
* @date 2017.06
* @brief PageBuilder for Windows Pentagon class file
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

namespace PageBuilder.Data
{
    /**
    * @class Pentagon 
    * @brief 오각형 개체 클래스
    */
    public class Pentagon : Graphic
    {
        /**
        * @brief Pentagon class constructor
        */
        public Pentagon() : base()
        {
        }

        /**
        * @brief Pentagon class constructor
        * @param rect : 개체의 좌표
        * @param layerId : 개체의 소속 레이어 ID
        */
        public Pentagon(Rect rect, int layerId) : base(rect, layerId)
        {
            Line.Color = Color.FromArgb(0x00, 0, 0, 0);
        }
        /**
        * @brief 개체의 종류를 리턴하는 가상함수
        */
        public override GraphicType ObjectType()
        {
            return GraphicType.GraphicCylinder;
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

        public override void OnRenderRubber(DrawingContext drawingContext, Brush brush, Pen pen, MouseDragMode mode, EditHandleType handleType, Point dragAmount, int keyState)
        {
            Rect bound = DragAction.MakeResizedRect(GetBounds(), mode, handleType, dragAmount);

            StreamGeometry streamGeometry = new StreamGeometry();
            using (StreamGeometryContext geometryContext = streamGeometry.Open())
            {
                PointCollection points = new PointCollection();
                double xr = bound.Width * 0.194;
                double yr = bound.Height * 0.384;

                geometryContext.BeginFigure(new Point(bound.Right - xr, bound.Bottom), true, true);
                points.Add(new Point(bound.Right, bound.Top + yr));
                points.Add(new Point(bound.Left + bound.Width / 2, bound.Top));
                points.Add(new Point(bound.Left, bound.Top + yr));
                points.Add(new Point(bound.Left + xr, bound.Bottom));
                geometryContext.PolyLineTo(points, true, true);
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
                PathFigure pathFigure = new PathFigure();

                double xr = Width * 0.194;
                double yr = Height * 0.384;
                pathFigure.StartPoint = new Point(Right() - xr, Bottom());
                pathFigure.Segments.Add(new LineSegment(new Point(Right(), Y + yr), true));
                pathFigure.Segments.Add(new LineSegment(new Point(X + Width / 2, Y), true));
                pathFigure.Segments.Add(new LineSegment(new Point(X, Y + yr), true));
                pathFigure.Segments.Add(new LineSegment(new Point(X + xr, Bottom()), true));
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
