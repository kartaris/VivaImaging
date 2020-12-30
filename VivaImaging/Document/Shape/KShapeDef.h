/**
* @file KShapeDef.h
* @date 2017.05
* @brief KShape for definition file
*/
#pragma once

typedef enum class tagMouseDragMode
{
  DragNone,
  CreateNewObject, // 새로운 개체 생성 위치 지정 모드
  MoveObject, // 개체 이동 모드
  ResizeObject, // 개체 크기 조정(8개 핸들)
  ResizeGroupObject, // 개체 그룹 크기 조정(8개 핸들)
  MoveGuideline, // 가이드라인 이동
  BlockSelect, // 블럭 지정 선택 모드.
  HandleObject, // 개체 핸들 조정(shape's special handle)
  CreateHorzGuideline, // 수평 가이드라인 추가
  CreateVertGuideline, // 수직 가이드라인 추가
  MouseDragMax
} MouseDragMode;

typedef enum class tagLineStyle
{
  LineHollow,
  LineSolid,
  LineDot,
  LineDash,
  LineDashDot,
  LineMax
} LineStyle;

typedef struct tagOutlineStyle
{
  /** 개체의 선 색 */
  DWORD Color;
  /** 선두께(1~100) */
  BYTE Width;
  /** dot-dash style */
  BYTE Style;
} OutlineStyle;

typedef enum class tagShapeType
{
  ShapeNone,
  ShapeLine,
  ShapePolygon,
  ShapeRectangle,
  ShapeTriangle,
  ShapeRhombus,
  ShapePentagon,
  ShapeStar,
  ShapeEllipse,
  ShapeTextbox,
  
  ShapeSmartLine,
  ShapeSmartRect,
  ShapeSmartGroup,
  ShapeSmartCallout,

  // KSST
	Rectangle = 101,
	RoundRectangle,
	Ellipse,
	Diamond,
	IsocelesTriangle,
	RightTriangle,
	Parallelogram,
	Trapezoid,
	Hexagon,
	Octagon,
	Plus,
	Star,
	Arrow,
	ThickArrow, //?
	HomePlate,
	Cube,
	Balloon,
	Seal,
	Arc,
	Line,
	Plaque,
	Can,
	Donut,
	TextSimple,
	TextOctagon,
	TextHexagon,
	TextCurve,
	TextWave,
	TextRing,
	TextOnCurve,
	TextOnRing,
	StraightConnector1 = 132,
	BentConnector2,
	BentConnector3,
	BentConnector4,
	BentConnector5,
	CurvedConnector2,
	CurvedConnector3,
	CurvedConnector4,
	CurvedConnector5,
	Callout1 = 141,
	Callout2,
	Callout3,
	AccentCallout1,
	AccentCallout2,
	AccentCallout3,
	BorderCallout1,
	BorderCallout2,
	BorderCallout3,
	AccentBorderCallout1,
	AccentBorderCallout2,
	AccentBorderCallout3,
	Ribbon = 153,
	Ribbon2,
	Chevron,
	Pentagon,
	NoSmoking,
	Seal8,
	Seal16,
	Seal32,
	WedgeRectCallout,
	WedgeRRectCallout,
	WedgeEllipseCallout,
	Wave = 164,
	FoldedCorner,
	LeftArrow,
	DownArrow,
	UpArrow,
	LeftRightArrow,
	UpDownArrow,
	IrregularSeal1,
	IrregularSeal2,
	LightningBolt,
	Heart,
	PictureFrame,
	QuadArrow,
	LeftArrowCallout = 177,
	RightArrowCallout,
	UpArrowCallout,
	DownArrowCallout,
	LeftRightArrowCallout,
	UpDownArrowCallout,
	QuadArrowCallout,
	Bevel = 184,
	LeftBracket,
	RightBracket,
	LeftBrace,
	RightBrace,
	LeftUpArrow,
	BentUpArrow,
	BentArrow,
	Seal24,
	StripedRightArrow,
	NotchedRightArrow,
	BlockArc = 195,
	SmileyFace,
	VerticalScroll,
	HorizontalScroll,
	CircularArrow,
	NotchedCircularArrow,
	UturnArrow,
	CurvedRightArrow,
	CurvedLeftArrow,
	CurvedUpArrow,
	CurvedDownArrow,
	CloudCallout = 206,
	EllipseRibbon,
	EllipseRibbon2,
	FlowChartProcess = 209,
	FlowChartDecision,
	FlowChartInputOutput,
	FlowChartPredefinedProcess,
	FlowChartInternalStorage,
	FlowChartDocument,
	FlowChartMultidocument,
	FlowChartTerminator,
	FlowChartPreparation,
	FlowChartManualInput,
	FlowChartManualOperation,
	FlowChartConnector,
	FlowChartPunchedCard,
	FlowChartPunchedTape,
	FlowChartSummingJunction,
	FlowChartOr,
	FlowChartCollate,
	FlowChartSort,
	FlowChartExtract,
	FlowChartMerge,
	FlowChartOfflineStorage,
	FlowChartOnlineStorage,
	FlowChartMagneticTape,
	FlowChartMagneticDisk,
	FlowChartMagneticDrum,
	FlowChartDisplay,
	FlowChartDelay,
	TextPlainText = 236,
	TextStop,
	TextTriangle,
	TextTriangleInverted,
	TextChevron,
	TextChevronInverted,
	TextRingInside,
	TextRingOutside,
	TextArchUpCurve,
	TextArchDownCurve,
	TextCircleCurve,
	TextButtonCurve,
	TextArchUpPour,
	TextArchDownPour,
	TextCirclePour,
	TextButtonPour,
	TextCurveUp,
	TextCurveDown,
	TextCascadeUp,
	TextCascadeDown,
	TextWave1,
	TextWave2,
	TextWave3,
	TextWave4,
	TextInflate,
	TextDeflate,
	TextInflateBottom,
	TextDeflateBottom,
	TextInflateTop,
	TextDeflateTop,
	TextDeflateInflate,
	TextDeflateInflateDeflate,
	TextFadeRight,
	TextFadeLeft,
	TextFadeUp,
	TextFadeDown,
	TextSlantUp,
	TextSlantDown,
	TextCanUp,
	TextCanDown,
	FlowChartAlternateProcess = 276,
	FlowChartOffpageConnector,
	Callout90 = 278,
	AccentCallout90,
	BorderCallout90,
	AccentBorderCallout90,
	LeftRightUpArrow,
	Sun = 283,
	Moon,
	BracketPair,
	BracePair,
	Seal4,
	DoubleWave,
	HostControl,
	TextBox,
	Group,
	CubeShadow = 300, // 300~315 ; 그림자 쉐입
	CanShadow,
	BevelShadow,
	CurvedRightArrowShadow,
	CurvedLeftArrowShadow,
	CurvedUpArrowShadow,
	CurvedDownArrowShadow,
	FlowChartMultidocumentShadow,
	FlowChartMagneticDiskShadow,
	FlowChartMagneticDrumShadow,
	RibbonShadow,
	Ribbon2Shadow,
	VerticalScrollShadow,
	HorizontalScrollShadow,
	EllipseRibbonShadow,
	EllipseRibbon2Shadow,
	SmileyFaceShadow,
	FoldedCornerShadow,
	StripedRightArrowShadow,
	ButtonHelp = 320,   //execution buttons
	ButtonInfo,
	ButtonNext,
	ButtonPrev,
	ButtonStart,
	ButtonEnd,
	ButtonUturn,
	ButtonDoc,
	ButtonSound,
	ButtonMove,
	ButtonUserSetting,
	ButtonHome,

  ShapeMaxx
} ShapeType;

