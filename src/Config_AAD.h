#include <xc.h>

// ����� �����-������
#define	INJ1	LC0	// ���������� ��������� PEACK
#define	INJ2	LC1	// ���������� ��������� HOLD
#define	INJ3	LC2
#define	INJ4	LC3
#define	INJ5	LC4
#define	INJ6	LC5


#define	IGN1	LD3

#define	PUMP	LD0	// ���������� ������������
#define	IDLE	LD1	// �������� ��������� ��
#define	STEPA	LB4	// ����� ����� � �� ������� ��
#define	STEPB	LB3	// ����� ����� � �� ������� ��
#define	ACCEL	LD2	// ������������ ���������� �� ��������� (��)
#define	WARM_UP	LD4	// ������ ������ ��������
#define	PIN1	PORTA,4	// ������� ���


#define Base_Req_Fuel	5000
#define LPG_Req_Fuel	10000

//���� ��������� ��������  ���������� ��
#define	IAC_COLT	110	// ���������� ����� �� �������� ����� ������������� ��� -40 �(80)
#define	IAC_WARM	25	// ���������� ����� �� �������� ����� ������������� ��� +77 �(20)
#define	IAC_MAX		210	// ���������� ����� ��� ������������� �������� ������� (100)
#define	IAC_MIN		6	// ����������� ��������� ���
//��������� ��������� ����
#define	IAC_SKIP	600	// ��������� �������� ��� ������ ������������� (mV)
#define	FL_CLEAR	3027	// ��������� �������� ��� ��������� ������ �������� (mV)
//��������� ���������� ���������
#define	PR_PULSE	1000	// ����� ����������������� �������� ������� (2��)
#define	WARM_TRSH	54	//����������� ���������� ������������ ��������
#define	SPD_HOT		850	// ������� ��������� ���� (850)
#define	SPD_COLT	1250	// ������� ��������� ���� ��� �������� (1250)
#define	SPD_EPHH	1400	// ������� ��������� ������������ ��������������� ��������� ����
#define	DEMFER		26	// ������ ������ ������ ������������
#define	ACL_THR		5	// Accelerate theshold
#define Inject_Mode	0
#define	CH_Inject	4
#define	Cyl		4	// ���������� ��������� � ��������� �� ������������ 4�
#define MAP_type		2	// ������ �������� �� �������� ����������
#define MAP_LPG_type	3	// ������ �������� ����
//��� �������� � ����������� �� ����������
//                     	      8.6  10.2 11.8 13.4 15
const unsigned int LAG[5] =   {2050,1700,1450,1250,1010};//ZMZ6354 194CC
const unsigned int LAG_LPG[5]={5600,4300,3500,3000,2400};//Lovato LP


//��������� ������� ������� ��� ������ ���������
#define	CRANK_HOT	3000	// ����� ������� ��� ������� �� ������� (>80)
#define	CRANK_COLT	30000	// ����� ������� ��� ������� �� �������� (-40)


//��������� ���������� ������
#define	ADC_MAP		ADC_CH0	// AN0 ������ ����������� ��������
#define	ADC_TPS		ADC_CH1	// AN1 ������ ��������� ����������� ��������
#define	ADC_COLT	ADC_CH2	// AN2 ������ ����������� ��
#define	ADC_PWR		ADC_CH4	// AN3 ���������� ���������� �������� (�������� 4�/1�)
#define	ADC_IAT		ADC_CH3	// AN4 ������������ �������� �����
#define	ADC_LAMBDA	ADC_CH5 // ������ ���������
#define	ADC_EX		ADC_CH25// ������ ���� ����     

