//======================================================================
//
// IVirtual Machine Asm Loader
// 虚拟机汇编读取程序，将读取虚拟机的汇编并把它翻译成虚拟机的字节码
// 是的虚拟机可以直接读取并且运行这些字节码。
//
// 创建者：林伟
// 创建日期：4/4/2002
//
// 程序从流(Istream)中读取汇编程序，关于流的定义见Istream和Ixstream
// 程序用到了iaccident.h是一个词法/语法分析工具包，这里用了里面的
// IIdentFinder类，一个简单的词法分析工具基本功能就是拆分单词，字串
// 还有识别关键字，符号等，读取虚拟机汇编是件简单的工作因此不再叙述
// 打算在这里提供动态连接读取的功能。。。
//
//======================================================================

#ifndef __I_VMSCA_H__
#define __I_VMSCA_H__

#include "ivmsbeta.h"	// 虚拟机Beta的类定义
#include "iacciden.h"	// 词法/语法分析常用工具定义
#include "ifstream.h"	// 流定义文件，用于读取
#include "ivector.h"

class IVMBetaAsm
{
protected:
	IIdentFinder IdentReader;	// 词法分析器
	IStringIdent LabelIdent;	// 申明地址记录器
	IStringIdent UseIdent;		// 使用地址记录器
	IVector<vbyte> Code;		// 代码生成记录器
	int  StackSize;				// 栈的大小
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

