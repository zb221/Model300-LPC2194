
#define Channel_data_base_addr 0x08
#define channel_data_0

/*Channel Setup*/
#define NP_125  0x00
#define P_125   0x01
#define NP_0625 0x02
#define P_0625  0x03
#define NP_25   0x04
#define P_25    0x05

#define Channel_Continuous_conversion_enable 1<<3 
#define Status_Option 0<<4
#define AINx_AINx 0x3<<5
#define AINx_AINCOM 0x0<<5