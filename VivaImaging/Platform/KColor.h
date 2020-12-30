#pragma once

#define     Pie         	3.141592
#define     Pie_180     	(180.0 / 3.141592)
#define     DOTSIZE			4
#define     DOTSIZE1		5
#define     WDOTSIZE		8

#define		CMY_UNIT		100
#define		LSV_UNIT		100
#define		RGB_UNIT		255
#define		HUE_UNIT		360

#ifndef		COLOR_DEFINED

#define COLOR_RGB		0x01
#define COLOR_HSV		0x02
#define COLOR_HLS		0x04
#define COLOR_CMY		0x08
#define COLOR_KSI		0x10
#define COLOR_PAN		0x20
#define COLOR_CUS		0x40

#define	HSV_MAP		1
#define	HLS_MAP		2
#define	RGB_BOX		3
#define	CMY_BOX		4

typedef struct tagRGB_C		far *LP_RGB_C;
typedef struct tagCMYKC		far *LP_CMYKC;
typedef struct tagHSV_C		far *LP_HSV_C;
typedef struct tagHLS_C		far *LP_HLS_C;

typedef  struct tagCMYKC					// Unit is 100
    {
    BYTE        Cyn;
    BYTE        Mgn;
    BYTE        Yel;
    BYTE        Blk;
    } CMYKC;

typedef  struct	tagHSV_C
    {
    short       Hue;				// Unit is 360
    BYTE        Sat;				// Unit is 100
    BYTE        Val;				// Unit is 100
    } HSV_C;

typedef  struct tagHLS_C
    {
    short       Hue;				// Unit is 360
    BYTE        Lit;				// Unit is 100
    BYTE        Sat;				// Unit is 100
    } HLS_C;

typedef struct tagRGB_C
	{
	BYTE		Red;
	BYTE		Grn;
	BYTE		Blu;
	//BYTE		Tps;
	} RGB_C;

#endif // COLOR_DEFINED

RGB_C 	WINAPI ConvertRGB2CMY(RGB_C cur);
CMYKC 	WINAPI ConvertRGB2CMYK(RGB_C cur);
RGB_C	WINAPI ConvertCMY2RGB(RGB_C cur);
RGB_C 	WINAPI ConvertCMYK2RGB(CMYKC cur);
HLS_C 	WINAPI ConvertRGB2HLS(RGB_C cur);
HSV_C 	WINAPI ConvertRGB2HSV(RGB_C cur);
RGB_C 	WINAPI ConvertHLS2RGB(HLS_C cur);
RGB_C 	WINAPI ConvertHSV2RGB(HSV_C cur);
CMYKC	WINAPI ConvertCMYK2CMY(CMYKC cur);
CMYKC	WINAPI ConvertCMY2CMYK(CMYKC cur);
COLORREF WINAPI ConvertHSV2COLORREF(HSV_C cur);


#define MAKE_ARGB(a, r,g,b) ((COLORREF) ((((BYTE)(r)|((WORD)((BYTE)(g))<<8))|(((DWORD)(BYTE)(b))<<16)) | (((DWORD)(BYTE)(a))<<24)) )
