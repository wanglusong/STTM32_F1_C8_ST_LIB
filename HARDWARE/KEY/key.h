#ifndef __KEY_H
#define __KEY_H	 

#include "sys.h"
	 
 

#define KEY1  GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_4)//��ȡ����1
#define KEY2  GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_5)//��ȡ����2
#define KEY3  GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_6)//��ȡ����3
#define KEY4  GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_7)//��ȡ����4

void KEY_Init(void);//IO��ʼ��
u8 KEY_Scan(u8 mode);  	//����ɨ�躯��				

#endif
