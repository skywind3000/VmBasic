#include "idxdraw.h"
#include "ifstream.h"



//////////////////////////////////////////////////////////////////////////////////
// iDDScreen Class
//////////////////////////////////////////////////////////////////////////////////
iDDScreen::iDDScreen()
{
	m_lpDD = NULL;
	m_lpDDSFront = NULL;
	m_lpDDSBack = NULL;
	m_lpDDPalette = NULL;
	m_lpClipper = NULL;
	m_ZBuffer = NULL;
}

iDDScreen::~iDDScreen()
{
	ReleaseScreen();
}

BOOL iDDScreen::ReleaseScreen()
{
	if(m_lpDD != NULL) m_lpDD->RestoreDisplayMode();
	if (m_lpDDSBack)   { delete m_lpDDSBack; }
	if (m_lpDDSFront)  { delete m_lpDDSFront; }
	if (m_ZBuffer)     m_ZBuffer->Release(); 
	if (m_lpClipper)   m_lpClipper->Release();
	if (m_lpDDPalette) m_lpDDPalette->Release();
	if (m_lpDD)        m_lpDD->Release();
	m_lpDDSBack=NULL;
	m_lpDDSFront=NULL;
	m_ZBuffer=NULL;
	m_lpClipper=NULL;
	m_lpDDPalette=NULL;
	m_lpDD=NULL;
	return NULL;
}

int iDDScreen::GetDesktopBpp()
{
	int Bpp;
	HDC hDC;
	hDC = GetDC(NULL);
	Bpp = GetDeviceCaps(hDC, PLANES) * GetDeviceCaps(hDC, BITSPIXEL);
	ReleaseDC(NULL, hDC);
	return Bpp;
}
int iDDScreen::CreateDirectDraw()
{
	if (m_lpDD) return 0;
	LPDIRECTDRAW lpDD;
	HRESULT rval;
	rval = DirectDrawCreate(NULL, &lpDD, NULL);
	if(rval != DD_OK) return -1;
	rval = lpDD->QueryInterface(IID_IDirectDraw2, (LPVOID*)&m_lpDD);
	if(rval != DD_OK) { lpDD->Release(); return -2; }
	lpDD->Release();
	return 0;
}

BOOL iDDScreen::CreateWindowed(void *hWnd, int Width, int Height)
{
	HRESULT rval;
	DWORD dwFlags;
	DDSURFACEDESC ddsd;

	m_lpDD = NULL;
	m_bFullScreen = FALSE;
	m_lpDDPalette = NULL;
	m_dwPixelWidth  = Width;
	m_dwPixelHeight = Height;
	m_hWnd = hWnd;

	m_BPP = GetDesktopBpp();
	dwFlags = DDSCL_NORMAL;

	ReleaseScreen();
	if (CreateDirectDraw()) return false;

	rval = m_lpDD->SetCooperativeLevel((HWND)hWnd, dwFlags);
	if(rval != DD_OK) { ReleaseScreen(); return false; }

	ddsd.dwSize = sizeof(ddsd);
	ddsd.dwFlags = DDSD_CAPS;
	ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;

	m_lpDDSFront = new iDDSurface();
	m_lpDDSBack = new  iDDSurface();

	rval = m_lpDDSFront->CreateDDSurface(this, &ddsd, NULL);
	if (rval != 0) { ReleaseScreen(); return false; }

	ddsd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;    
	ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
	ddsd.dwWidth = Width;
	ddsd.dwHeight = Height;

	rval = m_lpDDSBack->CreateDDSurface(this, &ddsd, NULL);
	if(rval != 0) { ReleaseScreen(); return false; }

	// Create the window clipper
	rval = m_lpDD->CreateClipper(0, &m_lpClipper, NULL);
	if(rval != DD_OK) { ReleaseScreen(); return false; }

	rval = m_lpClipper->SetHWnd(0, (HWND)hWnd);
	if(rval != DD_OK) { ReleaseScreen(); return false; }

	rval = m_lpDDSFront->lpGetDDS()->SetClipper(m_lpClipper);
	if(rval != DD_OK) { ReleaseScreen(); return false; }

	return TRUE;
}

