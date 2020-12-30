/**
* @file Graphic.cs
* @date 2017.05
* @brief PageBuilder for Windows Graphic class file
*/

using System.Windows;
using System.Windows.Media;
using System.Windows.Controls;
using System.Windows.Shapes;
using System;
using System.Xml.Serialization;

namespace PageBuilder.Data
{
    /** 
    * @enum EditHandleType
    * @brief 마우스 드래깅 핸들 종류
    */
    public enum EditHandleType
    {
        None,
        ResizeLeftTop,
        ResizeTop,
        ResizeRightTop,
        ResizeLeft,
        MoveObject,
        ResizeRight,
        ResizeLeftBottom,
        ResizeBottom,
        ResizeRightBottom,
        MoveHorzGuide,
        MoveVertGuide,
        ObjectHandle1,
        ObjectHandle2,
        ObjectHandle3,
        ObjectHandle4,
        ActionHandleMax
    }

    /** 
    * @enum GraphicType
    * @brief 개체 종류
    */
    public enum GraphicType
    {
        GraphicEmpty = 0,
        GraphicLine = 1,
        GraphicBezierPath,
        GraphicRectangle,
        GraphicEllipse,
        GraphicTriangle,
        GraphicParallelogram,
        GraphicTrapezoid,
        GraphicCylinder,
        GraphicRevCylinder,
        GraphicBevelCylinder,
        GraphicHourGlass,
        GraphicPentagon,

        GraphicTextBox,

        GraphicWebBox,
        GraphicMovieBox,

        GraphicGroup,
        GraphicImageBox,
        GraphicScrollBox,
        GraphicMax
    };

    /*
    [XmlInclude(typeof(RevCylinder))]
    [XmlInclude(typeof(BevelCylinder))]
    [XmlInclude(typeof(HourGlass))]
    */

    [XmlInclude(typeof(Line))]
    [XmlInclude(typeof(BezierPath))]
    [XmlInclude(typeof(Rectangle))]
    [XmlInclude(typeof(Ellipse))]
    [XmlInclude(typeof(Triangle))]
    [XmlInclude(typeof(Parallelogram))]
    [XmlInclude(typeof(Trapezoid))]
    [XmlInclude(typeof(Cylinder))]
    [XmlInclude(typeof(Pentagon))]
    [XmlInclude(typeof(TextBox))]
    [XmlInclude(typeof(WebBox))]
    [XmlInclude(typeof(MovieBox))]
    [XmlInclude(typeof(Group))]
    [XmlInclude(typeof(ImageBox))]
    [XmlInclude(typeof(ScrollBox))]
    /**
    * @class Graphic
    * @brief PageBuilder 기본 개체 클래스(base class)
    */
    public class Graphic
    {

        /** 
        * @enum ImageScaleType
        * @brief 이미지 스케일링 모드
        */
        public enum ImageScaleType
        {
            IMAGE_SCALE_NONE,
            IMAGE_FIT_TO,
            IMAGE_FILL
        };

        /** 
        * @enum ImageButtonType
        * @brief 이미지 버튼 동작
        */
        public enum ImageButtonType
        {
            NONE,
            TOGGLE,
            FLICKER
        };

        /** 
        * @enum GraphDrawType
        * @brief 드로잉 형식
        */
        public enum GraphDrawType
        {
            HOLLOW,
            SOLID,
        }

        /** 
        * @enum GraphFlip
        * @brief 플립 정보
        */
        public enum GraphFlip
        {
            HORZ_FLIP=1,
            VERT_FLIP=2,
        }

        /** 개체의 이름 어레이 */
        static string[] GraphicObjectName =
        {
            "Empty",
            "Line",
            "BezierPath",
            "Rectangle",
            "Ellipse",
            "Triangle",
            "Parallelogram",
            "Trapezoid",
            "Cylinder",
            "RevCylinder",
            "BevelCylinder",
            "HourGlass",
            "Pentagon",

            "TextBox",

            "WebBox",
            "MovieBox",

            "Group",
            "ImageBox",
            "ScrollBox"
        };


        /**
        * @brief 개체의 종류에 따른 이름을 리턴하는 static 함수
        */
        public static string GetDefaultName(GraphicType type)
        {
            return GraphicObjectName[(int)type];
        }

        /** shape's geometry */
        [XmlIgnore]
        public Geometry pathGeom = null;

