#include "VmWin.h"
#include "Terminal.h"

HWND InitWindow(HINSTANCE);
HWND hWnd;
IKeyBuffer KeyBuffer;
TermStream tty;

int APIENTRY WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nShowCmd)
{
	if ((hWnd=InitWindow(hInstance))==NULL) return -1;
	ShowWindow(hWnd, SW_NORMAL);
	tty.SetWindow(GetDlgItem(hWnd,IDC_TERMINAL));
	isout.Bind(&tty);
	iserr.Bind(&tty);
	isin.Bind(&tty);
	VmWin WinVm;
	return WinVm.Run(lpCmdLine);
}

LRESULT CALLBACK WinProc(HWND,UINT,WPARAM,LPARAM);
HINSTANCE hInstance;

HWND InitWindow(HINSTANCE hInstance)
{
	HWND hwnd;
	WNDCLASSEX wincl; 
	wincl.hInstance		= hInstance;
	wincl.lpszClassName = "VmBetaWin";
	wincl.lpfnWndProc	= WinProc;
	wincl.style			= CS_DBLCLKS;
	wincl.cbSize		= sizeof(WNDCLASSEX);
	wincl.hIcon			= LoadIcon(hInstance,(LPCTSTR)IDI_ICON1);
	wincl.hIconSm		= LoadIcon(hInstance,(LPCTSTR)IDI_ICON1);
	wincl.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wincl.lpszMenuName	= NULL;
	wincl.cbClsExtra	= 0;
	wincl.cbWndExtra	= 0;
	wincl.hbrBackground = (HBRUSH) GetStockObject(LTGRAY_BRUSH);
	if(!RegisterClassEx(&wincl)) return 0;
	hwnd = CreateWindowEx(0,"VmBetaWin",
			"VmBeta Terminal v1.1b",WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT,CW_USEDEFAULT,
			544,375,HWND_DESKTOP,NULL,hInstance,NULL);
	::hInstance=hInstance;
	return hwnd;
}
#include <stdio.h>
#include <string.h>
LRESULT CALLBACK WinProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam)
{
	static char line[90]="KEY= ";
	static int Font=(int)GetStockObject(DEFAULT_GUI_FONT),i;
	switch (message)
	{
	case WM_CREATE:
		CreateWindow("EDIT", "",
			WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL | ES_MULTILINE |
			ES_WANTRETURN,
			CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
			hWnd, (HMENU)IDC_TERMINAL, hInstance, NULL);
		SendDlgItemMessage(hWnd, IDC_TERMINAL, WM_SETFONT,(WPARAM)Font, MAKELPARAM(TRUE, 0));
		for (i=0;i<256;i++) KeyBuffer[i]=0;
		break;
	case WM_CLOSE:
		if (MessageBox(hWnd,"The action will terminate the program, continue ?","Warning",MB_YESNO)!=IDYES) break;
		DestroyWindow(hWnd);
		exit(0);
		break;
	case WM_SIZE:
		if (wParam != SIZE_MINIMIZED) 
			MoveWindow(GetDlgItem(hWnd,IDC_TERMINAL),0,0,
					   LOWORD(lParam),HIWORD(lParam), TRUE);
		break;
	case WM_KEYDOWN:
		KeyBuffer[wParam]=1;
		break;
	case WM_KEYUP:
		KeyBuffer[wParam]=0;
		break;
	case WM_CHAR:
		KeyBuffer.UnGetCh(wParam);
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_TERMINAL:
			if (HIWORD(wParam)==WM_KEYDOWN) SetFocus(hWnd);
			return true;
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd,message,wParam,lParam);
	}
	return 0;
}
