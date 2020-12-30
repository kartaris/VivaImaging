// KShapeSmartBase.h: interface for the KShapeSmartBase class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#if !defined(K_SMARTSHAPE_HEADER)
#define K_SMARTSHAPE_HEADER

#include "stdafx.h"
#include "KShapeBase.h"

typedef enum class tagBasicGraphElement
{
	None,
	RectElement,
	RoundRectangle,
	EllipseElement,
	PolygonElement,
} BasicGraphElement;

typedef enum class tagBasicElementColorType
{
	HOLLOW,
	NORMAL,
	LOW_BRIGHT,
	LOW_MEDIUM_BRIGHT,
	HIGH_BRIGHT,
	HIGH_MEDIUM_BRIGHT,
}BasicElementColorType;

/*
typedef enum class tagKSmartShapeType
{
	Min = 0,
	NotPrimitive = Min,
	Rectangle = 1,
	RoundRectangle = 2,
	Ellipse = 3,
	Diamond = 4,
	IsocelesTriangle = 5,
	RightTriangle = 6,
	Parallelogram = 7,
	Trapezoid = 8,
	Hexagon = 9,
	Octagon = 10,
	Plus = 11,
	Star = 12,
	Arrow = 13,
	ThickArrow = 14,
	HomePlate = 15,
	Cube = 16,
	Balloon = 17,
	Seal = 18,
	Arc = 19,
	Line = 20,
	Plaque = 21,
	Can = 22,
	Donut = 23,
	TextSimple = 24,
	TextOctagon = 25,
	TextHexagon = 26,
	TextCurve = 27,
	TextWave = 28,
	TextRing = 29,
	TextOnCurve = 30,
	TextOnRing = 31,
	StraightConnector1 = 32,
	BentConnector2 = 33,
	BentConnector3 = 34,
	BentConnector4 = 35,
	BentConnector5 = 36,
	CurvedConnector2 = 37,
	CurvedConnector3 = 38,
	CurvedConnector4 = 39,
	CurvedConnector5 = 40,
	Callout1 = 41,
	Callout2 = 42,
	Callout3 = 43,
	AccentCallout1 = 44,
	AccentCallout2 = 45,
	AccentCallout3 = 46,
	BorderCallout1 = 47,
	BorderCallout2 = 48,
	BorderCallout3 = 49,
	AccentBorderCallout1 = 50,
	AccentBorderCallout2 = 51,
	AccentBorderCallout3 = 52,
	Ribbon = 53,
	Ribbon2 = 54,
	Chevron = 55,
	Pentagon = 56,
	NoSmoking = 57,
	Seal8 = 58,
	Seal16 = 59,
	Seal32 = 60,
	WedgeRectCallout = 61,
	WedgeRRectCallout = 62,
	WedgeEllipseCallout = 63,
	Wave = 64,
	FoldedCorner = 65,
	LeftArrow = 66,
	DownArrow = 67,
	UpArrow = 68,
	LeftRightArrow = 69,
	UpDownArrow = 70,
	IrregularSeal1 = 71,
	IrregularSeal2 = 72,
	LightningBolt = 73,
	Heart = 74,
	PictureFrame = 75,
	QuadArrow = 76,
	LeftArrowCallout = 77,
	RightArrowCallout = 78,
	UpArrowCallout = 79,
	DownArrowCallout = 80,
	LeftRightArrowCallout = 81,
	UpDownArrowCallout = 82,
	QuadArrowCallout = 83,
	Bevel = 84,
	LeftBracket = 85,
	RightBracket = 86,
	LeftBrace = 87,
	RightBrace = 88,
	LeftUpArrow = 89,
	BentUpArrow = 90,
	BentArrow = 91,
	Seal24 = 92,
	StripedRightArrow = 93,
	NotchedRightArrow = 94,
	BlockArc = 95,
	SmileyFace = 96,
	VerticalScroll = 97,
	HorizontalScroll = 98,
	CircularArrow = 99,
	NotchedCircularArrow = 100,
	UturnArrow = 101,
	CurvedRightArrow = 102,
	CurvedLeftArrow = 103,
	CurvedUpArrow = 104,
	CurvedDownArrow = 105,
	CloudCallout = 106,
	EllipseRibbon = 107,
	EllipseRibbon2 = 108,
	FlowChartProcess = 109,
	FlowChartDecision = 110,
	FlowChartInputOutput = 111,
	FlowChartPredefinedProcess = 112,
	FlowChartInternalStorage = 113,
	FlowChartDocument = 114,
	FlowChartMultidocument = 115,
	FlowChartTerminator = 116,
	FlowChartPreparation = 117,
	FlowChartManualInput = 118,
	FlowChartManualOperation = 119,
	FlowChartConnector = 120,
	FlowChartPunchedCard = 121,
	FlowChartPunchedTape = 122,
	FlowChartSummingJunction = 123,
	FlowChartOr = 124,
	FlowChartCollate = 125,
	FlowChartSort = 126,
	FlowChartExtract = 127,
	FlowChartMerge = 128,
	FlowChartOfflineStorage = 129,
	FlowChartOnlineStorage = 130,
	FlowChartMagneticTape = 131,
	FlowChartMagneticDisk = 132,
	FlowChartMagneticDrum = 133,
	FlowChartDisplay = 134,
	FlowChartDelay = 135,
	TextPlainText = 136,
	TextStop = 137,
	TextTriangle = 138,
	TextTriangleInverted = 139,
	TextChevron = 140,
	TextChevronInverted = 141,
	TextRingInside = 142,
	TextRingOutside = 143,
	TextArchUpCurve = 144,
	TextArchDownCurve = 145,
	TextCircleCurve = 146,
	TextButtonCurve = 147,
	TextArchUpPour = 148,
	TextArchDownPour = 149,
	TextCirclePour = 150,
	TextButtonPour = 151,
	TextCurveUp = 152,
	TextCurveDown = 153,
	TextCascadeUp = 154,
	TextCascadeDown = 155,
	TextWave1 = 156,
	TextWave2 = 157,
	TextWave3 = 158,
	TextWave4 = 159,
	TextInflate = 160,
	TextDeflate = 161,
	TextInflateBottom = 162,
	TextDeflateBottom = 163,
	TextInflateTop = 164,
	TextDeflateTop = 165,
	TextDeflateInflate = 166,
	TextDeflateInflateDeflate = 167,
	TextFadeRight = 168,
	TextFadeLeft = 169,
	TextFadeUp = 170,
	TextFadeDown = 171,
	TextSlantUp = 172,
	TextSlantDown = 173,
	TextCanUp = 174,
	TextCanDown = 175,
	FlowChartAlternateProcess = 176,
	FlowChartOffpageConnector = 177,
	Callout90 = 178,
	AccentCallout90 = 179,
	BorderCallout90 = 180,
	AccentBorderCallout90 = 181,
	LeftRightUpArrow = 182,
	Sun = 183,
	Moon = 184,
	BracketPair = 185,
	BracePair = 186,
	Seal4 = 187,
	DoubleWave = 188,	
	HostControl = 201,
	TextBox = 202,
	Group = 203,
	CubeShadow = 300, // 300~315 ; 그림자 쉐입
	CanShadow = 301,
	BevelShadow = 302, 
	CurvedRightArrowShadow = 303,
	CurvedLeftArrowShadow = 304,
	CurvedUpArrowShadow = 305,
	CurvedDownArrowShadow = 306,
	FlowChartMultidocumentShadow = 307,
	FlowChartMagneticDiskShadow = 308,
	FlowChartMagneticDrumShadow = 309,
	RibbonShadow = 310,
	Ribbon2Shadow = 311,
	VerticalScrollShadow = 312,
	HorizontalScrollShadow = 313,
	EllipseRibbonShadow = 314,
	EllipseRibbon2Shadow = 315,
	SmileyFaceShadow = 316,
	FoldedCornerShadow = 317,
	StripedRightArrowShadow = 318,
	ButtonHelp = 350,   //execution buttons
	ButtonInfo = 351,
	ButtonNext = 352,
	ButtonPrev = 353,
	ButtonStart = 354,
	ButtonEnd = 355,
	ButtonUturn = 356, 
	ButtonDoc = 357	,
	ButtonSound = 358 ,
	ButtonMove = 359 ,
	ButtonUserSetting = 360,			
	ButtonHome = 361,		
	Max,
	Nil = 0x0FFF,
} ShapeType;

*/

