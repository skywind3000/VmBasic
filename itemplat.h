/****************************************************************
 * 通用数据模板：
 * 1。数组模板(IDataArray)：
 *    有时候常常操作数组，如array[index]。模板IDataArray的设计使
 *    得你可以随意地灵活根改数组的大小，象Delphi里面的SetLength
 *    模板会快速重新分配内存并且复制原来的数据。如果你把自动分配
 *    开关打开，模板会根据你访问的数组元素索引值自动根改内存大小
 *    而使你不再有内存超限的顾虑。在释放模板时模板会自动清除占用
 *    的内存，模板支持通用数据和自定义结构，但不支持类，用法举例
 *               IDataArray<int> myarray;   // 模板用int类型
 *               myarray.SetAutoMode(1);    // 打开自动分配开关
 *               myarray[0]=1;              // 模板都自动分配了
 *               myarray[1]=myarray[0]+1;
 * 2。分配列表模板(ILister)
 */

#ifndef __I_TEMPLATE_H__
#define __I_TEMPLATE_H__

#ifndef __cplusplus
#error  This file can only be compiled with a C++ compiler !!
#endif

template <class ClassType>
class IDataArray			// 数组模板
{
protected:
	ClassType *TypeData;
	ClassType Nil;
	int DataCount;
	int AutoMode;
public:
	int Error;				// 错误计数器(分配错误,索引错误等)
	virtual ~IDataArray();
	IDataArray();
	IDataArray(int InitSize);
	int SetLength(int l);
	int GetLength();
	int SetAutoAlloc(int flag);
	ClassType& operator[](int index);
};


template <class ClassType>
class ILister
{
protected:
	struct TypeRow
	{
		ClassType **data;
		int ColCount;
	};
	TypeRow *TypeRows;
	ClassType Nil;
	int RowCount;
	int AddRow();
public:
	int Error;
	ILister();
	virtual ~ILister();
	int Alloc();
	int Release(int handle);
	void Clear();
	ClassType* operator[](int handle);
};


template <class ClassType> IDataArray<ClassType>::IDataArray()
{
	TypeData=0; 
	DataCount=0;
	AutoMode=0;
	Error=0;
}

template <class ClassType> IDataArray<ClassType>::IDataArray(int InitSize)
{
	IDataArray::IDataArray();
	SetLength(InitSize);
}

template <class ClassType> IDataArray<ClassType>::~IDataArray()
{ 
	delete []TypeData; 
	TypeData=0;
	DataCount=0; 
}
template <class ClassType> int IDataArray<ClassType>::SetLength(int NewLen)
{
	int block1=(DataCount+127)>>7,block2=(NewLen+127)>>7; 

	if (block1==block2) { DataCount=NewLen; return 0; }
	if (block1==0||TypeData==0) {
		TypeData=new ClassType[block2<<7];
		if (!TypeData) { Error++; return -1; }
		DataCount=NewLen;
	} else {
		ClassType *NewData;
		NewData=new ClassType[block2<<7];
		if (!NewData) { Error++; return -1; }
		int min=(DataCount<=NewLen)? DataCount : NewLen;
		for (int i=0;i<min;i++) NewData[i] = TypeData[i];
		delete []TypeData;
		TypeData=NewData;
		DataCount=NewLen;
	}
	return 0;
}
template <class ClassType> int IDataArray<ClassType>::GetLength()
{
	return DataCount;
}
template <class ClassType> int IDataArray<ClassType>::SetAutoAlloc(int flag)
{
	AutoMode=flag;
	return 0;
}
template <class ClassType> ClassType& IDataArray<ClassType>::operator[](int index)
{
	if (index<0) { Error++; return Nil; }
	if (index>=DataCount) {
		if (!AutoMode) { Error++; return Nil; }
		if (SetLength(index+1)) { Error++; return Nil; }
	}
	return TypeData[index];
}

template <class ClassType> ILister<ClassType>::ILister()
{ 
	RowCount=0; 
	TypeRows=0; 
	Error=0;
}

template <class ClassType> int ILister<ClassType>::AddRow()
{
	int NewIndex=0;
	if (!TypeRows) {
		TypeRows=new TypeRow;
		if (TypeRows==0) return -1;
		NewIndex=0;
	}  else {
		TypeRow *NewRows=new TypeRow[RowCount+1];
		if (!NewRows) return -1;
		for (int i=0;i<RowCount;i++) NewRows[i] = TypeRows[i];
		delete []TypeRows;
		TypeRows=NewRows;
		NewIndex=RowCount;
	}
	TypeRows[NewIndex].ColCount=0;
	TypeRows[NewIndex].data=new ClassType*[64];
	if (!TypeRows[NewIndex].data) return -2;
	for (int i=0;i<64;i++) TypeRows[NewIndex].data[i]=0;
	RowCount++;
	return 0;
}

template <class ClassType> int ILister<ClassType>::Alloc()
{
	int line,i;
	for (line=0;line<RowCount;line++) if (TypeRows[line].ColCount<64) break;
	if (line==RowCount) if (AddRow()) { Error++; return 0; }
	for (i=0;i<64;i++) if (TypeRows[line].data[i]==0) break;
	if (i==64) { Error++; return 0; }
	TypeRows[line].data[i]=new ClassType;
	if (!TypeRows[line].data[i]) { Error++; return 0; }
	TypeRows[line].ColCount++;
	return line*64+i;
}

template <class ClassType> int ILister<ClassType>::Release(int handle)
{
	int line=handle>>6;
	int index=handle&63;
	if (line<0||line>=RowCount) return -1;
	if (TypeRows[line].data[index]==0) return -2;
	delete TypeRows[line].data[index];
	TypeRows[line].data[index]=0;
	TypeRows[line].ColCount--;
	return 0;
}

template <class ClassType> ClassType* ILister<ClassType>::operator[](int handle)
{
	int line=handle>>6;
	int index=handle&63;
	if (line<0||line>=RowCount) { Error++; return &Nil; }
	if (index<0||TypeRows[line].data[index]==0) { Error++; return &Nil; }
	return TypeRows[line].data[index];
}

template <class ClassType> ILister<ClassType>::~ILister()
{
	for (int line=0;line<RowCount;line++) {
		for (int i=0;i<64;i++) 
			if (TypeRows[line].data[i]) delete TypeRows[line].data[i];
		delete []TypeRows[line].data;
	}
	if (TypeRows) delete []TypeRows;
	TypeRows=0;
	RowCount=0;
}

template <class ClassType> void ILister<ClassType>::Clear()
{
	for (int line=0;line<RowCount;line++) {
		for (int i=0;i<64;i++) 
			if (TypeRows[line].data[i]) { 
				delete TypeRows[line].data[i]; 
				TypeRows[line].data[i]=0;
			}
		TypeRows[line].ColCount=0;
	}
	Error=0;
}


#endif

