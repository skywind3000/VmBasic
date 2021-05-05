#include "iacciden.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/////////////////////////////////////////////////////////////////////
IIdentFinder::IIdentFinder()
{
	root=NULL;
	Count=0;
}

IIdentFinder::~IIdentFinder()
{
	list.SetLength(0);
	Clear();
}

int IIdentFinder::Clear()
{
	for (IIdentLink *p;root;root=p)
	{
		if (root->sValue) delete root->sValue;
		p=root->Next;
		delete root;
	}
	Count=0;
	return 0;
}

int IIdentFinder::FindIdent(char *str,char **key_words,int HighCase)
{
	IIdentLink *root=NULL;
	IIdentLink *prev=root,*n;
	char buffer[256];
	char **key=key_words;
	char *d[2]={ 0,0 };
	int len=strlen(str);
	int i,p,m,c,point,j;

	if (!str) return -1;
	if (!key) key=d;
	Clear();
	for (i=0,p=0,m=0,c=0;i<len;)
	{
		switch (m)
		{
		case 0:
			if (isspace(str[i])) i++;
			else if (isalpha(str[i])||str[i]=='_') m=1, c++;
			else if (isalnum(str[i])) m=2, c++, point=0;
			else if (str[i]=='$'&&((str[i+1]>='0'&&str[i+1]<='9')||(str[i+1]>='A'&&str[i+1]<='F')||(str[i+1]>='a'&&str[i+1]<='f'))) m=3, i++, c++;
			else if (str[i]=='\"') m=4, c++;
			else if (str[i]==';'||str[i]=='\'') m=5;
			else m=6, c++;
			break;
		case 1:
			buffer[p]=str[i];
			p++; i++;
			if (isalnum(str[i])||isalpha(str[i])||str[i]=='_');
			else {
				if (HighCase) {
					for (int k=0;k<p;k++) if (buffer[k]>='a'&&buffer[k]<='z') buffer[k]-='a'-'A';
				}
				buffer[p]=0, p=m=0;
				n=new IIdentLink;
				if (n)
				{
					n->Type=Ident;
					n->Next=NULL;
					for (j=0;key[j];j++) if (strcmp(buffer,key[j])==0) break;
					if (key[j]) n->Type=KeyWord, n->Value=j;
					n->sValue=new char[strlen(buffer)+1];
					if (n->sValue) strcpy(n->sValue,buffer);
					else { delete n; n=NULL; }
					if (n) { if (prev) prev->Next=n; else root=n; prev=n; }
				}
			}
			break;
		case 2:
			buffer[p]=str[i];
			p++; i++;
			if (isalnum(str[i]));
			else if (str[i]=='.'&&point==0) point=1;
			else { 
				buffer[p]=0, p=m=0;
				n=new IIdentLink;
				if (n) {
					if (!point) n->Type=Integer, n->iValue=atol(buffer);
					else n->Type=Float, n->fValue=(float)atof(buffer);
					n->Next=NULL;
					n->sValue=new char[strlen(buffer)+1];
					if (n->sValue) strcpy(n->sValue,buffer);
					if (prev) prev->Next=n; else root=n;
					prev=n;
				}
			}
			break;
		case 3:
			buffer[p]=str[i];
			p++; i++;
			if ((str[i]>='0'&&str[i]<='9')||(str[i]>='A'&&str[i]<='F')||(str[i]>='a'&&str[i]<='f'));
			else {
				buffer[p]=0; p=m=0;
				n=new IIdentLink;
				if (n) {
					n->Type=Integer;
					n->Next=NULL;
					sscanf(buffer,"%lx",&n->iValue);
					n->sValue=new char[strlen(buffer)+3];
					if (n->sValue) {
						n->sValue[0]='$';
						strcpy(n->sValue+1,buffer);
					}
					if (prev) prev->Next=n; else root=n;
					prev=n;
				}
			}
			break;
		case 4:
			buffer[p]=str[i];
			p++; i++;
			if (str[i]=='\"') {
				buffer[p]='\"', buffer[p+1]=0, i++, p=m=0;
				n=new IIdentLink;
				if (n) {
					n->Type=String;
					n->Next=NULL;
					n->sValue=new char[strlen(buffer)+3];
					if (n->sValue) { strcpy(n->sValue,buffer); if (prev) prev->Next=n; else root=n; prev=n; }
					else free(n);
				}
			}
			break;
		case 5:
			i++; if (str[i]=='\n')  m=0;
			break;
		case 6:
			buffer[p]=str[i];
			buffer[p+1]=0;
			i++; p=m=0;
			n=new IIdentLink;
			if (n) {
				n->Type=Operator;
				n->Next=NULL;
				n->Value=buffer[0];
				n->sValue=new char[strlen(buffer)+3];
				if (n->sValue) n->sValue[0]=buffer[0],n->sValue[1]=0;
				if (prev) prev->Next=n; else root=n;
				prev=n;
			}
			break;
		}
	}
	this->root=root;
	for (Count=0,n=root;n;n=n->Next) Count++;
	list.SetLength(Count);
	for (n=root,i=0;i<Count&&n;n=n->Next,i++) list[i]=*n;
	return Count;
}

