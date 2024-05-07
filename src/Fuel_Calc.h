#include <xc.h>

long map(long x, long in_min, long in_max, long out_min, long out_max);
unsigned int ReadTable(unsigned int a,const unsigned  int* Massive);
int IntpRes(int Low_d,int Hi_d,unsigned char C);
int IntpResL(int Low_d,int Hi_d,unsigned char C);
int SMA_filter(unsigned int a,unsigned  int* Massive);
int EX_filter(unsigned int a,unsigned  int* Massive);
unsigned char Get_Table_tiny (unsigned char C,unsigned char *Table);
unsigned char Get_VE (unsigned char RPM,unsigned char *Table);
void Fuel_Calculate(void);
unsigned char VE(unsigned char a,unsigned char *Table);
void CheckInj(void);