BOOL iDDScreen::CreateFullScreen(void *hWnd, DWORD Width, DWORD Height, DWORD BPP, BOOL bVGA)
{
	HRESULT rval;
	DWORD dwFlags;
	DDSURFACEDESC ddsd;
	DDSCAPS ddscaps;

	m_lpDD = NULL;
	m_bFullScreen = TRUE;
	m_lpDDPalette = NULL;
	m_dwPixelWidth = Width;
	m_dwPixelHeight = Height;
	m_BPP = BPP;
	m_hWnd = hWnd;

	dwFlags = DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN | DDSCL_ALLOWREBOOT | DDSCL_ALLOWMODEX;

	ReleaseScreen();
	if (CreateDirectDraw()) return false;

	rval = m_lpDD->SetCooperativeLevel((HWND)hWnd, dwFlags);
	if(rval != DD_OK) { ReleaseScreen(); return false; }

	if(bVGA) rval = m_lpDD->SetDisplayMode(m_dwPixelWidth, m_dwPixelHeight, m_BPP, 0, DDSDM_STANDARDVGAMODE);
	else rval = m_lpDD->SetDisplayMode(m_dwPixelWidth, m_dwPixelHeight, m_BPP, 0, 0);
	if(rval != DD_OK) { ReleaseScreen(); return false; }
	ddsd.dwSize = sizeof(ddsd);
	ddsd.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
	ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_FLIP | DDSCAPS_COMPLEX;
	ddsd.dwBackBufferCount = 1;

	m_lpDDSFront = new iDDSurface;
	m_lpDDSBack = new iDDSurface;

	rval = m_lpDDSFront->CreateDDSurface(this, &ddsd, NULL);
	if (rval != 0) { ReleaseScreen(); return false; }
	ddscaps.dwCaps = DDSCAPS_BACKBUFFER;

	rval = m_lpDDSBack->CreateBackSurface(m_lpDDSFront);
	if(rval) { ReleaseScreen(); return false; }
	return TRUE;
}

HRESULT iDDScreen::Flip()
{
	HRESULT rval;

	if (!m_lpDDSFront) return -1;

	if(m_bFullScreen)
	{
		rval = m_lpDDSFront->lpGetDDS()->Flip(NULL, DDFLIP_WAIT);
		if(rval == DDERR_SURFACELOST) Restore();
	}
	else
	{
		RECT Window;
		POINT pt;

		GetClientRect((HWND)m_hWnd, &Window);
		pt.x = pt.y = 0;
		ClientToScreen((HWND)m_hWnd, &pt);
		OffsetRect(&Window, pt.x, pt.y);
		rval = m_lpDDSFront->lpGetDDS()->Blt(&Window, m_lpDDSBack->lpGetDDS(), NULL, DDBLT_WAIT, NULL);
		if(rval == DDERR_SURFACELOST) Restore();
	}

	return rval;
}

void iDDScreen::Restore()
{
	if(m_lpDDSFront->lpGetDDS() != NULL)
		if (m_lpDDSFront->lpGetDDS()->IsLost() != DD_OK)
	{
		m_lpDDSFront->lpGetDDS()->Restore();
	}
}

