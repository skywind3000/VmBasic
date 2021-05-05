//======================================================================
//
// The declaration of IVirtual Machine Beta
// ͨ���̳�IVMInstanceʵ���������IVMBeta(�����Beta��)
//
// �����ߣ���ΰ
// �������ڣ�4/4/2002
//
// ����������ָ����������/����ϵͳ�������������������
// ָ����þ���ָ�ָ��˵����JMP(��������ת)JPC(������ת)
// CALL(����)RET(����)LD(����װ��)CAL(��������)CMP(���ݱȽ�)
// PUSH(ѹջ)POP(��ջ)NOOP(��ָ��)IN(����ָ��)OUT(���ָ��)
// EXIT(�Ƴ�ָ��) ��13��ָ�֧�ֵ�����������DWORD/WORD/
// SHORT/BYTE/FLOAT������ʵ����һ��ջʽCPU�ĳ��ù���
//
// �Ĵ����а˸�: RP(ָ���ַ�Ĵ���) RF(��־�Ĵ���) RS(ջ��
// ����) RB(����ջ�Ĵ���) R0-R3(ͨ�üĴ���)
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

