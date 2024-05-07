#include <xc.h>
#include "IAC.h"
#include "Config.h"
#include "Main.h"
extern ECU_struct ECU;
extern bit Flag_Init_Pos;
/*
	ФАЗА:	F1	F2	F3	F4
	STEPA	1	0	0	1	- Линия А
	STEPB	1	1	0	0	- Линия Б
*/
// Движение шагового двигателя на открытие заслонки
void STP_Open()
{
	if(ECU.STEPCNT>=IAC_MAX)return;
	ECU.STEPCNT++;
	if(STEPA==0 & STEPB==0){STEPA=1;return;}//F3->F4
	if(STEPA==1 & STEPB==0){STEPB=1;return;}//F4->F1
	if(STEPA==1 & STEPB==1){STEPA=0;return;}//F1->F2
	if(STEPA==0 & STEPB==1){STEPB=0;return;}//F2->F3
}
// Движение шагового двигателя на закрытие заслонки
void STP_Close()
{
	if(ECU.STEPCNT<=IAC_MIN)return;
	ECU.STEPCNT--;
	if(STEPA==0 & STEPB==0){STEPB=1;return;}//F3->F2
	if(STEPA==0 & STEPB==1){STEPA=1;return;}//F2->F1
	if(STEPA==1 & STEPB==1){STEPB=0;return;}//F1->F4
	if(STEPA==1 & STEPB==0){STEPA=0;return;}//F4->F3
}
// Инициализация шагового двигателя
void STP_Init()
{
	for (char a=IAC_MAX;a>0;a--)
	{
	ECU.STEPCNT=IAC_MAX;
	delay(3);
	STP_Close();
	}
	ECU.STEPCNT=0;
	Flag_Init_Pos=1;
}
void STP_Set(char p)
{
	if (p > ECU.STEPCNT) {STP_Open();return;}
	if (p < ECU.STEPCNT) {STP_Close();return;}	
}
void IAC_Stab(void)
{
	if (Flag_Init_Pos) 
		{
		// Выставляем пусковое положение РХХ
		//unsigned int Step_Init_Pos =((((ECU.COLT * ECU.COLT)>>8)*(IAC_COLT-IAC_WARM))>>8) + IAC_WARM;
		unsigned char CLTR=(ECU.COLT * (int)ECU.COLT)>>8;
		unsigned char Step_Init_Pos =(((CLTR*(int)(IAC_COLT-IAC_WARM))>>8) + IAC_WARM;
		if (Step_Init_Pos > ECU.STEPCNT) {STP_Open();}
		else {Flag_Init_Pos=0;}
		return;
		}
	
	if (ECU.Mtr_On==0){return;}// Двигатель остановлен
	if (ECU.IAC)
		{
		const unsigned char TRSHH = ((((WARM_TRSH+273)*51)/100)-0x75) * 4;
		if (ECU.COLT<(255-TRSHH))
			{
			// Стабилизация прогретого двигателя
			const unsigned char Motor_Spd_Hot=SPD_HOT/25;
			if (ECU.RPM<Motor_Spd_Hot-1){}
			if (ECU.RPM>Motor_Spd_Hot+2){}
				
			}
		else	{
			// Стабилизация холодного двигателя
			const unsigned char Motor_Spd_Colt=SPD_COLT/25;
			if (ECU.RPM<Motor_Spd_Colt-2){}
			if (ECU.RPM>Motor_Spd_Colt+2){}
			
			}
		
		}
	else	{
		// Режим нагрузки
		if (DEMFER < ECU.STEPCNT){STP_Open();}
		
		
		}
}