#define TURE 1

/*---------------------------------------------AD7738 DATASHEET-----------------------------------*/
/*---------------------------ADDRESS--------------------------------------------------------------*/
#define IO_PORT_REG          0x01

#define ADC_STATUS_REG    0x04

enum {
	channel_data_0 = 0x08,
	channel_data_1,
	channel_data_2,
	channel_data_3,
	channel_data_4,
	channel_data_5,
	channel_data_6,
	channel_data_7
}CHANNEL_DATA_REG_ADDR;

enum {
	channel_status_0 = 0x20,
	channel_status_1,
	channel_status_2,
	channel_status_3,
	channel_status_4,
	channel_status_5,
	channel_status_6,
	channel_status_7
}CHANNEL_STATUS_REG_ADDR;

/*----------------------------------------Channel Setup---------------------------------*/
enum {
	channel_setup_0 = 0x28,
	channel_setup_1,
	channel_setup_2,
	channel_setup_3,
	channel_setup_4,
	channel_setup_5,
	channel_setup_6,
	channel_setup_7
}CHANNEL_SETUP_REG_ADDR;
#define AINx_AINx                        (0x3<<5)
#define AINx_AINCOM                   (0x0<<5)
#define Status_Option                   (0<<4)
#define Channel_Continuous_conversion_enable (1<<3)
#define NP_125  0x00
#define P_125   0x01
#define NP_0625 0x02
#define P_0625  0x03
#define NP_25   0x04
#define P_25    0x05

/*------------------------------------Channel Conversion Time Registers----------------------*/
enum {
	channel_conv_time_0 = 0x30,
	channel_conv_time_1,
	channel_conv_time_2,
	channel_conv_time_3,
	channel_conv_time_4,
	channel_conv_time_5,
	channel_conv_time_6,
	channel_conv_time_7
}CHANNEL_CONV_TIME_REG_ADDR;
#define Chop_Enable		(TURE<<7)
#define FW	17

/*---------------------------------------Mode Register------------------------------------*/
enum {
	channel_mode_0 = 0x38,
	channel_mode_1,
	channel_mode_2,
	channel_mode_3,
	channel_mode_4,
	channel_mode_5,
	channel_mode_6,
	channel_mode_7
}CHANNEL_MODE_REG_ADDR;
#define Idle_Mode							(0<<5)
#define Continues_Conversion_Mode			(1<<5)
#define Single_Conversion_Mode				(2<<5)
#define Power_Down_Mode					(3<<5)
#define ADC_Zero_Scale_Self_Calibration		(4<<5)
#define Channel_Zero_Scale_System_Calibration	(6<<5)
#define Channel_Full_Scale_System_Calibration		(7<<5)
#define BIT24		(1<<1)
#define BIT16		(0<<1)




