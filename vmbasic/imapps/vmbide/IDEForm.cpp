//---------------------------------------------------------------------------

#include <vcl.h>
#include <stdio.h>
#pragma hdrstop

#include "IDEForm.h"
#include "AsmForm.h"
#include "VmBasic.h"
#include "ToolForm.h"
#include "AboutForm.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormIDE *FormIDE;
//---------------------------------------------------------------------------
__fastcall TFormIDE::TFormIDE(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------


void __fastcall TFormIDE::FormCreate(TObject *Sender)
{
    SaveBasic->DefaultExt="BAS";
    OpenBasic->DefaultExt="BAS";
    OpenVms->DefaultExt="VMS";
    SaveExe->DefaultExt="EXE";
    FileName="";
    isout.Bind(&LogText);
    iserr.Bind(&isout);
    LogText.Startup(VmMsg);
    char line[1024];
    for (int i=0;i<1024;i++) line[i]=0;
    GetCurrentDirectory(1023,line);
    RootDir=line;
    Modified=0;
}
//---------------------------------------------------------------------------
int  TFormIDE::NewFile()
{
    if (Modified) {
        if (MessageBox(GetActiveWindow(),"文件被修改，是否放弃编辑？","提示",
            MB_ICONINFORMATION|MB_YESNO)!=IDYES) return 0;
    }
    FileName="";
    VmText->Lines->Text="";
    StatusBar1->Panels->Items[0]->Text="";
    SetFile(NULL);
    Modified=0;
    ShowPos();
    return 1;
}
void __fastcall TFormIDE::VmTextChange(TObject *Sender)
{
    if (!Modified) {
        Modified=1;
        StatusBar1->Panels->Items[0]->Text="已修改";
    }
}
//---------------------------------------------------------------------------

void __fastcall TFormIDE::FormClose(TObject *Sender, TCloseAction &Action)
{
    if (!NewFile()) Action=caNone;
}
//---------------------------------------------------------------------------
void __fastcall TFormIDE::R2Click(TObject *Sender)
{
    iString name;
    name=FileName.c_str();
    if (name=="") name="UNNAME.BAS";
    if (VmBasic.Compile()==0) return;
    VmBasic.SaveVm(name.GetString());
}
//---------------------------------------------------------------------------



void TFormIDE::ShowPos()
{
    static TPoint op(0,0);
    TPoint p;
    p=VmText->CaretPos;
    if (p.x!=op.x||p.y!=op.y) {
        op=p;
        iString v;
        v.VarArg("[%d:%d]",p.x+1,p.y+1);
        StatusBar1->Panels->Items[1]->Text=v.GetString();
    }
}



void __fastcall TFormIDE::VmTextKeyUp(TObject *Sender, WORD &Key,
      TShiftState Shift)
{
    ShowPos();
    if (Key!=VK_RETURN) return;
    TPoint p=VmText->CaretPos;          // 自动缩进
    int NewLine=p.y,ReLine=p.y-1;
    if (ReLine<0) return;
    iString reline,line;
    reline=VmText->Lines->Strings[ReLine].c_str();
    line=VmText->Lines->Strings[NewLine].c_str();
    if (line!="") return;    
    int i,j;
    for (i=0;i<reline.GetLength()&&reline[i]==' ';i++);
    if (i==reline.GetLength()) i=0;
    if (i==0) return;
    for (j=0;j<i-1;j++) line.Append(' ');
    VmText->Lines->Strings[NewLine]=line.GetString();
    SetCaretPos(i-1,NewLine);
    PostMessage(VmText->Handle,WM_KEYDOWN,' ',0);
    ShowPos();
}
//---------------------------------------------------------------------------

void __fastcall TFormIDE::VmTextMouseUp(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
    ShowPos();
}
//---------------------------------------------------------------------------

void __fastcall TFormIDE::VmTextEnter(TObject *Sender)
{
    ShowPos();
}
//---------------------------------------------------------------------------

void __fastcall TFormIDE::ActionNewExecute(TObject *Sender)
{
    NewFile();
}
//---------------------------------------------------------------------------

void __fastcall TFormIDE::ActionOpenFileExecute(TObject *Sender)
{
    if (OpenBasic->Execute()){
        if (!NewFile()) return;
        FileName=OpenBasic->FileName;
        VmText->Lines->LoadFromFile(FileName);
        SetFile(FileName.c_str());
        StatusBar1->Panels->Items[0]->Text="";
        Modified=0;
    }
}
//---------------------------------------------------------------------------

void __fastcall TFormIDE::ActionSaveFileExecute(TObject *Sender)
{
    if (FileName!="") {
        VmText->Lines->SaveToFile(FileName);
        StatusBar1->Panels->Items[0]->Text="";
        Modified=0;        
    } else {
        ActionSaveAsExecute(NULL);
    }
}
//---------------------------------------------------------------------------

void __fastcall TFormIDE::ActionSaveAsExecute(TObject *Sender)
{
    if (SaveBasic->Execute()){
        FileName=SaveBasic->FileName;
        VmText->Lines->SaveToFile(FileName);
        SetFile(FileName.c_str());
        StatusBar1->Panels->Items[0]->Text="";
        Modified=0;
    }
}
//---------------------------------------------------------------------------
void TFormIDE::SetFile(char *name)
{
    if (name==NULL) FormIDE->Caption="VmBasic IDE";
    else {
        iString text;
        text.VarArg("VmBasic IDE [%s]",name);
        FormIDE->Caption=text.GetString();
    }
}
void __fastcall TFormIDE::ActionRunExecute(TObject *Sender)
{
    iString name;
    name=FileName.c_str();
    if (name=="") name="UNNAME.BAS";
    if (VmBasic.Compile()==0) return;
    VmBasic.SaveVm(name.GetString());
    VmBasic.RunProg(name.GetString());
}
//---------------------------------------------------------------------------

void __fastcall TFormIDE::ActionShowAsmExecute(TObject *Sender)
{
    if (VmBasic.Compile()==0) return;
    TMemoryStream *pms = new TMemoryStream;
    VmBasic.ShowAsm(pms);
    pms->Position=0;
    extern TFormAsm *FormAsm;
    FormAsm->Memo1->Lines->LoadFromStream(pms);
    delete pms;
    FormAsm->Show();
}
//---------------------------------------------------------------------------


void __fastcall TFormIDE::ActionCompileExecute(TObject *Sender)
{
    iString name;
    name=FileName.c_str();
    if (name=="") name="UNNAME.BAS";
    if (VmBasic.Compile()==0) return;
    VmBasic.SaveVm(name.GetString());
}
//---------------------------------------------------------------------------

void __fastcall TFormIDE::EditCopyExecute(TObject *Sender)
{   VmText->CopyToClipboard();
}
//---------------------------------------------------------------------------

void __fastcall TFormIDE::EditCutExecute(TObject *Sender)
{   VmText->CutToClipboard();
}
//---------------------------------------------------------------------------

void __fastcall TFormIDE::EditPasteExecute(TObject *Sender)
{   VmText->PasteFromClipboard();
}
//---------------------------------------------------------------------------

void __fastcall TFormIDE::EditUndoExecute(TObject *Sender)
{   VmText->Undo();
}
//---------------------------------------------------------------------------

void __fastcall TFormIDE::FormActivate(TObject *Sender)
{
    static int flag=0;
    VmText->SetFocus();
    if (flag==0) {
        flag=1;
        VmBasic.hWnd=FormIDE->Handle;
        VmBasic.LoadConfig();
    }
}
//---------------------------------------------------------------------------

void __fastcall TFormIDE::ToolButton9Click(TObject *Sender)
{
    extern TFormAsm *FormAsm;
    FormAsm->Show();
}
//---------------------------------------------------------------------------
void __fastcall TFormIDE::ActionVmsExecute(TObject *Sender)
{
    if (OpenVms->Execute())
        VmBasic.RunVms(OpenVms->FileName.c_str());
}
//---------------------------------------------------------------------------


void __fastcall TFormIDE::ActionSettingExecute(TObject *Sender)
{
    VmBasic.Config();
}
//---------------------------------------------------------------------------

void __fastcall TFormIDE::VmTextKeyPress(TObject *Sender, char &Key)
{
    ShowPos();
}
//---------------------------------------------------------------------------

void __fastcall TFormIDE::VmTextMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
    ShowPos();
}
//---------------------------------------------------------------------------