        /** 화면에 개체 핸들을 표시할 크기. 화면의 확대 축소에 관계없이 동일한 크기로 나타나도록 zoom이 변경될 때마다 업데이트 된다. */
        public static double SELECTED_HANDLE_SIZE = 6;
        /** 화면에 개체 핸들을 표시할 때 외곽선 두께. */
        public static double BASIC_PEN_WIDTH = 1;

        /** Path의 외곽선 플래그 */
        public const int PATH_STROKE = 1;
        /** Path의 채움 플래그 */
        public const int PATH_FILL = 2;
        /** Path의 외곽선 및 채움 플래그 */
        public const int PATH_FILL_STROKE = 3;


        [XmlAttribute]

        /** 개체의 x 좌표 */
        public double X;
        /** 개체의 y 좌표 */
        public double Y;
        /** 개체의 width */
        public double Width;
        /** 개체의 height */
        public double Height;

        /** 개체의 회전 */
        public double Rotate;
        /** 개체의 이름 */
        public string Name;
        /** 개체의 소속 레이어 ID */
        public int LayerId;
        /** 개체의 잠금 상태 */
        public int Lock;
        /** 개체의 크기조절 시 비율 유지 */
        public bool RatioConstrain;
        /** 개체의 채움 정보 */
        public Fill Fill;
        /** 개체의 테두리 선 정보 */
        public OutLine Line;

        /** 개체의 그림자 방향 */
        public int Shadow;
        /** 개체의 투명도 */
        public int Alpha;
        /** 개체의 플립 정보 */
        public int Flip;
        /** 개체의 수평 기울임 정보 */
        public float HorzShear;
        /** 개체의 수직 기울임 정보 */
        public float VertShear;

        /** 개체의 ID */
        public int ObjectId;

        /** 개체의 Label Text */
        public string Text;
        /** 개체의 Text 수평 정렬값 */
        public HorizontalAlignment TextHorzAlign = HorizontalAlignment.Center;
        /** 개체의 Text 수직 정렬값 */
        public VerticalAlignment TextVertAlign = VerticalAlignment.Center;

        /**
        * @brief 개체 선택 핸들을 표시할 크기를 설정하는 static 함수
        * @param size : 개체의 핸들 크기
        */
        public static void SetSelectHandleSize(double size)
        {
            SELECTED_HANDLE_SIZE = size;
            BASIC_PEN_WIDTH = SELECTED_HANDLE_SIZE / 8;
        }

        /**
        * @brief 두 점을 포함하는 사각형을 리턴하는  static 함수
        */
        public static Rect RectFromTwoPoints(Point s, Point e)
        {
            return new Rect(Math.Min(s.X, e.X), Math.Min(s.Y, e.Y), Math.Abs(s.X - e.X), Math.Abs(s.Y - e.Y));
        }

        /** 개체 특수 핸들 외곽선 색 */
        public static Color GRAPH_EDIT_HANDLE_OUTLINE = Colors.Red;
        /** 개체 특수 핸들 채움 색 */
        public static Color GRAPH_EDIT_HANDLE_FILL = Colors.White;
        /** 개체 기본 핸들 외곽선 색 */
        public static Color RESIZE_HANDLE_OUTLINE = Colors.Navy;
        /** 개체 기본 핸들 채움 색 */
        public static Color RESIZE_HANDLE_FILL = Colors.White;

        /** 개체 선택됨 */
        public const int GRAPHIC_SELECTED = 0x01;
        /** 개체 숨김, 마우스 선택 안됨 */
        public const int GRAPHIC_HIDDEN = 0x02;
        /** 개체 잠금, 마우스 선택 안됨 */
        public const int GRAPHIC_LOCKED = 0x04;

        /** 개체 속성 플래그, 선택, 숨김, 잠금 속성 */
        public int GraphicFlag = 0;

        /** 개체의 고유 ID를 설정하기 위한 static 변수 */
        static int MaxGraphicObjectId = 1;

        /**
        * @brief Graphic constructor
        */
        public Graphic()
        {

        }

