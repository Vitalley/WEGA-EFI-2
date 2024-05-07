#include <xc.h>
#include "Config.h"
#include "Main.h"
#include "Config_bits.h"
#include "Fuel_Calc.h"
#include "IAC.h"
#include "Lambda.h"
#include "pnd_emul.h"
#include "injection.h"
volatile ECU_struct ECU;
volatile unsigned char VE_table[16]=179,179,179,179,179,179,179,179,179,179,179,179,179,179,179,179;
volatile uint16_t ADC,TMRC,PUMPCounter;
volatile unsigned int LoadCounter;
unsigned int MAP_array[6],PWR_array[6],SPD_array[6],CLT_array[6],IAT_array[6], RPM_array[6], EXT_array[6],AirPressure, LPGPressure, Load_array[6];
unsigned char Byte_resieve, Cylinder;
unsigned char *p;
volatile bit  
 	Flag_UART_recieved, 
 	Flag_Send_data, 
 	Flag_Speed_equal, 
 	Flag_Corr_equal, 
 	Flag_IAC, 
 	Flag_set_IAC,
 	Flag_setADC_chenal,
 	Flag_Sec_Clock,
 	Flag_FiveSeconds,
 	Flag_FuelEq,
	Flag_Init_Pos;
//rom char tm[] = __TIME__ ;
const char dt[] = __DATE__;
void INIT()
{    
    OSCCON = 0x30;// SCS FOSC; IRCF 1MHz_HFINTOSC/16; IDLEN disabled; 
    OSCCON2 = 0x04;// PRISD enabled; SOSCGO disabled; MFIOSEL disabled; 

    LATE = 0x00;LATD = 0x00;LATA = 0x00;LATB = 0x00;LATC = 0x00;

    TRISE = 0B00000011;
    TRISA = 0xFF;
    TRISB = 0B00100111;
    TRISC = 0B10000000;
    TRISD = 0b00100000;
#ifdef __DEBUG
   //while(PLLRDY == 0){}// Wait for PLL to stabilize
#endif
    ANSELD = 0xFF;
    ANSELC = 0x3C;
    ANSELB = 0x3C;
    ANSELE = 0xff;
    ANSELA = 0x2F;

    WPUB = 0x00;//WPUx registers ПОДТЯГИВАЮЩИЕ РЕЗИСТОРЫ
    nRBPU = 1;
    PUMP=1;IDLE=1;ACCEL=0;WARM_UP=0;
//Конфигурирование периферии
//TMR0 1.568ms
	TMR0H = 0x00;TMR0L = 0x60;
	T0CON = 0xD6;// T0PS 1:128; T08BIT 8-bit; T0SE Increment_hi_lo; T0CS FOSC/4; TMR0ON enabled; PSA assigned; 
// TMR1 - Free run timer, gated by tmr2, resulting increment per 4us, 16bit read
T1GCON = 0b11000001; // Gate enable, source - TMR2
T1CON = 0b00000011; //prescaler 1/1, sysclock 1/4, 16bit read
TMR1ON=0;
TMR1H = 0;TMR1L = 0; 
PIR1bits.TMR1IF = 0;


// TMR2 - gate for tmr1 - prescaler 1/1, postscaler 1/1, period 63 (4us)
PR2 = 63;
T2CON = 0b01111100;
TMR2IE = 0;

//TMR3 - Free run timer, resulting increment per 0.5us, 16bit read
T3GCON = 0x00;//T3GSS T3G_pin; TMR3GE disabled; T3GTM disabled; T3GPOL low; T3GGO done; T3GSPM disabled;
T3CON = 0b00110111;// T3CKPS 1:8; T3OSCEN disabled; nT3SYNC do_not_synchronize; TMR3CS FOSC/4; TMR3ON enabled; T3RD16 enabled;
TMR3H=0;TMR3L=0;
TMR3IE=0;
 

//USART 8bit 9600
BAUDCON1 = 0x08;RCSTA1 = 0x90;TXSTA1 = 0x24;SPBRG1 = 0x22;SPBRGH1 = 0x06;//SPBRG1 = 0x40;SPBRGH1 = 0x03;
SPBRG1 = 0x82; TX1IE=0;
	//SetTmrCCPSrc(T1_SOURCE_CCP);
//PWM5 61кГц
OpenTimer6(TIMER_INT_OFF&T2_PS_1_1&T2_POST_1_1);// Таймер для PWM аналогового выхода
OpenPWM5(255,CCP_5_SEL_TMR56);//PWM period = [(period ) + 1] x 4 x Tosc x TMRx prescaler
SetDCPWM5(45);// 0.21В на выходе
    

    IPEN = 1;// Enable Interrupt Priority Vectors
    
	CCP1IP = 1;
	CCP2IP = 1;
	CCP3IP = 1;
	TMR1IP = 1;	// TMRI - high priority
	TMR0IP = 0;	// TMRI - low priority
	ADIP = 0;	// ADI - low priority
	TX1IP = 0;	// TXI - low priority
	RC1IP = 0;	// RCI - low priority
	INT1IP = 1;	// INT1I - low priority 
	INT2IP = 0;

  	C2IP = 0;  	
  	C2IP = 0;
  	
	INTEDG0=0;
	INTEDG1=1;
	INTEDG2=0;

	TMR0IF = 0;// Clear Interrupt flag before enabling the interrupt
  	TMR0IE = 1;// Enabling TMR0 interrupt.
	TMR1IE = 1;// Enabling TMR1 interrupt.
	RC1IE = 1;// RCI - low priority
        
	
	CCP1IE=1;
	CCP2IE=1;
	CCP3IE=1;
  	C2IE = 1;
  	C2IE = 1;
	OpenADC(ADC_FOSC_64&ADC_20_TAD&ADC_RIGHT_JUST, ADC_COLT&ADC_INT_ON, ADC_REF_VDD_VSS); 
  
//Сброс переменных 
	for(p=(char*)&ECU;p<((char*)&ECU+38);p=p+1) {*p=0;}//Очистка буфера передачи

//	for(char pr=0;pr<38;pr=++)
	
//	{ECU[pr]=0;}//Очистка буфера передачи
		
	if (TO==0){ECU.WDT=1;}
	if (BOR==0){ECU.BOR=1;BOR=1;}
    	ECU.BYTEAF=0xAF; ECU.Byte_Counter=0x00; ECU.Paket_version=0x10; ECU.Paket_size=38;
    	p=(char*)&ECU;
    //	ECU.CYL=0;//CYL-1;
    	//VE_table[]={0,255,0,255,0,255,0};
	ECU.SeqINJ=0;
	ECU.INJ_LAG=LAG[3];
	ConvertADC();	
	Cylinder=0;
	ECU.MAPtype=1;
	ECU.COLT=165;
	PUMPCounter=0;
	TMR0ON=1;
	LoadCounter=0;
	STP_Init();
	//ECU.WARM_12=127;

	ECU.TIME_INJ=PR_PULSE;INT0IF=1;
	ECU.RqF=Base_Req_Fuel;   
    GIEL=1;GIEH=1;
    INT0IE=1;INT1IE=1; 
}
void Motor_stopped()
{
		ECU.RPM=0;ECU.Mtr_On=0;ECU.AFSTEN=1;Flag_Speed_equal=0;
		ECU.Accel=0;ECU.D_Accel=0;ECU.Econ=0;ECU.EMR=0;
		ACCEL=0;ECU.SeqINJ=0;
		INJ1=0;INJ2=0;INJ3=0;INJ4=0;INJ5=0;INJ6=0;
		IGN1=0;
		if (ECU.Mtr_On){ECU.COMP=1;}
		Flag_Init_Pos=1;	
	
	}
