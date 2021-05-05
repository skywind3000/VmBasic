//======================================================================
//
// istring.h
//
// NOTE: by Linwei on 3-12-2002
//
//======================================================================



#ifndef __I_STRING_H__
#define __I_STRING_H__

#ifndef __cplusplus
#error  This file can only be compiled with a C++ compiler !!
#endif

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "ivector.h"

class iString
{
protected:
	int StrLen;
	int BlockLength;
	char *StrBuf;
	char Nil;
	void SetupBuf()   { StrLen=0; StrBuf=NULL; BlockLength=64; AdjustLength(1); Nil=0; }
	void ReleaseBuf() { if (StrBuf) delete []StrBuf; StrBuf=NULL; StrLen=0; }
	int  GetBlock(int Length) { return (Length+63)/64; }
	int  AdjustLength(int NewLength) { 
		int BlockCount=GetBlock(StrLen),NewCount=GetBlock(NewLength+3);
		char *NewBuf;
		if (BlockCount==NewCount) { StrLen=NewLength; return 0; }
		NewBuf=new char[BlockLength*NewCount];
		if (!NewBuf) return -1;
		if (StrBuf) {
			strncpy(NewBuf,StrBuf,(StrLen<NewLength)?StrLen:NewLength);
			NewBuf[NewLength]=0;
			delete []StrBuf;
		}
		StrBuf=NewBuf;
		StrLen=NewLength;
		return 0;
	}
protected:
	int StrCpy(char *str,int Len) {
		if (AdjustLength(Len)) return -1;
		memcpy(StrBuf,str,Len);
		StrBuf[Len]=0;
		return 0;
	}
	int StrCat(char *str,int len) {
		int SaveLen=StrLen;
		if (AdjustLength(StrLen+len)) return -1;
		memcpy(StrBuf+SaveLen,str,len);
		StrBuf[StrLen]=0;
		return 0;
	}
	int StrCat(char ch) { char d[2]; d[0]=ch,d[1]=0; return StrCat(d,1); }
	int Replace(int startpos,int endpos,char *s,int len) {
		int NewLength,SaveLen=StrLen,LeftLen;
		if (startpos>StrLen||StrLen==0||endpos>StrLen) return -1;
		NewLength=StrLen-(endpos-startpos)+len;
		LeftLen=StrLen-endpos;
		if (AdjustLength(NewLength)) return -2;
		memmove(StrBuf+startpos+len,StrBuf+endpos,LeftLen);
		memcpy(StrBuf+startpos,s,len);
		StrBuf[NewLength]=0;
		return 0;
	}
	//iString(iString &s1,char *s2,int l) { iString::iString(); StrCpy(s1.GetString(),s1.GetLength()); StrCat(s2,l); }
public:
	virtual ~iString() { ReleaseBuf(); }
	iString() { SetupBuf(); }
	iString(char ch)      { SetupBuf(); Set(ch); }
	iString(char *string) { SetupBuf(); Set(string); }
	iString(iString &s)   { SetupBuf(); Set(s);  }

	int GetLength()   { return StrLen; }
	char *GetString() { static char d[2]; d[0]=0; return (StrBuf)?StrBuf:d; }

	void Set(char ch)    { char d[2]; d[0]=ch; d[1]=0; StrCpy(d,2); }
	void Set(char *s)    { StrCpy(s,strlen(s)); }
	void Set(iString &s) { StrCpy(s.GetString(),s.GetLength()); }
	iString &Append(char ch)    { StrCat(ch); return *this; }
	iString &Append(char *text) { StrCat(text,strlen(text)); return *this; }
	iString &Append(iString &s) { StrCat(s.GetString(),s.GetLength()); return *this; }
	int Insert(int pos,char ch)    { char d[2]; d[0]=ch; d[1]=0; return Replace(pos,pos,d,1); }
	int Insert(int pos,char *s)    { return Replace(pos,pos,s,strlen(s)); }
	int Insert(int pos,iString &s) { return Replace(pos,pos,s.GetString(),s.GetLength()); }

	int operator ==(char *s)   { return (strcmp(StrBuf,s))?0:1; }
	int operator ==(iString&s) { return (strcmp(StrBuf,s.GetString()))?0:1; }
	int operator !=(char *s)   { return (strcmp(StrBuf,s))?1:0; }
	int operator !=(iString&s) { return (strcmp(StrBuf,s.GetString()))?1:0; }
	char& operator[](int id)   { return (id>=0&&id<=StrLen)?StrBuf[id]:Nil; }
	iString& operator=(char ch)    { Set(ch); return *this; }
	iString& operator=(char *s)    { Set(s);  return *this; }
	iString& operator=(iString &s) { Set(s); return *this; }
	iString& operator+=(char ch) { Append(ch); return *this; }
	iString& operator+=(char *s) { Append(s); return *this; }
	iString& operator+=(iString &s) { Append(s); return *this; }

	/****** special functions *******/
	void VarArg(char *format,...) {	char buf[1000];	va_list argptr;	va_start(argptr,format);vsprintf(buf,format,argptr);va_end(argptr);	Set(buf); }
	void ToLowCase() { for (int i=0;i<StrLen;i++) if (StrBuf[i]>='A'&&StrBuf[i]<='Z') StrBuf[i]+=('a'-'A'); }
	void ToHiCase() { for (int i=0;i<StrLen;i++) if (StrBuf[i]>='a'&&StrBuf[i]<='z') StrBuf[i]-=('a'-'A'); }
	void Split(char *Splitters, int SplitterCount, IVector<iString> &Result) { 
		iString line("");
		Result.SetLength(0); 
		for (int i=0;i<StrLen;i++) {
			int ok=1;
			for (int j=0;j<SplitterCount;j++) if(StrBuf[i]==Splitters[j]) ok=0;
			if (ok) line.Append(StrBuf[i]); 
			else {
				Result.SetLength(Result.GetLength()+1);
				Result[Result.GetLength()-1]=line;
				line="";
			}
		}
		Result.SetLength(Result.GetLength()+1);
		Result[Result.GetLength()-1]=line;
	}
};

#endif
