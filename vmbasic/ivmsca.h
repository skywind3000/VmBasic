//======================================================================
//
// IVirtual Machine Asm Loader
// ���������ȡ���򣬽���ȡ������Ļ�ಢ�����������������ֽ���
// �ǵ����������ֱ�Ӷ�ȡ����������Щ�ֽ��롣
//
// �����ߣ���ΰ
// �������ڣ�4/4/2002
//
// �������(Istream)�ж�ȡ�����򣬹������Ķ����Istream��Ixstream
// �����õ���iaccident.h��һ���ʷ�/�﷨�������߰����������������
// IIdentFinder�࣬һ���򵥵Ĵʷ��������߻������ܾ��ǲ�ֵ��ʣ��ִ�
// ����ʶ��ؼ��֣����ŵȣ���ȡ���������Ǽ��򵥵Ĺ�����˲�������
// �����������ṩ��̬���Ӷ�ȡ�Ĺ��ܡ�����
//
//======================================================================

#ifndef __I_VMSCA_H__
#define __I_VMSCA_H__

#include "ivmsbeta.h"	// �����Beta���ඨ��
#include "iacciden.h"	// �ʷ�/�﷨�������ù��߶���
#include "ifstream.h"	// �������ļ������ڶ�ȡ
#include "ivector.h"

class IVMBetaAsm
{
protected:
	IIdentFinder IdentReader;	// �ʷ�������
	IStringIdent LabelIdent;	// ������ַ��¼��
	IStringIdent UseIdent;		// ʹ�õ�ַ��¼��
	IVector<vbyte> Code;		// �������ɼ�¼��
	int  StackSize;				// ջ�Ĵ�С
	char *KeyWords[20];
	enum SymbolType { SymReg=0,SymRegPoint=1,SymImm=2,SymImmPoint=3 };
	enum DataType   { DWord=0,Word=1,Byte=2,Float=3,Integer=4 };
	vdword SymType;
	vdword SymData;
	char   ErrorMsg[1024];
	int    SymError;
	int    SymPreType;
	char   SymIdent[1024];
	void   SetDWord(int ip,vdword value);
	int    GetSymbol(int IdentP);
	int    MakeOneSymbol(int ip,int IdentP);
	int    MakeTwoSymbol(int ip,int IdentP);
public:
	IVMBetaAsm();
	int LoadAsm(IVMBeta &NewVM,Istream &stream);
	int LoadAsm(IVMBeta &NewVM,char *filename);
	char *ErrorCode() { return ErrorMsg; }
};

class IVMBetaDebug:public IVMBeta
{
public:
	int Debug();
};

#endif

