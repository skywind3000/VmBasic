//---------------------------------------------------------------------------

#ifndef AsmFormH
#define AsmFormH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
//---------------------------------------------------------------------------
class TFormAsm : public TForm
{
__published:	// IDE-managed Components
        TRichEdit *Memo1;
private:	// User declarations
public:		// User declarations
        __fastcall TFormAsm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormAsm *FormAsm;
//---------------------------------------------------------------------------
#endif
