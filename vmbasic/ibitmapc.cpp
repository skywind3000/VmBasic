#include "ibitmapc.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


///////////////////////////////////////////////////////////////////
// IBitmap 位图类
///////////////////////////////////////////////////////////////////
IBitmap::IBitmap()
{
	bmPixel=NULL;
	Release();
	ClassCode=0;
	if (lpTransferN == NULL) Options(0, (void*)TransferN);
	if (lpTransferX == NULL) Options(1, (void*)TransferX);
}

IBitmap::~IBitmap()
{
	Release();
}

int IBitmap::GetClassCode()
{
	return ClassCode;
}

int IBitmap::GetH()
{
	return bmHeight;
}

int IBitmap::GetW()
{
	return bmWidth;
}

int IBitmap::GetDepth()
{
	return bmColorDepth;
}

int IBitmap::Create(int w,int h,int c)
{
	Release();
	bmLinearSize=((w*(c>>3)+3)>>2)<<2;
	bmPixel=new char[bmLinearSize*h];
	if (bmPixel==NULL) return -1;
	bmHeight=h;
	bmWidth=w;
	bmColorDepth=c;
	return 0;
}

int IBitmap::Release()
{
	if (bmPixel) delete []bmPixel;
	bmPixel=NULL;
	bmHeight=0;
	bmWidth=0;
	bmColorDepth=0;
	bmLinearSize=0;
	return 0;
}

void *IBitmap::Lock(int *LinearSize)
{
	if (!bmPixel||bmWidth==0||bmHeight==0) { return NULL; }
	if (LinearSize) *LinearSize=bmLinearSize;
	return bmPixel;
}

int  IBitmap::Unlock()
{
	return 0;
}

int IBitmap::Blit(int x,int y,IBitmap* bitmap,int left,int top,int w,int h,int mode,void *data)
{
	data=data;
	return BlitSoft(x,y,bitmap,left,top,w,h,mode,((mode>=0)?IBITMAP_BLT_COLORKEY:0)|IBITMAP_BLT_CLIP);
}

///////////////////////////////////////////////////////////////////
// IBitmap::BlitSoft
// 软件实现的 Blit操作
///////////////////////////////////////////////////////////////////
int IBitmap::BlitSoft(int x,int y,IBitmap *bitmap,int left,int top,int w,int h,int colorkey,int mode)
{
	int   LinearSize1,LinearSize2;
	int   pixelbyte;
	int   right=left+w,bottom=top+h;
	char *Buffer1,*Buffer2;

	if (bitmap==NULL) return -1;
	if (mode&IBITMAP_BLT_CLIP) 
		if (!clip(bmWidth,bmHeight,bitmap->GetW(),bitmap->GetH(),&x,&y,&left,&top,&right,&bottom)) return 0;

	if (bitmap->GetDepth()!=bmColorDepth) return -2;

	Buffer1=(char*)Lock(&LinearSize1);
	if (!Buffer1) return -3;
	Buffer2=(char*)(bitmap->Lock(&LinearSize2));
	if (!Buffer2) { Unlock(); return -4; }

	if (bmColorDepth==15||bmColorDepth==16) pixelbyte=2;
	else if (bmColorDepth==24) pixelbyte=3;
	else if (bmColorDepth==8) pixelbyte=1;
	else pixelbyte=(bmColorDepth>>3);
	if (mode&IBITMAP_BLT_COLORKEY) lpTransferX(Buffer1,x,y,LinearSize1,Buffer2,left,top,right-left,bottom-top,LinearSize2,pixelbyte,colorkey);
	else lpTransferN(Buffer1,x,y,LinearSize1,Buffer2,left,top,right-left,bottom-top,LinearSize2,pixelbyte);
	Unlock();
	bitmap->Unlock();

	return 0;
}