#define SHAPE_ADJUST_NUMBER 8

// 정위치에 표시
#define SHAPE_HANDLE_OUT_MARGIN 0

// drawFlags
#define DRAW_FILL_AREA 1

// penBrushFlag
#define DRAW_WITH_PEN 1
#define DRAW_WITH_BRUSH 2

typedef struct tagShapeDrawerHelper
{
	RECT renderRect;
	BOOL isDominant;
	BasicElementColorType colorBright;
	KImageDrawer* pInfo;
	UINT penBrushFlag;
}ShapeDrawerHelper;


class KShapeSmartBaseObject : public KShapeBase
{
public:
	KShapeSmartBaseObject(ShapeType type = ShapeType::ShapeNone, LONG* adjust=NULL);

	virtual ~KShapeSmartBaseObject();

	static int GetItemCount(int category);
	static ShapeType GetShapeID(int indx, int category);

	static BOOL IsValidShapeID(ShapeType index);

	static int GetDefaultAdjustValue(ShapeType objType, LONG* pAdjustValue);
	//static int GetValidAdjustValue(LP_OBJECT obj, LONG* pAdjustValue);
	static int GetCalloutType(ShapeType nDefNum);

	static KShapeSmartBaseObject* CreateShapeObject(ShapeType objType);

	static KShapeSmartBaseObject* CreateShapeObject(ShapeType objType, RECT& area, DWORD fillcolor, OutlineStyle linestyle, LONG* adjust);

