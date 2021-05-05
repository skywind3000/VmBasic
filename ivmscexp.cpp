#include "ivmscc.h"


int IVMBetaScript::ParseExpression()
{
	IVector<EStack> Estack;
	int stacktop=0;
	Estack.SetLength(0);
	Estack.AutoSize(1);
	Output<<";<Epression>\n";
	if (!IntExpress(Estack,stacktop)) {
		iserr.VarArg("Error Find in Expression: %d\n",Error);
		return 0;
	}
	int r=FlushStack(Estack,stacktop);
	Output<<";</Epression>\n";
	return r;
}

struct IdentLink{
		long Type;	// ����
		long Value;	// ֵ
		long vType; // �����������������
		long sStart;
		long sCount;
		long pCount;	// ��������
		long VarSet;	// 0: global 1: local
		iString Text;
};

int IVMBetaScript::IntExpress(IVector<EStack> &Estack,int &stacktop)
{
	IVector<int> behind;		// �����׺���ʽ
	IVector<int> input;		// �ṩ����Է����������������
	IVector<EStack> MyStack;	// �����м����ջ

	enum { ID_OPERATOR=0,ID_INTEGER=1,ID_FLOAT=2,ID_STRING=3,ID_VAR=4,
		   ID_ARRAY=5,ID_FUN=6 };

	ILister<IdentLink> Mident;
	int mp,i=0,MyPoint=0;

	input.SetLength(0);
	input.AutoSize(1);
	behind.SetLength(0);
	behind.AutoSize(1);
	MyStack.SetLength(0);
	MyStack.AutoSize(1);

	int paren=0;
	for (;Token.Type!=IAccToken::ENDL&&Token.Type!=IAccToken::ENDF;) {
		if (Token.Type==IAccToken::KEYWORD) {	// �����ȡģ���߼�����
			if (Token.Value==modsym) { input[i++]='%'; Token.Read(); } else
			if (Token.Value==notsym) { input[i++]='!'; Token.Read(); } else
			if (Token.Value==andsym) { input[i++]='&'; Token.Read(); } else
			if (Token.Value==orsym)  { input[i++]='|'; Token.Read(); } else break;
			mp=Mident.Alloc();
			Mident[mp]->Type=ID_OPERATOR;
			Mident[mp]->Value=input[i-1];
		}	else
		if (Token.Type==IAccToken::INTEGER) {		// ���������
			mp=Mident.Alloc();
			Mident[mp]->Type=ID_INTEGER;
			Mident[mp]->Value=Token.Value;
			input[i++]=0x100+mp;
			Token.Read();
		}	else 
		if (Token.Type==IAccToken::FLOAT) {
			mp=Mident.Alloc();
			Mident[mp]->Type=ID_FLOAT;
			Mident[mp]->Value=Token.Value;
			input[i++]=0x100+mp; 
			Token.Read();
		}	else
		if (Token.Type==IAccToken::STRING) {
			mp=Mident.Alloc();
			Mident[mp]->Type=ID_STRING;
			Mident[mp]->Text=&Token.Text[0];
			input[i++]=0x100+mp; 
			Token.Read();
		}	else
		if (Token.Type==IAccToken::IDENT) {	// ����Ǳ���
			mp=Mident.Alloc();
			Mident[mp]->Type=ID_VAR;
			Mident[mp]->Text=&Token.Text[0];
			input[i++]=0x100+mp;
			Token.Read();
			Mident[mp]->vType=ReadType();
			int p,mode=0,x,y;
			int j,type=Mident[mp]->vType;
			iString oname,vname,fname;
			oname=Mident[mp]->Text;
			vname=VarName(oname.GetString(),type);
			fname=FunName(oname.GetString(),type);
			for (x=0,y=-1;x<FunCount;x++) if (fname==function[x]->name) y=x;
			if (y>=0) {			// ����Ǻ���
				Mident[mp]->Type=ID_FUN;
				Mident[mp]->Value=y;
				Mident[mp]->sStart=MyPoint;
				mode=function[y]->parcount;
				if (Token.Type==IAccToken::OPERATOR&&Token.Value=='(') Token.Read();
				else { iserr<<"Not find (\n"; return 0; }
				for (j=0;j<mode;j++) {
					if (!IntExpress(MyStack,MyPoint)) return 0;
					if (Token.Type==IAccToken::OPERATOR) {
						if (Token.Value==',') Token.Read(); else
						if (Token.Value!=')') {
							iserr<<"expression error\n";
							Error=30;
							return 0;
						}
					}	else {
						iserr<<"expression error\n";
						Error=31;
						return 0;
					}
				}
				if (Token.Type==IAccToken::OPERATOR&&Token.Value==')') Token.Read();
				else { iserr<<"Not find )\n"; Error=32; return 0; }
				Mident[mp]->pCount=mode;
				Mident[mp]->sCount=MyPoint-Mident[mp]->sStart;
			}	else {			// ������Ǻ������Ǳ���
				p=global.pos(vname.GetString());
				IVMBetaVariable *set=&global;
				if (p>=0) mode=set->GetMode(p); else {
					if (FunPoint) {
						set=&FunPoint->var;
						p=set->pos(vname.GetString());
						if (p>=0) { p=set->pos(vname.GetString()); mode=set->GetMode(p); }
						else if ((p=set->regvar(vname.GetString(),type,NULL))<0) return 0;
					}	else if ((p=global.regvar(vname.GetString(),type,NULL))<0) return 0;
				}
				Mident[mp]->VarSet=(set==&global)?0:1;
				Mident[mp]->Value=p;
				if (Mident[mp]->Value<0) {
					iserr<<"pos value error\n";
					Error=33;
					return 0;
				}
				if (mode>0) {
					Mident[mp]->Type=ID_ARRAY;
					Mident[mp]->sStart=MyPoint;
					if (Token.Type==IAccToken::OPERATOR&&Token.Value=='(') Token.Read();
					else {
						iserr<<"expression error\n";
						Error=34;
						return 0;
					}
					for (j=0;j<mode;j++) {
						if (!IntExpress(MyStack,MyPoint)) { Error=50; return 0; }
						if (Token.Type==IAccToken::OPERATOR) {
							if (Token.Value==',') Token.Read(); else
							if (Token.Value!=')') {
								iserr<<"expression error\n";
								Error=35;
								return 0;
							}
						}	else {
							iserr<<"expression error\n";
							Error=36;
							return 0;
						}
					}
					if (Token.Type==IAccToken::OPERATOR&&Token.Value==')') Token.Read();
					else { iserr<<"not find )\n"; Error=37; return 0; }
					Mident[mp]->pCount=mode;
					Mident[mp]->sCount=MyPoint-Mident[mp]->sStart;
				}	else if (Token.Type==IAccToken::OPERATOR&&Token.Value=='(') {
					iserr<<"array "<<vname.GetString()<<" not declared\n";
				}
			}	// end if var and array
		}	else
		if (Token.Type==IAccToken::OPERATOR) {	// ������
			int v=Token.Value;
			if (v=='+'||v=='-'||v=='*'||v=='/'||v=='('||v==')'||v=='=') {
				if (v=='(') paren++;
				if (v==')') paren--;
				if (paren<0) break;
				Token.Read(); 
				input[i++]=v; 
			} else
			if (v=='<') { 
				Token.Read();
				if (Token.Type!=IAccToken::OPERATOR) input[i++]='<';
				else {
					v=Token.Value;
					if (v=='>') { Token.Read(); input[i++]='#';	 } else	// ������
					if (v=='=') { Token.Read(); input[i++]='[';	 } else	// С�ڵ���
					input[i++]='<';
				}
			}	else
			if (v=='>') {
				Token.Read();
				if (Token.Type!=IAccToken::OPERATOR) input[i++]='>';
				else {
					if (Token.Value=='=') { Token.Read(); input[i++]=']'; }
					else input[i++]='>';
				}
			}	else break;
		}	else break;
	}
	input[i++]=0;
	ISExpress exp;							// ���ʽ������
	if (exp.express(&input[0],behind,1)) {	// ת���ɺ�׺���ʽ
		Error=15;
		return 0;
	}
	/****************************
	   ��������Ľ�����ɼ���ջ
	 */
	mp=Mident.Alloc();
	for (i=0;i<behind.GetLength();i++) {	// Ϊ���������������ջ
		int p=behind[i];
		if (p>=0x100) {
			/*********************
			  ��������������
			 */
			p-=0x100;	// ȡ���ڵ��ʱ������λ��
			if (Mident[p]->Type==ID_INTEGER) {	// ���������
				Estack[stacktop].type=ES_IMMINT;
				Estack[stacktop++].v=Mident[p]->Value;
			}	else
			if (Mident[p]->Type==ID_FLOAT) {		// ����Ǹ�����
				Estack[stacktop].type=ES_IMMFLOAT;
				Estack[stacktop++].v=Mident[p]->Value;
			}	else
			if (Mident[p]->Type==ID_STRING) {		// ������ַ���
				iString s; int sid;
				iString statics;
				s=Mident[p]->Text;
				statics.VarArg("CSTRING_%d",LABEL_ID++);
				global.regvar(statics.GetString(),ES_CSTRING,NULL);
				sid=global.GetCount()-1;
				strings.Add(s.GetString(),sid);				// �����ִ����б�
				Estack[stacktop].type=ES_VARINT;
				Estack[stacktop].mode=0;
				Estack[stacktop].v=sid;
				Estack[stacktop++].set=0;
				Estack[stacktop].type=ES_OPERATOR;
				Estack[stacktop++].v='C';
			}	else
			if (Mident[p]->Type==ID_VAR) {
				Estack[stacktop].type=Mident[p]->vType;
				Estack[stacktop].mode=0;
				Estack[stacktop].v=Mident[p]->Value;
				Estack[stacktop++].set=(char)Mident[p]->VarSet;
				if (Mident[p]->vType==ES_VARSTRING) {
					Estack[stacktop-1].type=ES_VARINT;
					Estack[stacktop].type=ES_OPERATOR;
					Estack[stacktop++].v='C';
				}
			}	else
			if (Mident[p]->Type==ID_ARRAY) {
				int j;
				for (j=0;j<Mident[p]->sCount;j++) 
					Estack[stacktop++]=MyStack[Mident[p]->sStart+j];
				Estack[stacktop].type=Mident[p]->vType;
				Estack[stacktop].mode=1;
				Estack[stacktop].v=Mident[p]->Value;
				Estack[stacktop].set=(char)Mident[p]->VarSet;
				Estack[stacktop++].behind=Mident[p]->pCount;
				if (Mident[p]->vType==ES_VARSTRING) {
					Estack[stacktop-1].type=ES_VARINT;
					Estack[stacktop].type=ES_OPERATOR;
					Estack[stacktop++].v='C';
				}
			}	else
			if (Mident[p]->Type==ID_FUN) {
				int j;
				for (j=0;j<Mident[p]->sCount;j++) 
					Estack[stacktop++]=MyStack[Mident[p]->sStart+j];
				Estack[stacktop].type=Mident[p]->vType;
				Estack[stacktop].mode=2;
				Estack[stacktop].v=Mident[p]->Value;
				Estack[stacktop++].behind=Mident[p]->pCount;
			}
		}	else	Estack[stacktop].type=ES_OPERATOR,		// ����������
					Estack[stacktop++].v=p;
	}
	Mident.Clear();
	return 1;
}
/////////////////////////////////////////////////////////////////////
int IVMBetaScript::FlushStack(IVector<EStack> &Estack,int stacktop)
{
	IVector<EStack> Astack;
	int ap=0;
	Astack.AutoSize(1);
	Astack.SetLength(1);
	Output<<"; Flush stack: ";
	for (int j=0;j<stacktop;j++) {
		if (Estack[j].type==0) Output.VarArg("%c ",Estack[j].v);
		else Output.VarArg("%d ",Estack[j].type);
	}	Output.VarArg(" [%d]\n",stacktop);
	/*for (int j=0;j<stacktop;j++) {
		if (Estack[j].type==0) iserr.VarArg("%c ",Estack[j].v);
		else iserr.VarArg("%d-%d ",Estack[j].type,Estack[j].mode);
	}	iserr.VarArg(" [%d]\n",stacktop);
	*/
	for (int i=0;i<stacktop;i++) {
		int type=Estack[i].type;
		long v=Estack[i].v;
		if (type==ES_IMMINT||type==ES_IMMFLOAT) {	// ����������ѹջ
			Astack[ap].type=type;
			Astack[ap++].v=v;
			if (type==ES_IMMINT) Output.VarArg("PUSH %ld\n",v);
			else Output.VarArg("PUSH %ld \t; float value: %f\n",v,*(float*)&v);
		}	else
		if ((type&0x10)==0x10) {					// ������ѹջ
			IVMBetaVariable *set=&global;			// ��������
			if (Estack[i].mode==0) {
				if (Estack[i].set==1&&FunPoint) set=&FunPoint->var;
				set->pushv(v,Output,LABEL_ID);
			}	else
			if (Estack[i].mode==1) {				// �������
				int j;
				if (Estack[i].set==1&&FunPoint) set=&FunPoint->var;
				for (j=0;j<Estack[i].behind;j++) if (Astack[--ap].type!=ES_IMMINT) {
					iserr.VarArg("Array index error type=%ld ",Astack[ap].type);
					return 0;
				}
				set->pushv(v,Output,LABEL_ID);
			}	else
			if (Estack[i].mode==2) {				// ��������
				int tp,k,j;
				for (j=function[v]->parcount-1;j>=0;j--) {
					tp=Astack[--ap].type; k=1;
					if (tp==ES_IMMINT&&function[v]->parlist[j]!=ES_VARINT) k=0;
					if (tp==ES_IMMFLOAT&&function[v]->parlist[j]!=ES_VARFLOAT) k=0;
					if (tp==ES_VARSTRING&&function[v]->parlist[j]!=ES_VARSTRING) k=0;
					if (!k) {
						iserr<<"Parameters type in "<<function[v]->name.GetString()<<" do no match\n";
						return 0;
					}
				}
				Output<<"CALL "<<function[v]->name.GetString()<<"\n";
				Output.VarArg("CAL int ADD rs,%d\n",function[v]->parcount*4);
				Output<<"PUSH r3\n";
				type=function[v]->type;
			}
			switch (type)
			{
			case ES_VARSTRING:	Astack[ap++].type=ES_VARSTRING; break;
			case ES_VARINT:		Astack[ap++].type=ES_IMMINT; break;
			case ES_VARFLOAT:	Astack[ap++].type=ES_IMMFLOAT; break;
			}
		}	else
		if (type==ES_OPERATOR) {
			if (v=='+'||v=='-'||v=='*'||v=='/'||v=='%'||	// ��Ԫ�����
				v=='='||v=='>'||v=='<'||v==']'||v=='['||
				v=='&'||v=='|') {
				int type1,type2;
				int v1,v2,vresult;
				type2=Astack[--ap].type; v2=Astack[ap].v;	// ��һ������
				type1=Astack[--ap].type; v1=Astack[ap].v;	// �ڶ�������
				vresult=type1;
				Output<<"POP r1\n";
				Output<<"POP r0\n";
				if (type1!=type2) {		// ���Ͳ�ͬʱ����������ת������
					if (type1==ES_IMMINT&&type2==ES_IMMFLOAT) {
						Output<<"LD dword r3,r0\nIN r0,1\n";
						vresult=ES_IMMFLOAT;
					}	else 
					if (type1==ES_IMMFLOAT&&type2==ES_IMMINT) {
						Output<<"LD dword r3,r1\nIN r1,1\n";
						vresult=ES_IMMFLOAT;
					}	else
					if (type1==ES_VARSTRING&&type2==ES_IMMINT) {
						Output<<"LD dword r3,r1\nIN r2,2\n";	// INT -> STRING
						Output<<"IN r3,4\nLD dword r1,r2\n";
						vresult=ES_VARSTRING;
					}	else
					if (type1==ES_IMMINT&&type2==ES_VARSTRING) {
						Output<<"LD dword r3,r0\nIN r2,2\n";	// INT -> STRING
						Output<<"IN r3,4\nLD dword r0,r2\n";
						vresult=ES_VARSTRING;
					}	else 
					if (type1==ES_VARSTRING&&type2==ES_IMMFLOAT) {
						Output<<"LD dword r3,r1\nIN r2,2\n";
						Output<<"IN r3,10\nLD dword r1,r2\n";
						vresult=ES_VARSTRING;
					}	else
					if (type1==ES_IMMFLOAT&&type2==ES_VARSTRING) {
						Output<<"LD dword r3,r0\nIN r2,2\n";
						Output<<"IN r3,10\nLD dword r0,r2\n";
						vresult=ES_VARSTRING;
					}	else {
						iserr<<"Can not match the variable's type\n";
						Error=21;
						return 0;
					}
					type1=type2=vresult;
				}
				vresult=MakeOperate(v,type1);
				if (vresult==0) return 0;
				Astack[ap++].type=vresult;
				v1=v2+v1;
			}	else
			if (v=='!') {
				type=Astack[--ap].type;
				if (type==ES_VARSTRING) {
					iserr<<"Can not carry out the string operate NOT\n";
					Error=22;
					return 0;
				}
				Output<<"POP r1\n";
				Output<<"CMP int r1,0\n";
				Output.VarArg("JPC NZ LABEL%d\n",LABEL_ID);
				Output.VarArg("PUSH 1\n");
				Output.VarArg("JMP LABEL%d\n",LABEL_ID+1);
				Output.VarArg("LABEL%d:\n",LABEL_ID);
				Output.VarArg("PUSH 0\n");
				Output.VarArg("LABEL%d:\n",LABEL_ID+1);
				LABEL_ID+=2;
				Astack[ap++].type=ES_IMMINT;
			}	else
			if (v=='^') {
				type=Astack[--ap].type;
				if (type==ES_VARSTRING) {
					iserr<<"Can not carry out the string operate -\n";
					Error=22;
					return 0;
				}
				Output<<"POP r1\n";
				Output<<"LD int r0,0\n";
				Output.VarArg("CAL %s SUB r0,r1\n",(type==ES_IMMINT)?"int":"float");
				Output<<"PUSH r0\n";
				Astack[ap++].type=type;
			}	else
			if (v=='C') {
				type=Astack[--ap].type;
				Output<<"POP r2\n";
				Output<<"IN r3,2\n";
				Output<<"IN r2,5\n";
				Output<<"PUSH r3\n";
				Astack[ap++].type=ES_VARSTRING;
			}	else {
				iserr.VarArg("!! unknow operator %c\n",v);;
			}
		}
	}
	Output<<"POP r3\n";
	ap--;
	if (ap) {
		iserr.VarArg("Generally stack error at LINE %d ap=%d\n",Token.Line,ap);
		return 0;
	}
	return Astack[0].type;
}

