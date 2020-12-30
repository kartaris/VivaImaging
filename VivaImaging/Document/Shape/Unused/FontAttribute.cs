using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace PageBuilder.Data
{
    public class FontAttribute
    {
        public String Facename;
        public int PointSize;
        public Fill Color;
        //PBFill BgColor;
        public int Bold;
        public int Italic;
        public int Underline;
        public int StrikeOut;

        public FontAttribute()
        {

        }
    }
}