        /**
        * @brief Graphic constructor
        * @param rect : 개체의 좌표
        * @param layerId : 개체의 소속 레이어 ID
        */
        public Graphic(Rect rect, int layerId)
        {
            //Bounds = rect;
            X = rect.X;
            Y = rect.Y;
            Width = rect.Width;
            Height = rect.Height;
            LayerId = layerId;

            Fill = new Fill();
            Fill.FillType = GraphDrawType.HOLLOW;
            Fill.Color = Color.FromArgb(0x00, 0, 0, 0);

            Line = new OutLine();
            Line.LineType = GraphDrawType.HOLLOW;
            Line.Join = PenLineJoin.Bevel;
            Line.StartCap = PenLineCap.Flat;
            Line.EndCap = PenLineCap.Flat;
            Line.Color = Color.FromArgb(0x00, 0, 0, 0);

            Shadow = 0;
            Alpha = 0xFF;
            Flip = 0;
            HorzShear = 0;
            VertShear = 0;

            Name = GetDefaultName(ObjectType());

            Text = null;

            ObjectId = MaxGraphicObjectId;
            MaxGraphicObjectId++;
        }

        /**
        * @brief 개체의 ID를 리턴한다.
        * @return int : 개체의 ID
        */
        public int GetObjectId()
        {
            return ObjectId;
        }

        /**
        * @brief 개체의 좌표를 리턴한다.
        * @return Rect : 개체의 좌표
        */
        public Rect GetBounds()
        {
            //return Bounds;
            return new Rect(X, Y, Width, Height);
        }

        /**
        * @brief 개체의 크기값을 Size로 리턴한다.
        */
        public Size Size()
        {
            return new Size(Width, Height);
        }

        /**
        * @brief 개체의 오른 끝값을 리턴한다.
        */
        public double Right()
        {
            return (X + Width);
        }

        /**
        * @brief 개체의 아래 끝값을 리턴한다.
        */
        public double Bottom()
        {
            return (Y + Height);
        }

        /**
        * @brief 개체의 좌측 상단점을 리턴한다.
        */
        public Point TopLeft()
        {
            return new Point(X, Y);
        }
        /**
        * @brief 개체의 우측 상단점을 리턴한다.
        */
        public Point TopRight()
        {
            return new Point(X + Width, Y);
        }
        /**
        * @brief 개체의 우측 하단점을 리턴한다.
        */
        public Point BottomRight()
        {
            return new Point(X + Width, Y + Height);
        }
        /**
        * @brief 개체의 좌측 하단점을 리턴한다.
        */
        public Point BottomLeft()
        {
            return new Point(X, Y + Height);
        }
        /**
        * @brief 개체의 좌표를 설정한다.
        * @param rect : 개체의 좌표
        */
        public void SetBounds(Rect rect)
        {
            //Bounds = rect;
            X = rect.X;
            Y = rect.Y;
            Width = rect.Width;
            Height = rect.Height;
        }

        /**
        * @brief 개체의 투명 Alpha값을 설정한다.
        * @param alpha : 개체의 Alpha값(0 ~ 255)
        */
        public void SetAlpha(int alpha)
        {
            Alpha = (alpha & 0x0FF);
        }

        /**
        * @brief 개체의 채우기 속성을 설정한다.
        * @param type : 개체의 채우기 속성 종류
        */
        public void SetFill(Graphic.GraphDrawType type)
        {
            Fill.FillType = type;
        }
        /**
        * @brief 개체의 채우기 속성을 설정한다.
        * @param type : 개체의 채우기 속성 종류
        * @param color : 개체의 채우기 컬러 값
        */
        public void SetFill(Graphic.GraphDrawType type, Color color)
        {
            Fill.FillType = type;
            Fill.Color = color;
        }
        /**
        * @brief 개체의 채우기 속성을 설정한다.
        * @param fill : 개체의 채우기 속성
        */
        public void SetFill(Fill fill)
        {
            Fill = fill;
        }

        /**
        * @brief 개체의 외곽선 속성을 설정한다.
        * @param type : 개체의 채우기 속성 종류
        */
        public void SetOutLine(Graphic.GraphDrawType type)
        {
            Line.LineType = type;
        }
        /**
        * @brief 개체의 외곽선 속성을 설정한다.
        * @param type : 개체의 외곽선 속성 종류
        * @param color : 개체의 외곽선 컬러 값
        * @param width : 개체의 외곽선 두께
        */
        public void SetOutLine(Graphic.GraphDrawType type, Color color, int width)
        {
            Line.LineType = type;
            Line.Color = color;
            Line.Width = width;
        }

