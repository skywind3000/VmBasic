//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "ToolForm.h"
#include "IDEForm.h"
#include "AsmForm.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "CGRID"
#pragma resource "*.dfm"
TFormTool *FormTool;
//---------------------------------------------------------------------------
__fastcall TFormTool::TFormTool(TComponent* Owner)
        : TForm(Owner)
{
    NameVmbeta="";
    NameLib="";
    ShowColor();
}
void TFormTool::ShowColor()
{
    Edit3->Font->Color=Color1;
    Edit4->Font->Color=Color2;
}
//---------------------------------------------------------------------------
void __fastcall TFormTool::BitBtn3Click(TObject *Sender)
{
    if (OpenVmbeta->Execute()) {
        Edit1->Text=OpenVmbeta->FileName;
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormTool::BitBtn4Click(TObject *Sender)
{
    if (OpenLib->Execute()) {
        Edit2->Text=OpenLib->FileName;
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormTool::BitBtn2Click(TObject *Sender)
{
    Close();        
}
//---------------------------------------------------------------------------
void __fastcall TFormTool::BitBtn1Click(TObject *Sender)
{
    extern TFormIDE *FormIDE;
    extern TFormAsm *FormAsm;
    NameVmbeta=Edit1->Text;
    NameLib=Edit2->Text;
    FormIDE->VmText->Font->Color=Color1;
    FormAsm->Memo1->Font->Color=Color2;
    Close();
}
//---------------------------------------------------------------------------
void __fastcall TFormTool::CColorGrid1Change(TObject *Sender)
{
    Color1=CColorGrid1->ForegroundColor;
    Color2=CColorGrid1->BackgroundColor;
    ShowColor();
}
//---------------------------------------------------------------------------
void __fastcall TFormTool::FormActivate(TObject *Sender)
{
    extern TFormIDE *FormIDE;
    extern TFormAsm *FormAsm;
    Color1=FormIDE->VmText->Font->Color;
    Color2=FormAsm->Memo1->Font->Color;
    Edit1->Text=NameVmbeta;
    Edit2->Text=NameLib;
    ShowColor();
}
//---------------------------------------------------------------------------
