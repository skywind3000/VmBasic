#ifndef _VMBASIC_H_
#define _VMBASIC_H_

#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Menus.hpp>
#include <Grids.hpp>
#include <ExtCtrls.hpp>
#include <ComCtrls.hpp>
#include <ToolWin.hpp>
#include <Buttons.hpp>
#include <ImgList.hpp>
#include <Dialogs.hpp>

#include "..\..\ivmscc.h"

class IVmBasic
{
protected:
    iString StartDir;
    IVMBetaScript VmBSC;
    iString VmbetaName;
    iString VmlibName;
    Imstream LogText;
    Imstream VmCode;
    Imstream VmBasic;
    int TextColor;
    int AsmColor;
    void SplitFileName(char *name,iString &path,iString &main,iString &ext);    
public:
    HWND hWnd;
    IVmBasic() { iserr.Bind(&LogText); }
    void LoadConfig();
    void SaveConfig();
    int Compile();
    void SaveVm(char *FileName);
    void ShowAsm(TStream *stream);
    void RunProg(char *FileName);
    void RunVms(char *VmsFile);
    void Config();
};

class ILogStream:public Ixstream
{
protected:
        TMemo *VmLog;
	virtual int DoEof() { return 0; }
	virtual int DoRead(void *buffer,long size) { return size; }
	virtual int DoWrite(void *buffer,long size) { VmLog->Lines->Add((char*)buffer); return size; }
	virtual int DoSeek(long Pos,int mode) { return 0; }
	virtual int DoGetPos() { return 0; }
	virtual int DoPutCh(int ch) { VmLog->Lines->Text+=(char)ch; return 0; }
	virtual int DoGetCh() { VmLog->Lines->Text=""; return 0; }
public:
        void Startup(TMemo *log) { VmLog=log; }
        void Clear() { VmLog->Lines->Text=""; }
};
#endif

