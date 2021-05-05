#ifndef VMBETA_H
#define VMBETA_H

#include "..\..\ivmsbeta.h"
#include "..\..\igdibmp.h"

class IVMBetaV:public IVMBeta
{
protected:
	ILister<IGDIBitmap>	bmList;
	void fnCreateBmp();
	void fnReleaseBmp();
	void fnLoadBmp();
	void fnBlitBmp();
	void fnShowBmp();
	void fnPixelBmp();
	void fnGetPixelBmp();
	void fnFillBmp();
	void fnPlayMidi();
	void fnStopMidi();
public:
	int RunFlag;
	IVMBetaV();
	virtual ~IVMBetaV();
	virtual void   VmError(vdword error);
	virtual void   VmOut(vdword port,vdword value);
	virtual void Excute();
	virtual void VmReset() { bmList.Clear(); IVMBeta::VmReset(); }
	int GetStatus() { return VmStatus; }
};

#endif
