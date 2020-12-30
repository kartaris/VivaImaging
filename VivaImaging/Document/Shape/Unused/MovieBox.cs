/**
* @file MovieBox.cs
* @date 2017.05
* @brief PageBuilder for Windows MovieBox class file
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
    * @class MovieBox
    * @brief Movie 개체 클래스
    */
    public class MovieBox : Graphic
    {
        /** 동영상 파일의 경로명 */
        public string Url = null;

        /**
        * @brief MovieBox class constructor
        */
        public MovieBox() : base()
        {
        }

        /**
        * @brief MovieBox class constructor
        * @param rect : 개체의 좌표
        * @param layerId : 개체의 소속 레이어 ID
        */
        public MovieBox(Rect rect, int layerId) : base (rect, layerId)
        {

        }

        /**
        * @brief 개체의 종류를 리턴하는 가상함수
        */
        public override GraphicType ObjectType()
        {
            return GraphicType.GraphicMovieBox;
        }

    }
}