	static BOOL IsDominantOfGrpObject(ShapeType nObjType, int nIndex);
	static BOOL GetShadowShapeType(ShapeType type, ShapeType& shadowType);
	static BOOL isExtraFillShape(ShapeType type);

	virtual ShapeType GetShapeType() { return mType; }
	virtual int	GetChildNumber() { return 1; }
	virtual BasicGraphElement GetChild(int child, ShapeDrawerHelper& helper, LPRECT rect, CPOINT** lppPts, int* pnum, LONG* pAdjustValue) { return BasicGraphElement::None; }

	void drawRectangleElement(Graphics& g, ShapeDrawerHelper& helper, RECT& r, COLORREF color, OutlineStyle& outline);
	void drawPathElement(Graphics& g, ShapeDrawerHelper& helper, GraphicsPath& path, COLORREF color, OutlineStyle& outline);
	void drawRoundRectangleElement(Graphics& g, ShapeDrawerHelper& helper, RECT& rect, int rx, int ry, COLORREF color, OutlineStyle& outline);
	void drawEllipseElement(Graphics& g, ShapeDrawerHelper& helper, RECT& r, COLORREF color, OutlineStyle& outline);
	void drawPolygonElement(Graphics& g, ShapeDrawerHelper& helper, LP_CPOINT points, int pointNumber, COLORREF color, OutlineStyle& outline, BOOL isEvenOdd);

	virtual int GetAdjustHandlePoint(LPPOINT pnts) { return 0; }
	virtual BOOL SetAdjustHandlePoint(int idx, LPPOINT pnts, BOOL bSet, LONG* pAdjustValue) { return FALSE; }

#ifdef USE_XML_STORAGE
	virtual HRESULT Store(CComPtr<IXmlWriter> pWriter);
	virtual HRESULT StoreShapeData(CComPtr<IXmlWriter> pWriter) { return S_OK; }
#endif // USE_XML_STORAGE

	virtual BOOL LoadFromBufferExt(KMemoryStream& mf);
	virtual BOOL LoadShapeDataFromBufferExt(KMemoryStream& mf) { return TRUE; }

	virtual BOOL StoreToBufferExt(KMemoryStream& mf);
	virtual BOOL StoreShapeDataToBufferExt(KMemoryStream& mf) { return TRUE; }

	ShapeType mType;
	LONG mAdjustValue[SHAPE_ADJUST_NUMBER];
	WORD drawFlags;

	BOOL IsShadowType(ShapeType shadowType); // 그림자 타입인지를 검사

	virtual BOOL IsColorChange() { return FALSE; }
	virtual BOOL HasPen(int child) { return TRUE; }
	virtual BOOL HasBrush(int child) { return TRUE; }
	// virtual BasicElementColorType GetColorBright(int child) { return BasicElementColorType::NORMAL; }
	virtual BOOL IsEvenOddFill() { return TRUE; }

