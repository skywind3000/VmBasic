#include "icanvas.h"
#include "ifstream.h"
#include <stdlib.h>
#include <math.h>

void ICanvas::Fill(IColor c)
{
	short c16;
	int x,y,c32;
	if (!Scanlines) return;
	switch (canBpp)
	{
	case 8:
		memset(*Scanlines,IColor8(c),canHeight*canLinearSize);
		break;
	case 15:
		c16=IColor15(c);
	case 16:
		if (canBpp==16) c16=IColor16(c);
		for (y=0;y<canHeight;y++) {
			unsigned short *line=Pixels16[y];
			for (x=0;x<canWidth;x++,line++) *line=c16;
		}
		break;
	case 24:
		for (y=0;y<canHeight;y++)
			for (x=0;x<canWidth;x++) Pixels24[y][x]=c;
		break;
	case 32:
		c32=c.r+(c.g<<8)+(c.b<<16);
		for (y=0;y<canHeight;y++)
			for (x=0;x<canWidth;x++) Pixels32[y][x]=*(IColorA*)&c32;
		break;
	}
}

void ICanvas::Pixel(int x,int y,IColor c)
{
	if (x<0||y<0||x>=canWidth||y>=canHeight||*Scanlines==NULL) return;

	switch (canBpp)
	{
	case 8:
		Pixels8[y][x]=((c.r>>6)<<5)|((c.g>>5)<<3)|(c.b>>6);
		break;
	case 15:
		Pixels16[y][x]=IColor15(c);
		break;
	case 16:
		Pixels16[y][x]=IColor16(c);
		break;
	case 24:
		Pixels24[y][x]=c;
		break;
	case 32:
		char *b;
		b=(char*)&Pixels32[y][x];
		*(IColor*)b=c;
		b[3]=0;
		break;
	}
}
void ICanvas::PixelBit(int x,int y,int c)
{
	if (x<0||y<0||x>=canWidth||y>=canHeight||*Scanlines==NULL) return;
	switch (canBpp)
	{
	case 8:  Pixels8[y][x]=(unsigned char)c; break;
	case 15: Pixels16[y][x]=(unsigned short)c; break;
	case 16: Pixels16[y][x]=(unsigned short)c; break;
	case 24: Pixels24[y][x]=*((IColor*)&c);  break;
	case 32: Pixels32[y][x]=*((IColorA*)&c); break;
	}
}

IColor ICanvas::GetPixel(int x,int y)
{
	static IColor c={0,0,0};
	short t;
	if (x<0||y<0||x>=canWidth||y>=canHeight) return c;
	switch (canBpp)
	{
	case 8:
		c.r=Pixels8[y][x];
		break;
	case 15:
		t=Pixels16[y][x];
		c.r=(t&0x7c00)>>10;
		c.g=(t&0x03e0)>>5;
		c.b=(t&0x001f);
		break;
	case 16:
		t=Pixels16[y][x];
		c.r=(t&0xf800)>>11;
		c.g=(t&0x07e0)>>5;
		c.b=(t&0x001f);
		break;
	case 24:
		c=Pixels24[y][x];
		break;
	case 32:
		char *b;
		b=(char*)&Pixels32[y][x];
		c=*(IColor*)b;
		break;
	}
	return c;
}
int ICanvas::GetPixelBit(int x,int y)
{
	int t;
	IColor c;
	if (x<0||y<0||x>=canWidth||y>=canHeight) return 0;
	switch (canBpp)
	{
	case 8:  t=Pixels8[y][x];  break;
	case 15: t=Pixels16[y][x]; break;
	case 16: t=Pixels16[y][x]; break;
	case 24: c=Pixels24[y][x]; t=c.r; t=(t<<16)|(*((unsigned short*)&c)); break;
	case 32: t=*((int*)&Pixels32[y][x]); break;
	}
	return t;
}
int  ICanvas::ColorBit(IColor c)
{
	int color;
	if (canBpp==8)  color=IColor8(c);
	else if (canBpp==15) color=IColor15(c);
	else if (canBpp==16) color=IColor16(c);
	else color=*(int*)&c;
	return color;
}
void ICanvas::Rectangle(int left,int top,int right,int bottom)
{
	int color=WorkColor;
	for (int x=left;x<right;x++) {
		PixelBit(x,top,color);
		PixelBit(x,bottom-1,color);
	}
	for (int y=top;y<bottom;y++) {
		PixelBit(left,y,color);
		PixelBit(right-1,y,color);
	}
}
void ICanvas::FillRactangle(int left,int top,int right,int bottom)
{
	int color=WorkColor;
	for (int y=top;y<bottom;y++)
		for (int x=left;x<right;x++) PixelBit(x,y,color);
}

