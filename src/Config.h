#include <xc.h>
#define _XTAL_FREQ 64000000


//#include "Config_2108"
#include "Config_AAD"

// Порты ввода-вывода
#define TestPoint1 LD5
#define TestPoint2 RD6
#define TestPoint3 RD7

const unsigned short long 	SPD_CONST=1200000/Cyl;	// Констатна для вычисления оборотов двигателя
const unsigned char		IACskipc = (long int)IAC_SKIP*255/5000;


// Параметры датчиков абсолютного давления
#define	MAP100_slope	242
#define	MAP100_Offcet	153

#define	MAP115_slope	285
#define	MAP115_Offcet	95

#define	MAP250_slope	640
#define	MAP250_Offcet	40

#define	MAP400_slope	1060
#define	MAP400_Offcet	8

//
#if 	MAP_type==1	// MPX4100	
#define	MAP_slope	MAP100_slope
#define	MAP_Offcet	MAP100_Offcet
#elif 	MAP_type==2	// MPX4115	
#define	MAP_slope	MAP115_slope
#define	MAP_Offcet	MAP115_Offcet
#elif 	MAP_type==3	// MPX4250	
#define	MAP_slope	MAP250_slope
#define	MAP_Offcet	MAP250_Offcet
#elif 	MAP_type==4	// MPX6400	
#define MAP_slope	MAP400_slope
#define	MAP_Offcet	MAP400_Offcet
#else
	#error "MAP not defined"
#endif


//
#if 	MAP_LPG_type==3	// MPX4250	
#define	MAP_LPG_slope	MAP250_slope
#define	MAP_LPG_Offcet	MAP250_Offcet
#elif 	MAP_LPG_type==4	// MPX6400	
#define MAP_LPG_slope	MAP400_slope
#define	MAP_LPG_Offcet	MAP400_Offcet
#else
	#error "MAP_LPG not defined"
#endif


//#define	MAP_slope	(P2-P1)/(V2-V1)//18.888
//#define	MAP_Offcet	(1/MAP_slope)*P2-V2//0/238	

//Pres=(adc_dad + offset)*slope
// шторки трамблёра PG 16мм окно, 19мм шторка (окружность 140) 720/140=0.097 окно 82.28, шторка 97.71
//шторки ABS 13мм окно, 22мм шторка(окружность 140) окно 66.85, шторка 113.14
//Замерил на своём: 15.5мм окно и 19,5мм шторки.(окружность 140) -  79,1 градус окно

//ABK  трамблёр 16мм окно и 19мм шторка, диаметр 45.5 - 82градуса окно
//AAD размер окна 12,7мм

// Пусковая искра по входу шторки трамблёра
// Основное зажигание считается от выхода шторки с задержкой
// окно - 0, шторка - 1


//период зажигания = 180град. коленвала
//задержка зажигания = Х град. 
//Х = 180*задержка*обороты
//Vz=1000000*((Umax-Uoz)/(24*Od)) и значения в таблицах начинаются с 220 до 5100 проверял на файле 2108 Значения Vz в микросек 
//Uoz=Umax-Vz*Od*6 

//1. Таймер меряет период между искрами. Это время поворота КВ на 180гр (для 4цил)
//2. Значение таймера делится на 3(для 4цил) - мы получаем время поворота КВ на 60гр - назвем его TMR. С этим значением и работаем, оно одинаковое для разного количества цилиндров.
//3. Задержка рассчитывается по формуле
//dT = TMR - UOZ*TMR/60
//где UOZ - значение УОЗ в градусах.

//https://ecubuy.wixsite.com/ecufix/kopiya-kak-dostoverno-opredelit-isp

//https://web.archive.org/web/20151001063919/http://www.motronic.ws/afm_tf.htm
//AFM1 B2 A3 95 89 FB E6 D3 C2 
//AFM2 01 02 03 04 04 05 06 07
//AFM3 81 84 86 89 8B 8E 91 94 96 99 9C 9F A2 A5 A9 AC AF B2 B6 B9 BD C0 C4 C8 CC D0 D4 D8 DC E0 E4 E9



//https://www.drive2.ru/l/486837086695981922/?page=0#comments
//Итак, как писал Redsky, характеристика расходомера в Мотронике записана в 3х таблицах:
//AFM1 (8x1): 160 149 139 130 242 225 210 196
//AFM2 (8x1): -4 -3 -2 -1 -1 0 1 2
//AFM3 (32x1): 129 132 134 137 140 142 145 148 151 154 157 160 163 166 170 173 177 180 184 187 191 195 198 202 206 210 214 219 223 227 232 236