int IBitmap::clip(int DestWidth,int DestHeight,int SrcWidth,int SrcHeight,int *xx,int *yy,int *myLeft,int *myTop,int *myRight,int *myBottom)
{
	long i, width, height;
	long x = *xx, y = *yy;
	register long left = *myLeft;
	register long top = *myTop;
	int right = *myRight;
	int bottom = *myBottom;

	if (left > right) i = left, left = right, right = i;
	if (top > bottom) i = top, top = bottom, bottom = i;
	if (right <= 0 || left >= SrcWidth || bottom <= 0 || top >= SrcHeight || left==right || top==bottom) return 0;

	if (left < 0) { x -= left; left = 0; }
	if (top < 0) { y -= top; top = 0; }
	if (right > SrcWidth) right = SrcWidth;
	if (bottom > SrcHeight) bottom = SrcHeight;
	width = right - left, height = bottom - top;

	if (x >= DestWidth || y >= DestHeight || x + width <= 0 || y + height <= 0) return 0;
	if (x < 0) { width += x; left -= x; x = 0; }
	if (y < 0) { height += y; top -= y; y = 0; }
	i = x + width - DestWidth; if (i > 0) right -= i;
	i = y + height - DestHeight; if (i > 0) bottom -= i;

	*xx = x; *yy = y;
	*myLeft=left;     *myRight=right;
	*myTop=top;       *myBottom=bottom;

	return 1;
}

void (*IBitmap::lpTransferN)(char *, int, int, int, char *, int, int, int, int, int, int) = NULL;
void (*IBitmap::lpTransferX)(char *, int, int, int, char *, int, int, int, int, int, int, int) = NULL;

typedef void (*IBitmap_TransferTypeN)(char *, int, int, int, char *, int, int, int, int, int, int);
typedef void (*IBitmap_TransferTypeX)(char *, int, int, int, char *, int, int, int, int, int, int, int);

void IBitmap::Options(int mode, void *lptr)
{
	if (mode == 0) lpTransferN = (lptr)? ((IBitmap_TransferTypeN)lptr) : TransferN;
	if (mode == 1) lpTransferX = (lptr)? ((IBitmap_TransferTypeX)lptr) : TransferX;
}
///////////////////////////////////////////////////////////////////////////////////
void IBitmap::TransferN(char *dest,int x,int y,int LinearSize1,char *source,int left,int top,int w,int h,int LinearSize2,int pixelbyte)
{
	int linebytes=pixelbyte*w,i;
	char *sd,*ss;

	if (dest==NULL||source==NULL) return;
	sd=dest+y*LinearSize1+x*pixelbyte;
	ss=source+top*LinearSize2+left*pixelbyte;
	for (i=0;i<h;i++)
	{
		memcpy(sd,ss,linebytes);
		sd+=LinearSize1;
		ss+=LinearSize2;
	}
}

void IBitmap::TransferX(char *dest,int x,int y,int LinearSize1,char *source,int left,int top,int w,int h,int LinearSize2,int pixelbyte,int colorkey)
{
	int linebytes=pixelbyte*w,i,c;
	unsigned char  key8=(unsigned char)colorkey;
	unsigned short key16=(unsigned short)colorkey;
	dest+=y*LinearSize1+x*pixelbyte;
	source+=top*LinearSize2+left*pixelbyte;

	if (dest==NULL||source==NULL) return;
	switch (pixelbyte)
	{
	case 1:
		for (;h;h--) {
			for (i=0;i<w;i++) {
				if (*(unsigned char*)source-key8) *dest=*source;
				source++;
				dest++;
			}
			dest+=LinearSize1-linebytes;
			source+=LinearSize2-linebytes;
		}
		break;
	case 2:
		for (;h;h--) {
			for (i=0;i<w;i++) {
				if (*(unsigned short*)source-key16) *(short*)dest=*(short*)source;
				source+=2;
				dest+=2;
			}
			dest+=LinearSize1-linebytes;
			source+=LinearSize2-linebytes;
		}
		break;
	case 3:
		for (;h;h--) {
			for (i=0;i<w;i++) {
				c=(unsigned char)source[2];
				c=(c<<16)|(*(unsigned short*)source);
				if (c-colorkey) *(short*)dest=*(short*)&c,dest[2]=c>>16;
				source+=3;
				dest+=3;
			}
			dest+=LinearSize1-linebytes;
			source+=LinearSize2-linebytes;
		}
		break;
	case 4:
		for (;h;h--) {
			for (i=0;i<w;i++) {
				if (*(int*)source-colorkey) *(int*)dest=*(int*)source;
				source+=4;
				dest+=4;
			}
			dest+=LinearSize1-linebytes;
			source+=LinearSize2-linebytes;
		}
		break;
	}
}


