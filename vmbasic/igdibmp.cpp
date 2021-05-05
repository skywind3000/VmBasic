#include "igdibmp.h"
#include <stdio.h>
#include <string.h>

IGDIBitmap::IGDIBitmap()
{
	ClassCode=1;
	ZeroMemory(&bmInfo,sizeof(bmInfo));
	bmInfo.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
	bmInfo.bmiHeader.biPlanes=1;
	Scanlines=NULL;   hDC=NULL; Handle=NULL;
	bshr=0;  rshr=0;  Gap=0;    gshl=0; 
	gshr=0;  rshl=0;  Mask=0;
}

IGDIBitmap::~IGDIBitmap()
{
	Release();
}

int IGDIBitmap::Blit(int x,int y,IBitmap* bitmap,int left,int top,int w,int h,int mode,void *data)
{
	int retcode=0;

	if (bitmap==NULL) return -1;

	if (bitmap->GetClassCode()==ClassCode) {
		if (mode >= 0 && bitmap->GetDepth() == bmColorDepth)  
			IBitmap::Blit(x,y,bitmap,left,top,w,h,mode,data);
		else 
			BitBlt(hDC,x,y,w,h,((IGDIBitmap*)bitmap)->GetDC(),left,top,SRCCOPY);
	}	else {
		retcode=IBitmap::Blit(x,y,bitmap,left,top,w,h,mode,data);
	}
	return retcode;
}

void IGDIBitmap::BlitDC(HDC dc,int x,int y,int w,int h)
{
	if (dc!=0&&w==bmWidth&&h==bmHeight) BitBlt(dc,x,y,w,h,hDC,0,0,SRCCOPY);
	else StretchBlt(dc,x,y,w,h,hDC,0,0,bmWidth,bmHeight,SRCCOPY);
}
int IGDIBitmap::Create(int width,int height,int colordepth)
{
	Release();
    SetInterface(NULL,width,height,colordepth,0);
	Handle=CreateDIBSection(0,(BITMAPINFO*)&bmInfo,0,(void**)&bmPixel,NULL,0);
	if (!Handle) return -1;
	hDC=CreateCompatibleDC(0);
	if (!hDC) { Release(); return -3; }
	SelectObject(hDC,Handle);
	InitPixels(bmPixel);

	return 0;
}

int IGDIBitmap::Release()
{
	if (hDC) DeleteDC(hDC);
	if (Handle) DeleteObject(Handle);
	if (Scanlines) delete []Scanlines;
	hDC=NULL;
	Handle=NULL;
	Scanlines=NULL;
	bmPixel=NULL;
	IBitmap::Release();
	return 0;
}

int IGDIBitmap::SetInterface(char *Bits,int Width,int Height,int fBpp,int fMask)
{
	if (fBpp==0) {  //default Bpp is current screen
		HDC sDC=::GetDC(0);
		fBpp=GetDeviceCaps(sDC,BITSPIXEL);
		ReleaseDC(0,sDC);
	}
	if (fMask==0) {
		if (fBpp==16) fMask=Get16Mask(); else
		if (fBpp==32) fMask=888;
	}
	bmColorDepth=fBpp;
	int  Bpp=bmColorDepth;
	bmLinearSize=((Width*(fBpp>>3)+3)>>2)<<2;

	bmWidth=Width;
	bmHeight=Height;
	BWidth=((Width*fBpp+31)>>5)<<2;
	bmInfo.bmiHeader.biHeight=-abs(Height);
	bmInfo.bmiHeader.biWidth=Width;
	bmInfo.bmiHeader.biSizeImage=BWidth*Height;

	if (Bpp<8) Gap=BWidth-(Width/(8/Bpp)); else
	if (Bpp>8) Gap=BWidth-(Width*(Bpp/8));
	else Gap=BWidth-Width;

	Mask=fMask;
	int  rMask,gMask,bMask;

	if((Bpp==16)||(Bpp==32)) {
		bmInfo.bmiHeader.biCompression=BI_BITFIELDS;
		if (fMask==555) {
			rMask=31<<10;
			gMask=31<<5;
			bMask=31;
		} else if (fMask==565) {
			rMask=31<<11;
			gMask=63<<5;
			bMask=31;
		} else if (fMask==888) {
			bMask=255<<16;
			gMask=255<<8;
			rMask=255;
		}
	}   else bmInfo.bmiHeader.biCompression=BI_RGB;
	bmInfo.r=rMask;
	bmInfo.g=gMask;
	bmInfo.b=bMask;
	bmInfo.bmiHeader.biBitCount=Bpp;
	if (Bits!=NULL) return InitPixels(Bits);
	return 0;
}

int IGDIBitmap::InitPixels(void *fBits)
{
	int x,i;
	bmPixel=(char*)fBits;
	Scanlines=new void*[bmHeight];
	if (!Scanlines) return -2;
	for (i=0,x=0;i<bmHeight;i++) {
		Scanlines[i]=(char*)fBits+x;
		x+=bmLinearSize;
	}
	Pixels8=(unsigned char**)Scanlines;
	Pixels16=(unsigned short**)Scanlines;
	Pixels24=(IColor**)Scanlines;
	Pixels32=(IColorA**)Scanlines;
	canWidth=bmWidth;
	canHeight=bmHeight;
	canBpp=bmColorDepth;
	canLinearSize=bmLinearSize;
	return 0;
}

int IGDIBitmap::Get16Mask() // returns 555 or 565 depending on the
{							// current 16bit video mode via cheap
	return 565;
}


HDC IGDIBitmap::GetDC()
{
	return hDC;
}

void IGDIBitmap::Fill(IColor c)
{
	short c16;
	int x,y,c32;
	if (!bmPixel) return;
	switch (bmColorDepth)
	{
	case 8:
		memset(bmPixel,((c.r>>6)<<5)|((c.g>>5)<<3)|(c.b>>6),bmHeight*bmLinearSize);
		break;
	case 15:
		c16=IColor15(c);
	case 16:
		if (bmColorDepth==16) c16=IColor16(c);
		for (y=0;y<bmHeight;y++) {
			unsigned short *line=Pixels16[y];
			for (x=0;x<bmWidth;x++,line++) *line=c16;
		}
		break;
	case 24:
		for (y=0;y<bmHeight;y++)
			for (x=0;x<bmWidth;x++) Pixels24[y][x]=c;
		break;
	case 32:
		c32=c.r+(c.g<<8)+(c.b<<16);
		for (y=0;y<bmHeight;y++)
			for (x=0;x<bmWidth;x++) Pixels32[y][x]=*(IColorA*)&c32;
		break;
	}
}

void IGDIBitmap::Pixel(int x,int y,IColor c)
{
	if (x<0||y<0||x>=bmWidth||y>=bmHeight||bmPixel==NULL) return;
	switch (bmColorDepth)
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

IColor IGDIBitmap::GetPixel(int x,int y)
{
	static IColor c={0,0,0};
	short t;
	if (x<0||y<0||x>=bmWidth||y>=bmHeight) return c;
	switch (bmColorDepth)
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

int  IGDIBitmap::LoadFromStream(Istream* stream,int type)
{
	int ret=IBitmap::LoadFromStream(stream,type);
	if (ret==0) {
		int i,j;
		int LinearSize;
		if (bmColorDepth==24) {
			char *data=(char*)Lock(&LinearSize),k;
			for (j=0;j<bmHeight;j++,data+=LinearSize)
				for (i=0;i<bmWidth*3;i+=3) 
					k=data[i],data[i]=data[i+2],data[i+2]=k;
		}
	}
	return ret;
}