	static const POINT	_rectangle[];
	static const POINT	_diamond[];
	static const POINT	_octagon[];
	static const POINT	_triangle[];
	static const POINT	_hexagon[];
	static const POINT	_pluspoly[];
	static const POINT	_pentagon[];
	static const POINT	_arrow[];
	static const POINT	_leftarrow[];
	static const POINT	_uparrow[];
	static const POINT	_downarrow[];
	static const POINT	_leftrightarrow[];
	static const POINT	_updownarrow[];
	static const POINT	_quadarrow[];
	static const POINT	_leftrightuparrow[];
	static const POINT	_leftuparrow[];
	static const POINT	_bentuparrow[];
	static const POINT	_notchedrightarrow[];
	static const POINT	_chevron[];
	static const POINT	_RightArrowCallout[];
	static const POINT	_LeftArrowCallout[];
	static const POINT	_UpArrowCallout[];
	static const POINT	_DownArrowCallout[];
	static const POINT	_LeftRightArrowCallout[];
	static const POINT	_UpDownArrowCallout[];
	static const POINT	_QuadArrowCallout[];
	static const POINT	_FlowChartPredefinedProcess[];
	static const POINT	_FlowChartInternalStorage[];
	static const POINT	_FlowChartOffpageConnector[];
	static const POINT	_FlowChartPunchedCard[];
	static const POINT	_FlowChartCollate[];
	static const POINT	_FlowChartSort[];
	static const POINT	_FlowChartMerge[];
	static const POINT	_Seal4[];
	static const POINT	_Star[];
	static const POINT	_IrregularSeal1[];
	static const POINT	_IrregularSeal2[];
	static const POINT	_Cube[];
	static const POINT	_Bevel[];
	static const CPOINT _FlowChartMultidocumentShadow[];

	virtual void convertPoints(CPOINT* lpdest, const POINT* lpsource, int cnt, int width, int height);
	virtual void convertPoints(CPOINT* lpdest, const POINT* lpsource, int cnt, int width, int height, int offx, int offy);
	virtual void convertPoints(CPOINT* lpdest, const CPOINT* lpsource, int cnt, int width, int height);
	virtual void convertPoints(CPOINT* lpdest, const CPOINT* lpsource, int cnt, int width, int height, int offx, int offy);

	void ReadyPolygonFlag(CPOINT* lpPoint, int nCount);
	void RotatePt(int cx, int cy, int angle, int *x, int *y, int divide);
	void RotatePt(int cx, int cy, int angle, CPOINT* lppt);
	CPOINT* GetSealPoint(int& pnum, int num, long adjustValue, int width, int height);
};

class KShapeSmartLineObject : public KShapeSmartBaseObject
{
public:
	KShapeSmartLineObject(ShapeType type = ShapeType::ShapeNone);
	KShapeSmartLineObject(ShapeType type, POINT& sp, POINT& ep, OutlineStyle linestyle, LONG* adjust = NULL, BYTE start = 0, BYTE end = 0);
	virtual ~KShapeSmartLineObject();

	virtual ShapeType GetResizeType() { return ShapeType::ShapeLine; }

	virtual BOOL GetOutlineStyle(OutlineStyle& linestyle);
	virtual BOOL SetOutlineStyle(OutlineStyle& linestyle);

#ifdef USE_XML_STORAGE
	virtual HRESULT StoreShapeData(CComPtr<IXmlWriter> pWriter);
#endif // USE_XML_STORAGE

	virtual BOOL LoadShapeDataFromBufferExt(KMemoryStream& mf);
	virtual BOOL StoreShapeDataToBufferExt(KMemoryStream& mf);

	/** 시작점 좌표 */
	POINT StartPoint;
	/** 끝점 좌표 */
	POINT EndPoint;
	/** 개체의 선 정보 */
	OutlineStyle Outline;
	/** 개체의 화살표 정보 */
	BYTE StartArrow;
	BYTE EndArrow;
};

class KShapeSmartRectObject : public KShapeSmartBaseObject
{
public:
	KShapeSmartRectObject(ShapeType type = ShapeType::ShapeNone);
	KShapeSmartRectObject(ShapeType type, RECT& rect, DWORD fillcolor, OutlineStyle linestyle, LONG* adjust = NULL);
	virtual ~KShapeSmartRectObject();

	virtual ShapeType GetResizeType() { return ShapeType::ShapeRectangle; }

	virtual BOOL GetFillColor(DWORD& color);
	virtual BOOL GetOutlineStyle(OutlineStyle& linestyle);

	virtual BOOL SetFillColor(DWORD& color);
	virtual BOOL SetOutlineStyle(OutlineStyle& linestyle);

	virtual BasicGraphElement GetChild(int child, ShapeDrawerHelper& helper, LPRECT rect, CPOINT** lppPts, int* pnum, LONG* pAdjustValue);
	
	int GetAdjustHandlePoint(LPPOINT pnts);
	int GetAdjustHandlePoint(LPPOINT pnts, RECT& rect, LONG* pAdjustValue);

	virtual BOOL SetAdjustHandlePoint(int idx, LPPOINT pnts, BOOL bSet, LONG* pAdjustValue);

