/**************************************************************
  简易OpenGL初始化库：

  提供用于常用方式的OpenGL初始化函数，和反初始化函数，使你
  更快，更简单地开始OpenGL编程。提醒一下还需要opengl32.lib还
  有glu32.lib两个库文件来运行OpenGL，下面是使用例子：

	int APIENTRY WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nShowCmd)
	{
		IEasyGlFrame EasyGl;		// 这个头文件提供的类
		// 初始化400x300x32的窗口
		if (EasyGl.InitGl(hInstance,400,300,32)==NULL) return -1;
		while (EasyGl.ProcessMessage())
		{
			DoMyRenderScene();		// 你自己的作图函数
			.......
			EasyGl.CalculateFrameRate(true);	// 显示FPS在标题
			SwapBuffers(EasyGl.GetHDC());		// 交换缓存
		}
		EasyGl.DeInitGl();						// 放初始化
		return 0;
	}
 */

#ifndef __EASY_OPENGL_H__
#define __EASY_OPENGL_H__

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <gl\gl.h>										// Header File For The OpenGL32 Library
#include <gl\glu.h>										// Header File For The GLu32 Library

static RECT IEasyGlFrame_Private_Rect;
static bool IEasyGlFrame_Private_FullScreen;
static bool IEasyGlFrame_Private_EscapeKey;

class IEasyGlFrame
{
protected:
	HINSTANCE	g_hInstance;
	HDC			g_hDC;
	HWND		g_hWnd;
	HGLRC		g_hRC;
	static void SizeOpenGLScreen(int width, int height)
	{
		if (height==0) height=1;
		glViewport(0,0,width,height);						// Make our viewport the whole window
		glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
		glLoadIdentity();									// Reset The Projection Matrix
		gluPerspective(45.0f,(GLfloat)width/(GLfloat)height,1 ,150.0f);
		glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
		glLoadIdentity();									// Reset The Modelview Matrix
	}
	static HRESULT CALLBACK WinProc(HWND hWnd,UINT Message,WPARAM wParam,LPARAM lParam)
	{
		LONG    lRet = 0; 
		PAINTSTRUCT    ps;
		switch (Message)
		{ 
	    case WM_SIZE:										// If the window is resized
			if(IEasyGlFrame_Private_FullScreen==false)							// Do this only if we are NOT in full screen
			{
				SizeOpenGLScreen(LOWORD(lParam),HIWORD(lParam));// LoWord=Width, HiWord=Height
				GetClientRect(hWnd,&IEasyGlFrame_Private_Rect);					// Get the window rectangle
			}
		    break; 
		case WM_PAINT:										// If we need to repaint the scene
			BeginPaint(hWnd, &ps);							// Init the paint struct		
			EndPaint(hWnd, &ps);							// EndPaint, Clean up
			break;
		case WM_KEYDOWN:
			if(wParam==VK_ESCAPE) IEasyGlFrame_Private_EscapeKey=true;			// Quit if we pressed ESCAPE
			break;
	    case WM_DESTROY:
			IEasyGlFrame_Private_EscapeKey=true;
		    break; 
	    default:
			lRet=DefWindowProc(hWnd,Message,wParam,lParam); 
		    break; 
	    } 
		return lRet;
	}

protected:
	int ChangeToFullScreen(int ScrWidth,int ScrHeight)
	{
		DEVMODE dmSettings;	
		memset(&dmSettings,0,sizeof(dmSettings));
		if(!EnumDisplaySettings(NULL,ENUM_CURRENT_SETTINGS,&dmSettings)) return -1;
		dmSettings.dmPelsWidth	= ScrWidth;				// Selected Screen Width
		dmSettings.dmPelsHeight	= ScrHeight;			// Selected Screen Height
		int result = ChangeDisplaySettings(&dmSettings,CDS_FULLSCREEN);	
		if(result != DISP_CHANGE_SUCCESSFUL)
		{
			MessageBox(NULL, "Display Mode Not Compatible", "Error", MB_OK);
			PostQuitMessage(0);
		}
		return 0;
	}
	HWND NewWindow(LPSTR strWindowName,int width,int height,DWORD dwStyle,bool bFullScreen,HINSTANCE hInstance)
	{
		HWND hWnd;
		WNDCLASS wndclass;
		memset(&wndclass,0,sizeof(WNDCLASS));
		wndclass.style=CS_HREDRAW|CS_VREDRAW;
		wndclass.lpfnWndProc=WinProc;
		wndclass.hInstance=hInstance;
		wndclass.hIcon=LoadIcon(NULL,IDI_APPLICATION);
		wndclass.hCursor=LoadCursor(NULL,IDC_ARROW);
		wndclass.hbrBackground=(HBRUSH)(COLOR_WINDOW+1);
		wndclass.lpszClassName="GameTutorials";
		RegisterClass(&wndclass);
		if(bFullScreen&&!dwStyle)
		{
			dwStyle=WS_POPUP|WS_CLIPSIBLINGS|WS_CLIPCHILDREN;
			ChangeToFullScreen(width,height);
			ShowCursor(FALSE);
		} else if(!dwStyle) dwStyle = WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
		g_hInstance=hInstance;
		RECT rWindow;
		rWindow.left	= 0;
		rWindow.right	= width;
		rWindow.top	    = 0;
		rWindow.bottom	= height;
		AdjustWindowRect( &rWindow, dwStyle, false);
		hWnd = CreateWindow("GameTutorials", strWindowName, dwStyle, 0, 0,
							rWindow.right  - rWindow.left, rWindow.bottom - rWindow.top, 
							NULL, NULL, hInstance, NULL);
		if(!hWnd) return NULL;
		ShowWindow(hWnd, SW_SHOWNORMAL);
		UpdateWindow(hWnd);
		SetFocus(hWnd);
		return hWnd;
	}
	int bSetupPixelFormat(HDC hdc,int ColorDepth) 
	{ 
	    PIXELFORMATDESCRIPTOR pfd; 
		int pixelformat; 
	    pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);			// Set the size of the structure
		pfd.nVersion = 1;									// Always set this to 1
		pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER; 
		pfd.dwLayerMask = PFD_MAIN_PLANE;					// We want the standard mask (this is ignored anyway)
		pfd.iPixelType = PFD_TYPE_RGBA;						// We want RGB and Alpha pixel type
		pfd.cColorBits = ColorDepth;						// Here we use our #define for the color bits
		pfd.cDepthBits = ColorDepth;						// Depthbits is ignored for RGBA, but we do it anyway
		pfd.cAccumBits = 0;									// No special bitplanes needed
		pfd.cStencilBits = 0;								// We desire no stencil bits
		if ((pixelformat=ChoosePixelFormat(hdc,&pfd))==FALSE) return -1;
		if (SetPixelFormat(hdc,pixelformat,&pfd)==FALSE) return -2;
		return 0;											// Return a success!
	}
	int InitializeOpenGL(int width,int height,int ColorDepth)
	{  
		g_hDC=GetDC(g_hWnd);											
		if (bSetupPixelFormat(g_hDC,ColorDepth)) return -1;
		g_hRC=wglCreateContext(g_hDC);
		wglMakeCurrent(g_hDC, g_hRC);
		SizeOpenGLScreen(width, height);
		return 0;
	}
