//======================================================================
//
// IVirtual Machine Basic Script Compiler
// ������ű�Basic�ű�������򣺽��������VMBasic���Խ��ͳ�����
// �����(.BAS->.VMS)ʹ��ͨ��ivmsca.h����Ķ�ȡ�������ֱ�Ӷ�ȡ
//
// �����ߣ���ΰ
// �������ڣ�4/14/2002
//
// �����Ǵ����ж�ȡ�ģ����Ķ����istream.h��ifstream.h������
// �����﷨������ʱ�������õ��Ǵʷ���������iacciden.h�������
// IAccIdent�̳�����������IBasicIdentר�����ڷ���Basic����ķ���
// ������IAccIdent��һ���ʷ��������Ļ��࣬�Ǹ������ࡣ
// ��������������յ��ķ����£�
//     <Script>::=�����������(����Make...��������)
//     <Line>::=<Script>|<Script>:<Line>   ����һ�����Ķ���
//     <Statement>::=һϵ�е�<Line>���ɵ����ģ����END IF��Ϊ����
//
// ��������ģ��Ĺ��̶����� ParseXXX()�ɵݹ��½��ķ������������
// ��ParseProgram()������ý���һ�д���Ŀ飺ParseLine()�����ٵ�
// ����ؼ���ģ��ParseScript()��������һ��Basicָ������б�Ҫ��
// ��������������ģ��(�������IF/WHILE�����ʱ)
// ���ʽ�����õ���iacciden.h���涨�������ISExpress�������Խ���
// ׺ʽת���ɺ�׺ʽ��֧�����������ģ�����ivmscexp.cpp����ʵ������
// ������ı��ʽ���֣�����ؼ��Ĳ���֮һ����ž���ͨ�����ñ��ʽ
// ������������һ��ģ��ʵ��ִ�е�ջEStack�����ɴ��룬�������ת��
//
//======================================================================

#ifndef __I_VMSCC_H__
#define __I_VMSCC_H__

#include "ivmsbeta.h"	// �����Beta���ඨ��
#include "iacciden.h"	// �ʷ�/�﷨�������ù��߶���
#include "ifstream.h"	// �������ļ������ڶ�ȡ


class IVMBetaVariable	// �ű������࣬��������ȫ�ֻ�ֲ�������ı���������
{
protected:
	struct Variable
	{
		iString name;
		int mode;		// ģʽ��0:��������  >0:��������
		int type;		// ���ͣ�INT/REAL/STRING
		int size;		// ����Ĵ�С
		int spos;		// ��ջ�����sp��ƫ��
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
	int pushv(int index,Ixstream &output,long &LABEL_ID);	// ��������ջ����ļ�������
	int value(int index,int type,Ixstream &output,long &LABEL_ID);	// ��R3��ֵ
	int getaddr(int index,Ixstream &output,long &LABEL_ID);			// �ɱ�����ַװ��R0
	char *GetName(int index) { return vars[index]->name.GetString(); }
	int GetMode(int index) { return vars[index]->mode; }
	int GetType(int index) { return vars[index]->type; }
	int GetPos(int index)  { return vars[index]->spos; }
	int GetSize(int index) { return vars[index]->size; }
	int GetCount() { return count; }
	char *varname(char *name,int type);
	void FreeSpace(Ixstream &output);	// �ͷ��ַ���
};

class IVMBetaScript		// VmBasic�ű��������࣬������������VmBasicԴ����
{
protected:				// ���protected�µ�Ϊ��������
	class Func
	{
	protected:
		int comp;
	public:
		Func() { comp=0; }
		int CompIt() { comp=1; return 0; }
		int Comped() { return comp; }
		iString name;	// ��������
		int type;		// INT/REAL/STRING
		IVMBetaVariable var;	// �����õ��ı����Լ�����
		int parcount;			// ��������
		IVector<char> parlist;	// ��������
		Imstream code;				// ����������
	};
	IVMBetaVariable global;			// ȫ�ֱ�����
	ILister<Func> function;			// ���к���
	Func *FunPoint;					// ��ǰ���ں�����������ں���������ΪNULL
	int	FunCount;					// ��������
	long LABEL_ID;
	char *FunName(char *name,int type);
	char *VarName(char *name,int type);
	int FunPos(char *name);
	int FunDeclare(char *name,int type,int *par);
protected:
	/* ����������� */
	Ifstream TagOut;		// �������
	Ifstream Input;			// ����
	Ifstream Output;		// ���
	/* �ʷ��������� */
	char *KeyWords[40];
	enum { gotosym=0,ifsym=1,thensym=2,elsesym=3,dimsym=4,	// �ؼ��ֵ�ֵ
		   modsym=5,notsym=6,andsym=7,orsym=8,endsym=9,vasmsym=10,
		   printsym=11,inputsym=12,sharesym=13,subsym=14,funsym=15,
		   declaresym=16,callsym=17,retsym=18,whilesym=19,wendsym=20,
		   gosubsym=21,inportsym=22,outpsym=23,cflinesym=24,ldptrsym=25,
		   };
	IBasicToken Token;

