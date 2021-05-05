#include "ivmscc.h"

IVMBetaScript::IVMBetaScript()
{
	int i=0;
	static char *keyword[]={"GOTO","IF","THEN","ELSE","DIM","MOD","NOT",
							"AND","OR","END","VASM","PRINT","INPUT","SHARED",
							"SUB","FUNCTION","DECLARE","CALL","RETURN",
							"WHILE","WEND","GOSUB","INPORT","OUTPORT",
							"CFLINE","LOADPTR",0};
	for (i=0;keyword[i];i++) KeyWords[i]=keyword[i];
	KeyWords[i]=0;
}
int IVMBetaScript::GoScript(Istream &input,Istream &output)
{
	Token.Start(input,KeyWords);	// 设置词法分析器
	Input.Bind(&input);				// 设置输入
	TagOut.Bind(&output);			// 设置输出
	Output.Bind(&output);
	global.Clear();					// 清除全局变量
	labels.Clear();					// 清除地址
	reference.Clear();				// 清除跳转地址
	strings.Clear();				// 清除常量字符串
	function.Clear();				// 清除函数表
	FunPoint=NULL;					// 当前为全局代码
	FunCount=0;						// 函数个数为零
	Error=0;		
	LABEL_ID=0;
	Token.Read();
	return ParseProgram();
}
int IVMBetaScript::ParseProgram()	// 编译一个脚本程序
{
	int label=LABEL_ID++;
	int failed=LABEL_ID++;
	int def=LABEL_ID++;
	int i,j;
	Output.VarArg("CALL LABEL%d ; For initialization\n",label);
	Output<<"\n;================ MAIN PROG ==================\n";
	if ((i=ParseStatement())<0) return 0;
	Output<<"EXIT\n";
	Output<<"\n;================ FUNCTIONS ==================\n";
	for (i=0;i<FunCount;i++) if (ExportFun(i)) return 0;
	Output<<"\n;================ Init Code ==================\n";
	Output.VarArg("LABEL%d:\n",label);
	for (i=0;i<global.GetCount();i++) {
		if (global.GetType(i)==ES_CSTRING) {
			Output.VarArg("LD dword [%s],CS_%s\n",global.GetName(i),global.GetName(i));
		}	else
		if (global.GetType(i)==ES_VARSTRING) {
			if (global.GetMode(i)==0) {
				Output.VarArg("IN [%s],2\n",global.GetName(i));
			}	else {
				int k;
				for (k=0;k<global.GetSize(i);k++) {
					Output.VarArg("LD int r0,%s\n",global.GetName(i));
					Output.VarArg("LD int r1,%d\n",k);
					Output<<"CAL int MUL r1,4\n";
					Output<<"CAL int ADD r0,r1\n";
					Output<<"IN [r0],2\n";
				}
			}
		}
	}
	Output<<"RET\n";
	Output.VarArg("LABEL%d:\n",failed);
	Output.VarArg("EXIT\n");
	Output.VarArg("\n; Declaration:\n");
	Output.VarArg("LABEL%d:\n",def);
	for (i=0;i<global.GetCount();i++)
		switch (global.GetType(i))
		{
		case ES_VARINT:
		case ES_VARFLOAT:
		case ES_VARSTRING:
			if (global.GetMode(i)==0) 
				Output.VarArg("DATA %s dword 0\n",global.GetName(i)); 
			else	{
				Output.VarArg("DATA %s dword 0\n",global.GetName(i));
				Output.VarArg(".BLOCK %d 0\n",global.GetSize(i)*4);
			}
			break;
		case ES_CSTRING:
			j=strings.Index(i);
			Output.VarArg("DATA %s dword 0\n",global.GetName(i));
			Output.VarArg("DATA CS_%s char \"%s\",0\n",global.GetName(i),strings.String(j));
			break;
		}
	return 1;
}
/////////////////////////////////////////////////////////////////////
int IVMBetaScript::ParseStatement()
{
	if (Token.Type==IAccToken::ENDF) return 1; 
	int r=ParseLine();
	if (r==2) return 2;	// 如果模块结束
	if (r==0) return 0; // 如果程序结束
	if (r<0) {			// 如果模块有错
		iserr.VarArg("ERROR FIND IN LINE %d CODE=%d\n",Token.Line,Error);
		return -1;
	}
	if (Token.Type==IAccToken::ENDL) {
		Token.Read();
		return ParseStatement();
	}
	iserr<<"Unknow ident ";
	return -2;
}
// 编译一行
// 如果返回值>0表示正常退出，0为编译结束
int IVMBetaScript::ParseLine()
{
	if (Token.Type==IAccToken::ENDF) return 0;
	if (Token.Type==IAccToken::ENDL) return 1;
	int r=ParseScript();
	if (r==2) return 2;	// 模块结束
	if (r<0) return -1; // 错误
	if (Token.Type==IAccToken::ENDL) return 1;
	if (Token.Type==IAccToken::ENDF) return 0;
	if (Token.Type==IAccToken::OPERATOR&&Token.Value==':') {
		Token.Read();
		return ParseLine();
	}
	iserr.VarArg("Operator : not find but find %d %s\n",Token.Type,&Token.Text[0]);
	return -2;
}
/////////////////////////////////////////////////////////////////////
int IVMBetaScript::MakePrint()
{
	int ok=1,r;
	while (1)
	{
		if (Token.Type==IAccToken::ENDL||Token.Type==IAccToken::ENDF) {
			Output.VarArg("OUT 4,%d\n",'\n');
			break;
		}
		if (Token.Type==IAccToken::OPERATOR) {
			if (Token.Value==':') { Output.VarArg("OUT 4,%d\n",'\n'); break; }
			if (Token.Value==';') { 
				Token.Read();	
				if (Token.Type==IAccToken::ENDL||Token.Type==IAccToken::ENDF) break;
			} else 
			if (Token.Value==',') { 
				Token.Read();
				Output.VarArg("OUT 4,%d\n",'\t'); 
				if (Token.Type==IAccToken::ENDL||Token.Type==IAccToken::ENDF) break;
			} 
			if (Token.Type==IAccToken::ENDL||Token.Type==IAccToken::ENDF) {
				Output.VarArg("OUT 4,%d\n",'\n');
				break;
			}
		}
		r=ParseExpression();
		if (r==0) {	ok=0; break; }	else
		if (r==ES_IMMINT) {
			Output<<"OUT 3,r3\n";
		}	else
		if (r==ES_IMMFLOAT) {

			Output<<"OUT 5,r3\n";
		}	else
		if (r==ES_VARSTRING) {
			Output<<"OUT 2,r3\n";
			Output<<"IN r3,8\n";
		}
	}
	return ok;
}

