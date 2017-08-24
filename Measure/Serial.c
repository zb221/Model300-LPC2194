/******************************************************************************/
/* SERIAL.C: Low Level Serial Routines                                        */
/******************************************************************************/
/* This file is part of the uVision/ARM development tools.                    */
/* Copyright (c) 2005-2006 Keil Software. All rights reserved.                */
/* This software may only be used under the terms of a valid, current,        */
/* end user licence from KEIL for a compatible version of KEIL software       */
/* development tools. Nothing else gives you the right to use this software.  */
/******************************************************************************/

#include <LPC21xx.H>                     /* LPC21xx definitions               */
#include "config.h"
#include "AD7738.h"
#include "Cubic.h"

#define CR     0x0D

#ifndef Channel1
//#define Channel1
#endif

#ifndef Channel2
//#define Channel2
#endif

#ifndef Channel3
#define Channel3
#endif

//	float Temp_resistance_slope = ((388.3-369.8)/(85-65)+(369.8-351.7)/(65-45)+(351.7-334.5)/(45-25))/3;
//	float Hydrogen_resistance_slope = ((float)(658-645)/(85-65)+(float)(645-631)/(65-45)+(float)(631-616)/(45-25))/3;
	
	
	float AD7738_resolution = 8388607/2500;
	float AD7738_resolution_NP_125 = 8388607/1250;
	float AD7738_resolution_NP_0625 = 8388607/625;

	float Current_of_Temperature_resistance = 5;
	float Current_of_Hydrogen_Resistance = 0.75;


/*********************************************************************************************************
** Function name:		DelayNS
** Descriptions:		ns delay
** input parameters:    uiDly
** output parameters:   void
** Returned value:      void
*********************************************************************************************************/
void DelayNS (unsigned int uiDly)
{
    unsigned int  i;
    
    for(; uiDly > 0; uiDly--){
        for(i = 0; i < 50000; i++);
    }
} 

void CLOCK_SET(void)
{
	/*--Phase Locked Loop(PLL)--*/
	PLLCON |= 1<<1|1<<0;		/*PLL enable and Connect*/
	PLLCFG |= 1<<5| 3<<0;			/*P|M: set cclk->48MHz*/
	PLLFEED |= 0xAA;
	PLLFEED |= 0x55;
	/*--APB divider--*/
	VPBDIV |= 0;			/*--00:VPB=1/4CCLK----01: APB =processor clock. Pclk->60MHz--10:VPB=1/2CCLK--------*/
}

void init_serial (void)  {               
                                                                                   /* Initialize Serial Interface(uart0/1) */
  PINSEL0 = 0x00000000 | 0x01<<18 | 0x01<<16 | 0x01<<2 | 0x01<<0;                  /* Enable RxD1 and TxD1              */
  U1LCR = 0x83;                          /* 8 bits, no Parity, 1 Stop bit   0x1<<7|0x11<<0  */
  U1DLL = 8;                            /* 9600 Baud Rate @ 15MHz VPB Clock  */
  U1LCR = 0x03;                          /* DLAB = 0                          */

/*---------------------------------SPI0-----------------------------------------------*/
	PINSEL0 = (PINSEL0 & 0xFFFF00FF) | 0x15<<8; /*SPI0 PIN SEL*/
	S0SPCR = 0x00|(0 << 3)|(1 << 4)|(1 << 5)|(0 << 6)|(0 << 7);  /*Master mode*/
	S0SPCCR = 0x00 | 0x1E; /*SPI Clock Counter: PCLK / SnSPCCR=15MHz/12*/

/*---------------------------------SPI1-----------------------------------------------*/	
	PINSEL1 = (PINSEL1 & 0xFFFFFF03)| 0x10<<2|0x10<<4|0x10<<6; /*SPI1 PIN SEL*/
	S1SPCR = 0x00|(0 << 3)|(1 << 4)|(1 << 5)|(0 << 6)|(0 << 7);  /*Master mode*/
	S1SPCCR = 0x00 | 0x1E; /*SPI Clock Counter: PCLK / SnSPCCR*/
}

