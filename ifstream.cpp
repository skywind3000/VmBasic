#include "ifstream.h"
#include <string.h>
#include <stdarg.h>

///////////////////////////////////////////////////////////////////////////
Ixstream::Ixstream()
{
        BindStream=0;
        Mode=0;
}

Istream* Ixstream::GetBind()
{
	return BindStream;
}
int Ixstream::Bind(Istream *stream)
{
	BindStream=stream;
	return 0;
}
int Ixstream::Eof()
{
	if (BindStream) return BindStream->Eof();
	return DoEof();
}
int Ixstream::Seek(long Pos,int mode)
{
	if (BindStream) return BindStream->Seek(Pos,mode);
	return DoSeek(Pos,mode);
}
int Ixstream::Read(void *buffer,long size)
{
	if (BindStream) return BindStream->Read(buffer,size);
	return DoRead(buffer,size);
}
int Ixstream::Write(void *buffer,long size)
{
	if (BindStream) return BindStream->Write(buffer,size);
	return DoWrite(buffer,size);
}
int Ixstream::GetCh()
{
	if (BindStream) return BindStream->GetCh();
	return DoGetCh();
}
int Ixstream::PutCh(int c)
{
	if (BindStream) return BindStream->PutCh(c);
	return DoPutCh(c);
}
int Ixstream::GetPos()
{
	if (BindStream) return BindStream->GetPos();
	return DoGetPos();
}
void Ixstream::VarArg(char *format,...)
{
	static char buf[1000];	
	va_list argptr;	
	va_start(argptr,format);
	vsprintf(buf,format,argptr);
	va_end(argptr);	
	Write(buf,strlen(buf));
}
Ixstream& Ixstream::operator<<(char *s)
{
	Write(s,strlen(s));
	return *this;
}
///////////////////////////////////////////////////////////////////////////
int Ifstream::Exist(char *filename)
{
	FILE *fp;
	if ((fp=fopen(filename,"rb"))==NULL) return 0;
	fclose(fp);
	return 1;
}

int Ifstream::Open(char *filename,int mode)
{
	int BINARY=(mode&ISTREAM_BINARY);
	char *m;
	Close();
	if (!filename) { stdio=1; Mode=mode; return 0; }
	if (BINARY) mode^=ISTREAM_BINARY;
	if (mode==ISTREAM_READ) {
		if (BINARY) m="rb"; else m="r";
		f=fopen(filename,m);
	} else if (mode==ISTREAM_WRITE) {
		if (BINARY) m="wb"; else m="w";
		f=fopen(filename,m);
	} else if (mode==(ISTREAM_READ|ISTREAM_WRITE)) {
		if (Exist(filename)) {
			if (BINARY) m="rb+"; else m="r+";
		} else {
			if (BINARY) m="wb+"; else m="w+";
		}
		f=fopen(filename,m);
	} else if (mode==ISTREAM_APPEND) {
		if (BINARY) m="ab+"; else m="a+";
		f=fopen(filename,m);
	} else if (mode==log) {
		FileName.SetLength(0);
		FileName.AutoSize(1);
		for (int i=0;filename[i];i++) FileName[i]=filename[i];
		FileName[FileName.GetLength()]=0;
		FileName.AutoSize(0);
		Mode=mode;
		return 0;
	}
	if (!f) return -1;
	Mode=mode;
	return 0;
}

int Ifstream::Close()
{
	if (f) fclose(f);
	f=NULL;
	stdio=0;
	FileName.SetLength(0);
	FileName.AutoSize(0);
	return 0;
}

Ifstream::Ifstream()
{
	f=NULL;
	stdio=0;
}

Ifstream::Ifstream(char *filename,int mode)
{ 
	f=NULL;
	stdio=0;
	Open(filename,mode); 
}

Ifstream::~Ifstream()
{
	Close();
}

int Ifstream::DoSeek(long pos,int mode)
{
	switch (mode)
	{
	case ISTREAM_START:   mode=SEEK_SET; break;
	case ISTREAM_CURRENT: mode=SEEK_CUR; break;
	case ISTREAM_END:     mode=SEEK_END; break;
	}
	if (!f) return -1;
	return fseek(f,pos,mode);
}

int Ifstream::DoRead(void *buffer,long size)
{
	if (stdio) {
		for (int i=0;i<size;i++) *((char*)buffer+i)=(char)GetCh();
		return size;
	}
	if (!f) return 0;
	return fread(buffer,1,size,f);
}

int Ifstream::DoWrite(void *buffer,long size)
{
	if (stdio) {
		for (int i=0;i<size;i++) DoPutCh(*((char*)buffer+i));
		return size;
	}
	if (Mode==log) {
		FILE *of;
		if ((of = fopen(&FileName[0],"r+"))!=NULL)
		{
			fseek(of,0,SEEK_END);
			fwrite(buffer,1,size,of);
			fclose(of);
		}	else {
			of = fopen(&FileName[0],"w");
			fwrite(buffer,1,size,of);
			fclose(of);
		}
		return size;
	}
	if (!f) return 0;
	return fwrite(buffer,1,size,f);
}

int Ifstream::DoGetCh()
{
	if (stdio) return getchar();
	if (!f) return -1;
	return fgetc(f);
}

int Ifstream::DoPutCh(int ch)
{
	if (stdio) return putchar(ch);
	if (Mode==log) return DoWrite(&ch,1);
	if (!f) return -1;
	return fputc(ch,f);
}

int Ifstream::DoEof()
{
	if (!f) return 1;
	return feof(f);
}

int Ifstream::DoGetPos()
{
	if (!f) return -1;
	return ftell(f);
}

Ifstream isin(NULL,Ixstream::in);
Ifstream isout(NULL,Ixstream::out);
Ifstream islog;
Ifstream iserr;

int Imstream::DoEof()
{
	if (pos>=text.GetLength()) return 1;
	return 0;
}

int Imstream::DoRead(void *buffer,long size)
{
	if (pos>=text.GetLength()) return 0;
	size=(pos+size<=text.GetLength())?size:(text.GetLength()-pos);
	memcpy(buffer,&text[pos],size);
	pos+=size;
	return size;
}
int Imstream::DoWrite(void *buffer,long size)
{
	if (text.SetLength(pos+size)) return 0;
	memcpy(&text[pos],buffer,size);
	pos+=size;
	return size;
}
int Imstream::DoSeek(long p,int mode)
{
	switch (mode)
	{
	case ISTREAM_START:   pos=p; break;
	case ISTREAM_CURRENT: pos+=p; break;
	case ISTREAM_END:     pos=text.GetLength()+p; break;
	}
	return 0;
}
int Imstream::DoGetPos()
{
	return pos;
}
int Imstream::DoGetCh()
{
	if (pos>=text.GetLength()) return -1;
	return text[pos++];
}
int Imstream::DoPutCh(int ch)
{
	text[pos++]=ch;
	return 0;
}
char *Imstream::Lock()
{
	return &text[0];
}
void Imstream::Unlock()
{
}
Imstream::Imstream(char* str)
{
	pos=0;
	text.SetLength(0);
	text.AutoSize(1);
	Write(str, strlen(str));
}
Imstream::Imstream()
{
	pos=0;
	text.SetLength(0);
	text.AutoSize(1);
}
Imstream::Imstream(void *buffer,int len)
{
	pos=0;
	text.SetLength(0);
	text.AutoSize(1);
	Write(buffer,len);
}
void Imstream::Clear()
{
	text.SetLength(0);
	pos=0;
}
char& Imstream::operator[](int i)
{
	return text[i];
}

