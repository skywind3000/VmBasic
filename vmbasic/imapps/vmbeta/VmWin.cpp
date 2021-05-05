#include "VmWin.h"
#include "Terminal.h"
#include "VmBetaV.h"
#include "..\..\ivmsca.h"

extern TermStream tty;

int VmWin::Run(char *line)
{
	extern HWND hWnd;
	CmdLine=line;
	int i=0,len=CmdLine.GetLength();
	while (line[i]==' '&&i<len) i++;
	FileName=&line[i];

	IVMBetaAsm vmasm;
	if (FileName=="") {
		logo();
		KeyBuffer.Clear();
		tty.GetCh();
		DestroyWindow(hWnd);
		return 0;		
	}
	int result;
	result=vmasm.LoadAsm(vmbeta,FileName.GetString());
	if (result<0) {
		iserr.VarArg("%s\n",vmasm.ErrorCode());
		iserr.VarArg("load %s failed %d\n",FileName.GetString(),result);
		KeyBuffer.Clear();
		tty.GetCh();
		DestroyWindow(hWnd);
		return -2;
	}
	char linex[1024];
	GetCurrentDirectory(1000,linex);
	iString path,mname,ext;
	SplitFileName(FileName.GetString(),path,mname,ext);
	SetCurrentDirectory(path.GetString());
	vmbeta.Excute();
	SetCurrentDirectory(linex);
	isout.VarArg("\nProgram exit with code %d, press key to exit...",vmbeta.GetStatus());
	KeyBuffer.Clear();
	tty.GetCh();
	DestroyWindow(hWnd);
	return 0;
}

void VmWin::logo()
{
	isout<<"IVirtual Machine Visual Version 1.1 for Windows\n";
	isout<<"Produced by newbird 1997-2002 game group written by skywind skywindt@yeah.net\n";
	isout<<"\n";
	isout<<"Syntax is: vmbeta filename";
}


void VmWin::SplitFileName(char *name,iString &path,iString &main,iString &ext)
{
    int len,i,j,k;
    len=strlen(name);
    for (i=len-1;i>=0;i--) if (name[i]=='\\'||name[i]=='/') break;
    if (i>=0) i=i+1;
    else i=0;
    path="";
    main="";
    ext="";
    for (j=0;j<i;j++) path.Append(name[j]);
    for (j=len-1;j>=i;j--) if (name[j]=='.') break;
    if (j<i) j=len;
    for (k=i;k<j;k++) main.Append(name[k]);
    for (k=j+1;k<len;k++) ext.Append(name[k]);
}
