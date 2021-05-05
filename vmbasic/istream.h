//======================================================================
//
// The Declaration of Stream
//
// 对流的申明，申明基本类IStream并且提供Eof/Read/Write/Seek等基本申明
// 这个类，并没有任何实现，属于一个基础类！！
//
// 创建者：林伟
// 创建时间：4/4/2002
//
// 流的介绍：
// 所谓流，就是数据流。使程序用象文件的方式操作数据，使得我可以不关心
// 数据从哪里来的，这里定义了流Istream。但是什么方法都没有实现，为什
// 么呢？因为要靠它的子类来实现，比如我写了一个读入图片的函数，但是都
// 是直接从文件中读取，如果要从压缩包里读取，从网络上一个sock连接读取
// 图片信息，那么以前只能从文件读取的代码不就要重新写了么？那么我该怎
// 么办？写一个流的子类，比如文件流，sock流，然后重定义流的读写方式就
// 是重载Eof/Read/Write/Seek等几个虚函数。然后把一个流传递给图片读取
// 程序，程序就通过Read/Write等流的操作函数来读取数据了，即使以后有了
// 新的数据来源，也只需重新继承一个流。然后把这个新流传给程序^_^。大
// 大提高了代码的工作能力！！
//
//======================================================================



#ifndef __I_STREAM_H__
#define __I_STREAM_H__

#ifndef __cplusplus
#error  This file can only be compiled with a C++ compiler !!
#endif

#define ISTREAM_READ	1
#define ISTREAM_WRITE	2
#define ISTREAM_APPEND	4
#define ISTREAM_BINARY  8

#define ISTREAM_START	0
#define ISTREAM_CURRENT 1
#define ISTREAM_END     2

class Istream
{
public:
	virtual int  Eof()=0;
	virtual int  Read(void *buffer,long size)=0;
	virtual int  Write(void *buffer,long size)=0;
	virtual int  Seek(long pos,int mode)=0;
	virtual int  GetPos()=0;
	virtual int  GetCh()=0;
	virtual int  PutCh(int ch)=0;
};


#endif
