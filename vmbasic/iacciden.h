//======================================================================
//
// Token Parser and Expression Parser
// 简单词法分析器和通用表达式分析器
//
// 创建者：林伟
// 创建日期：4/4/2002
//
// 提供独立于任何脚本语言的词法分析器基类和抽象的表达式分析器
// 词法分析器基类是IAccToken负责从输入流中提取一个标志，并且按照
// ENDF, ENDL, KEYWORD, IDENT, INTEGER, FLOAT, OPERATOR, STRING,
// IERROR做出分类并没有实现而是由他的派生类去具体实现，比如这里
// 实现了一个Basic语言的词法分析器 IBasicToken用于从输入流中提取
// Basic识别符和语句。
// 表达式分析器是提供中缀式转化成为后缀式的功能，实际并没有实现
// 而是依据具体用途用栈的方法来实现后缀式的求值，或表达式代码生成
// 具体的文法和后缀式栈式求值的方法大部分都可以从书中找到，因此这
// 里就不再费周折了，表达式分析器支持条件判断的。
// 使用表达式分析器要向他输入一系列以0结束的整型数据当数据小于256
// 时被看作是一个ASCII码表示的运算符。比如表达'+'用43，'*'用42等
// 还有一些特殊的逻辑符号 '>' '<' 和']'(大于等于)'['(小于等于)等
// 见 ISExpress类的注释，如果输入数据大于等于256将被看作是一个运算
// 的元素，所以可以用一系列大于等于256的数字来代表实际表达式中的数
// 据而直接用Ascii码表示表达式中的运算符。比如说：
//   int exp[]={ 0x100,'+',0x101,'/',0x102,0 } 作为输入时可以得到
//   输出的后缀式 { 0x100, 0x101, 0x102, '/', '+', 0 }然后就可以简
// 单的利用一个栈来求出后缀式的值或者求值生成代码了
// 
//======================================================================


#ifndef __I_ACCIDENCE_H__
#define __I_ACCIDENCE_H__

#ifndef __cplusplus
#error  This file can only be compiled with a C++ compiler !!
#endif

#include "istring.h"
#include "ivector.h"
#include "itemplat.h"
#include "ifstream.h"

class IAccToken		// 标准词法分析器(流式)
{
public:
	int  Type;
	long Value;
	IVector<char> Text;
public:
	enum {	ENDF=0,ENDL=1,KEYWORD=2,IDENT=3,INTEGER=4,FLOAT=5,
			OPERATOR=6,STRING=7,IERROR=8 };
	virtual int Read(Istream *in)=0;
};

class IIdentFinder	// 小型C式链表词法分析器
{
public:
	struct IIdentLink	// 分析结果
	{
		char   Type;
		int    Value;
		long   iValue;
		float  fValue;
		char  *sValue;
		struct IIdentLink *Next;
	};
protected:
	IIdentLink *root;
	IVector<IIdentLink> list;
	int Count;
	int Clear();
public:
	enum IdentMode { KeyWord=0,Ident=1,Integer=2,Float=3,String=4,Operator=5 };
	IIdentFinder();
	~IIdentFinder();
	virtual int FindIdent(char *s,char **key_words,int HighCase=0);
	IIdentFinder::IIdentLink* operator[](int i);
	int GetCount();
};

class IBasicToken:public IAccToken	// Basic式词法分析器
{
protected:
	Istream *is;
	char **KeyWord;
	int ch;
	int getnc();
public:
	int Line;
	IBasicToken() { is=NULL; KeyWord=0; }
	void Start(Istream &input,char **keyword) { is=&input; ch=0; KeyWord=keyword; Line=1; }
	int Read(Istream *in=NULL);
};

class IStringIdent					// 字符串编号表
{
protected:
	struct StringIdent
	{
		char *string;
		int id;
	};
	IVector<StringIdent> Strings;
	int Count;
public:
	int Error;
	IStringIdent();
	~IStringIdent();
	void Clear();
	void Add(char *string,int id);
	char *String(int index);
	char *Id2Str(int id);
	int  Id(int index);
	int  Id(char *string);
	int  Index(int id);
	int  Index(char *string);
	int  GetCount();
};


class ISExpress		// 表达式分析器
{
protected:			// [小于等于 ]大于等于 !否  =等于 |或 &且
	struct CNode	// ^是取负操作 # 不等于  @ 赋值
	{
		int value;
		int left,right;
	};
	ILister<CNode> node;
	IVector<int> *output;
	int *input;
	int ch,p,op;
	int getnc();
	int make_node(int value,int left,int right);
	int error;
	int I();
	int E() { return R(T()); }
	int R(int in);
	int T() { return P(F()); }
	int P(int in);
	int F();
	int BE() { return BR(BT()); }
	int BR(int in);
	int BT() { return BP(E()); }
	int BP(int in);
	int EXP() { return (AN_BOOL)?BE():E(); }
	int AN_BOOL;
	void walk_tree(int node);
	void Clear() { node.Clear(); error=0; node.Alloc(); }
public:
	ISExpress() { Clear(); }
	int express(int *in,IVector<int> &t,int anbool=0);
};

#endif

