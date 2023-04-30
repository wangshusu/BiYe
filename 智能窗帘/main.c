#include <reg52.h>
#include <intrins.h>

#define uchar unsigned char		// �Ժ�unsigned char�Ϳ�����uchar����
#define uint  unsigned int		// �Ժ�unsigned int �Ϳ�����uint ����

sbit CS=P1^6;		//CS����ΪP3�ڵĵ�2λ�ţ�����ADC0832CS��  PCB
sbit SCL=P1^7;	//SCL����ΪP3�ڵĵ�3λ�ţ�����ADC0832SCL��
sbit DO=P3^0;		//DO����ΪP3�ڵĵ�4λ�ţ�����ADC0832DO��
	
sbit IRIN = P3^2;         //���������������
uchar IRCOM[7];							//�ɹ����ձ�־

sbit SCK_P      = P1^0;				// ʱ��оƬDS1302��SCK�ܽ�
sbit SDA_P      = P1^1;				// ʱ��оƬDS1302��SDA�ܽ�
sbit RST_P      = P1^2;				// ʱ��оƬDS1302��RST�ܽ�
sbit LcdRs_P    = P1^3;       // 1602Һ����RS�ܽ�       
sbit LcdRw_P    = P1^4;       // 1602Һ����RW�ܽ� 
sbit LcdEn_P    = P1^5;       // 1602Һ����EN�ܽ�
sbit KeyMode_P  = P3^3;				// ģʽ�л�
sbit KeySet_P   = P3^4;				// ����ʱ�䰴��
sbit KeySet2_P  = P3^5;				// ����ʱ��ģʽ�Ŀ���ʱ��͹��տ���ǿ��
sbit KeyDown_P  = P3^6;				// ������
sbit KeyUp_P    = P3^7;				// �Ӱ���
sbit Led_P      = P2^0;				// ָʾ��

uchar gMode=1;								// 1���ֶ�ģʽ��2��ʱ���Զ�ģʽ��3�������Զ�ģʽ
uchar OpenHour    = 18;				// ����������Сʱ
uchar OpenMinute  = 20;				// ���������ķ���
uchar CloseHour   = 10;				// �رմ�����Сʱ
uchar CloseMinute = 30;				// �رմ����ķ���
uchar gLight      = 40;				// �������ص���ֵ

uchar code Clock[]={0x10,0x20,0x40,0x80}; 			// �������˳ʱ����ת����
uchar code AntiClock[]={0x80,0x40,0x20,0x10};		// ���������ʱ����ת����

uchar TimeBuff[7]={17,9,1,6,18,30,40};					// ʱ�����飬Ĭ��2017��9��1�գ������壬18:30:40
// TimeBuff[0] ������ݣ���Χ00-99
// TimeBuff[1] �����·ݣ���Χ1-12
// TimeBuff[2] �������ڣ���Χ1-31
// TimeBuff[3] �������ڣ���Χ1-7
// TimeBuff[4] ����Сʱ����Χ00-23
// TimeBuff[5] ������ӣ���Χ00-59
// TimeBuff[6] �������ӣ���Χ00-59

/*********************************************************/
// ���뼶����ʱ������time��Ҫ��ʱ�ĺ�����
/*********************************************************/
void DelayMs(uint time)
{
	uint i,j;
	for(i=0;i<time;i++)
		for(j=0;j<112;j++);
}

/*********************************************************/
// 1602Һ��д�������cmd����Ҫд�������
/*********************************************************/
void LcdWriteCmd(uchar cmd)
{ 
	LcdRs_P = 0;
	LcdRw_P = 0;
	LcdEn_P = 0;
	P0=cmd;
	DelayMs(2);
	LcdEn_P = 1;    
	DelayMs(2);
	LcdEn_P = 0;	
}


/*********************************************************/
// 1602Һ��д���ݺ�����dat����Ҫд�������
/*********************************************************/
void LcdWriteData(uchar dat)
{
	LcdRs_P = 1; 
	LcdRw_P = 0;
	LcdEn_P = 0;
	P0=dat;
	DelayMs(2);
	LcdEn_P = 1;    
	DelayMs(2);
	LcdEn_P = 0;
}


/*********************************************************/
// 1602Һ����ʼ������
/*********************************************************/
void LcdInit()
{
	LcdWriteCmd(0x38);        // 16*2��ʾ��5*7����8λ���ݿ�
	LcdWriteCmd(0x0C);        // ����ʾ������ʾ���
	LcdWriteCmd(0x06);        // ��ַ��1����д�����ݺ�������
	LcdWriteCmd(0x01);        // ����
}


