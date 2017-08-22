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

#define CR     0x0D

	float Temp_resistance_slope = ((388.3-369.8)/(85-65)+(369.8-351.7)/(65-45)+(351.7-334.5)/(45-25))/3;
	float Hydrogen_resistance_slope = ((float)(658-645)/(85-65)+(float)(645-631)/(65-45)+(float)(631-616)/(45-25))/3;
	float DAC_Din_Temp_slope =  ((57643-57054)/(74.85-64.85)+(57054-56465)/(64.85-54.80))/2;
	float AD7738_resolution = 8388607/2500;
	float AD7738_resolution_NP_125 = 8388607/1250;
	float Current_of_Temperature_resistance = 5.01;
	float Current_of_Hydrogen_Resistance = 0.755;


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

void init_serial (void)  {               
                                                                                   /* Initialize Serial Interface(uart0/1) */
  PINSEL0 = 0x00000000 | 0x01<<18 | 0x01<<16 | 0x01<<2 | 0x01<<0;                  /* Enable RxD1 and TxD1              */
  U1LCR = 0x83;                          /* 8 bits, no Parity, 1 Stop bit   0x1<<7|0x11<<0  */
  U1DLL = 97;                            /* 9600 Baud Rate @ 12MHz VPB Clock  */
  U1LCR = 0x03;                          /* DLAB = 0                          */

/*---------------------------------SPI0-----------------------------------------------*/
	PINSEL0 = (PINSEL0 & 0xFFFF00FF) | 0x15<<8; /*SPI0 PIN SEL*/
	S0SPCR = 0x00|(0 << 3)|(1 << 4)|(1 << 5)|(0 << 6)|(0 << 7);  /*Master mode*/
	S0SPCCR = 0x00 | 0x1<<5; /*SPI Clock Counter: PCLK / SnSPCCR*/

/*---------------------------------SPI1-----------------------------------------------*/	
	PINSEL1 = (PINSEL1 & 0xFFFFFF03)| 0x10<<2|0x10<<4|0x10<<6; /*SPI1 PIN SEL*/
	S1SPCR = 0x00|(0 << 3)|(1 << 4)|(1 << 5)|(0 << 6)|(0 << 7);  /*Master mode*/
	S1SPCCR = 0x00 | 0x1<<5; /*SPI Clock Counter: PCLK / SnSPCCR*/
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
	IOCLR0 = IOCLR0 | 0x1<<20;		/*CS1/SSEL1 set LOW*/
	SPI0_SendDate(0x00);		/*RESET AD7738*/
	SPI0_SendDate(0xFF);
	SPI0_SendDate(0xFF);
	SPI0_SendDate(0xFF);
	SPI0_SendDate(0xFF);
	IOSET0 = IOSET0 | 0x1<<20;		/*CS1/SSEL1 set HIGHT*/
	
	DelayNS(100);
	
/*-----------------------------------------------set common register of AD7738-----------------------------------------------------*/
	AD7738_read(IO_PORT_REG,&IO_Port_Reg);
	AD7738_write(IO_PORT_REG,IO_Port_Reg|1<<3);		/*0: the RDY pin will only go low if any, 1: the RDY pin will only go low if all enabled channels have unread data*/

/*----------------------------------------------set channel 1 register of AD7738-----------------------------------------------------*/
	AD7738_write(channel_setup_1,0<<7|AINx_AINx|0<<4|Channel_Continuous_conversion_enable|NP_25);	/*Channel_0 Setup Registers:BUF_OFF<<7|COM1|COM0|Stat|Channel_CCM|RNG2_0*/
	AD7738_write(channel_conv_time_1,Chop_Enable|FW);	/*channel coversion time*/

/*-----------------------------------------------set channel 2 register of AD7738-----------------------------------------------------*/
	AD7738_write(channel_setup_2,0<<7|AINx_AINx|0<<4|Channel_Continuous_conversion_enable|NP_125);	/*Channel_0 Setup Registers:BUF_OFF<<7|COM1|COM0|Stat|Channel_CCM|RNG2_0*/
	AD7738_write(channel_conv_time_2,Chop_Enable|FW);	/*channel coversion time*/

/*-----------------------------------------------set Mode register of AD7738-----------------------------------------------------*/
	AD7738_write(channel_mode_2,Continues_Conversion_Mode|1<<4|0<<3|0<<2|BIT24|1);		/*Mode Register: Mod2_0|CLKDIS|DUMP|CONT_RD|24_16|CLAMP*/

}

