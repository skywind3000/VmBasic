#include "ivmscc.h"

void IVMBetaVariable::FreeSpace(Ixstream &output)
{
	int i;
	for (i=0;i<count;i++) if (vars[i]->type==2) {
		if (InStack) {
			output.VarArg("LD int r0,rb\n");
			output.VarArg("CAL int ADD r0,%d\n",vars[i]->spos);
			output.VarArg("LD int r3,[r0]\n");
			output.VarArg("IN r3,8\n");
		}	else {
			output.VarArg("LD int r3,[%s]\n",vars[i]->name.GetString());
			output.VarArg("IN r3,8\n");
		}
	}
}
int IVMBetaVariable::getaddr(int i,Ixstream &output,long &LABEL_ID)
{
	if (i<0||i>=count) return 0;
	if (vars[i]->mode==0) {
		if (InStack) {
			output.VarArg("LD int r0,rb\n");
			output.VarArg("CAL int ADD r0,%d\n",vars[i]->spos);
		}	else output.VarArg("LD int r0,%s\n",vars[i]->name.GetString());
	}	else {
		output<<"POP r1\n";
		int j;
		for (j=vars[i]->mode-1;j>0;j--) {
			output<<"POP r0\n";
			output.VarArg("CAL int MUL r0,%d\n",vars[i]->array[j]);
			output<<"CAL int ADD r1,r0\n";
		}
		output.VarArg("CMP dword r1,%d\n",vars[i]->size);
		output.VarArg("JPC B LABEL%d\n",LABEL_ID++);
		output.VarArg("LD int r1,%d\n",vars[i]->size-1);
		output.VarArg("LABEL%d:\n",LABEL_ID-1);
		if (InStack) {
			output.VarArg("LD int r0,rb\n");
			output.VarArg("CAL int ADD r0,%d\n",vars[i]->spos);
		}	else output.VarArg("LD int r0,%s\n",vars[i]->name.GetString());
		output<<"CAL int MUL r1,4\n";
		output<<"CAL int ADD r0,r1\n";
	}
	return 1;
}
int IVMBetaVariable::value(int i,int type,Ixstream &output,long &LABEL_ID)
{
	if (!getaddr(i,output,LABEL_ID)) return 0;
	if (vars[i]->type==0x10) {
		if (type==1) output<<"LD int [r0],r3\n";	else
		if (type==2) output<<"IN [r0],0\n";	else {
			output<<"IN [r0],3\n";
			output<<"IN r3,8\n";
		}
	}	else 
	if (vars[i]->type==0x11) {
		if (type==1) output<<"IN [r0],1\n"; else
		if (type==2) output<<"LD int [r0],r3\n"; else {
			output<<"IN [r0],11\n";
			output<<"IN r3,8\n";
		}
	}	else {
		if (type==1) {
			output<<"LD int r2,[r0]\n";
			output<<"IN r3,4\n";
		}	else
		if (type==2) {
			output<<"LD int r2,[r0]\n";
			output<<"IN r3,10\n";
		}	else {
			output<<"LD int r2,r3\n";
			output<<"LD int r3,[r0]\n";
			output<<"IN r3,5\n";
			output<<"LD int r3,r2\n";
			output<<"IN r3,8\n";
		}
	}
	return 1;
}
int IVMBetaVariable::pushv(int i,Ixstream &output,long &LABEL_ID)
{
	if (i<0||i>=count) return -1;
	if (vars[i]->mode==0) {
		if (InStack) {
			output.VarArg("LD int r0,rb\n");
			output.VarArg("CAL int ADD r0,%d\n",vars[i]->spos);
			output.VarArg("PUSH [r0]\n");
			return 0;
		}
		output.VarArg("PUSH [%s]\n",vars[i]->name.GetString());
		return 0;
	}
	output<<"POP r3\n";
	int j;
	for (j=vars[i]->mode-1;j>0;j--) {
		output<<"POP r0\n";
		output.VarArg("CAL int MUL r0,%d\n",vars[i]->array[j]);
		output<<"CAL int ADD r3,r0\n";
	}
	output.VarArg("CMP dword r3,%d\n",vars[i]->size);
	output.VarArg("JPC B LABEL%d\n",LABEL_ID++);
	output.VarArg("LD int r3,%d\n",vars[i]->size-1);
	output.VarArg("LABEL%d:\n",LABEL_ID-1);
	if (InStack) {
		output.VarArg("LD int r0,rb\n");
		output.VarArg("CAL int ADD r0,%d\n",vars[i]->spos);
	}	else output.VarArg("LD int r0,%s\n",vars[i]->name.GetString());
	output<<"CAL int MUL r3,4\n";
	output<<"CAL int ADD r0,r3\n";
	output<<"PUSH [r0]\n";
	return vars[i]->mode;
}
int IVMBetaVariable::regvar(char *name,int type,int *array,int off)
{
	int i;
	for (i=0;i<count;i++) 
		if (vars[i]->name==name) break;
	if (i<count) {
		if (vars[i]->type!=type) {
			iserr<<"Variable type error\n";
			return -1;
		}
		if (array&&vars[i]->mode<=0) {
			iserr<<"Variable types are not the same\n";
			return -2;
		}
		return i;
	}
	i=vars.Alloc();
	count++;
	vars[i]->name=name;
	vars[i]->type=type;
	vars[i]->mode=0;
	vars[i]->size=1;
	if (off) vars[i]->spos=off;
	else vars[i]->spos=StackP;
	if (array) {
		int j,size=1;
		vars[i]->array.AutoSize(1);
		for (j=0;array[j];j++) vars[i]->array[j]=array[j],size*=array[j];
		vars[i]->array[j]=0;
		vars[i]->mode=j;
		vars[i]->array.AutoSize(0);
		vars[i]->size=size;
		vars[i]->spos=vars[i]->spos-size*4+4;
	}
	StackP-=vars[i]->size*4;
	return i;
}
char *IVMBetaVariable::varname(char *name,int type)
{
	static iString n;
	switch (type)
	{
	case 0x10:	n="VINT_"; break;
	case 0x11:	n="VFLO_"; break;
	case 0x12:	n="VSTR_"; break;
	}
	n.Append(name);
	return n.GetString();
}
int IVMBetaVariable::pos(char *name)
{
	int i;
	for (i=0;i<count;i++) 
		if (vars[i]->name==name) break;
	if (i>=count) return -1;
	return i;
}
/////////////////////////////////////////////////////////////////////
int IVMBetaScript::MakeFunc()
{
	Token.Read();
	if (FunPoint) {
		iserr<<"Can not define a function or sub inside a function or sub\n";
		return 0;
	}
	iString oname,name;
	char *onamep=&Token.Text[0];
	oname=onamep;
	Token.Read();
	int type=ReadType();
	name=FunName(oname.GetString(),type);
	if (Token.Type!=IAccToken::OPERATOR||Token.Value!='(') {
		iserr<<"DECLARE function or sub must define parameters\n";
		return 0;
	}
	Token.Read();
	IVector<int> plist;
	plist.AutoSize(1);
	int i=0,ptype;
	iString pname,poname;
	char *ppname;
	IStringIdent parameter;	// 记录参数表
	while (Token.Type!=IAccToken::OPERATOR&&Token.Value!=')') {
		if (Token.Type!=IAccToken::IDENT) {
			iserr<<"DECLARE command not find )\n";
			return 0;
		}
		ppname=&Token.Text[0];
		poname=ppname;
		Token.Read();
		ptype=ReadType();
		pname=VarName(poname.GetString(),ptype);
		plist[i++]=ptype;
		parameter.Add(pname.GetString(),ptype);
		if (Token.Type==IAccToken::OPERATOR) {
			if (Token.Value==',') Token.Read(); else
			if (Token.Value!=')') {	iserr<<"DECLARE parameters error\n"; return 0; }
		}	else {	iserr<<"DECLARE parameters format error\n";	return 0; }
	}
	plist[i++]=0;
	Token.Read();
	if (!FunDeclare(name.GetString(),type,&plist[0])) return 0;
	int p=FunPos(name.GetString());
	if (p<0) {
		iserr<<"ERROR can not position function in the list\n";
		return 0;
	}
	function[p]->CompIt();
	FunPoint=function[p];
	IVMBetaVariable *varset=&function[p]->var;
	for (i=parameter.GetCount()-1,p=12;i>=0;i--,p+=4) {		// 注册参数到函数变量表
		varset->regvar(parameter.String(i),parameter.Id(i),NULL,p);
	}
	varset->StackP=0;
	pname=VarName(oname.GetString(),type);					// 函数要返回的数据
	varset->regvar(pname.GetString(),type,NULL);			// 注册此变量
	Istream *stream=Output.GetBind();
	Output.Bind(&FunPoint->code);	// 设置代码输出流
	Output<<"; This is a comment line\n";
	if (!ParseStatement()) return 0;
	Output.Bind(stream);			// 还原代码流
	FunPoint=NULL;
	if (Token.Type!=IAccToken::KEYWORD||(Token.Value!=subsym&&Token.Value!=funsym)) {
		iserr<<"Not find END SUB or END FUNCTION\n";
		return 0;
	}
	Token.Read();
	return 1;
}
/////////////////////////////////////////////////////////////////////
int IVMBetaScript::ExportFun(int i)
{
	if (i<0||i>=FunCount) return -1;
	if (function[i]->Comped()==0) {
		iserr<<"Function "<<function[i]->name.GetString()<<" not declared\n";
		return -2;
	}
	IVMBetaVariable *vset=&function[i]->var;
	Output.VarArg(";(%s) Function\n",function[i]->name.GetString());
	Output.VarArg(";parcount=%d\n",function[i]->parcount);
	Output.VarArg("%s:\n",function[i]->name.GetString());
	Output<<"PUSH rb\n";
	Output<<"LD int rb,rs\n";
	Output.VarArg("CAL int ADD rs,%d\n",function[i]->var.StackP);
	// ........
	int j;
	for (j=0;j<vset->GetCount();j++) // 初始化字符串
		if (vset->GetType(j)==ES_VARSTRING&&vset->GetPos(j)<=0) {
			if (vset->GetMode(j)==0) {
				Output<<"LD int r0,rb\n";
				Output.VarArg("CAL int ADD r0,%d\n",vset->GetPos(j));
				Output<<"IN [r0],2\n";
			}	else {
				int k;
				for (k=0;k<vset->GetSize(j);k++) {
					Output<<"LD int r0,rb\n";
					Output.VarArg("LD int r1,%d\n",k);
					Output<<"CAL int MUL r1,4\n";
					Output.VarArg("CAL int ADD r0,%d\n",vset->GetPos(j));
					Output<<"CAL int ADD r0,r1\n";
					Output<<"IN [r0],2\n";
				}
			}
		}
	function[i]->code.Seek(0,ISTREAM_START);
	while (!function[i]->code.Eof()) {	// 拷贝程序代码	
		Output.PutCh(function[i]->code.GetCh());
	}
	// ........
	Output.VarArg("%s_EXIT:\n",function[i]->name.GetString());
	for (j=0;j<vset->GetCount();j++) {	// 释放字符串变量
		if (vset->GetType(j)==ES_VARSTRING&&vset->GetPos(j)!=0) {
			if (vset->GetMode(j)==0) {
				Output<<"LD int r0,rb\n";
				Output.VarArg("CAL int ADD r0,%d\n",vset->GetPos(j));
				Output<<"LD int r3,[r0]\n";
				Output<<"IN r3,8\n";
			}	else {
				int k;
				for (k=0;k<vset->GetSize(j);k++) {
					Output<<"LD int r0,rb\n";
					Output.VarArg("LD int r1,%d\n",k);
					Output<<"CAL int MUL r1,4\n";
					Output.VarArg("CAL int ADD r0,%d\n",vset->GetPos(j));
					Output<<"CAL int ADD r0,r1\n";
					Output<<"LD int r3,[r0]\n";
					Output<<"IN r3,8\n";
				}
			}
		}
	}
	Output<<"LD int r3,[rb]\n";
	Output<<"LD int rs,rb\n";
	Output<<"POP rb\n";
	Output<<"RET\n";
	Output.VarArg("; END FUN %s\n\n",function[i]->name.GetString());	
	return 0;
}

