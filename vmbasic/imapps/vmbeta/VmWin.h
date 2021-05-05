#ifndef _VMWIN_H
#define _VMWIN_H

#include <windows.h>
#include "..\..\ikeybord.h"
#include "..\..\istring.h"
#include "resource.h"
#include "VmBetaV.h"

#define IDC_TERMINAL   1001

extern  IKeyBuffer KeyBuffer;

HRESULT WinMessage();
int     MsgPrint(char *fmt,...);

class VmWin
{
protected:
	iString  CmdLine;
	iString  FileName;
	IVMBetaV vmbeta;
	void logo();
	void SplitFileName(char *name,iString &path,iString &main,iString &ext);
public:
	int Run(char *CmdLine);
};

#endif