public:
	IEasyGlFrame() { g_hDC=NULL; g_hRC=NULL; IEasyGlFrame_Private_FullScreen=false; };
	~IEasyGlFrame() { DeInitGl(); }
	HWND InitGl(HINSTANCE hInstance,int Width,int Height,int ColorDepth,bool FullScreen=false,bool cansize=true)
	{
		RECT Rect;
		DWORD dwStyle;
		IEasyGlFrame_Private_FullScreen=FullScreen;
		IEasyGlFrame_Private_EscapeKey=false;
		dwStyle=(cansize&&FullScreen==false)?(0):(WS_VISIBLE|WS_SYSMENU);
		g_hWnd=NewWindow("",Width,Height,dwStyle,FullScreen,hInstance);
		if (!g_hWnd) return NULL;
		GetClientRect(g_hWnd,&Rect);
		if (InitializeOpenGL(Rect.right,Rect.bottom,ColorDepth)) {
			DestroyWindow(g_hWnd);
			return NULL;
		}
		return g_hWnd;
	}
	void DeInitGl()
	{
		if (g_hRC)											
		{
			wglMakeCurrent(NULL, NULL);						// This frees our rendering memory and sets everything back to normal
			wglDeleteContext(g_hRC);						// Delete our OpenGL Rendering Context	
		}
		if (g_hDC) ReleaseDC(g_hWnd, g_hDC);	
		if(IEasyGlFrame_Private_FullScreen)
		{
			ChangeDisplaySettings(NULL,0);					// If So Switch Back To The Desktop
			ShowCursor(TRUE);								// Show Mouse Pointer
		}
		g_hDC=NULL;
		g_hRC=NULL;
		IEasyGlFrame_Private_FullScreen=false;
		UnregisterClass("GameTutorials", g_hInstance);		// Free the window class
		PostQuitMessage(0);
	}
	int ProcessMessage()
	{
		MSG msg;
		if (IEasyGlFrame_Private_EscapeKey==true) return 0;
		if (PeekMessage(&msg,NULL,0,0,PM_NOREMOVE))
			if (GetMessage(&msg,NULL,0,0))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		return 1;
	}
	float CalculateFrameRate(bool TitleView=false,int TestTime=1)
	{
		static float FrameRate=0;
		static long  LastTime=0;
		static long  FramePerSecond=0;
		static char  Title[100];
		long CurrentTime=GetTickCount()-LastTime;

		FramePerSecond++;
		if (TestTime<1) TestTime=1;
		if (CurrentTime>TestTime*1000) 
		{
			FrameRate=((float)FramePerSecond/TestTime);
			LastTime=GetTickCount();
			FramePerSecond=0;
			if (TitleView)
			{
				sprintf(Title,"Current Frames Per Second: %.2f",FrameRate);
				SetWindowText(g_hWnd,Title);
			}
		}
		return FrameRate;
	}
	HDC GetHDC() { return g_hDC; }
};

#endif