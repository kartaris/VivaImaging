/**
* @file ScrollBox.cs
* @date 2017.05
* @brief PageBuilder for Windows ScrollBox class file
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
    * @class ScrollBox
    * @brief 내부 스크롤 기능을 가지는 Group 개체 클래스
    */
    public class ScrollBox : Group
    {
        /** 스크롤 오프셋과 스크롤 Width, Height */
        public Rect Scroll;

        /**
        * @brief ScrollBox class constructor
        */
        public ScrollBox() : base()
        {
        }

        /**
        * @brief ScrollBox class constructor
        * @param rect : 개체의 좌표
        * @param layerId : 개체의 소속 레이어 ID
        */
        public ScrollBox(Rect rect, int layerId) : base (rect, layerId)
        {
            Scroll = new Rect(new Point(0, 0), rect.Size);

        }

        /**
        * @brief 개체의 종류를 리턴하는 가상함수
        */
        public override GraphicType ObjectType()
        {
            return GraphicType.GraphicScrollBox;
        }

        /**
        * @brief 개체의 화면 출력을 위해 StackPanel에 Geometry를 생성하는 가상 함수.
        * @param dc : 대상 Panel
        * @details A. 
        * @n B. base class의 CreateDrawing()을 호출한다.
        */
        public override void CreateDrawing(Canvas dc)
        {
            // adjust scroll & clip
            base.CreateDrawing(dc);

            // draw label textbox
            // CreateDrawingText(dc);
        }

    }
}
