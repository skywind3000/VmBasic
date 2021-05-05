#define VMBASIC

#if defined(__unix)
#include "../../iacciden.cpp"
#include "../../ifstream.cpp"
#include "../../ivmscexp.cpp"
#include "../../ivmscs.cpp"
#include "../../ivmscv.cpp"
#else
#include "..\..\iacciden.cpp"
#include "..\..\ifstream.cpp"
#include "..\..\ivmscexp.cpp"
#include "..\..\ivmscs.cpp"
#include "..\..\ivmscv.cpp"
#endif

int main()
{
	char fin[100];
	char fou[100];
	IVMBetaScript script;  // 虚拟机编译类
	Ifstream fs,os;        // 输出输入流
	printf("input the vm-basic file: ");
	scanf("%s",fin);
	printf("input the ouput file: ");
	scanf("%s",fou);
	if (fs.Open(fin,ISTREAM_READ)) {
		printf("file not find\n");
		return -1;
	}
	os.Open(fou,ISTREAM_WRITE);
	iserr.Bind(&isout);      // 标准错误流绑定到标准输出流
	script.GoScript(fs,os);  // 编译
	os.Close();
	fs.Close();
	return 0;
}

