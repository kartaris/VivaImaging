/**
* @file Document.cs
* @date 2017.05
* @brief PageBuilder for Windows Document class file
*/
using System.Collections.Generic;
using System.Windows;
using System.Windows.Media;
using System.Linq;
using System.Xml.Serialization;
using System;

namespace PageBuilder.Data
{
    [XmlRootAttribute("PageBuilderDocument", Namespace = "http://clbee.com", IsNullable = false)]

    /** 
    * @enum PageType
    * @brief 페이지 종류
    */
    public enum PageType
    {
        IPHONE_768_1024,
        IPHONE_1024_768,
        HD_800_1280,
        HD_1280_800,
        FHD_1080_1920,
        FHD_1920_1080,

        MAX_PAGE_TYPE
    };

    /**
    * @class Document
    * @brief 문서 정보를 나타내는 클래스
    */
    public class Document
    {
        /** predefined page size in pixel*/
        [XmlIgnore]
        public Size[] DefPageSize =
        {
            new Size(768, 1024),
            new Size(1024, 768),
            new Size(800, 1280),
            new Size(1280, 800),
            new Size(1080, 1920),
            new Size(1920, 1080),
            new Size(1024, 1024)
        };

        static string[] PaperName =
        {
            "IPHONE_768_1024",
            "IPHONE_1024_768",
            "HD_800_1280",
            "HD_1280_800",
            "FHD_1080_1920",
            "FHD_1920_1080",
            "Nonamed"
        };


        /** Snap to Guideline values */
        public const int SNAP_TO_GUIDELINE = 1;
        /** Snap to object values */
        public const int SNAP_TO_OBJECT = 2;

        public const string CURRENT_DOCUMENT_VERSION = "1.5.0";

        static int NewFileIndex = 1;

        [XmlIgnore]
        public string FileName;
        bool hasFileName = false;

        // public int CurrentSpreadList;
        [XmlIgnore]
        public Spread CurrentSpread;
        [XmlIgnore]
        int CurrentSpreadIndex;

        [XmlIgnore]
        public List<Style> StyleList;

        public const double BASIC_UNIT_PER_INCH = 72;

        public DocumentInfo Info;

        public int PageWidth;
        public int PageHeight;

        public Color BackgroundColor;
        public string BackgroundImage;
        public string BackgroundSound;

        public int PageTransitionEffect;
        public int PageTransitionTime;
        public int PageTransitionZoom;

        // public string DocumentMemo; -> Info.Description

        // public int SnapMode; -> Info.Snap

        public List<Spread> Pages; //  BodySpreanList;

        /**
        * @brief 픽셀 단위를 내부 데이터 단위로 변환한다.
        * @param x : 픽셀 단위값.
        * @return int : 데이터 단위값.
        */
        public int PIXEL_TO_LOGIC(int x)
        {
            return (int)(BASIC_UNIT_PER_INCH * x / 96);
        }
        public double PIXEL_TO_LOGIC(double x)
        {
            return (BASIC_UNIT_PER_INCH * x / 96);
        }
        public void PIXEL_TO_LOGIC(ref Point pt)
        {
            pt.X = PIXEL_TO_LOGIC(pt.X);
            pt.Y = PIXEL_TO_LOGIC(pt.Y);
        }
        public void PIXEL_TO_LOGIC(ref Rect rect)
        {
            rect.X = PIXEL_TO_LOGIC(rect.X);
            rect.Y = PIXEL_TO_LOGIC(rect.Y);
            rect.Width = PIXEL_TO_LOGIC(rect.Width);
            rect.Height = PIXEL_TO_LOGIC(rect.Height);
        }

