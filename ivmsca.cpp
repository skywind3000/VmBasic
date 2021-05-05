#include "ivmsca.h"

IVMBetaAsm::IVMBetaAsm()
{
 	int i;
	char *keywords[]={"rp","rf","rs","rb","r0","r1","r2","r3","dword","word","byte","float","int",0};
	StackSize=1000;
	Code.AutoSize(1);
	Code.SetLength(0);
	for (i=0;keywords[i];i++) KeyWords[i]=keywords[i];
	KeyWords[i]=0;
}

int IVMBetaAsm::GetSymbol(int IdentP)
{
	long v_i=1;
	float v_f=1;
	SymError=0;
	SymIdent[0]=0;
	if (IdentReader[IdentP]->Type==IIdentFinder::KeyWord) {
		if (IdentReader[IdentP]->Value<8) { SymType=SymReg, SymData=IdentReader[IdentP]->Value; return 1; }
		else SymError=1;
	}	else if (IdentReader[IdentP]->Type==IIdentFinder::Operator&&
				 IdentReader[IdentP]->Value=='-') {
		if (IdentReader[IdentP+1]->Type==(IIdentFinder::Integer)||
			IdentReader[IdentP+1]->Type==(IIdentFinder::Float)) {
			SymType=SymImm;
			if (SymPreType!=Float) v_i=IdentReader[IdentP+1]->iValue*(-1), SymData=v_i;
			else v_f=IdentReader[IdentP+1]->fValue*(-1), SymData=*((long*)(&v_f));
			return 2;
		}	else SymError=2;
	}	else if (IdentReader[IdentP]->Type==IIdentFinder::Integer||IdentReader[IdentP]->Type==IIdentFinder::Float) {
		SymType=SymImm;
		if (SymPreType!=Float) v_i=IdentReader[IdentP]->iValue, SymData=v_i;
		else v_f=IdentReader[IdentP]->fValue, SymData=*((long*)(&v_f));
		return 1;
	}	else if (IdentReader[IdentP]->Type==IIdentFinder::Operator&&
				 IdentReader[IdentP]->Value=='[') {
		if (IdentReader[IdentP+1]->Type==IIdentFinder::KeyWord &&
			IdentReader[IdentP+1]->Value<8&&IdentReader[IdentP+2]->Value==']') {
			SymType=SymRegPoint;
			SymData=IdentReader[IdentP+1]->Value;
			return 3;
		}	else if (IdentReader[IdentP+1]->Type==IIdentFinder::Integer) {
			SymType=SymImmPoint;
			SymData=IdentReader[IdentP+1]->iValue;
			return 3;
		}	else if (IdentReader[IdentP+1]->Type==IIdentFinder::Ident) {
			SymType=SymImmPoint;
			SymData=0;
			strcpy(SymIdent,IdentReader[IdentP+1]->sValue);
			return 3;
		}
	}	else if (IdentReader[IdentP]->Type==IIdentFinder::Ident) {
		SymType=SymImm;
		SymData=0;
		strcpy(SymIdent,IdentReader[IdentP]->sValue);
		return 1;
	}
	SymError=10;	
	return 0;
}
void IVMBetaAsm::SetDWord(int ip,vdword value)
{
	Code[ip+0]=(vbyte)(value&255); value>>=8; 
	Code[ip+1]=(vbyte)(value&255); value>>=8;
	Code[ip+2]=(vbyte)(value&255); value>>=8; 
	Code[ip+3]=(vbyte)value;
}

