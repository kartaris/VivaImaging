/**
* @file Spread.cs
* @date 2017.05
* @brief PageBuilder for Windows Spread class file
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
using System.Windows.Media.Imaging;
using System.Xml.Serialization;

namespace PageBuilder.Data
{

    /**
    * @class ObjectOrderInfo
    * @brief 스프레드 내의 Graph 개체의 order 정보를 나타내는 클래스
    */
    public class ObjectOrderInfo
    {
        /** graphic objec id */
        public int object_id;
        /** graphic objec z-order */
        public int z_order;

        public ObjectOrderInfo(int id, int order)
        {
            object_id = id;
            z_order = order;
        }
    }

    /**
    * @class ObjectPositionInfo
    * @brief 스프레드 내의 Graph 개체의 위치 정보를 나타내는 클래스
    */
    public class ObjectPositionInfo
    {
        /** graphic objec id */
        public int object_id;
        /** graphic objec position */
        public Point p;

        public ObjectPositionInfo(int id, double x, double y)
        {
            object_id = id;
            p.X = x;
            p.Y = y;
        }

        public ObjectPositionInfo(int id, Point pos)
        {
            object_id = id;
            p = pos;
        }
    }

    /**
    * @class Spread
    * @brief 단면/양면 페이지를 가지는 스프레드 클래스
    */
    public class Spread
    {
        // Spread Tyle
        public const int GRAPHIC_PAGE = 1;
        public const int TEXT_PAGE = 2;
        public const int MENU_PAGE = 4;

        public static double GUIDELINE_PICK_DISTANCE = 4; /** 가이드라인 이동이 시작되는 최소 거리 */
        public static double SNAP_DISTANCE = 4; /** 스냅이 적용되는 최소 거리 */

        public const int SHOW_GUIDELINE = 1;

        public const int SNAP_TO_GUIDELINE = 1;
        public const int SNAP_TO_BOX = 2;

        const int MIN_GUIDELINE_SPACE = 3;

        /** Spread type : graphic or text */
        public int SpreadType;

        /** Spread width */
        public int SpreadWidth;
        /** Spread height */
        public int SpreadHeight;

        //public int NumberOfPages;

        /** active layer id */
        public int ActiveLayerId;

        public string Name;

        /** Page List */
        public List<Page> Pages;

        /** Horizontal Guideline List */
        public List<double> H_GuideLines;
        /** Vertical Guideline List */
        public List<double> V_GuideLines;

        /** Object List */
        public List<Graphic> GraphicList;
        /** Layer List */
        public List<Layer> Layers;

        public static void SetGuidelineHandleSize(double size)
        {
            GUIDELINE_PICK_DISTANCE = size;
            SNAP_DISTANCE = size;
        }

        /**
        * @brief Spread class constructor
        */
        public Spread()
        {

        }

        /**
        * @brief Spread class constructor
        * @param width : Spread width
        * @param height : Spread height
        * @param numbers : number of pages
        * @param name : name of spread
        * @param type : type of spread
        * @details A. 크기값을 저장하고.
        * @n B. 페이지 개수만큼 페이지 개체를 생성하고,
        * @n C. 기본 레이어를 생성한다.
        */
        public Spread(int width, int height, int numbers, string name, int type)
        {
            SpreadWidth = width;
            SpreadHeight = height;
            Name = name;
            //NumberOfPages = numbers;
            SpreadType = type;

            int pageWidth = width / numbers;
            int pageHeight = height;
            Pages = new List<Page>();

            for(int i=0; i < numbers; i++)
            {
                Rect r = new Rect(pageWidth * i, 0, pageWidth * (i + 1), pageHeight);
                Page page = new Page(r);
                Pages.Add(page);
            }

            Layers = new List<Layer>();
            Layer layer = new Layer("BaseLayer");
            Layers.Add(layer);
            ActiveLayerId = layer.GetId();

            GraphicList = new List<Graphic>();

            H_GuideLines = new List<double>();
            V_GuideLines = new List<double>();

            // 텍스트 페이지이면 기본 텍스트 박스를 추가한다.
            if ((SpreadType & TEXT_PAGE) == TEXT_PAGE)
            {
                for (int i = 0; i < numbers; i++)
                {
                    Rect r = new Rect(pageWidth * i, 0, pageWidth * (i + 1), pageHeight);
                    TextBox t = new TextBox(r, ActiveLayerId);
                    GraphicList.Add(t);
                }
            }

        }

        public bool IsGraphicPage()
        {
            return ((SpreadType & GRAPHIC_PAGE) == GRAPHIC_PAGE);
        }

        /**
        * @brief 대상 개체를 현재의 스프레드에 추가한다.
        * @param rect : 대상 개체의 좌표
        * @param type : 대상 개체의 종류
        * @param fill : 대상 개체의 채우기 속성
        * @param line : 대상 개체의 외곽선 속성
        * @param flag : 플래그
        * @return int : 생성된 개체의 id를 리턴한다.
        * @details A. 개체 종류에 해당하는 개체를 생성하고, 
        * @n B. PageObject에 추가하고, ActiveLayerId로 layer를 설정한다.
        * @n C. flag에 GRAPHIC_SELECTED이 있으면 대상 개체의 선택 플래그를 세트한다.
        */
        public int AddGraphicObject(Rect rect, GraphicType type, Fill fill, OutLine line, int flag)
        {
            Graphic o = null;

            /*
            if (type == GraphicType.GraphicLine)
                o = new Line(rect.TopLeft, rect.BottomRight, ActiveLayerId);
                */
            if (type == GraphicType.GraphicRectangle)
                o = new Rectangle(rect, ActiveLayerId);
            else if (type == GraphicType.GraphicEllipse)
                o = new Ellipse(rect, ActiveLayerId);
            else if (type == GraphicType.GraphicTriangle)
                o = new Triangle(rect, ActiveLayerId);
            else if (type == GraphicType.GraphicTrapezoid)
                o = new Trapezoid(rect, ActiveLayerId);
            else if (type == GraphicType.GraphicParallelogram)
                o = new Parallelogram(rect, ActiveLayerId);

            else if (type == GraphicType.GraphicCylinder)
                o = new Cylinder(rect, ActiveLayerId);
            else if (type == GraphicType.GraphicRevCylinder)
                o = new RevCylinder(rect, ActiveLayerId);
            else if (type == GraphicType.GraphicBevelCylinder)
                o = new BevelCylinder(rect, ActiveLayerId);
            else if (type == GraphicType.GraphicHourGlass)
                o = new HourGlass(rect, ActiveLayerId);
            else if (type == GraphicType.GraphicPentagon)
                o = new Pentagon(rect, ActiveLayerId);

            else if (type == GraphicType.GraphicTextBox)
                o = new TextBox(rect, ActiveLayerId);
            else if (type == GraphicType.GraphicWebBox)
                o = new WebBox(rect, ActiveLayerId);
            else if (type == GraphicType.GraphicMovieBox)
                o = new MovieBox(rect, ActiveLayerId);

            else if (type == GraphicType.GraphicGroup)
                o = new Group(rect, ActiveLayerId);
            else if (type == GraphicType.GraphicImageBox)
                o = new ImageBox(rect, ActiveLayerId);
            else if (type == GraphicType.GraphicScrollBox)
                o = new ScrollBox(rect, ActiveLayerId);

            if (o != null)
            {
                GraphicList.Add(o);
                o.SetLayerId(ActiveLayerId);
                o.SetFill(fill);
                o.SetOutLine(line);

                if ((flag & Graphic.GRAPHIC_SELECTED) == Graphic.GRAPHIC_SELECTED)
                    o.SetGraphFlag(true, Graphic.GRAPHIC_SELECTED);

                return o.GetObjectId();
            }
            return -1;
        }

        /**
        * @brief 라인 타입의 개체를 현재의 스프레드에 추가한다.
        * @param sp : 대상 개체의 시작점
        * @param ep : 대상 개체의 끝점
        * @param type : 대상 개체의 종류
        * @param flag : 플래그
        * @return int : 생성된 개체의 id를 리턴한다.
        * @details A. 개체 종류에 해당하는 개체를 생성하고, 
        * @n B. PageObject에 추가하고, ActiveLayerId로 layer를 설정한다.
        * @n C. flag에 GRAPHIC_SELECTED이 있으면 대상 개체의 선택 플래그를 세트한다.
        */
        public int AddGraphicObject(Point sp, Point ep, GraphicType type, Fill fill, OutLine line, int flag)
        {
            Graphic o = null;

            if (type == GraphicType.GraphicLine)
                o = new Line(sp, ep, ActiveLayerId);
            else if (type == GraphicType.GraphicBezierPath)
                o = new BezierPath(sp, ep, ActiveLayerId);

            if (o != null)
            {
                GraphicList.Add(o);
                o.SetLayerId(ActiveLayerId);
                o.SetFill(fill);
                o.SetOutLine(line);

                if ((flag & Graphic.GRAPHIC_SELECTED) == Graphic.GRAPHIC_SELECTED)
                    o.SetGraphFlag(true, Graphic.GRAPHIC_SELECTED);

                return o.GetObjectId();
            }
            return -1;
        }

        public List<int> AddImageBox(string filenames, Rect rect, int flag)
        {
            string[] files = filenames.Split(',');
            List<int> result_ids = new List<int>();

            Point pt = new Point(rect.X + rect.Width / 2, rect.Y + rect.Height / 2);

            foreach (string name in files)
            {
                if (result_ids.Count > 0)
                {
                    pt.X += 10;
                    pt.Y += 10;
                }

                BitmapImage bi = new BitmapImage();
                bi.BeginInit();
                bi.UriSource = new Uri(name, UriKind.Absolute);
                bi.EndInit();

                if (bi.Height > 0 && bi.Width > 0)
                {
                    rect.X = pt.X - bi.Width / 2;
                    rect.Y = pt.Y - bi.Height / 2;
                    rect.Width = bi.Width;
                    rect.Height = bi.Height;

                    ImageBox o = new ImageBox(rect, ActiveLayerId);
                    o.SetImageFiles(name);
                    GraphicList.Add(o);

                    if ((flag & Graphic.GRAPHIC_SELECTED) == Graphic.GRAPHIC_SELECTED)
                        o.SetGraphFlag(true, Graphic.GRAPHIC_SELECTED);

                    result_ids.Add(o.ObjectId);
                }
            }
            return result_ids;
        }

        /**
        * @brief 지정한 index에 해당하는 개체를 찾아 리턴한다.
        * @param index : 찾을 대상 개체의 index
        */
        public Graphic GetObjectIndex(int index)
        {
            if (GraphicList.Count() > index)
                return GraphicList.ElementAt(index);
            return null;
        }

        /**
        * @brief 지정한 object_id에 해당하는 개체를 찾아 리턴한다.
        * @param object_id : 찾을 대상 개체의 id
        */
        public Graphic GetObject(int object_id)
        {
            foreach(Graphic g in GraphicList)
            {
                if (g.ObjectId == object_id)
                    return g;
            }
            return null;
        }

        /**
        * @brief 지정한 objectId 목록에 해당하는 개체를 지정한 만큼 이동한다.
        * @param objectId : 대상 개체의 id 리스트
        * @param type : 이동하는 핸들 종류
        * @param offset : 이동할 양
        * @return int : 이동된 개체의 개수를 리턴한다.
        */
        public int MoveObjects(List<int> objectId, EditHandleType type, Point offset)
        {
            int count = 0;
            foreach (Graphic g in GraphicList)
            {
                if (objectId.Contains(g.ObjectId))
                {
                    // g.Bounds.Offset((Vector)offset);
                    g.Move((Vector)offset);
                    ++count;
                }
            }
            return count;
        }

        /**
        * @brief 지정한 objectId 목록에 해당하는 개체를 지정한 크기로 변경한다.
        * @param objectId : 대상 개체의 id 리스트
        * @param type : 이동하는 핸들 종류
        * @param offset : 이동할 양
        * @return int : 이동된 개체의 개수를 리턴한다.
        */
        public int ResizeObjects(List<int> objectId, EditHandleType type, Point offset)
        {
            int count = 0;
            foreach (Graphic g in GraphicList)
            {
                if (objectId.Contains(g.ObjectId))
                {
                    /*
                    Rect rect = DragAction.MakeResizedRect(g.Bounds, MouseDragMode.ResizeObject, type, offset);
                    g.ResizeObjects(rect);
                    */
                    g.ResizeByObjectHandle(type, offset);
                    ++count;
                }
            }
            return count;
        }

        public int RemoveObjectsById(List<int> objectId)
        {
            int count = 0;
            for (int i = 0; i < GraphicList.Count; i++)
            {
                if (objectId.Contains(GraphicList[i].ObjectId))
                {
                    GraphicList.RemoveAt(i);
                    i--;
                    count++;
                }
            }

            return count;
        }

        /**
        * @brief 지정한 objectId 목록에 해당하는 개체의 특수 핸들을 지정한 위치로 변경한다.
        * @param objectId : 대상 개체의 id 리스트
        * @param type : 이동하는 핸들 종류
        * @param offset : 이동할 양
        * @param keyState : 키보드 상태(Shift, Ctrl, Alt)
        * @return int : 이동된 개체의 개수를 리턴한다.
        */
        public int HandleEdit(List<int> objectId, EditHandleType type, Point offset, int keyState)
        {
            int count = 0;
            foreach (Graphic g in GraphicList)
            {
                if (objectId.Contains(g.ObjectId))
                {
                    if (g.HandleEdit(type, offset, keyState))
                        ++count;
                }
            }
            return count;
        }

        /**
        * @brief 지정한 위치에 가이드라인을 추가한다.
        * @param pt : 가이드라인 좌표
        * @param bHorz : true이면 수평 가이드라인, 아니면 수직 가이드라인을 추가한다.
        * @return int : 추가되었으면 1을 리턴한다. 인접한 위치에 가이드라인이 존재하면 추가하지 않고 0을 리턴한다.
        */
        public int AddGuideline(Point pt, bool bHorz)
        {
            if (bHorz)
            {
                // check if exist already
                foreach(int p in H_GuideLines)
                {
                    if (Math.Abs(p - pt.Y) <= MIN_GUIDELINE_SPACE)
                        return 0;
                }
                H_GuideLines.Add(pt.Y);
                return 1;
            }
            else
            {
                foreach (int p in V_GuideLines)
                {
                    if (Math.Abs(p - pt.X) <= MIN_GUIDELINE_SPACE)
                        return 0;
                }
                V_GuideLines.Add(pt.X);
                return 1;
            }
        }

        /**
        * @brief 지정한 index의 가이드라인의 위치를 지정한 만큼 이동한다.
        * @param bHorz : true이면 수평 가이드라인, 아니면 수직 가이드라인.
        * @param index : 이동할 가이드라인 인덱스(zero based)
        * @param amount : 이동할 거리
        * @return bool : 정상적으로 이동하였으면 true를 리턴한다.
        */
        public bool MoveGuidelinePosition(bool bHorz, int index, Point amount)
        {
            if (bHorz)
            {
                if ((index >= 0) && (index < H_GuideLines.Count()))
                {
                    H_GuideLines[index] += amount.Y;
                    return true;
                }
            }
            else
            {
                if ((index >= 0) && (index < V_GuideLines.Count()))
                {
                    V_GuideLines[index] += amount.X;
                    return true;
                }
            }
            return false;
        }

        /**
        * @brief 지정한 index의 가이드라인을 제거한다.
        * @param bHorz : true이면 수평 가이드라인, 아니면 수직 가이드라인.
        * @param index : 삭제할 가이드라인 인덱스(zero based)
        * @return bool : 성공하면 true를 리턴한다.
        */
        public bool RemoveGuideline(bool bHorz, int index)
        {
            if (bHorz)
            {
                if ((index >= 0) && (index < H_GuideLines.Count()))
                {
                    H_GuideLines.RemoveAt(index);
                    return true;
                }
            }
            else
            {
                if ((index >= 0) && (index < V_GuideLines.Count()))
                {
                    V_GuideLines.RemoveAt(index);
                    return true;
                }
            }
            return false;
        }

        /**
        * @brief 지정한 좌표에 대해 스냅 적용된 좌표를 리턴한다.
        * @param pt : true이면 수평 가이드라인, 아니면 수직 가이드라인.
        * @param snapFlag : snap flag(SNAP_TO_GUIDELINE, SNAP_TO_BOX)
        * @return Point : 스냅 적용 결과 좌표
        */
        public Point GetSnappedPoint(Point pt, int snapFlag)
        {
            Point move = new Point(0, 0);
            double min_diff_x = SNAP_DISTANCE;
            double min_diff_y = SNAP_DISTANCE;

            if ((snapFlag & SNAP_TO_GUIDELINE) == SNAP_TO_GUIDELINE)
            {
                foreach (double y in H_GuideLines)
                {
                    double diff = Math.Abs(y - pt.Y);
                    if (diff <= min_diff_y)
                    {
                        min_diff_y = diff;
                        move.Y = y - pt.Y;
                    }
                }
                foreach (double x in V_GuideLines)
                {
                    double diff = Math.Abs(x - pt.X);
                    if (diff <= min_diff_x)
                    {
                        min_diff_x = diff;
                        move.X = x - pt.X;
                    }
                }
            }

            if ((snapFlag & SNAP_TO_BOX) == SNAP_TO_BOX)
            {
                foreach (Graphic g in GraphicList)
                {
                    double diff = Math.Abs(g.X - pt.X);
                    if (diff <= min_diff_x)
                    {
                        min_diff_x = diff;
                        move.X = g.X - pt.X;
                    }
                    diff = Math.Abs(g.Right() - pt.X);
                    if (diff <= min_diff_x)
                    {
                        min_diff_x = diff;
                        move.X = g.Right() - pt.X;
                    }
                    diff = Math.Abs(g.Y - pt.Y);
                    if (diff <= min_diff_y)
                    {
                        min_diff_y = diff;
                        move.Y = g.Y - pt.Y;
                    }
                    diff = Math.Abs(g.Bottom() - pt.Y);
                    if (diff <= min_diff_y)
                    {
                        min_diff_y = diff;
                        move.Y = g.Bottom() - pt.Y;
                    }
                }
            }

            Point snap_point = new Point(pt.X + move.X, pt.Y + move.Y);
            return snap_point;
        }

        public Point GetSnappedMovement(Rect rect, int snapFlag)
        {
            double min_diff_x = SNAP_DISTANCE;
            double min_diff_y = SNAP_DISTANCE;
            Point move = new Point(0, 0);

            if ((snapFlag & SNAP_TO_GUIDELINE) == SNAP_TO_GUIDELINE)
            {
                foreach(double y in H_GuideLines)
                {
                    double diff = Math.Abs(y - rect.Top);
                    if (diff <= min_diff_y)
                    {
                        min_diff_y = diff;
                        move.Y = y - rect.Top;
                    }
                    diff = Math.Abs(y - rect.Bottom);
                    if (diff <= min_diff_y)
                    {
                        min_diff_y = diff;
                        move.Y = y - rect.Bottom;
                    }
                }
                foreach (double x in V_GuideLines)
                {
                    double diff = Math.Abs(x - rect.Left);
                    if (diff <= min_diff_x)
                    {
                        min_diff_x = diff;
                        move.X = x - rect.Left;
                    }
                    diff = Math.Abs(x - rect.Right);
                    if (diff <= min_diff_x)
                    {
                        min_diff_x = diff;
                        move.X = x - rect.Right;
                    }
                }
            }

            if ((snapFlag & SNAP_TO_BOX) == SNAP_TO_BOX)
            {
                foreach (Graphic g in GraphicList)
                {
                    double diff = Math.Abs(g.X - rect.Left);
                    if (diff <= min_diff_x)
                    {
                        min_diff_x = diff;
                        move.X = g.X - rect.Left;
                    }
                    diff = Math.Abs(g.Right() - rect.Left);
                    if (diff <= min_diff_x)
                    {
                        min_diff_x = diff;
                        move.X = g.Right() - rect.Left;
                    }
                    diff = Math.Abs(g.X - rect.Right);
                    if (diff <= min_diff_x)
                    {
                        min_diff_x = diff;
                        move.X = g.X - rect.Right;
                    }
                    diff = Math.Abs(g.Right() - rect.Right);
                    if (diff <= min_diff_x)
                    {
                        min_diff_x = diff;
                        move.X = g.Right() - rect.Right;
                    }

                    diff = Math.Abs(g.Y - rect.Top);
                    if (diff <= min_diff_y)
                    {
                        min_diff_y = diff;
                        move.Y = g.Y - rect.Top;
                    }
                    diff = Math.Abs(g.Bottom() - rect.Top);
                    if (diff <= min_diff_y)
                    {
                        min_diff_y = diff;
                        move.Y = g.Bottom() - rect.Top;
                    }
                    diff = Math.Abs(g.Y - rect.Bottom);
                    if (diff <= min_diff_y)
                    {
                        min_diff_y = diff;
                        move.Y = g.Y - rect.Bottom;
                    }
                    diff = Math.Abs(g.Bottom() - rect.Bottom);
                    if (diff <= min_diff_y)
                    {
                        min_diff_y = diff;
                        move.Y = g.Bottom() - rect.Bottom;
                    }
                }
            }

            return move;
        }

        /**
        * @brief 지정한 id의 개체를 스프레드 Graphic 목록에서 제거하고 개체를 리턴한다.
        * @param child_id : 개체의 ID
        * @return Graphic : 스프레드에서 제거된 개체
        */
        private Graphic DetachChildObject(int child_id)
        {
            foreach (Graphic g in GraphicList)
            {
                if (child_id == g.ObjectId)
                {
                    GraphicList.Remove(g);
                    return g;
                }
            }
            return null;
        }

        /**
        * @brief 지정한 이름의 레이어를 생성하고 레이어 리스트에 추가한다.
        * @param name : 새로 생성되는 레이어 이름
        * @return int : 생성된 레이어의 id
        */
        public int AddLayer(string name)
        {
            Layer layer = new Layer(name);
            Layers.Add(layer);
            // return Layers.Count() - 1;
            return layer.GetId();
        }

        /**
        * @brief 지정한 레이어 id의 레이어를 Active Layer로 설정한다.
        * @param id : 레이어 id
        * @return int : Active Layer ID
        */
        public int SetActiveLayer(int id)
        {
            foreach(Layer layer in Layers)
            {
                if (layer.GetId() == id)
                {
                    ActiveLayerId = id;
                    return id;
                }
            }

            return -1;
        }

        /**
        * @brief 지정한 인덱스의 레이어를 리턴한다.
        * @param index : 레이어 index
        * @return PBLayer : 레이어 개체
        */
        public Layer GetLayer(int index)
        {
            if ((index >= 0) && (index < Layers.Count()))
            {
                return Layers[index];
            }
            return null;
        }

        /**
        * @brief 지정한 layerId를 가진 레이어를 리턴한다.
        * @param layerId : 레이어 id
        * @return Layer : 레이어 개체
        */
        public Layer FindLayer(int layerId)
        {
            foreach(Layer l in Layers)
            {
                if (l.Id == layerId)
                    return l;
            }
            return null;
        }

        /**
        * @brief 지정한 layerId를 가진 레이어의 visible 속성을 리턴한다.
        * @param layerId : 레이어 id
        * @return bool : 대상 레이어가 visible이면 true를 리턴한다.
        */
        public bool IsLayerVisible(int layerId)
        {
            Layer layer = FindLayer(layerId);
            if ((layer != null) && layer.IsLayerVisible())
                return true;
            return false;
        }

        /**
        * @brief 지정한 layerId를 가진 레이어의 visible 속성을 설정한다.
        * @param layerId : 레이어 id
        * @param bVisible : true이면 visible
        * @details A. 숨김 레이어이면 그 레이어에 속한 개체들은 선택이 안되고 표시도 안됨.
        */
        public void SetLayerVisible(int layerId, bool bVisible)
        {
            Layer layer = FindLayer(layerId);
            if (layer != null)
            {
                layer.SetLayerVisible(bVisible);

                setLayerObjectVisible(layerId, bVisible);
            }
        }

        /**
        * @brief 지정한 layerId에 속한 개체들의 visible 속성을 설정한다.
        * @param layerId : 레이어 id
        * @param bVisible : true이면 visible
        * @return int : 설정된 개체 개수를 리턴한다.
        */
        int setLayerObjectVisible(int layerId, bool bVisible)
        {
            int count = 0;
            foreach (Graphic g in GraphicList)
            {
                if (g.LayerId == layerId)
                {
                    g.SetVisible(bVisible);
                    ++count;
                }
            }
            return count;
        }

        /**
        * @brief 지정한 layerId를 가진 레이어의 lock 속성을 리턴한다.
        * @param layerId : 레이어 id
        * @return bool : 대상 레이어가 잠금이면 true를 리턴한다.
        */
        public bool IsLayerLocked(int layerId)
        {
            Layer layer = FindLayer(layerId);
            if ((layer != null) && layer.IsLayerLocked())
                return true;
            return false;
        }

        /**
        * @brief 지정한 layerId를 가진 레이어의 잠금 속성을 설정한다.
        * @param layerId : 레이어 id
        * @param bLocked : true이면 잠금
        * @details A. 잠금 레이어이면 그 레이어에 속한 개체들은 선택이 안되고 수정도 안됨.
        */
        public void SetLayerLock(int layerId, bool bLocked)
        {
            Layer layer = FindLayer(layerId);
            if (layer != null)
            {
                layer.SetLayerLock(bLocked);

                setLayerObjectLock(layerId, bLocked);
            }
        }

        /**
        * @brief 지정한 layerId에 속한 개체들의 잠금 속성을 설정한다.
        * @param layerId : 레이어 id
        * @param bLocked : true이면 잠금
        * @return int : 설정된 개체 개수를 리턴한다.
        */
        int setLayerObjectLock(int layerId, bool bLocked)
        {
            int count = 0;
            foreach (Graphic g in GraphicList)
            {
                if (g.LayerId == layerId)
                {
                    g.SetLocked(bLocked);
                    ++count;
                }
            }
            return count;
        }

        /**
        * @brief 개체의 화면 출력을 위해 StackPanel에 Geometry를 생성하는 가상 함수.
        * @param dc : 대상 Panel
        * @param showFlag : 화면 출력 플래그
        * @details A. 모든 페이지에 대해 CreateDrawing() 함수를 호출한다.
        * @n B. 모든 개체에 대해 CreateDrawing()을 호출한다.
        */
        public void CreateDrawing(Canvas dc, int showFlag)
        {
            // background
            RectangleGeometry rg = new RectangleGeometry();
            rg.Rect = new Rect(-SpreadWidth, -SpreadHeight, SpreadWidth * 3, SpreadHeight * 3);

            Path path = new Path();
            path.Fill = new SolidColorBrush(Color.FromArgb(0xFF, 0xFF, 0xFF, 0xFF));
            path.Data = rg;
            dc.Children.Add(path);

            // page border
            foreach (Page p in Pages)
            {
                p.CreateDrawing(dc);
            }

            if ((SpreadType & TEXT_PAGE) == TEXT_PAGE)
                return;

            // graphic objects
            foreach (Graphic g in GraphicList)
            {
                /*
                Layer layer = FindLayer(g.LayerId);
                if ((layer != null) && layer.IsLayerVisible())
                */
                if (g.IsVisible())
                {
                    g.CreateDrawing(dc);
                }
            }
            string str = string.Format("Render : {0}", GraphicList.Count);
            Console.WriteLine(str);

            // guidelines
            if ((showFlag & SHOW_GUIDELINE) == SHOW_GUIDELINE)
            {
                Point sp = new Point(DragAction.MIN_START, 0);
                Point ep = new Point(DragAction.MAX_END, 0);

                foreach (double y in H_GuideLines)
                {
                    Path gpath = new Path();
                    gpath.Stroke = new SolidColorBrush(Color.FromArgb(0xFF, 0xFF, 0x80, 0x80));
                    gpath.StrokeThickness = Page.PageBorderWidth;

                    sp.Y = y;
                    ep.Y = y;
                    gpath.Data = new LineGeometry(sp, ep);

                    dc.Children.Add(gpath);
                }

                sp = new Point(0, DragAction.MIN_START);
                ep = new Point(0, DragAction.MAX_END);
                foreach (double x in V_GuideLines)
                {
                    Path gpath = new Path();
                    gpath.Stroke = new SolidColorBrush(Color.FromArgb(0xFF, 0xFF, 0x80, 0x80));
                    gpath.StrokeThickness = Page.PageBorderWidth;

                    sp.X = x;
                    ep.X = x;
                    gpath.Data = new LineGeometry(sp, ep);

                    dc.Children.Add(gpath);
                }
            }

        }

        /**
        * @brief 선택된 개체의 개수를 리턴한다.
        */
        public int GetSelectedObjectsCount()
        {
            int count = 0;
            foreach (Graphic g in GraphicList)
            {
                /*
                Layer layer = FindLayer(g.LayerId);
                if ((layer != null) && layer.IsLayerVisible() && (layer.IsLayerLocked() == false))
                */
                {
                    if (g.IsSelected())
                    {
                        count++;
                        if (count >= 2) // 2 이상이면 더이상 카운트할 필요 없음.
                            return count;
                    }
                }
            }
            return count;
        }

        /**
        * @brief 선택된 개체의 목록을 리턴한다.
        */
        public List<Graphic> GetSelectedObjects()
        {
            List<Graphic> list = new List<Graphic>();
            foreach (Graphic g in GraphicList)
            {
                /*
                Layer layer = FindLayer(g.LayerId);
                if ((layer != null) && layer.IsLayerVisible() && (layer.IsLayerLocked() == false))
                */
                {
                    if (g.IsSelected())
                        list.Add(g);
                }
            }

            if (list.Count() == 0)
                list = null;
            return list;
        }

        /**
        * @brief 지정한 위치에 개체의 핸들, 개체, 가이드라인이 있는지 확인한다.
        * @param pt : 입력 좌표값
        * @param freeIfNotSelected : true이고 대상 개체가 선택되지 않은 상태이면 새로 선택한다.
        * @param editActionData : 선택된 핸들 정보를 담아 리턴할 버퍼
        * @return EditHandleType : 선택된 핸들의 종류
        * @details A. 선택된 개체의 핸들에 해당하는지 검사하고 맞으면 그 핸들 type을 리턴한다.
        * @n B. 모든 개체에 대해 HitTestOver가 맞으면 MoveObject를 리턴한다.
        */
        public EditHandleType GetSelectedObjectsHandle(Point pt, bool freeIfNotSelected, ref DragAction editActionData)
        {
            // check if selected objects handle
            foreach (Graphic g in Enumerable.Reverse(GraphicList))
            {
                /*
                Layer layer = FindLayer(g.LayerId);
                if ((layer != null) && layer.IsLayerVisible() && (layer.IsLayerLocked() == false) &&
                    g.IsSelected())
                */
                if (g.IsVisible() && !g.IsLocked() && g.IsSelected())
                {
                    EditHandleType a = g.HitTestObjectsHandle(pt);
                    if (a != EditHandleType.None)
                    {
                        editActionData.handleType = a;
                        editActionData.targetObject = g;
                        //editActionData.SetStartPoint(dragStart);
                        return a;
                    }
                }
            }

            // check inside of objects
            foreach (Graphic g in Enumerable.Reverse(GraphicList))
            {
                /*
                Layer layer = FindLayer(g.LayerId);
                if ((layer != null) && layer.IsLayerVisible() && (layer.IsLayerLocked() == false))
                */
                if (g.IsVisible() && !g.IsLocked())
                {
                    if (g.HitTestOver(pt))
                    {
                        // 대상 개체가 선택되지 않은 상태이면 선택 해제하고 선택한다.
                        if (g.IsSelected() == false)
                        {
                            if (freeIfNotSelected)
                                FreeAllSelection();
                            g.SetGraphFlag(true, Graphic.GRAPHIC_SELECTED);
                        }

                        editActionData.handleType = EditHandleType.MoveObject;
                        editActionData.targetObject = g;
                        return EditHandleType.MoveObject;
                    }
                }
            }

            // check horizontal guidelines
            if (H_GuideLines != null)
            {
                int index = 0;
                foreach (double y in H_GuideLines)
                {
                    if (Math.Abs(y - pt.Y) <= GUIDELINE_PICK_DISTANCE)
                    {
                        editActionData.targetIndex = index;
                        editActionData.handleType = EditHandleType.MoveHorzGuide;
                        return EditHandleType.MoveHorzGuide;
                    }
                    ++index;
                }
            }

            // check vertical guidelines
            if (V_GuideLines != null)
            {
                int index = 0;
                foreach (double x in V_GuideLines)
                {
                    if (Math.Abs(x - pt.X) < GUIDELINE_PICK_DISTANCE)
                    {
                        editActionData.targetIndex = index;
                        editActionData.handleType = EditHandleType.MoveVertGuide;
                        return EditHandleType.MoveVertGuide;
                    }
                    ++index;
                }
            }
            return EditHandleType.None;
        }

        /**
        * @brief 선택된 개체들을 모두 해제한다.
        * @details A. 전체 개체 리스트에서 각 개체의 플래그에 선택 플래그가 있으면 해제하고 해제된 개체 개수를 리턴한다.
        */
        public int FreeAllSelection()
        {
            int count = 0;
            foreach (Graphic g in GraphicList)
            {
                if (g.IsSelected())
                {
                    g.ClearSelected();
                    count++;
                }
            }
            return count;
        }

        public int SelectAll()
        {
            int count = 0;
            foreach (Graphic g in GraphicList)
            {
                if (g.IsVisible() && !g.IsLocked() &&
                    (g.IsSelected() == false))
                {
                    g.SetSelected();
                    count++;
                }
            }
            return count;
        }


        /**
        * @brief 지정한 영역 내의 개체들을 모두 선택한다.
        * @details A. 전체 개체 리스트에서 각 개체의 레이어가 visible이고, 잠금 상태가 아니면,
        * @n      - 개체의 bound가 지정한 사각형 내부이면 선택 플래그를 셋트하고, 바깥이고 선택된 상태이면 선택을 해제한다.
        * @n B. 선택이 바뀐 개체의 개수를 리턴한다.
        */
        public int AddSelectionBy(Rect rect, bool freeIfOut)
        {
            int count = 0;
            foreach (Graphic g in GraphicList)
            {
                /*
                Layer layer = FindLayer(g.LayerId);
                if ((layer != null) && layer.IsLayerVisible() && (layer.IsLayerLocked() == false))
                */
                if (g.IsVisible() && !g.IsLocked())
                {
                    if (rect.Contains(g.GetBounds()))
                    {
                        g.SetGraphFlag(true, Graphic.GRAPHIC_SELECTED);
                        count++;
                    }
                    else if (freeIfOut && g.IsSelected())
                    {
                        g.ClearSelected();
                        count++;
                    }
                }
                else if(g.IsSelected())
                {
                    g.ClearSelected();
                    count++;
                }
            }
            return count;
        }

        /**
        * @brief 지정한 개체들을 하나의 그룹 개체로 묶는다.
        * @param targets : 그룹을 행할 대상 개체들의 objectID 리스트.
        * @param flag : Graphic.GRAPHIC_SELECTED이면 새로 생성된 그룹 개체를 선택 상태로 한다.
        * @details A. 그룹 개체를 생성한다.
        * @n      - 개체의 bound가 지정한 사각형 내부이면 선택 플래그를 셋트하고, 바깥이고 선택된 상태이면 선택을 해제한다.
        * @n B. 대상 개체ID 목록의 각 ID에 대하여, DetachChildObject로 스프레드의 개체 목록에서 제거하고,
        * @n      - 이 개체를 그룹의 child object로 등록한다.
        * @n C. 새로 생성된 그룹 개체의 child가 유효하면
        * @n      - Bounds를 새로 계산하고, 스프레드 개체 목록에 추가한다.
        * @n      - flag가 Graphic.GRAPHIC_SELECTED이면 새로 생성된 그룹 개체를 선택 상태로 한다.
        * @n      - 새로 생성된 그룹 개체의 ID를 리턴한다.
        */
        public int GroupObjects(List<int> targets, int flag)
        {
            Group o = new Group(new Rect(0,0,0,0), ActiveLayerId);

            foreach (int child_id in targets)
            {
                Graphic c = DetachChildObject(child_id);
                if (c != null)
                {
                    if (c.IsSelected())
                        c.ClearSelected();
                    o.AddChild(c, false);
                }
                else
                {
                    string str = string.Format("Cannot found object to add-to-group-as-child : {0}", child_id);
                    Console.WriteLine(str);
                }
            }

            if (o.GetChildCount() > 0)
            {
                o.RefreshBounds();

                // append to GraphicList
                GraphicList.Add(o);
                o.SetLayerId(ActiveLayerId);

                if ((flag & Graphic.GRAPHIC_SELECTED) == Graphic.GRAPHIC_SELECTED)
                    o.SetSelected();

                return o.ObjectId; // 새로 생성된 그룹의 ID를 리턴한다.
            }
            return 0;
        }

        /**
        * @brief 지정한 개체 목록중 그룹 개체에 대해 그룹 풀기를 행한다.
        * @param targets : 그룹 풀기를 행할 대상 개체들의 objectID 리스트.
        * @param flag : Graphic.GRAPHIC_SELECTED이면 새로 생성된 개체들를 선택 상태로 한다.
        * @details A. 대상 개체ID 목록의 각 ID에 대하여, 
        *       - 개체의 종류가 그룹이고, 하위 항목이 유효하면, 하위 항목들을 스프레드 개체 목록에 추가하고
        *       - flag가 Graphic.GRAPHIC_SELECTED이면 그 항목을 선택 상태로 한다.
        *       - 그룹 개체의 하위 항목 리스트를 해제한다.
        *       - 그룹 풀기의 결과로 스프레드 개체 목록에 추가된 개체의 개수를 리턴한다.
        */
        public int UngroupObjects(List<int> targets, int flag)
        {
            int count = 0;
            foreach (int child_id in targets)
            {
                Graphic c = DetachChildObject(child_id);
                if (c != null)
                {
                    if (c.ObjectType() == GraphicType.GraphicGroup)
                    {
                        Group g = (Group)c;
                        if (g.GetChildCount() > 0)
                        {
                            foreach(Graphic gc in g.ChildArray)
                            {
                                GraphicList.Add(gc);

                                if ((flag & Graphic.GRAPHIC_SELECTED) == Graphic.GRAPHIC_SELECTED)
                                    gc.SetGraphFlag(true, Graphic.GRAPHIC_SELECTED);

                                count++;
                            }
                            g.ChildArray.Clear();
                        }
                    }
                }
                else
                {
                    string str = string.Format("Cannot found object on ungrouping : {0}", child_id);
                    Console.WriteLine(str);
                }
            }
            return count;
        }

        public List<ObjectOrderInfo> GetObjectOrderList(List<int> targets)
        {
            List<ObjectOrderInfo> store = new List<ObjectOrderInfo>();
            int index = 0;
            foreach (Graphic g in GraphicList)
            {
                if (targets.Contains(g.ObjectId))
                {
                    store.Add(new ObjectOrderInfo(g.ObjectId, index));
                }
                index++;
            }
            return store;
        }

        public List<ObjectPositionInfo> GetObjectPositionList(List<int> targets)
        {
            List<ObjectPositionInfo> store = new List<ObjectPositionInfo>();
            foreach (int oid in targets)
            {
                Graphic g = GetObject(oid);
                store.Add(new ObjectPositionInfo(oid, g.X, g.Y));
            }
            return store;
        }

        /**
        * @brief List<ObjectOrderInfo> 정보에 따라 개체의 순서를 바꾼다.
        * @param store : 대상 개체들의 order정보 리스트.
        * @return int : 순서가 복구된 개체 개수를 리턴한다.
        * @details A. 스프레드 개체중  대상 개체 리스트에 속하는 항목을 임시 리스트로 옮기고
        * @n B. 임시 리스트에 저장된 개체들을 store에 저장된 순서에 위치시킨다.
        */
        public int RestoreObjectOrder(List<ObjectOrderInfo> store)
        {
            List<Graphic> temp_list = new List<Graphic>();
            foreach (ObjectOrderInfo info in store)
            {
                temp_list.Add(DetachChildObject(info.object_id));
            }
            int r = 0;
            int n = 0;
            foreach (ObjectOrderInfo info in store)
            {
                if (temp_list[n] != null)
                {
                    GraphicList.Insert(info.z_order, temp_list[n]);
                    r++;
                }
                else
                {
                    string str = string.Format("cannot found Graphic object(object_id={0})", info.object_id);
                    System.Diagnostics.Debug.WriteLine(str);
                }
                n++;
            }
            return r;
        }

        /**
        * @brief List<ObjectPositionInfo>에 저장된 정보에 따라 개체의 위치를 복구한다.
        * @param store : 대상 개체들의 위치 정보 리스트.
        * @return int : 위치가 복구된 개체 개수를 리턴한다.
        */
        public int RestoreObjectPosition(List<ObjectPositionInfo> store)
        {
            int r = 0;
            foreach (ObjectPositionInfo info in store)
            {
                Graphic g = GetObject(info.object_id);
                if (g != null)
                {
                    g.X = info.p.X;
                    g.Y = info.p.Y;
                    g.ClearPathGeometry(); // clear cached geometry
                    r++;
                }
            }
            return r;
        }


        /**
        * @brief 지정한 개체의 순서를 맨 위로 올린다.
        * @param targets : 대상 개체들의 objectID 리스트.
        * @details A. 스프레드 개체중 
        * @n      - 대상 개체 리스트에 속하고  selected_top이 음이면 이 개체의 순서를 selected_top으로 설정한다.
        * @n      - 속하지 않고 selected_top이 유효하면 unselected_top에 지금의 순서를 넣는다.
        * @n      - unselected_top은 selected_top 하위 개체중 선택 안된 첫번째 개체임.
        * @n B. unselected_top이 유효하면,
        * @n      - 대상 개체들을 임시 버퍼 target_items로 이동한다.
        * @n      - target_items의 개체들을 GraphicList의 뒤에 추가한다.
        * @n      - target_items를 해제하고, 그 개수를 리턴한다.
        */
        public int BringToTop(List<int> targets)
        {
            int n = 0;
            int selected_top = -1;
            int unselected_top = -1;

            // 선택된 개체중 첫번째 것과 그 아래중 선택 안된 개체의 order를 찾는다.
            foreach (Graphic g in GraphicList)
            {
                if (targets.Contains(g.ObjectId))
                {
                    if (selected_top < 0)
                        selected_top = n;
                }
                else
                {
                    if (selected_top >= 0)
                        unselected_top = n;
                }
                n++;
            }

            n = 0;
            if (unselected_top > 0)
            {
                // 대상 개체를 일단 임시 버퍼로 이동
                List<Graphic> target_items = new List<Graphic>();

                for (int i = 0; i < GraphicList.Count(); i++)
                {
                    Graphic g = GraphicList[i];
                    if (targets.Contains(g.ObjectId))
                    {
                        GraphicList.RemoveAt(i);
                        i--;
                        target_items.Add(g);
                    }
                }

                // 다시 뒤에 추가
                foreach (Graphic g in target_items)
                {
                    GraphicList.Add(g);
                }
                n = target_items.Count();
                target_items.Clear();
            }
            return n;
        }

        /**
        * @brief 지정한 개체의 순서를 맨 아래로 내린다.
        * @param targets : 대상 개체들의 objectID 리스트.
        * @details A. 스프레드 개체중 
        * @n      - 대상 개체 리스트에 속하고  target_order가 유효하면( >= 0) 이 개체의 id를 moving_items에 넣는다.
        * @n      - 속하지 않으며 target_order가 음이면 현재의 순서를 target_order로 한다.
        * @n B. 현재의 순서를 유지하도록 임시 리스트에 대상 개체를 옮긴다.(targets의 순서는 무작위임)
        * @n C. order를 하나 올리고 그것이 0보다 크면,
        *       - 대상 개체들을 order 아래로 이동한다.
        */
        public int SendToBottom(List<int> targets)
        {
            int target_order = -1;
            int n = 0;
            List<int> moving_items = new List<int>();

            // 들어갈 order와 이동될 개체들 목록을 준비한다.
            foreach (Graphic g in GraphicList)
            {
                if (targets.Contains(g.ObjectId))
                {
                    if (target_order >= 0)
                        moving_items.Add(g.ObjectId);
                }
                else
                {
                    if (target_order < 0)
                        target_order = n;
                }
                n++;
            }

            n = 0;
            if ((target_order >= 0) && (moving_items.Count() > 0))
            {
                foreach(int child_id in moving_items)
                {
                    Graphic c = DetachChildObject(child_id);
                    if (c != null)
                    {
                        GraphicList.Insert(target_order, c);
                        target_order++;
                        n++;
                    }
                    else
                    {
                        string str = string.Format("Cannot found object on BringToTop : {0}", child_id);
                        Console.WriteLine(str);
                    }
                }
            }
            return n; // number of items which are moved
        }

        /**
        * @brief 지정한 개체의 순서를 위로 올린다.
        * @param targets : 대상 개체들의 objectID 리스트.
        * @details A. 스프레드 개체중 
        * @n      - 대상 개체 리스트에 속하면 임시로  moving_items에 이동한다.
        * @n      - 속하지 않으며 moving_item에 들어있는 개체들을 이 개체 다음에 삽입한다.
        * @n B. moving_item에 남아있는 개체들을 리스트 뒤에 삽입한다.
        */
        public int BringForward(List<int> targets)
        {
            List<Graphic> moving_items = new List<Graphic>();
            int n = 0;

            // 선택된 개체이면 moving_item으로 옮기고, 선택되지 않은 개체이면 moving_item에 있는 개체를 그 다음에 삽입한다.
            for (int i=0; i < GraphicList.Count(); i++)
            {
                Graphic g = GraphicList[i];
                if (targets.Contains(g.ObjectId))
                {
                    GraphicList.RemoveAt(i);
                    moving_items.Add(g);
                    i--;
                }
                else
                {
                    if (moving_items.Count() > 0)
                    {
                        foreach (Graphic c in moving_items)
                        {
                            GraphicList.Insert(i + 1, c);
                            i++;
                        }
                        n += moving_items.Count();
                        moving_items.Clear();
                    }
                }
            }

            // 남은 항목은 맨 뒤에 붙인다.
            int p = GraphicList.Count();
            if (moving_items.Count() > 0)
            {
                foreach (Graphic c in moving_items)
                {
                    GraphicList.Insert(p, c);
                    p++;
                }
                n += moving_items.Count();
                moving_items.Clear();
            }
            return n;
        }

        /**
        * @brief 지정한 개체의 순서를 아래로 내린다.
        * @param targets : 대상 개체들의 objectID 리스트.
        * @details A. 스프레드 개체중 역순으로
        * @n      - 대상 개체 리스트에 속하면 임시로  moving_items에 이동한다.
        * @n      - 속하지 않으며 moving_item에 들어있는 개체들을 이 개체 앞에 삽입한다.
        * @n B. moving_item에 남아있는 개체들을 리스트 맨 앞에 삽입한다.
        */
        public int SendBackward(List<int> targets)
        {
            List<Graphic> moving_items = new List<Graphic>();
            int n = 0;

            // 역순서로 선택된 개체이면 moving_item으로 옮기고, 선택되지 않은 개체이면 moving_item 개체들을 그 다음에 삽입한다.
            for (int i = GraphicList.Count() - 1; i >= 0; i--)
            {
                Graphic g = GraphicList[i];
                if (targets.Contains(g.ObjectId))
                {
                    GraphicList.RemoveAt(i);
                    moving_items.Add(g);
                }
                else
                {
                    if (moving_items.Count() > 0)
                    {
                        foreach (Graphic c in moving_items)
                        {
                            GraphicList.Insert(i, c);
                        }
                        n += moving_items.Count();
                        moving_items.Clear();
                    }
                }
            }

            if (moving_items.Count() > 0)
            {
                foreach (Graphic c in moving_items)
                {
                    GraphicList.Insert(0, c);
                }
                n += moving_items.Count();
                moving_items.Clear();
            }
            return n;
        }

        /**
        * @brief 지정한 개체를 왼쪽 정렬한다.
        * @param targets : 대상 개체들의 objectID 리스트.
        * @details A. 대상 개체가 하나이면 스프레드 왼쪽 테두리에 붙도록 X좌표를 맞춘다.
        * @n B. 대상 개체가 하나 이상이면,
        * @n      - 가장 왼쪽인 좌표를 찾고
        * @n      - 대상 개체들을 가장 왼쪽 좌표로 이동한다.
        */
        public int AlignLeft(List<int> targets)
        {
            if (targets.Count() == 1)
            {
                Graphic c = GetObject(targets[0]);
                if (c != null)
                {
                    c.X = 0;
                }
                return 1;
            }
            else if (targets.Count() > 1)
            {
                // find left-most position
                int count = 0;
                double x = 0;

                foreach (int id in targets)
                {
                    Graphic c = GetObject(id);
                    if (c != null)
                    {
                        if (count == 0)
                            x = c.X;
                        else if (c.X < x)
                            x = c.X;
                        count++;
                    }
                }

                // move items
                foreach (int id in targets)
                {
                    Graphic c = GetObject(id);
                    if (c != null)
                    {
                        c.X = x;
                        c.ClearPathGeometry();
                    }
                }
                return count;
            }
            else
            {
                return 0;
            }
        }

        /**
        * @brief 지정한 개체를 가운데 정렬한다.
        * @param targets : 대상 개체들의 objectID 리스트.
        * @details A. 대상 개체가 하나이면 스프레드의 중앙으로 X 좌표를 맞춘다.
        * @n B. 대상 개체가 하나 이상이면,
        * @n      - 가장 왼쪽, 가장 오른쪽인 좌표를 찾고 이의 가운데 점을 구한다.
        * @n      - 대상 개체들의 중심을 이 가운데 좌표로 이동한다.
        */
        public int AlignCenter(List<int> targets)
        {
            if (targets.Count() == 1)
            {
                Graphic c = GetObject(targets[0]);
                if (c != null)
                {
                    c.X = (SpreadWidth - c.Width) / 2;
                }
                return 1;
            }
            else if (targets.Count() > 1)
            {
                // find left-most right-mode position
                int count = 0;
                double xs = 0;
                double xe = 0;

                foreach (int id in targets)
                {
                    Graphic c = GetObject(id);
                    if (c != null)
                    {
                        if (count == 0)
                        {
                            xs = c.X;
                            xe = c.Right();
                        }
                        else
                        {
                            if (xs > c.X)
                                xs = c.X;
                            if (xe < c.Right())
                                xe = c.Right();
                        }
                        count++;
                    }
                }

                // move items center to (xs + xe) / 2
                double cx = (xs + xe) / 2;
                foreach (int id in targets)
                {
                    Graphic c = GetObject(id);
                    if (c != null)
                    {
                        c.X = cx - c.Width / 2;
                        c.ClearPathGeometry();
                    }
                }
                return count;
            }
            else
            {
                return 0;
            }
        }

        /**
        * @brief 지정한 개체를 오른쪽 정렬한다.
        * @param targets : 대상 개체들의 objectID 리스트.
        * @details A. 대상 개체가 하나이면 스프레드의 오른쪽 테두리에 붙도록 X 좌표를 맞춘다.
        * @n B. 대상 개체가 하나 이상이면,
        * @n      - 가장 오른쪽인 좌표를 찾고,
        * @n      - 대상 개체들의 오른쪽을 이 좌표로 이동한다.
        */
        public int AlignRight(List<int> targets)
        {
            if (targets.Count() == 1)
            {
                Graphic c = GetObject(targets[0]);
                if (c != null)
                {
                    c.X = (SpreadWidth - c.Width);
                }
                return 1;
            }
            else if (targets.Count() > 1)
            {
                // find right-most position
                int count = 0;
                double x = 0;

                foreach (int id in targets)
                {
                    Graphic c = GetObject(id);
                    if (c != null)
                    {
                        if (count == 0)
                            x = c.Right();
                        else if (x < c.Right())
                            x = c.Right();
                        count++;
                    }
                }

                // move items
                foreach (int id in targets)
                {
                    Graphic c = GetObject(id);
                    if (c != null)
                    {
                        c.X = x - c.Width;
                        c.ClearPathGeometry();
                    }
                }
                return count;
            }
            else
            {
                return 0;
            }
        }

        /**
        * @brief 지정한 개체를 위쪽 정렬한다.
        * @param targets : 대상 개체들의 objectID 리스트.
        * @details A. 대상 개체가 하나이면 스프레드 위쪽 테두리에 붙도록 Y좌표를 맞춘다.
        * @n B. 대상 개체가 하나 이상이면,
        * @n      - 가장 위쪽인 좌표를 찾고
        * @n      - 대상 개체들을 가장 위쪽 좌표로 이동한다.
        */
        public int AlignTop(List<int> targets)
        {
            if (targets.Count() == 1)
            {
                Graphic c = GetObject(targets[0]);
                if (c != null)
                {
                    c.Y = 0;
                }
                return 1;
            }
            else if (targets.Count() > 1)
            {
                // find top-most position
                int count = 0;
                double y = 0;

                foreach (int id in targets)
                {
                    Graphic c = GetObject(id);
                    if (c != null)
                    {
                        if (count == 0)
                            y = c.Y;
                        else if (c.Y < y)
                            y = c.Y;
                        count++;
                    }
                }

                // move items
                foreach (int id in targets)
                {
                    Graphic c = GetObject(id);
                    if (c != null)
                    {
                        c.Y = y;
                        c.ClearPathGeometry();
                    }
                }
                return count;
            }
            else
            {
                return 0;
            }
        }

        /**
        * @brief 지정한 개체를 수직 가운데 정렬한다.
        * @param targets : 대상 개체들의 objectID 리스트.
        * @details A. 대상 개체가 하나이면 스프레드의 중앙으로 Y 좌표를 맞춘다.
        * @n B. 대상 개체가 하나 이상이면,
        *       - 가장 위쪽, 가장 아래인 좌표를 찾고 이의 가운데 점을 구한다.
        *       - 대상 개체들의 중심을 이 가운데 좌표로 이동한다.
        */
        public int AlignVCenter(List<int> targets)
        {
            if (targets.Count() == 1)
            {
                Graphic c = GetObject(targets[0]);
                if (c != null)
                {
                    c.Y = (SpreadHeight - c.Height) / 2;
                }
                return 1;
            }
            else if (targets.Count() > 1)
            {
                // find top-most bottom-mode position
                int count = 0;
                double ys = 0;
                double ye = 0;

                foreach (int id in targets)
                {
                    Graphic c = GetObject(id);
                    if (c != null)
                    {
                        if (count == 0)
                        {
                            ys = c.Y;
                            ye = c.Bottom();
                        }
                        else
                        {
                            if (ys > c.Y)
                                ys = c.Y;
                            if (ye < c.Bottom())
                                ye = c.Bottom();
                        }
                        count++;
                    }
                }

                // move items center to (ys + ye) / 2
                double cy = (ys + ye) / 2;
                foreach (int id in targets)
                {
                    Graphic c = GetObject(id);
                    if (c != null)
                    {
                        c.Y = cy - c.Height / 2;
                        c.ClearPathGeometry();
                    }
                }
                return count;
            }
            else
            {
                return 0;
            }
        }

        /**
        * @brief 지정한 개체를 아래쪽 정렬한다.
        * @param targets : 대상 개체들의 objectID 리스트.
        * @details A. 대상 개체가 하나이면 스프레드의 아래쪽 테두리에 붙도록 Y 좌표를 맞춘다.
        * @n B. 대상 개체가 하나 이상이면,
        *       - 가장 아래쪽인 좌표를 찾고,
        *       - 대상 개체들의 아래쪽을 이 좌표로 이동한다.
        */
        public int AlignBottom(List<int> targets)
        {
            if (targets.Count() == 1)
            {
                Graphic c = GetObject(targets[0]);
                if (c != null)
                {
                    c.Y = (SpreadHeight - c.Height);
                }
                return 1;
            }
            else if (targets.Count() > 1)
            {
                // find bottom-most position
                int count = 0;
                double y = 0;

                foreach (int id in targets)
                {
                    Graphic c = GetObject(id);
                    if (c != null)
                    {
                        if (count == 0)
                            y = c.Bottom();
                        else if (y < c.Bottom())
                            y = c.Bottom();
                        count++;
                    }
                }

                // move items
                foreach (int id in targets)
                {
                    Graphic c = GetObject(id);
                    if (c != null)
                    {
                        c.Y = y - c.Height;
                        c.ClearPathGeometry();
                    }
                }
                return count;
            }
            else
            {
                return 0;
            }
        }

    }
}
