#include "VmBetaV.h"
#include "VmWin.h"
#include "Terminal.h"
#include <time.h>

_int64 CpuRDTSC();
HRESULT WinMessage();
/////////////////////////////////////////////////////////////////////
IVMBetaV::IVMBetaV()
{
}

IVMBetaV::~IVMBetaV()
{
}

extern  TermStream tty;
extern  IKeyBuffer KeyBuffer;
extern  HWND hWnd;

void IVMBetaV::VmOut(vdword port,vdword value)
{
	static char line[256];
	switch (port)
	{
	case 0x0A: tty.Read(line,250); sscanf(line,"%ld",&VmRegs.R3); break;
	case 0x0B: tty.Read(line,250); (*VmStrings)[-(long)VmRegs.R3]->Set(line); break;
	case 0x0C: tty.Read(line,250); sscanf(line,"%f",&VmRegs.R3); break;
	case 50: fnCreateBmp(); break;
	case 51: fnReleaseBmp(); break;
	case 52: fnLoadBmp(); break;
	case 53: fnBlitBmp(); break;
	case 54: fnShowBmp(); break;
	case 55: fnPixelBmp(); break;
	case 56: fnGetPixelBmp(); break;
	case 57: fnFillBmp(); break;
	case 58: ShowWindow(GetDlgItem(hWnd,IDC_TERMINAL),SW_HIDE);
			 ShowWindow(GetDlgItem(hWnd,IDC_TERMINAL),SW_SHOWNORMAL);
		     break;
	case 59: if (VmRegs.R3>0) 
				 VmRegs.R3=(DWORD)((CpuRDTSC()^0xffffffffL)+(CpuRDTSC()>>10))%VmRegs.R3; 
		     break;
	case 60: VmRegs.R3=KeyBuffer[VmRegs.R3]; break;
	default:
		IVMBeta::VmOut(port,value);
	}
}


void IVMBetaV::fnCreateBmp()
{
	int h=bmList.Alloc();
	if (h<0) { VmRegs.R3=-1; return; }
	IGDIBitmap *bmp=bmList[h];
	if (bmp->Create(VmRegs.R2,VmRegs.R3,16)) { VmRegs.R3=-2; return; }
	VmRegs.R3=h;
}
void IVMBetaV::fnReleaseBmp()
{
	IGDIBitmap *bmp=bmList[VmRegs.R3];
	if (bmList.Error) { VmError(200); return; }
	bmp->Release();
	bmList.Release(VmRegs.R3);
}
void IVMBetaV::fnFillBmp()
{
	struct td { int color,h,w,y,x,dest; };
	td *data=(td*)&VirtualMem[VmRegs.R3];
	IGDIBitmap *bmp=bmList[data->dest];
	if (bmList.Error) { bmList.Error=0; return; }
	bmp->Fill(*(IColor*)&data->color);
}
void IVMBetaV::fnLoadBmp()
{
	IGDIBitmap nbmp,*bmp;
	int h=bmList.Alloc();
	char *text=VmIOString(VmRegs.R3);
	VmRegs.R3=-1;
	bmp=bmList[h];
	MsgPrint("load file %s\r\n",text);
	if (text==NULL) { VmError(201); return; }
	if (h<0) return;
	if (nbmp.LoadFromStream(&Ifstream(text,ISTREAM_READ|ISTREAM_BINARY),1)) return;
	if (bmp->Create(nbmp.GetW(),nbmp.GetH(),16)) return;
	int i,j;
	for (j=0;j<nbmp.GetH();j++) 
		for (i=0;i<nbmp.GetW();i++) 
			bmp->Pixel(i,j,nbmp.GetPixel(i,j));
	MsgPrint("the pic size is %dx%d\r\n",bmp->GetW(),bmp->GetH());
	nbmp.Release();
	VmRegs.R3=h;
}
void IVMBetaV::fnBlitBmp()
{
	struct td { int colorkey; int y,x,h,w,dy,dx; int src,dst; };
	td *data=(td*)&VirtualMem[VmRegs.R3];
	IGDIBitmap *dest=bmList[data->dst],*src=bmList[data->src];
	dest->Blit(data->dx,data->dy,src,data->x,data->y,data->w,data->h,data->colorkey,NULL);
}
void IVMBetaV::fnShowBmp()
{
	RECT rt;
	GetClientRect(hWnd, &rt);
	IGDIBitmap *bmp=bmList[VmRegs.R3];
	if (bmList.Error) return;
	HDC hDC=GetDC(hWnd);
	bmp->BlitDC(hDC,0,0,rt.right,rt.bottom);
	ReleaseDC(hWnd,hDC);
}
void IVMBetaV::fnPixelBmp()
{
	struct td { int c,y,x,dest; };
	td *data=(td*)&VirtualMem[VmRegs.R3];
	IGDIBitmap *bmp=bmList[data->dest];
	if (bmList.Error) { bmList.Error=0; return; }
	bmp->Pixel(data->x,data->y,*(IColor*)&data->c);
}
void IVMBetaV::fnGetPixelBmp()
{
	struct td { int y,x,dest; };
	td *data=(td*)&VirtualMem[VmRegs.R3];
	IGDIBitmap *bmp=bmList[data->dest];
	if (bmList.Error) { bmList.Error=0; return; }
	IColor c=bmp->GetPixel(data->x,data->y),d={0,0,0};
	VmRegs.R3=*(int*)&c;
}
/////////////////////////////////////////////////////////////////////

void IVMBetaV::VmError(vdword error)
{
	IVMBeta::VmError(error);
}
void IVMBetaV::Excute()
{
	_int64 delta=200000L,time=0;
	VmReset();
	RunFlag=1;
	while (VmStatus==0&&RunFlag) 
	{
		Process();
		if (CpuRDTSC()-time>delta) {
			time=CpuRDTSC();
			WinMessage();
		}
	}
	if (VmStatus==1) VmStatus=0;
}

/////////////////////////////////////////////////////////////////////
_int64 CpuRDTSC()
{
	vdword HI,LO;
	_asm {
		rdtsc
		mov HI,edx
		mov LO,eax
	}
	return (((_int64)HI<<32)+(_int64)LO);
}

HRESULT WinMessage() 
{
    MSG msg; 
    if (PeekMessage(&msg,NULL,0,0,PM_NOREMOVE)) // 如果有消息
        if (GetMessage(&msg,NULL,0,0))          // 取得消息
        {
            TranslateMessage(&msg);             // 处理虚拟击键
            DispatchMessage(&msg);              // 投递消息到窗口
        } 
    return 0; 
}

int MsgPrint(char *fmt,...)
{
	int i;
	static char line[4096];
	char *s=line;
	va_list argptr;

	va_start(argptr,fmt);
	vsprintf(line,fmt,argptr);
	va_end(argptr);

	HWND hwnd;
	hwnd = FindWindow("MsgViewerClass","MsgViewer");
	if (hwnd==0) return -1;
	for (i=0;(*s)&&i<0xffff;s++,i++) SendMessage(hwnd,WM_USER+1,(i&0xffff)|(((int)*s)<<16),0);
	SendMessage(hwnd,WM_USER+1,(i&0xffff),0);
	SendMessage(hwnd,WM_USER+2,0,0);
	return 0;
} 