        /**
        * @brief 내부 데이터 단위를 픽셀 단위로 변환한다.
        * @param x : 데이터 단위값.
        * @return int : 픽셀 단위값.
        */
        public int LOGIC_TO_PIXEL(int x)
        {
            return (int)(96 * x / BASIC_UNIT_PER_INCH);
        }
        public double LOGIC_TO_PIXEL(double x)
        {
            return (96 * x / BASIC_UNIT_PER_INCH);
        }
        public void LOGIC_TO_PIXEL(ref Rect rect)
        {
            rect.X = LOGIC_TO_PIXEL(rect.X);
            rect.Y = LOGIC_TO_PIXEL(rect.Y);
            rect.Width = LOGIC_TO_PIXEL(rect.Width);
            rect.Height = LOGIC_TO_PIXEL(rect.Height);
        }

        public Document()
        {
            
        }

        public void SetFileName(string pathname)
        {
            FileName = pathname;
            hasFileName = true;
        }

        public bool HasFileName()
        {
            return hasFileName;
        }

        public void Clear()
        {
            if (Pages != null)
            {
                Pages.Clear();
            }
            if (StyleList != null)
            {
                StyleList.Clear();
            }
            CurrentSpread = null;
            CurrentSpreadIndex = -1;
        }

        /**
        * @brief 새 문서를 생성한다.
        * @param type : 페이지 형식
        * @details A. 기본 파일명을 설정한다.
        * @n B. PageWidth, PageHeight를 픽셀 단위로 계산하고, 내부 데이터 좌표로 변환한다.
        * @n C. 배경색, 화면 전환 효과 등 기본값을 설정한다.
        * @n D. Pages 생성하고 첫번째 Spread를 생성한다.
        */
        public void NewDocument(PageType type)
        {
            FileName = "New Document " + NewFileIndex;
            hasFileName = false;
            NewFileIndex++;

            if (Info == null)
                Info = new DocumentInfo();

            if (type < PageType.MAX_PAGE_TYPE)
            {
                PageWidth = (int)DefPageSize[(int)type].Width;
                PageHeight = (int)DefPageSize[(int)type].Height;
                Info.PaperName = PaperName[(int)type];
            }
            else
            {
                PageWidth = 1024;
                PageHeight = 1024;
                Info.PaperName = PaperName[(int)PageType.MAX_PAGE_TYPE];
            }

            Info.PaperSize.Width = PageWidth;
            Info.PaperSize.Height = PageHeight;

            // convert to twips_96
            PageWidth = PIXEL_TO_LOGIC(PageWidth);
            PageHeight = PIXEL_TO_LOGIC(PageHeight);

            BackgroundColor = Color.FromArgb(0xFF, 0xFF, 0xFF, 0xFF);
            PageTransitionEffect = 0;
            PageTransitionTime = 0;
            PageTransitionZoom = 100;

            // ready Body Spread List
            Pages = new List<Spread>();
            string name = string.Format("Menu Page");
            Pages.Add(new Spread(PageWidth, PageHeight, 1, name, Spread.GRAPHIC_PAGE|Spread.MENU_PAGE));

            name = string.Format("Page {0}", 1);
            Pages.Add(new Spread(PageWidth, PageHeight, 1, name, Spread.GRAPHIC_PAGE));

            CurrentSpread = Pages[1];
            CurrentSpreadIndex = 1;

            // ready Menu Spread List
            // MenuSpreadList = new List<Spread>();
        }

        /**
        * @brief 파일에서 문서를 로드한 후, 초기화 작업을 한다.
        */
        public void InitializeAfterLoad()
        {
            if ((Pages != null) && (Pages.Count > 0))
            {
                CurrentSpreadIndex = Info.WorkingPage;
                if (CurrentSpreadIndex > Pages.Count)
                    CurrentSpreadIndex = Pages.Count - 1;

                CurrentSpread = Pages.ElementAt(CurrentSpreadIndex);
            }
        }

        /**
        * @brief 파일에 문서를 저장할 때 필요한 정보를 업데이트 한다.
        */
        public void UpdateInfoBeforeSave()
        {
            DateTime date = DateTime.Now;

            if ((Info.Created == null) || (Info.Created.Length == 0))
                Info.Created = date.ToString();

            Info.Saved = date.ToString();
            Info.Pages = Pages.Count();
            Info.WorkingPage = CurrentSpreadIndex;
            Info.Version = CURRENT_DOCUMENT_VERSION;
        }