int IIdentFinder::GetCount()
{
	return Count;
}

IIdentFinder::IIdentLink* IIdentFinder::operator[](int i)
{
	static IIdentLink Default={String,0,0,0,"",&Default};
	if (Count<1) Default.Next=&Default;
	if (i>=Count||i<0) return &Default;
	return &list[i];
}
/////////////////////////////////////////////////////////////////////
int IBasicToken::getnc()
{
	if (!is) { ch=-1; return 0; }
	if (is->Eof()) { ch=-1; return 0; }
	ch=is->GetCh();
	if (ch<-1&&ch>-128) ch=256+ch;       
	return 1;
}
int IBasicToken::Read(Istream *in)
{
	int i=0;
	if (in==NULL) in=is;
	if (in==NULL) return 0;
	is=in;
	Text.SetLength(0);
	Text.AutoSize(1);
	while ((ch==' '||ch=='\t'||ch==0)&&ch>=0) getnc();	// 除去空格
	if (ch=='\'') {										// 除去注释
		getnc();
		while (ch>=0&&ch!='\n'&&ch!='\r') getnc();
	}
	if (ch=='\n'||ch=='\r') {							// 一行结束
		getnc();
		Text[0]='\n';
		Text[1]=0;
		Type=ENDL;
		Value=0;
		Line++;
		return 1;
	}
	if (ch==-1) {								// 流结束
		Type=ENDF;
		Value=-1;
		Text[20]=0;
		strcpy(&Text[0],"--EOF--");
		return 0;
	}
	if (ch=='_'||isalpha(ch)) {		// 识别符和关键字
		while (ch=='_'||isalpha(ch)||isalnum(ch)) {
			Text[i++]=ch;
			getnc();
		}
		Text[i++]=0;
		int j;
		for (j=0;j<i;j++) if (Text[j]>='a'&&Text[j]<='z') Text[j]=(Text[j]-'a')+'A';
		if (KeyWord) for (j=0;KeyWord[j];j++) 
			if (strcmp(&Text[0],KeyWord[j])==0) {
				Type=KEYWORD;
				Value=j;
				return 1;
			}
		Type=IDENT;
		Value=0;
		return 1;
	}
	if (ch=='&') {					// 十六近制数字
		getnc();
		if (ch!='H'&&ch!='h') {
			Type=IERROR;
			Value=0;
			return 0;
		}
		getnc();
		while (isalnum(ch)||(ch>='a'&&ch<='f')||(ch>='A'&&ch<='Z')) { Text[i++]=ch; getnc(); }
		Text[i++]=0;
		sscanf(&Text[0],"%lx",&Value);
		Type=INTEGER;
		return 1;
	}
	if (isalnum(ch)) {				// 整数，浮点
		int dot=0;
		while (isalnum(ch)||(dot==0&&ch=='.')) {
			if (ch=='.') dot=1;
			Text[i++]=ch;
			getnc();
		}
		Text[i++]=0;
		if (!dot) {
			sscanf(&Text[0],"%ld",&Value);
			Type=INTEGER;
			return 1;
		}
		sscanf(&Text[0],"%f",&Value);
		Type=FLOAT;
		return 1;
	}
	if (ch=='"') {					// 如果是字串
		getnc();
		while (ch!='\n'&&ch!='\r'&&ch>=0&&ch!='"') { Text[i++]=ch; getnc(); }
		Text[i++]=0;
		if (ch=='"') getnc();
		Type=STRING;
		Value=0;
		return 1;
	}
	Type=OPERATOR;
	Value=ch;
	Text[i++]=ch;
	Text[i++]=0;
	getnc();
	return 1;
}
IStringIdent::IStringIdent()
{
	Count=0;
	Error=0;
	Strings.SetLength(0);
	Strings.AutoSize(1);
}
IStringIdent::~IStringIdent()
{
	Clear();
}
void IStringIdent::Clear()
{
	for (int i=0;i<Strings.GetLength();i++) 
		if (Strings[i].string) delete Strings[i].string;
	Strings.SetLength(0);
	Count=0;
	Error=0;
}
void IStringIdent::Add(char *string,int id)
{
	if (Strings.SetLength(Count+1)) { Error++; return; }
	Strings[Count].string=new char[strlen(string)+1];
	if (!Strings[Count].string) { Error++; return; }
	strcpy(Strings[Count].string,string);
	Strings[Count].id=id;
	Count++;
}

