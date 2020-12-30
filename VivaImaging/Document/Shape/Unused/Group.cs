/**
* @file Group.cs
* @date 2017.05
* @brief PageBuilder for Windows Group class file
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
    * @class Group
    * @brief 하위에 Graphic 개체 목록을 갖는 Group 개체 클래스
    */
    public class Group : Graphic
    {
        /** child object array */
        public List<Graphic> ChildArray;

        /**
        * @brief Group class constructor
        */
        public Group() : base()
        {
            ChildArray = null;
        }

        /**
        * @brief PBGroup class constructor
        * @param rect : 개체의 좌표
        * @param layerId : 개체의 소속 레이어 ID
        */
        public Group(Rect rect, int layerId) : base(rect, layerId)
        {
            ChildArray = new List<Graphic>();
        }

        public override GraphicType ObjectType()
        {
            return GraphicType.GraphicGroup;
        }

        /**
        * @brief 개체 목록을 Child object로 추가한다.
        * @param childs : 추가될 child object list
        * @return int : 추가된 개체의 개수
        * @details A. 지정한 목록의 개체들을 ChildArray에 추가한다.
        * @n B. RefreshBounds()를 호출하여 좌표를 갱신한다.
        * @n C. 지정한 목록을 비우고 추가된 항목 개수를 리턴한다.
        */
        public int AddChildArray(List<Graphic> childs)
        {
            int count = 0;
            foreach (Graphic c in childs)
            {
                ChildArray.Add(c);
                count++;
            }

            RefreshBounds();
            childs.Clear();
            return count;
        }

        public int AddChild(Graphic child, bool refreshChild)
        {
            ChildArray.Add(child);
            if (refreshChild)
                RefreshBounds();
            return 1;
        }

        public int GetChildCount()
        {
            return ChildArray.Count();
        }

        public override bool Move(Vector offset)
        {
            foreach (Graphic c in ChildArray)
                c.Move(offset);
            RefreshBounds();
            return true;
        }

        public override bool ResizeObjects(Rect rect)
        {
            //Bounds = rect;
            Rect nbox = new Rect();
            foreach (Graphic c in ChildArray)
            {
                if (Width > 0)
                {
                    double offset = (c.X - X) / Width;
                    nbox.X = rect.X + (offset * rect.Width);
                    offset = (c.Right() - X) / Width;
                    nbox.Width = rect.X + (offset * rect.Width) - nbox.X;
                }
                else
                {
                    nbox.X = rect.X + c.X - X;
                    nbox.Width = 0;
                }

                if (Height > 0)
                {
                    double offset = (c.Y - Y) / Height;
                    nbox.Y = rect.Y + (offset * rect.Height);
                    offset = (c.Bottom() - Y) / Height;
                    nbox.Height = rect.Y + (offset * rect.Height) - nbox.Y;
                }
                else
                {
                    nbox.Y = rect.Y + c.Y - Y;
                    nbox.Height = 0;
                }
                c.ResizeObjects(nbox);
            }

            RefreshBounds();
            return true;
        }


        /**
        * @brief child 개체들의 좌표를 더하여 그룹 개체의 좌표를 계산한다.
        */
        public void RefreshBounds()
        {
            Rect r = new Rect(0, 0, 0, 0);
            foreach (Graphic c in ChildArray)
            {
                Rect cb = c.GetBounds();
                if ((r.X == 0) && (r.Y == 0) && (r.Width == 0) && (r.Height == 0))
                {
                    r = cb;
                }
                else
                {
                    if (r.X > cb.X)
                        r.X = cb.X;
                    if (r.Y > cb.Y)
                        r.Y = cb.Y;
                    if (r.Right < cb.Right)
                        r.Width = cb.Right - r.X;
                    if (r.Bottom < cb.Bottom)
                        r.Height = cb.Bottom - r.Y;
                }
            }
            SetBounds(r);
        }

        /**
        * @brief 개체의 화면 출력을 위해 StackPanel에 Geometry를 생성하는 가상 함수.
        * @param dc : 대상 Panel
        * @details A. Child 항목들에 대해 CreateDrawing()을 호출한다.
        * @n B. base class의 CreateDrawing()을 호출한다.
        */
        public override void CreateDrawing(Canvas dc)
        {
            foreach (Graphic c in ChildArray)
            {
                c.CreateDrawing(dc);
            }

            // draw label textbox
            CreateDrawingText(dc);
        }

    }
}
