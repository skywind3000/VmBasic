#include "ivmsbeta.h"
#include <string.h>
#include <stdio.h>

/////////////////////////////////////////////////////////////////////
int IVMBeta::VmCreate(int size,vbyte *data,vdword *VmRegs)
{
	if (VirtualMem) {
		vbyte *nvm=new vbyte[size];
		if (!size) return -1;
		memcpy(nvm,VirtualMem,(VmSize<(unsigned)size)?VmSize:size);
		delete []VirtualMem;
		VirtualMem=nvm;
		VmSize=size;
	} else {
		VirtualMem=new vbyte[size];
		if (!VirtualMem) return -1;
		memset(VirtualMem,0,size);
		VmSize=size;
	}
	VmStrings=new ILister<iString>;
	if (!VmStrings) { VmRelease(); return -1; }
	memset(&VmStartup,0,sizeof(VmStartup));
	if (data)   memcpy(VirtualMem,data,size);
	if (VmRegs) memcpy(&VmStartup,VmRegs,sizeof(VmStartup));
	VmReset();
	return 0;
}

void IVMBeta::VmRelease()
{
	if (VirtualMem) delete []VirtualMem;
	if (VmStrings) delete VmStrings;
	VirtualMem=NULL;
	VmStrings=NULL;
	VmSize=0;
}
void IVMBeta::VmReset()
{ 
	VmRegs=VmStartup;
	VmStatus=0;
	if (!VirtualMem) return;
	if (!VmStrings) return;
	VmStrings->Clear(); 
	VmStrings->Alloc(); 
}
IVMBeta::IVMBeta()
{ 
	VirtualMem=0;
	VmStrings=0;
	VmSize=0;
	VmStatus=0; 
}
IVMBeta::~IVMBeta()
{ 
	VmRelease(); 
}

