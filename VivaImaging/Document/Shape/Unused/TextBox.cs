/**
* @file TextBox.cs
* @date 2017.05
* @brief PageBuilder for Windows TextBox class file
*/
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media;
using System.Xml.Serialization;

namespace PageBuilder.Data
{
    /**
    * @class TextBox
    * @brief RichText 개체 클래스
    */
    public class TextBox : Graphic
    {
        // scroll view
        //public Rect Scroll;

        [XmlElement(DataType = "hexBinary", Type = typeof(byte[]))]
        public object TextStream;

        /**
        * @brief TextBox class constructor
        */
        public TextBox() : base()
        {
        }

        /**
        * @brief TextBox class constructor
        * @param rect : 개체의 좌표
        * @param layerId : 개체의 소속 레이어 ID
        */
        public TextBox(Rect rect, int layerId) : base(rect, layerId)
        {
            //Scroll = new Rect(new Point(0, 0), rect.Size);
        }

        /**
        * @brief 개체의 종류를 리턴하는 가상함수
        */
        public override GraphicType ObjectType()
        {
            return GraphicType.GraphicTextBox;
        }

        /**
        * @brief 개체의 화면 출력을 위해 Canvas에 Geometry를 생성하는 가상 함수.
        * @param dc : 대상 Canvas
        */
        public override void CreateDrawing(Canvas dc)
        {
            //base.CreateDrawing(dc);
            TextBlock textBlock = new TextBlock();
            textBlock.Text = (Text == null) ? "click hear to edit" : UTF8Encoding.UTF8.GetString((byte[])TextStream);
            textBlock.Foreground = Brushes.Black;
            textBlock.Width = Width;
            textBlock.Height = Height;
            textBlock.TextWrapping = TextWrapping.Wrap;

            Canvas.SetLeft(textBlock, X);
            Canvas.SetTop(textBlock, Y);

            dc.Children.Add(textBlock);
        }

        /**
        * @brief RichTextBox의 Document가 저장된 xaml 데이터 스트림을 byte array로 TextStream에 저장한다.
        */
        public void SetTextStream(MemoryStream stream)
        {
            if (stream.Length > 0)
            {
                TextStream = stream.ToArray();
            }
            else
            {
                TextStream = null;
            }
            // 테스트로 임시 파일로 저장
            /*
            FileStream fs = new FileStream("C:\\temp\\abc.txt", FileMode.Create, System.IO.FileAccess.Write);
            stream.WriteTo(fs);
            fs.Close();
            */
        }

        /**
        * @brief byte array인 TextStream에서 스트림을 생성하여 리턴한다.
        */
        public MemoryStream GetStream()
        {
            if (TextStream != null)
            {
                MemoryStream stream = new MemoryStream((byte[])TextStream);
                stream.Position = 0;
                return stream;
            }
            return null;
        }


    }
}