void AD7738_CS_INIT(void)
{
	PINSEL1 = PINSEL1 & (~(0x03 << 8));			/*CS1/SSEL1->P0.20*/	
	IODIR0 = IODIR0 | 0x1<<20;								
	IOSET0 = IOSET0 | 0x1<<20;
	
	PINSEL0 = PINSEL0 & (~(0x03 << 30));    /*RDY1->P0.15*/
	IODIR0 = IODIR0 | 0x0<<15;								
 	IOCLR0 = IOCLR0 | 0x1<<15;
}

void M25P16_CS_INIT(void)
{
	PINSEL1 = PINSEL1 & (~(0x03 << 18));			/*RD1->P0.25*/	
	IODIR0 = IODIR0 | 0x1<<25;								
	IOSET0 = IOSET0 | 0x1<<25;
}

void DAC8568_CS_INIT(void)
{
	PINSEL0 = PINSEL0 & (~(0x03 << 20));			/*/SYNC->P0[10]*/
	IODIR0 = IODIR0 | 0x1<<10;								
	IOSET0 = IOSET0 | 0x1<<10;
	
	PINSEL0 = PINSEL0 & (~(0x03 << 24));			/*LDAC->P0[12]*/
	IODIR0 = IODIR0 | 0x1<<12;								
	IOSET0 = IOSET0 | 0x1<<12;
}

/* implementation of putchar (also used by printf function to output data)    */
int sendchar (int ch)  {                 /* Write character to Serial Port    */

  if (ch == '\n')  {
    while (!(U1LSR & 0x20));
    U1THR = CR;                          /* output CR */
  }
  while (!(U1LSR & 0x20));
  return (U1THR = ch);
}


int getkey (void)  {                    /* Read character from Serial Port   */

  while (!(U1LSR & 0x01));              /* wait until character ready        */
  return (U1RBR);
}

unsigned char SPI0_SendDate(unsigned char date)
{
	S0SPDR = date;			//Send date
	
	while ((S0SPSR & 0x80) == 0);

	return (S0SPDR);
}

unsigned char SPI1_SendDate(unsigned char date)
{
	S1SPDR = date;			//Send date

	while ((S1SPSR & 0x80) == 0);

	return (S1SPDR);
}

void DAC8568_SET(unsigned char PB,unsigned char CB,unsigned char AB,unsigned short DB,unsigned char FB)
{	
	IOCLR0 = IOCLR0 | 0x1<<10;	/*/SYNC  SET LOW */
	
	SPI0_SendDate(PB<<4|CB);
	SPI0_SendDate(AB<<4|DB>>12);
	SPI0_SendDate(0xFF & (DB>>4));
	SPI0_SendDate((0xF & DB)<<4 | FB);
	
	IOSET0 = IOSET0 | 0x1<<10;	/*/SYNC  SET HIGHT */
	DelayNS(100);
	IOCLR0 = IOCLR0 | 0x1<<12;  /*/LDAC set LOW*/
	DelayNS(100);
	IOSET0 = IOSET0 | 0x1<<12;  /*/LDAC set HIGHT*/
}

void AD7738_write(unsigned char Register,unsigned char data)
{
	IOCLR0 = IOCLR0 | 0x1<<20;		/*CS1/SSEL1 set LOW*/
	
	SPI0_SendDate(0<<6|(0x3F & Register));

 	SPI0_SendDate(data);
	
	IOSET0 = IOSET0 | 0x1<<20;    /*CS1/SSEL1 set HIGHT*/
}
void AD7738_read(unsigned char Register,unsigned char *data)
{
	IOCLR0 = IOCLR0 | 0x1<<20;		/*CS1/SSEL1 set LOW*/
	
	SPI0_SendDate(1<<6|(0x3F & Register));
 	*data = SPI0_SendDate(0x00);
	
	IOSET0 = IOSET0 | 0x1<<20;    /*CS1/SSEL1 set HIGHT*/
}

