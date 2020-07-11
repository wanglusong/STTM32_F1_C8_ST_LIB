#include "rda5820.h"
#include "myiic.h"
#include "delay.h"

/*---------------------------------RDA5820程序--------------------------------------------------------------*/
uint ReadReg(uchar regAddr)			//读RDA5820指令
{
	u8 ack;
	
	uint buf;
	IIC_Start();
	IIC_Send_Byte(RDA5820_WRITE);	  //发送写命令
	IIC_Wait_Ack();	
	IIC_Send_Byte(regAddr);			  //发送地址
	IIC_Wait_Ack();		
	IIC_Start();
	IIC_Send_Byte(RDA5820_READ); 	  //发送读命令
	ack = IIC_Wait_Ack();	
	if(ack)return 1;
	buf = IIC_Read_Byte(1); 
	buf = buf<<8; 
	buf =buf|IIC_Read_Byte(0);
	IIC_Stop();
	return buf;
}
void WriteReg(uchar regAddr,uint val) //写RDA5820指令
{
	IIC_Start();
	IIC_Send_Byte(RDA5820_WRITE);		//发送写命令
	IIC_Wait_Ack();	 
	IIC_Send_Byte(regAddr);				 //发送地址
	IIC_Wait_Ack();	 
	IIC_Send_Byte(val>>8);
	IIC_Wait_Ack();	
	IIC_Send_Byte(val&0XFF);
	IIC_Wait_Ack();	
	IIC_Stop();
}

void TX_Fm(rda5820_func_t *rda5820_t)//FM发送模式
{
   TX_Mode();
   TxPAG_Set(63);
   TxAdc_Set(1);//
   TxPGA_Set(7);//
   Freq_Set(9200);//
	
	 rda5820_t->rda_TxPAG = 63;
	 rda5820_t->Txgain_t = 7;
	 rda5820_t->rda_ADCPAG = 1;
	 rda5820_t->Txfreq_t = 9200;	
}

void RX_Mode(void)		 //接收模式
{
	uint temp;
	temp=ReadReg(0X40);	
	temp&=0xfff0;				 
	WriteReg(0X40,temp) ;	
}	
void TX_Mode(void)			 //发射模式
{
	uint temp;
	temp=ReadReg(0X40);		
	temp&=0xfff0;
	temp|=0x0001;				   
	WriteReg(0X40,temp) ;		 
}
void Vol_Set(uchar vol)	//设置音量	 0~15
{
	uint temp;
	temp = ReadReg(0X05);	
	temp&=0xfff0;			
  temp|=vol&0x0f;	
	WriteReg(0x05,temp) ;			
}
void Mute_Set(uchar mute)	//静音设置	1：静音  0：不静音
{
	uint temp=0xd281;
	if(!mute)temp|=1<<14;
	else temp&=~(1<<14);	    		   		 
	WriteReg(0X02,temp) ;	
}
void Bass_Set(uchar bass)	  //BASS设置	1：打开BASS  0：不打开BASS
{
	uint temp=0xd281;	
	if(bass)temp|=1<<12;
	else temp&=~(1<<12);	    		   		 
	WriteReg(0X02,temp) ;		
}

uchar Rssi_Get(void)	//接收信号强度		0~63
{
	uint temp;
	temp=ReadReg(0X0B);		
	 temp=(temp>>9);     
	 return     temp;     
}

//设置灵敏度
//rssi:0~127;
void Rda5820RssiSet(u8 rssi)
{
    int temp;
    temp=ReadReg(0X05); //??0X05???
    temp&=0X80FF;
    temp|=(u16)rssi<<8;    
    WriteReg(0X05,temp) ; //设置RSSI
}

void TxPAG_Set(uchar gain)	 //发射功率设置	 0~63
{
	uint temp;
	temp=ReadReg(0X41);		
	temp&=0X01C0;
	temp|=gain;				   	
	WriteReg(0X41,temp) ;	
}