char *iDDScreen::GetErrorMsg(HRESULT DDRVAL)
{
	struct DDErrorMessage
	{
		HRESULT Code;
		char *EngMsg;
		char *ChnMsg;
	};
	static DDErrorMessage LookupTable[] = 
	{
		{ DDERR_ALREADYINITIALIZED, "AlreadyInitialized", "" },
		{ DDERR_CANNOTATTACHSURFACE,"CannotAttachSurface", "" },
		{ DDERR_CANNOTDETACHSURFACE,"CannotDetachSurface", "" },
		{ DDERR_CURRENTLYNOTAVAIL,"CurrentlyNotAvail", "" },
		{ DDERR_EXCEPTION, "Exception", "" },
		{ DDERR_GENERIC, "Generic", "" },
		{ DDERR_HEIGHTALIGN, "HeightAlign", "" },
		{ DDERR_INCOMPATIBLEPRIMARY, "IncompatiblePrimary", "" },
		{ DDERR_INVALIDCAPS, "InvalidCaps", "" },
		{ DDERR_INVALIDCLIPLIST, "InvalidClipList", "" },
		{ DDERR_INVALIDMODE, "InvalidMode", "" },
		{ DDERR_INVALIDOBJECT, "InvalidObject", "" },
		{ DDERR_INVALIDPARAMS, "InvalidParams", "参数出错" },
		{ DDERR_INVALIDPIXELFORMAT, "InvalidPixelFormat", "" },
		{ DDERR_INVALIDRECT, "InvalidRect", "" },
		{ DDERR_LOCKEDSURFACES, "LockedSurfaces", "一个或者多个操作无法执行应为表面已被锁定" },
		{ DDERR_NO3D, "No3D", "不支持3D" },
		{ DDERR_NOALPHAHW, "NoAlphaHardware", "没有支持Alpha混色的硬件" },
		//{ DDERR_NOSTEREOHARDWARE, "NoStereoHardware", "没有支持Stereo(我不知道)的硬件" }，
		//{ DDERR_NOSURFACELEFT, "NoSurfaceLeft", "" },
		{ DDERR_NOCLIPLIST, "NoClipList", "" },
		{ DDERR_NOCOLORCONVHW, "NoColorConversion", "没有支持色彩转化的硬件" },
		{ DDERR_NOCOOPERATIVELEVELSET, "NoCooperativeLevelSet", "" },
		{ DDERR_NOCOLORKEY, "NoColorkey", "没有设置关键色" },
		{ DDERR_NOCOLORKEYHW, "NoColorkeyHardware", "没有支持关键色的硬件" },
		{ DDERR_NODIRECTDRAWSUPPORT, "NoDirectDrawSupport", "DDRAW不支持显示设备" },
		{ DDERR_NOEXCLUSIVEMODE, "NoExclusiveMode", "" },
		{ DDERR_NOFLIPHW, "NoFlipHardware", "" },
		{ DDERR_NOGDI, "NoGDI", "" },
		{ DDERR_NOMIRRORHW, "NoMirrorHardware", "" },
		{ DDERR_NOTFOUND, "ItemNotFind", "" },
		{ DDERR_NOOVERLAYHW, "NoOverlayHardware", "" },
		//{ DDERR_OVERLAPPINGRECTS, "OverlappingRects", "" },
		{ DDERR_NORASTEROPHW, "NoRasterOPHardware", "" },
		{ DDERR_NOROTATIONHW, "NoRotationHardware", "" },
		{ DDERR_NOSTRETCHHW, "NoStretchHardware", "" },
		{ DDERR_NOT4BITCOLOR, "No4BitColor", "" },
		{ DDERR_NOT4BITCOLORINDEX, "No4BitColorIndex", "" },
		{ DDERR_NOT8BITCOLOR, "No8BitColor", "" },
		{ DDERR_NOTEXTUREHW, "NoTextureMappingHardware", "" },
		{ DDERR_NOVSYNCHW, "NoVsyncHardware", "" },
		{ DDERR_NOZBUFFERHW, "NoZBufferHardware", "" },
		{ DDERR_NOZOVERLAYHW, "NoZOverlayHardware", "" },
		{ DDERR_OUTOFCAPS, "OutofCaps", "" },
		{ DDERR_OUTOFMEMORY, "OutofMemory", "" },
		{ DDERR_OUTOFVIDEOMEMORY, "OutofVideoMemory", "" },
		{ DDERR_OVERLAYCANTCLIP, "OverlayCantClip", "" },
		{ DDERR_OVERLAYCOLORKEYONLYONEACTIVE, "OverlayColorkeyOnlyOneActive", "" },
		{ DDERR_PALETTEBUSY, "PaletteBusy", "" },
		{ DDERR_COLORKEYNOTSET, "ColorkeyNotSet", "" },
		{ DDERR_SURFACEALREADYATTACHED, "SurfaceAlreadyAttached", "" },
		{ DDERR_SURFACEALREADYDEPENDENT,"SurfaceAlreadyDependent", "" },
		{ DDERR_SURFACEBUSY, "SurfaceBusy", "" },
		{ DDERR_NOBLTHW, "NoBltHardware", "" },
		{ DDERR_SURFACELOST, "SurfaceLost", "" },
		{ DDERR_UNSUPPORTED, "UnSupported", "" },
		{ DDERR_CANTLOCKSURFACE, "CantLockSurface", "" },
		{ DDERR_WASSTILLDRAWING, "WasStillDrawing", "" },
		{ DD_OK, "OK", "正常" },
		{ 0, 0, 0 },
	};
	char *msg = "NotFind";

	for (int i=0;LookupTable[i].ChnMsg!=NULL;i++)
		if (LookupTable[i].Code == DDRVAL)
			return LookupTable[i].EngMsg;
	return msg;
}
//////////////////////////////////////////////////////////////////////////////////
// iDDSurface Class
//////////////////////////////////////////////////////////////////////////////////
iDDSurface::iDDSurface()
{
	ClassCode = 2;
	m_lpDDS = NULL;
	Screen = NULL;
}