        /**
        * @brief 개체의 외곽선 속성을 설정한다.
        * @param line : 개체의 외곽선 속성 종류
        */
        public void SetOutLine(OutLine line)
        {
            Line = line;
        }
        /**
        * @brief 개체의 플립 속성을 설정한다.
        * @param set : 설정 또는 해제
        * @param horz : 수평 또는 수직
        */
        public void SetFlip(bool set, bool horz)
        {
            if (set)
            {
                Flip |= (int) (horz ? GraphFlip.HORZ_FLIP : GraphFlip.VERT_FLIP);
            }
            else
            {
                Flip &= ~(int)(horz ? GraphFlip.HORZ_FLIP : GraphFlip.VERT_FLIP);
            }
        }

        /**
        * @brief 개체의 속성을 설정한다.
        * @param set : 설정 또는 해제
        * @param flag : 개체 속성(GRAPHIC_SELECTED, GRAPHIC_HIDDEN, GRAPHIC_LOCKED)
        */
        public void SetGraphFlag(bool set, int flag)
        {
            if (set)
                GraphicFlag |= flag;
            else
                GraphicFlag &= ~flag;
        }

        /**
        * @brief 개체의 레이어를 설정한다.
        * @param layerId : 레이어 ID
        */
        public void SetLayerId(int layerId)
        {
            LayerId = layerId;
        }

        /**
        * @brief 개체가 선택되었는지 확인한다.
        * @return bool : 선택되어 있으면 true를 리턴한다.
        */
        public bool IsSelected()
        {
            return ((GraphicFlag & GRAPHIC_SELECTED) == GRAPHIC_SELECTED);
        }

        /**
        * @brief 개체의 선택을 설정한다.
        * @return bool : 선택되어 있지 않으면 true를 리턴한다.
        */
        public bool SetSelected()
        {
            if ((GraphicFlag & GRAPHIC_SELECTED) == 0)
            {
                GraphicFlag |= GRAPHIC_SELECTED;
                return true;
            }
            return false;
        }

        /**
        * @brief 개체의 선택을 해제한다.
        * @return bool : 선택되어 있으면 true를 리턴한다.
        */
        public bool ClearSelected()
        {
            if ((GraphicFlag & GRAPHIC_SELECTED) == GRAPHIC_SELECTED)
            {
                GraphicFlag &= ~GRAPHIC_SELECTED;
                return true;
            }
            return false;
        }

        /**
        * @brief 개체가 보임 상태인지 확인한다.
        * @return bool : 보임 상태면 true를 리턴한다.
        */
        public bool IsVisible()
        {
            return ((GraphicFlag & GRAPHIC_HIDDEN) == 0);
        }

        /**
        * @brief 개체의 보임 상태를 설정한다.
        * @param bVisible : true이면 보임 상태로 한다.
        */
        public void SetVisible(bool bVisible)
        {
            if (bVisible)
                GraphicFlag &= ~GRAPHIC_HIDDEN;
            else
                GraphicFlag |= GRAPHIC_HIDDEN;
        }

        /**
        * @brief 개체가 잠금 상태인지 확인한다.
        * @return bool : 잠금 상태면 true를 리턴한다.
        */
        public bool IsLocked()
        {
            return ((GraphicFlag & GRAPHIC_LOCKED) == GRAPHIC_LOCKED);
        }

        /**
        * @brief 개체의 잠금 상태를 설정한다.
        * @return bLocked : true이면 잠금 상태로 한다.
        */
        public void SetLocked(bool bLocked)
        {
            if (bLocked)
                GraphicFlag |= GRAPHIC_LOCKED;
            else
                GraphicFlag &= ~GRAPHIC_LOCKED;
        }

        /**
        * @brief 개체의 핸들을 이동하여 크기극 조절할 때의 동작 모드를 리턴한다.
        * @return bool : 직선 스타일 핸들 이동이면 true를 리턴 한다.
        */
        public bool IsLineStyleRubber()
        {
            GraphicType type = ObjectType();
            return ((type == GraphicType.GraphicLine) ||
                (type == GraphicType.GraphicBezierPath));
        }

        /**
        * @brief 개체의 시작점을 리턴하는 가상 함수.
        * @return Point : 좌상단 점을 시작점으로 리턴 한다.
        */
        public virtual Point GetStartPoint()
        {
            //return Bounds.TopLeft;
            return new Point(X, Y);
        }

