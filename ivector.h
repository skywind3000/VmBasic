//======================================================================
//
// ivector.h
//
// NOTE: This is a copy which is just like template Vector in STL
// By Lin Wei 5/4/2002
//
//======================================================================

#ifndef __IDL_VECTOR_H__
#define __IDL_VECTOR_H__

#ifndef __cplusplus
#error  This file can only be compiled with a C++ compiler !!
#endif

template <class T>
class IVector			// 数组模板
{
protected:
	T *TypeData;
	T Nil;
	int AutoMode;
	unsigned long DataCount;
	unsigned long BlockLen;
public:
	int Error;				// 错误计数器(分配错误,索引错误等)
	IVector();
	virtual ~IVector();
	int SetLength(unsigned long NewLen);
	int GetLength() const;
	int AutoSize(int flag);
	inline T& operator[](unsigned long index);
};


template <class T> IVector<T>::IVector()
{
	TypeData=0;	
	DataCount=0;
	BlockLen=0;
	AutoMode=0;
	Error=0;
}

template <class T> IVector<T>::~IVector()
{ 
	delete []TypeData; 
	TypeData=0;
	DataCount=0; 
	BlockLen=0;
}

template <class T> int IVector<T>::SetLength(unsigned long NewLen)
{
	unsigned long block,i;

	if (NewLen>DataCount&&NewLen<=BlockLen) { DataCount=NewLen; return 0; }

	for (i=0,block=1;i<32;i++,block<<=1) if (block>=NewLen) break;
	if (block==BlockLen) { DataCount=NewLen; return 0; }
	if (BlockLen==0||TypeData==0) {
		TypeData=new T[block];
		if (!TypeData) { Error++; return -1; }
		DataCount=NewLen;
		BlockLen=block;
	} else {
		T *NewData;
		NewData=new T[block];
		if (!NewData) { Error++; return -1; }
		unsigned long min=(DataCount<=NewLen)? DataCount : NewLen;
		for (i=0;i<min;i++) NewData[i]=TypeData[i];
		delete []TypeData;
		TypeData=NewData;
		DataCount=NewLen;
		BlockLen=block;
	}
	return 0;
}

template <class T> int IVector<T>::GetLength() const
{
	return DataCount;
}

template <class T> int IVector<T>::AutoSize(int flag)
{
	AutoMode=flag;
	return 0;
}

template <class T> T& IVector<T>::operator[](unsigned long index)
{
	if (index>=DataCount) {
		if (!AutoMode) { Error++; return Nil; }
		if (SetLength(index+1)) { Error++; return Nil; }
	}
	return TypeData[index];
}

#endif

