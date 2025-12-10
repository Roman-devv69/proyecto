#define TEMP_MSB    0xFA
#define TEMP_LSB    0xFB
#define TEMP_XLSB   0xFC

#define PRESS_MSB   0xF7
#define PRESS_LSB   0xF8
#define PRESS_XLSB  0xF9

#define STATUS_HEIGHT   0xF3
#define CTRL_MEAS       0xF4
#define CONFIG_HEIGHT   0xF5
#define RESET_HEIGHT    0xE0
#define ID_HEIGHT       0xD0
#define ADRESS_HEIGHT   0x76 // если SDO -> ground (если SDO -> Vcc, то 0x77)

#define T_SB        5
#define FILTER      2
#define SPI3W_EN    0
#define OSRS_T      5
#define OSRS_P      2
#define MODE        0
#define MEASURING   3
#define IM_UPDATE   0