        /**
        * @brief 개체의 끝점을 리턴하는 가상 함수.
        * @return Point : 우하단 점을 시작점으로 리턴 한다.
        */
        public virtual Point GetEndPoint()
        {
            //return Bounds.BottomRight;
            return new Point(X+Width, Y+Height);
        }

        /**
        * @brief 개체의 Outline 속성을 Pen으로 생성하여 리턴한다.
        * @return Pen : 개체의 Outline 속성
        */
        public Pen GetPen()
        {
            Pen pen = null;

            if (Line.LineType == GraphDrawType.SOLID)
            {
                pen = new Pen();
                pen.Brush = new SolidColorBrush(Line.Color);
                pen.Thickness = 1;
                pen.LineJoin = Line.Join; // PenLineJoin.Bevel;
                pen.StartLineCap = Line.StartCap; // PenLineCap.Flat;
                pen.EndLineCap = Line.EndCap; // PenLineCap.Flat;
            }
            return pen;
        }

        /**
        * @brief 개체의 Fill 속성을 Brush로 생성하여 리턴한다.
        * @return Brush : 개체의 Brush 속성
        */
        public Brush GetBrush()
        {
            Brush brush = null;
            if (Fill.FillType == GraphDrawType.SOLID)
            {
                brush = new SolidColorBrush(Fill.Color);
            }
            return brush;
        }

        /**
        * @brief 개체의 Fill 및 아웃라인 속성을 path에 설정한다.
        * @param path : 대상 Path
        * @param flag : PATH_STROKE, PATH_FILL flag
        */
        public void GetFillStroke(ref Path path, int flag = PATH_FILL_STROKE)
        {
            if ((flag & PATH_FILL) == PATH_FILL)
            {
                path.Fill = GetBrush();
            }
            if ((flag & PATH_STROKE) == PATH_STROKE)
            {
                path.Stroke = new SolidColorBrush(Line.Color);
                path.StrokeStartLineCap = Line.StartCap;
                path.StrokeEndLineCap = Line.EndCap;
                path.StrokeLineJoin = Line.Join;
                path.StrokeThickness = Line.Width;
            }
        }

        /**
        * @brief 개체의 종류를 리턴하는 가상함수
        * @return GraphicType : 개체의 형식 종류
        */
        public virtual GraphicType ObjectType()
        {
            return GraphicType.GraphicEmpty;
        }

        /**
        * @brief 개체의 위치를 지정한 만큼 이동한다.
        * @param offset : 이동할 양
        * @return bool : 성공적으로 이동되면 true를 리턴한다.
        */
        public virtual bool Move(Vector offset)
        {
            //Bounds.Offset(offset);
            X += offset.X;
            Y += offset.Y;

            ClearPathGeometry();
            return true;
        }

        /**
        * @brief 개체의 위치와 크기를 지정한 Rect로 설정한다.
        * @param rect : 새로운 위치와 크기값
        * @return bool : 성공적으로 이동되면 true를 리턴한다.
        */
        public virtual bool ResizeObjects(Rect rect)
        {
            SetBounds(rect);
            ClearPathGeometry();
            return true;
        }

        /**
        * @brief 개체의 화면 드로잉 캐시 정보를 삭제한다. 
        * 개체의 좌표나 크기가 변경되면 화면에 다시 그려지도록 이 캐시값이 삭제되어야 한다.
        */
        public void ClearPathGeometry()
        {
            pathGeom = null;
        }

        /**
        * @brief 개체의 화면 출력을 위해 StackPanel에 Geometry를 생성하는 가상 함수.
        * @n 개체가 선택되어 있으면 핸들을 출력한다.
        * @param dc : 대상 Panel
        */
        public virtual void CreateDrawingText(Canvas dc)
        {
            if (Text != null)
            {
                TextBlock textBlock = new TextBlock();
                textBlock.Text = Text;
                textBlock.Foreground = Brushes.Black;
                textBlock.Width = Width;
                textBlock.Height = Height;
                textBlock.TextWrapping = TextWrapping.Wrap;
                textBlock.HorizontalAlignment = TextHorzAlign;
                textBlock.VerticalAlignment = TextVertAlign;

                Canvas.SetLeft(textBlock, X);
                Canvas.SetTop(textBlock, Y);

                dc.Children.Add(textBlock);
            }
        }

