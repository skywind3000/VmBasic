//======================================================================
//
// IVirtual Machine Basic Script Compiler
// 虚拟机脚本Basic脚本编译程序：将虚拟机的VMBasic语言解释成虚拟
// 机汇编(.BAS->.VMS)使得通过ivmsca.h里面的读取程序可以直接读取
//
// 创建者：林伟
// 创建日期：4/14/2002
//
// 程序都是从流中读取的，流的定义见istream.h和ifstream.h两个，
// 进行语法分析的时候程序调用的是词法分析器：iacciden.h里面的类
// IAccIdent继承下来的子类IBasicIdent专门用于分析Basic代码的分析
// 器，而IAccIdent是一个词法分析器的基类，是个纯虚类。
// 整个编译程序依照的文法如下：
//     <Script>::=单条语句描述(调用Make...函数处理)
//     <Line>::=<Script>|<Script>:<Line>   处理一行语句的定义
//     <Statement>::=一系列的<Line>构成的语句模块以END IF等为结束
//
// 处理所有模块的过程都叫做 ParseXXX()由递归下降的分析方法程序块
// 是ParseProgram()下面调用解释一行代码的块：ParseLine()其下再调
// 用最关键的模块ParseScript()它将解释一条Basic指令如果有必要它
// 会调用上面的两个模块(比如编译IF/WHILE等语句时)
// 表达式分析用的是iacciden.h里面定义过的类ISExpress，它可以将中
// 缀式转化成后缀式，支持条件分析的，程序ivmscexp.cpp里面实现了整
// 个程序的表达式部分，是最关键的部分之一，大概就是通过调用表达式
// 分析器和利用一个模拟实际执行的栈EStack来生成代码，完成类型转化
//
//======================================================================

#ifndef __I_VMSCC_H__
#define __I_VMSCC_H__

#include "ivmsbeta.h"	// 虚拟机Beta的类定义
#include "iacciden.h"	// 词法/语法分析常用工具定义
#include "ifstream.h"	// 流定义文件，用于读取


class IVMBetaVariable	// 脚本变量类，用来描述全局或局部变量里的变量和数组
{
protected:
	struct Variable
	{
		iString name;
		int mode;		// 模式：0:单个变量  >0:变量数组
		int type;		// 类型：INT/REAL/STRING
		int size;		// 数组的大小
		int spos;		// 在栈中相对sp的偏移
		IVector<int> array;
	};
	ILister<Variable> vars;
	int count,InStack;
public:
	int StackP;
	IVMBetaVariable(): count(0),InStack(0) {}
	void Clear() { vars.Clear(); count=0; InStack=0; StackP=0; }
	void SetMode(int isstack) { InStack=isstack; }
	int regvar(char *name,int type,int *array,int off=0);
	int pos(char *name);
	int pushv(int index,Ixstream &output,long &LABEL_ID);	// 返回用了栈里面的几个数据
	int value(int index,int type,Ixstream &output,long &LABEL_ID);	// 从R3赋值
	int getaddr(int index,Ixstream &output,long &LABEL_ID);			// 吧变量地址装入R0
	char *GetName(int index) { return vars[index]->name.GetString(); }
	int GetMode(int index) { return vars[index]->mode; }
	int GetType(int index) { return vars[index]->type; }
	int GetPos(int index)  { return vars[index]->spos; }
	int GetSize(int index) { return vars[index]->size; }
	int GetCount() { return count; }
	char *varname(char *name,int type);
	void FreeSpace(Ixstream &output);	// 释放字符串
};