	virtual BOOL Move(POINT& offset);

	virtual BOOL HandleEdit(EditHandleType handleType, POINT& handle, int keyState);
	virtual EditHandleType HitObjectsHandle(KImageDrawer& drawer, POINT& pt);
	virtual EditHandleType HitOver(KImageDrawer& drawer, POINT& pt);

	virtual BOOL HitOver(POINT& pt);

	virtual void GetRectangle(RECT& rect);
	virtual void GetNormalRectangle(RECT& rect);

	virtual RECT GetBounds(KImageDrawer* drawer);
	virtual RECT GetBounds(RECT& bound, KImageDrawer* drawer);

	virtual BOOL SetRectangle(RECT& rect);
	virtual void AdjustShapeBound(RECT& rect, long* pAdjustValue);

	void drawShape(Graphics& g, KImageDrawer& info, RECT& r, int flag, LONG* pAdjustValue = NULL);

	virtual void Draw(Graphics& g, KImageDrawer& info, int flag);

	virtual void OnRenderRubber(Graphics& g, KDragAction* drag, KImageDrawer& info, int flag);
	virtual void GetDragBounds(KDragAction* drag, int flag, RECT& rect);
	virtual void OnDragEnd(KDragAction* drag, POINT& amount);
	virtual void GetDragBounds(RECT& bound, KDragAction* drag);

#ifdef USE_XML_STORAGE
	virtual HRESULT StoreShapeData(CComPtr<IXmlWriter> pWriter);
#endif // USE_XML_STORAGE

	virtual BOOL LoadShapeDataFromBufferExt(KMemoryStream& mf);
	virtual BOOL StoreShapeDataToBufferExt(KMemoryStream& mf);

	RECT Rect;
	OutlineStyle Outline;
	DWORD Fill;

	BOOL SetAdjustHandlePointHelper(LPPOINT pnts, BOOL bSet, LONG* pAdjustValue, int nAdjustIndex, int nXLog, int nYLog, int nMin, int nMax);
	void SetValidAdjust(LONG* pAdjustValue, int nAdjIndex, int nValue, BOOL bSet, BOOL& rtn);

};

class KShapeSmartGroupObject : public KShapeSmartRectObject
{
public:
	KShapeSmartGroupObject(ShapeType type = ShapeType::ShapeNone);
	KShapeSmartGroupObject(ShapeType type, RECT& rect, DWORD fillcolor, OutlineStyle linestyle, LONG* adjust=NULL);
	virtual ~KShapeSmartGroupObject();

	virtual ShapeType GetResizeType() { return ShapeType::ShapeRectangle; }

	virtual BOOL IsColorChange();
	virtual BOOL HasBrush(int child);
	virtual int GetChildNumber();
	virtual BasicGraphElement GetChild(int child, ShapeDrawerHelper& helper, LPRECT rect, CPOINT** lppPts, int* pnum, LONG* pAdjustValue);
	// virtual BasicElementColorType GetColorBright(int child);

	// static BOOL CalculateFullEllipseByLogicRect(LP_OBJECT obj, LPRECT cood, LPRECT fullrect);

	static CPOINT _Can[];
	static CPOINT _FoldedCorner[];
	static CPOINT _SmileMouse[];
	static CPOINT _NoSmoking[];
	static CPOINT _Heart[];
	static CPOINT _LightningBolt[];
	static CPOINT _Sun[];
	static CPOINT _UturnArrow[];
	static CPOINT _FlowChartDocument[];
	static CPOINT _FlowChartMultidocument[];
	static CPOINT _FlowChartSummingJunction[];
	static CPOINT _FlowChartOr[];
	static CPOINT _FlowChartMagneticTape[];
	static CPOINT _BannerWave[];
	static CPOINT _BannerDoubleWave[];
	static CPOINT _HorizontalScrollBanner2[];
	static CPOINT _HorizontalScrollBanner4[];
	BOOL IsEvenOddFill();
};

class KShapeSmartCalloutObject : public KShapeSmartRectObject
{
public:
	KShapeSmartCalloutObject(ShapeType type = ShapeType::ShapeNone);
	KShapeSmartCalloutObject(ShapeType type, RECT& rect, DWORD fillcolor, OutlineStyle linestyle, LONG* adjust=NULL);
	virtual ~KShapeSmartCalloutObject();

	virtual BOOL HasPen(int child);
	virtual BOOL HasBrush(int child);
	virtual int GetChildNumber();
	virtual BasicGraphElement GetChild(int child, ShapeDrawerHelper& helper, LPRECT rect, CPOINT** lppPts, int* pnum, LONG* pAdjustValue);