/*********************************************************/
// Һ����궨λ����
/*********************************************************/
void LcdGotoXY(uchar line,uchar column)
{
	// ��һ��
	if(line==0)        
		LcdWriteCmd(0x80+column); 
	// �ڶ���
	if(line==1)        
		LcdWriteCmd(0x80+0x40+column); 
}


/*********************************************************/
// Һ������ַ�������
/*********************************************************/
void LcdPrintStr(uchar *str)
{
	while(*str!='\0')
			LcdWriteData(*str++);
}


/*********************************************************/
// Һ���������(0-99)
/*********************************************************/
void LcdPrintNum(uchar num)
{
	LcdWriteData(num/10+48);		// ʮλ
	LcdWriteData(num%10+48); 		// ��λ
}


/*********************************************************/
// ��ʾģʽ
/*********************************************************/
void LcdPrintMode(uchar num)
{
	switch(num)			
	{
		case 1: LcdPrintStr("Manual");	break;
		case 2: LcdPrintStr("Timing");	break;
		case 3: LcdPrintStr("Liging");	break;
		default:												break;
	}
}


/*********************************************************/
// Һ����ʾ���ݵĳ�ʼ��
/*********************************************************/
void LcdShowInit()
{
	LcdGotoXY(0,0);
	LcdPrintStr("20  -  -     :  ");
	LcdGotoXY(1,0);
	LcdPrintStr("           gz:  ");
	LcdGotoXY(1,0);
	LcdPrintMode(gMode);
}



/*********************************************************/
// ˢ��ʱ����ʾ
/*********************************************************/
void FlashTime()
{
	LcdGotoXY(0,2);										// ���
	LcdPrintNum(TimeBuff[0]);
	LcdGotoXY(0,5);										// �·�
	LcdPrintNum(TimeBuff[1]);
	LcdGotoXY(0,8);										// ����
	LcdPrintNum(TimeBuff[2]);
	LcdGotoXY(0,11);									// Сʱ
	LcdPrintNum(TimeBuff[4]);
	LcdGotoXY(0,14);									// ����
	LcdPrintNum(TimeBuff[5]);
	LcdGotoXY(0,13);									// ����
	if(TimeBuff[6]%2==0)							// ������ż����ʾð��
		LcdWriteData(':');
	else															// ������������ʾ�ո�
		LcdWriteData(' ');
}


/*********************************************************/
// ��ʼ��DS1302
/*********************************************************/
void DS1302_Init(void)
{
	RST_P=0;			// RST���õ�
	SCK_P=0;			// SCK���õ�
	SDA_P=0;			// SDA���õ�				
}


/*********************************************************/
// ��DS1302����һ�ֽ�����
/*********************************************************/
uchar DS1302_Read_Byte(uchar addr) 
{
	uchar i;
	uchar temp;
	
	RST_P=1;								
	
	/* д��Ŀ���ַ��addr*/
	for(i=0;i<8;i++) 
	{     
		if(addr&0x01) 
			SDA_P=1;
		else 
			SDA_P=0;
		
		SCK_P=1;
		_nop_();
		SCK_P=0;
		_nop_();
		
		addr=addr>> 1;
	}
	
	/* �����õ�ַ������ */
	for(i=0;i<8;i++) 
	{
		temp=temp>>1;
		
		if(SDA_P) 
			temp|= 0x80;
		else 
			temp&=0x7F;
		
		SCK_P=1;
		_nop_();
		SCK_P=0;
		_nop_();
	}
	
	RST_P=0;
	
	return temp;
}


/*********************************************************/
// ��DS1302д��һ�ֽ�����
/*********************************************************/
void DS1302_Write_Byte(uchar addr, uchar dat)
{
	uchar i;
	
	RST_P = 1;
	
	/* д��Ŀ���ַ��addr*/
	for(i=0;i<8;i++) 
	{ 
		if(addr&0x01) 
			SDA_P=1;
		else 
			SDA_P=0;

		SCK_P=1;
		_nop_();
		SCK_P=0;
		_nop_();
		
		addr=addr>>1;
	}
	
	/* д�����ݣ�dat*/
	for(i=0;i<8;i++) 
	{
		if(dat&0x01) 
			SDA_P=1;
		else 
			SDA_P=0;
	
		SCK_P=1;
		_nop_();
		SCK_P=0;
		_nop_();
		
		dat=dat>>1;
	}
	
	RST_P=0;					
}
/*********************************************************/
// ��DS1302д��ʱ������
/*********************************************************/
void DS1302_Write_Time() 
{
  uchar i;
	uchar temp1;
	uchar temp2;
	
	for(i=0;i<7;i++)			// ʮ����תBCD��
	{
		temp1=(TimeBuff[i]/10)<<4;
		temp2=TimeBuff[i]%10;
		TimeBuff[i]=temp1+temp2;
	}
	
	DS1302_Write_Byte(0x8E,0x00);								// �ر�д���� 
	DS1302_Write_Byte(0x80,0x80);								// ��ͣʱ�� 
	DS1302_Write_Byte(0x8C,TimeBuff[0]);				// �� 
	DS1302_Write_Byte(0x88,TimeBuff[1]);				// �� 
	DS1302_Write_Byte(0x86,TimeBuff[2]);				// �� 
	DS1302_Write_Byte(0x8A,TimeBuff[3]);				// ����
	DS1302_Write_Byte(0x84,TimeBuff[4]);				// ʱ 
	DS1302_Write_Byte(0x82,TimeBuff[5]);				// ��
	DS1302_Write_Byte(0x80,TimeBuff[6]);				// ��
	DS1302_Write_Byte(0x80,TimeBuff[6]&0x7F);		// ����ʱ��
	DS1302_Write_Byte(0x8E,0x80);								// ��д����  
}