iDDSurface::~iDDSurface()
{
	Release();
}

int iDDSurface::Create(int Width, int Height, int ColorDepth)
{
	Release();
	if (!Screen) return -1;

	DDSURFACEDESC ddsd;
	int IsSystemMem = (ColorDepth<0)? DDSCAPS_SYSTEMMEMORY : 0;
	int retcode = 0;

	ColorDepth = (ColorDepth<0)? (-ColorDepth) : (ColorDepth);

	ZeroMemory(&ddsd,sizeof(ddsd));
	ddsd.dwSize = sizeof(ddsd);

	switch (ColorDepth) 
	{
	case 0:	
		ddsd.dwFlags = DDSD_CAPS|DDSD_WIDTH|DDSD_HEIGHT;
		ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN|IsSystemMem;
		break;
	case 8:
		ddsd.dwFlags = DDSD_CAPS|DDSD_WIDTH|DDSD_HEIGHT|DDSD_PIXELFORMAT;
		ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN|IsSystemMem;
		ddsd.ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);
		ddsd.ddpfPixelFormat.dwFlags = DDPF_RGB|DDPF_PALETTEINDEXED8;
		ddsd.ddpfPixelFormat.dwRGBBitCount = 8;
		break;
	case 15:
		ddsd.dwFlags = DDSD_CAPS|DDSD_WIDTH|DDSD_HEIGHT|DDSD_PIXELFORMAT;
		ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN|IsSystemMem;
		ddsd.ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);
		ddsd.ddpfPixelFormat.dwFlags = DDPF_RGB;
		ddsd.ddpfPixelFormat.dwRGBBitCount = 16;
		ddsd.ddpfPixelFormat.dwRBitMask = 0xfc00;
		ddsd.ddpfPixelFormat.dwGBitMask = 0x03e0;
		ddsd.ddpfPixelFormat.dwBBitMask = 0x001f;
		break;
	case 16:
		ddsd.dwFlags = DDSD_CAPS|DDSD_WIDTH|DDSD_HEIGHT|DDSD_PIXELFORMAT;
		ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN|IsSystemMem;
		ddsd.ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);
		ddsd.ddpfPixelFormat.dwFlags = DDPF_RGB;
		ddsd.ddpfPixelFormat.dwRGBBitCount = 16;
		ddsd.ddpfPixelFormat.dwRBitMask = 0xf800;
		ddsd.ddpfPixelFormat.dwGBitMask = 0x07e0;
		ddsd.ddpfPixelFormat.dwBBitMask = 0x001f;
		break;
	case 24:
		ddsd.dwFlags = DDSD_CAPS|DDSD_WIDTH|DDSD_HEIGHT|DDSD_PIXELFORMAT;
		ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN|IsSystemMem;
		ddsd.ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);
		ddsd.ddpfPixelFormat.dwFlags = DDPF_RGB;
		ddsd.ddpfPixelFormat.dwRGBBitCount = 24;
		ddsd.ddpfPixelFormat.dwRBitMask = 0xff0000;
		ddsd.ddpfPixelFormat.dwGBitMask = 0x00ff00;
		ddsd.ddpfPixelFormat.dwBBitMask = 0x0000ff;
		break;
	case 32:
		ddsd.dwFlags = DDSD_CAPS|DDSD_WIDTH|DDSD_HEIGHT|DDSD_PIXELFORMAT;
		ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN|IsSystemMem;
		ddsd.ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);
		ddsd.ddpfPixelFormat.dwFlags = DDPF_RGB;
		ddsd.ddpfPixelFormat.dwRGBBitCount = 32;
		ddsd.ddpfPixelFormat.dwRBitMask = 0xff0000;
		ddsd.ddpfPixelFormat.dwGBitMask = 0x00ff00;
		ddsd.ddpfPixelFormat.dwBBitMask = 0x0000ff;
		ddsd.ddpfPixelFormat.dwRGBAlphaBitMask = 0xff000000;
		break;
	default:
		ddsd.dwFlags = DDSD_CAPS|DDSD_WIDTH|DDSD_HEIGHT;
		ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN|DDSCAPS_VIDEOMEMORY;
		break;
	}
	ddsd.dwWidth = Width;
	ddsd.dwHeight = Height;

	retcode = CreateDDSurface(Screen, &ddsd, NULL);

	return retcode;
}