int ICanvas::LoadPal(char *filename,IColorTable* pal)
{
	char FileHeader[14];
	int pos;
	Ifstream fs;
	if (fs.Open(filename,ISTREAM_READ|ISTREAM_BINARY)) 
		return -1;
	fs.Seek(0,ISTREAM_END);
	pos=fs.GetPos();
	fs.Seek(0,ISTREAM_START);
	if (pos==768) {
		for (int i=0;i<256;i++) {
			fs.Read(&(pal->colors[i].r),1);
			fs.Read(&(pal->colors[i].g),1);
			fs.Read(&(pal->colors[i].b),1);
		}
		fs.Close();
		return 0;
	}
	fs.Read(&FileHeader,14);
	if (FileHeader[0]!=0x42 || FileHeader[1]!=0x4d) {
		fs.Close();
		return -2;
	}
	int PalCount=((*(long*)&FileHeader[10])-54)/4;
	fs.Seek(54,ISTREAM_START);
	for (int i=0;i<PalCount;i++) {
		IColorA ra;
		fs.Read(&ra,sizeof(ra));
		pal->colors[i].r=ra.r;
		pal->colors[i].g=ra.g;
		pal->colors[i].b=ra.b;
	}
	fs.Close();
	return 0;
}

IColorTable ICanvas::PalWin={ /* { b,g,r } */
	   0,  0,  0 ,  64,  0,  0 , 128,  0,  0 , 255,  0,  0 ,   0, 32,  0 ,
	  64, 32,  0 , 128, 32,  0 , 255, 32,  0 ,   0, 64,  0 ,  64, 64,  0 ,
	 128, 64,  0 , 255, 64,  0 ,   0, 96,  0 ,  64, 96,  0 , 128, 96,  0 ,
	 255, 96,  0 ,   0,128,  0 ,  64,128,  0 , 128,128,  0 , 255,128,  0 ,
	   0,160,  0 ,  64,160,  0 , 128,160,  0 , 255,160,  0 ,   0,192,  0 ,
	  64,192,  0 , 128,192,  0 , 255,192,  0 ,   0,255,  0 ,  64,255,  0 ,
	 128,255,  0 , 255,255,  0 ,   0,  0, 32 ,  64,  0, 32 , 128,  0, 32 ,
	 255,  0, 32 ,   0, 32, 32 ,  64, 32, 32 , 128, 32, 32 , 255, 32, 32 ,
	   0, 64, 32 ,  64, 64, 32 , 128, 64, 32 , 255, 64, 32 ,   0, 96, 32 ,
	  64, 96, 32 , 128, 96, 32 , 255, 96, 32 ,   0,128, 32 ,  64,128, 32 ,
	 128,128, 32 , 255,128, 32 ,   0,160, 32 ,  64,160, 32 , 128,160, 32 ,
	 255,160, 32 ,   0,192, 32 ,  64,192, 32 , 128,192, 32 , 255,192, 32 ,
	   0,255, 32 ,  64,255, 32 , 128,255, 32 , 255,255, 32 ,   0,  0, 64 ,
	  64,  0, 64 , 128,  0, 64 , 255,  0, 64 ,   0, 32, 64 ,  64, 32, 64 ,
	 128, 32, 64 , 255, 32, 64 ,   0, 64, 64 ,  64, 64, 64 , 128, 64, 64 ,
	 255, 64, 64 ,   0, 96, 64 ,  64, 96, 64 , 128, 96, 64 , 255, 96, 64 ,
	   0,128, 64 ,  64,128, 64 , 128,128, 64 , 255,128, 64 ,   0,160, 64 ,
	  64,160, 64 , 128,160, 64 , 255,160, 64 ,   0,192, 64 ,  64,192, 64 ,
	 128,192, 64 , 255,192, 64 ,   0,255, 64 ,  64,255, 64 , 128,255, 64 ,
	 255,255, 64 ,   0,  0, 96 ,  64,  0, 96 , 128,  0, 96 , 255,  0, 96 ,
	   0, 32, 96 ,  64, 32, 96 , 128, 32, 96 , 255, 32, 96 ,   0, 64, 96 ,
	  64, 64, 96 , 128, 64, 96 , 255, 64, 96 ,   0, 96, 96 ,  64, 96, 96 ,
	 128, 96, 96 , 255, 96, 96 ,   0,128, 96 ,  64,128, 96 , 128,128, 96 ,
	 255,128, 96 ,   0,160, 96 ,  64,160, 96 , 128,160, 96 , 255,160, 96 ,
	   0,192, 96 ,  64,192, 96 , 128,192, 96 , 255,192, 96 ,   0,255, 96 ,
	  64,255, 96 , 128,255, 96 , 255,255, 96 ,   0,  0,128 ,  64,  0,128 ,
	 128,  0,128 , 255,  0,128 ,   0, 32,128 ,  64, 32,128 , 128, 32,128 ,
	 255, 32,128 ,   0, 64,128 ,  64, 64,128 , 128, 64,128 , 255, 64,128 ,
	   0, 96,128 ,  64, 96,128 , 128, 96,128 , 255, 96,128 ,   0,128,128 ,
	  64,128,128 , 128,128,128 , 255,128,128 ,   0,160,128 ,  64,160,128 ,
	 128,160,128 , 255,160,128 ,   0,192,128 ,  64,192,128 , 128,192,128 ,
	 255,192,128 ,   0,255,128 ,  64,255,128 , 128,255,128 , 255,255,128 ,
	   0,  0,160 ,  64,  0,160 , 128,  0,160 , 255,  0,160 ,   0, 32,160 ,
	  64, 32,160 , 128, 32,160 , 255, 32,160 ,   0, 64,160 ,  64, 64,160 ,
	 128, 64,160 , 255, 64,160 ,   0, 96,160 ,  64, 96,160 , 128, 96,160 ,
	 255, 96,160 ,   0,128,160 ,  64,128,160 , 128,128,160 , 255,128,160 ,
	   0,160,160 ,  64,160,160 , 128,160,160 , 255,160,160 ,   0,192,160 ,
	  64,192,160 , 128,192,160 , 255,192,160 ,   0,255,160 ,  64,255,160 ,
	 128,255,160 , 255,255,160 ,   0,  0,192 ,  64,  0,192 , 128,  0,192 ,
	 255,  0,192 ,   0, 32,192 ,  64, 32,192 , 128, 32,192 , 255, 32,192 ,
	   0, 64,192 ,  64, 64,192 , 128, 64,192 , 255, 64,192 ,   0, 96,192 ,
	  64, 96,192 , 128, 96,192 , 255, 96,192 ,   0,128,192 ,  64,128,192 ,
	 128,128,192 , 255,128,192 ,   0,160,192 ,  64,160,192 , 128,160,192 ,
	 255,160,192 ,   0,192,192 ,  64,192,192 , 128,192,192 , 255,192,192 ,
	   0,255,192 ,  64,255,192 , 128,255,192 , 255,255,192 ,   0,  0,255 ,
	  64,  0,255 , 128,  0,255 , 255,  0,255 ,   0, 32,255 ,  64, 32,255 ,
	 128, 32,255 , 255, 32,255 ,   0, 64,255 ,  64, 64,255 , 128, 64,255 ,
	 255, 64,255 ,   0, 96,255 ,  64, 96,255 , 128, 96,255 , 255, 96,255 ,
	   0,128,255 ,  64,128,255 , 128,128,255 , 255,128,255 ,   0,160,255 ,
	  64,160,255 , 128,160,255 , 255,160,255 ,   0,192,255 ,  64,192,255 ,
	 128,192,255 , 255,192,255 ,   0,255,255 ,  64,255,255 , 128,255,255 ,
	 255,255,255 };