// Таймер внутренней синхронизации	
void TMR0Event() {
	TMR0H = 0x00;TMR0L = 0x60;
static int TXdelay, mckc, ADCdelay,STPdelay,Cdelay;
     if (TXdelay==0){TXdelay=100;Flag_Send_data=1;} else {TXdelay--;}
     if (PUMPCounter>=1600){PUMPCounter=0;PUMP=0;Motor_stopped();} else {PUMPCounter++;}
     if (STPdelay<=0){Flag_IAC=1;STPdelay=3;} else {STPdelay--;}
     if (ADCdelay<=0){Flag_setADC_chenal=1;ADCdelay=15;} else {ADCdelay--;}
     if (mckc<=0){Flag_Sec_Clock=1;mckc=784;} else {mckc--;}
     if (Cdelay<=0){Flag_Corr_equal=1;Cdelay=50;} else {Cdelay--;}
     ECU.reserv2=SMA_filter(LoadCounter,Load_array)>>1;LoadCounter=0;
     }
// Переполнение TMR1
void TMR1Event(){TMR1ON=0;TMRC=0;TMR1=0;
		Motor_stopped();
		}
void Emul_PND (void)
{
	unsigned int Vout;
	//Vout=((ECU.RPM/2)*(ECU.APressure/4))/4;
	Vout=Emul_PND_T(ECU.RPM,AirPressure);
	if (Vout < 10) Vout = 10;
	ECU.UOZ=Vout;//>>2;
	SetDCPWM5((Vout<<2)+2);
}
void setADC()
{
static uint8_t ADCChan;
if (GO==0)
{	ADIF=0;
	switch (ADCChan)
	{
	case 0: if(ECU.Mtr_On==0){SetChanADC(ADC_MAP);ADCChan++;} else {ADCChan++;return;} break;
	case 1: SetChanADC(ADC_TPS); ADCChan++; break;
	case 2: SetChanADC(ADC_COLT); ADCChan++; break;
	case 3: SetChanADC(ADC_PWR); ADCChan++; break;
	case 4: SetChanADC(ADC_IAT); ADCChan++; break;
	case 5: SetChanADC(ADC_LAMBDA); ADCChan++; break;
	case 6: SetChanADC(ADC_EX); ADCChan=0; break;
	default: ADCChan=0;
	}
//	NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); 
//	NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); 
	__delay_us(22);ADIF=0;
	ConvertADC();
}
}
void GetColt(void)
{
#define	T_COLT		0x1D4	// 75Показания АЦП при -40С
#define	T_HOT		0xB4	// +80C B3
unsigned int CLT_DATA=SMA_filter(ReadADC(),CLT_array);
if (CLT_DATA>0x1D4 && CLT_DATA<0x3D3)
	{
	ECU.CLT_ERR=0;
	ECU.ColtF=(CLT_DATA-0x1D4)>>1;
	if ((CLT_DATA-0x1D4)>0xFF)
		{ECU.COLT=0;ECU.Warm=0;}
	else
		{ECU.COLT=0xFF-(CLT_DATA-0x1D4);ECU.Warm=1;}
	}
else	{// Эмуляция прогрева в случае неисправного датчика температуры
	
	if (ECU.CLT_ERR==0 && ECU.Mtr_On==1) {ECU.Warm=0;ECU.COLT=0;}// Если выходит из строя датчик во время работы двигателя, то считаем двигатель прогретым
	if (ECU.CLT_ERR==0 && ECU.Mtr_On==0) {ECU.Warm=1;ECU.COLT=164;}// Если при старте не исправен датчик, то начинаем отсчёт с 0 градусов
	ECU.CLT_ERR=1;
	if (CLT_DATA<0x1D4){ECU.ColtF=0;}else{ECU.ColtF=0xFF;}
	if (Flag_FiveSeconds==1)
		{
			Flag_FiveSeconds=0;
			if (ECU.COLT>0 && ECU.Mtr_On==1)ECU.COLT--;
		}
	if (ECU.COLT==0 && ECU.Mtr_On){ECU.Warm=0;}
	}
}
void ADCread() {
	/*ADC=(((unsigned int)ADRESH)<<8)|(ADRESL);*/
	if ((ADCON0&0b01111100) == (ADC_MAP&0b01111100)) 
		{
			int ADCPressure=EX_filter(ReadADC(),MAP_array);
			if (ADCPressure<0x14 || ADCPressure>0x3C0){ECU.MAP_ERR=1;}else{ECU.MAP_ERR=0;}
			AirPressure=((ADCPressure+MAP_Offcet)*(unsigned short long)MAP_slope)>>10;
			ECU.APressure=AirPressure>>1;
			Flag_FuelEq=1;
			if(ECU.Mtr_On==0){C2IF=1;}
			return;
		}
	if ((ADCON0&0b01111100) == (ADC_TPS&0b01111100)) 
		{
			ECU.TPS=ReadADC()>>2;
			if (ECU.TPS<=IACskipc){ECU.IAC=1;IDLE=1;}else{ECU.IAC=0;IDLE=0;}
			return;
		}
	if ((ADCON0&0b01111100) == (ADC_COLT&0b01111100)) 
		{GetColt();return;}
	if ((ADCON0&0b01111100) == (ADC_PWR&0b01111100)) 
		{
			ECU.POWER=SMA_filter(ReadADC(),PWR_array)>>2;
			if (ECU.POWER<24){TestPoint1=1;}else{TestPoint1=0;}
			return;
		}
	if ((ADCON0&0b01111100) == (ADC_IAT&0b01111100)) 
		{
			ADC=SMA_filter(ReadADC(),IAT_array);
			ECU.reserv2=ADC>>2;//Измеритель уровня ПДН
			ECU.IAT=ADC>>1;
			LPGPressure=((ADC+MAP_LPG_Offcet)*(unsigned short long)MAP_LPG_slope)/1024;
			ECU.RCO=LPGPressure;
			return;
		}
	if ((ADCON0&0b01111100) == (ADC_LAMBDA&0b01111100)) 
		{
			ADC=ReadADC();
			if (ADC > 203 || ADC < 12) {ECU.O2S=127;ECU.Lambda_ERR=1;return;}
			ECU.O2S=Lambda[ADC&0xFF];
			return;
		}
	if ((ADCON0&0b01111100) == (ADC_EX&0b01111100)) 
		{
			int EGRD_CURR=EX_filter(ReadADC(),EXT_array);
			if (EGRD_CURR < 254) EGRD_CURR=254;
			if (EGRD_CURR > 764) EGRD_CURR=764;
			ECU.WARM_12=(EGRD_CURR-254)>>1;//Ток ЭГРД
			return;
		}
    }