        public bool AddNewSpread(int spreadType) // Spread.GRAPHIC_PAGE, Spread.TEXT_PAGE
        {
            CurrentSpreadIndex++;

            string name = string.Format("Page {0}", Pages.Count);
            Spread s = new Spread(PageWidth, PageHeight, 1, name, spreadType);
            Pages.Insert(CurrentSpreadIndex, s);

            CurrentSpread = Pages[CurrentSpreadIndex];
            return true;
        }

        public bool DeleteCurrentSpread()
        {
            if ((CurrentSpreadIndex > 0) && // cannot delete menu page
                (Pages.Count() > 2))
            {
                Pages.RemoveAt(CurrentSpreadIndex);

                if (CurrentSpreadIndex >= Pages.Count())
                    CurrentSpreadIndex = Pages.Count() - 1;

                CurrentSpread = Pages[CurrentSpreadIndex];
                return true;
            }
            return false;
        }

        public bool PreviousSpread()
        {
            if (CurrentSpreadIndex > 0)
            {
                CurrentSpreadIndex--;
                CurrentSpread = Pages[CurrentSpreadIndex];
                return true;
            }
            return true;
        }

        public bool NextSpread()
        {
            if (CurrentSpreadIndex < (Pages.Count() - 1))
            {
                CurrentSpreadIndex++;
                CurrentSpread = Pages[CurrentSpreadIndex];
                return true;
            }
            return true;
        }

        public string GetDocumentMemo()
        {
            return Info.Description; //  DocumentMemo;
        }

        public void SetDocumentMemo(string str)
        {
            // DocumentMemo = str;
            Info.Description = str;
        }

        public string GetBackgroundImage()
        {
            return BackgroundImage;
        }

        public void SetBackgroundImage(string filename)
        {
            BackgroundImage = filename;
        }

        public string GetBackgroundSound()
        {
            return BackgroundSound;
        }

        public void SetBackgroundSound(string filename)
        {
            BackgroundSound = filename;
        }

        public int GetPageTransitionEffect()
        {
            return PageTransitionEffect;
        }

        public void SetPageTransitionEffect(int effect)
        {
            PageTransitionEffect = effect;
        }

        public int GetPageTransitionTime()
        {
            return PageTransitionTime;
        }

        public void SetPageTransitionTime(int time)
        {
            PageTransitionTime = time;
        }

        public int GetPageTransitionZoom()
        {
            return PageTransitionZoom;
        }

        public void SetPageTransitionZoom(int zoom)
        {
            PageTransitionZoom = zoom;
        }

        /**
        * @brief 지정한 인덱스의 페이지로 이동한다.
        * @param index : 페이지 인덱스
        * @param addIfLast : 페이지가 없으면 새로 생성한다.
        */
        public bool GotoPage(int index, bool addIfLast)
        {
            int prev = CurrentSpreadIndex;
            while (index >= Pages.Count())
            {
                string name = string.Format("Page {0}", Pages.Count() + 1);
                Pages.Add(new Spread(PageWidth, PageHeight, 1, name, Spread.GRAPHIC_PAGE));
            }

            if ((index >= 0) && (index < Pages.Count()))
            {
                CurrentSpreadIndex = index;
                CurrentSpread = Pages[index];
            }

            return (prev != CurrentSpreadIndex);
        }

        public bool GotoPage(string name)
        {
            int index = 0;
            foreach(Spread spread in Pages)
            {
                if (spread.Name.CompareTo(name) == 0)
                {
                    if (CurrentSpreadIndex != index)
                    {
                        CurrentSpreadIndex = index;
                        CurrentSpread = Pages[index];
                        return true;
                    }
                    return false;
                }
                ++index;
            }
            return false;
        }

