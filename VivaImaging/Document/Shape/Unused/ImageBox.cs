/**
* @file ImageBox.cs
* @date 2017.05
* @brief PageBuilder for Windows ImageBox class file
*/
using System;
using System.Collections.Generic;
using System.Windows;
using System.Windows.Media;
using System.Windows.Shapes;
using System.Windows.Controls;
using System.Windows.Media.Imaging;

namespace PageBuilder.Data
{
    /**
    * @class ImageBox
    * @brief 하나 또는 다수의 이미지를 가지는 ImageBox 개체 클래스
    */
    public class ImageBox : Graphic
    {
        /** 이미지 파일 이름 목록 */
        public List<string> ImageFileNames;
        /** 이미지 스케일 형식 */
        public ImageScaleType ScaleType; // none, fit-to, fill
        /** 이미지 버튼 형식 */
        public ImageButtonType ButtonType; // none, toggle, flicker
        /** 이미지 회전 각도 */
        public float ImageRotate;
        /** 시작시에 화면에 보이지 않음 플래그 */
        public bool NotShowAtStart;

        /** 현재 표시되는 이미지 인덱스 */
        int ShowPageIndex;

        /**
        * @brief ImageBox class constructor
        */
        public ImageBox() : base()
        {
        }

        /**
        * @brief ImageBox class constructor
        * @param rect : 개체의 좌표
        * @param layerId : 개체의 소속 레이어 ID
        */
        public ImageBox(Rect rect, int layerId) : base (rect, layerId)
        {
            ScaleType = ImageScaleType.IMAGE_SCALE_NONE;
            ButtonType = ImageButtonType.NONE;
            ImageRotate = 0;
            NotShowAtStart = false;

            ShowPageIndex = 0;
        }

        /**
        * @brief 개체의 종류를 리턴하는 가상함수
        */
        public override GraphicType ObjectType()
        {
            return GraphicType.GraphicImageBox;
        }
        /**
        * @brief 첫번째 이미지 파일명을 리턴한다.
        * @return string : 이미지 파일명(절대 또는 상대 경로)
        */
        public string GetImageFiles()
        {
            return ImageFileNames[0];
        }

        /**
        * @brief 지정한 파일명을 이미지 파일 목록에 추가한다.
        * @param filenames : 여러개인 경우 콤마 구분되는 파일명
        */
        public void SetImageFiles(string filenames)
        {
            if (ImageFileNames == null)
                ImageFileNames = new List<string>();
            ImageFileNames.Add(filenames);
        }

        /**
        * @brief 이미지 스케일 형식을 설정한다.
        */
        public void SetImageScaleType(ImageScaleType type)
        {
            ScaleType = type;
        }
        /**
        * @brief 이미지 스케일 형식을 리턴한다.
        */
        public ImageScaleType GetImageScaleType()
        {
            return ScaleType;
        }
        /**
        * @brief 이미지 버튼 형식을 설정한다.
        */
        public void SetImageButtonType(ImageButtonType type)
        {
            ButtonType = type;
        }
        /**
        * @brief 이미지 버튼 형식을 리턴한다.
        */
        public ImageButtonType GetImageButtonType()
        {
            return ButtonType;
        }
        /**
        * @brief 이미지 회전값을 설정한다.
        * @param angle : 각도(0 ~ 360)
        */
        public void SetImageRotate(float angle)
        {
            ImageRotate = angle;
        }
        /**
        * @brief 이미지 회전값을 리턴한다.
        */
        public float GetImageRotate()
        {
            return ImageRotate;
        }

        /**
        * @brief 시작시 보이지 않음 속성을 리턴한다.
        */
        public bool GetNotShowAtStart()
        {
            return NotShowAtStart;
        }
        /**
        * @brief 시작시 보이지 않음 속성을 설정한다.
        */
        public void SetNotShowAtStart(bool notShow)
        {
            NotShowAtStart = notShow;
        }

        /**
        * @brief 개체의 화면 출력을 위해 StackPanel에 Geometry를 생성하는 가상 함수.
        * @param dc : 대상 Panel
        * @details A. BitmapImage 개체를 생성하고 UriSource에 파일명을 설정한다.
        * @n B. ImageBrush를 생성하고 그 소스를 위의 BitmapImage로 설정한다.
        * @n C. RectangleGeometry를 생성하고 좌표를 설정한다.
        * @n D. Path를 생성하고 채우기 속성에 브러시를, Data에 RectangleGeometry를 설정한다.
        * @n E. 준비된 Path를 StackPanel에 추가한다.
        * @n F. base class의 CreateDrawing()을 호출한다.
        */
        public override void CreateDrawing(Canvas dc)
        {
            /*
            BitmapImage bi = new BitmapImage();
            bi.BeginInit();
            bi.UriSource = new Uri("smiley_stackpanel.PNG", UriKind.Absolute);
            bi.EndInit();

            Image image = new Image();
            image.Source = bi;
            image.Stretch = Stretch.Fill;

            //image.Clip;
            image.Opacity = 1.0;
            */

            if (ImageFileNames != null)
            {
                BitmapImage bi = new BitmapImage();
                bi.BeginInit();
                bi.UriSource = new Uri(ImageFileNames[0], UriKind.Absolute);
                bi.EndInit();

                ImageBrush brush = new ImageBrush();
                brush.ImageSource = bi;

                RectangleGeometry rg = new RectangleGeometry();
                rg.Rect = GetBounds();

                Path path = new Path();
                path.Data = rg;
                path.Fill = brush;

                dc.Children.Add(path);

                // draw label textbox
                CreateDrawingText(dc);
            }
            else
            {
                base.CreateDrawing(dc);
            }
        }

    }
}
