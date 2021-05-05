//======================================================================
//
// icanvas.h
//
// NOTE: by Linwei on 7-6-2002
//
//======================================================================

#ifndef __I_CANVAS_H__
#define __I_CANVAS_H__

#include "ibitmapc.h"

struct IColor  { unsigned char b,g,r;    };
struct IColorA { unsigned char b,g,r,a;  };
struct IColorTable { IColor colors[256]; };

class ICanvas
{
protected:
	char    bshr,gshr,gshl,rshr,rshl,Gap;
	int     Mask;
	int		canWidth;
	int		canHeight;
	int		canBpp;
	int		canLinearSize;
	void    **Scanlines;
	unsigned char  **Pixels8;
	unsigned short **Pixels16;
	IColor  **Pixels24;
	IColorA **Pixels32;
protected:
	int     WorkColor;
	static IColorTable PalWin;
public:
	ICanvas(): Scanlines(0) {}
	virtual void   Fill(IColor c);
	virtual void   Pixel(int x,int y,IColor c);
	virtual IColor GetPixel(int x,int y);
	virtual void   SetColor(IColor c) { WorkColor=ColorBit(c); }
	void	SetColor(int c) { WorkColor=c; }
	void	PixelBit(int x,int y,int c);
	int		GetPixelBit(int x,int y);
	int		ColorBit(IColor c);
	void	Rectangle(int left,int top,int right,int bottom);
	void	FillRactangle(int left,int top,int right,int bottom);
	void	Line(int x1,int y1,int x2,int y2);
	void    Circle(int x,int y,int r,int FillMode=0);
	int		LoadPal(char *PalFile,IColorTable* pal);
	int		LoadPalWin(IColorTable* pal) { *pal=PalWin; return 0; }
	int		PalMatch(IColorTable* pal,short r,short g,short b);
	void*	LockLine(int line) { return Scanlines[line]; }
};

inline int IColorRGB(int r,int g,int b) { return (r<<16)|(g<<8)|(b); }
inline int IColor8(IColor c) { return ((c.r>>5)<<5)|((c.g>>5)<<2)|(c.b>>6); }
inline int IColor15(IColor c) { return ((int)(c.r>>3)<<10)|((int)(c.g>>3)<<5)|(c.b>>3); }
inline int IColor16(IColor c) { return ((int)(c.r>>3)<<11)|((int)(c.g>>2)<<5)|(c.b>>3); }

#endif
