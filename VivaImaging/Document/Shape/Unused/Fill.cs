using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Media;

namespace PageBuilder.Data
{
    public class Fill
    {
        public Color Color;
        public Graphic.GraphDrawType FillType;

        public Fill()
        {

        }

        public Fill(Graphic.GraphDrawType type)
        {
            FillType = type;
        }

        public Fill(Graphic.GraphDrawType type, Color color)
        {
            FillType = type;
            Color = color;
        }
    }
}
