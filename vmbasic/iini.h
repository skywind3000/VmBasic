//======================================================================
//
// iini.h
//
// NOTE: INI file operation
// By Lin Wei 15/4/2002
//
//======================================================================

#ifndef __I_LWINDOWS_H__
#define __I_LWINDOWS_H__

#ifndef __cplusplus
#error  This file can only be compiled with a C++ compiler !!
#endif

class IIni
{
private:
	char	*IniText;
	int		TextLen;
	int		ItemNum;
	int		*Items;
	char	*FileName;
	int		SkipMemo(int pos);
	int		UpdateItem();
	int		FindItem(char *item);
	int		FindData(char *item,char *name,int *len);
	int		Startup();

public:
	~IIni();
	IIni();
	IIni(char *filename);
	int  Open(char *filename);
	int  Close();
	int  Save(char *filename=0);

	int  ItemCount();
	char*ItemName(int index, char *text, int len);

	int	 ReadInt(char *section,char *name);
	char*ReadText(char *section,char *name,char *buffer,int len);
	int Write(char *section,char *name,char *string);
	int Write(char *section,char *name,int value);

	char*StringChop(char *string);
};

#endif
