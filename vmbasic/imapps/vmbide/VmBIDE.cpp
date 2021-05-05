//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
USERES("VmBIDE.res");
USEFORM("IDEForm.cpp", FormIDE);
USEUNIT("..\..\ivmscv.cpp");
USEUNIT("..\..\ifstream.cpp");
USEUNIT("..\..\iini.cpp");
USEUNIT("..\..\iplatfrm.cpp");
USEUNIT("..\..\ivmscexp.cpp");
USEUNIT("..\..\ivmscs.cpp");
USEUNIT("..\..\iacciden.cpp");
USEUNIT("VmBasic.cpp");
USEFORM("AsmForm.cpp", FormAsm);
USEFORM("ToolForm.cpp", FormTool);
USEFORM("AboutForm.cpp", FormAbout);
//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
        try
        {
                 Application->Initialize();
                 Application->Title = "VmBasic IDE";
                 Application->CreateForm(__classid(TFormIDE), &FormIDE);
                 Application->CreateForm(__classid(TFormAsm), &FormAsm);
                 Application->CreateForm(__classid(TFormTool), &FormTool);
                 Application->CreateForm(__classid(TFormAbout), &FormAbout);
                 Application->Run();
        }
        catch (Exception &exception)
        {
                 Application->ShowException(&exception);
        }
        return 0;
}
//---------------------------------------------------------------------------