void IVMBeta::Process()
{
	vbyte opr;
	vbyte mode;
	vbyte *code=VirtualMem;
	vdword ip=VmRegs.RP;
	vdword minsp=0;
	vdword maxsize=VmSize-1;
	vbyte  type1,type2;
	vdword *regs=&VmRegs.RP;
	vbyte  *data1,*data2;
	vbyte  oprlen=0;
	short i;

	opr=code[ip]&0xf0;
	mode=code[ip]&0x0f;
	VmStatus=0;
	switch (opr)
	{
	case 0x00: oprlen=1; break;			/* NOOP OPERATOR */
	case 0x10:							/* LD OPERATOR */
		oprlen=10;
		type1=((code[ip+1]>>2)&3);	type2=(code[ip+1]&3);
		data1=code+ip+2;			data2=data1+4;
		if (type1==0) data1=(vbyte*)&(regs[*data1]);
		else if (type1==1) data1=code+regs[*data1];
		else if (type1==2) { VmError(1001); break; }
		else if (type1==3) data1=code+*(vdword*)data1;
		if (type2==0) data2=(vbyte*)&(regs[*data2]);
		else if (type2==1) data2=code+regs[*data2];
		else if (type2==3) data2=code+*(vdword*)data2;
		if (mode==2) *(vbyte*)data1=*(vbyte*)data2;
		else if (mode==1) *(vword*)data1=*(vword*)data2;
		else *(vdword*)data1=*(vdword*)data2;
		break;
	case 0x20:							/* PUSH OPERATOR */
		oprlen=5;
		data1=code+ip+1;
		if (mode==0x0f) { /* PUSH ALL */
			for (i=0;i<8;i++) {
				if (VmRegs.RS<minsp||VmRegs.RS>=maxsize) { VmError(1002); break; }
				*(vdword*)(code+VmRegs.RS)=regs[i];
				VmRegs.RS-=4;
			}
		} else {
			if (VmRegs.RS<minsp||VmRegs.RS>=maxsize) { VmError(1002); break; }
			if (mode==0) data1=(vbyte*)&(regs[*data1]);
			else if (mode==1) data1=code+regs[*data1];
			else if (mode==3) data1=code+*(vdword*)data1;
			*(vdword*)(code+VmRegs.RS)=*(vdword*)data1;
			VmRegs.RS-=4;
		}
		break;
	case 0x30:							/* POP OPERATOR */
		oprlen=5;
		data1=code+ip+1;
		if (mode==0x0f) { /* POP ALL */
			for (i=7;i>=0;i--) {
				VmRegs.RS+=4;
				if (VmRegs.RS>=maxsize) { VmError(1002); break; }
				regs[i]=*(vdword*)(code+VmRegs.RS);
			}
		} else {
			VmRegs.RS+=4;
			if (VmRegs.RS>=maxsize) { VmError(1002); break; }
			if (mode==0) data1=(vbyte*)&(regs[*data1]);
			else if (mode==1) data1=code+regs[*data1];
			else if (mode==2) { VmError(1001); break; }
			else if (mode==3) data1=code+*(vdword*)data1;
			*(vdword*)data1=*(vdword*)(code+VmRegs.RS);
		}
		break;
	case 0x40:							/* IN OPERATOR */
		oprlen=10;
		type1=((code[ip+1]>>2)&3);	type2=(code[ip+1]&3);
		data1=code+ip+2;			data2=data1+4;
		if (type1==0) data1=(vbyte*)&(regs[*data1]);
		else if (type1==1) data1=code+regs[*data1];
		else if (type1==2) { VmError(1001); break; }
		else if (type1==3) data1=code+*(vdword*)data1;
		if (type2==0) data2=(vbyte*)&(regs[*data2]);
		else if (type2==1) data2=code+regs[*data2];
		else if (type2==3) data2=code+*(vdword*)data2;
		*(vdword*)data1=VmIn(*(vdword*)data2);
		break;
	case 0x50:							/* OUT OPERATOR */
		oprlen=10;
		type1=((code[ip+1]>>2)&3);	type2=(code[ip+1]&3);
		data1=code+ip+2;			data2=data1+4;
		if (type1==0) data1=(vbyte*)&(regs[*data1]);
		else if (type1==1) data1=code+regs[*data1];
		else if (type1==3) data1=code+*(vdword*)data1;
		if (type2==0) data2=(vbyte*)&(regs[*data2]);
		else if (type2==1) data2=code+regs[*data2];
		else if (type2==3) data2=code+*(vdword*)data2;
		VmOut(*(vdword*)data1,*(vdword*)data2);
		break;
	case 0x60:							/* JMP OPERATOR */
		oprlen=5;
		data1=code+ip+1;
		if (mode==0) data1=(vbyte*)&(regs[*data1]);
		else if (mode==1) data1=code+regs[*data1];
		else if (mode==3) data1=code+*(vdword*)data1;
		VmRegs.RP=*(vdword*)data1;
		if (VmRegs.RP>=maxsize) { VmError(1003); break; }
		oprlen=0;
		break;
	case 0x70:							/* JPC OPERATOR */
		oprlen=6;
		data1=code+ip+2;
		if (code[ip+1]==0) data1=(vbyte*)&(regs[*data1]);
		else if (code[ip+1]==1) data1=code+regs[*data1];
		else if (code[ip+1]==3) data1=code+*(vdword*)data1;
		if (VmRegs.RF&(code[ip]&7)) VmRegs.RP=*(vdword*)data1, oprlen=0;
		if (VmRegs.RP>=maxsize) { VmError(1003); break; }
		break;
	case 0x80:							/* CALL OPERATOR */
		oprlen=5;
		data1=code+ip+1;
		if (mode==0) data1=(vbyte*)&(regs[*data1]);
		else if (mode==1) data1=code+regs[*data1];
		else if (mode==3) data1=code+*(vdword*)data1;
		*(vdword*)&code[VmRegs.RS]=VmRegs.RP+oprlen;
		VmRegs.RS-=4;
		if (VmRegs.RS<minsp||VmRegs.RS>=maxsize) VmError(1005);
		VmRegs.RP=*(vdword*)data1;
		if (VmRegs.RP>=maxsize) { VmError(1005); break; }
		oprlen=0;
		break;
	case 0x90:							/* RET OPERATOR */
		oprlen=5;
		VmRegs.RS+=4;
		if (VmRegs.RS>=maxsize) VmError(1006);
		VmRegs.RP=*(vdword*)&code[VmRegs.RS];
		if (VmRegs.RP>=maxsize) { VmError(1007); break; }
		oprlen=0;
		break;
	case 0xA0:							/* CMP OPERATOR */
		oprlen=10;
		i=0;
		type1=((code[ip+1]>>2)&3);	type2=(code[ip+1]&3);
		data1=code+ip+2;			data2=data1+4;
		if (type1==0) data1=(vbyte*)&(regs[*data1]);
		else if (type1==1) data1=code+regs[*data1];
		else if (type1==3) data1=code+*(vdword*)data1;
		if (type2==0) data2=(vbyte*)&(regs[*data2]);
		else if (type2==1) data2=code+regs[*data2];
		else if (type2==3) data2=code+*(vdword*)data2;
		if (mode==0) {
			if (*(vdword*)data1==*(vdword*)data2) i=1;
			if (*(vdword*)data1>*(vdword*)data2) i+=4;
			if (*(vdword*)data1<*(vdword*)data2) i+=2;
		} else if (mode==1) {
			if (*(vword*)data1==*(vword*)data2) i=1;
			if (*(vword*)data1>*(vword*)data2) i+=4;
			if (*(vword*)data1<*(vword*)data2) i+=2;
		} else if (mode==2) {
			if (*(vbyte*)data1==*(vbyte*)data2) i=1;
			if (*(vbyte*)data1>*(vbyte*)data2) i+=4;
			if (*(vbyte*)data1<*(vbyte*)data2) i+=2;
		} else if (mode==3) {
			if (*(vfloat*)data1==*(vfloat*)data2) i=1;
			if (*(vfloat*)data1>*(vfloat*)data2) i+=4;
			if (*(vfloat*)data1<*(vfloat*)data2) i+=2;
		} else if (mode==4) {
			if (*(vint*)data1==*(vint*)data2) i=1;
			if (*(vint*)data1>*(vint*)data2) i+=4;
			if (*(vint*)data1<*(vint*)data2) i+=2;
		}
		VmRegs.RF=(VmRegs.RF&(0xffffffff-7))|i;
		break;
	case 0xB0:							/* CAL OPERATOR */
		oprlen=10;
		i=code[ip+1]>>4;
		type1=((code[ip+1]>>2)&3);	type2=(code[ip+1]&3);
		data1=code+ip+2;			data2=data1+4;
		if (type1==0) data1=(vbyte*)&(regs[*data1]);
		else if (type1==1) data1=code+regs[*data1];
		else if (type1==2) { VmError(1010); break; }
		else if (type1==3) data1=code+*(vdword*)data1;
		if (type2==0) data2=(vbyte*)&(regs[*data2]);
		else if (type2==1) data2=code+regs[*data2];
		else if (type2==3) data2=code+*(vdword*)data2;
		if (mode==0) {
			if (i==0) *(vdword*)data1+=*(vdword*)data2;	else
			if (i==1) *(vdword*)data1-=*(vdword*)data2;	else
			if (i==2) *(vdword*)data1*=*(vdword*)data2; else {
				if (*(vdword*)data2==0) { VmError(1); break; }
				if (i==3) *(vdword*)data1/=*(vdword*)data2; else
				if (i==4) *(vdword*)data1%=*(vdword*)data2;
			}
		} else if (mode==1) {
			if (i==0) *(vword*)data1+=*(vword*)data2; else
			if (i==1) *(vword*)data1-=*(vword*)data2; else
			if (i==2) *(vword*)data1*=*(vword*)data2; else {
				if (*(vword*)data2==0) { VmError(1); break; }
				if (i==3) *(vword*)data1/=*(vword*)data2; 
				if (i==4) *(vword*)data1%=*(vword*)data2;
			}
		} else if (mode==2) {
			if (i==0) *(vbyte*)data1+=*(vbyte*)data2; else
			if (i==1) *(vbyte*)data1-=*(vbyte*)data2; else
			if (i==2) *(vbyte*)data1*=*(vbyte*)data2; else {
				if (*(vbyte*)data2==0) { VmError(1); break; }
				if (i==3) *(vbyte*)data1/=*(vbyte*)data2; else
				if (i==4) *(vbyte*)data1%=*(vbyte*)data2;
			}
		} else if (mode==3) {
			if (i==0) *(vfloat*)data1+=*(vfloat*)data2; else
			if (i==1) *(vfloat*)data1-=*(vfloat*)data2; else
			if (i==2) *(vfloat*)data1*=*(vfloat*)data2; else {
				if (*(vfloat*)data2==0) { VmError(1); break; }
				if (i==3) *(vfloat*)data1/=*(vfloat*)data2; 
				else { VmError(2); break; }
			}
		} else if (mode==4) {
			if (i==0) *(vint*)data1+=*(vint*)data2; else
			if (i==1) *(vint*)data1-=*(vint*)data2; else
			if (i==2) *(vint*)data1*=*(vint*)data2; else {
				if (*(vint*)data2==0) { VmError(1); break; }
				if (i==3) *(vint*)data1/=*(vint*)data2; else 
				if (i==4) *(vint*)data1%=*(vint*)data2;
			}
		}
		break;
	case 0xF0:							/* EXIT OPERATOR */
		oprlen=1;
		VmStatus=1;
		break;
	}
	if (VmRegs.RP==ip) VmRegs.RP+=oprlen;
}

