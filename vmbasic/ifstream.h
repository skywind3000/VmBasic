//======================================================================
//
// The extending of class Istream
//
// 对流的基类Istream的扩充定义，继承实现了扩展流类型Ixstream和文件流
// Ifstream还有内存缓冲区流Imstream并且定义了标准输入输出isout/isin
// 标准错误输出标准日志输出iserr/islog
//
// 创建者：林伟
// 创建日期：4/4/2002
//
// 为什么要扩展流？以前Istream简单的读写接口并不能满足灵活的数据使用
// 因此在Ixstream里面对Istream做了扩充：重新定义了读写接口就是下面可
// 以看到的DoRead/DoWrite等Do开头的成员函数，Ixstream实现了以前的读写
// 通过调用Doxxx的函数来完成功能，这样就产生了一个新的功能：流的绑定
// (Bind Stream)一个流如果绑定到另外一个流上，对这个流的输入输出将被
// 重定向到它绑定的那个流上，这样通过绑定功能使数据真正如同水流般得以
// 源源不断的传输诸如在Windows下编程我可以吧标准输出isout或者islog绑
// 定到一个窗口输出的流上，以前代码调用isout的输出将全部被导向一个窗
// 口，而不需要改变以前的代码，也可以吧输出捆绑到一个文件流，便于调试
// 某些图形化程序，或者服务程序。
// 除了绑定功能外Ixstream还一共了一些有用的实现，比如<<符号来输出字符
// 串，而VarArg()来实现sprintf式的格式化字符串输出。
//
//======================================================================

#ifndef __I_FSTREAM_H__
#define __I_FSTREAM_H__

#include "istream.h"
#include <stdio.h>
#include <stdlib.h>

// 扩展流的类型，原类型Istream请参见istream.h里面的定义
class Ixstream : public Istream
{
protected:
	int Mode;
	Istream *BindStream;			// 绑定的流指针
	virtual int DoEof()=0;			// 下面是输出输入的新接口
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
	int Bind(Istream *stream);		// 绑定到流stream,NULL代表不绑定
	int Eof();
	int Read(void *buffer,long size);
	int Write(void *buffer,long size);
	int Seek(long Pos,int mode);
	int GetPos();
	int PutCh(int ch);
	int GetCh();
	void VarArg(char *format,...);	// 格式化输出字符串
	Ixstream& operator <<(char *s);	// 输出字符串
};

#include "ivector.h"

// 文件流的实现，继承了Ixstream的方法
class Ifstream : public Ixstream
{
protected:
	FILE *f;
	int stdio;
	IVector<char> FileName;
	virtual int  DoEof();	// 具体输出输入的实现
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
	virtual int Exist(char *filename);			// 查看某文件是否存在
	virtual int Open(char *filename,int mode);	// 打开文件，如果文件名NULL则打开标准输出输入
	virtual int Close();						// 关闭文件
};

extern Ifstream isout;	// 标准输出
extern Ifstream isin;	// 标准输入
extern Ifstream islog;	// 标准LOG输出
extern Ifstream iserr;	// 标准错误

#include <string.h>

// 内存流Imstream这里的内存流会自动调整大小，比如你向里面写
// 东西的话它会自增加整缓冲区的大小，因为用到了IVector模板
// 实现了自动数组功能，其定义需要参考ivector.h

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

