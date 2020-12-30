/**
* @file BezierPath.cs
* @date 2017.06
* @brief PageBuilder for Windows BezierPath class file
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
    * @class BezierPath 
    * @brief Bezier segment 개체 클래스
    */
    public class BezierPath : Graphic
    {
        /**
        * point array
        */
        List<Point> BezierPoints;

        /**
        * closed or opened
        */
        public bool Closed = false;

        /**
        * @brief BezierPath  class constructor
        */
        public BezierPath() : base()
        {

        }

        /**
        * @brief BezierPath  class constructor
        * @param sp : Bezier curve 시작점
        * @param ep : Bezier curve 끝점
        * @param layerId : 개체의 소속 레이어 ID
        */
        public BezierPath(Point sp, Point ep, int layerId) : base(RectFromTwoPoints(sp, ep), layerId)
        {
            Line.Color = Color.FromArgb(0x00, 0, 0, 0);
            BezierPoints = new List<Point>();
            BezierPoints.Add(sp);

            double sx = sp.X;
            if (ep.X != sp.X)
                sx += (ep.X - sp.X) / 4;
            double sy = sp.Y;
            if (ep.Y != sp.Y)
                sy += (ep.Y - sp.Y) / 4;
            BezierPoints.Add(new Point(sx, sy));

            sx = ep.X;
            if (ep.X != sp.X)
                sx += (sp.X - ep.X) / 4;
            sy = ep.Y;
            if (ep.Y != sp.Y)
                sy += (sp.Y - ep.Y) / 4;
            BezierPoints.Add(new Point(sx, sy));

            BezierPoints.Add(ep);
        }

        public override GraphicType ObjectType()
        {
            return GraphicType.GraphicBezierPath;
        }

        public override Point GetStartPoint()
        {
            return BezierPoints[0];
        }

        public override Point GetEndPoint()
        {
            return BezierPoints[BezierPoints.Count()-1];
        }

        /**
        * @brief 개체의 특수 핸들의 위치츷 리턴하는 가상 함수.
        * @param h : 핸들의 종류
        */
        public virtual Point CalculateHandlePosition(EditHandleType h)
        {
            if (EditHandleType.ObjectHandle1 == h)
                return BezierPoints[1];
            else if (EditHandleType.ObjectHandle2 == h)
                return BezierPoints[2];

            return BezierPoints[0];
        }

        /**
        * @brief 개체가 선택된 상태의 핸들을 출력하는 가상 함수.
        * @param drawingContext : 대상 Context
        * @details A. base.RenderObjectHandle()으로 기본 핸들을 출력한다.
        * @n B. 오른쪽 아래 핸들의 위치를 계산하고 RenderSpecialHandle()으로 출력한다.
        */
        public override void RenderObjectHandle(DrawingContext drawingContext)
        {
            // draw handle 
            Pen handlePen = new Pen(new SolidColorBrush(RESIZE_HANDLE_OUTLINE), BASIC_PEN_WIDTH);
            drawingContext.DrawLine(handlePen, BezierPoints[0], BezierPoints[1]);
            drawingContext.DrawLine(handlePen, BezierPoints[2], BezierPoints[3]);

            base.RenderObjectHandle(drawingContext);

            Point edge = CalculateHandlePosition(EditHandleType.ObjectHandle1);
            RenderSpecialHandle(drawingContext, edge);
            edge = CalculateHandlePosition(EditHandleType.ObjectHandle2);
            RenderSpecialHandle(drawingContext, edge);
        }

        public override void RenderObjectHandle(DrawingContext drawingContext, Pen handlePen, Brush brush)
        {
            drawingContext.DrawEllipse(brush, handlePen, BezierPoints[0], SELECTED_HANDLE_SIZE, SELECTED_HANDLE_SIZE);
            drawingContext.DrawEllipse(brush, handlePen, BezierPoints[3], SELECTED_HANDLE_SIZE, SELECTED_HANDLE_SIZE);
        }

        /**
        * @brief 지정한 좌표에 개체가 위치하는지 체크하는 가상 함수.
        * @param pt : 지정한 좌표
        * @return bool : 위치가 해당되면 true를 리턴한다.
        */
        public override bool HitTestOver(Point pt)
        {
            return pathGeom.StrokeContains(GetPen(), pt, 2, ToleranceType.Absolute);
        }

        /**
        * @brief 지정한 좌표에 개체의 핸들이 있으면 그 핸들 형식을 리턴하는 가상 함수.
        * @param pt : 지정한 좌표
        * @return EditHandleType : 핸들 형식
        */
        public override EditHandleType HitTestObjectsHandle(Point pt)
        {
            if (BezierPoints.Count() > 0)
            {
                double handle_half = SELECTED_HANDLE_SIZE / 2;
                int index = 0;

                foreach (Point bz in BezierPoints)
                {
                    if ((Math.Abs(pt.X - bz.X) <= handle_half) &&
                        (Math.Abs(pt.Y - bz.Y) <= handle_half))
                    {
                        int offset = (index % 4);
                        if (offset == 0)
                            return EditHandleType.ResizeLeftTop;
                        else if (offset == 1)
                            return EditHandleType.ObjectHandle1;
                        else if (offset == 2)
                            return EditHandleType.ObjectHandle2;
                        else
                            return EditHandleType.ResizeRightBottom;
                    }
                    ++index;
                }
            }

            return EditHandleType.None;
        }

        /**
        * @brief 개체의 위치를 지정한 만큼 이동한다.
        * @param offset : 이동할 양
        * @return bool : 성공적으로 이동되면 true를 리턴한다.
        */
        public override bool Move(Vector offset)
        {
            for (int i = 0; i < BezierPoints.Count();  i++)
            {
                BezierPoints[i] = Point.Add(BezierPoints[i], offset);
            }

            int e = BezierPoints.Count() - 1;
            SetBounds(RectFromTwoPoints(BezierPoints[0], BezierPoints[e]));
            ClearPathGeometry();
            return true;
        }

        /**
        * @brief 핸들의 이동에 따라 개체의 크기를 변경하는 가상 함수.
        * @param type : 핸들 종류
        * @return offset : 핸들의 이동량
        */
        public override bool ResizeByObjectHandle(EditHandleType type, Point offset)
        {
            if ((offset.X != 0) || (offset.Y != 0))
            {
                int e = BezierPoints.Count() - 1;
                if (type == EditHandleType.ResizeLeftTop)
                {
                    BezierPoints[0] = Point.Add(BezierPoints[0], (Vector)offset);
                    BezierPoints[1] = Point.Add(BezierPoints[1], (Vector)offset);
                }
                else if (type == EditHandleType.ResizeRightBottom)
                {
                    BezierPoints[e] = Point.Add(BezierPoints[e], (Vector)offset);
                    BezierPoints[e-1] = Point.Add(BezierPoints[e-1], (Vector)offset);
                    // BezierPoints.ElementAt(e - 1).Offset(offset.X, offset.Y);
                }
                /*
                else if (type == EditHandleType.ObjectHandle1)
                {
                    BezierPoints.ElementAt(1).Offset(offset.X, offset.Y);
                }
                else if (type == EditHandleType.ObjectHandle2)
                {
                    BezierPoints.ElementAt(2).Offset(offset.X, offset.Y);
                }
                */
                SetBounds(RectFromTwoPoints(BezierPoints[0], BezierPoints[e]));
                ClearPathGeometry();
                return true;
            }
            return false;
        }

        /**
        * @brief 핸들의 좌표 이동에 따른 변경된 개체 외곽선을 출력하는 가상 함수.
        * @param drawingContext : 출력할 Context
        * @param brush : 출력할 brush
        * @param pen : 출력할 pen
        * @param mode : 마우스 드래깅 모드
        * @param handleType : 이동중인 핸들 종류
        * @param dragAmount : 핸들을 이동한 양
        * @param keyState : Shift/Ctrl 키 누름에 의한 WITH_XY_SAME_RATIO 플래그
        * @details A. 현재 핸들의 위치를 계산하고, 이를 dragAmount 만큼 이동시킨 좌표에 대한 핸들값을 계산한다.
        * @n B. 핸들값의 최대 최소 범위를 설정하고, 결과 위치를 계산한다.
        * @n C. 새로운 핸들값에 대한 round-rectangle 도형을 geometry로 생성하여 context에 출력한다.
        */
        public override void OnRenderRubber(DrawingContext drawingContext, Brush brush, Pen pen, MouseDragMode mode, EditHandleType handleType, Point dragAmount, int keyState)
        {
            StreamGeometry streamGeometry = new StreamGeometry();

            if ((handleType == EditHandleType.ObjectHandle1) ||
                (handleType == EditHandleType.ObjectHandle2))
            {
                Point sh;
                Point eh;

                sh = new Point(BezierPoints[1].X, BezierPoints[1].Y);
                eh = new Point(BezierPoints[2].X, BezierPoints[2].Y);

                if (handleType == EditHandleType.ObjectHandle1)
                {
                    sh.Offset(dragAmount.X, dragAmount.Y);
                }
                else if (handleType == EditHandleType.ObjectHandle2)
                {
                    eh.Offset(dragAmount.X, dragAmount.Y);
                }
                using (StreamGeometryContext geometryContext = streamGeometry.Open())
                {
                    geometryContext.BeginFigure(sh, false, false);

                    geometryContext.LineTo(BezierPoints[0], true, true);

                    geometryContext.BezierTo(
                        sh,
                        eh,
                        BezierPoints[3], true, true);

                    geometryContext.LineTo(eh, true, true);
                }
            }
            else if ((handleType == EditHandleType.ResizeLeftTop) ||
                (handleType == EditHandleType.ResizeRightBottom) ||
                (handleType == EditHandleType.MoveObject))
            {
                Point[] curve = new Point[4] { BezierPoints[0], BezierPoints[1], BezierPoints[2], BezierPoints[3] };

                if ((handleType == EditHandleType.ResizeLeftTop) ||
                    (handleType == EditHandleType.MoveObject))
                {
                    for (int i = 0; i < 2; i++)
                    {
                        curve[i].Offset(dragAmount.X, dragAmount.Y);
                    }
                }

                if ((handleType == EditHandleType.ResizeRightBottom) ||
                    (handleType == EditHandleType.MoveObject))
                {
                    for (int i = 2; i < 4; i++)
                    {
                        curve[i].Offset(dragAmount.X, dragAmount.Y);
                    }
                }

                using (StreamGeometryContext geometryContext = streamGeometry.Open())
                {
                    geometryContext.BeginFigure(curve[0], false, false);
                    geometryContext.BezierTo(
                        curve[1],
                        curve[2],
                        curve[3], true, true);
                }
            }

            drawingContext.DrawGeometry(brush, pen, streamGeometry);
        }

        /**
        * @brief 핸들의 좌표 이동에 따른 변경된 핸들값을 계산하여 리턴하는 가상 함수.
        * @param handleType : 이동중인 핸들 종류
        * @param dragAmount : 핸들을 이동한 양
        * @details A. 현재 핸들의 위치를 계산하고, 이를 dragAmount 만큼 이동시킨 좌표에 대한 핸들값을 계산한다.
        * @n B. 핸들값의 최대 최소 범위를 설정하고 결과를 리턴한다.
        */
        public override Point GetResultRubber(EditHandleType handleType, Point dragAmount)
        {
            Point result;

            if (handleType == EditHandleType.ObjectHandle1)
                result = new Point(BezierPoints[1].X, BezierPoints[1].Y);
            else if (handleType == EditHandleType.ObjectHandle2)
                result = new Point(BezierPoints[2].X, BezierPoints[2].Y);
            else if ((handleType == EditHandleType.ResizeLeftTop) ||
                (handleType == EditHandleType.ResizeRightBottom) ||
                (handleType == EditHandleType.MoveObject))
                return dragAmount;
            else
                result = new Point(BezierPoints[0].X, BezierPoints[0].Y);

            result.Offset(dragAmount.X, dragAmount.Y);
            return result;
        }

        /**
        * @brief 개체별 특수 핸들을 드롭하였을때, 결과값을 Handle에 적용하는 가상 함수.
        * @param handleType : 드래깅하는 핸들의 종류
        * @param handle : 드래깅에 의해 변동된 handle값
        * @param keyState : Shift/Ctrl 키 상태값
        * @return bool : 값이 정상적으로 설정되었으면 true를 리턴한다.
        */
        public override bool HandleEdit(EditHandleType handleType, Point handle, int keyState)
        {
            if (handleType == EditHandleType.ObjectHandle1)
            {
                if (BezierPoints[1] != handle)
                {
                    BezierPoints[1] = handle;
                    return true;
                }
            }
            else if (handleType == EditHandleType.ObjectHandle2)
            {
                if (BezierPoints[2] != handle)
                {
                    BezierPoints[2] = handle;
                    return true;
                }
            }
            else if (handleType == EditHandleType.MoveObject)
            {
                foreach (Point pt in BezierPoints)
                {
                    pt.Offset(handle.X, handle.Y);
                }
                return true;
            }
            else if ((handleType == EditHandleType.ResizeLeftTop) ||
                (handleType == EditHandleType.ResizeRightBottom))
            {
                int offset = (handleType == EditHandleType.ResizeLeftTop) ? 0 : 2;
                for (int i = 0; i < 2; i++)
                {
                    BezierPoints.ElementAt(i+offset).Offset(handle.X, handle.Y);
                }
            }
            return false;
        }


        /**
        * @brief 개체의 화면 출력을 위해 StackPanel에 Geometry를 생성하는 가상 함수.
        * @param dc : 대상 Panel
        * @details A. Path 개체를 생성하고 외곽선 속성을 설정한다.
        * @n B. PathGeometry 생성하여 Path의 Data로 설정한다.
        * @n C. StackPanel의 child로 추가한다.
        * @n D. base 클래스의 CreateDrawing()을 호출하여 선택 핸들을 출력한다.
        */
        public override void CreateDrawing(Canvas dc)
        {
            if (pathGeom == null)
            {
                pathGeom = new PathGeometry();

                PathFigure pathFigure = new PathFigure();
                pathFigure.StartPoint = BezierPoints[0];
                pathFigure.Segments.Add(new BezierSegment(
                    BezierPoints[1],
                    BezierPoints[2],
                    BezierPoints[3], true));

                ((PathGeometry)pathGeom).Figures.Add(pathFigure);
            }

            Path path = new Path();
            GetFillStroke(ref path, PATH_STROKE);
            path.Data = pathGeom;

            dc.Children.Add(path);
        }

    }
}