class IVMBetaScript		// VmBasic脚本编译器类，从输入流编译VmBasic源程序
{
protected:				// 这个protected下的为函数定义
	class Func
	{
	protected:
		int comp;
	public:
		Func() { comp=0; }
		int CompIt() { comp=1; return 0; }
		int Comped() { return comp; }
		iString name;	// 函数名字
		int type;		// INT/REAL/STRING
		IVMBetaVariable var;	// 函数用到的变量以及参数
		int parcount;			// 参数个数
		IVector<char> parlist;	// 参数种类
		Imstream code;				// 代码生成区
	};
	IVMBetaVariable global;			// 全局变量区
	ILister<Func> function;			// 所有函数
	Func *FunPoint;					// 当前所在函数如果不是在函数里面则为NULL
	int	FunCount;					// 函数个数
	long LABEL_ID;
	char *FunName(char *name,int type);
	char *VarName(char *name,int type);
	int FunPos(char *name);
	int FunDeclare(char *name,int type,int *par);
protected:
	/* 输入输出部分 */
	Ifstream TagOut;		// 最终输出
	Ifstream Input;			// 输入
	Ifstream Output;		// 输出
	/* 词法分析部分 */
	char *KeyWords[40];
	enum { gotosym=0,ifsym=1,thensym=2,elsesym=3,dimsym=4,	// 关键字的值
		   modsym=5,notsym=6,andsym=7,orsym=8,endsym=9,vasmsym=10,
		   printsym=11,inputsym=12,sharesym=13,subsym=14,funsym=15,
		   declaresym=16,callsym=17,retsym=18,whilesym=19,wendsym=20,
		   gosubsym=21,inportsym=22,outpsym=23,cflinesym=24,ldptrsym=25,
		   };
	IBasicToken Token;

	/* 标识部分 */
	IStringIdent reference;	// GOTO语句用过的地址，用于检测是否地址没有被定义
	IStringIdent labels;	// 跳转地址
	IStringIdent strings;	// 程序里面的常量字符串

	int Error;
	int ParseExpression();	// 生成分析表达式的代码
	int ParseScript();		// 单条指令
	int ParseLine();		// 单行指令
	int ParseStatement();	// 一段指令
	int ParseProgram();		// 一个脚本程序

	int ExportFun(int i);
	int MakeLabel(char *label);		/* 处理标识部分	*/
	int MakeGoto(char callf=0);		/* 处理跳转部分	*/
	int MakeVAsm();					/* 处理内铅汇编	*/
	int MakePrint();				/* 处理打印		*/
	int MakeDim();					/* 定义部分		*/
	int MakeIf();					/* 条件判断部分 */
	int MakeValue();				/* 赋值或函数块 */
	int MakeDeclare();				/* 函数定义部分 */
	int MakeFunc();					/* 函数实现部分 */
	int MakeWhile();				/* 循环模块实现 */
	int MakeInput();				/* 输入处理     */
	int MakeLoadptr();

	int MakeOperate(int opr,int type);	// 使R0,R1按照Opr来运算(+-*/)
	int ReadType();
	int VarInfo(char *name,int type,IVMBetaVariable* &set,int &index,int &mode);
	int VarAddr();		// 将变量地址放入R0中，返回变量类型
protected:				// 这个protected里面的用作表达试代码生成
	struct EStack {
		long type;		// 类型见下面enum里面的定义
		long v;			// 如果类型是运算符，则为ASCII码
		char *name;		// 字符串指向变量本身
		char mode;      // 模式0变量1数组2函数
		char set;		// 0为全局变量，1为局部变量
		long behind;    // 后续的栈有多少个？ 
	};
	enum {	ES_OPERATOR=0,ES_IMMINT=1,ES_IMMFLOAT=2,ES_CSTRING=3,	// 栈的类型
			ES_VARINT=0x10,ES_VARFLOAT=0x11,ES_VARSTRING=0x12,		// 全局变量
			ES_PTRINT=0x20,ES_PTRFLOAT=0x21,ES_PTRSTRING=0x22 };	// 指针变量
	char *ConstStrName(int id);
	int IntExpress(IVector<EStack> &Estack,int &stacktop);	// 分析产生表达式后缀式
	int FlushStack(IVector<EStack> &Estack,int stacktop);	// 根据后缀式生成代码
public:
	IVMBetaScript();
	int GoScript(Istream &input,Istream &output);
};


#endif