void IVMBeta::Excute()
{
	VmReset();
	while (VmStatus==0) Process();
}
/////////////////////////////////////////////////////////////////////
#include <time.h>
#include <math.h>

vdword IVMBeta::VmIn(vdword port)
{
	long i=VmRegs.R3;
	long j=VmRegs.R2;
	float f;
	char *text,*textp;
	switch (port)
	{
	case 0: i=(int)(*(float*)&VmRegs.R3); break;
	case 1: f=(float)i; i=*(long*)&f; break;
	case 2: i=VmStrings->Alloc()*(-1);(*VmStrings)[-i]->Set(""); break;
	case 3: text=VmIOString(i); if (!text) { VmError(103); break; }
			sscanf(text,"%ld",&i);	break;
	case 4: if (j>=0) { VmError(104); break; }
			(*VmStrings)[-j]->VarArg("%ld",(long)VmRegs.R3);
			break;
	case 5: text=VmIOString(j); if (i>=0||!text) { VmError(105); break; }
			(*VmStrings)[-i]->Set(text); break;
	case 6: text=VmIOString(j); if (i>=0||!text) { VmError(106); break; }
			(*VmStrings)[-i]->Append(text); break;
	case 7: text=VmIOString(i); if (!text) { VmError(107); break; }
			i=strlen(text);	break;
	case 8: VmStrings->Release(-i); break;
	case 9: text=VmIOString(i); textp=VmIOString(j);
			if (text==NULL||textp==NULL) { i=-1; break; }
			i=strcmp(text,textp); break;
	case 0x0a: if (j>=0) { VmError(104); break; }
			   (*VmStrings)[-j]->VarArg("%f",(float)VmRegs.R3);
			   break;
	case 0x0b: text=VmIOString(i); if (!text) { VmError(103); break; }
  			   sscanf(text,"%f",&i); break;
	case 0x0c: text=VmIOString(i); if (text==NULL) { i=-1; break; }
	           i=text[j]; break;
	case 0x0d: text=VmIOString(i); if (text==NULL) { i=-1; break; }
	           if (j<0||j>=(long)strlen(text)) break;
			   text[j]=(char)VmRegs.R1;
			   break;
	case 0x0e: i=0x10000L; break;
	case 0x0f: i=clock()*1000/(long)CLK_TCK; break;
	case 0x10: f=*(float*)&i; f=(float)sin(f);  i=*(long*)&f; break;
	case 0x11: f=*(float*)&i; f=(float)cos(f);  i=*(long*)&f; break;
	case 0x12: f=*(float*)&i; f=(float)tan(f);  i=*(long*)&f; break;
	case 0x13: f=*(float*)&i; f=(float)sqrt(f); i=*(long*)&f; break;
	case 0x14: i=((i>=0)?(i):(-i)); break;
	case 0x15: f=*(float*)&i; f=((f>=0)?(f):(-f)); i=*(long*)&f; break;
	}	
	return i;
}

