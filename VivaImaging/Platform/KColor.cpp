#include "stdafx.h"
#include <math.h>

#include "KColor.h"

static int HLSValue(long m1, long m2, int Hue);
static int GetMaxFrom3(int x1, int x2, int x3);
static int GetMinFrom3(int x1, int x2, int x3);

// CMY & CMYK unit is 100,		94, 4, 25
// Saturation, Value, Light Unit is 100

RGB_C WINAPI ConvertRGB2CMY(RGB_C cur)
{
  RGB_C	nclr;

  nclr.Red = (BYTE) ((unsigned int) (RGB_UNIT - cur.Red) * CMY_UNIT / RGB_UNIT);
  nclr.Grn = (BYTE) ((unsigned int) (RGB_UNIT - cur.Grn) * CMY_UNIT / RGB_UNIT);
  nclr.Blu = (BYTE) ((unsigned int) (RGB_UNIT - cur.Blu) * CMY_UNIT / RGB_UNIT);
  return(nclr);
}

CMYKC WINAPI ConvertRGB2CMYK(RGB_C cur)
{
  CMYKC nclr;
  RGB_C tmp;

  tmp.Red = (BYTE) ((unsigned int) (RGB_UNIT - cur.Red) * CMY_UNIT / RGB_UNIT);
  tmp.Grn = (BYTE) ((unsigned int) (RGB_UNIT - cur.Grn) * CMY_UNIT / RGB_UNIT);
  tmp.Blu = (BYTE) ((unsigned int) (RGB_UNIT - cur.Blu) * CMY_UNIT / RGB_UNIT);

	nclr.Blk = tmp.Red;
	if(nclr.Blk > tmp.Grn)
		nclr.Blk = tmp.Grn;
	if(nclr.Blk > tmp.Blu)
		nclr.Blk = tmp.Blu;

	nclr.Cyn = tmp.Red - nclr.Blk;
	nclr.Mgn = tmp.Grn - nclr.Blk;
	nclr.Yel = tmp.Blu - nclr.Blk;
  return(nclr);
}

RGB_C WINAPI ConvertCMY2RGB(RGB_C cur)
{
  RGB_C nclr;

  nclr.Red = (BYTE) (RGB_UNIT - (unsigned int) cur.Red * RGB_UNIT / CMY_UNIT);
  nclr.Grn = (BYTE) (RGB_UNIT - (unsigned int) cur.Grn * RGB_UNIT / CMY_UNIT);
  nclr.Blu = (BYTE) (RGB_UNIT - (unsigned int) cur.Blu * RGB_UNIT / CMY_UNIT);
  return(nclr);
}

RGB_C WINAPI ConvertCMYK2RGB(CMYKC cur)
{
  RGB_C			nclr;
	unsigned int	c, m, y;

  c = (unsigned int) cur.Blk + cur.Cyn;
  m = (unsigned int) cur.Blk + cur.Mgn;
  y = (unsigned int) cur.Blk + cur.Yel;
	if(c > 100)
		c = 100;
	if(m > 100)
		m = 100;
	if(y > 100)
		y = 100;

  nclr.Red = (BYTE) (RGB_UNIT - (unsigned int) c * RGB_UNIT / CMY_UNIT);
  nclr.Grn = (BYTE) (RGB_UNIT - (unsigned int) m * RGB_UNIT / CMY_UNIT);
  nclr.Blu = (BYTE) (RGB_UNIT - (unsigned int) y * RGB_UNIT / CMY_UNIT);
  return(nclr);
}


HLS_C WINAPI ConvertRGB2HLS(RGB_C cur)
{
  HLS_C	nclr;
  int 	Max, Min;
  float	rc, gc, bc;

  Max = GetMaxFrom3(cur.Red, cur.Grn, cur.Blu);
  Min = GetMinFrom3(cur.Red, cur.Grn, cur.Blu);
  nclr.Lit = (BYTE) ((unsigned int) ((Max + Min) / 2) * LSV_UNIT / RGB_UNIT);        // lightness

  if(Max == Min)
  {
		nclr.Hue = 0;				   // Hue
    nclr.Sat = 0;                   // Saturation
  }
  else
  {
    if(nclr.Lit <= 128)
      nclr.Sat = (BYTE) ((float) (Max - Min) * LSV_UNIT / (Max + Min)); // Saturation
    else
      nclr.Sat = (BYTE) ((float) (Max - Min) * LSV_UNIT / (RGB_UNIT * 2 - Max - Min)); // Saturation

    rc = (float) (Max - cur.Red) / (Max - Min);
    gc = (float) (Max - cur.Grn) / (Max - Min);
    bc = (float) (Max - cur.Blu) / (Max - Min);
    if(Max == (int) cur.Red)
      rc = (bc - gc);
    else if(Max == (int) cur.Grn)
      rc = (2 + rc - bc);
    else
      rc = (4 + gc - rc);
    rc = rc * 60;
    if(rc < 0)
      rc = rc + 360;
    nclr.Hue = (int) rc;
  }

  return(nclr);
}