int iDDSurface::Release()
{
	Unlock();
	if (m_lpDDS) m_lpDDS->Release();
	m_lpDDS = NULL;
	this->bmPixel = NULL;
	if (this->Scanlines) delete this->Scanlines;
	this->Scanlines = NULL;
	this->ColorKey = 0xffffffffl;
	IBitmap::Release();
	return 0;
}

int iDDSurface::CreateBackSurface(iDDSurface *Front)
{
	Release();

	DDSCAPS ddscaps;
	ddscaps.dwCaps = DDSCAPS_BACKBUFFER;
	HRESULT rval = Front->lpGetDDS()->GetAttachedSurface(&ddscaps, &m_lpDDS);
	if (rval != DD_OK) {
		iserr<<"[iDDSurface::CreateBackSurface] Can not attach back surface\n";
		return -1;
	}

	ZeroMemory(&m_DDSD, sizeof(DDSURFACEDESC));
	m_DDSD.dwSize = sizeof(DDSURFACEDESC);

	m_lpDDS->GetSurfaceDesc(&m_DDSD);
	this->bmWidth = m_DDSD.dwWidth;
	this->bmHeight = m_DDSD.dwHeight;
	this->bmLinearSize = m_DDSD.dwLinearSize;
	this->bmColorDepth = m_DDSD.ddpfPixelFormat.dwRGBBitCount;

	isLocked = false;
	int LinearSize;
	bmPixel = (char*)Lock(&LinearSize);
	if (!bmPixel) {
		iserr<<"[iDDSurface::CreateBackSurface] Can not Lock Surface\n";
		Release();
		return -3;
	}
	Unlock();
	return 0;
}

int iDDSurface::CreateDDSurface(iDDScreen *Screen,DDSURFACEDESC *ddsd,void *DATA)
{
	Release();

	SetScreen(Screen);
	if (!Screen) return -1;

	LPDIRECTDRAW2 lpDD = Screen->lpGetDD();

	HRESULT rval = lpDD->CreateSurface(ddsd, &m_lpDDS, NULL);

	if (rval != DD_OK) {
		Release();
		DATA = DATA;
		iserr.VarArg("[iDDSurface::CreateDDSurface] %s\n",Screen->GetErrorMsg(rval));
		return -2;
	}

	ZeroMemory(&m_DDSD, sizeof(DDSURFACEDESC));
	m_DDSD.dwSize = sizeof(DDSURFACEDESC);

	m_lpDDS->GetSurfaceDesc(&m_DDSD);
	this->bmWidth = m_DDSD.dwWidth;
	this->bmHeight = m_DDSD.dwHeight;
	this->bmLinearSize = m_DDSD.dwLinearSize;
	this->bmColorDepth = m_DDSD.ddpfPixelFormat.dwRGBBitCount;

	isLocked = false;
	int LinearSize;
	bmPixel = (char*)Lock(&LinearSize);
	if (!bmPixel) {
		iserr<<"[iDDSurface::CreateDDSurface] Can not Lock Surface\n";
		Release();
		return -3;
	}
	Unlock();
	return 0;
}

void iDDSurface::SetColorKey(DWORD Color)
{
	DDCOLORKEY ddck;
	HRESULT ddrval;

	ddck.dwColorSpaceLowValue = Color;
	ddck.dwColorSpaceHighValue = Color;
	if (!m_lpDDS) return;
	if (isLocked) Unlock();
	ddrval = m_lpDDS->SetColorKey(DDCKEY_SRCBLT, &ddck);
	ColorKey = (ddrval == DD_OK)? Color : 0xffffffff;
}

