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
	IVMBetaDebug vmbeta;   // �����������
	IVMBetaAsm vmasm;      // ���������ȡ��
	iserr.Bind(&isout);    // ��׼�������󶨵���׼�����
	if (argc==1) {
		isout<<"VmBeta version 1.0 Copyright 1997-2002 newbird game studio\n";
		isout<<"Syntax is: vmbeta filename\n";
		return 1;
	}
	int result;
	result=vmasm.LoadAsm(vmbeta,argv[1]);  // ��������ɶ����ƴ���
	if (result<0) {
		iserr.VarArg("%s\n",vmasm.ErrorCode());
		iserr.VarArg("load %s failed %d\n",argv[1],result);
		return -2;
	}
	if (argc>2) vmbeta.Debug();           // �Ƿ���ҪDebugģʽ
	else vmbeta.Excute();                 // ����ֱ��ִ��
	return 0;
}

