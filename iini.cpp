//======================================================================
//
// iini.cpp
//
// NOTE: INI file operation
// By Lin Wei 15/4/2002
//
//======================================================================


#include "iini.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#ifdef IS_SPACEX
#undef IS_SPACEX
#endif

#define IS_SPACEX(x) ((x==' ')||(x=='\t')||(x=='\r')||(x=='\n'))

int IIni::Startup()
{
	IniText=NULL;
	Items=NULL;
	FileName=NULL;
	TextLen=0;
	ItemNum=0;
	return 0;
}
IIni::IIni()
{
	Startup();
}
IIni::~IIni()
{
	Close();
}
IIni::IIni(char *filename)
{
	Startup();
	Open(filename);
}
int IIni::Save(char *filename)
{
	FILE *f;
	if (!filename) filename=FileName;
	if (filename&&Items&&IniText)
	{
		f=fopen(filename,"w");
		if (f)
		{
			fwrite(IniText,TextLen,1,f);
			fclose(f);
		}
	}
	return 0;
}
int IIni::Close()
{
	if (IniText) delete IniText;
	if (Items) delete Items;
	if (FileName) delete FileName;
	IniText=NULL;
	Items=NULL;
	FileName=NULL;
	TextLen=0;
	ItemNum=0;
	return 0;
}
int IIni::Open(char *filename)
{
	FILE *f;
	int ch,i;

	Close();
	FileName=new char[strlen(filename)+1];
	if (!FileName) return -1;
	strcpy(FileName,filename);
	if ((f=fopen(filename,"r"))==NULL) 
		if ((f=fopen(filename,"w+"))==NULL) {
			delete FileName;
			FileName=NULL;
			return -2;
		}
	fseek(f,0,SEEK_END);
	TextLen=ftell(f);
	fseek(f,0,SEEK_SET);
	IniText=new char[TextLen+10];
	memset(IniText,0,TextLen+10);
	if (!IniText)
	{
		delete FileName;
		FileName=NULL;
		fclose(f);
		return -3;
	}
	for (i=0;!feof(f);) {
		ch=fgetc(f);
		if (ch!='\r'&&ch>=0) IniText[i++]=(char)ch;
	}
	TextLen=i;
	fclose(f);
	if (UpdateItem()) {
		Close();
		return -4;
	}
	return 0;
}
int IIni::SkipMemo(int pos)
{
	
	if (!IniText) return 0;
	while (IS_SPACEX(IniText[pos])&&pos<TextLen) pos++;
	if (IniText[pos]==';') while (IniText[pos]!='\n'&&pos<TextLen) pos++;
	while (IS_SPACEX(IniText[pos])&&pos<TextLen) pos++;

	return pos;
}
int IIni::UpdateItem()
{
	int i,j,count;
	if (!IniText) return -1;
	if (Items) delete Items;
	for (i=0,count=0;i<TextLen;)
	{
		i=SkipMemo(i);
		if (IniText[i]=='[') count++;
		while (IniText[i]!='\n'&&i<TextLen) i++;
	}
	Items=new int[count+1];
	if (!Items) return -1;
	for (i=0,j=0;i<TextLen;)
	{
		i=SkipMemo(i);
		if (IniText[i]=='[') Items[j++]=i;
		while (IniText[i]!='\n'&&i<TextLen) i++;
	}
	ItemNum=count;
	Items[j]=-1;
	return 0;
}

int IIni::FindItem(char *item)
{
	int i,j,k,pos,len=strlen(item);
	if (!Items) return -1;
	for (i=0,k=-1;Items[i]!=-1&&k==-1;i++)
	{
		for (j=0,pos=Items[i]+1+j;IniText[pos]==item[j]&&IniText[pos]!=']'&&pos<TextLen;j++,pos++);
		if (j==len&&IniText[pos]==']') k=i;
	}
	return k;
}

int	IIni::FindData(char *item,char *data,int *slen)
{
	int i,j,k,l,len=strlen(data);
	i=FindItem(item);
	*slen=0;
	if (i<0) return -1;
	i=Items[i];
	while (IniText[i]!='\n'&&i<TextLen) i++;
	for (k=-1;k==-1&&i<TextLen;)
	{
		i=SkipMemo(i);
		if (IniText[i]=='[') break;
		for (j=0;IniText[i]==data[j]&&(IniText[i]!='='&&IniText[i]!=' ')&&(i<TextLen);j++,i++);
		if (j==len&&(IniText[i]==' '||IniText[i]=='=')) k=i;
		if (k>=0) while (IniText[k]!='='&&IniText[k]!='\n'&&i<TextLen) k++,i++;
		for (i=i+1,l=0;IniText[i]!='\n'&&i<TextLen;i++,l++);
	}
	*slen=l;
	return (k>=0)?(k+1):-1;
}

