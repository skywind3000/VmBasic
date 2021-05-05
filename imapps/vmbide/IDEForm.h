//---------------------------------------------------------------------------

#ifndef IDEFormH
#define IDEFormH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Menus.hpp>
#include <Grids.hpp>
#include <ExtCtrls.hpp>
#include <ComCtrls.hpp>
#include <ToolWin.hpp>
#include <Buttons.hpp>
#include <ImgList.hpp>
#include <Dialogs.hpp>
#include "VmBasic.h"
#include <ActnList.hpp>
#include <StdActns.hpp>
//---------------------------------------------------------------------------
class TFormIDE : public TForm
{
__published:	// IDE-managed Components
        TMainMenu *MainMenu1;
        TMenuItem *F1;
        TMenuItem *O1;
        TMenuItem *S1;
        TMenuItem *N1;
        TMenuItem *R1;
        TMenuItem *C1;
        TMenuItem *R2;
        TMenuItem *E1;
        TMenuItem *H1;
        TMenuItem *BASICB1;
        TMenuItem *IDEI1;
        TMenuItem *A1;
        TSplitter *Splitter1;
        TToolBar *ToolBar1;
        TSplitter *Splitter2;
        TStatusBar *StatusBar1;
        TPanel *Panel1;
        TTreeView *CmdTree;
        TToolButton *ToolButton1;
        TImageList *ImageList1;
        TToolButton *ToolButton2;
        TToolButton *ToolButton3;
        TToolButton *ToolButton4;
        TToolButton *ToolButton5;
        TMenuItem *A2;
        TOpenDialog *OpenBasic;
        TSaveDialog *SaveBasic;
        TSaveDialog *SaveExe;
        TMenuItem *A3;
        TMenuItem *N2;
        TMenuItem *N3;
        TMenuItem *N4;
        TMenuItem *N5;
        TToolButton *ToolButton6;
        TToolButton *ToolButton8;
        TToolButton *ToolButton9;
        TMenuItem *T1;
        TMenuItem *N6;
        TActionList *ActionList1;
        TEditCopy *EditCopy;
        TEditCut *EditCut;
        TEditDelete *EditDelete;
        TEditPaste *EditPaste;
        TEditSelectAll *EditSelectAll;
        TEditUndo *EditUndo;
        TAction *ActionNew;
        TAction *ActionOpenFile;
        TAction *ActionSaveFile;
        TAction *ActionSaveAs;
        TAction *ActionShowAsm;
        TAction *ActionRun;
        TAction *ActionCompile;
        TMenuItem *E2;
        TMenuItem *Undo;
        TMenuItem *N7;
        TMenuItem *Cut1;
        TMenuItem *Copy1;
        TMenuItem *Paste1;
        TMemo *VmMsg;
        TToolButton *ToolButton10;
        TToolButton *ToolButton11;
        TToolButton *ToolButton12;
        TToolButton *ToolButton13;
        TMenuItem *VMS1;
        TAction *ActionVms;
        TAction *ActionExport;
        TOpenDialog *OpenVms;
        TAction *ActionSetting;
        TAction *ActionHelp;
        TAction *ActionHelpIDE;
        TAction *ActionAbout;
        TAction *ActionSite;
        TMenuItem *S2;
        TToolButton *ToolButton7;
        TRichEdit *VmText;
        void __fastcall FormCreate(TObject *Sender);
        void __fastcall VmTextChange(TObject *Sender);
        void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
        void __fastcall R2Click(TObject *Sender);
        void __fastcall VmTextKeyUp(TObject *Sender, WORD &Key,
          TShiftState Shift);
        void __fastcall VmTextMouseUp(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
        void __fastcall VmTextEnter(TObject *Sender);
        void __fastcall ActionNewExecute(TObject *Sender);
        void __fastcall ActionOpenFileExecute(TObject *Sender);
        void __fastcall ActionSaveFileExecute(TObject *Sender);
        void __fastcall ActionSaveAsExecute(TObject *Sender);
        void __fastcall ActionRunExecute(TObject *Sender);
        void __fastcall ActionShowAsmExecute(TObject *Sender);
        void __fastcall ActionCompileExecute(TObject *Sender);
        void __fastcall EditCopyExecute(TObject *Sender);
        void __fastcall EditCutExecute(TObject *Sender);
        void __fastcall EditPasteExecute(TObject *Sender);
        void __fastcall EditUndoExecute(TObject *Sender);
        void __fastcall FormActivate(TObject *Sender);
        void __fastcall ToolButton9Click(TObject *Sender);
        void __fastcall ActionVmsExecute(TObject *Sender);
        void __fastcall ActionSettingExecute(TObject *Sender);
        void __fastcall VmTextKeyPress(TObject *Sender, char &Key);
        void __fastcall VmTextMouseDown(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y);
        void __fastcall ActionHelpExecute(TObject *Sender);
        void __fastcall ActionHelpIDEExecute(TObject *Sender);
        void __fastcall ActionAboutExecute(TObject *Sender);
        void __fastcall ActionSiteExecute(TObject *Sender);
        void __fastcall FormDestroy(TObject *Sender);
        void __fastcall N1Click(TObject *Sender);
private:	// User declarations
        AnsiString FileName;
        AnsiString RootDir;
        IVmBasic VmBasic;
        ILogStream LogText;
        int  Modified;
        int  NewFile();
        void SetFile(char *name);
        void ShowPos();
public:		// User declarations
        __fastcall TFormIDE(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormIDE *FormIDE;
//---------------------------------------------------------------------------
#endif