int ICanvas::PalMatch(IColorTable* pal,short r,short g,short b)
{
	int l=0;
	for (int j=1024,i=0;i<256;i++) {
		int k=abs(pal->colors[i].r-r)+
			  abs(pal->colors[i].g-g)+
			  abs(pal->colors[i].b-b);
		if (k<j) j=k,l=i;
	}
	return l;
}

void ICanvas::Line(int x1,int y1,int x2,int y2)
{
	int x,y,p,n,tn;
	int c=WorkColor;

	if (y1==y2)
	{ 
		if (x1>x2) { x=x1; x1=x2; x2=x; }
		for (x=x1;x<=x2;x++) PixelBit(x,y1,c);
		return;
	}
	if (x1==x2)
	{ 
		if (y1>y2) { y=y2; y2=y1; y1=y; }
		for (y=y1;y<=y2;y++) PixelBit(x1,y,c);
		return;
	}

	if ( abs(y2-y1) <= abs(x2-x1) )
	{
		if ( (y2<y1&&x2<x1) || (y1<=y2&&x1>x2) )
		{
			x=x2; y=y2; x2=x1; y2=y1; x1=x; y1=y;
		}
		if ( y2>=y1 && x2>=x1 )
		{
			x=x2-x1; y=y2-y1;
			p=2*y; n=2*x-2*y; tn=x;
			while (x1<=x2) {
				if (tn>=0) tn-=p;
				else tn+=n, y1++;
				PixelBit(x1,y1,c);
				x1++;
			}
		}	else {
			x=x2-x1; y=y2-y1;
			p=-2*y; n=2*x+2*y; tn=x;
			while (x1<=x2) {
				if (tn>=0) tn-=p;
				else tn+=n, y1--;
				PixelBit(x1,y1,c);
				x1++;
			}
		}
	}	else	{
		x=x1; x1=y2; y2=x; y=y1; y1=x2; x2=y;
		if ( (y2<y1&&x2<x1) || (y1<=y2&&x1>x2) )
	    {
			x=x2; y=y2; x2=x1; x1=x; y2=y1; y1=y;
	    }
		if ( y2>=y1 && x2>=x1 )
		{
			x=x2-x1; y=y2-y1;
			p=2*y; n=2*x-2*y; tn=x;
			while (x1<=x2)  {
				if (tn>=0) tn-=p;
				else tn+=n, y1++;
				PixelBit(y1,x1,c);
				x1++;
			}
	    }	else	{
			x=x2-x1; y=y2-y1;
			p=-2*y; n=2*x+2*y; tn=x;
			while (x1<=x2) {
				if (tn>=0) tn-=p;
				else { tn+=n; y1--; }
				PixelBit(y1,x1,c);
				x1++;
			}
		}
	}
}

