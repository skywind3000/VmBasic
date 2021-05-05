#ifndef __I_KEYBOARD_H__
#define __I_KEYBOARD_H__

#ifndef __cplusplus
#error  This file can only be compiled with a C++ compiler !!
#endif

class IKeyBuffer
{
protected:
	int  KeyP1,KeyP2;
	int  Block;
	char KeyBuffer[1024];
	int  KeyStatus[256];
public:
	IKeyBuffer(int n=0)
	{ 
		int i;
		KeyP1=0;
		KeyP2=0;
		Block=(n)?n:32;
		Block=(Block>1024)?Block:1024;
		for (i=0;i<256;i++) KeyStatus[i]=0;
	}
	int GetCh()
	{
		int c;
		if (KeyP1==KeyP2) return -1;
		KeyP2=(KeyP2+1)%Block;
		c=KeyBuffer[KeyP2];
		return c;
	}
	int UnGetCh(char c)
	{
		int newp=(KeyP1+1)%Block;
		if (newp==KeyP2) return -1;
		KeyBuffer[newp]=c;
		KeyP1=newp;
		return 0;
	}
	int Clear() { KeyP1=KeyP2=0; return 0; }
	int Count() { return (KeyP1+Block-KeyP2)%Block; }
	int& operator[](int i) { return KeyStatus[(DWORD)i&255]; }
};


#endif
