//======================================================================
//
// ibitmap.h
//
// NOTE: by Linwei on 7-3-2002
//
//======================================================================


#ifndef __I_BITMAPC_H__
#define __I_BITMAPC_H__

#define IBITMAP_BLT_COLORKEY 1
#define IBITMAP_BLT_CLIP	 2

#include "istream.h"

class IBitmap
{
protected:
	int   bmHeight;
	int   bmWidth;
	int   bmColorDepth;
	int   bmLinearSize;
	char *bmPixel;
	int   ClassCode;
private:
	static void TransferN(char *dest,int x,int y,int LinearSize1,char *source,int left,int top,int w,int h,int LinearSize2,int pixelbyte);
	static void TransferX(char *dest,int x,int y,int LinearSize1,char *source,int left,int top,int w,int h,int LinearSize2,int pixelbyte,int colorkey);
	static void (*lpTransferN)(char *, int, int, int, char *, int, int, int, int, int, int);
	static void (*lpTransferX)(char *, int, int, int, char *, int, int, int, int, int, int, int);
	int   LoadTGAStream(Istream *stream);
	int   LoadBMPStream(Istream *stream);
protected:
	int   clip(int DestWidth,int DestHeight,int SrcWidth,int SrcHeight,int *xx,int *yy,int *myLeft,int *myTop,int *myRight,int *myBottom);
	int   BlitSoft(int x,int y,IBitmap* bitmap,int left,int top,int w,int h,int colorkey,int mode);
public:
	IBitmap();
	virtual ~IBitmap();

	virtual int  Create(int width,int height,int colordepth);
	virtual int  Release();
	virtual int  LoadFromStream(Istream *stream,int type);
	int GetClassCode();
	int GetH();
	int GetW();
	int GetDepth();
	virtual int  Blit(int x,int y,IBitmap* bitmap,int left,int top,int w,int h,int mode,void *data);
	virtual void *Lock(int *LinearSize);
	virtual int  Unlock();
	virtual void Options(int mode, void *lptr);
};

#endif