void __fastcall TFormIDE::ActionHelpExecute(TObject *Sender)
{
    iString name;
    name=RootDir.c_str();
    name.Append("\\Basic.htm");
    ShellExecute(this->Handle,"open",name.GetString(),"",RootDir.c_str(),SW_SHOWNORMAL);
}
//---------------------------------------------------------------------------
void __fastcall TFormIDE::ActionHelpIDEExecute(TObject *Sender)
{
    iString name;
    name=RootDir.c_str();
    name.Append("\\Readme.htm");
    ShellExecute(this->Handle,"open",name.GetString(),"",RootDir.c_str(),SW_SHOWNORMAL);
}
//---------------------------------------------------------------------------
void __fastcall TFormIDE::ActionAboutExecute(TObject *Sender)
{
    extern TFormAbout *FormAbout;
    FormAbout->ShowModal();
}
//---------------------------------------------------------------------------

void __fastcall TFormIDE::ActionSiteExecute(TObject *Sender)
{
    ShellExecute(this->Handle,"open","http://softnb.51.net","","",SW_SHOWNORMAL);
}
//---------------------------------------------------------------------------
void __fastcall TFormIDE::FormDestroy(TObject *Sender)
{
    VmBasic.SaveConfig();
}
//---------------------------------------------------------------------------

void __fastcall TFormIDE::N1Click(TObject *Sender)
{
    PostMessage(this->Handle,WM_CLOSE,0,0);
}
//---------------------------------------------------------------------------