/*********************************************************/
// ��DS1302����ʱ������
/*********************************************************/
void DS1302_Read_Time()  
{ 
	uchar i;

	TimeBuff[0]=DS1302_Read_Byte(0x8D);						// �� 
	TimeBuff[1]=DS1302_Read_Byte(0x89);						// �� 
	TimeBuff[2]=DS1302_Read_Byte(0x87);						// �� 
	TimeBuff[3]=DS1302_Read_Byte(0x8B);						// ����
	TimeBuff[4]=DS1302_Read_Byte(0x85);						// ʱ 
	TimeBuff[5]=DS1302_Read_Byte(0x83);						// �� 
	TimeBuff[6]=(DS1302_Read_Byte(0x81))&0x7F;		// �� 

	for(i=0;i<7;i++)		// BCDתʮ����
	{           
		TimeBuff[i]=(TimeBuff[i]/16)*10+TimeBuff[i]%16;
	}
}
unsigned char ad0832read(bit SGL,bit ODD)
{
	unsigned char i=0,value=0,value1=0;		
		SCL=0;
		DO=1;
		CS=0;		//��ʼ
		SCL=1;		//��һ��������	
		SCL=0;
		DO=SGL;
		SCL=1;  	//�ڶ���������
		SCL=0;
		DO=ODD;
		SCL=1;	    //������������
		SCL=0;	    //�������½���
		DO=1;
		for(i=0;i<8;i++)
		{
			SCL=1;
			SCL=0; //��ʼ�ӵ��ĸ��½��ؽ�������
			value<<=1;
			if(DO)
				value++;						
		}
		for(i=0;i<8;i++)
		{			//����У������
			value1>>=1;
			if(DO)
				value1+=0x80;
			SCL=1;
			SCL=0;
		}
		CS=1;
		SCL=1;	
		if(value==value1)				//��У�����ݱȽϣ���ȷ�ͷ������ݣ����򷵻�0	
			return value;
	return 0;
}
/*********************************************************/
// ����ɨ��(���õ�ǰʱ��)
/*********************************************************/
void KeyScanf1()
{
	if(KeySet_P==0)
	{
		LcdGotoXY(0,13);				// ��ʾ���ӵ�ð��
		LcdWriteData(':');
		LcdWriteCmd(0x0f);			// ���������˸
		LcdGotoXY(0,3);					// ��λ��굽�����˸
		DelayMs(10);						// ��ʱ�ȴ��������������µĶ���
		while(!KeySet_P);				// �ȴ������ͷ�
		DelayMs(10);						// ��ʱ�ȴ������������ɿ��Ķ���
		
		/* ������� */
		while(1)
		{
			if(KeyDown_P==0)							// �������������ȥ
			{
				if(TimeBuff[0]>0)						// �ж�����Ƿ����0		
					TimeBuff[0]--;						// �ǵĻ��ͼ�ȥ1
				LcdGotoXY(0,2);							// ��궨λ����ݵ�λ��
				LcdPrintNum(TimeBuff[0]);		// ˢ����ʾ�ı������
				LcdGotoXY(0,3);							// ��λ��굽�����˸
				DelayMs(300);								// ��ʱ0.3������
			}
			
			if(KeyUp_P==0)								// ����Ӱ�������ȥ
			{
				if(TimeBuff[0]<99)					// �ж�����Ƿ�С��99
					TimeBuff[0]++;						// �ǵĻ��ͼ���1
				LcdGotoXY(0,2);							// ��궨λ����ݵ�λ��
				LcdPrintNum(TimeBuff[0]);		// ˢ����ʾ�ı������
				LcdGotoXY(0,3);							// ��λ��굽�����˸
				DelayMs(300);								// ��ʱ0.3������
			}
			
			if(KeySet_P==0)
			{
				break;
			}
		}
		
		LcdGotoXY(0,6);					// ��λ��굽�·���˸
		DelayMs(10);						// ��ʱ�ȴ��������������µĶ���
		while(!KeySet_P);				// �ȴ������ͷ�
		DelayMs(10);						// ��ʱ�ȴ������������ɿ��Ķ���
			
		/* �����·� */
		while(1)
		{
			if(KeyDown_P==0)							// �������������ȥ
			{
				if(TimeBuff[1]>1)						// �ж��·��Ƿ����1		
					TimeBuff[1]--;						// �ǵĻ��ͼ�ȥ1
				LcdGotoXY(0,5);							// ��궨λ���·ݵ�λ��
				LcdPrintNum(TimeBuff[1]);		// ˢ����ʾ�ı����·�
				LcdGotoXY(0,6);							// ��λ��굽�·���˸
				DelayMs(300);								// ��ʱ0.3������
			}
			
			if(KeyUp_P==0)								// ����Ӱ�������ȥ
			{
				if(TimeBuff[1]<12)					// �ж��·��Ƿ�С��12
					TimeBuff[1]++;						// �ǵĻ��ͼ���1
				LcdGotoXY(0,5);							// ��궨λ���·ݵ�λ��
				LcdPrintNum(TimeBuff[1]);		// ˢ����ʾ�ı����·�
				LcdGotoXY(0,6);							// ��λ��굽�·���˸
				DelayMs(300);								// ��ʱ0.3������
			}
			
			if(KeySet_P==0)
			{
				break;
			}
		}
		
		LcdGotoXY(0,9);					// ��λ��굽������˸
		DelayMs(10);						// ��ʱ�ȴ��������������µĶ���
		while(!KeySet_P);				// �ȴ������ͷ�
		DelayMs(10);						// ��ʱ�ȴ������������ɿ��Ķ���
		
		/* �������� */
		while(1)
		{
			if(KeyDown_P==0)							// �������������ȥ
			{
				if(TimeBuff[2]>1)						// �ж������Ƿ����1		
					TimeBuff[2]--;						// �ǵĻ��ͼ�ȥ1
				LcdGotoXY(0,8);							// ��궨λ�����ڵ�λ��
				LcdPrintNum(TimeBuff[2]);		// ˢ����ʾ�ı�������
				LcdGotoXY(0,9);							// ��λ��굽������˸
				DelayMs(300);								// ��ʱ0.3������
			}
			
			if(KeyUp_P==0)								// ����Ӱ�������ȥ
			{
				if(TimeBuff[2]<31)					// �ж������Ƿ�С��31
					TimeBuff[2]++;						// �ǵĻ��ͼ���1
				LcdGotoXY(0,8);							// ��궨λ�����ڵ�λ��
				LcdPrintNum(TimeBuff[2]);		// ˢ����ʾ�ı�������
				LcdGotoXY(0,9);							// ��λ��굽������˸
				DelayMs(300);								// ��ʱ0.3������
			}
			
			if(KeySet_P==0)
			{
				break;
			}
		}
		
		LcdGotoXY(0,12);				// ��λ��굽Сʱ��˸
		DelayMs(10);						// ��ʱ�ȴ��������������µĶ���
		while(!KeySet_P);				// �ȴ������ͷ�
		DelayMs(10);						// ��ʱ�ȴ������������ɿ��Ķ���
		
		
		/* ����Сʱ */
		while(1)
		{
			if(KeyDown_P==0)							// �������������ȥ
			{
				if(TimeBuff[4]>0)						// �ж�Сʱ�Ƿ����0
					TimeBuff[4]--;						// �ǵĻ��ͼ�ȥ1
				LcdGotoXY(0,11);						// ��궨λ��Сʱ��λ��
				LcdPrintNum(TimeBuff[4]);		// ˢ����ʾ�ı���Сʱ
				LcdGotoXY(0,12);						// ��λ��굽Сʱ��˸
				DelayMs(300);								// ��ʱ0.3������
			}
			
			if(KeyUp_P==0)								// ����Ӱ�������ȥ
			{
				if(TimeBuff[4]<23)					// �ж�Сʱ�Ƿ�С��23
					TimeBuff[4]++;						// �ǵĻ��ͼ���1
				LcdGotoXY(0,11);						// ��궨λ��Сʱ��λ��
				LcdPrintNum(TimeBuff[4]);		// ˢ����ʾ�ı���Сʱ
				LcdGotoXY(0,12);						// ��λ��굽Сʱ��˸
				DelayMs(300);								// ��ʱ0.3������
			}
			
			if(KeySet_P==0)
			{
				break;
			}
		}
		
		LcdGotoXY(0,15);				// ��λ��굽������˸
		DelayMs(10);						// ��ʱ�ȴ��������������µĶ���
		while(!KeySet_P);				// �ȴ������ͷ�
		DelayMs(10);						// ��ʱ�ȴ������������ɿ��Ķ���
		
		/* �������� */
		while(1)
		{
			if(KeyDown_P==0)							// �������������ȥ
			{
				if(TimeBuff[5]>0)						// �жϷ����Ƿ����0
					TimeBuff[5]--;						// �ǵĻ��ͼ�ȥ1
				LcdGotoXY(0,14);						// ��궨λ�����ӵ�λ��
				LcdPrintNum(TimeBuff[5]);		// ˢ����ʾ�ı��ķ���
				LcdGotoXY(0,15);						// ��λ��굽������˸
				DelayMs(300);								// ��ʱ0.3������
			}
			
			if(KeyUp_P==0)								// ����Ӱ�������ȥ
			{
				if(TimeBuff[5]<59)					// �жϷ����Ƿ�С��59
					TimeBuff[5]++;						// �ǵĻ��ͼ���1
				LcdGotoXY(0,14);						// ��궨λ�����ӵ�λ��
				LcdPrintNum(TimeBuff[5]);		// ˢ����ʾ�ı��ķ���
				LcdGotoXY(0,15);						// ��λ��굽������˸
				DelayMs(300);								// ��ʱ0.3������
			}
			
			if(KeySet_P==0)
			{
				break;
			}
		}
		
		/* �˳�ǰ������ */
		LcdWriteCmd(0x0C);			// �رչ����˸
		DS1302_Write_Time();		// �������õ�ʱ��ֵ����DS1302оƬ
		DelayMs(10);						// ��ʱ�ȴ��������������µĶ���
		while(!KeySet_P);				// �ȴ������ͷ�
		DelayMs(10);						// ��ʱ�ȴ������������ɿ��Ķ���
	}
}


