

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE -1
#endif
#ifndef NULL
#define NULL 0
#endif
#ifndef ERR
#define ERR  -2
#endif

#define Temp_Res 0
#define Hydrogen_Res 1
#define DAC_temp 2

float Cubic_main(float value,unsigned char type);
void Linear_slope(float *slope, float *x, float *y, unsigned char type);