///////////////////////////////////////////////////////////////////
// IBitmap::LoadFromStream 从流里面读取图片
///////////////////////////////////////////////////////////////////
int IBitmap::LoadFromStream(Istream* stream,int type)
{
	return (type==0)?LoadBMPStream(stream):LoadTGAStream(stream);
}

int IBitmap::LoadTGAStream(Istream* mstream)
{
	char		UTGAheader[12] = {0,0,2,0,0,0,0,0,0,0,0,0};		// Uncompressed TGA header
	char		CTGAheader[12] = {0,0,10,0,0,0,0,0,0,0,0,0};	// Compressed TGA Header
	char		TGAcompare[12];									// Used to compare TGA header
	unsigned char	header[6];									// The first six useful bytes from the header
	int				BytesPerPixel;								// Holds the bpp of the TGA
	int				ImageSize;									// Used to store image size while in RAM
	unsigned int	TGAcompressed;
	unsigned int	width,height,bpp;
	unsigned char	*data;

	Release();
	if (mstream->Eof()) return -1;
	if (mstream->Read(TGAcompare, sizeof(TGAcompare)) == 0)	return -2;
	if (memcmp(UTGAheader,TGAcompare,6)==0) TGAcompressed=0;
	else if (memcmp(CTGAheader,TGAcompare,6)==0) TGAcompressed=1;
	else return -3;
	if (mstream->Read(header,sizeof(header))==0)	return -4;
    width  = *(short*)&header[0];
	height = *(short*)&header[2];
 	if (width<=0||height<=0||(header[4]!=24 && header[4]!=32)) return -5;
	bpp = header[4];
	BytesPerPixel = bpp / 8;
	ImageSize	  = width*height*BytesPerPixel;	
	if (Create(width,height,bpp)) return -5;
	int LinearSize;
	data=(unsigned char*)Lock(&LinearSize);
	if (data==NULL) {	Unlock();	Release();	return -6;	}
	if (!TGAcompressed)	
	{
		if (mstream->Read(data,ImageSize)!=ImageSize) { Release(); return -7; }
		for (int i=0; i<(int)ImageSize; i+=BytesPerPixel)
			data[i] ^= data[i+2] ^= data[i] ^= data[i+2];
	}	else	{
		unsigned int  PixelCount	= width * height;
		unsigned int  CurrentPixel	= 0;
		unsigned int  CurrentByte	= 0;
		unsigned char ColorBuffer[10];
		do
		{
			unsigned char ChunkHeader = 0;
			if (mstream->Read(&ChunkHeader, sizeof(char)) == 0)
			{
				Release();
				return -8;
			}
			if (ChunkHeader<128)			// The data is RAW type
			{
				ChunkHeader++;											// Add 1 to get number of following color values
				for (unsigned short counter=0; counter<ChunkHeader; counter++)	// Read RAW color values
				{
					if(mstream->Read(ColorBuffer,BytesPerPixel)!=BytesPerPixel)		// Try to read one pixel
					{
						Release();
						return -9;
					}
					data[CurrentByte]   = ColorBuffer[2];
					data[CurrentByte+1] = ColorBuffer[1];
					data[CurrentByte+2] = ColorBuffer[0];
					if (BytesPerPixel == 4)	data[CurrentByte+3] = ColorBuffer[3];
					CurrentByte += BytesPerPixel;			// Increase thecurrent byte by the number of bytes per pixel
					CurrentPixel++;							// Increase current pixel by 1
					if (CurrentPixel > PixelCount)			// Make sure we have not read too many pixels
					{
						Release();
						return -10;
					}
				}
			}
			else							// The data is RLE type
			{
				ChunkHeader-=127;
				if(mstream->Read(ColorBuffer,BytesPerPixel)!=BytesPerPixel) { Release(); return -11; }
				for (short counter=0; counter<ChunkHeader; counter++)
				{
					data[CurrentByte]   = ColorBuffer[2];
					data[CurrentByte+1] = ColorBuffer[1];
					data[CurrentByte+2] = ColorBuffer[0];
					if (BytesPerPixel==4) data[CurrentByte+3]=ColorBuffer[3];
					CurrentByte+=BytesPerPixel;				// Increase thecurrent byte by the number of bytes per pixel
					CurrentPixel++;							// Increase current pixel by 1
					if (CurrentPixel>PixelCount)			// Make sure we have not read too many pixels
					{
						Release();
						return -12;
					}
				}
			}
		} while (CurrentPixel<PixelCount);				// Loop while there are still pixels left
	}
	if (!(header[5]&0x20)) {
		unsigned char *swap=new unsigned char[LinearSize];
		if (swap) {
			for (unsigned int j=0;j<(height>>1);j++) {
				memcpy(swap,&data[j*LinearSize],LinearSize);
				memcpy(&data[j*LinearSize],&data[(height-1-j)*LinearSize],LinearSize);
				memcpy(&data[(height-1-j)*LinearSize],swap,LinearSize);
			}
			delete swap;
		}
	}
	Unlock();
	return 0;
}


