/**
* @file Style.cs
* @date 2017.05
* @brief PageBuilder for Windows Style class file
*/
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace PageBuilder.Data
{
    public class Style
    {
        public String Name;

        /*
        public Line TopBorder;
        public Line LeftBorder;
        public Line BottomBorder;
        public Line RightBorder;
        */
        public Fill Fill;
        public FontAttribute Font;
        public ParagraphAttribute Paragraph;

        public Style()
        {

        }
    }

}
