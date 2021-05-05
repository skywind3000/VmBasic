//======================================================================
//
// Declaration of IVirtual Machine System
// ����������ļ���������������ļĴ���IVMREGS�������
// ��IVMInstance��ֻ�����˲�û��ʵ�֡�
//
// �����ߣ���ΰ
// �������ڣ�4/4/2002
//
//======================================================================

#ifndef _I_VMS_H__
#define _I_VMS_H__

#ifndef __cplusplus
#error  This file can only be compiled with a C++ compiler !!
#endif

typedef unsigned long vdword;
typedef unsigned short vword;
typedef unsigned char vbyte;
typedef float vfloat;
typedef long vint;


struct IVMREGS			// Virtual Machine Registers
{
	vdword  RP,RF,RS,RB;
	vdword  R0,R1,R2,R3; 
};

class IVMInstance
{
protected:
	vbyte   *VirtualMem;		// Virtual Memory of the Virtual Machine
	vdword   VmSize;			// Size of Virtual Memory
	IVMREGS  VmRegs;			// Registers of the Virtual Machine
public:
	virtual vdword VmIn(vdword port)=0;
	virtual void   VmOut(vdword port,vdword value)=0;
	virtual void   VmError(vdword error)=0;
	virtual void   Process()=0;
};

#endif