	/* ��ʶ���� */
	IStringIdent reference;	// GOTO����ù��ĵ�ַ�����ڼ���Ƿ��ַû�б�����
	IStringIdent labels;	// ��ת��ַ
	IStringIdent strings;	// ��������ĳ����ַ���

	int Error;
	int ParseExpression();	// ���ɷ������ʽ�Ĵ���
	int ParseScript();		// ����ָ��
	int ParseLine();		// ����ָ��
	int ParseStatement();	// һ��ָ��
	int ParseProgram();		// һ���ű�����

	int ExportFun(int i);
	int MakeLabel(char *label);		/* �����ʶ����	*/
	int MakeGoto(char callf=0);		/* ������ת����	*/
	int MakeVAsm();					/* ������Ǧ���	*/
	int MakePrint();				/* �����ӡ		*/
	int MakeDim();					/* ���岿��		*/
	int MakeIf();					/* �����жϲ��� */
	int MakeValue();				/* ��ֵ������ */
	int MakeDeclare();				/* �������岿�� */
	int MakeFunc();					/* ����ʵ�ֲ��� */
	int MakeWhile();				/* ѭ��ģ��ʵ�� */
	int MakeInput();				/* ���봦��     */
	int MakeLoadptr();

	int MakeOperate(int opr,int type);	// ʹR0,R1����Opr������(+-*/)
	int ReadType();
	int VarInfo(char *name,int type,IVMBetaVariable* &set,int &index,int &mode);
	int VarAddr();		// ��������ַ����R0�У����ر�������
protected:				// ���protected�������������Դ�������
	struct EStack {
		long type;		// ���ͼ�����enum����Ķ���
		long v;			// ������������������ΪASCII��
		char *name;		// �ַ���ָ���������
		char mode;      // ģʽ0����1����2����
		char set;		// 0Ϊȫ�ֱ�����1Ϊ�ֲ�����
		long behind;    // ������ջ�ж��ٸ��� 
	};
	enum {	ES_OPERATOR=0,ES_IMMINT=1,ES_IMMFLOAT=2,ES_CSTRING=3,	// ջ������
			ES_VARINT=0x10,ES_VARFLOAT=0x11,ES_VARSTRING=0x12,		// ȫ�ֱ���
			ES_PTRINT=0x20,ES_PTRFLOAT=0x21,ES_PTRSTRING=0x22 };	// ָ�����
	char *ConstStrName(int id);
	int IntExpress(IVector<EStack> &Estack,int &stacktop);	// �����������ʽ��׺ʽ
	int FlushStack(IVector<EStack> &Estack,int stacktop);	// ���ݺ�׺ʽ���ɴ���
public:
	IVMBetaScript();
	int GoScript(Istream &input,Istream &output);
};


#endif