// font style
#define FONT_BOLD 1
#define FONT_ITALIC 2
#define FONT_UNDERLINE 4

#define KSHAPE_SELECTED 1
#define KSHAPE_VISIBLE 2
#define KSHAPE_LOCKED 4

#define KSHAPE_STORED_FLAGS (KSHAPE_VISIBLE | KSHAPE_LOCKED)

#define SHAPE_DRAG_EDITING 8 // KDragAction에서 드래깅 편집중임.

extern TCHAR vivaXmlDocName[];

extern TCHAR tagDocInformation[]; // L"information"
extern TCHAR tagCreator[]; //  L"Creator"
extern TCHAR tagCreatorOID[]; //  L"CreatorOID"
extern TCHAR tagCreatedDate[]; // L"CreatedDate"
extern TCHAR tagLastModifier[]; // _T("LastModifier");
extern TCHAR tagLastModifierOID[]; // _T("LastModifierOID");
extern TCHAR tagLastModifiedDate[]; // _T("LastModifiedDate");

extern TCHAR tagFolder[]; // L"Folder"
extern TCHAR tagImage[]; // L"Image"
extern TCHAR tagName[];// L"Name"
extern TCHAR tagFileName[]; // L"FileName"
extern TCHAR tagEcmInfo[]; // L"EcmInfo"

extern TCHAR tagExtDocType[];
extern TCHAR tagExtDocTypeOid[];
extern TCHAR tagExtDocTypeName[];

extern TCHAR tagExtDocTypeColumn[];
extern TCHAR tagExtDocTypeColumnKey[];
extern TCHAR tagExtDocTypeColumnType[];
extern TCHAR tagExtDocTypeColumnName[];
extern TCHAR tagExtDocTypeColumnMaxLength[];
extern TCHAR tagExtDocTypeColumnValue[];

extern TCHAR tagMetaData[]; // L"MetaData"
extern TCHAR tagKey[]; // L"Key"

extern TCHAR tagAnnotates[];

extern TCHAR tagEllipse[]; // = L"Ellipse"
extern TCHAR tagRectangle[];
extern TCHAR tagSmartShape[];

extern TCHAR tagLeft[];
extern TCHAR tagRight[];
extern TCHAR tagTop[];
extern TCHAR tagBottom[];

extern TCHAR tagFill[];
extern TCHAR tagOutline[];
extern TCHAR tagColor[];
extern TCHAR tagWidth[];
extern TCHAR tagStyle[];

extern TCHAR tagLine[];
extern TCHAR tagStartX[];
extern TCHAR tagStartY[];
extern TCHAR tagEndX[];
extern TCHAR tagEndY[];

extern TCHAR tagNumberOfPoints[];
extern TCHAR tagClosed[];

extern TCHAR tagPolygon[];

extern TCHAR tagTextbox[];

extern TCHAR tagTextColor[];
extern TCHAR tagFontInfo[];
extern TCHAR tagFaceName[];
extern TCHAR tagPointSize[];
extern TCHAR tagFontBold[];
extern TCHAR tagFontItalic[];
extern TCHAR tagFontUnderline[];

extern LPTSTR tagBoolValue[];