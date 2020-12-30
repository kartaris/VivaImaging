/**
* @file OutLine.cs
* @date 2017.05
* @brief PageBuilder for Windows OutLine class file
*/

using System.Windows.Media;

namespace PageBuilder.Data
{
    /**
    * @class OutLine
    * @brief 개체의 외곽선 정보를 나타내는 클래스
    */
    public class OutLine
    {
        /** 외곽선 색상 */
        public Color Color;
        /** 외곽선 드로잉 타입 */
        public Graphic.GraphDrawType LineType;
        /** 외곽선 두께 */
        public int Width;
        /** 외곽선 Join 속성 */
        public PenLineJoin Join;
        /** 외곽선 시작점 Cap 속성 */
        public PenLineCap StartCap;
        /** 외곽선 끝점 Cap 속성 */
        public PenLineCap EndCap;

        //int Position;
        //float Dash;

        /**
        * @brief Outline class constructor : Graphic 개체의 아웃라인을 정의한다
        */
        public OutLine()
        {

        }

        public OutLine(Graphic.GraphDrawType type)
        {
            LineType = type;
        }

        public OutLine(Graphic.GraphDrawType type, Color color, int width)
        {
            LineType = type;
            Color = color;
            Width = width;
        }

        public OutLine(Graphic.GraphDrawType type, Color color, int width, PenLineJoin join, PenLineCap startCap, PenLineCap endCap)
        {
            LineType = type;
            Color = color;
            Width = width;
            Join = join;
            StartCap = startCap;
            EndCap = endCap;
        }
    }
}
