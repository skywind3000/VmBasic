//======================================================================
//
// The declaration of IVirtual Machine Beta
// 通过继承IVMInstance实现了虚拟机IVMBeta(虚拟机Beta版)
//
// 创建者：林伟
// 创建日期：4/4/2002
//
// 它有完整的指令集和命令解释/运行系统，用来运行虚拟机代码
// 指令采用精简指令，指令说明：JMP(无条件跳转)JPC(条件跳转)
// CALL(调用)RET(返回)LD(数据装载)CAL(数据运算)CMP(数据比较)
// PUSH(压栈)POP(弹栈)NOOP(空指令)IN(输入指令)OUT(输出指令)
// EXIT(推出指令) 等13条指令。支持的数据类型有DWORD/WORD/
// SHORT/BYTE/FLOAT等五种实现了一个栈式CPU的常用功能
//
// 寄存器有八个: RP(指令地址寄存器) RF(标志寄存器) RS(栈寄
// 存器) RB(辅助栈寄存器) R0-R3(通用寄存器)
//
//======================================================================

#ifndef __I_VMSBETA_H__
#define __I_VMSBETA_H__

#include "ivms.h"
#include "istring.h"
#include "itemplat.h"

class IVMBeta:public IVMInstance
{
protected:
	IVMREGS VmStartup;
	int VmStatus;
	ILister<iString> *VmStrings;
	char *VmIOString(int id);
public:
	IVMBeta();
	virtual ~IVMBeta();
	virtual void VmReset();
	virtual int  VmCreate(int size,vbyte *data,vdword *regs);
	virtual void VmRelease();
	virtual void Process();
	virtual void   VmError(vdword error);
	virtual void   VmOut(vdword port,vdword value);
	virtual vdword VmIn(vdword port);
	virtual void Excute();
};

#endif