int IBitmap::LoadBMPStream(Istream* mstream)
{
	struct MyBITMAPINFOHEADER{ // bmih  
		long	biSize; 
		long	biWidth; 
		long	biHeight; 
		short   biPlanes; 
		short   biBitCount;
		long	biCompression; 
		long	biSizeImage; 
		long	biXPelsPerMeter; 
		long	biYPelsPerMeter; 
		long	biClrUsed; 
		long	biClrImportant; 
	} InfoHeader; 
	char FileHeader[14];
	long pos=mstream->GetPos();
	mstream->Read(FileHeader, 14);
	if (FileHeader[0]!=0x42 || FileHeader[1]!=0x4d) return -2;
	mstream->Read(&InfoHeader, sizeof(InfoHeader));
	mstream->Seek(pos+*(long*)&FileHeader[10],ISTREAM_START);

	int width	  = InfoHeader.biWidth;
	int height	  = InfoHeader.biHeight;
	int bpp		  = InfoHeader.biBitCount;
	long ImageSize = width * height * bpp / 8;
	unsigned char *data,*ch;
	int LinearSize;
	if (Create(width,height,bpp)) return -3;
	data=(unsigned char*)Lock(&LinearSize);
	mstream->Read(data, ImageSize);
	long i=0,j; unsigned char k;
	if (bpp==24)
		for (j=0,ch=data;j<height;j++,ch+=LinearSize)
			for (i=0;i<width*3;i+=3) 
				k=ch[i], ch[i]=ch[i+2], ch[i+2]=k;
	unsigned char *swap=new unsigned char[LinearSize];
	if (swap) {
		for (j=0;j<height/2;j++) {
			memcpy(swap,&data[j*LinearSize],LinearSize);
			memcpy(&data[j*LinearSize],&data[(height-1-j)*LinearSize],LinearSize);
			memcpy(&data[(height-1-j)*LinearSize],swap,LinearSize);
		}
		delete swap;
	}
	Unlock();
	return 0;
}
