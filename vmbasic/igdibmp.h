//======================================================================
//
// igdibmp.h
//
// NOTE: by Linwei on 7-13-2002
//
//======================================================================



#ifndef __I_BMPGDI_H__
#define __I_BMPGDI_H__

#ifndef _WIN32
#error This file can only be compiled under windows !!
#endif

#include "ibitmapc.h"
#include "icanvas.h"

#include <windows.h>

class IGDIBitmap:public IBitmap,public ICanvas
{
private:
	HDC hDC;
	HBITMAP Handle;
	struct BITMAPINFOX
	{
		BITMAPINFOHEADER bmiHeader;
		long r,g,b,e;
	};
	BITMAPINFOX bmInfo;
	int  BWidth;
	int  SetInterface(char *Bits,int Width,int Height,int Bpp,int fMask);
	int  Get16Mask();
	int  InitPixels(void *fBits);
public:
	IGDIBitmap();
	virtual ~IGDIBitmap();

	virtual int  Blit(int x,int y,IBitmap* bitmap,int left,int top,int w,int h,int mode,void *data);
	virtual int  LoadFromStream(Istream* stream,int type);
	void BlitDC(HDC dc,int x,int y,int w,int h);
	int  Create(int width,int height,int colordepth);
	int  Release();
	HDC  GetDC();
	void Fill(IColor c);
	void Pixel(int x,int y,IColor c);
	IColor GetPixel(int x,int y);
};

#endif
