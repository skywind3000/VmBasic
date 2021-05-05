#include "VmBasic.h"
#include "IDEForm.h"
#include "ToolForm.h"
#include "AsmForm.h"
#include "..\..\IIni.h"

void IVmBasic::LoadConfig()
{
    IIni IniFile;
    static char line[1025];
    int i;
    for (i=0;i<1024;i++) line[i]=0;
    GetCurrentDirectory(1024,line);
    StartDir=line;
    VmbetaName="VmBeta.Exe";
    extern TFormIDE *FormIDE;
    extern TFormIDE *FormIDE;
    extern TFormAsm *FormAsm;
    TextColor=(int)clTeal;
    AsmColor=(int)clGray;
    line[0]=0;
    if (IniFile.Open("VmBIDE.ini")==0) {
        IniFile.ReadText("IDE","VMBETA",line,1024); VmbetaName=line;
        IniFile.ReadText("IDE","STDLIB",line,1024); VmlibName=line;
        IniFile.ReadText("IDE","TEXTCOLOR",line,1024); sscanf(line,"%d",&TextColor);
        IniFile.ReadText("IDE","ASMCOLOR",line,1024);  sscanf(line,"%d",&AsmColor);
        IniFile.Close();
        if (VmlibName=="")  VmlibName="StdLib.VBL";
        if (VmbetaName=="") VmbetaName="VmBeta.Exe";
        if (TextColor==0) TextColor=clTeal;
        if (AsmColor==0)  AsmColor=clGray;
    }
    FormIDE->VmText->Font->Color=(TColor)TextColor;
    FormAsm->Memo1 ->Font->Color=(TColor)AsmColor;
}
void IVmBasic::Config()
{
    extern TFormTool *FormTool;
    extern TFormIDE *FormIDE;
    extern TFormAsm *FormAsm;
    FormTool->NameVmbeta=VmbetaName.GetString();
    FormTool->NameLib=VmlibName.GetString();
    FormTool->ShowModal();
    VmbetaName=FormTool->NameVmbeta.c_str();
    VmlibName=FormTool->NameLib.c_str();
    TextColor=FormIDE->VmText->Font->Color;
    AsmColor=FormAsm->Memo1->Font->Color;
}
void IVmBasic::SaveConfig()
{
    IIni IniFile;
    SetCurrentDirectory(StartDir.GetString());
    if (IniFile.Open("VmBIDE.ini")==0) {
        IniFile.Write("IDE","VMBETA",VmbetaName.GetString());
        IniFile.Write("IDE","STDLIB",VmlibName.GetString());
        IniFile.Write("IDE","TEXTCOLOR",TextColor);
        IniFile.Write("IDE","ASMCOLOR",AsmColor);        
        IniFile.Save();
        IniFile.Close();
    }
}
int  IVmBasic::Compile()
{
    extern TFormIDE *FormIDE;
    TMemoryStream *pms = new TMemoryStream;

    iserr.GetCh();
    VmBasic.Clear();
    VmCode.Clear();
    iString text1,text2;
    text1="开始编VmBasic程序...";
    text2="";
    Ifstream fstream;
    char line[1024];
    GetCurrentDirectory(1024,line);
    SetCurrentDirectory(StartDir.GetString());
    if (fstream.Open(VmlibName.GetString(),ISTREAM_READ)==0) {
        while (!fstream.Eof()) {
           int ch=fstream.GetCh();
           if (ch!=-1) VmBasic.PutCh(ch);
        }
        fstream.Close();
        VmBasic<<"\nCFLINE 0\n";
        text2.VarArg("预连接标准库%s",VmlibName.GetString());
    } else text2.VarArg("无预连接库文件");
    SetCurrentDirectory(line);
    text1.Append(text2);
    text1.Append("......");
    isout<<text1.GetString();
    FormIDE->VmText->Lines->SaveToStream(pms);
    pms->Position=0;
    VmBasic.Write(pms->Memory,pms->Size);
    VmBasic<<"\n";
    delete pms;
    VmBasic.Seek(0,ISTREAM_START);
    if (VmBSC.GoScript(VmBasic,VmCode)) {
        iserr<<"程序编译成功 ";
    } else {
        iserr<<"试图编译时找到程序中的错误 ";
        return 0;
    }
    return 1;
}
void IVmBasic::ShowAsm(TStream *stream)
{
    VmCode.Seek(0,ISTREAM_START);
    while (!VmCode.Eof()) {
           int ch=VmCode.GetCh();
           if (ch!=-1) stream->Write(&ch,1);
    }
}
void IVmBasic::RunProg(char *FileName)
{
    iString Path,Main,Ext;
    SplitFileName(FileName,Path,Main,Ext);
    iString name;
    name=Path;
    name.Append(Main);
    name.Append(".vms");
    RunVms(name.GetString());
}
void IVmBasic::RunVms(char *VmsFile)
{
    char line[1024];
    int i;
    isout.VarArg("运行程序 %s....",VmsFile);
    for (i=0;i<1024;i++) line[i]=0;
    GetCurrentDirectory(1024,line);
    SetCurrentDirectory(StartDir.GetString());
    ShellExecute(hWnd,"open",VmbetaName.GetString(),VmsFile,".\\",SW_SHOW);
    SetCurrentDirectory(line);
}
void IVmBasic::SaveVm(char *FileName)
{
    iString Path,Main,Ext;
    VmCode.Seek(0,ISTREAM_START);
    SplitFileName(FileName,Path,Main,Ext);
    iString name;
    name=Path;
    name.Append(Main);
    name.Append(".vms");
    Ifstream fstream;
    if (fstream.Open(name.GetString(),ISTREAM_WRITE)) return;
    while (!VmCode.Eof()) fstream.PutCh(VmCode.GetCh());
    fstream.Close();
    isout.VarArg("成功生成文件: %s",name.GetString());
}

void IVmBasic::SplitFileName(char *name,iString &path,iString &main,iString &ext)
{
    int len,i,j,k;
    len=strlen(name);
    for (i=len-1;i>=0;i--) if (name[i]=='\\'||name[i]=='/') break;
    if (i>=0) i=i+1;
    else i=0;
    path="";
    main="";
    ext="";
    for (j=0;j<i;j++) path.Append(name[j]);
    for (j=len-1;j>=i;j--) if (name[j]=='.') break;
    if (j<i) j=len;
    for (k=i;k<j;k++) main.Append(name[k]);
    for (k=j+1;k<len;k++) ext.Append(name[k]);
}