/*********************************************************/
// ����ɨ��(���ô����Ķ���)
/*********************************************************/
void KeyScanf2()
{
	if(KeySet2_P==0)
	{
		LcdGotoXY(0,0);										// ��궨λ
		LcdPrintStr(" OpenTime   :   ");	// ��ʾ��1������
		LcdGotoXY(1,0);										// ��궨λ
		LcdPrintStr("CloseTime   :   ");	// ��ʾ��2������
		LcdGotoXY(0,10);									// ��궨λ
		LcdPrintNum(OpenHour);						// ��ʾ����������Сʱ
		LcdGotoXY(0,13);									// ��궨λ
		LcdPrintNum(OpenMinute);					// ��ʾ���������ķ���
		LcdGotoXY(1,10);									// ��궨λ
		LcdPrintNum(CloseHour);						// ��ʾ�رմ�����Сʱ
		LcdGotoXY(1,13);									// ��궨λ
		LcdPrintNum(CloseMinute);					// ��ʾ�رմ����ķ���		
		
		LcdWriteCmd(0x0f);							// ���������˸
		LcdGotoXY(0,11);								// ��λ���
		DelayMs(10);										// ��ʱ�ȴ��������������µĶ���
		while(!KeySet2_P);							// �ȴ������ͷ�
		DelayMs(10);										// ��ʱ�ȴ������������ɿ��Ķ���
		
		/* ����������Сʱ */
		while(1)
		{
			if(KeyDown_P==0)							// �������������ȥ
			{
				if(OpenHour>0)							// �ж�Сʱ�Ƿ����0		
					OpenHour--;								// �ǵĻ��ͼ�ȥ1
				LcdGotoXY(0,10);						// ��궨λ
				LcdPrintNum(OpenHour);			// ˢ����ʾ�ı���Сʱ
				LcdGotoXY(0,11);						// ��λ���
				DelayMs(300);								// ��ʱ0.3������
			}
			
			if(KeyUp_P==0)								// ����Ӱ�������ȥ
			{
				if(OpenHour<23)							// �ж�Сʱ�Ƿ�С��23
					OpenHour++;								// �ǵĻ��ͼ���1
				LcdGotoXY(0,10);						// ��궨λ
				LcdPrintNum(OpenHour);			// ˢ����ʾ�ı���Сʱ
				LcdGotoXY(0,11);						// ��λ���
				DelayMs(300);								// ��ʱ0.3������
			}
			
			if(KeySet2_P==0)
			{
				break;
			}
		}
		
		LcdGotoXY(0,14);								// ��λ���
		DelayMs(10);										// ��ʱ�ȴ��������������µĶ���
		while(!KeySet2_P);							// �ȴ������ͷ�
		DelayMs(10);										// ��ʱ�ȴ������������ɿ��Ķ���
		
		/* ���������ķ��� */
		while(1)
		{
			if(KeyDown_P==0)							// �������������ȥ
			{
				if(OpenMinute>0)						// �жϷ����Ƿ����0
					OpenMinute--;							// �ǵĻ��ͼ�ȥ1
				LcdGotoXY(0,13);						// ��궨λ
				LcdPrintNum(OpenMinute);		// ˢ����ʾ�ı��ķ���
				LcdGotoXY(0,14);						// ��λ���
				DelayMs(300);								// ��ʱ0.3������
			}
			
			if(KeyUp_P==0)								// ����Ӱ�������ȥ
			{
				if(OpenMinute<59)						// �жϷ����Ƿ�С��59
					OpenMinute++;							// �ǵĻ��ͼ���1
				LcdGotoXY(0,13);						// ��궨λ
				LcdPrintNum(OpenMinute);		// ˢ����ʾ�ı��ķ���
				LcdGotoXY(0,14);						// ��λ���
				DelayMs(300);								// ��ʱ0.3������
			}
			
			if(KeySet2_P==0)
			{
				break;
			}
		}
		
		LcdGotoXY(1,11);								// ��λ���
		DelayMs(10);										// ��ʱ�ȴ��������������µĶ���
		while(!KeySet2_P);							// �ȴ������ͷ�
		DelayMs(10);										// ��ʱ�ȴ������������ɿ��Ķ���
		
		/* �����رյ�Сʱ */
		while(1)
		{
			if(KeyDown_P==0)							// �������������ȥ
			{
				if(CloseHour>0)							// �ж�Сʱ�Ƿ����0		
					CloseHour--;							// �ǵĻ��ͼ�ȥ1
				LcdGotoXY(1,10);						// ��궨λ
				LcdPrintNum(CloseHour);			// ˢ����ʾ�ı���Сʱ
				LcdGotoXY(1,11);						// ��λ���
				DelayMs(300);								// ��ʱ0.3������
			}
			
			if(KeyUp_P==0)								// ����Ӱ�������ȥ
			{
				if(CloseHour<23)						// �ж�Сʱ�Ƿ�С��23
					CloseHour++;							// �ǵĻ��ͼ���1
				LcdGotoXY(1,10);						// ��궨λ
				LcdPrintNum(CloseHour);			// ˢ����ʾ�ı���Сʱ
				LcdGotoXY(1,11);						// ��λ���
				DelayMs(300);								// ��ʱ0.3������
			}
			
			if(KeySet2_P==0)
			{
				break;
			}
		}
		
		LcdGotoXY(1,14);								// ��λ���
		DelayMs(10);										// ��ʱ�ȴ��������������µĶ���
		while(!KeySet2_P);							// �ȴ������ͷ�
		DelayMs(10);										// ��ʱ�ȴ������������ɿ��Ķ���
		
		/* �����رյķ��� */
		while(1)
		{
			if(KeyDown_P==0)							// �������������ȥ
			{
				if(CloseMinute>0)						// �жϷ����Ƿ����0
					CloseMinute--;						// �ǵĻ��ͼ�ȥ1
				LcdGotoXY(1,13);						// ��궨λ
				LcdPrintNum(CloseMinute);		// ˢ����ʾ�ı��ķ���
				LcdGotoXY(1,14);						// ��λ���
				DelayMs(300);								// ��ʱ0.3������
			}
			
			if(KeyUp_P==0)								// ����Ӱ�������ȥ
			{
				if(CloseMinute<59)					// �жϷ����Ƿ�С��59
					CloseMinute++;						// �ǵĻ��ͼ���1
				LcdGotoXY(1,13);						// ��궨λ
				LcdPrintNum(CloseMinute);		// ˢ����ʾ�ı��ķ���
				LcdGotoXY(1,14);						// ��λ���
				DelayMs(300);								// ��ʱ0.3������
			}
			
			if(KeySet2_P==0)
			{
				break;
			}
		}
		
		DelayMs(10);										// ��ʱ�ȴ��������������µĶ���
		while(!KeySet2_P);							// �ȴ������ͷ�
		DelayMs(10);										// ��ʱ�ȴ������������ɿ��Ķ���
		
		/* ����ǿ�ȵ����� */
		LcdWriteCmd(0x0C);								// �رչ����˸
		LcdGotoXY(0,0);										// ��궨λ
		LcdPrintStr("   Light Set    ");	// ��ʾ��1������
		LcdGotoXY(1,0);										// ��궨λ
		LcdPrintStr("                ");	// ��ʾ��2������
		LcdGotoXY(1,7);										// ��궨λ
		LcdPrintNum(gLight);							// ��ʾ�����Ĺ��߿���ǿ����ֵ
		
		while(1)
		{
			if(KeyDown_P==0)							// �������������ȥ
			{
				if(gLight>0)								// �жϹ�����ֵ�Ƿ����0
					gLight--;									// �ǵĻ��ͼ�ȥ1
				LcdGotoXY(1,7);							// ��궨λ
				LcdPrintNum(gLight);				// ˢ����ʾ�ı��Ĺ�����ֵ
				DelayMs(300);								// ��ʱ0.3������
			}
			
			if(KeyUp_P==0)								// ����Ӱ�������ȥ
			{
				if(gLight<99)								// �жϹ�����ֵ�Ƿ�С��59
					gLight++;									// �ǵĻ��ͼ���1
				LcdGotoXY(1,7);							// ��궨λ
				LcdPrintNum(gLight);				// ˢ����ʾ�ı��Ĺ�����ֵ
				DelayMs(300);								// ��ʱ0.3������
			}
			
			if(KeySet2_P==0)
			{
				break;
			}
		}
		
		/* �˳�ǰ������ */
		LcdShowInit();					// Һ����ʾ���ݳ�ʼ��
		DelayMs(10);						// ��ʱ�ȴ��������������µĶ���
		while(!KeySet2_P);			// �ȴ������ͷ�
		DelayMs(10);						// ��ʱ�ȴ������������ɿ��Ķ���
	}
}


