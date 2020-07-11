#include "delay.h"
#include "sys.h"
#include "lcd.h"
#include "touch.h"
#include "gui.h"
#include "test.h"
#include "key.h"
#include "usart.h"
#include "led.h"
#include "rda5820.h"
#include "myiic.h"
#include "string.h"

rda5820_func_t rda5820_t;

int main(void)
{	
	u8 key =0;
	
	uchar Bass=0;
	uchar Vol=8,TxPAG=63,Gain=7,Rssi=0;
	uint TXFreq=9200,RXFreq=10110;		
	
	memset(&rda5820_t,0,sizeof(rda5820_func_t));
	
	rda5820_t.Rxfreq_t = RXFreq;
	rda5820_t.volume = Vol;
	rda5820_t.Txfreq_t = TXFreq;
	rda5820_t.Bass_t = Bass;
	rda5820_t.Signal_str = Rssi;
	rda5820_t.Txgain_t = Gain;
	
	SystemInit();//��ʼ��RCC ����ϵͳ��ƵΪ72MHZ
	delay_init(72);	     //��ʱ��ʼ��
	LCD_Init();	   //Һ������ʼ��
	uart_init(9600);
	IIC_Init();
	
	KEY_Init();
	LED_Init();
	
	LCD_Clear(BLACK); //����

	POINT_COLOR=WHITE; 
	
	LCD_DrawRectangle(0,0,128-1,128-1);	//������ 

	Show_Str(16,5,RED,BLACK,"����ϵͳ����",16,1);//���� 16 �Ƿ���ʾ�߿� 0/1

	Show_Str(8,40,WHITE,BLACK,"��ǰƵ��",16,1);
	LCD_ShowChar(8+16*4,40,WHITE,BLACK,':',16,1);
	
	Show_Str(10,60,WHITE,BLACK,"���书��",16,1);
	LCD_ShowChar(10+16*4,60,WHITE,BLACK,':',16,1);	

	Show_Str(10,80,WHITE,BLACK,"��������",16,1);
	LCD_ShowChar(10+16*4,80,WHITE,BLACK,':',16,1);
	
	Show_Str(10,100,WHITE,BLACK,"�ź�ǿ��",16,1);
	LCD_ShowChar(10+16*4,100,WHITE,BLACK,':',16,1);	
	
	LCD_ShowNum(10+16*4+8,100+3,rda5820_t.Signal_str,2,12);//�ź�ǿ��
	
	LCD_ShowNum(8+16*4+8,43,rda5820_t.Txfreq_t/100,3,12);//Ƶ������
	LCD_ShowChar(8+16*4+8+18,43,WHITE,BLACK,'.',12,1);
	LCD_ShowNum(8+16*4+8+18+4,43,rda5820_t.Txfreq_t%100,2,12);//Ƶ������
	LCD_ShowNum(10+16*4+8,60+3,TxPAG,2,12);//���书��

	while(RDA5820_Init())
	{
		LCD_ShowNum(10+16*4+8,80+3,rda5820_t.Txgain_t,2,12);//��������
		delay_ms(200);
	}
//	rda5820_t.mode = 0;//����ģʽ
//	RX_Mode();

  rda5820_t.mode = 1;//����ģʽ
	TX_Fm(&rda5820_t);		

	printf("system is running!\r\n");
	while(1)
	{
		
		if(rda5820_t.mode == 1)
		{

			rda5820_t.Signal_str = Rssi_Get();		//�ź�ǿ��
			LCD_ShowNum(10+16*4+8,100+3,rda5820_t.Signal_str,2,12);//�ź�ǿ��
			
			LCD_ShowNum(8+16*4+8,43,rda5820_t.Txfreq_t/100,3,12);//Ƶ������
			LCD_ShowChar(8+16*4+8+18,43,WHITE,BLACK,'.',12,1);
			LCD_ShowNum(8+16*4+8+18+4,43,rda5820_t.Txfreq_t%100,2,12);//Ƶ������
			
			LCD_ShowNum(10+16*4+8,80+3,rda5820_t.Txgain_t,2,12);//��������
			//���书��
			LCD_ShowNum(10+16*4+8,60+3,TxPAG,2,12);//���书��
			while(rda5820_t.mode == 1)
			{
				  LED1=!LED1;
				  delay_ms(20);
					rda5820_t.Signal_str = Rssi_Get();		//�ź�ǿ��
					if(rda5820_t.last_Signal_str != rda5820_t.Signal_str)
					{
						LCD_ShowNum(10+16*4+8,100+3,rda5820_t.Signal_str,2,12);//�ź�ǿ��
					}
				  rda5820_t.last_Signal_str = rda5820_t.Signal_str;
				
					key = KEY_Scan(0);
					if(key)
					{
						switch(key)
						{
							case 1:
							{
								key = 0; 
								rda5820_t.function++;
								if(rda5820_t.function == 3)
								 rda5820_t.function = 0;
								break;//Ƶ��
							}
							case 2:
							{
								key = 0; 

								break;
							}
							case 3:
							{
								key = 0; 
								
								if(rda5820_t.Rxfreq_t < 10800 && rda5820_t.function == 0)
								{
									rda5820_t.Txfreq_t = rda5820_t.Txfreq_t + 10; 
								  Freq_Set(rda5820_t.Txfreq_t);
									
									LCD_ShowNum(8+16*4+8,43,rda5820_t.Txfreq_t/100,3,12);//Ƶ������
									LCD_ShowChar(8+16*4+8+18,43,WHITE,BLACK,'.',12,1);
									LCD_ShowNum(8+16*4+8+18+4,43,rda5820_t.Txfreq_t%100,2,12);//Ƶ������
								}
								
								if(rda5820_t.Txgain_t < 7 && rda5820_t.function == 2)
								{
									rda5820_t.Txgain_t = rda5820_t.Txgain_t + 1; 
									TxPGA_Set(rda5820_t.Txgain_t);  //��������
									LCD_ShowNum(10+16*4+8,80+3,rda5820_t.Txgain_t,2,12);
								}		

								if(TxPAG < 63 && rda5820_t.function == 1)
								{
									TxPAG+=1;
									TxPAG_Set(TxPAG);	   //���÷��书��
									LCD_ShowNum(10+16*4+8,60+3,TxPAG,2,12);
								}	
								
								break;//����
							}
							case 4:
							{
								key = 0;
								
								if(rda5820_t.Rxfreq_t > 8700 && rda5820_t.function == 0)
								{
									rda5820_t.Txfreq_t = rda5820_t.Txfreq_t - 10; 
									Freq_Set(rda5820_t.Txfreq_t);	   //����Ƶ��
									
									LCD_ShowNum(8+16*4+8,43,rda5820_t.Txfreq_t/100,3,12);//Ƶ������
									LCD_ShowChar(8+16*4+8+18,43,WHITE,BLACK,'.',12,1);
									LCD_ShowNum(8+16*4+8+18+4,43,rda5820_t.Txfreq_t%100,2,12);//Ƶ������
								}
								
								if(rda5820_t.Txgain_t > 0 && rda5820_t.function == 2)
								{
									rda5820_t.Txgain_t = rda5820_t.Txgain_t - 1; 
									TxPGA_Set(rda5820_t.Txgain_t);	//��������
									LCD_ShowNum(10+16*4+8,80+3,rda5820_t.Txgain_t,2,12); 
								}
								
								if(TxPAG > 0 && rda5820_t.function == 1)
								{
									TxPAG-=1;
									TxPAG_Set(TxPAG);	   //���÷��书��
									LCD_ShowNum(10+16*4+8,60+3,TxPAG,2,12);
								}	
								break;
							}
							default:break;
						}
					}
			}		
		}
		
		LED1=!LED1;
		delay_ms(30);
		
	}
			  	
}