	virtual BOOL SetAdjustHandlePoint(int idx, LPPOINT pnts, BOOL bSet, LONG* pAdjustValue);

	void SetCallout90Adjust(LPPOINT pnts, LONG* pAdjustValue, int idx, BOOL bSet, BOOL& rtnX, BOOL& rtnY);
	void SetCallout12Adjust(LPPOINT pnts, LONG* pAdjustValue, int idx, BOOL bSet, BOOL& rtnX, BOOL& rtnY);
	void SetCallout3Adjust(LPPOINT pnts, LONG* pAdjustValue, int idx, BOOL bSet, BOOL& rtnX, BOOL& rtnY);

	static CPOINT _CloudCallout[];
};


#if 0
class KShapeSmart : public KShapeBase
{
public:	
	KShapeSmart();
	virtual ~KShapeSmart();

	static void AddHandleArea(LP_OBJECT obj, LP_RECTANGLE rt);
	static void CheckDefaultCoord(ShapeType index, LP_OBJECT obj, BOOL bDefaultSize);
	static BOOL IsDefaultColor(ShapeType index);
	static BOOL IsFixedColor(ShapeType index);
	static BOOL GetDefaultFillBrush(ShapeType index, int nChild, LP_FILLBRUSH brush, BOOL bInit=TRUE);
	static BOOL GetDefaultLinePen(ShapeType index, int nChild, LP_LINEPEN pen, BOOL bInit=TRUE);

	static BOOL IsAngleLengthBase(int shapeType);
	static int GetAdjustHandlePoint(LP_OBJECT obj, LPPOINT pnts);
	static BOOL SetAdjustHandlePoint(LP_OBJECT obj, int idx, LPPOINT pnts, BOOL bSet);
	static BOOL SetAdjustHandlePointHelper(LP_OBJECT obj, LPPOINT pnts, BOOL bSet, int nAdjustIndex, int nXLog, int nYLog, int nMin, int nMax);
	static int GetCalloutType(int nDefNum);
	static void SetValidAdjust(LP_OBJECT obj, int nAdjIndex, int nValue, BOOL bSet, BOOL &rtn);
	static void SetCallout90Adjust(LP_OBJECT obj, LPPOINT pnts, int idx, BOOL bSet, BOOL &rtnX, BOOL &rtnY);
	static void SetCallout12Adjust(LP_OBJECT obj, LPPOINT pnts, int idx, BOOL bSet, BOOL &rtnX, BOOL &rtnY);
	static void SetCallout3Adjust(LP_OBJECT obj, LPPOINT pnts, int idx, BOOL bSet, BOOL &rtnX, BOOL &rtnY);

	int		GetCategoryCount();
	KString	GetCategoryName(int idx);
	int		SelectCategory(int idx);
	int		GetItemCount(int category = -1);
	KString GetItemName(int category, int index);
	void AdjustIconObjectCoord(LP_OBJECT obj);
	WORD	GetShapeID(int indx, int category = -1);

	LP_OBJECT MakeGroupPolygon(LP_OBJECT obj, BOOL noLabel= FALSE, LP_OBJDRAW_INFO info = NULL);
	BOOL GetShadowShapeType(short type, ShapeType& shadowType); // 그룹으로 바뀌는 쉐입들의 타입이 인자로 들어오면, 그에 해당하는 그림자 타입(KSST_)을 넘겨준다.
	BOOL isExtraFillShape(LP_OBJECT obj);

	KShapeSmartBase* CreateShapeObj(ShapeType objType, CRect& area, LONG* adjust);

	LP_OBJECT MakeSimpleChildObject(KShapeSmartBase* object, CRect& rect, int ntype, CRect& cr, CPOINT* ppoint, int pnum, BOOL bHasPen);
	LP_OBJLINK MakeGroupObject();
	LP_OBJLINK MakeRectangleObject(LP_RECTANGLE rt);
	LP_OBJLINK MakeEllipseObject(LP_RECTANGLE rt);
	LP_OBJLINK MakePolygonObject(CRect& rect, LP_CPOINT pp, int nPoint, short fillType=EVEN_ODD_FILL);
	// LP_OBJLINK MakePolygonObject(CRect& rect, POINT* ppoint, int pcnt, int* pArray, short fillType=EVEN_ODD_FILL);



	int		m_nActiveShape;
	int		m_nNumberOf;

};
#endif

#endif // !defined(K_SMARTSHAPE_HEADER)