int	IIni::ReadInt(char *item,char *string)
{
	int index,len;
	char *data,*p;
	index=FindData(item,string,&len);
	if (index<0) return -1;
	data=new char[len+1];
	if (!data) return -1;
	memcpy(data,&IniText[index],len);
	data[len]=0;
	p = data;
	if (p[0]=='0'&&(p[1]=='x'||p[1]=='X')) 
		sscanf(p + 2, "%x", &index);
	else 
		index=atoi(data);
	delete data;
	return index;
}

char *IIni::ReadText(char *item,char *string,char *buffer,int len)
{
	static char line[1024];
	int index,tlen;
	if (!buffer||len<1) buffer=line,len=1024;
	len--; line[0]=0;
	index=FindData(item,string,&tlen);
	if (index<0) return buffer;
	len=(len<tlen)?len:tlen;
	memcpy(buffer,&IniText[index],len);
	buffer[len]=0;
	return buffer;
}

int IIni::Write(char *item,char *name,char *string)
{
	int base,size;
	int itempos;
	int len1=strlen(item);
	int len2=strlen(name);
	int len3=strlen(string);
	char *buff;
	if (!IniText) return -1;
	if (FindItem(item)<0)	// new item
	{
		buff=new char[TextLen+len1+10];
		if (!buff) return -1;
		memcpy(buff,IniText,TextLen);
		itempos=TextLen;
		if (itempos>0) if (IniText[itempos-1]=='\n') itempos--;
		sprintf(buff+itempos,"%s[%s]\n",(itempos)?"\n":"",item);
		delete IniText;
		IniText=buff;
		TextLen=itempos+len1+3+((itempos)?1:0);
		if (UpdateItem()) return -2;
	}
	base=FindItem(item);
	if (base<0) return -3;
	itempos=FindData(item,name,&size);
	if (itempos<0)
	{
		if (Items[base+1]!=-1) itempos=Items[base+1]; 
		else itempos=TextLen;
		buff=new char[TextLen+len2+len3+10];
		if (!buff) return -1;
		memcpy(buff,IniText,itempos);
		base=itempos;
		if (itempos>0) if (IniText[itempos-1]=='\n') itempos--;
		sprintf(buff+itempos,"\n%s=%s",name,string);
		memcpy(buff+itempos+len2+len3+2,IniText+itempos,TextLen-base);
		delete IniText;
		IniText=buff;
		TextLen=itempos+len2+len3+2+TextLen-base;
		if (UpdateItem()) return -3;
	}	else {
		buff=new char[TextLen+len3-size+10];
		if (!buff) return -1;
		memcpy(buff,IniText,itempos);
		sprintf(buff+itempos,"%s",string);
		memcpy(buff+itempos+len3,IniText+itempos+size,TextLen-itempos-size);
		delete IniText;
		IniText=buff;
		TextLen=TextLen+len3-size;
		if (UpdateItem()) return -4;
	}
	return 0;
}

int IIni::Write(char *item,char *name,int value)
{
	char line[20];
	sprintf(line,"%d",value);
	return Write(item,name,line);
}

int IIni::ItemCount()
{
	return ItemNum;
}

char* IIni::ItemName(int index, char *buffer, int len)
{
	static char line[1024];
	int pos,i,n;
	if (!buffer||len<1) buffer=line,len=1024;
	len--; line[0]=0;
	if (index<0||index>=ItemNum) return buffer;
	pos=Items[index]+1;
	if (pos<0) return buffer;
	for (i=pos,n=0;IniText[i]!='\n'&&IniText[i]!=']'&&i<TextLen;i++,n++);
	n=(n<len)?n:len;
	memcpy(buffer,IniText+pos,n);
	buffer[n]=0;
	return buffer;
}

char* IIni::StringChop(char *string)
{
	char *p = string;
	if (p == NULL) return NULL;

	while (IS_SPACEX(p[0])) p++;

	int len = strlen(p);
	for (;len > 0 && IS_SPACEX(p[len - 1]);  len--);
	for (int i = 0; i <= len; i++)
		string[i] = p[i];
	string[len] = 0;
	return string;
}