/*********************************************************/
// ����ɨ��(ģʽ�л�)
/*********************************************************/
void KeyScanf3()
{
	if(KeyMode_P==0)
	{
		gMode++;							// �л�����һģʽ
		if(gMode==4)					// �������ͷ��
			gMode=1;						// �ص���һ��ģʽ
		LcdGotoXY(1,0);				// ��궨λ
		LcdPrintMode(gMode);	// ��ʾģʽ
		DelayMs(10);					// ȥ���������µĶ���
		while(!KeyMode_P);		// �ȴ������Ƿ�
		DelayMs(10);					// ȥ�������ɿ��Ķ���
	}
}


/*********************************************************/
// ����0x10,0x20,0x40,0x80
/*********************************************************/
sbit dj1=P2^7;
sbit dj2=P2^6;
sbit dj3=P2^5;
sbit dj4=P2^4;
void Open()
{
	uint i,j;
	for(j=0;j<220;j++)		// ���Ʋ��������ת
	{
		for(i=0;i<4;i++)
		{
			//P2=Clock[i];
			if(i==0){dj4=1;dj3=0;dj2=0;dj1=0;}
			if(i==1){dj4=0;dj3=1;dj2=0;dj1=0;}
			if(i==2){dj4=0;dj3=0;dj2=1;dj1=0;}
			if(i==3){dj4=0;dj3=0;dj2=0;dj1=1;}
			DelayMs(3);
		}
	}
	Led_P=0;
}



