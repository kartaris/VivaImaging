/**
* @file Cylinder.cs
* @date 2017.06
* @brief PageBuilder for Windows Cylinder class file
*/
using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media;
using System.Windows.Shapes;
using System.Xml.Serialization;

namespace PageBuilder.Data
{

    [XmlInclude(typeof(BevelCylinder))]
    [XmlInclude(typeof(RevCylinder))]
    [XmlInclude(typeof(HourGlass))]
    /**
    * @class Cylinder 
    * @brief 옆으로 누운 실린더 개체 클래스
    */
    public class Cylinder : Graphic
    {
        /**
        * relative handle position prom BottomRight (0 ~ 0.5)
        */
        public double Handle;

        /**
        * @brief Cylinder  class constructor
        */
        public Cylinder() : base()
        {

        }
        /**
        * @brief Cylinder  class constructor
        * @param rect : 개체의 좌표
        * @param layerId : 개체의 소속 레이어 ID
        */
        public Cylinder(Rect rect, int layerId) : base (rect, layerId)
        {
            Line.Color = Color.FromArgb(0x00, 0, 0, 0);

            Handle = 0.25;
        }

        /**
        * @brief 개체의 종류를 리턴하는 가상함수
        */
        public override GraphicType ObjectType()
        {
            return GraphicType.GraphicCylinder;
        }

        /**
        * @brief 핸들의 위치값을 설정한다.
        * @param r : relative handle position prom BottomRight (0 ~ 0.5)
        */
        public void SetHandle(double r)
        {
            if (r < 0)
                r = 0;
            if (r > 0.5)
                r = 0.5;
            Handle = r;
            ClearPathGeometry();
        }

        /**
        * @brief 개체의 특수 핸들의 위치츷 리턴하는 가상 함수.
        * @param h : 핸들의 종류
        */
        public virtual Point CalculateHandlePosition(EditHandleType h)
        {
            double move = Width * Handle;
            return new Point(X + Width - move, Y + Height);
        }

        /**
        * @brief 개체가 선택된 상태의 핸들을 출력하는 가상 함수.
        * @param drawingContext : 대상 Context
        * @details A. base.RenderObjectHandle()으로 기본 핸들을 출력한다.
        * @n B. 오른쪽 아래 핸들의 위치를 계산하고 RenderSpecialHandle()으로 출력한다.
        */
        public override void RenderObjectHandle(DrawingContext drawingContext)
        {
            base.RenderObjectHandle(drawingContext);

            Point edge = CalculateHandlePosition(EditHandleType.ObjectHandle1);
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
        * @details A. 오른쪽 아래의 핸들 좌표를 계산하고,지정한 좌표가 핸들의 좌표 위에 있으면 ObjectHandle1 을 리턴한다.
        * @n B. base.HitTestObjectsHandle() 을 호출하여 기본 핸들의 마우스 HitTest를 행한다.
        */
        public override EditHandleType HitTestObjectsHandle(Point pt)
        {
            Point edge = CalculateHandlePosition(EditHandleType.ObjectHandle1);

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
        * @n C. 새로운 핸들값에 대한 round-rectangle 도형을 geometry로 생성하여 context에 출력한다.
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

                move = handle * Width;
                bound = GetBounds();
            }
            else
            {
                bound = DragAction.MakeResizedRect(GetBounds(), mode, handleType, dragAmount);
                move = bound.Width * Handle;
            }

            double radiusY = bound.Height / 2;

            RectangleGeometry rectangleGeometry = new RectangleGeometry(bound, move, radiusY);
            drawingContext.DrawGeometry(brush, pen, rectangleGeometry);
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
            double move = Width * Handle;
            double edge_pt = Width - Width * Handle;

            Point result = new Point(0, 0);
            result.X = 1 - (edge_pt + dragAmount.X) / Width;
            if (result.X < 0)
                result.X = 0;
            if (result.X > 0.5)
                result.X = 0.5;
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
                if (Handle != handle.X)
                {
                    Handle = handle.X;
                    ClearPathGeometry();
                    return true;
                }
            }
            return false;
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
                RectangleGeometry rg = new RectangleGeometry();
                rg.Rect = GetBounds();

                if (Handle > 0)
                {
                    rg.RadiusX = Handle * Width;
                    rg.RadiusY = Height / 2;
                }
                pathGeom = rg;
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
