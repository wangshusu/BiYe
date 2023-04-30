#include <reg52.h>
#include <intrins.h>

#define uchar unsigned char		// 以后unsigned char就可以用uchar代替
#define uint  unsigned int		// 以后unsigned int 就可以用uint 代替

sbit CS=P1^6;		//CS定义为P3口的第2位脚，连接ADC0832CS脚  PCB
sbit SCL=P1^7;	//SCL定义为P3口的第3位脚，连接ADC0832SCL脚
sbit DO=P3^0;		//DO定义为P3口的第4位脚，连接ADC0832DO脚
	
sbit IRIN = P3^2;         //红外接收器数据线
uchar IRCOM[7];							//成功接收标志

sbit SCK_P      = P1^0;				// 时钟芯片DS1302的SCK管脚
sbit SDA_P      = P1^1;				// 时钟芯片DS1302的SDA管脚
sbit RST_P      = P1^2;				// 时钟芯片DS1302的RST管脚
sbit LcdRs_P    = P1^3;       // 1602液晶的RS管脚       
sbit LcdRw_P    = P1^4;       // 1602液晶的RW管脚 
sbit LcdEn_P    = P1^5;       // 1602液晶的EN管脚
sbit KeyMode_P  = P3^3;				// 模式切换
sbit KeySet_P   = P3^4;				// 设置时间按键
sbit KeySet2_P  = P3^5;				// 设置时间模式的开关时间和光照控制强度
sbit KeyDown_P  = P3^6;				// 减按键
sbit KeyUp_P    = P3^7;				// 加按键
sbit Led_P      = P2^0;				// 指示灯

uchar gMode=1;								// 1是手动模式，2是时间自动模式，3是亮度自动模式
uchar OpenHour    = 18;				// 开启窗帘的小时
uchar OpenMinute  = 20;				// 开启窗帘的分钟
uchar CloseHour   = 10;				// 关闭窗帘的小时
uchar CloseMinute = 30;				// 关闭窗帘的分钟
uchar gLight      = 40;				// 窗帘开关的阈值

uchar code Clock[]={0x10,0x20,0x40,0x80}; 			// 步进电机顺时针旋转数组
uchar code AntiClock[]={0x80,0x40,0x20,0x10};		// 步进电机逆时针旋转数组

uchar TimeBuff[7]={17,9,1,6,18,30,40};					// 时间数组，默认2017年9月1日，星期五，18:30:40
// TimeBuff[0] 代表年份，范围00-99
// TimeBuff[1] 代表月份，范围1-12
// TimeBuff[2] 代表日期，范围1-31
// TimeBuff[3] 代表星期，范围1-7
// TimeBuff[4] 代表小时，范围00-23
// TimeBuff[5] 代表分钟，范围00-59
// TimeBuff[6] 代表秒钟，范围00-59

/*********************************************************/
// 毫秒级的延时函数，time是要延时的毫秒数
/*********************************************************/
void DelayMs(uint time)
{
	uint i,j;
	for(i=0;i<time;i++)
		for(j=0;j<112;j++);
}

/*********************************************************/
// 1602液晶写命令函数，cmd就是要写入的命令
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
// 1602液晶写数据函数，dat就是要写入的数据
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
// 1602液晶初始化函数
/*********************************************************/
void LcdInit()
{
	LcdWriteCmd(0x38);        // 16*2显示，5*7点阵，8位数据口
	LcdWriteCmd(0x0C);        // 开显示，不显示光标
	LcdWriteCmd(0x06);        // 地址加1，当写入数据后光标右移
	LcdWriteCmd(0x01);        // 清屏
}


/*********************************************************/
// 液晶光标定位函数
/*********************************************************/
void LcdGotoXY(uchar line,uchar column)
{
	// 第一行
	if(line==0)        
		LcdWriteCmd(0x80+column); 
	// 第二行
	if(line==1)        
		LcdWriteCmd(0x80+0x40+column); 
}


/*********************************************************/
// 液晶输出字符串函数
/*********************************************************/
void LcdPrintStr(uchar *str)
{
	while(*str!='\0')
			LcdWriteData(*str++);
}


/*********************************************************/
// 液晶输出数字(0-99)
/*********************************************************/
void LcdPrintNum(uchar num)
{
	LcdWriteData(num/10+48);		// 十位
	LcdWriteData(num%10+48); 		// 个位
}