void AD7738_read_channel_data(unsigned char Register,unsigned char *buf0,unsigned char *buf1,unsigned char *buf2)
{
	*buf0 = 0;
	*buf1 = 0;
	*buf2 = 0;
	
	IOCLR0 = IOCLR0 | 0x1<<20;		/*CS1/SSEL1 set LOW*/
	
	SPI0_SendDate(1<<6|(0x3F & Register));
	
//	IOSET0 = IOSET0 | 0x1<<20;    /*CS1/SSEL1 set HIGHT*/
//	DelayNS(100);
//	IOCLR0 = IOCLR0 | 0x1<<20;		/*CS1/SSEL1 set LOW*/
	
 	*buf0 = SPI0_SendDate(0x00);
	
//	IOSET0 = IOSET0 | 0x1<<20;    /*CS1/SSEL1 set HIGHT*/
//	DelayNS(100);
//	IOCLR0 = IOCLR0 | 0x1<<20;		/*CS1/SSEL1 set LOW*/
	
	*buf1 = SPI0_SendDate(0x00);
	
//	IOSET0 = IOSET0 | 0x1<<20;    /*CS1/SSEL1 set HIGHT*/
//	DelayNS(100);
//	IOCLR0 = IOCLR0 | 0x1<<20;		/*CS1/SSEL1 set LOW*/
	
	*buf2 = SPI0_SendDate(0x00);
	
	IOSET0 = IOSET0 | 0x1<<20;    /*CS1/SSEL1 set HIGHT*/
}

void AD7738_SET(void)
{
	unsigned char IO_Port_Reg = 0;

	
	DelayNS(100);
	
/*-----------------------------------------------set common register of AD7738-----------------------------------------------------*/
	AD7738_read(IO_PORT_REG,&IO_Port_Reg);
	AD7738_write(IO_PORT_REG,IO_Port_Reg & (~(1<<3)));		/*0: the RDY pin will only go low if any, 1: the RDY pin will only go low if all enabled channels have unread data*/

#ifdef Channel1
/*-----------------------------------------------set channel 1 register of AD7738-----------------------------------------------------*/
	AD7738_write(channel_setup_1,0<<7|AINx_AINx|0<<4|Channel_Continuous_conversion_enable|NP_125);	/*Channel_1 Setup Registers:BUF_OFF<<7|COM1|COM0|Stat|Channel_CCM|RNG2_0*/
	AD7738_write(channel_conv_time_1,Chop_Enable|FW);	/*channel coversion time*/
#endif

#ifdef Channel2
/*-----------------------------------------------set channel 2 register of AD7738-----------------------------------------------------*/
	AD7738_write(channel_setup_2,0<<7|AINx_AINx|0<<4|Channel_Continuous_conversion_enable|NP_125);	/*Channel_2 Setup Registers:BUF_OFF<<7|COM1|COM0|Stat|Channel_CCM|RNG2_0*/
	AD7738_write(channel_conv_time_2,Chop_Enable|FW);	/*channel coversion time*/
#endif

#ifdef Channel3
/*-----------------------------------------------set channel 3 register of AD7738-----------------------------------------------------*/
	AD7738_write(channel_setup_3,0<<7|AINx_AINx|0<<4|Channel_Continuous_conversion_enable|NP_125);	/*Channel_3 Setup Registers:BUF_OFF<<7|COM1|COM0|Stat|Channel_CCM|RNG2_0*/
	AD7738_write(channel_conv_time_3,Chop_Enable|FW);	/*channel coversion time*/
#endif
/*-----------------------------------------------set Mode register of AD7738-----------------------------------------------------*/
#ifdef Channel1
	AD7738_write(channel_mode_1,Continues_Conversion_Mode|1<<4|0<<3|0<<2|BIT24|1);		/*Mode Register: Mod2_0|CLKDIS|DUMP|CONT_RD|24_16|CLAMP*/
#endif
#ifdef Channel2
	AD7738_write(channel_mode_2,Continues_Conversion_Mode|1<<4|0<<3|0<<2|BIT24|1);		/*Mode Register: Mod2_0|CLKDIS|DUMP|CONT_RD|24_16|CLAMP*/
#endif
#ifdef Channel3
	AD7738_write(channel_mode_3,Continues_Conversion_Mode|1<<4|0<<3|0<<2|BIT24|1);		/*Mode Register: Mod2_0|CLKDIS|DUMP|CONT_RD|24_16|CLAMP*/
#endif
}

