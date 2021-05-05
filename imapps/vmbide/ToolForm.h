//---------------------------------------------------------------------------

#ifndef ToolFormH
#define ToolFormH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
#include "CGRID.h"
#include <Dialogs.hpp>
//---------------------------------------------------------------------------
class TFormTool : public TForm
{
__published:	// IDE-managed Components
        TGroupBox *GroupBox1;
        TGroupBox *GroupBox2;
        TPanel *Panel1;
        TPanel *Panel2;
        TBitBtn *BitBtn1;
        TBitBtn *BitBtn2;
        TCColorGrid *CColorGrid1;
        TColorDialog *ColorDialog1;
        TOpenDialog *OpenLib;
        TOpenDialog *OpenVmbeta;
        TEdit *Edit1;
        TLabel *Label1;
        TPanel *Panel3;
        TPanel *Panel4;
        TEdit *Edit2;
        TLabel *Label2;
        TBitBtn *BitBtn3;
        TBitBtn *BitBtn4;
        TLabel *Label3;
        TLabel *Label4;
        TEdit *Edit3;
        TEdit *Edit4;
        void __fastcall BitBtn3Click(TObject *Sender);
        void __fastcall BitBtn4Click(TObject *Sender);
        void __fastcall BitBtn2Click(TObject *Sender);
        void __fastcall BitBtn1Click(TObject *Sender);
        void __fastcall CColorGrid1Change(TObject *Sender);
        void __fastcall FormActivate(TObject *Sender);
private:	// User declarations
        void ShowColor();
public:		// User declarations
        AnsiString NameVmbeta;
        AnsiString NameLib;
        TColor Color1;
        TColor Color2;
        __fastcall TFormTool(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormTool *FormTool;
//---------------------------------------------------------------------------
#endif