// Пусковая доза или 1-4 форсунки
void CCP1Event() {
	 CCP1CON=0;CompareGroup1();
 
    }
// 2-3 форсунки
void CCP2Event()
{	
	CCP2CON=0;CompareGroup2();

}
// Управление зажиганием
void CCP3Event()
{
	CCP3CON=0;
	IGN1=0;
}

// Импульсы синхронизации
void SPARKEvent()
     {
	TMR1IE=0;TMR1ON=0;TMRC=TMR1;TMR1=0;
	TMR1ON=1;PUMPCounter=0; PUMP=1;TMR1IF=0;TMR1IE=1;
	Flag_Speed_equal=1;
	if (Cylinder++ >=Cyl)Cylinder=1;//Номер цилиндра 1..4..6;
	if (ECU.Mtr_On==0){IGN1=0;}else{IGN1=1;}
	SetInjection();
	C1IF=1;
     }
void Spark2()
{// Обратная поляность синхронизации, управление  зажиганием
if (ECU.Mtr_On==0)	{IGN1=1;} else {OpenECompare3(ECOM_UNCHG_MATCH&ECCP_3_SEL_TMR12&COM_INT_ON, TMR1+(TMRC/10));}
}
void Sync (void)
{
	
	
	
	
}

void Get_Lag (void)
{
	if (ECU.LPG)
		{
		if (ECU.POWER>0x96) {ECU.INJ_LAG=LAG_LPG[4];return;}//Более 15 вольт
		if (ECU.POWER<0x57) {ECU.INJ_LAG=LAG_LPG[0];return;}//Меньше 8,6в

		ECU.INJ_LAG=ReadTable((ECU.POWER - 0x57)<<2,LAG_LPG);			
		}
	else	{
		if (ECU.POWER>0x96) {ECU.INJ_LAG=LAG[4];return;}//Более 15 вольт
		if (ECU.POWER<0x57) {ECU.INJ_LAG=LAG[0];return;}//Меньше 8,6в

		ECU.INJ_LAG=ReadTable((ECU.POWER - 0x57)<<2,LAG);
		}
//ECU.Warm_Enr=ReadTable(ECU.COLT,WrmEnr);

}
void delay(unsigned int ms)
{
	while(ms--){__delay_ms(1);}
}
void Set_ADC_MAP (void)
{
	SetChanADC(ADC_MAP);
	ADIF=0;
	__delay_us(20);
	ConvertADC();	
}
void Set_TPS_MAP (void)
{
	SetChanADC(ADC_TPS);
	ADIF=0;
	__delay_us(20);
	ConvertADC();	
}
void main() 
    {
    INIT();
    while(1)
    {
    LoadCounter++;
//Расчёт цикловой подачи
    if (Flag_FuelEq)
        {
        Flag_FuelEq=0;Fuel_Calculate();
        }

//Передача пакета
    if (Flag_Send_data && TXSTAbits.TRMT){
    
        if (ECU.Byte_Counter==ECU.Paket_size)
            {ECU.Byte_Counter=0;ECU.CHK_SUM_TX=0;Flag_Send_data=0;p=(char*)&ECU;}
        else
            {RCSTAbits.CREN=0;TXREG=*p;ECU.CHK_SUM_TX=ECU.CHK_SUM_TX+TXREG; p=p+1;ECU.Byte_Counter++;}}
    //        
    if (TXSTAbits.TRMT==1 && Flag_Send_data==0) {RCSTAbits.CREN=1;}  
    //Отлавливаем ошибку переполнения буфера приёма     
    if (RCSTAbits.OERR==1){RCSTAbits.CREN=0;RCSTAbits.CREN=1;TXSTAbits.TXEN=0;TXSTAbits.TXEN=1;}
//Флаг расчёта оборотов
    if (Flag_Speed_equal){
        unsigned int MSpeed, TCycle;
        INT0IE=0;TCycle=TMRC;INT0IE=1;Flag_Speed_equal=0;
	//TCycle=SMA_filter(TCycle,RPM_array);
        if (TCycle==0 || SPD_CONST<TCycle) {ECU.RPM=0,ECU.Mtr_On=0;ECU.SeqINJ=0;ECU.CutOff=0;}
        else
        {    
        MSpeed=(unsigned short long)SPD_CONST/TCycle;//Здесь считаем обороты двигателя
        if (MSpeed>255) {ECU.RPM=255;} else {ECU.RPM=MSpeed;}
	if (MSpeed>260 && ECU.CutOff==0) {ECU.CutOff=1;}
	if (MSpeed<250 && ECU.CutOff==1) {ECU.CutOff=0;}
        }
// Определяем запуск двигателя (обо>400) и включаем флаг на послепусковое обогащение
        if (ECU.RPM<16) {ECU.Mtr_On=0;ECU.AFSTEN=1;ECU.CutOff=0;} else {ECU.Mtr_On=1;}
        }
    //расчёт корректировки
    if (Flag_Corr_equal){Flag_Corr_equal=0;
    				Get_Lag();
    				Emul_PND();
    				ECU.SPD_Enr=VE(ECU.RPM,VE_table);}
    //стабилизация оборотов двигателя
    if (Flag_IAC){Flag_IAC=0;IAC_Stab();}
    //позиционирование РХХ
    if (Flag_set_IAC){Flag_set_IAC=0;STP_Set(10);}
    //Приём данных с UART
    if (Flag_UART_recieved){Flag_UART_recieved=0;}
    if (Flag_setADC_chenal){Flag_setADC_chenal=0; setADC();}
    //Ежесекундное прерывание
    if (Flag_Sec_Clock){Flag_Sec_Clock=0;
	    		static char Minutes,Seconds;
    			if (Minutes>=60){Minutes=0;ECU.MotoClock++;}else{Minutes++;}
    			if (Seconds>=5){Seconds=0;Flag_FiveSeconds=1;}else{Seconds++;}
    			}
    }     
}
// Обработка прерываний высокого приоритета
void interrupt high_priority high_isr(void)
{
//Импульс синхронизации
if (INT0IF) {INT0IF=0;SPARKEvent();}
//Переполнение основного циклового таймера
if (TMR1IF) {TMR1IF=0;TMR1Event();}
//Сигнал фазы
if (INT1IF) {INT1IF=0;Spark2();}
//Событие от модуля сравнения основного циклового таймера
if (CCP1IF) {CCP1IF=0;CCP1Event();}
if (CCP2IF) {CCP2IF=0;CCP2Event();}
if (CCP3IF) {CCP3IF=0;CCP3Event();}
}

// Обработка прерываний низкого приоритета
void interrupt low_priority low_isr(void)
{
if (INT1IF) {INT1IF=0;Spark2();}
//Таймер внутренней синхронизации
if (T0IF) {T0IF=0;TMR0Event();}
//Прибём байта из USART
if (RCIF) {RCIF=0; if (Flag_Send_data=0){Byte_resieve=RCREG; Flag_UART_recieved=1;}/*else RCREG=RCREG; */}
//Считывание с АЦП
if (ADIF && ADIE) {ADIF=0;ADCread();}
if (INT2IF) {INT2IF=0;Sync();}
if (C1IF) {C1IF=0;Set_ADC_MAP();}
if (C2IF) {C2IF=0;Set_TPS_MAP();}
}