HSV_C WINAPI ConvertRGB2HSV(RGB_C cur)
{
  HSV_C nclr;
  int Max, Min;
  int h;
  float rc, gc, bc;

  Max = GetMaxFrom3(cur.Red, cur.Grn, cur.Blu);
  Min = GetMinFrom3(cur.Red, cur.Grn, cur.Blu);

  nclr.Val = (BYTE) ((long) Max * LSV_UNIT / RGB_UNIT);
  if(Max != 0)
    nclr.Sat = (BYTE) ((long) LSV_UNIT * (Max - Min) / Max);
  else
    nclr.Sat = 0;

  if(nclr.Sat == 0)
    nclr.Hue = 0;
  else
  {
    rc = (float) (Max - cur.Red) / (Max - Min);
    gc = (float) (Max - cur.Grn) / (Max - Min);
    bc = (float) (Max - cur.Blu) / (Max - Min);

    if(cur.Red == (BYTE) Max)
        h = (int) ((bc - gc) * 60);
    else if(cur.Grn == (BYTE) Max)
        h = 120 + (int) ((rc - bc) * 60);
    else if(cur.Blu == (BYTE) Max)
        h = 240 + (int) ((gc - rc) * 60);

    if(h < 0)
        h = h + 360;

    nclr.Hue = h;
  }
  return(nclr);
}

RGB_C WINAPI ConvertHLS2RGB(HLS_C cur)
{
  RGB_C 	nclr;
  long 	m1, m2;
  int		h;

  h = cur.Hue;
  if(h >= 360)
      h = h - 360;
  if(h < 0)
      h = h + 360;

  if(cur.Lit <= LSV_UNIT / 2)
      m2 = (unsigned long) cur.Lit * (LSV_UNIT + cur.Sat) / LSV_UNIT;
  else
      m2 = (unsigned long) cur.Lit + cur.Sat - (unsigned long) (cur.Lit * cur.Sat) / LSV_UNIT;

  m1 = 2 * cur.Lit - m2;

  if(cur.Sat == 0)
  {
    nclr.Red = (BYTE) ((unsigned long) cur.Lit * RGB_UNIT / LSV_UNIT);
    nclr.Grn = nclr.Red;
    nclr.Blu = nclr.Red;
  }
  else
  {
    nclr.Red = (BYTE) HLSValue(m1, m2, h + 120);
    nclr.Grn = (BYTE) HLSValue(m1, m2, h);
    nclr.Blu = (BYTE) HLSValue(m1, m2, h - 120);
    nclr.Red = (BYTE) ((unsigned long) nclr.Red * RGB_UNIT / LSV_UNIT);
    nclr.Grn = (BYTE) ((unsigned long) nclr.Grn * RGB_UNIT / LSV_UNIT);
    nclr.Blu = (BYTE) ((unsigned long) nclr.Blu * RGB_UNIT / LSV_UNIT);
  }
  return(nclr);
}

static int HLSValue(long m1, long m2, int Hue)
{
  int rev;

  if(Hue >= 360)
      Hue = Hue - 360;
  if(Hue < 0)
      Hue = Hue + 360;

  if(Hue < 60)
      rev = (int) (m1 + (m2 - m1) * Hue / 60);
  else if(Hue < 180)
      rev = (int) m2;
  else if(Hue < 240)
      rev = (int) (m1 + (m2 - m1) * (240 - Hue) / 60);
  else
      rev = (int) m1;

	if(rev > LSV_UNIT)
		rev = LSV_UNIT;
  return( rev );
}