        /**
        * @brief 개체의 화면 출력을 위해 StackPanel에 Geometry를 생성하는 가상 함수.
        * @n 개체가 선택되어 있으면 핸들을 출력한다.
        * @param dc : 대상 Panel
        */
        public virtual void CreateDrawing(Canvas dc)
        {
            RectangleGeometry rg = new RectangleGeometry();
            rg.Rect = new Rect(X, Y, Width, Height); //  Bounds;

            Path path = new Path();
            path.Stroke = Brushes.DarkGray;
            path.StrokeThickness = BASIC_PEN_WIDTH;
            path.Data = rg;

            dc.Children.Add(path);

            TextBlock textBlock = new TextBlock();
            textBlock.Text = Name;
            textBlock.Foreground = Brushes.Black;

            Canvas.SetLeft(textBlock, X);
            Canvas.SetTop(textBlock, Y);

            dc.Children.Add(textBlock);
        }

        /**
        * @brief 지정한 좌표에 개체의 핸들이 있으면 그 핸들 형식을 리턴하는 가상 함수.
        * @param pt : 지정한 좌표
        * @return EditHandleType : 핸들 형식
        * @details A. x축 핸들 종류, y축 핸들 종류를 계산하고
        * @n B. 그 결과를 합산해 리턴한다.
        */
        public virtual EditHandleType HitTestObjectsHandle(Point pt)
        {
            int x = -1;
            int y = -1;
            double right = X + Width;
            double bottom = Y + Height;

            double handle_half = SELECTED_HANDLE_SIZE / 2;
            if (((X - handle_half) < pt.X) && (pt.X < X + handle_half))
                x = 0;
            else if ((right - handle_half < pt.X) && (pt.X < (right + handle_half)))
                x = 2;
            else
            {
                double cx = X + Width / 2 - handle_half;
                if ((cx < pt.X) && (pt.X < (cx + SELECTED_HANDLE_SIZE)))
                    x = 1;
            }

            if (((Y - handle_half) < pt.Y) && (pt.Y < Y + handle_half))
                y = 0;
            else if ((bottom - handle_half < pt.Y) && (pt.Y < (bottom + handle_half)))
                y = 2;
            else
            {
                double cy = Y + Height / 2 - handle_half;
                if ((cy < pt.Y) && (pt.Y < (cy + SELECTED_HANDLE_SIZE)))
                    y = 1;
            }

            /*
            if (((Bounds.X - SELECTED_HANDLE_SIZE) < pt.X) && (pt.X < Bounds.X))
                x = 0;
            if ((Bounds.Right < pt.X) && (pt.X < (Bounds.Right + SELECTED_HANDLE_SIZE)))
                x = 2;
            double cx = Bounds.X + Bounds.Width / 2 - SELECTED_HANDLE_SIZE / 2;
            if ((cx < pt.X) && (pt.X < (cx + SELECTED_HANDLE_SIZE)))
                x = 1;

            if (((Bounds.Y - SELECTED_HANDLE_SIZE) < pt.Y) && (pt.Y < Bounds.Y))
                y = 0;
            if ((Bounds.Bottom < pt.Y) && (pt.Y < (Bounds.Bottom + SELECTED_HANDLE_SIZE)))
                y = 2;
            double cy = Bounds.Y + Bounds.Height / 2 - SELECTED_HANDLE_SIZE / 2;
            if ((cy < pt.Y) && (pt.Y < (cy + SELECTED_HANDLE_SIZE)))
                y = 1;
            */

            if ((x >= 0) && (y >= 0) && ((x != 1) || (y != 1)))
            {
                /*
                if (x == 0)
                    origin.X = Bounds.X;
                else if (x == 2)
                    origin.X = Bounds.Right;
                else
                    origin.X = Bounds.X + Bounds.Width / 2;
                if (y == 0)
                    origin.Y = Bounds.Y;
                else if (y == 2)
                    origin.Y = Bounds.Bottom;
                else
                    origin.Y = Bounds.Y + Bounds.Height / 2;
                */
                return ((EditHandleType)(x + y * 3 + EditHandleType.ResizeLeftTop));
            }
            return EditHandleType.None;
        }