/*********************************************************/
// 显示模式
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
// 液晶显示内容的初始化
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
// 刷新时间显示
/*********************************************************/
void FlashTime()
{
	LcdGotoXY(0,2);										// 年份
	LcdPrintNum(TimeBuff[0]);
	LcdGotoXY(0,5);										// 月份
	LcdPrintNum(TimeBuff[1]);
	LcdGotoXY(0,8);										// 日期
	LcdPrintNum(TimeBuff[2]);
	LcdGotoXY(0,11);									// 小时
	LcdPrintNum(TimeBuff[4]);
	LcdGotoXY(0,14);									// 分钟
	LcdPrintNum(TimeBuff[5]);
	LcdGotoXY(0,13);									// 秒钟
	if(TimeBuff[6]%2==0)							// 秒钟是偶数显示冒号
		LcdWriteData(':');
	else															// 秒钟是奇数显示空格
		LcdWriteData(' ');
}


/*********************************************************/
// 初始化DS1302
/*********************************************************/
void DS1302_Init(void)
{
	RST_P=0;			// RST脚置低
	SCK_P=0;			// SCK脚置低
	SDA_P=0;			// SDA脚置低				
}


/*********************************************************/
// 从DS1302读出一字节数据
/*********************************************************/
uchar DS1302_Read_Byte(uchar addr) 
{
	uchar i;
	uchar temp;
	
	RST_P=1;								
	
	/* 写入目标地址：addr*/
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
	
	/* 读出该地址的数据 */
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
// 向DS1302写入一字节数据
/*********************************************************/
void DS1302_Write_Byte(uchar addr, uchar dat)
{
	uchar i;
	
	RST_P = 1;
	
	/* 写入目标地址：addr*/
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
	
	/* 写入数据：dat*/
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
// 向DS1302写入时间数据
/*********************************************************/
void DS1302_Write_Time() 
{
  uchar i;
	uchar temp1;
	uchar temp2;
	
	for(i=0;i<7;i++)			// 十进制转BCD码
	{
		temp1=(TimeBuff[i]/10)<<4;
		temp2=TimeBuff[i]%10;
		TimeBuff[i]=temp1+temp2;
	}
	
	DS1302_Write_Byte(0x8E,0x00);								// 关闭写保护 
	DS1302_Write_Byte(0x80,0x80);								// 暂停时钟 
	DS1302_Write_Byte(0x8C,TimeBuff[0]);				// 年 
	DS1302_Write_Byte(0x88,TimeBuff[1]);				// 月 
	DS1302_Write_Byte(0x86,TimeBuff[2]);				// 日 
	DS1302_Write_Byte(0x8A,TimeBuff[3]);				// 星期
	DS1302_Write_Byte(0x84,TimeBuff[4]);				// 时 
	DS1302_Write_Byte(0x82,TimeBuff[5]);				// 分
	DS1302_Write_Byte(0x80,TimeBuff[6]);				// 秒
	DS1302_Write_Byte(0x80,TimeBuff[6]&0x7F);		// 运行时钟
	DS1302_Write_Byte(0x8E,0x80);								// 打开写保护  
}



/*********************************************************/
// 从DS1302读出时间数据
/*********************************************************/
void DS1302_Read_Time()  
{ 
	uchar i;

	TimeBuff[0]=DS1302_Read_Byte(0x8D);						// 年 
	TimeBuff[1]=DS1302_Read_Byte(0x89);						// 月 
	TimeBuff[2]=DS1302_Read_Byte(0x87);						// 日 
	TimeBuff[3]=DS1302_Read_Byte(0x8B);						// 星期
	TimeBuff[4]=DS1302_Read_Byte(0x85);						// 时 
	TimeBuff[5]=DS1302_Read_Byte(0x83);						// 分 
	TimeBuff[6]=(DS1302_Read_Byte(0x81))&0x7F;		// 秒 

	for(i=0;i<7;i++)		// BCD转十进制
	{           
		TimeBuff[i]=(TimeBuff[i]/16)*10+TimeBuff[i]%16;
	}
}
unsigned char ad0832read(bit SGL,bit ODD)
{
	unsigned char i=0,value=0,value1=0;		
		SCL=0;
		DO=1;
		CS=0;		//开始
		SCL=1;		//第一个上升沿	
		SCL=0;
		DO=SGL;
		SCL=1;  	//第二个上升沿
		SCL=0;
		DO=ODD;
		SCL=1;	    //第三个上升沿
		SCL=0;	    //第三个下降沿
		DO=1;
		for(i=0;i<8;i++)
		{
			SCL=1;
			SCL=0; //开始从第四个下降沿接收数据
			value<<=1;
			if(DO)
				value++;						
		}
		for(i=0;i<8;i++)
		{			//接收校验数据
			value1>>=1;
			if(DO)
				value1+=0x80;
			SCL=1;
			SCL=0;
		}
		CS=1;
		SCL=1;	
		if(value==value1)				//与校验数据比较，正确就返回数据，否则返回0	
			return value;
	return 0;
}
/*********************************************************/
// 按键扫描(设置当前时间)
/*********************************************************/
void KeyScanf1()
{
	if(KeySet_P==0)
	{
		LcdGotoXY(0,13);				// 显示秒钟的冒号
		LcdWriteData(':');
		LcdWriteCmd(0x0f);			// 启动光标闪烁
		LcdGotoXY(0,3);					// 定位光标到年份闪烁
		DelayMs(10);						// 延时等待，消除按键按下的抖动
		while(!KeySet_P);				// 等待按键释放
		DelayMs(10);						// 延时等待，消除按键松开的抖动
		
		/* 调整年份 */
		while(1)
		{
			if(KeyDown_P==0)							// 如果减按键被下去
			{
				if(TimeBuff[0]>0)						// 判断年份是否大于0		
					TimeBuff[0]--;						// 是的话就减去1
				LcdGotoXY(0,2);							// 光标定位到年份的位置
				LcdPrintNum(TimeBuff[0]);		// 刷新显示改变后的年份
				LcdGotoXY(0,3);							// 定位光标到年份闪烁
				DelayMs(300);								// 延时0.3秒左右
			}
			
			if(KeyUp_P==0)								// 如果加按键被下去
			{
				if(TimeBuff[0]<99)					// 判断年份是否小于99
					TimeBuff[0]++;						// 是的话就加上1
				LcdGotoXY(0,2);							// 光标定位到年份的位置
				LcdPrintNum(TimeBuff[0]);		// 刷新显示改变后的年份
				LcdGotoXY(0,3);							// 定位光标到年份闪烁
				DelayMs(300);								// 延时0.3秒左右
			}
			
			if(KeySet_P==0)
			{
				break;
			}
		}
		
		LcdGotoXY(0,6);					// 定位光标到月份闪烁
		DelayMs(10);						// 延时等待，消除按键按下的抖动
		while(!KeySet_P);				// 等待按键释放
		DelayMs(10);						// 延时等待，消除按键松开的抖动
			
		/* 调整月份 */
		while(1)
		{
			if(KeyDown_P==0)							// 如果减按键被下去
			{
				if(TimeBuff[1]>1)						// 判断月份是否大于1		
					TimeBuff[1]--;						// 是的话就减去1
				LcdGotoXY(0,5);							// 光标定位到月份的位置
				LcdPrintNum(TimeBuff[1]);		// 刷新显示改变后的月份
				LcdGotoXY(0,6);							// 定位光标到月份闪烁
				DelayMs(300);								// 延时0.3秒左右
			}
			
			if(KeyUp_P==0)								// 如果加按键被下去
			{
				if(TimeBuff[1]<12)					// 判断月份是否小于12
					TimeBuff[1]++;						// 是的话就加上1
				LcdGotoXY(0,5);							// 光标定位到月份的位置
				LcdPrintNum(TimeBuff[1]);		// 刷新显示改变后的月份
				LcdGotoXY(0,6);							// 定位光标到月份闪烁
				DelayMs(300);								// 延时0.3秒左右
			}
			
			if(KeySet_P==0)
			{
				break;
			}
		}
		
		LcdGotoXY(0,9);					// 定位光标到日期闪烁
		DelayMs(10);						// 延时等待，消除按键按下的抖动
		while(!KeySet_P);				// 等待按键释放
		DelayMs(10);						// 延时等待，消除按键松开的抖动
		
		/* 调整日期 */
		while(1)
		{
			if(KeyDown_P==0)							// 如果减按键被下去
			{
				if(TimeBuff[2]>1)						// 判断日期是否大于1		
					TimeBuff[2]--;						// 是的话就减去1
				LcdGotoXY(0,8);							// 光标定位到日期的位置
				LcdPrintNum(TimeBuff[2]);		// 刷新显示改变后的日期
				LcdGotoXY(0,9);							// 定位光标到日期闪烁
				DelayMs(300);								// 延时0.3秒左右
			}
			
			if(KeyUp_P==0)								// 如果加按键被下去
			{
				if(TimeBuff[2]<31)					// 判断日期是否小于31
					TimeBuff[2]++;						// 是的话就加上1
				LcdGotoXY(0,8);							// 光标定位到日期的位置
				LcdPrintNum(TimeBuff[2]);		// 刷新显示改变后的日期
				LcdGotoXY(0,9);							// 定位光标到日期闪烁
				DelayMs(300);								// 延时0.3秒左右
			}
			
			if(KeySet_P==0)
			{
				break;
			}
		}
		
		LcdGotoXY(0,12);				// 定位光标到小时闪烁
		DelayMs(10);						// 延时等待，消除按键按下的抖动
		while(!KeySet_P);				// 等待按键释放
		DelayMs(10);						// 延时等待，消除按键松开的抖动
		
		
		/* 调整小时 */
		while(1)
		{
			if(KeyDown_P==0)							// 如果减按键被下去
			{
				if(TimeBuff[4]>0)						// 判断小时是否大于0
					TimeBuff[4]--;						// 是的话就减去1
				LcdGotoXY(0,11);						// 光标定位到小时的位置
				LcdPrintNum(TimeBuff[4]);		// 刷新显示改变后的小时
				LcdGotoXY(0,12);						// 定位光标到小时闪烁
				DelayMs(300);								// 延时0.3秒左右
			}
			
			if(KeyUp_P==0)								// 如果加按键被下去
			{
				if(TimeBuff[4]<23)					// 判断小时是否小于23
					TimeBuff[4]++;						// 是的话就加上1
				LcdGotoXY(0,11);						// 光标定位到小时的位置
				LcdPrintNum(TimeBuff[4]);		// 刷新显示改变后的小时
				LcdGotoXY(0,12);						// 定位光标到小时闪烁
				DelayMs(300);								// 延时0.3秒左右
			}
			
			if(KeySet_P==0)
			{
				break;
			}
		}
		
		LcdGotoXY(0,15);				// 定位光标到分钟闪烁
		DelayMs(10);						// 延时等待，消除按键按下的抖动
		while(!KeySet_P);				// 等待按键释放
		DelayMs(10);						// 延时等待，消除按键松开的抖动
		
		/* 调整分钟 */
		while(1)
		{
			if(KeyDown_P==0)							// 如果减按键被下去
			{
				if(TimeBuff[5]>0)						// 判断分钟是否大于0
					TimeBuff[5]--;						// 是的话就减去1
				LcdGotoXY(0,14);						// 光标定位到分钟的位置
				LcdPrintNum(TimeBuff[5]);		// 刷新显示改变后的分钟
				LcdGotoXY(0,15);						// 定位光标到分钟闪烁
				DelayMs(300);								// 延时0.3秒左右
			}
			
			if(KeyUp_P==0)								// 如果加按键被下去
			{
				if(TimeBuff[5]<59)					// 判断分钟是否小于59
					TimeBuff[5]++;						// 是的话就加上1
				LcdGotoXY(0,14);						// 光标定位到分钟的位置
				LcdPrintNum(TimeBuff[5]);		// 刷新显示改变后的分钟
				LcdGotoXY(0,15);						// 定位光标到分钟闪烁
				DelayMs(300);								// 延时0.3秒左右
			}
			
			if(KeySet_P==0)
			{
				break;
			}
		}
		
		/* 退出前的设置 */
		LcdWriteCmd(0x0C);			// 关闭光标闪烁
		DS1302_Write_Time();		// 把新设置的时间值存入DS1302芯片
		DelayMs(10);						// 延时等待，消除按键按下的抖动
		while(!KeySet_P);				// 等待按键释放
		DelayMs(10);						// 延时等待，消除按键松开的抖动
	}
}


/*********************************************************/
// 按键扫描(设置窗帘的动作)
/*********************************************************/
void KeyScanf2()
{
	if(KeySet2_P==0)
	{
		LcdGotoXY(0,0);										// 光标定位
		LcdPrintStr(" OpenTime   :   ");	// 显示第1行内容
		LcdGotoXY(1,0);										// 光标定位
		LcdPrintStr("CloseTime   :   ");	// 显示第2行内容
		LcdGotoXY(0,10);									// 光标定位
		LcdPrintNum(OpenHour);						// 显示开启窗帘的小时
		LcdGotoXY(0,13);									// 光标定位
		LcdPrintNum(OpenMinute);					// 显示开启窗帘的分钟
		LcdGotoXY(1,10);									// 光标定位
		LcdPrintNum(CloseHour);						// 显示关闭窗帘的小时
		LcdGotoXY(1,13);									// 光标定位
		LcdPrintNum(CloseMinute);					// 显示关闭窗帘的分钟		
		
		LcdWriteCmd(0x0f);							// 启动光标闪烁
		LcdGotoXY(0,11);								// 定位光标
		DelayMs(10);										// 延时等待，消除按键按下的抖动
		while(!KeySet2_P);							// 等待按键释放
		DelayMs(10);										// 延时等待，消除按键松开的抖动
		
		/* 调整开启的小时 */
		while(1)
		{
			if(KeyDown_P==0)							// 如果减按键被下去
			{
				if(OpenHour>0)							// 判断小时是否大于0		
					OpenHour--;								// 是的话就减去1
				LcdGotoXY(0,10);						// 光标定位
				LcdPrintNum(OpenHour);			// 刷新显示改变后的小时
				LcdGotoXY(0,11);						// 定位光标
				DelayMs(300);								// 延时0.3秒左右
			}
			
			if(KeyUp_P==0)								// 如果加按键被下去
			{
				if(OpenHour<23)							// 判断小时是否小于23
					OpenHour++;								// 是的话就加上1
				LcdGotoXY(0,10);						// 光标定位
				LcdPrintNum(OpenHour);			// 刷新显示改变后的小时
				LcdGotoXY(0,11);						// 定位光标
				DelayMs(300);								// 延时0.3秒左右
			}
			
			if(KeySet2_P==0)
			{
				break;
			}
		}
		
		LcdGotoXY(0,14);								// 定位光标
		DelayMs(10);										// 延时等待，消除按键按下的抖动
		while(!KeySet2_P);							// 等待按键释放
		DelayMs(10);										// 延时等待，消除按键松开的抖动
		
		/* 调整开启的分钟 */
		while(1)
		{
			if(KeyDown_P==0)							// 如果减按键被下去
			{
				if(OpenMinute>0)						// 判断分钟是否大于0
					OpenMinute--;							// 是的话就减去1
				LcdGotoXY(0,13);						// 光标定位
				LcdPrintNum(OpenMinute);		// 刷新显示改变后的分钟
				LcdGotoXY(0,14);						// 定位光标
				DelayMs(300);								// 延时0.3秒左右
			}
			
			if(KeyUp_P==0)								// 如果加按键被下去
			{
				if(OpenMinute<59)						// 判断分钟是否小于59
					OpenMinute++;							// 是的话就加上1
				LcdGotoXY(0,13);						// 光标定位
				LcdPrintNum(OpenMinute);		// 刷新显示改变后的分钟
				LcdGotoXY(0,14);						// 定位光标
				DelayMs(300);								// 延时0.3秒左右
			}
			
			if(KeySet2_P==0)
			{
				break;
			}
		}
		
		LcdGotoXY(1,11);								// 定位光标
		DelayMs(10);										// 延时等待，消除按键按下的抖动
		while(!KeySet2_P);							// 等待按键释放
		DelayMs(10);										// 延时等待，消除按键松开的抖动
		
		/* 调整关闭的小时 */
		while(1)
		{
			if(KeyDown_P==0)							// 如果减按键被下去
			{
				if(CloseHour>0)							// 判断小时是否大于0		
					CloseHour--;							// 是的话就减去1
				LcdGotoXY(1,10);						// 光标定位
				LcdPrintNum(CloseHour);			// 刷新显示改变后的小时
				LcdGotoXY(1,11);						// 定位光标
				DelayMs(300);								// 延时0.3秒左右
			}
			
			if(KeyUp_P==0)								// 如果加按键被下去
			{
				if(CloseHour<23)						// 判断小时是否小于23
					CloseHour++;							// 是的话就加上1
				LcdGotoXY(1,10);						// 光标定位
				LcdPrintNum(CloseHour);			// 刷新显示改变后的小时
				LcdGotoXY(1,11);						// 定位光标
				DelayMs(300);								// 延时0.3秒左右
			}
			
			if(KeySet2_P==0)
			{
				break;
			}
		}
		
		LcdGotoXY(1,14);								// 定位光标
		DelayMs(10);										// 延时等待，消除按键按下的抖动
		while(!KeySet2_P);							// 等待按键释放
		DelayMs(10);										// 延时等待，消除按键松开的抖动
		
		/* 调整关闭的分钟 */
		while(1)
		{
			if(KeyDown_P==0)							// 如果减按键被下去
			{
				if(CloseMinute>0)						// 判断分钟是否大于0
					CloseMinute--;						// 是的话就减去1
				LcdGotoXY(1,13);						// 光标定位
				LcdPrintNum(CloseMinute);		// 刷新显示改变后的分钟
				LcdGotoXY(1,14);						// 定位光标
				DelayMs(300);								// 延时0.3秒左右
			}
			
			if(KeyUp_P==0)								// 如果加按键被下去
			{
				if(CloseMinute<59)					// 判断分钟是否小于59
					CloseMinute++;						// 是的话就加上1
				LcdGotoXY(1,13);						// 光标定位
				LcdPrintNum(CloseMinute);		// 刷新显示改变后的分钟
				LcdGotoXY(1,14);						// 定位光标
				DelayMs(300);								// 延时0.3秒左右
			}
			
			if(KeySet2_P==0)
			{
				break;
			}
		}
		
		DelayMs(10);										// 延时等待，消除按键按下的抖动
		while(!KeySet2_P);							// 等待按键释放
		DelayMs(10);										// 延时等待，消除按键松开的抖动
		
		/* 光照强度的设置 */
		LcdWriteCmd(0x0C);								// 关闭光标闪烁
		LcdGotoXY(0,0);										// 光标定位
		LcdPrintStr("   Light Set    ");	// 显示第1行内容
		LcdGotoXY(1,0);										// 光标定位
		LcdPrintStr("                ");	// 显示第2行内容
		LcdGotoXY(1,7);										// 光标定位
		LcdPrintNum(gLight);							// 显示窗帘的光线控制强度阈值
		
		while(1)
		{
			if(KeyDown_P==0)							// 如果减按键被下去
			{
				if(gLight>0)								// 判断光线阈值是否大于0
					gLight--;									// 是的话就减去1
				LcdGotoXY(1,7);							// 光标定位
				LcdPrintNum(gLight);				// 刷新显示改变后的光线阈值
				DelayMs(300);								// 延时0.3秒左右
			}
			
			if(KeyUp_P==0)								// 如果加按键被下去
			{
				if(gLight<99)								// 判断光线阈值是否小于59
					gLight++;									// 是的话就加上1
				LcdGotoXY(1,7);							// 光标定位
				LcdPrintNum(gLight);				// 刷新显示改变后的光线阈值
				DelayMs(300);								// 延时0.3秒左右
			}
			
			if(KeySet2_P==0)
			{
				break;
			}
		}
		
		/* 退出前的设置 */
		LcdShowInit();					// 液晶显示内容初始化
		DelayMs(10);						// 延时等待，消除按键按下的抖动
		while(!KeySet2_P);			// 等待按键释放
		DelayMs(10);						// 延时等待，消除按键松开的抖动
	}
}


/*********************************************************/
// 按键扫描(模式切换)
/*********************************************************/
void KeyScanf3()
{
	if(KeyMode_P==0)
	{
		gMode++;							// 切换到下一模式
		if(gMode==4)					// 如果到尽头了
			gMode=1;						// 回到第一种模式
		LcdGotoXY(1,0);				// 光标定位
		LcdPrintMode(gMode);	// 显示模式
		DelayMs(10);					// 去除按键按下的抖动
		while(!KeyMode_P);		// 等待按键是否
		DelayMs(10);					// 去除按键松开的抖动
	}
}


/*********************************************************/
// 开窗0x10,0x20,0x40,0x80
/*********************************************************/
sbit dj1=P2^7;
sbit dj2=P2^6;
sbit dj3=P2^5;
sbit dj4=P2^4;
void Open()
{
	uint i,j;
	for(j=0;j<220;j++)		// 控制步进电机正转
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
// 关窗
/*********************************************************/
void Close()
{
	uint i,j;
	for(j=0;j<220;j++)		// 控制步进电机反转
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



void init()//初始化函数
{
    IE = 0x81;                 //允许总中断中断,使能 INT0 外部中断
    TCON = 0x01;               //触发方式为脉冲负边沿触发
    
    IRIN=1;                    //I/O口初始化
}
/*********************************************************/
// 主函数
/*********************************************************/
void main()
{
	uchar light;
	init();
	LcdInit();			// 执行液晶初始化	
	DS1302_Init();	// 时钟芯片的初始化
	LcdShowInit();	// 液晶显示内容的初始化
	
	if(DS1302_Read_Byte(0x81)>=128)			// 判断时钟芯片是否正在运行
	{
		DS1302_Write_Time();							// 如果没有，则初始化一个时间
	}
	
	while(1)
	{
		DS1302_Read_Time();				// 获取当前时钟芯片的时间，存在数组time_buf中
		FlashTime();							// 刷新时间显示

		light=ad0832read(1,0);//Get_ADC0832();			// 读取光照强度
		light=light/2.5;					// 缩小光照检测结果(在0-99)
		if(light>99)							// 如果大于99
			light=99;								// 则依然保持99
		LcdGotoXY(1,14);					// 光标定位
		LcdPrintNum(light);				// 显示光照强度
		
		KeyScanf1();							// 按键扫描(时间的设置)
		KeyScanf2();							// 按键扫描(阈值的设置)
		KeyScanf3();							// 按键扫描(模式切换)
	
		/*手动控制模式*/
		if(gMode==1)
		{
			if(KeyDown_P==0)		// 如果关窗帘键按下了	
			{
				if(Led_P==0)			// 如果窗帘当前是打开的
				{
					Close();				// 则关闭窗帘
				}
			}
			if(KeyUp_P==0)			// 如果开窗帘键按下了
			{
				if(Led_P==1)			// 如果窗帘当前是关闭的
				{
					Open();					// 则打开窗帘
				}
			}	
		}
		
		/*时间控制模式*/
		if(gMode==2)
		{
			if((TimeBuff[4]==CloseHour)&&(TimeBuff[5]==CloseMinute)&&(TimeBuff[6]==0))	// 如果到了关窗帘的时间	
			{
				if(Led_P==0)			// 如果窗帘当前是打开的
				{
					Close();				// 则关闭窗帘
				}
			}
			if((TimeBuff[4]==OpenHour)&&(TimeBuff[5]==OpenMinute)&&(TimeBuff[6]==0))		// 如果到了开窗帘的时间	
			{
				if(Led_P==1)			// 如果窗帘当前是关闭的
				{
					Open();					// 则打开窗帘
				}
			}	
		}
		
		/*光线控制模式*/
		if(gMode==3)
		{
			if(light<gLight)		// 当前光线小于设置的阈值
			{
				if(Led_P==0)			// 如果窗帘当前是打开的
				{
					Close();				// 则关闭窗帘
				}
			}
			else								// 当前光线大于或等于设置的阈值
			{
				if(Led_P==1)			// 如果窗帘当前是关闭的
				{
					Open();					// 则打开窗帘
				}
			}	
		}
		
		DelayMs(100);							// 延时0.1秒
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
                           //确认IR信号出现
  while (!IRIN)            //等IR变为高电平，跳过9ms的前导低电平信号。
    {delay(1);}

 for (j=0;j<4;j++)         //收集四组数据
 { 
  for (k=0;k<8;k++)        //每组数据有8位
  {
   while (IRIN)            //等 IR 变为低电平，跳过4.5ms的前导高电平信号。
     {delay(1);}
    while (!IRIN)          //等 IR 变为高电平
     {delay(1);}
     while (IRIN)           //计算IR高电平时长
      {
    delay(1);
    N++;           
    if (N>=30)
	 { EX0=1;
	 return;}                  //0.14ms计数过长自动离开。
      }                        //高电平计数完毕                
     IRCOM[j]=IRCOM[j] >> 1;                  //数据最高位补“0”
     if (N>=8) {IRCOM[j] = IRCOM[j] | 0x80;}  //数据最高位补“1”
     N=0;
  }//end for k
 }//end for j
	if(IRCOM[2]==0x45)//全灭
	{
		if(gMode==1)
		{
			if(Led_P==0)			// 如果窗帘当前是打开的
			{
				Close();				// 则关闭窗帘
			}	
		}
	}

	if(IRCOM[2]==0x47)//全亮
	{
		if(gMode==1)
		{
			if(Led_P==1)			// 如果窗帘当前是关闭的
			{
				Open();					// 则打开窗帘
			}	
		}
	}

     EX0 = 1; 
} 