/////////////////////////////////////////////////////////////////////
int IVMBetaScript::MakeOperate(int opr,int type)
{
	char *OPR=NULL;
	char *mode="ERROR";
	switch (opr)
	{
	case '+': OPR="ADD"; break;
	case '-': OPR="SUB"; break;
	case '*': OPR="MUL"; break;
	case '/': OPR="DIV"; break;
	case '%': OPR="MOD"; break;
	case '|': OPR="ADD"; break;
	case '&': OPR="MUL"; break;
	}
	if (OPR&&type!=ES_VARSTRING) {			// ��ͨ����
		switch(type)
		{
		case ES_IMMINT:		mode="int"; break;
		case ES_IMMFLOAT:	mode="float"; break;
		default:
			static char line[100];
			sprintf(line,"[ERROR type=%d]",type);
			mode=line;
			break;
		}
		Output<<"CAL "<<mode<<" "<<OPR<<" r0,r1\n";
		Output<<"PUSH r0\n";
		return type;
	}
	if (type==ES_VARSTRING) {	// �ַ�������
		int rtype;
		Output<<"LD dword r3,r0\n";
		Output<<"LD dword r2,r1\n";
		switch (opr)
		{
		case '+':	Output<<"IN r1,6\n";	// �ַ�������
					Output<<"LD dword r3,r2\n";
					Output<<"IN r3,8\n";	// ɾ���ڶ����ַ���
					Output<<"PUSH r0\n";	// �ַ�����ջ
					rtype=ES_VARSTRING;
					break;
		case '=': case '<': case '>': case '[': case ']':
					switch (opr) {	case '=': mode="NZ"; break;	case '<': mode="AE"; break;
									case '[': mode="A";  break;	case '>': mode="BE"; break;
									case ']': mode="B";  break; }
					Output<<"IN r0,9\n";	// �ַ����Ƚ�
					Output<<"CMP int r0,0\n";
					Output.VarArg("JPC %s LABEL%ld\n",mode,LABEL_ID);
					Output.VarArg("PUSH 1\n");
					Output.VarArg("JMP LABEL%ld\n",LABEL_ID+1);
					Output.VarArg("LABEL%ld:\n",LABEL_ID);
					Output.VarArg("PUSH 0\n");
					Output.VarArg("LABEL%ld:\n",LABEL_ID+1);
					LABEL_ID+=2;
					Output<<"IN r3,9\n";	// ɾ�������ַ���
					Output<<"LD dword r3,r2\n";
					Output<<"IN r3,9\n";
					rtype=ES_IMMINT;
					break;
		default:	iserr.VarArg("Sorry can carry out the string operate: %c\n",opr);
					rtype=0; 
					break;
		}
		return rtype;
	}
	switch (opr) {	case '=': mode="NZ"; break;	case '<': mode="AE"; break;
					case '[': mode="A";  break;	case '>': mode="BE"; break;
					case ']': mode="B";  break; }
	Output.VarArg("CMP %s r0,r1\n",(type==ES_IMMINT)?"int":"float");
	Output.VarArg("JPC %s LABEL%ld\n",mode,LABEL_ID);
	Output.VarArg("PUSH 1\n");
	Output.VarArg("JMP LABEL%ld\n",LABEL_ID+1);
	Output.VarArg("LABEL%ld:\n",LABEL_ID);
	Output.VarArg("PUSH 0\n");
	Output.VarArg("LABEL%ld:\n",LABEL_ID+1);
	LABEL_ID+=2;
	return ES_IMMINT;
}
