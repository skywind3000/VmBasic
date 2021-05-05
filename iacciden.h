//======================================================================
//
// Token Parser and Expression Parser
// �򵥴ʷ���������ͨ�ñ��ʽ������
//
// �����ߣ���ΰ
// �������ڣ�4/4/2002
//
// �ṩ�������κνű����ԵĴʷ�����������ͳ���ı��ʽ������
// �ʷ�������������IAccToken���������������ȡһ����־�����Ұ���
// ENDF, ENDL, KEYWORD, IDENT, INTEGER, FLOAT, OPERATOR, STRING,
// IERROR�������ಢû��ʵ�ֶ���������������ȥ����ʵ�֣���������
// ʵ����һ��Basic���ԵĴʷ������� IBasicToken���ڴ�����������ȡ
// Basicʶ�������䡣
// ���ʽ���������ṩ��׺ʽת����Ϊ��׺ʽ�Ĺ��ܣ�ʵ�ʲ�û��ʵ��
// �������ݾ�����;��ջ�ķ�����ʵ�ֺ�׺ʽ����ֵ������ʽ��������
// ������ķ��ͺ�׺ʽջʽ��ֵ�ķ����󲿷ֶ����Դ������ҵ��������
// ��Ͳ��ٷ������ˣ����ʽ������֧�������жϵġ�
// ʹ�ñ��ʽ������Ҫ��������һϵ����0�������������ݵ�����С��256
// ʱ��������һ��ASCII���ʾ���������������'+'��43��'*'��42��
// ����һЩ������߼����� '>' '<' ��']'(���ڵ���)'['(С�ڵ���)��
// �� ISExpress���ע�ͣ�����������ݴ��ڵ���256����������һ������
// ��Ԫ�أ����Կ�����һϵ�д��ڵ���256������������ʵ�ʱ��ʽ�е���
// �ݶ�ֱ����Ascii���ʾ���ʽ�е������������˵��
//   int exp[]={ 0x100,'+',0x101,'/',0x102,0 } ��Ϊ����ʱ���Եõ�
//   ����ĺ�׺ʽ { 0x100, 0x101, 0x102, '/', '+', 0 }Ȼ��Ϳ��Լ�
// ��������һ��ջ�������׺ʽ��ֵ������ֵ���ɴ�����
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

class IAccToken		// ��׼�ʷ�������(��ʽ)
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

class IIdentFinder	// С��Cʽ����ʷ�������
{
public:
	struct IIdentLink	// �������
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

class IBasicToken:public IAccToken	// Basicʽ�ʷ�������
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

class IStringIdent					// �ַ�����ű�
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


class ISExpress		// ���ʽ������
{
protected:			// [С�ڵ��� ]���ڵ��� !��  =���� |�� &��
	struct CNode	// ^��ȡ������ # ������  @ ��ֵ
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

