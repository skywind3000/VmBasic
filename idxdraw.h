//======================================================================
//
// idxdraw.h
//
// NOTE: by Linwei on 3-13-2003
//
//======================================================================

#ifndef __I_DXDRAW_H__
#define __I_DXDRAW_H__

#ifndef _WIN32
#error This file can only be compiled under windows !!
#endif

#include "ibitmapc.h"
#include "icanvas.h"
#include "iscreen.h"

#include <windows.h>
#include <ddraw.h>

#if DIRECTDRAW_VERSION < 0x500
#error The version of DirectDraw must be 5.00 or higher
#endif

class iDDSurface;

//////////////////////////////////////////////////////////////////////////////////
// iDDScreen Class
//////////////////////////////////////////////////////////////////////////////////
class iDDScreen : public iScreen
{
public:
	iDDScreen();
	virtual ~iDDScreen();

	BOOL CreateFullScreen(void *hWnd, DWORD Width, DWORD Height, DWORD BPP, BOOL bVGA = FALSE);
	BOOL CreateWindowed(void *hWnd, int Width, int Height);
	BOOL ReleaseScreen();
	void Restore();
	void ScreenView() { Flip(); }
	HRESULT Flip();

	iDDSurface* GetFrontSurface() { return m_lpDDSFront; }
	iDDSurface* GetBackSurface()  { return m_lpDDSBack;  }
	int GetDesktopBpp();
	int GetScreenWidth() { return m_dwPixelWidth; }
	int GetScreenHeight() { return m_dwPixelHeight; }
	int GetScreenBPP() { return m_BPP; }
	char *GetErrorMsg(HRESULT DDRVAL);
	LPDIRECTDRAW2	lpGetDD() { return m_lpDD; }
protected:
	int CreateDirectDraw();
protected:
	LPDIRECTDRAW2	m_lpDD;

	iDDSurface*		m_lpDDSFront;
	iDDSurface*		m_lpDDSBack;

	LPDIRECTDRAWSURFACE m_ZBuffer;
	LPDIRECTDRAWPALETTE m_lpDDPalette;
	LPDIRECTDRAWCLIPPER m_lpClipper;

	DWORD m_dwPixelWidth;
	DWORD m_dwPixelHeight;
	DWORD m_BPP;
	void* m_hWnd;

	BOOL m_bFullScreen;
};

//////////////////////////////////////////////////////////////////////////////////
// iDDSurface Class
//////////////////////////////////////////////////////////////////////////////////
class iDDSurface : public IBitmap, public ICanvas
{
public:
	iDDSurface();
	virtual ~iDDSurface();

	int CreateDDSurface(iDDScreen *Screen,DDSURFACEDESC *ddsd,void *DATA);
	int Create(int width,int height,int colordepth);
	int Release();
	int SetScreen(iDDScreen *Screen) { this->Screen = Screen; return 0; }

	void *Lock(int* LinearSize);
	int Unlock();

	void SetColorKey(DWORD ColorKey);
	int  GetColorKey() { return ColorKey; }
	virtual int Blit(int x,int y,IBitmap *bitmap,int left,int top,int w,int h,int mode,void *data);

	LPDIRECTDRAWSURFACE lpGetDDS() { return m_lpDDS; }
	int CreateBackSurface(iDDSurface *Front);
protected:
	DWORD ColorKey;
	int DDrawBlit(int x,int y,iDDSurface* bitmap,int left,int top,int w,int h,int mode,void *data);
protected:
	HDC m_DC;
	int m_ColorKey;
	bool isLocked;

	RECT SrcRect;
	RECT DestRect;
	HFONT m_Font;
	DDSURFACEDESC m_DDSD;
	LPDIRECTDRAWSURFACE m_lpDDS;
	iDDScreen *Screen;
};

#endif