void  ICanvas::Circle(int x0,int y0,int r,int FillMode)
{
	int cx=0, cy=r, df=1-r,d_e=3,d_se=-2*r+5,
		x,y,xmax,tn;
	int color=WorkColor;

	switch (FillMode)
	{  
	case 0: 
		y=r; x=0; xmax=(int)((double)r*sin(3.1415926/4) + 0.5);
		tn=(1-r*2);
		while (x<xmax) { 
			if (tn>=0) {  
				tn+=( 6 + ((x-y)<<2) );
				y--;
			} else tn+=( (x<<2) + 2 );
			PixelBit(x0+y,y0+x,color);
			PixelBit(x0+x,y0+y,color);
			PixelBit(x0-x,y0+y,color);
			PixelBit(x0-y,y0+x,color);
			PixelBit(x0-y,y0-x,color);
			PixelBit(x0-x,y0-y,color);
			PixelBit(x0+x,y0-y,color);
			PixelBit(x0+y,y0-x,color);
			x++;
		}
		break;
	case 1: 
		do {
			Line(x0-cy,y0-cx,x0-cy+(cy<<1),y0-cx);
			if (cx) Line(x0-cy,y0+cx,x0-cy+(cy<<1),y0+cx);
			if (df<0) { df+=d_e; d_e+=2; d_se+=2; } 
			else { 
				if (cx!=cy) { 
					Line(x0-cx,y0-cy,x0+cy+(cx<<1),y0-cy);
					if (cy) Line(x0-cx,y0+cy,x0+cy+(cx<<1),y0+cy);
				}
				df+=d_se; d_e+=2; d_se+=4;
				cy--; 
			} 
			cx++;
		}	while(cx<=cy);
		break;
   }
}