void TxAdc_Set(uchar gain)	 //ADC 0-3
{
	uint temp;
	//temp=ReadReg(0X41);		
	temp=0X00F0;
	temp|=gain<<8;				   	
	WriteReg(0X68,temp) ;	
}

void TxPGA_Set(uchar gain)	  //输入信号PGA增益
{
	uint temp;
	
	temp=ReadReg(0X68);
	temp|=gain<<10;			    
	WriteReg(0X68,temp) ;		
}
void Band_Set(uchar band)	//频率带宽设置	  0:87-108 1:76-91  2:76-108
{
	uint temp;
	temp=ReadReg(0X03);	
	temp&=0XFFF3;
	temp|=band<<2;			     
	WriteReg(0X03,temp) ;	
}
void Space_Set(uchar spc)		//设置步进频率 0:100KHz  1:200KHz  3:50KHZ
{
	uint temp;
	temp=ReadReg(0X03);	
	temp&=0XFFFC;
	temp|=spc;			     
	WriteReg(0X03,temp) ;		
}
void Freq_Set(uint freq)	//频率设置  单位10KHz 6500~10800
{
	uint temp;
	uchar spc=0,band=0;
	uint fbtm,chan;
	temp=ReadReg(0X03);	
	temp&=0X001F;
	band=(temp>>2)&0x03;		
	spc=temp&0x03; 				
	if(spc==0)spc=10;
	else if(spc==1)spc=20;
	else spc=5;
	if(band==0)fbtm=8700;
	else if(band==1||band==2)fbtm=7600;
	else 
	{
		fbtm=ReadReg(0X53);
		fbtm*=10;
	}
	if(freq<fbtm)return;
	chan=(freq-fbtm)/spc;		
	chan&=0X3FF;				 
	temp|=chan<<6;
	temp|=1<<4;						     
	WriteReg(0X03,temp) ;	
	delay_ms(20);
	while((ReadReg(0X0B)&(1<<7))==0);//等待FM_READY	
}

//得到当前频率
//返回值:频率值(单位10Khz)
u16 RDA5820_Freq_Get(void)
{
	u16 temp;
	u8 spc=0,band=0;
	u16 fbtm,chan;
	temp=ReadReg(0X03);		//读取0X03的内容
	chan=temp>>6;   
	band=(temp>>2)&0x03;		//得到频带
	spc=temp&0x03; 				//得到分辨率
	if(spc==0)spc=10;
	else if(spc==1)spc=20;
	else spc=5;
	if(band==0)fbtm=8700;
	else if(band==1||band==2)fbtm=7600;
	else 
	{
		fbtm=ReadReg(0X53);//得到bottom频率
		fbtm*=10;
	}
 	temp=fbtm+chan*spc;				 
	return temp;//返回频率值
}

char RDA5820_Init(void)		  //RDA5820初始化
{	
       uint id;
       IIC_SCL=1;
		   IIC_SDA=1;                                                //初始化IIC口     
//       id=ReadReg(RDA5820_R00);     //读取ID =0X5820
//       if(id==0X5820)                                        //读取ID正确
//       {
				 WriteReg(RDA5820_R02,0x0002);     //软复位
				 delay_ms(400);
				 WriteReg(RDA5820_R02,0x0001);    //立体声,上电
				 delay_ms(800);                                                //等待时钟稳定
				 WriteReg(RDA5820_R02,0Xd201);   
					//搜索强度8,LNAN,1.8mA,VOL最大
//				 WriteReg(0X07,0X7800);          
//				 WriteReg(0X13,0X0008);          
//				 WriteReg(0X15,0x1420);            //VCO设置  0x17A0/0x1420
//				 WriteReg(0X16,0XC000);         
//				 WriteReg(0X1C,0X3126);         
//				 WriteReg(0X22,0X9C24);          //fm_true
//				 WriteReg(0X47,0XF660) ;          //偷笑 rds
//      }else return 1;//初始化失败
       return 0;
}