void PCB_TEMP (void) 
{

}

void init_PWM (void) 
{
  PINSEL1 |= 0x01<<10;											//Enable pin 0.21   as PWM5
  PWMPR    = 0x00000000;                    /* Load prescaler  */
  
  PWMPCR = 0x1<<13;                      /* PWM channel 5 single edge control, output enabled */
  PWMMCR = 1<<1;                      /* PWMMR0/PWMMR5 On match with timer reset the counter */
  PWMMR0 = 80;                           /* PWMMR0       */
  PWMMR5 = 40;                               /* PWMMR5    */
  PWMLER = 0xF;                             /* enable shadow latch for match 1 - 3   */ 
  PWMTCR = 0x00000002;                      /* Reset counter and prescaler           */ 
  PWMTCR = 0x00000009;                      /* enable counter and PWM, release counter from reset */
}

void DAC_SET_Chanel_Din(float temperature,int *DAC_DIN)
{
//	*DAC_DIN = 13107*Current_of_Temperature_resistance*(temperature*Temp_resistance_slope + (369.8-Temp_resistance_slope*65));

	float DAC_Din_Temp_slope = 0, x = 0, y = 0;

	Linear_slope(&DAC_Din_Temp_slope, &x, &y, DAC_temp);
//	printf("*DAC_Din_Temp_slope=%.3f,x=%.3f,y=%.3f\n",DAC_Din_Temp_slope,x,y);
	
	*DAC_DIN = (temperature - (y-(DAC_Din_Temp_slope*x)))/DAC_Din_Temp_slope;
	printf("*DAC_DIN=%d\n",*DAC_DIN);

}

void DAC8568_INIT_SET(float temperature)
{
	int DAC_Din = 0;
	DAC_SET_Chanel_Din(temperature,&DAC_Din); /*set want temp value*/
	
	DAC8568_SET(0x0,0x9,0x0,0xA000,0);		/*Power up internal reference all the time regardless DAC states*/
	
	DAC8568_SET(0x0,0x3,0x2,0xA000,0);		/*DAC-C*/
	DAC8568_SET(0x0,0x3,0x6,DAC_Din,0);		/*DAC-G*/

//	DAC8568_SET(0x0,0x3,0x5,0x1000,0);		/*DAC-F*/
//	DAC8568_SET(0x0,0x3,0x7,11336,0);		/*DAC-H*/
}


void Temperature_of_resistance_Parameter(unsigned char A,unsigned char B,unsigned char C)
{
	float resistance = 0;
	float Temperature = 0;
	float Temp_resistance_slope = 0, x = 0, y = 0;

	Linear_slope(&Temp_resistance_slope, &x, &y, Temp_Res);
	
	resistance = (A<<16|B<<8|C)/AD7738_resolution_NP_125/Current_of_Temperature_resistance;
//	printf("%.3fohm ",resistance);

	Temperature = (resistance-(y-Temp_resistance_slope*x))/Temp_resistance_slope;
#ifdef DEGUG	
	printf("%.3f : ",Temperature);
#endif
	
	Temperature = Cubic_main(resistance,Temp_Res);
#ifdef DEGUG	
	printf("%.3fC\n",Temperature);
#endif

//	printf("Temperature_of_resistance:\n %.2f\n\n",resistance);
	
}