int IVMBetaScript::MakeLabel(char *s)
{
	if (labels.Id(s)>=0) {
		Error=1;
		iserr<<"Label had already been defined";
		return 0;
	}
	labels.Add(s,Token.Line);
	iString label;
	if (FunPoint==NULL) label.VarArg("LABEL_%s",s);
	else label.VarArg("LABEL_%s_%s",FunPoint->name.GetString(),s);
	Output<<label.GetString()<<":\n";
	return 1;
}
int IVMBetaScript::MakeGoto(char callf)
{
	Token.Read();
	if (Token.Type!=IAccToken::IDENT) {
		iserr<<"GOTO/CALL command error\n";
		Error=3;
		return 0;
	}
	if (reference.Id(&Token.Text[0])<0) reference.Add(&Token.Text[0],Token.Line);
	iString label;
	if (FunPoint==NULL) label.VarArg("LABEL_%s",&Token.Text[0]);
	else label.VarArg("LABEL_%s_%s",FunPoint->name.GetString(),&Token.Text[0]);
	if (callf) Output<<"CALL "<<label.GetString()<<"\n";
	else Output<<"JMP "<<label.GetString()<<"\n";
	Token.Read();
	return 1;
}
int IVMBetaScript::MakeVAsm()
{
	if (Token.Value==vasmsym) {		// 内嵌汇编
		Token.Read();
		if (Token.Type==IAccToken::OPERATOR&&Token.Value=='(') {
			Token.Read();
			if (Token.Type==IAccToken::STRING) {
				Output.VarArg("%s\t;Inline VAsm\n",&Token.Text[0]);
				Token.Read();
				if (Token.Type==IAccToken::OPERATOR&&Token.Value==')') {
					Token.Read();
					return 1;
				}
			}
		}
	}
	iserr<<"Inline VAsm error\n";
	return 0;
}
int IVMBetaScript::MakeDim()
{
	int share=0;
	IVMBetaVariable *varset;
	Token.Read();
	if (FunPoint==NULL) share=1;
	if (Token.Type==IAccToken::KEYWORD&&Token.Value==sharesym) {
		Token.Read();
		share=1;
	}
	if (FunPoint==NULL) varset=&global;
	else varset=(share)?&global:&FunPoint->var;
	while (1) {
		if (Token.Type!=IAccToken::IDENT) {
			iserr<<"DIM format error\n";
			return 0;
		}
		char *name=&Token.Text[0];
		iString text;
		text.Set(name);
		Token.Read();
		int type=ReadType();
		if (Token.Type==IAccToken::OPERATOR&&Token.Value=='(') {
			Token.Read();
			IVector<int> array;
			array.AutoSize(1);
			int ap=0;
			while (Token.Type!=IAccToken::OPERATOR&&Token.Value!=')') {
				if (Token.Type!=IAccToken::INTEGER) {
					iserr<<"DIM array format error\n";
					return 0;
				}
				array[ap++]=Token.Value;
				Token.Read();
				if (Token.Type==IAccToken::OPERATOR) {
					if (Token.Value==',') Token.Read(); else
					if (Token.Value!=')') {
						iserr<<"DIM array format error: no , or )\n";
						return 0;
					}
				}	else {
					iserr<<"DIM array format error: no , or )\n";
					return 0;
				}
			}
			Token.Read();
			array[ap]=0;
			varset->regvar(varset->varname(text.GetString(),type),type,&array[0]);
		}	else varset->regvar(varset->varname(text.GetString(),type),type,NULL);
		if (Token.Type==IAccToken::OPERATOR) {
			if (Token.Value==',') Token.Read();
			else break;
		}	else break;
	}
	return 1;
}
int IVMBetaScript::MakeIf()
{
	long LabelElse=LABEL_ID++;
	long LabelEndif=LABEL_ID++;
	int stype,hasendif=0,i;
	Token.Read();
	Output<<"; If code\n";
	stype=ParseExpression();
	if (!stype) {
		iserr<<"Logic expression error\n";
		Error=21;
		return 0;
	}
	if (stype==ES_VARSTRING||stype==ES_VARFLOAT) {
		iserr<<"Logic expression result error\n";
		Error=22;
		return 0;
	}
	if (Token.Type!=IAccToken::KEYWORD&&Token.Value!=thensym) {
		iserr<<"THEN not find\n";
		Error=26;
		return 0;
	}
	Token.Read();
	Output<<"CMP int r3,0\n";
	Output.VarArg("JPC Z LABEL%ld\n",LabelElse);	// 如果条件不成立则跳到Elseif
	if (Token.Type==IAccToken::ENDL) {				// 如果不是单行指令
	if (ParseStatement()<1) return 0;
		hasendif=1; 
	}	else {
		if ((i=ParseLine())<0) { Error=i; Error=24; return 0; }
	}
	Output.VarArg("JMP LABEL%ld\n",LabelEndif);
	Output.VarArg("LABEL%ld:\t; Else\n",LabelElse);
	if (Token.Type==IAccToken::KEYWORD&&Token.Value==elsesym) {
		Token.Read();
		hasendif=0;
		if (Token.Type==IAccToken::ENDL) { 
			if (ParseStatement()<0) return 0;
			hasendif=1; 
		}
		else if (ParseLine()<0) return 0;
	}
	if (hasendif) {
		if (Token.Type==IAccToken::KEYWORD&&Token.Value==ifsym) Token.Read();
		else {
			iserr.VarArg("END IF not find in line %d\n",Token.Line);
			return 0;
		}
	}
	Output.VarArg("LABEL%ld:\t; End of If\n",LabelEndif);
	return 1;
}
int IVMBetaScript::MakeWhile()
{
	long WStart=LABEL_ID++;
	long WEnd=LABEL_ID++;
	int stype,mode=0;
	Token.Read();
	Output.VarArg("LABEL%d:\t; While code\n",WStart);
	stype=ParseExpression();
	if (!stype) {
		iserr<<"Logic expression error\n";
		Error=21;
		return 0;
	}
	if (stype==ES_VARSTRING||stype==ES_VARFLOAT) {
		iserr<<"Logic expression result error\n";
		Error=22;
		return 0;
	}
	if (Token.Type==IAccToken::OPERATOR) {
		if (Token.Value==':') {
			mode=1; 
			Token.Read();
		}	else {
			iserr<<"WHILE format error\n";
			return 0;
		}
	}	else
	if (Token.Type==IAccToken::ENDL) Token.Read();
	else {
		iserr<<"WHILE format error\n";
		return 0;
	}
	Output<<"CMP int r3,0\n";
	Output.VarArg("JPC Z LABEL%ld\n",WEnd);
	if (mode==0) mode=ParseStatement();
	else mode=ParseLine();
	if (mode!=2) {
		iserr<<"WEND not find\n";
		return 0;
	}
	Token.Read();
	Output.VarArg("JMP LABEL%d\n",WStart);
	Output.VarArg("LABEL%d:\t; End of While\n",WEnd);
	return 1;
}
int IVMBetaScript::MakeInput()
{
	Token.Read();
	while (1)
	{
		if (Token.Type==IAccToken::ENDL||Token.Type==IAccToken::ENDF) break;
		if (Token.Type==IAccToken::OPERATOR) {
			if (Token.Value==';') { Token.Read(); } else
			if (Token.Value==',') { Token.Read(); } else
			break;
		}	else
		if (Token.Type==IAccToken::IDENT) {
			int type=VarAddr();
			if (!type) return 0;
			Output<<"OUT 4,63\n";	// 输出问号
			switch (type)
			{
			case ES_VARINT:	  Output<<"OUT 10,0\n"<<"LD int [r0],r3\n"; break;
			case ES_VARFLOAT: Output<<"OUT 12,0\n"<<"LD float [r0],r3\n"; break;
			case ES_VARSTRING:Output<<"LD int r3,[r0]\n"<<"OUT 11,0\n"; break;
			}
		}
	}
	return 1;
}
int IVMBetaScript::MakeLoadptr()
{
	Token.Read();
	int t1=VarAddr();
	if (t1!=ES_VARINT) return 0;
	Output<<"PUSH r0\n";
	int t2=VarAddr();
	if (!t2) return 0;
	Output<<"POP r1\n";
	Output<<"LD int [r1],r0\n";
	return 1;
}
/////////////////////////////////////////////////////////////////////
/* 处理一条Basic指令，而碰到诸如IF/SUB等模块化指令
   则处理一块，这是语言最核心的部分 */