char *IStringIdent::String(int index)
{
	if (index<0||index>=Count) { Error++; return NULL; }
	return Strings[index].string;
}

int IStringIdent::Id(int index)
{
	if (index<0||index>=Count) { Error++; return -1; }
	return Strings[index].id;
}
char *IStringIdent::Id2Str(int id)
{
	for (int i=0;i<Count;i++) if (id==Strings[i].id) return Strings[i].string;
	return NULL;
}
int IStringIdent::Index(int id)
{
	for (int i=0;i<Count;i++) if (id==Strings[i].id) return i;
	return -1;
}
int IStringIdent::GetCount()
{
	return Count;
}
int IStringIdent::Id(char *filename)
{
	for (int i=0;i<Count;i++) if (strcmp(filename,Strings[i].string)==0) return Strings[i].id;
	return -1;
}
int IStringIdent::Index(char *filename)
{
	for (int i=0;i<Count;i++) if (strcmp(filename,Strings[i].string)==0) return i;
	return -1;
}
/////////////////////////////////////////////////////////////////////
int ISExpress::make_node(int value,int left,int right)
{
	if (error) return 0;
	int i=node.Alloc();
	if (node.Error) { error++; return 0; }
	node[i]->value=value;
	node[i]->left=left;
	node[i]->right=right;
	return i;
}

int ISExpress::getnc()
{
	ch=input[p];
	if (input[p]) p++;
	else ch='$';
	return ch;
}

int ISExpress::I()
{
	if (ch>=0x100) {
		int result=ch;
		getnc();
		return make_node(result,0,0);
	}	else {
		iserr<<"expression error\n";
		error++;
	}
	return 0;
}
int ISExpress::BR(int in)
{
	if (ch=='|') {
		getnc();
		return BR(make_node('|',in,BT()));
	}	else
	if (ch=='&') {
		getnc();
		return BR(make_node('&',in,BT()));
	}
	return in;
}
int ISExpress::BP(int in)
{
	if (ch=='=') {
		getnc();
		return BP(make_node('=',in,E()));
	}	else 
	if (ch=='#') {
		getnc();
		return BP(make_node('#',in,E()));
	}	else
	if (ch=='>') {
		getnc();
		return BP(make_node('>',in,E()));
	}	else
	if (ch=='<') {
		getnc();
		return BP(make_node('<',in,E()));
	}	else
	if (ch==']') {
		getnc();
		return BP(make_node(']',in,E()));
	}	else
	if (ch=='[') {
		getnc();
		return BP(make_node('[',in,E()));
	}
	return in;
}
int ISExpress::R(int in)
{
	if (ch=='+') {
		getnc();
		return R(make_node('+',in,T()));
	}	else
	if (ch=='-') {
		getnc();
		return R(make_node('-',in,T()));
	}
	return in;
}
int ISExpress::P(int in)
{
	if (ch=='*') {
		getnc();
		return P(make_node('*',in,F()));
	}	else
	if (ch=='/') {
		getnc();
		return P(make_node('/',in,F()));
	}	else
	if (ch=='%') {
		getnc();
		return P(make_node('%',in,F()));
	}
	return in;
}
int ISExpress::F()
{
	if (ch=='(') {
		int result;
		getnc();
		result=EXP();
		if (ch!=')') { iserr<<"')' not find\r\n"; error++; }
		getnc();
		return result;
	}	else
	if (ch=='-') {
		getnc();
		return make_node('^',0,F());
	}	else
	if (ch=='!') {
		getnc();
		return make_node('!',0,F());
	}
	return I();
}
void ISExpress::walk_tree(int n)
{
	if (!n) return;
	if (node[n]->left) walk_tree(node[n]->left);
	if (node[n]->right) walk_tree(node[n]->right);
	(*output)[op++]=node[n]->value;
}
int ISExpress::express(int *in,IVector<int> &t,int anbool)
{
	Clear();
	t.SetLength(0);
	t.AutoSize(1);
	output=&t;
	input=in;
	p=op=0;
	error=0;
	getnc();
	AN_BOOL=anbool;
	int root=EXP();
	if (ch!='$'||error) return -1;
	walk_tree(root);
	Clear();
	return 0;
}