        /**
        * @brief 지정한 인덱스의 페이지를 삭제한다.
        * @param index : 페이지 인덱스
        * @return PBSpread : 대상 스프레드.
        */
        public Spread RemovePage(int index)
        {
            if ((index >= 0) && (index < Pages.Count()))
            {
                Spread sp = Pages[index];
                Pages.RemoveAt(index);
                return sp;
            }
            return null;
        }

        /**
        * @brief 대상 개체를 현재의 스프레드에 추가한다.
        * @param rect : 대상 개체의 좌표
        * @param type : 대상 개체의 종류
        * @param fill: 대상 개체의 채우기 속성
        * @param line : 대상 개체의 외곽선 속성
        * @param flag : 플래그
        */
        public int AddGraphicObject(Rect rect, GraphicType type, Fill fill, OutLine line, int flag)
        {
            return CurrentSpread.AddGraphicObject(rect, type, fill, line, flag);
        }

        /**
        * @brief 대상 개체를 현재의 스프레드에 추가한다.
        * @param sp : 대상 개체의 시작 좌표
        * @param ep : 대상 개체의 끝 좌표
        * @param type : 대상 개체의 종류
        * @param fill: 대상 개체의 채우기 속성
        * @param line : 대상 개체의 외곽선 속성
        * @param flag : 플래그
        */
        public int AddGraphicObject(Point sp, Point ep, GraphicType type, Fill fill, OutLine line, int flag)
        {
            return CurrentSpread.AddGraphicObject(sp, ep, type, fill, line, flag);
        }

        public int MoveObjects(List<int> objectId, EditHandleType type, Point offset)
        {
            return CurrentSpread.MoveObjects(objectId, type, offset);
        }

        public int ResizeObjects(List<int> objectId, EditHandleType type, Point offset)
        {
            return CurrentSpread.ResizeObjects(objectId, type, offset);
        }

        public int RemoveObjectsById(List<int> targetObjectId)
        {
            return CurrentSpread.RemoveObjectsById(targetObjectId);
        }

        public int HandleEdit(List<int> objectId, EditHandleType type, Point offset, int keyState)
        {
            return CurrentSpread.HandleEdit(objectId, type, offset, keyState);
        }

        public int AddGuideline(Point pt, bool bHorz)
        {
            return CurrentSpread.AddGuideline(pt, bHorz);
        }

        public bool MoveGuidelinePosition(bool bHorz, int targetIndex, Point amount)
        {
            return CurrentSpread.MoveGuidelinePosition(bHorz, targetIndex, amount);
        }

        public bool RemoveGuideline(bool bHorz, int targetIndex)
        {
            return CurrentSpread.RemoveGuideline(bHorz, targetIndex);
        }

        /**
        * @brief 지정한 objec_id에 해당하는 대상 개체를 리턴한다.
        * @param object_id : 대상 개체의 id
        * @return PBGraphic : 찾은 개체
        */
        public Graphic GetCurrentObject(int object_id)
        {
            return CurrentSpread.GetObject(object_id);
        }

        /**
        * @brief 현재의 Spread를 리턴한다.
        * @return PBSpread : 현재의 Spread 개체
        */
        public Spread GetCurrentSpread()
        {
            return CurrentSpread;
        }

        public int GetSelectedObjectsCount()
        {
            return CurrentSpread.GetSelectedObjectsCount();
        }

        public List<Graphic> GetSelectedObjects()
        {
            return CurrentSpread.GetSelectedObjects();
        }

        public EditHandleType GetSelectedObjectsHandle(Point pt, bool freeIfNotSelected, ref DragAction editActionData)
        {
            return CurrentSpread.GetSelectedObjectsHandle(pt, freeIfNotSelected, ref editActionData);
        }

        public int ClearSelect()
        {
            return CurrentSpread.FreeAllSelection();
        }

        public int AddSelectionBy(Rect rect, bool freeIfOut)
        {
            return CurrentSpread.AddSelectionBy(rect, freeIfOut);
        }

        public int SelectAll()
        {
            return CurrentSpread.SelectAll();
        }

    }
}
