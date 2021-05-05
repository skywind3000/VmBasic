#if defined(__unix)
#include "../../ivmsbeta.cpp"
#include "../../ivmsca.cpp"
#include "../../ifstream.cpp"
#include "../../iacciden.cpp"
#else
#include "..\..\ivmsbeta.cpp"
#include "..\..\ivmsca.cpp"
#include "..\..\ifstream.cpp"
#include "..\..\iacciden.cpp"
#endif


int main(int argc,char *argv[])
{
	IVMBetaDebug vmbeta;   // 虚拟机调试类
	IVMBetaAsm vmasm;      // 虚拟机汇编读取类
	iserr.Bind(&isout);    // 标准错误流绑定到标准输出流
	if (argc==1) {
		isout<<"VmBeta version 1.0 Copyright 1997-2002 newbird game studio\n";
		isout<<"Syntax is: vmbeta filename\n";
		return 1;
	}
	int result;
	result=vmasm.LoadAsm(vmbeta,argv[1]);  // 将汇编读入成二进制代码
	if (result<0) {
		iserr.VarArg("%s\n",vmasm.ErrorCode());
		iserr.VarArg("load %s failed %d\n",argv[1],result);
		return -2;
	}
	if (argc>2) vmbeta.Debug();           // 是否需要Debug模式
	else vmbeta.Excute();                 // 否则直接执行
	return 0;
}

