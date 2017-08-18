/******************************************************************************/
/* MEASURE.H: struct type and extern definitions for MEASURE project          */
/******************************************************************************/
/* This file is part of the uVision/ARM development tools.                    */
/* Copyright (c) 2005-2006 Keil Software. All rights reserved.                */
/* This software may only be used under the terms of a valid, current,        */
/* end user licence from KEIL for a compatible version of KEIL software       */
/* development tools. Nothing else gives you the right to use this software.  */
/******************************************************************************/

struct clock {                           /* structure of the clock record     */
  unsigned char    hour;                 /* hour                              */
  unsigned char     min;                 /* minute                            */
  unsigned char     sec;                 /* second                            */
  unsigned short   msec;                 /* milliseconds                      */
};

struct mrec  {                           /* structure for measurement records */
  struct   clock   time;                 /* time of measurement               */
  unsigned long   port0;                 /* state of port 0                   */
  unsigned char  analog [4];             /* voltage on analog Pins AD0 .. AD3 */
};

struct interval  {                       /* structure for interval record     */
  unsigned char     min;                 /* minute                            */
  unsigned char     sec;                 /* second                            */
  unsigned short   msec;                 /* milli second                      */
};

/* external functions: */
extern void init_serial (void);          /* initialize serial interface       */
extern int  sendchar (int ch);           /* write character to serial port    */
extern int  getkey (void);               /* read character from serial port   */
extern void getline (char *line, int n);          /* input line               */
extern void measure_display (struct mrec disp);   /* display mrec             */
extern void set_time (char *buffer);              /* set current time         */
extern void set_interval (char *buffer);          /* set interval time        */

extern char   ERROR [];                  /* ERROR message string              */
extern struct mrec current;              /* current measurements              */
extern struct interval setinterval;      /* interval setting values           */
extern struct interval interval;         /* interval counter                  */

extern unsigned char SPI1_SendDate(unsigned char date);
extern unsigned char SPI0_SendDate(unsigned char date);
extern void DAC8568_CS_INIT(void);
extern void M25P16_CS_INIT(void);
extern void AD7738_CS_INIT(void);
extern void DelayNS (unsigned int uiDly);
extern void DAC8568_SET(unsigned char PB,unsigned char CB,unsigned char AB,unsigned short DB,unsigned char FB);
extern void AD7738_read(unsigned char Register,unsigned char *data);
extern void AD7738_write(unsigned char Register,unsigned char data);
extern unsigned char M25P16_SET(unsigned char Instruction,unsigned char data);
extern void AD7738_SET(unsigned char channel);
extern void AD7738_read_channel_data(unsigned char Register,unsigned char *buf0,unsigned char *buf1,unsigned char *buf2);
extern void init_PWM (void);

extern int DAC_SET_Chanel_Din(float temperature);
extern void Temperature_of_resistance_Parameter(unsigned char A,unsigned char B,unsigned char C);
extern void Hydrogen_Resistance_Parameter(unsigned char A,unsigned char B,unsigned char C);