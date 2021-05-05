//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "AboutForm.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormAbout *FormAbout;
//---------------------------------------------------------------------------
__fastcall TFormAbout::TFormAbout(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TFormAbout::BitBtn1Click(TObject *Sender)
{
    Close();
}
//---------------------------------------------------------------------------



void __fastcall TFormAbout::Label3Click(TObject *Sender)
{
    ShellExecute(this->Handle,"open","http://www.joynb.com","","",SW_SHOWNORMAL);
}
//---------------------------------------------------------------------------