int iDDSurface::Blit(int x,int y,IBitmap* bitmap,int left,int top,int w,int h,int mode,void *data)
{
	int retcode = 0;

	if (bitmap==NULL) return -1;

	if (ClassCode == bitmap->GetClassCode()) 
		retcode = DDrawBlit(x,y,(iDDSurface*)bitmap,left,top,w,h,mode,data);
	else
		retcode = IBitmap::Blit(x,y,bitmap,left,top,w,h,mode,data);

	return retcode;
}

int iDDSurface::DDrawBlit(int x,int y,iDDSurface* surface,int left,int top,int w,int h,int mode,void *data)
{
	int right = left + w;
	int bottom = top + h;
	int retcode;

	if (mode&IBITMAP_BLT_CLIP) 
		if (!clip(bmWidth,bmHeight,surface->GetW(),surface->GetH(),&x,&y,&left,&top,&right,&bottom)) return 0;

	RECT SrcRect = { left, top, right, bottom }; 
	DWORD flag = 0;

	if (!m_lpDDS) return -1;
	if (surface->lpGetDDS()==NULL) return -2;

	if (mode >= 0) {
		int KeyColor = (DWORD)mode;
		if (KeyColor != surface->GetColorKey()) 
			surface->SetColorKey(KeyColor);
		flag = DDBLTFAST_WAIT|DDBLTFAST_SRCCOLORKEY;
	}	else flag = DDBLTFAST_WAIT;

	retcode = m_lpDDS->BltFast(x,y, surface->lpGetDDS(), &SrcRect, flag);

	return retcode;
}

void *iDDSurface::Lock(int* LinearSize)
{
	if (LinearSize) *LinearSize = 0;
	if (!m_lpDDS) return NULL;

	if (isLocked) Unlock();

	DDSURFACEDESC ddsd;
	HRESULT ddrval;

	ZeroMemory(&ddsd, sizeof(DDSURFACEDESC));
	ddsd.dwSize = sizeof(DDSURFACEDESC);

	ddrval = m_lpDDS->Lock(NULL,&ddsd,DDLOCK_WAIT,NULL);

	if (ddrval == DDERR_SURFACELOST) {
		m_lpDDS->Restore();
		ddrval = m_lpDDS->Lock(NULL,&ddsd,DDLOCK_WAIT,NULL);
	}
	if (ddrval != DD_OK) {
		iserr.VarArg("[iDDSurface::Lock] Surface Lock error: %xh\n", ddrval);
		return NULL;
	}
	if (bmLinearSize != (int)ddsd.dwLinearSize || 
		bmPixel != ddsd.lpSurface) {
		delete Scanlines;
		Scanlines = NULL;
	}
	this->bmLinearSize = ddsd.dwLinearSize;
	this->bmPixel = (char*)ddsd.lpSurface;
	this->isLocked = true;
	if (!Scanlines) {
		Scanlines = new void*[bmHeight];

		if (!Scanlines) { 
			iserr<<"[iDDSurface::Lock] Can not alloc line index\n";
			Unlock(); 
			return NULL; 
		}
		for (int i = 0, j = 0; i < bmHeight; i++, j+=bmLinearSize)
			Scanlines[i] = bmPixel + j;
		Pixels8=(unsigned char**)Scanlines;
		Pixels16=(unsigned short**)Scanlines;
		Pixels24=(IColor**)Scanlines;
		Pixels32=(IColorA**)Scanlines;
		canWidth=bmWidth;
		canHeight=bmHeight;
		canBpp=bmColorDepth;
		canLinearSize=bmLinearSize;
	}
	if (LinearSize) *LinearSize = bmLinearSize;
	return bmPixel;
}

int iDDSurface::Unlock()
{
	if (isLocked == 0) return 0;
	if (!m_lpDDS) return 0;
	HRESULT ddrval = m_lpDDS->Unlock(NULL);
	if (ddrval != DD_OK) return -1;
	isLocked = false;
	return 0;
}
