#ifndef TERMINAL_H
#define TERMINAL_H

#include "VmWin.h"
#include "..\..\itemplat.h"
#include "..\..\ifstream.h"

class Terminal
{
public:
	IDataArray<char> Text;
protected:
	HWND hWnd;
	int MaxLine,Tp;
	int TabSpace;
	void DoPutCh(int c) {
		if (c=='\n'||c=='\r') { Text[Tp++]='\r'; Text[Tp++]='\n'; } else
		//if (c=='\r') { for (;Tp>0&&Text[Tp-1]!='\n';Tp--); Text[Tp]=' '; } else
		if (c==8) { if (Text[Tp-1]!='\n'&&Tp>0) Text[--Tp]=' '; } else
		if (c=='\t') { 
			int i,ntp; 
			for (i=Tp;i>=0&&Text[i]!='\n';i--); 
			ntp=i+(((Tp-i)/TabSpace)+1)*TabSpace;
			for (;Tp<=ntp;Tp++) Text[Tp]=' ';
		}	else Text[Tp++]=c;
	}
	int GetCh()
	{
		int c;
		do {
			c=KeyBuffer.GetCh();
			WinMessage();
		}	while (c<0);
		return c;
	}
public:
	Terminal() { Clear(); MaxLine=1000; TabSpace=10; }
	void SetWindow(HWND hWnd) { this->hWnd=hWnd; }
	void Clear()
	{
		Text.SetLength(0);
		Text.SetAutoAlloc(1);
		Tp=0;
	}
	void Puts(char *s) {
		int len=strlen(s),i,sp=Tp;
		Text.SetLength(Tp+len);
		for (i=0;i<len;i++) {
			DoPutCh(s[i]);
		}
		Text[Tp]=0;
		SendMessage(hWnd,EM_SETSEL,(WPARAM)(sp),(LPARAM)(sp));
		SendMessage(hWnd,EM_REPLACESEL,FALSE,(LPARAM)&Text[sp]);
		SendMessage(hWnd,EM_SCROLLCARET,0,0);		
	}
	void PutCh(int c) { 
		int sp=Tp;
		DoPutCh(c); 
		Text[Tp]=0;
		SendMessage(hWnd,EM_SETSEL,(WPARAM)(sp),(LPARAM)(sp));
		SendMessage(hWnd,EM_REPLACESEL,FALSE,(LPARAM)&Text[sp]);
		SendMessage(hWnd,EM_SCROLLCARET,0,0);		
	}
	void Gets(IDataArray<char> &text) {
		int i=0,ch;
		text.SetLength(0);
		text.SetAutoAlloc(1);
		for (ch=0;ch!='\r'&&ch!='\n';) {
			ch=GetCh();
			if (ch==8) { if (i>0) { i--; PutCh(ch); } }
			else { if (ch=='\r') ch='\n'; text[i++]=ch; PutCh(ch); }
		}
		text[i++]=0;
		text.SetLength(i);
	}
	int ReadKey()
	{
		int ch=GetCh();
		PutCh(ch);
		return 0;
	}
};

class TermStream:public Ixstream
{
protected:
	Terminal tty;
	int DoEof() { return 0; }
	int DoRead(void *buffer,long size) { 
		IDataArray<char> t; 
		tty.Gets(t); 
		strncpy((char*)buffer,&t[0],size-1); 
		((char*)buffer)[size]=0; 
		return 0;
	}
	int DoWrite(void *buffer,long size) { 
		tty.Puts((char*)buffer); 
		return 0;
	}
	int DoSeek(long Pos,int mode) { return Pos+mode; }
	int DoGetPos() { return 0; }
	int DoPutCh(int ch) { tty.PutCh(ch); return 0; }
	int DoGetCh() { return tty.ReadKey(); }
public:
	void SetWindow(HWND hWnd) { tty.SetWindow(hWnd); }
};

#endif

