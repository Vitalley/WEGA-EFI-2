#include <xc.h>
#include "Config.h"
#include "Main.h"
#include "Fuel_Calc.h"
#include "injection.h"
extern ECU_struct ECU;
extern unsigned char Cylinder;
void AllInjectionClose(void)
{
	INJ1=0;INJ2=0;
	INJ3=0;INJ4=0;
	INJ5=0;INJ6=0;
	CheckInj();
}
/*
void SetInjection(void)
{
	if (ECU.TIME_INJ>100 && ECU.CutOff==0)
{
//     if ((ECU.TIME_INJ<<1 + ECU.INJ_LAG)>0x7FFF) {Flag_Seq_Inj=0;}
unsigned int Injection;
     if (ECU.SeqINJ==1) 
	{
	if (Cylinder&1)
	    {// ПРи превышении времени впрыска, лаг форсунки не учитывается.	
	    if (INJ1==1)
	    {ECU.overINJ;Injection=ECU.TIME_INJ<<2;}
	    else {Injection=((ECU.TIME_INJ<<1)+ECU.INJ_LAG)<<1;}	    
	    INJ1=1;INJ4=1;
	    OpenECompare1(ECOM_UNCHG_MATCH&ECCP_1_SEL_TMR34&COM_INT_ON,ReadTimer3()+Injection);
	    }
	else
	    {
	    if (INJ2==1){ECU.overINJ;Injection=ECU.TIME_INJ<<2;}
	    else {Injection=((ECU.TIME_INJ<<1)+ECU.INJ_LAG)<<1;}
	    INJ2=1;INJ3=1;
	    OpenECompare2(ECOM_UNCHG_MATCH&ECCP_2_SEL_TMR34&COM_INT_ON,ReadTimer3()+Injection);
	    }
	}
     else 
     	{
	if (INJ1==1){Injection=ECU.TIME_INJ>>2;}
	else {Injection=((ECU.TIME_INJ+ECU.INJ_LAG)>>2);}
	INJ1=1;INJ2=1;INJ3=1;INJ4=1;
	OpenECompare1(ECOM_UNCHG_MATCH&ECCP_1_SEL_TMR12&COM_INT_ON, Injection);

     	}
}
    else	{AllInjectionClose();}
}

void CompareGroup1(void)
{
    if (ECU.SeqINJ) 
            {INJ1=0;INJ4=0;}
     else 
			{
			if (ECU.Mtr_On==1) {ECU.SeqINJ=1;}//Переводим по окончании одновременного впрыска в секвенциидальный
			AllInjectionClose();
			}    
    
}

void CompareGroup2(void)
{
	INJ2=0;INJ3=0;    
}
*/

void SetInjection(void)
{
if (ECU.TIME_INJ>100 && ECU.CutOff==0)
	{
	if (ECU.LPG)
	{
		if (Cylinder&1)
			{
				OpenECompare1(ECOM_UNCHG_MATCH&ECCP_1_SEL_TMR34&COM_INT_ON,ReadTimer3()+ECU.INJ_LAG);
				INJ3=1;INJ4=1;
			}
		else
			{
				OpenECompare2(ECOM_UNCHG_MATCH&ECCP_2_SEL_TMR34&COM_INT_ON,ReadTimer3()+ECU.INJ_LAG);
				INJ5=1;INJ6=1;
			}	

	}
	else
	{
		OpenECompare1(ECOM_UNCHG_MATCH&ECCP_1_SEL_TMR12&COM_INT_ON,ECU.INJ_LAG);
		INJ1=1;INJ2=1;	
	}
	
	}
	else	{AllInjectionClose();}
}
void CompareGroup1(void)
{
	if (ECU.LPG)
	{
		unsigned int Injection=ECU.TIME_INJ;
		if (Injection>16380)Injection=16381;
		if (INJ3) {INJ3=0;OpenECompare1(ECOM_UNCHG_MATCH&ECCP_1_SEL_TMR34&COM_INT_ON,ReadTimer3()+ (Injection<<2));}
		else	{INJ3=0;INJ4=0;CheckInj();}
	}
	else
	{
		if (INJ1) {INJ1=0;OpenECompare1(ECOM_UNCHG_MATCH&ECCP_1_SEL_TMR34&COM_INT_ON,ReadTimer3()+ ECU.TIME_INJ);}
		else	{INJ1=0;INJ2=0;CheckInj();}
	}
		
}
void CompareGroup2(void)
{
	if (ECU.LPG)
	{
		unsigned int Injection=ECU.TIME_INJ;
		if (Injection>16380)Injection=16381;
		if (INJ5) {INJ5=0;OpenECompare2(ECOM_UNCHG_MATCH&ECCP_2_SEL_TMR34&COM_INT_ON,ReadTimer3()+ (Injection<<2));}
		else	{INJ5=0;INJ6=0;CheckInj();}
				
	}
	else	{AllInjectionClose();}
	
	
}