void AD7738_READ_isr (void) 
{

}

void init_PWM (void) 
{
  PINSEL1 |= 0x01<<10;											//Enable pin 0.21   as PWM5
  PWMPR    = 0x00000000;                    /* Load prescaler  */
  
  PWMPCR = 0x1<<13;                      /* PWM channel 5 single edge control, output enabled */
  PWMMCR = 1<<1;                      /* PWMMR0/PWMMR5 On match with timer reset the counter */
  PWMMR0 = 100;                           /* PWMMR0       */
  PWMMR5 = 50;                               /* PWMMR5    */
  PWMLER = 0xF;                             /* enable shadow latch for match 1 - 3   */ 
  PWMTCR = 0x00000002;                      /* Reset counter and prescaler           */ 
  PWMTCR = 0x00000009;                      /* enable counter and PWM, release counter from reset */
}

void DAC_SET_Chanel_Din(float temperature,int *DAC_DIN)
{
//	*DAC_DIN = 13107*Current_of_Temperature_resistance*(temperature*Temp_resistance_slope + (369.8-Temp_resistance_slope*65));
	*DAC_DIN = DAC_Din_Temp_slope*temperature + (57054-(DAC_Din_Temp_slope*64.85));

}

void DAC8568_INIT_SET(float temperature)
{
	int DAC_Din = 0;
	DAC_SET_Chanel_Din(temperature,&DAC_Din); /*set want temp value*/
	
	DAC8568_SET(0x0,0x9,0x0,0xA000,0);		/*Power up internal reference all the time regardless DAC states*/
	DAC8568_SET(0x0,0x3,0x2,0x8000,0);		/*DAC-C*/
	DAC8568_SET(0x0,0x3,0x6,DAC_Din,0);		/*DAC-G*/
}


void Temperature_of_resistance_Parameter(unsigned char A,unsigned char B,unsigned char C)
{
	float resistance = 0;
	float Temp = 0;

	resistance = (A<<16|B<<8|C)/AD7738_resolution/Current_of_Temperature_resistance;
	Temp = (resistance-(369.8-Temp_resistance_slope*65))/Temp_resistance_slope;
#ifdef DEGUG	
	printf("Temperature_of_resistance:\n %.2f %.2f\n\n",Temp,resistance);
#endif
	printf("Temperature_of_resistance:\n %.2f\n\n",resistance);
	
}

void Hydrogen_Resistance_Parameter(unsigned char A,unsigned char B,unsigned char C,unsigned char temperature)
{
	float resistance = 0;
	float R = 0;

	resistance = (A<<16|B<<8|C)/AD7738_resolution_NP_125/Current_of_Hydrogen_Resistance;
	R = Hydrogen_resistance_slope*temperature + (645-65*Hydrogen_resistance_slope);
#ifdef DEGUG	
	printf("Hydrogen_Resistance:\n %.2f : %.2f\n\n",resistance,R);
#endif
	printf("Hydrogen_Resistance:\n %.2f\n\n",resistance);
}

void TEST_SENSE(unsigned char temperature)
{
	unsigned char data0 = 0;
	unsigned char data1 = 0;
	unsigned char data2 = 0;
	
	DelayNS(300);
	
	while(IO0PIN & 0x1<<15);		/*wait RDY goes LOW*/
	
	AD7738_read_channel_data(0x09,&data0,&data1,&data2);	/*09h: Data Register*/
#ifdef DEGUG
	printf("channel_1=%d ->%.2f\n",(data0<<16|data1<<8|data2),(data0<<16|data1<<8|data2)/AD7738_resolution);
#endif
	Temperature_of_resistance_Parameter(data0,data1,data2);

	DelayNS(300);

	AD7738_read_channel_data(0x0A,&data0,&data1,&data2);	/*0Ah: Data Register*/
#ifdef DEGUG
	printf("channel_2=%d ->%.2f\n",(data0<<16|data1<<8|data2),(data0<<16|data1<<8|data2)/AD7738_resolution_NP_125);
#endif
	Hydrogen_Resistance_Parameter(data0,data1,data2,temperature);
}
