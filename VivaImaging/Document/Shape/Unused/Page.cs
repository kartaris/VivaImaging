/**
* @file Page.cs
* @date 2017.05
* @brief PageBuilder for Windows Page class file
*/

using System.Windows;
using System.Windows.Media;
using System.Windows.Shapes;
using System.Windows.Controls;
using System.Xml.Serialization;

namespace PageBuilder.Data
{
    /**
    * @class Page
    * @brief 페이지 정보를 나타내는 클래스
    */
    public class Page
    {
        public static double PageBorderWidth = 1;

        public Rect Bounds;

        public static void SetPageBorderWidth(double width)
        {
            PageBorderWidth = width;
        }

        /**
        * @brief Page class constructor
        */
        public Page()
        {

        }

        /**
        * @brief Page class constructor
        * @param rect : 페이지의 좌표
        */
        public Page(Rect rect)
        {
            Bounds = rect;
        }

        /**
        * @brief 페이지 테두리를 화면 출력을 위해 StackPanel에 Geometry를 생성하는 가상 함수.
        * @param dc : 대상 Panel
        * @details A. RectangleGeometry를 생성하고 페이지 좌표를 Rect로 설정한다.
        * @n B. Path 개체를 생성하고 채우기는 흰색 브러시, 외곽선은 LightGray로, Data는 RectangleGeometry로 설정한다.
        * @n C. StackPanel의 child로 추가한다.
        */
        public void CreateDrawing(Canvas dc)
        {
            RectangleGeometry rg = new RectangleGeometry();
            rg.Rect = Bounds;

            Path path = new Path();
            //path.Fill = Brushes.White;
            path.Stroke = Brushes.Black; // new SolidColorBrush(Line.Color);
            path.StrokeThickness = PageBorderWidth;
            path.Data = rg;

            dc.Children.Add(path);
        }

    }

}
