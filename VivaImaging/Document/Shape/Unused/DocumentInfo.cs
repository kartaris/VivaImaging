/**
* @file DocumentInfo.cs
* @date 2017.07
* @brief PageBuilder for Windows DocumentInfo class file
*/

using System.Windows;

namespace PageBuilder.Data
{
    /**
    * @class DocumentInfo
    * @brief 문서의 세부 정보를 나타내는 클래스
    */
    public class DocumentInfo
    {
        public bool Box;
        public string Created;
        public string Description;
        public bool Guide;
        public PageInfo PageInfo;

        public int Pages;
        public string PaperName;
        public Size PaperSize;
        public bool Ruler;
        public string Saved;
        public int Snap;
        public string Version;
        public int WorkingPage;

        /**
        * @brief DocumentInfo class constructor
        */
        public DocumentInfo()
        {
            Box = false;
            Pages = 0;
            Ruler = true;
            Saved = null;
            Snap = 0;
            WorkingPage = 0;

            PageInfo = new PageInfo();
        }
    }

}