        /**
        * @brief 이 개체가 선택되었을 때 표시되는 기본 Handle을 화면에 출력하는 가상 함수.
        * @param drawingContext : 표시할 target
        */
        public virtual void RenderObjectHandle(DrawingContext drawingContext)
        {
            Pen handlePen = new Pen(new SolidColorBrush(RESIZE_HANDLE_OUTLINE), BASIC_PEN_WIDTH);
            Brush brush = new SolidColorBrush(RESIZE_HANDLE_FILL);
            RenderObjectHandle(drawingContext, handlePen, brush);
        }

        /**
        * @brief 이 개체가 선택되었을 때 표시되는 기본 Handle을 화면에 출력하는 가상 함수.
        * @param drawingContext : 표시할 target
        * @param handlePen : 표시에 사용할 펜
        * @param brush : 표시에 사용할 브러시
        */
        public virtual void RenderObjectHandle(DrawingContext drawingContext, Pen handlePen, Brush brush)
        {
            // top line
            drawingContext.DrawEllipse(brush, handlePen, new Point(X, Y), SELECTED_HANDLE_SIZE, SELECTED_HANDLE_SIZE);
            double cx = X + Width / 2;
            drawingContext.DrawEllipse(brush, handlePen, new Point(cx, Y), SELECTED_HANDLE_SIZE, SELECTED_HANDLE_SIZE);
            drawingContext.DrawEllipse(brush, handlePen, new Point(X+Width, Y), SELECTED_HANDLE_SIZE, SELECTED_HANDLE_SIZE);

            // center line
            double cy = Y + Height / 2;
            drawingContext.DrawEllipse(brush, handlePen, new Point(X, cy), SELECTED_HANDLE_SIZE, SELECTED_HANDLE_SIZE);
            drawingContext.DrawEllipse(brush, handlePen, new Point(X+Width, cy), SELECTED_HANDLE_SIZE, SELECTED_HANDLE_SIZE);

            // bottom line
            drawingContext.DrawEllipse(brush, handlePen, new Point(X, Y +Height), SELECTED_HANDLE_SIZE, SELECTED_HANDLE_SIZE);
            drawingContext.DrawEllipse(brush, handlePen, new Point(cx, Y + Height), SELECTED_HANDLE_SIZE, SELECTED_HANDLE_SIZE);
            drawingContext.DrawEllipse(brush, handlePen, new Point(X+Width, Y+Height), SELECTED_HANDLE_SIZE, SELECTED_HANDLE_SIZE);

            /*
            Rect rect = new Rect(Bounds.X - SELECTED_HANDLE_SIZE, Bounds.Y - SELECTED_HANDLE_SIZE, SELECTED_HANDLE_SIZE, SELECTED_HANDLE_SIZE);
            drawingContext.DrawRectangle(null, handlePen, rect);

            double cx = Bounds.X + Bounds.Width / 2 - SELECTED_HANDLE_SIZE / 2;
            rect = new Rect(cx, Bounds.Y - SELECTED_HANDLE_SIZE, SELECTED_HANDLE_SIZE, SELECTED_HANDLE_SIZE);
            drawingContext.DrawRectangle(null, handlePen, rect);

            rect = new Rect(Bounds.Right, Bounds.Y - SELECTED_HANDLE_SIZE, SELECTED_HANDLE_SIZE, SELECTED_HANDLE_SIZE);
            drawingContext.DrawRectangle(null, handlePen, rect);

            // center line
            double cy = Bounds.Y + Bounds.Height / 2 - SELECTED_HANDLE_SIZE / 2;

            rect = new Rect(Bounds.X - SELECTED_HANDLE_SIZE, cy, SELECTED_HANDLE_SIZE, SELECTED_HANDLE_SIZE);
            drawingContext.DrawRectangle(null, handlePen, rect);

            rect = new Rect(Bounds.Right, cy, SELECTED_HANDLE_SIZE, SELECTED_HANDLE_SIZE);
            drawingContext.DrawRectangle(null, handlePen, rect);

            // bottom line
            rect = new Rect(Bounds.X - SELECTED_HANDLE_SIZE, Bounds.Bottom, SELECTED_HANDLE_SIZE, SELECTED_HANDLE_SIZE);
            drawingContext.DrawRectangle(null, handlePen, rect);

            rect = new Rect(cx, Bounds.Bottom, SELECTED_HANDLE_SIZE, SELECTED_HANDLE_SIZE);
            drawingContext.DrawRectangle(null, handlePen, rect);

            rect = new Rect(Bounds.Right, Bounds.Bottom, SELECTED_HANDLE_SIZE, SELECTED_HANDLE_SIZE);
            drawingContext.DrawRectangle(null, handlePen, rect);
            */
        }