void Hydrogen_Resistance_Parameter(unsigned char A,unsigned char B,unsigned char C,unsigned char temperature)
{
	float resistance = 0;
	float Temp = 0;

	resistance = (A<<16|B<<8|C)/AD7738_resolution_NP_125/Current_of_Hydrogen_Resistance;
	
	Temp = Cubic_main(resistance,Hydrogen_Res);
//	R = Hydrogen_resistance_slope*temperature + (645-65*Hydrogen_resistance_slope);
	
#ifdef DEGUG	
	printf(" %.3fC\n",Temp);
#endif
//	printf("Hydrogen_Resistance:\n %.2f\n\n",resistance);
}

void TEST_SENSE(unsigned char temperature)
{
	unsigned int count = 0, time = 10;
	unsigned char data0 = 0;
	unsigned char data1 = 0;
	unsigned char data2 = 0;
	unsigned int temp1 = 0, temp2 = 0, temp3 = 0;
	unsigned char flag1 = 0, flag2 = 0, flag3 = 0;
/*-------------------------get the data of sense----------------------------------*/	
	DelayNS(300);
	
	for (count=0;count<time;count++){

//		while(IO0PIN & 0x1<<15);		/*wait RDY goes LOW*/
#ifdef Channel1
		data0 = 0;
		data1 = 0;
		data2 = 0;
		flag1 = 0;
		while(!(flag1&(1<<1))){
			AD7738_read(ADC_STATUS_REG,&flag1);
		}
		AD7738_read_channel_data(channel_data_1,&data0,&data1,&data2);
		temp1 += (data0<<16|data1<<8|data2);
//		printf("1:%d\n",(data0<<16|data1<<8|data2));
#endif
#ifdef Channel2	
		data0 = 0;
		data1 = 0;
		data2 = 0;
		flag2 = 0;
		while(!(flag2&(1<<2))){
			AD7738_read(ADC_STATUS_REG,&flag2);
		}
		AD7738_read_channel_data(channel_data_2,&data0,&data1,&data2);
		temp2 += (data0<<16|data1<<8|data2);
//		printf("2:%d\n",(data0<<16|data1<<8|data2));
#endif		
#ifdef Channel3
		data0 = 0;
		data1 = 0;
		data2 = 0;
		flag3 = 0;
		while(!(flag3&(1<<3))){
			AD7738_read(ADC_STATUS_REG,&flag3);
		}
		AD7738_read_channel_data(channel_data_3,&data0,&data1,&data2);
		temp3 += (data0<<16|data1<<8|data2);
//		printf("3:%d\n",(data0<<16|data1<<8|data2));
#endif
	}

#ifdef Channel1
/*-------------------------control the temp of sense----------------------------------*/
	data0 = (temp1/time)>>16;
	data1 = (temp1/time)>>8;
	data2 = (temp1/time)>>0;

#ifdef DEGUG
	printf("%d ->%.3fmv ",(data0<<16|data1<<8|data2),(data0<<16|data1<<8|data2)/AD7738_resolution_NP_125);
#endif
	Temperature_of_resistance_Parameter(data0,data1,data2);
#endif

#ifdef Channel2
/*-------------------------Hydrogen Resistance of sense----------------------------------*/
	data0 = (temp2/time)>>16;
	data1 = (temp2/time)>>8;
	data2 = (temp2/time)>>0;
	
#ifdef DEGUG
	printf("%d ->%.3fmv ",(data0<<16|data1<<8|data2),(data0<<16|data1<<8|data2)/AD7738_resolution_NP_125);
#endif
	Hydrogen_Resistance_Parameter(data0,data1,data2,temperature);
#endif

#ifdef Channel3
/*-------------------------Control the temp of board----------------------------------*/
	data0 = (temp3/time)>>16;
	data1 = (temp3/time)>>8;
	data2 = (temp3/time)>>0;
#ifdef DEGUG
	printf("%d ->%.3fmv\n\n",(data0<<16|data1<<8|data2),(data0<<16|data1<<8|data2)/AD7738_resolution_NP_125);
#endif

#endif

}
