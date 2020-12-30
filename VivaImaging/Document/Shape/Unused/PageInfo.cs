/**
* @file PageInfo.cs
* @date 2017.07
* @brief PageBuilder for Windows PageInfo class file
*/
using System.Windows.Media;

namespace PageBuilder.Data
{
    /**
    * @class PageInfo
    * @brief 스프레드의 세부 정보를 나타내는 클래스,(기존 data 호환을 위해 PageInfo 이름 사용함)
    */
    public class PageInfo
    {

        /** 
        * @enum PageControlType
        * @brief 페이지 컨트롤 정보
        */
        public enum PageControlType
        {
            SCROLL = 1,
            SWIPE = 2,
            NONE = 3
        };

        /** 
        * @enum PageTransitionType
        * @brief 페이지 전환 정보
        */
        public enum PageTransitionType
        {
            OVERWRITE = 1,
            SHOW,
            PUSH,
            FADE,
            NONE
        };

        public string DocumentAudio;
        public Color DocumentColor;
        public string DocumentImage;
        public PageControlType PageControl;
        public double PagingDuration;
        public bool SaveLastPage;
        public bool ZoomScreen; //(none, Pinch Gesture)
        public PageTransitionType AndroidPaging;
        public PageTransitionType IOSPaging;

        /**
        * @brief PageInfo class constructor
        */
        public PageInfo()
        {
            DocumentAudio = null;
            DocumentColor = Colors.White;
            DocumentImage = null;
            PageControl = PageControlType.NONE;
            PagingDuration = 0;
            SaveLastPage = false;
            ZoomScreen = false; //(none, Pinch Gesture)
            AndroidPaging = PageTransitionType.NONE;
            IOSPaging = PageTransitionType.NONE;
        }
}
}
