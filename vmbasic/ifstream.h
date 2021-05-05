//======================================================================
//
// The extending of class Istream
//
// �����Ļ���Istream�����䶨�壬�̳�ʵ������չ������Ixstream���ļ���
// Ifstream�����ڴ滺������Imstream���Ҷ����˱�׼�������isout/isin
// ��׼���������׼��־���iserr/islog
//
// �����ߣ���ΰ
// �������ڣ�4/4/2002
//
// ΪʲôҪ��չ������ǰIstream�򵥵Ķ�д�ӿڲ�����������������ʹ��
// �����Ixstream�����Istream�������䣺���¶����˶�д�ӿھ��������
// �Կ�����DoRead/DoWrite��Do��ͷ�ĳ�Ա������Ixstreamʵ������ǰ�Ķ�д
// ͨ������Doxxx�ĺ�������ɹ��ܣ������Ͳ�����һ���µĹ��ܣ����İ�
// (Bind Stream)һ��������󶨵�����һ�����ϣ���������������������
// �ض������󶨵��Ǹ����ϣ�����ͨ���󶨹���ʹ����������ͬˮ�������
// ԴԴ���ϵĴ���������Windows�±���ҿ��԰ɱ�׼���isout����islog��
// ����һ��������������ϣ���ǰ�������isout�������ȫ��������һ����
// �ڣ�������Ҫ�ı���ǰ�Ĵ��룬Ҳ���԰��������һ���ļ��������ڵ���
// ĳЩͼ�λ����򣬻��߷������
// ���˰󶨹�����Ixstream��һ����һЩ���õ�ʵ�֣�����<<����������ַ�
// ������VarArg()��ʵ��sprintfʽ�ĸ�ʽ���ַ��������
//
//======================================================================

#ifndef __I_FSTREAM_H__
#define __I_FSTREAM_H__

#include "istream.h"
#include <stdio.h>
#include <stdlib.h>

// ��չ�������ͣ�ԭ����Istream��μ�istream.h����Ķ���
class Ixstream : public Istream
{
protected:
	int Mode;
	Istream *BindStream;			// �󶨵���ָ��
	virtual int DoEof()=0;			// ���������������½ӿ�
	virtual int DoRead(void *buffer,long size)=0;
	virtual int DoWrite(void *buffer,long size)=0;
	virtual int DoSeek(long Pos,int mode)=0;
	virtual int DoGetPos()=0;
	virtual int DoPutCh(int ch)=0;
	virtual int DoGetCh()=0;
public:
	enum { in=1,out=2,append=4,binary=8,null=16,log=32 };
	Ixstream();
	Istream *GetBind();
	int Bind(Istream *stream);		// �󶨵���stream,NULL������
	int Eof();
	int Read(void *buffer,long size);
	int Write(void *buffer,long size);
	int Seek(long Pos,int mode);
	int GetPos();
	int PutCh(int ch);
	int GetCh();
	void VarArg(char *format,...);	// ��ʽ������ַ���
	Ixstream& operator <<(char *s);	// ����ַ���
};

#include "ivector.h"

// �ļ�����ʵ�֣��̳���Ixstream�ķ���
class Ifstream : public Ixstream
{
protected:
	FILE *f;
	int stdio;
	IVector<char> FileName;
	virtual int  DoEof();	// ������������ʵ��
	virtual int  DoRead(void *buffer,long size);
	virtual int  DoWrite(void *buffer,long size);
	virtual int  DoSeek(long pos,int mode);
	virtual int  DoGetPos();
	virtual int  DoGetCh();
	virtual int  DoPutCh(int ch);
public:
	Ifstream(char *filename,int mode);
	Ifstream();
	virtual ~Ifstream();
	virtual int Exist(char *filename);			// �鿴ĳ�ļ��Ƿ����
	virtual int Open(char *filename,int mode);	// ���ļ�������ļ���NULL��򿪱�׼�������
	virtual int Close();						// �ر��ļ�
};

extern Ifstream isout;	// ��׼���
extern Ifstream isin;	// ��׼����
extern Ifstream islog;	// ��׼LOG���
extern Ifstream iserr;	// ��׼����

#include <string.h>

// �ڴ���Imstream������ڴ������Զ�������С��������������д
// �����Ļ��������������������Ĵ�С����Ϊ�õ���IVectorģ��
// ʵ�����Զ����鹦�ܣ��䶨����Ҫ�ο�ivector.h

class Imstream : public Ixstream
{
protected:
	int pos;
	IVector<char> text;
	virtual int  DoEof();
	virtual int  DoRead(void *buffer,long size);
	virtual int  DoWrite(void *buffer,long size);
	virtual int  DoSeek(long pos,int mode);
	virtual int  DoGetPos();
	virtual int  DoGetCh();
	virtual int  DoPutCh(int ch);
public:
	char *Lock();
	void Unlock();
	Imstream();        
	Imstream(char *str);
	Imstream(void *buffer,int len);
	void Clear();
	char& operator[](int i);
};

#endif

