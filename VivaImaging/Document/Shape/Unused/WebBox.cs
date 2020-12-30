/**
* @file WebBox.cs
* @date 2017.05
* @brief PageBuilder for Windows WebBox class file
*/
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;

namespace PageBuilder.Data
{
    /**
    * @class WebBox
    * @brief Web 개체 클래스
    */
    public class WebBox : Graphic
    {
        public string Url;

        /**
        * @brief WebBox  class constructor
        */
        public WebBox() : base()
        {
        }

        /**
        * @brief WebBox  class constructor
        * @param rect : 개체의 좌표
        * @param layerId : 개체의 소속 레이어 ID
        */
        public WebBox(Rect rect, int layerId) : base (rect, layerId)
        {

        }

        /**
        * @brief 개체의 종류를 리턴하는 가상함수
        */
        public override GraphicType ObjectType()
        {
            return GraphicType.GraphicWebBox;
        }

        public void SetUrl(string url)
        {
            Url = url;
        }
        public string GetUrl()
        {
            return Url;
        }

    }
}