#include "ifstream.h"
void IVMBeta::VmError(vdword error)
{
	VmStatus=1002;
	iserr.VarArg("[Vm Error]: %d at RP=%lxh\r\n",error,VmRegs.RP);
}
char *IVMBeta::VmIOString(int id)
{
	char *text;
	VmStrings->Error=0;
	if (id==0) return NULL;
	if (id<0) {	
		text=(*VmStrings)[-id]->GetString(); 
		if (VmStrings->Error) return NULL;
	}	else {
		if (id>(int)VmSize) return NULL; 
		text=(char*)VirtualMem+id;
	}
	return text;
}
void IVMBeta::VmOut(vdword port,vdword value)
{
	int ivalue=*(int*)&value;
	static char line[256];

	switch (port)
	{
	case 0x00: isout.VarArg("%ld\r\n",value); break;
	case 0x01: case 2: 
		char *text;	if (ivalue>0) text=(char*)VirtualMem+value;
		else if (ivalue<0) text=(*VmStrings)[-ivalue]->GetString();
		else { VmError(202); break; }
		isout.VarArg("%s%s",text,(port==2)?"":"\r\n"); 
		break;
	case 0x03: isout.VarArg("%ld",value); break;
	case 0x04: isout.VarArg("%c",value); break;
	case 0x05: isout.VarArg("%f",*(float*)&value); break;
	case 0x0A: gets(line); sscanf(line,"%ld",&VmRegs.R3); break;
	case 0x0B: gets(line); (*VmStrings)[-(long)VmRegs.R3]->Set(line); break;
	case 0x0C: gets(line); sscanf(line,"%f",&VmRegs.R3); break;
	}
}

