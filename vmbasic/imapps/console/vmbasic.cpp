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
	IVMBetaScript script;  // �����������
	Ifstream fs,os;        // ���������
	printf("input the vm-basic file: ");
	scanf("%s",fin);
	printf("input the ouput file: ");
	scanf("%s",fou);
	if (fs.Open(fin,ISTREAM_READ)) {
		printf("file not find\n");
		return -1;
	}
	os.Open(fou,ISTREAM_WRITE);
	iserr.Bind(&isout);      // ��׼�������󶨵���׼�����
	script.GoScript(fs,os);  // ����
	os.Close();
	fs.Close();
	return 0;
}