int IVMBetaScript::ParseScript()
{
	if (Token.Type==IAccToken::KEYWORD)	// 第一个字符为关键字
	{
		if (Token.Value==gotosym)  { if (!MakeGoto()) return -1;	}	else
		if (Token.Value==callsym)  { if (!MakeGoto(1)) return -1; } else
		if (Token.Value==gosubsym) { if (!MakeGoto(1)) return -1; } else
		if (Token.Value==vasmsym) {	if (!MakeVAsm()) return -2;	}	else
		if (Token.Value==dimsym) { if (!MakeDim()) return -3; } else
		if (Token.Value==ifsym)  { if (!MakeIf()) return -4; }	else
		if (Token.Value==declaresym) { if (!MakeDeclare()) return -5; } else
		if (Token.Value==subsym||Token.Value==funsym) { if (!MakeFunc()) return -6; } else
		if (Token.Value==elsesym) { return 2; } else	// 碰到模块结束指令则不处理
		if (Token.Value==wendsym) { return 2; } else
		if (Token.Value==whilesym) { if (!MakeWhile()) return -7; }	else
		if (Token.Value==ldptrsym) { if (!MakeLoadptr()) return -10; } else
		if (Token.Value==inputsym) { if (!MakeInput()) return -11; } else
		if (Token.Value==cflinesym) { 
			Token.Read();
			if (Token.Type!=IAccToken::INTEGER) {
				Output<<"Syntax error in cfline ";
				return -9;
			}
			Token.Line=Token.Value;
			Token.Read();
		}	else
		if (Token.Value==retsym) {
			Token.Read();
			Output<<"RET\n";
		}	else
		if (Token.Value==endsym) {
			Token.Read();
			if (Token.Type==IAccToken::KEYWORD) return 2;
			if (!(Token.Type==IAccToken::OPERATOR&&Token.Value==':')) return 2;
			Output<<"EXIT\n";
		}	else
		if (Token.Value==printsym) {
			Token.Read();
			if (!MakePrint()) return -7;
		}
	}	else 
	if (Token.Type==IAccToken::IDENT)	// 第一个字符为识别符
	{
		if (!MakeValue()) return -8;
	}	else
	if (Token.Type==IAccToken::ENDL) return 0;
	else {
		iserr.VarArg("Unknow command %s\n",&Token.Text[0]);
		Error=7;
		return -7;
	}
	return 0;
}
int IVMBetaScript::MakeDeclare()
{
	Token.Read();
	if (Token.Type!=IAccToken::KEYWORD) {
		iserr<<"DECLARE command format error\n";
		return 0;
	}
	if (Token.Value!=subsym&&Token.Value!=funsym) {
		iserr<<"DECLARE command format can not be recognized\n";
		return 0;
	}
	Token.Read();
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
	int i=0;
	while (Token.Type!=IAccToken::OPERATOR&&Token.Value!=')') {
		if (Token.Type!=IAccToken::IDENT) {
			iserr<<"DECLARE command not find )\n";
			return 0;
		}
		Token.Read();
		plist[i++]=ReadType();
		if (Token.Type==IAccToken::OPERATOR) {
			if (Token.Value==',') Token.Read(); else
			if (Token.Value!=')') {	iserr<<"DECLARE parameters error\n"; return 0; }
		}	else {	iserr<<"DECLARE parameters format error\n";	return 0; }
	}
	plist[i++]=0;
	Token.Read();
	return FunDeclare(name.GetString(),type,&plist[0]);
}
/////////////////////////////////////////////////////////////////////
int IVMBetaScript::FunDeclare(char *name,int type,int *par)
{
	int p=FunPos(name);
	int pcount,i;
	if (par==NULL) pcount=0;
	else for (i=0,pcount=0;par[i];i++) pcount++;
	if (p>=0) {		// 如果以前定义过函数
		if (function[p]->type!=type) {
			iserr<<"Compiler error\n";
			return 0;
		}
		if (function[p]->parcount!=pcount) {
			iserr<<"The parameter count in "<<name<<" is different from before\n";
			return 0;
		}
		if (par) {
			for (i=0;i<pcount;i++) if (par[i]!=function[p]->parlist[i]) {
				iserr<<"The parameter in "<<name<<" is different from before\n";
				return 0;
			}
		}
		return 1;
	}
	p=function.Alloc();
	FunCount++;
	function[p]->name=name;
	function[p]->parcount=pcount;
	function[p]->type=type;
	function[p]->var.SetMode(1);
	function[p]->parlist.SetLength(pcount+1);
	if (par) for (i=0;i<pcount+1;i++) function[p]->parlist[i]=par[i];
	return 1;
}
int IVMBetaScript::FunPos(char *name)
{
	int i,j=-1;
	for (i=0;i<FunCount;i++) if (function[i]->name==name) { j=i; break; }
	return j;
}
char *IVMBetaScript::FunName(char *name,int type)
{
	static iString n;
	switch (type)
	{
	case 0x10:	n="FINT_"; break;
	case 0x11:	n="FFLO_"; break;
	case 0x12:	n="FSTR_"; break;
	}
	n.Append(name);
	return n.GetString();
}
char *IVMBetaScript::VarName(char *name,int type)
{
	static iString n;
	switch (type)
	{
	case ES_VARINT:		n="VINT_"; break;
	case ES_VARFLOAT:	n="VFLO_"; break;
	case ES_VARSTRING:	n="VSTR_"; break;
	}
	n.Append(name);
	return n.GetString();
}
int IVMBetaScript::ReadType()
{
	int type=ES_VARINT;
	if (Token.Type==IAccToken::OPERATOR) {
		if (Token.Value=='!') { Token.Read(); type=ES_VARFLOAT; }
		else if (Token.Value=='$') { Token.Read(); type=ES_VARSTRING; }
		else if (Token.Value=='%') { Token.Read(); type=ES_VARINT; }
	}
	return type;
}
int IVMBetaScript::MakeValue()
{
	iString name,var,fname;
	int stacktype,i,type;
	name=&Token.Text[0];
	Token.Read();
	if (Token.Type==IAccToken::OPERATOR&&Token.Value==':') {
		if (!MakeLabel(name.GetString())) return 0;	
		return 1;
	}
	/*************************/
	type=ReadType();
	var=VarName(name.GetString(),type);
	fname=FunName(name.GetString(),type);
	int fun=-1;
	for (i=0;i<FunCount;i++) if (function[i]->name==fname) fun=i;
	if (FunPoint&&fun>=0) 
		if (FunPoint->name==fname&&Token.Type==IAccToken::OPERATOR&&Token.Value=='=') fun=-1;
	if (fun>=0) {
		stacktype=type;
		if (Token.Type==IAccToken::OPERATOR&&Token.Value=='(') Token.Read();
		else if (function[fun]->parcount>0) {
			iserr<<"Parameters required ";
			return 0;
		}
		for (i=0;i<function[fun]->parcount;i++) {
			int k=1;
			type=ParseExpression();
			if (!type) return 0;
			if (type==ES_IMMINT&&function[fun]->parlist[i]!=ES_VARINT) k=0;
			if (type==ES_IMMFLOAT&&function[fun]->parlist[i]!=ES_VARFLOAT) k=0;
			if (type==ES_VARSTRING&&function[fun]->parlist[i]!=ES_VARSTRING) k=0;
			Output<<"PUSH r3\n";
			if (!k) {
				iserr<<"Parameters type in "<<function[fun]->name.GetString()<<" do no match\n";
				return 0;
			}
			if (Token.Type==IAccToken::OPERATOR&&Token.Value==',') Token.Read();
		}
		if (Token.Type==IAccToken::OPERATOR&&Token.Value==')') Token.Read();
		else if (function[fun]->parcount>0) {
			iserr<<"not find ) ";
			return 0;
		}
		Output<<"CALL "<<function[fun]->name.GetString()<<"\n";
		Output.VarArg("CAL int ADD rs,%d\n",function[fun]->parcount*4);
		if (stacktype==ES_VARSTRING) Output<<"IN r3,8\n";
		return 1;
	}
	int mode,index;
	IVMBetaVariable *set;
	if (!VarInfo(var.GetString(),type,set,index,mode)) return 0;
	if (Token.Type==IAccToken::OPERATOR&&Token.Value=='(') {	// 如果按照数组操作
		if (mode<1) {		// 如果不是数组
			iserr<<"Variable is not a array or function\n";
			return 0;
		}
		Token.Read();
		for (i=0;i<mode;i++) {
			stacktype=ParseExpression();
			if (stacktype!=ES_IMMINT) {
				iserr<<"Array of variable format error\n";
				return 0;
			}
			Output<<"PUSH r3\n";
			if (Token.Type==IAccToken::OPERATOR) {
				if (Token.Value==','||Token.Value==')') Token.Read();
				else {
					iserr<<"Array format error\n";
					return 0;
				}
			}	else { iserr<<"Array format error\n"; return 0; }
		}
	}	else if (mode!=0) {
		iserr<<"Can not find the index of array "<<name.GetString()<<"\n";
	}
	if (Token.Type==IAccToken::OPERATOR&&Token.Value=='=') {
		Token.Read();
		stacktype=ParseExpression();
		if (!stacktype) return 0;
		return set->value(index,stacktype,Output,LABEL_ID);
	}	else {
		iserr.VarArg("Unknow operate %s",&Token.Text[0]);
		Error=7;
		return 0;
	}
}
int IVMBetaScript::VarAddr()
{
	iString name,var,fname;
	name=&Token.Text[0];
	Token.Read();
	/*************************/
	int type=ReadType();
	var=VarName(name.GetString(),type);
	fname=FunName(name.GetString(),type);
	int fun=-1,i;
	for (i=0;i<FunCount;i++) if (function[i]->name==fname) fun=i;
	if (fun>=0) {
		iserr<<name.GetString()<<" should be a variable but not a function";
		return 0;
	}
	int mode,index,stacktype;
	IVMBetaVariable *set;
	if (!VarInfo(var.GetString(),type,set,index,mode)) return 0;
	if (Token.Type==IAccToken::OPERATOR&&Token.Value=='(') {	// 如果按照数组操作
		if (mode<1) {		// 如果不是数组
			iserr<<"Variable is not a array or function\n";
			return 0;
		}
		Token.Read();
		for (i=0;i<mode;i++) {
			stacktype=ParseExpression();
			if (stacktype!=ES_IMMINT) {
				iserr<<"Array of variable format error\n";
				return 0;
			}
			Output<<"PUSH r3\n";
			if (Token.Type==IAccToken::OPERATOR) {
				if (Token.Value==','||Token.Value==')') Token.Read();
				else {
					iserr<<"Array format error\n";
					return 0;
				}
			}	else { iserr<<"Array format error\n"; return 0; }
		}
	}	else if (mode!=0) {
		iserr<<"Can not find the index of array "<<name.GetString()<<"\n";
	}
	if (!set->getaddr(index,Output,LABEL_ID)) return 0;
	return type;
}
int IVMBetaScript::VarInfo(char *name,int type,IVMBetaVariable* &set,int &i,int &mode)
{
	i=global.pos(name);
	mode=0;
	set=&global;
	if (i>=0) {			// 如果在全局变量里面
		mode=global.GetMode(i);
	}	else {
		if (FunPoint) {	// 如果是函数
			i=FunPoint->var.pos(name);
			if (i>=0) mode=FunPoint->var.GetMode(i);
			else if ((i=FunPoint->var.regvar(name,type,NULL))<0) { iserr<<"regvar error\n"; return 0; }
			set=&FunPoint->var;
		}	else {
			if ((i=global.regvar(name,type,NULL))<0) { iserr<<"regvar error2\n"; return 0; }
		}
	}
	return 1;
}
