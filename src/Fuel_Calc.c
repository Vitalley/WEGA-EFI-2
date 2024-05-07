#include <xc.h>
#include "Config.h"
#include "Fuel_Calc.h"
#include "Main.h"
extern ECU_struct ECU;
extern unsigned int AirPressure,LPGPressure;

//
void Fuel_Calculate() 
{
unsigned short long Injection;
    if (ECU.Mtr_On==1)
    {
static unsigned char TPS1;	    
char DTPS=TPS1=ECU.TPS;TPS1=ECU.TPS;
//======================================================================================   
// Расчёт времени впрыска в рабочем режиме\
//======================================================================================

	if (ECU.AFSTEN)
		{if (ECU.Accel_ENR-- <= 128){ECU.AFSTEN=0;}}
	else	{if (ECU.IAC==1 && ECU.RPM > (SPD_EPHH/25)) {ECU.TIME_INJ=0;ECU.Econ=1;return;}// Режим принудительного холостого хода
		else{ECU.Econ=0;}
		}

// Основной режим работы двигателя			

		//if (DTPS>ACL_THR){ECU.Accel=1;}else{ECU.Accel=0;}
		//if (DTPS<ACL_THR){ECU.D_Accel=1;}else{ECU.D_Accel=1;}	
	
	
	
	
	if (ECU.LPG)
		{
		int GazPressure=LPGPressure-AirPressure;
		if (GazPressure < 1) {GazPressure=64;}
		Injection=((unsigned short long)ECU.RqF*AirPressure)>>8;
		Injection=((signed short long)(Injection<<8)/GazPressure);	
		}
	else	{
     		Injection=((unsigned short long)ECU.RqF*AirPressure)>>8;
     		
     		Injection=((unsigned short long)Injection*ECU.Accel_ENR)>>7;
     		}
// Коррекция топливной смеси по объёмной эффективности двигателя 
  		
//	Injection=((unsigned short long)Injection*ECU.SPD_Enr)>>8;
// Определяем включение экономайзера мощностного режима
/*	if (AirPressure>(70*2.56))
		{ECU.EMR=1;
		if (ECU.RPM>100 & ECU.Warm)
				{Injection=((unsigned short long)Injection*0xB3)>>7;}
			else
				{Injection=((unsigned short long)Injection*0x9D)>>7;}
		}
		else{ECU.EMR=0;}     	*/
// Определение обогащений по ускорению и после запуска двигателя
	if (ECU.Accel || ECU.D_Accel || ECU.AFSTEN)
		{
		Injection=((unsigned short long)Injection*ECU.Accel_ENR)>>7;	
			
			
			
		}


// управление выходными линиями			
	if (ECU.Accel || ECU.EMR) {ACCEL=1;}else{ACCEL=0;}
	if (ECU.IAC) {IDLE=1;} else {IDLE=0;}
	if (ECU.Warm) {WARM_UP=1;} else {WARM_UP=0;}
		}
    else
    {
//==================================================================================================
// Расчёт времени впрыска на запуске двигателя
//==================================================================================================
	ECU.EMR=0;
	ECU.AFSTEN=1;
	ECU.Econ=0;
	ECU.Accel_ENR=255;
const char FClear=FL_CLEAR/19;
        if(ECU.TPS > FClear)
        {
		Injection=0;// Продувка двигателя, форсунки закрыть
        }
        else
        {
	        Injection=((((unsigned short long)ECU.COLT*ECU.COLT>>8)*(CRANK_COLT-CRANK_HOT))>>8)+CRANK_HOT;
        }
    }
	

	if (ECU.CutOff)
		ECU.TIME_INJ = 0;
	else
		ECU.TIME_INJ=Injection;
}
bit LGP_st(void)
{
int LPGPres = LPGPressure-AirPressure;
if (LPGPres < 110) {return 1;} else {return 0;}
}


void CheckInj(void)
{// Проверка на закрытие всех форсунок
	if (INJ1|INJ2|INJ3|INJ4|INJ5|INJ6)
	{
		if (LGP_st())
			{
				ECU.LPG=0;
				ECU.RqF=Base_Req_Fuel;
				INJ1=0;INJ2=0;INJ3=0;INJ4=0;INJ5=0;INJ6=0;
			}	
			
	}
	else
	{
	if (1) 
		{
		if (LGP_st())
			{
				ECU.LPG=0;
				ECU.RqF=Base_Req_Fuel;
			}
		else
			{
				ECU.LPG=1;
				ECU.RqF=LPG_Req_Fuel;
			}
		} 
	else 
		{
		ECU.LPG=0;
		ECU.RqF=Base_Req_Fuel;
		}
	}	
	
}
unsigned int ReadTable(unsigned int a,const unsigned  int *Massive)
{
unsigned char Index,Span;
Span=a&0b00111111;
Index=a>>6;
if (Massive[Index] < Massive[Index+1])
	 {return IntpResL(Massive[Index],Massive[Index+1],Span);}
 else
 	{return IntpResL(Massive[Index+1],Massive[Index],0x3F-Span);}
}
//
unsigned char Get_Table_tiny (unsigned char a,unsigned char *Table)
{
unsigned char Index,Span,Slope;
Span=a&0b00111111;
Index=a>>6;

return Table[Index];
}
unsigned char Get_VE (unsigned char RPM,unsigned char *Table)
{
	
	
	
	
	
	
	
}
//
long map(long x, long in_min, long in_max, long out_min, long out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

//
int IntpRes(int Low_d,int Hi_d,unsigned char C)
{
return (Low_d + (((Hi_d-Low_d)*(short long)(C & 0x0F))>>2));
}
//
int IntpResL(int Low_d,int Hi_d,unsigned char C)
{
int prom = (((Hi_d-Low_d)*(short long)(C & 0x3F))>>6);
return (Low_d + prom);
}
//
int SMA_filter(unsigned int a,unsigned  int* Massive)
{
char n=Massive[4]&0x3;	
  Massive[n]=a;
  Massive[4]=(n+1)&0x3;
if (Massive[5] < 4) {Massive[5]++;return (a);}
  a=0;
  for(char j=0;j<4;j++){a=a+Massive[j];}

return (unsigned int)a>>2;
}

int EX_filter(unsigned int a,unsigned  int* Massive)
{
unsigned int n;
Massive[3] = Massive[2];
Massive[2] = Massive[1];
Massive[1] = Massive[0];
Massive[0] = a;
if (Massive[5] < 4) {Massive[5]++;return (a);}
n = (Massive[3] + Massive[2])>>1;
n = (n + Massive[1])>>1;
n = (n + Massive[0])>>1;

return (n);
}

unsigned char VE(unsigned char a,unsigned char* Table)
{
unsigned char Index,Span,Slope,lowdt,hidt;
Span=a&0b00001111;
Index=a>>4;
lowdt=Table[Index];hidt=Table[Index+1];
if (lowdt<hidt){return(lowdt+(((hidt-lowdt)*Span)>>4));}else{return(hidt+(((lowdt-hidt)*Span)>>4));}	
	
	
}