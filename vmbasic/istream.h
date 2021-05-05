//======================================================================
//
// The Declaration of Stream
//
// ����������������������IStream�����ṩEof/Read/Write/Seek�Ȼ�������
// ����࣬��û���κ�ʵ�֣�����һ�������࣡��
//
// �����ߣ���ΰ
// ����ʱ�䣺4/4/2002
//
// ���Ľ��ܣ�
// ��ν����������������ʹ���������ļ��ķ�ʽ�������ݣ�ʹ���ҿ��Բ�����
// ���ݴ��������ģ����ﶨ������Istream������ʲô������û��ʵ�֣�Ϊʲ
// ô�أ���ΪҪ������������ʵ�֣�������д��һ������ͼƬ�ĺ��������Ƕ�
// ��ֱ�Ӵ��ļ��ж�ȡ�����Ҫ��ѹ�������ȡ����������һ��sock���Ӷ�ȡ
// ͼƬ��Ϣ����ô��ǰֻ�ܴ��ļ���ȡ�Ĵ��벻��Ҫ����д��ô����ô�Ҹ���
// ô�죿дһ���������࣬�����ļ�����sock����Ȼ���ض������Ķ�д��ʽ��
// ������Eof/Read/Write/Seek�ȼ����麯����Ȼ���һ�������ݸ�ͼƬ��ȡ
// ���򣬳����ͨ��Read/Write�����Ĳ�����������ȡ�����ˣ���ʹ�Ժ�����
// �µ�������Դ��Ҳֻ�����¼̳�һ������Ȼ������������������^_^����
// ������˴���Ĺ�����������
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
