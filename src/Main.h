#include <xc.h>
typedef unsigned char uint8_t;
typedef unsigned int uint16_t;

typedef struct {
	int i1;
	int i2;
	} math_struct;
extern math_struct msb;


 
typedef struct
{
	uint8_t BYTEAF; //0xAF
	uint8_t Byte_Counter;//0x01
	uint8_t Paket_version;//0x10 
	uint8_t Paket_size; //38
  //-
    union
    {
        uint8_t STAT;   
        struct
        {
	unsigned Mtr_On :1;// ��������� �������
	unsigned Warm :1;// ������� ���������
	unsigned IAC :1;// ��������� �� �������� ����
	unsigned Accel :1;// ���������� �������
	unsigned D_Accel :1;//��������� �������
	unsigned Econ :1;//����������� ��������������� ��������� ����
	unsigned EMR :1;//����������� ����������� ������
	unsigned AFSTEN :1;//����� �������� ����������
        };
    };
    union
    {
        uint8_t ERR;      // ������ � ������� 16 ���� �� ������
        struct
        {
	unsigned WDT:1; //������������ �� WDT
	unsigned BOR:1;
	unsigned overINJ:1; //���������� ������� ������� ��� ������
	unsigned COMP:1;
	unsigned CLT_ERR:1;//������ ������� ����������� ��
	unsigned MAP_ERR:1; //������ ���
	unsigned Lambda_ERR:1;//������ ������� ����������� �������
	unsigned SYNC:1;// ������ ������� ���
        };
    };



	uint8_t STEPCNT;// ��������� ���
	uint8_t ColtF;// ����������� ��
	uint8_t COLT;//  ����������� �� �� -40 �� +80
	uint8_t APressure;// �������� �� �������� ����������
	uint8_t TPS;// ��������� ��
	uint8_t O2S;// ����� �� ������
	uint8_t POWER;// ���������� �������� ����
	uint8_t RPM;// ������� ���������
	uint16_t TIME_INJ;// �������� ����� ������� �������
	uint16_t INJ_LAG;// ��� ��������
	uint16_t RqF;// �������� ������ ������� ���
	uint16_t RCO;//��������� ������� �� ��
	uint16_t MotoClock;//������� ���������
	
	uint8_t WARM_80;
	uint8_t WARM_20;
    union
    {
	uint8_t ExtSTAT;
        struct
        {
	unsigned LPG:1; //���
	unsigned ParParINJ:1; //�������-����������� ������
	unsigned SeqINJ:1; // ���������������� ������
	unsigned Sync_On:1;//������ ������� ���
	unsigned Res11:1;
	unsigned Res22:1; 
	unsigned Res33:1;
	unsigned CutOff:1;//������� �������
        };
    };	
	uint8_t WARM_40;
	uint8_t WARM_50;
	uint8_t Accel_ENR;
	uint8_t WARM_12;
	uint8_t reserv2;
    union
{
	uint8_t ExtConfig;
	struct
	{
	unsigned MAPtype:1;
	unsigned CYL:3;
	unsigned CLTtype:1;
	unsigned ereserv:3;	
	};
};
	uint8_t UOZ;
	uint8_t IAT; 
	uint8_t SPD_Enr;//��������� ������� �� �������� �������������
	uint8_t Warm_Enr;//���������� ����� �� ��������
    	uint8_t CHK_SUM_TX;//����������� �����
}ECU_struct;    //	

typedef struct {
		uint8_t BYTEA5; //0xA5
		uint8_t Byte_Counter;//0x01
		uint8_t Paket_version;//0x10 
		uint8_t Paket_size; //25
		
		uint16_t	RqFb;	
		uint16_t	RqFc;		
		uint8_t		RCO;		
		
		
	union
		{
			uint8_t WarmTable[5];
		struct
			{
			uint8_t Warm_m_40;
			uint8_t Warm_m_12;
			uint8_t Warm_p_20;
			uint8_t Warm_p_50;
			uint8_t Warm_p_80;
			} WarmStruct;
		};
	union
		{
			uint8_t VE_Table[16];
		struct
			{
			uint8_t VE400;
			uint8_t VE800;
			uint8_t VE1200;
			uint8_t VE1600;
			uint8_t VE2400;
			uint8_t VE3200;
			uint8_t VE4000;
			uint8_t VE4800;
			uint8_t VE5600;
			uint8_t VE6400;
			}VEStrct;
	
	
		};
		uint8_t CHK_SUM_TX;//����������� �����	
	} Cal_struct;	

typedef struct {
		uint8_t BYTEAA; //0xAA
		uint8_t BYTE55; //0x55
		uint8_t Byte_Counter;//0x02
		uint8_t Paket_version;//0x10 
		uint8_t Paket_size; //38

		union
		{
			char Cyl1[3];
			struct
			{
				char MAP1;
				char EHXST1;
				int SPD1;
			};
		};		
		union
		{
			char Cyl2[3];
			struct
			{
				char MAP2;
				char EHXST2;
				int SPD2;
			};
		};				
		union
		{
			char Cyl3[3];
			struct
			{
				char MAP3;
				char EHXST3;
				int SPD3;
			};
		};	
		union
		{
			char Cyl4[3];
			struct
			{
				char MAP4;
				char EHXST4;
				int SPD4;
			};
		};	
		union
		{
			char Cyl5[3];
			struct
			{
				char MAP5;
				char EHXST5;
				int SPD5;
			};
		};	
		union
		{
			char Cyl6[3];
			struct
			{
				char MAP6;
				char EHXST6;
				int SPD6;
			};
		};	
		union
		{
			char Cyl7[3];
			struct
			{
				char MAP7;
				char EHXST7;
				int SPD7;
			};
		};	
		union
		{
			char Cyl8[3];
			struct
			{
				char MAP8;
				char EHXST8;
				int SPD8;
			};
		};
		uint8_t CHK_SUM_TX;//����������� �����		
} MtrTst_struct;	


typedef struct {
		uint8_t VE400;
		uint8_t VE800;
		uint8_t VE1200;
		uint8_t VE1600;
		uint8_t VE2400;
		uint8_t VE3200;
		uint8_t VE4000;
		uint8_t VE4800;
		uint8_t VE5600;
		uint8_t VE6400;
	
	} VE_struct;

void delay(unsigned int ms);