/*********************************************************/
// �ش�
/*********************************************************/
void Close()
{
	uint i,j;
	for(j=0;j<220;j++)		// ���Ʋ��������ת
	{
		for(i=0;i<4;i++)
		{
			//P2=AntiClock[i];
			if(i==0){dj4=0;dj3=0;dj2=0;dj1=1;}
			if(i==1){dj4=0;dj3=0;dj2=1;dj1=0;}
			if(i==2){dj4=0;dj3=1;dj2=0;dj1=0;}
			if(i==3){dj4=1;dj3=0;dj2=0;dj1=0;}
			DelayMs(3);
		}
	}
	Led_P=1;
}



void init()//��ʼ������
{
    IE = 0x81;                 //�������ж��ж�,ʹ�� INT0 �ⲿ�ж�
    TCON = 0x01;               //������ʽΪ���帺���ش���
    
    IRIN=1;                    //I/O�ڳ�ʼ��
}
/*********************************************************/
// ������
/*********************************************************/
void main()
{
	uchar light;
	init();
	LcdInit();			// ִ��Һ����ʼ��	
	DS1302_Init();	// ʱ��оƬ�ĳ�ʼ��
	LcdShowInit();	// Һ����ʾ���ݵĳ�ʼ��
	
	if(DS1302_Read_Byte(0x81)>=128)			// �ж�ʱ��оƬ�Ƿ���������
	{
		DS1302_Write_Time();							// ���û�У����ʼ��һ��ʱ��
	}
	
	while(1)
	{
		DS1302_Read_Time();				// ��ȡ��ǰʱ��оƬ��ʱ�䣬��������time_buf��
		FlashTime();							// ˢ��ʱ����ʾ

		light=ad0832read(1,0);//Get_ADC0832();			// ��ȡ����ǿ��
		light=light/2.5;					// ��С���ռ����(��0-99)
		if(light>99)							// �������99
			light=99;								// ����Ȼ����99
		LcdGotoXY(1,14);					// ��궨λ
		LcdPrintNum(light);				// ��ʾ����ǿ��
		
		KeyScanf1();							// ����ɨ��(ʱ�������)
		KeyScanf2();							// ����ɨ��(��ֵ������)
		KeyScanf3();							// ����ɨ��(ģʽ�л�)
	
		/*�ֶ�����ģʽ*/
		if(gMode==1)
		{
			if(KeyDown_P==0)		// ����ش�����������	
			{
				if(Led_P==0)			// ���������ǰ�Ǵ򿪵�
				{
					Close();				// ��رմ���
				}
			}
			if(KeyUp_P==0)			// �����������������
			{
				if(Led_P==1)			// ���������ǰ�ǹرյ�
				{
					Open();					// ��򿪴���
				}
			}	
		}
		
		/*ʱ�����ģʽ*/
		if(gMode==2)
		{
			if((TimeBuff[4]==CloseHour)&&(TimeBuff[5]==CloseMinute)&&(TimeBuff[6]==0))	// ������˹ش�����ʱ��	
			{
				if(Led_P==0)			// ���������ǰ�Ǵ򿪵�
				{
					Close();				// ��رմ���
				}
			}
			if((TimeBuff[4]==OpenHour)&&(TimeBuff[5]==OpenMinute)&&(TimeBuff[6]==0))		// ������˿�������ʱ��	
			{
				if(Led_P==1)			// ���������ǰ�ǹرյ�
				{
					Open();					// ��򿪴���
				}
			}	
		}
		
		/*���߿���ģʽ*/
		if(gMode==3)
		{
			if(light<gLight)		// ��ǰ����С�����õ���ֵ
			{
				if(Led_P==0)			// ���������ǰ�Ǵ򿪵�
				{
					Close();				// ��رմ���
				}
			}
			else								// ��ǰ���ߴ��ڻ�������õ���ֵ
			{
				if(Led_P==1)			// ���������ǰ�ǹرյ�
				{
					Open();					// ��򿪴���
				}
			}	
		}
		
		DelayMs(100);							// ��ʱ0.1��
	}
}
void delay(unsigned char x)    //x*0.14MS
{
 unsigned char i;
  while(x--)
 {
  for (i = 0; i<13; i++) {}
 }
}
/**********************************************************/
void IR_IN() interrupt 0 using 0
{
  unsigned char j,k,N=0;
     EX0 = 0;   
	 delay(15);
	 if (IRIN==1) 
     { EX0 =1;
	   return;
	  } 
                           //ȷ��IR�źų���
  while (!IRIN)            //��IR��Ϊ�ߵ�ƽ������9ms��ǰ���͵�ƽ�źš�
    {delay(1);}

 for (j=0;j<4;j++)         //�ռ���������
 { 
  for (k=0;k<8;k++)        //ÿ��������8λ
  {
   while (IRIN)            //�� IR ��Ϊ�͵�ƽ������4.5ms��ǰ���ߵ�ƽ�źš�
     {delay(1);}
    while (!IRIN)          //�� IR ��Ϊ�ߵ�ƽ
     {delay(1);}
     while (IRIN)           //����IR�ߵ�ƽʱ��
      {
    delay(1);
    N++;           
    if (N>=30)
	 { EX0=1;
	 return;}                  //0.14ms���������Զ��뿪��
      }                        //�ߵ�ƽ�������                
     IRCOM[j]=IRCOM[j] >> 1;                  //�������λ����0��
     if (N>=8) {IRCOM[j] = IRCOM[j] | 0x80;}  //�������λ����1��
     N=0;
  }//end for k
 }//end for j
	if(IRCOM[2]==0x45)//ȫ��
	{
		if(gMode==1)
		{
			if(Led_P==0)			// ���������ǰ�Ǵ򿪵�
			{
				Close();				// ��رմ���
			}	
		}
	}

	if(IRCOM[2]==0x47)//ȫ��
	{
		if(gMode==1)
		{
			if(Led_P==1)			// ���������ǰ�ǹرյ�
			{
				Open();					// ��򿪴���
			}	
		}
	}

     EX0 = 1; 
} 