RGB_C WINAPI ConvertHSV2RGB(HSV_C cur)
{
	RGB_C nclr;
	int   h, f;
	long  p, q, t;

	if(cur.Sat == 0)
	{
		nclr.Red = (BYTE) ((unsigned long) cur.Val * RGB_UNIT / LSV_UNIT);
		nclr.Grn = (BYTE) ((unsigned long) cur.Val * RGB_UNIT / LSV_UNIT);
		nclr.Blu = (BYTE) ((unsigned long) cur.Val * RGB_UNIT / LSV_UNIT);
	}
	else
	{
		if(cur.Hue >= 360)
			cur.Hue -= 360;
		if(cur.Hue < 0)
			cur.Hue += 360;

		h = (int) (cur.Hue / 60);
		f = (int) (cur.Hue % 60);
		p = (unsigned long) cur.Val * (LSV_UNIT - (unsigned long) cur.Sat) / LSV_UNIT;
		q = (unsigned long) cur.Val * (LSV_UNIT - (unsigned long) cur.Sat * f / 60) / LSV_UNIT;
		t = (unsigned long) cur.Val * (LSV_UNIT - (unsigned long) cur.Sat * (60 - f) / 60) / LSV_UNIT;
		p = p * RGB_UNIT / LSV_UNIT;
		q = q * RGB_UNIT / LSV_UNIT;
		t = t * RGB_UNIT / LSV_UNIT;

		switch (h)
			{
			case 0:
				nclr.Red = (BYTE) ((unsigned long) cur.Val * RGB_UNIT / LSV_UNIT);
				nclr.Grn = (BYTE) t;
				nclr.Blu = (BYTE) p;
				break;
			case 1:
				nclr.Red = (BYTE) q;
				nclr.Grn = (BYTE) ((unsigned long) cur.Val * RGB_UNIT / LSV_UNIT);
				nclr.Blu = (BYTE) p;
				break;
			case 2:
				nclr.Red = (BYTE) p;
				nclr.Grn = (BYTE) ((unsigned long) cur.Val * RGB_UNIT / LSV_UNIT);
				nclr.Blu = (BYTE) t;
				break;
			case 3:
				nclr.Red = (BYTE) p;
				nclr.Grn = (BYTE) q;
				nclr.Blu = (BYTE) ((unsigned long) cur.Val * RGB_UNIT / LSV_UNIT);
				break;
			case 4:
				nclr.Red = (BYTE) t;
				nclr.Grn = (BYTE) p;
				nclr.Blu = (BYTE) ((unsigned long) cur.Val * RGB_UNIT / LSV_UNIT);
				break;
			case 5:
				nclr.Red = (BYTE) ((unsigned long) cur.Val * RGB_UNIT / LSV_UNIT);
				nclr.Grn = (BYTE) p;
				nclr.Blu = (BYTE) q;
				break;
			default:
				break;
    	}
	}
	return(nclr);
}

static int GetMaxFrom3(int x1, int x2, int x3)
{
  int mv;

  mv = x1;
  if(mv < x2)
      mv = x2;
  if(mv < x3)
      mv = x3;
  return(mv);
}

static int GetMinFrom3(int x1, int x2, int x3)
{
  int mv;

  mv = x1;
  if(mv > x2)
      mv = x2;
  if(mv > x3)
      mv = x3;
  return(mv);
}

CMYKC WINAPI ConvertCMYK2CMY(CMYKC cur)
{
	CMYKC	nclr;

  nclr.Cyn = (unsigned int) cur.Blk + cur.Cyn;
  nclr.Mgn = (unsigned int) cur.Blk + cur.Mgn;
  nclr.Yel = (unsigned int) cur.Blk + cur.Yel;

	if(nclr.Cyn > CMY_UNIT)
		nclr.Cyn = CMY_UNIT;
	if(nclr.Mgn > CMY_UNIT)
		nclr.Mgn = CMY_UNIT;
	if(nclr.Yel > CMY_UNIT)
		nclr.Yel = CMY_UNIT;
	return(nclr);
}

CMYKC WINAPI ConvertCMY2CMYK(CMYKC cur)
{
	CMYKC	nclr;

	nclr.Blk = cur.Cyn;
	if(nclr.Blk > cur.Mgn)
		nclr.Blk = cur.Mgn;
	if(nclr.Blk > cur.Yel)
		nclr.Blk = cur.Yel;

  nclr.Cyn = (BYTE) ((unsigned int) cur.Cyn - nclr.Blk);
  nclr.Mgn = (BYTE) ((unsigned int) cur.Mgn - nclr.Blk);
  nclr.Yel = (BYTE) ((unsigned int) cur.Yel - nclr.Blk);
	return(nclr);
}

COLORREF WINAPI ConvertHSV2COLORREF(HSV_C cur)
{
	RGB_C rgb;
	
	rgb = ConvertHSV2RGB(cur);
	return( RGB(rgb.Red, rgb.Grn, rgb.Blu) );
}