int IVMBetaAsm::MakeOneSymbol(int ip,int IdentP)
{
	ErrorMsg[0]=0;
	SymPreType=DWord;
	GetSymbol(IdentP);
	if (SymError) {	sprintf(ErrorMsg,"get symbol error"); return -1; }
	SetDWord(ip+1,SymData);
	if (SymIdent[0]) UseIdent.Add(SymIdent,ip+1);
	return 0;
}
int IVMBetaAsm::MakeTwoSymbol(int ip,int IdentP)
{
	ErrorMsg[0]=0;
	SymPreType=DWord;
	IdentP+=GetSymbol(IdentP)+1;
	if (SymError) {	sprintf(ErrorMsg,"get first symbol error"); return -1; }
	SetDWord(ip+2,SymData);
	Code[ip+1]=(vbyte)(SymType*4);
	if (SymIdent[0]) UseIdent.Add(SymIdent,ip+2);

	GetSymbol(IdentP);
	if (SymError) {	sprintf(ErrorMsg,"get second symbol error"); return -1; }
	SetDWord(ip+6,SymData);
	Code[ip+1]+=(vbyte)SymType;
	if (SymIdent[0]) UseIdent.Add(SymIdent,ip+6);

	return 0;
}
int IVMBetaAsm::LoadAsm(IVMBeta &NewVM,char *filename)
{
	Ifstream fs;
	if (fs.Open(filename,ISTREAM_READ)) return -1;
	int r=LoadAsm(NewVM,fs);
	fs.Close();
	return r;
}
int IVMBetaAsm::LoadAsm(IVMBeta &NewVM,Istream &stream)
{
	int  ip=0;
	int  i=0,j=0,c=0,k=0;
	int  LinePoint=1,LoadOK;
	char AsmError[256];
	Code.SetLength(0);
	StackSize=1000;
	UseIdent.Clear();
	LabelIdent.Clear();
 	for (LoadOK=0;(stream.Eof()==0&&!LoadOK);LinePoint++)
	{
		iString Line;	// 读取一行代码
		Line="";
		for (c=stream.GetCh();c&&c!='\n'&&c!=-1&&!stream.Eof();c=stream.GetCh()) Line.Append(c);
		IdentReader.FindIdent(Line.GetString(),KeyWords); // 对这行代码词法分析
		if (IdentReader.GetCount())		// 本行有代码
		{
			iString AsmCmd,CmdDat;
			int Ident=0,ResetIdent=1;
			if (ResetIdent) Ident=0;
			ResetIdent=1;	
			AsmError[0]=0;
			SymPreType=DWord;
			AsmCmd.Set(IdentReader[Ident]->sValue);
			CmdDat.Set(IdentReader[Ident+1]->sValue);
			for (i=0;i<AsmCmd.GetLength();i++) if (AsmCmd[i]>='a'&&AsmCmd[i]<='z') AsmCmd[i]=AsmCmd[i]-'a'+'A';
			for (i=0;i<CmdDat.GetLength();i++) if (CmdDat[i]>='a'&&CmdDat[i]<='z') CmdDat[i]=CmdDat[i]-'a'+'A';
			if (CmdDat==":") {			// 地址标识
				int FindMatch;
				for (i=0,FindMatch=0;i<LabelIdent.GetCount();i++) if (strcmp(LabelIdent.String(i),IdentReader[Ident]->sValue)==0) FindMatch=1;
				if (FindMatch) {
					sprintf(AsmError,"Error (%d) label had already been defined !!",LinePoint);
					LoadOK=-1;
					break;
				}
				LabelIdent.Add(IdentReader[Ident]->sValue,ip);
				ResetIdent=0;
			}	else
			if (AsmCmd==".") {			// 预处理指令
				if (CmdDat=="STACK") { if (IdentReader[Ident+2]->iValue>StackSize) StackSize=IdentReader[Ident+2]->iValue; }
				else if (CmdDat=="BLOCK") for (int i=0;i<IdentReader[Ident+2]->iValue;i++) Code[ip++]=(vbyte)IdentReader[Ident+3]->iValue;
				else if (CmdDat=="LINE") { LinePoint=IdentReader[Ident+2]->iValue; }
			}	else 
			if (AsmCmd=="EXIT") Code[ip++]=(vbyte)0xf0;	else 
			if (AsmCmd=="NOOP") Code[ip++]=(vbyte)0x00;	else
			if (AsmCmd=="END")  LoadOK=1;				else
			if (AsmCmd=="DATA") {
				int FindMatch;
				for (i=0,FindMatch=0;i<LabelIdent.GetCount();i++) if (strcmp(LabelIdent.String(i),IdentReader[Ident+1]->sValue)==0) FindMatch=1;
				if (FindMatch) {
					sprintf(AsmError,"Error (%d) variable had already been defined !!",LinePoint);
					LoadOK=-1;
					break;
				}
				LabelIdent.Add(IdentReader[Ident+1]->sValue,ip);
				SymPreType=IdentReader[Ident+2]->Value-8;
				int   iValue;
				float fValue;
			
				for (i=3;i+Ident<IdentReader.GetCount();i++) {
					iValue=1;	fValue=1;
					if (IdentReader[Ident+i]->Type==5&&IdentReader[Ident+i]->Value==','); else
					if (IdentReader[Ident+i]->Type==5&&IdentReader[Ident+i]->Value=='-') i++,iValue=-1,fValue=-1; else
					if (IdentReader[Ident+i]->Type==4) {
						j=strlen(IdentReader[Ident+i]->sValue);
						for (k=1;k<j-1;k++) Code[ip++]=IdentReader[Ident+i]->sValue[k];
					}	else {
						vdword *vt;
						if (SymPreType==3) { fValue*=IdentReader[Ident+i]->fValue; vt=(vdword*)&fValue; }
						else { iValue*=IdentReader[Ident+i]->iValue; vt=(vdword*)&iValue; }
						if (SymPreType==2) Code[ip++]=*(vbyte*)vt; else
						if (SymPreType==1) { Code[ip++]=*(vbyte*)vt; *vt>>=8; Code[ip++]=*(vbyte*)vt; } 
						else { SetDWord(ip,*vt); ip+=4; }
					}
				}	
			}	else
			if (AsmCmd=="LD") {
				if (IdentReader[Ident+1]->Type!=0) {
					LoadOK=-2;
					sprintf(AsmError,"Error (%d) instruction LD need a data type",LinePoint);
					break;
				}
				SymPreType=IdentReader[Ident+1]->Value-8;
				Code[ip]=(0x10|SymPreType);
				if (MakeTwoSymbol(ip,Ident+2)) {
					LoadOK=-3;
					sprintf(AsmError,"Error (%d) %s",ErrorMsg);
					break;
				}
				ip+=10;
			}	else
			if (AsmCmd=="PUSH") {
				if (CmdDat=="ALL") Code[ip++]=0x2f; else {
					if (MakeOneSymbol(ip,Ident+1)) {
						LoadOK=-3;
						sprintf(AsmError,"Error (%d) %s",ErrorMsg);
						break;
					}
					Code[ip]=0x20+(vbyte)SymType;
					ip+=5;
				}
			}	else
			if (AsmCmd=="POP") {
				if (CmdDat=="ALL") Code[ip++]=0x3f; else {
					if (MakeOneSymbol(ip,Ident+1)) {
						LoadOK=-3;
						sprintf(AsmError,"Error (%d) %s",ErrorMsg);
						break;
					}
					Code[ip]=0x30+(vbyte)SymType;
					ip+=5;
				}
			}	else
			if (AsmCmd=="IN") {
				Code[ip]=0x40;
				if (MakeTwoSymbol(ip,Ident+1)) {
					LoadOK=-3;
					sprintf(AsmError,"Error (%d) %s",ErrorMsg);
					break;
				}
				ip+=10;
			}	else 
			if (AsmCmd=="OUT") {
				Code[ip]=0x50;
				if (MakeTwoSymbol(ip,Ident+1)) {
					LoadOK=-3;
					sprintf(AsmError,"Error (%d) %s",ErrorMsg);
					break;
				}
				ip+=10;
			}	else
			if (AsmCmd=="JMP") {
				if (MakeOneSymbol(ip,Ident+1)) {
					LoadOK=-3;
					sprintf(AsmError,"Error (%d) %s",ErrorMsg);
					break;
				}
				Code[ip]=0x60+(vbyte)SymType;
				ip+=5;
			}	else
			if (AsmCmd=="JPC") {
				Code[ip]=0x70;
				if (MakeOneSymbol(ip+1,Ident+2)) {
					LoadOK=-3;
					sprintf(AsmError,"Error (%d) %s",ErrorMsg);
					break;
				}
				Code[ip+1]=(vbyte)SymType;
				if (CmdDat=="Z")  Code[ip]+=1; else
				if (CmdDat=="NZ") Code[ip]+=6; else
				if (CmdDat=="A")  Code[ip]+=4; else
				if (CmdDat=="B")  Code[ip]+=2; else
				if (CmdDat=="AE") Code[ip]+=5; else
				if (CmdDat=="BE") Code[ip]+=3; else { 
					LoadOK=-3; 
					sprintf(AsmError,"Error unknow condition jump command %s",CmdDat.GetString());
					break;
				}
				ip+=6;
			}	else
			if (AsmCmd=="CALL") {
				if (MakeOneSymbol(ip,Ident+1)) {
					LoadOK=-3;
					sprintf(AsmError,"Error (%d) %s",ErrorMsg);
					break;
				}
				Code[ip]=0x80+(vbyte)SymType;
				ip+=5;
			}	else
			if (AsmCmd=="RET") Code[ip++]=(vbyte)0x90; else
			if (AsmCmd=="CMP") {
				SymPreType=IdentReader[Ident+1]->Value-8;
				Code[ip]=(0xA0|SymPreType);
				if (MakeTwoSymbol(ip,Ident+2)) {
					LoadOK=-3;
					sprintf(AsmError,"Error (%d) %s",ErrorMsg);
					break;
				}
				ip+=10;
			}	else
			if (AsmCmd=="CAL") {
				SymPreType=IdentReader[Ident+1]->Value-8;
				Code[ip]=(0xB0|SymPreType);
				if (MakeTwoSymbol(ip,Ident+3)) {
					LoadOK=-3;
					sprintf(AsmError,"Error (%d) %s",LinePoint,ErrorMsg);
					break;
				}
				char *Method;
				Method=IdentReader[Ident+2]->sValue;
				for (i=strlen(Method)-1;i>=0;i--) if (Method[i]>='a'&&Method[i]<='z') Method[i]=Method[i]-'a'+'A';
				if (strcmp(Method,"ADD")==0) Code[ip+1]+=0;    else
				if (strcmp(Method,"SUB")==0) Code[ip+1]+=1*16; else
				if (strcmp(Method,"MUL")==0) Code[ip+1]+=2*16; else
				if (strcmp(Method,"DIV")==0) Code[ip+1]+=3*16; else
				if (strcmp(Method,"MOD")==0) Code[ip+1]+=4*16;
				ip+=10;
			}	else {
				sprintf(AsmError,"Error (%d) unknow instruction %s",LinePoint,AsmCmd.GetString());
				LoadOK=-4;
				break;
			}
		}
		if (LoadOK==0&&stream.Eof()) LoadOK=1;
	}
	if (LoadOK==1) {
 		int FindPos;
		for (i=0;i<UseIdent.GetCount()&&LoadOK==1;i++) {
			for (j=0,FindPos=-1;j<LabelIdent.GetCount()&&FindPos==-1;j++)
				if (strcmp(UseIdent.String(i),LabelIdent.String(j))==0) FindPos=j;
			if (FindPos>=0) SetDWord(UseIdent.Id(i),LabelIdent.Id(FindPos)); else {
				sprintf(AsmError,"Error symbol %s not find",UseIdent.String(i));
				LoadOK=-5;
			}
		}
		if (LoadOK==1) {
			Code.SetLength(Code.GetLength()+StackSize+10);
			vdword VmRegs[8];
			memset(&VmRegs,0,8*sizeof(vdword));
			VmRegs[2]=ip+StackSize+2;
			VmRegs[3]=ip+2;
			VmRegs[0]=0;
			VmRegs[1]=0;
			if (NewVM.VmCreate(Code.GetLength(),(vbyte*)&Code[0],VmRegs)) LoadOK=-6;
			else { LoadOK=0; sprintf(AsmError,"Load OK"); }
		}
	}
	sprintf(ErrorMsg,"%s in line %d",AsmError,LinePoint);
	return LoadOK;
}

int IVMBetaDebug::Debug()
{
	int InstLength[23]={ 1,10,5,5,10,10,5,6,5,5,10,10,1,0,0,0,0,0,0,0,0,0 };
	int i,j;
	printf("\nStarting IVM Beta debugging...code length=%d\n",VmSize);
	while (VmStatus==0) {
		j=InstLength[VirtualMem[VmRegs.RP]>>4];
		printf("\n[%4x] : ",VmRegs.RP);
		if (j) {
			printf("(");
			for (i=0;i<j;i++) printf("%x ",VirtualMem[VmRegs.RP+i]);
			printf(") Enter...");
		}
		fgetc(stdin);
		Process();
		printf("RP=%8lx RF=%8lx RS=%8lx RB=%8lx\n",VmRegs.RP,VmRegs.RF,VmRegs.RS,VmRegs.RB);
		printf("R0=%8lx R1=%8lx R2=%8lx R3=%8lx\n",VmRegs.R0,VmRegs.R1,VmRegs.R2,VmRegs.R3);
	}
	printf("\nProgram exit with code %d\n",VmStatus);
	return VmStatus;
}

