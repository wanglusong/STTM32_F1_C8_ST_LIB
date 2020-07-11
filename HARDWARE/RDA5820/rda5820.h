#ifndef __RDA5820_H
#define __RDA5820_H	 

#include "stm32f10x.h"

typedef unsigned int uint;
typedef unsigned char uchar;

//RDA5820 µÄ¼Ä´æÆ÷µØÖ·
#define RDA5820_READ    0X23  //¶ÁRDA5820
#define RDA5820_WRITE	0X22  //Ğ´RDA5820
#define RDA5820_WRITE	0X22  //Ğ´RDA5820
#define RDA5820_R00         0X00  //¶Á³ö16¸öÎ»µÄID =0X5820
#define RDA5820_R02					0X02
#define RDA5820_R05					0X05											//¡­¡­´Ë´¦Ê¡ÂÔ²¿·Ö¼Ä´æÆ÷¶¨Òå
#define RDA5820_R64     0X64  //RDS_RXMOD[4:3](FIFOÄ£Ê½[10]»òÕß¼Ä´æÆ÷Ä£Ê½[11])    #define RDA5820_READ  0X23            //¶ÁRDA5820
#define RDA5820_WRITE  0X22   

typedef struct RDA5820{
	u8 mode;//Ä£Ê½
	u8 Signal_str;//ĞÅºÅÇ¿¶È
	u8 last_Signal_str;//ÉÏ´ÎĞÅºÅÇ¿¶È
	u16 Rxfreq_t;//½ÓÊÕÆµÂÊ
	u16 Txfreq_t;//·¢ÉäÆµÂÊ
	u8 Txgain_t;//·¢ÉäÔöÒæ
	u8 volume;
	u8 Bass_t;//ÖØµÍÒô
	u8 function;//¹¦ÄÜ
	u8 rda_TxPAG;//·¢ËÍ¹¦ÂÊ
	u8 rda_ADCPAG;//adcÔöÒæ
}rda5820_func_t;

uint ReadReg(uchar regAddr);
void WriteReg(uchar regAddr,uint val);
void RX_Mode(void);
void TX_Mode(void);
void Vol_Set(uchar vol);
void Mute_Set(uchar mute);
void Bass_Set(uchar bass);
uchar Rssi_Get(void);
void TxPAG_Set(uchar gain);
void TxPGA_Set(uchar gain);
void Band_Set(uchar band);
void Space_Set(uchar spc);
void Freq_Set(uint freq);
u16 RDA5820_Freq_Get(void);
char RDA5820_Init(void);
void TxAdc_Set(uchar gain);	 //ADC 0-3£
void TX_Fm(rda5820_func_t *rda5820_t);//FM·¢ËÍÄ£Ê½
		 				    
#endif