        /**
        * @brief 개체별 특수 핸들을 화면에 출력하는 가상 함수.
        * @param drawingContext : 표시할 target
        * @param pt : 표시될 위치
        */
        public virtual void RenderSpecialHandle(DrawingContext drawingContext, Point pt)
        {
            Pen handlePen = new Pen(new SolidColorBrush(GRAPH_EDIT_HANDLE_OUTLINE), SELECTED_HANDLE_SIZE / 8);
            Brush brush = new SolidColorBrush(GRAPH_EDIT_HANDLE_FILL);
            drawingContext.DrawEllipse(brush, handlePen, pt, SELECTED_HANDLE_SIZE, SELECTED_HANDLE_SIZE);
        }

        /**
        * @brief 개체별 특수 핸들을 드래그할 때 RuberBand를 화면에 출력하는 가상 함수.
        * @param drawingContext : 표시할 target 화면
        * @param brush : 표시에 사용할 브러시
        * @param pen : 표시에 사용할 펜
        * @param mode : 마우스 드래그 모드
        * @param handleType : 드래깅하는 핸들의 종류
        * @param dragAmount : 드래깅에 의한 이동량
        * @param keyState : Shift/Ctrl 키 상태값
        */
        public virtual void OnRenderRubber(DrawingContext drawingContext, Brush brush, Pen pen, MouseDragMode mode, EditHandleType handleType, Point dragAmount, int keyState)
        {
            Rect rect = new Rect(X, Y, Width, Height);
            Rect bound = DragAction.MakeResizedRect(rect, mode, handleType, dragAmount);

            StreamGeometry streamGeometry = new StreamGeometry();
            using (StreamGeometryContext geometryContext = streamGeometry.Open())
            {
                geometryContext.BeginFigure(new Point(bound.Left, bound.Bottom), true, true);

                PointCollection points = new PointCollection();
                points.Add(new Point(bound.Right, bound.Bottom));
                points.Add(new Point(bound.Right, bound.Top));
                points.Add(new Point(bound.Left, bound.Top));

                geometryContext.PolyLineTo(points, true, true);
            }
            drawingContext.DrawGeometry(brush, pen, streamGeometry);
        }

        /**
        * @brief 개체별 특수 핸들을 드래그에 따라 결과값(Handle값)을 리턴하는 가상 함수.
        * @param handleType : 드래깅하는 핸들의 종류
        * @param dragAmount : 드래깅에 의한 이동량
        * @return Point : 드래그 동작에 따라 변동되는 Handle 값.
        */
        public virtual Point GetResultRubber(EditHandleType handleType, Point dragAmount)
        {
            return new Point(0, 0);
        }

        /**
        * @brief 개체별 특수 핸들을 드롭하고 결과값을 Handle에 적용하는 가상 함수.
        * @param handleType : 드래깅하는 핸들의 종류
        * @param handle : 드래깅에 의해 변동된 handle값
        * @return bool : 값이 정상적으로 설정되었으면 true를 리턴한다.
        */
        public virtual bool HandleEdit(EditHandleType handleType, Point handle, int keyState)
        {
            return false;
        }

        /**
        * @brief 지정한 좌표에 개체가 위치하는지 체크하는 가상 함수.
        * @param pt : 지정한 좌표
        * @return bool : 위치가 해당되면 true를 리턴한다.
        */
        public virtual bool HitTestOver(Point pt)
        {
            Rect rect = new Rect(X, Y, Width, Height);
            return rect.Contains(pt);
        }

        /**
        * @brief 핸들의 이동에 따라 개체의 크기를 변경하는 가상 함수.
        * @param type : 핸들 종류
        * @param offset : 핸들의 이동량
        * @return bool : 성공하면 true를 리턴한다.
        */
        public virtual bool ResizeByObjectHandle(EditHandleType type, Point offset)
        {
            Rect bound = new Rect(X, Y, Width, Height);
            Rect rect = DragAction.MakeResizedRect(bound, MouseDragMode.ResizeObject, type, offset);
            return ResizeObjects(rect);
        }


    }

}
