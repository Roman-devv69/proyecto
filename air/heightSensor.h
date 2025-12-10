#define __AVR_ATmega328P__ 
#include <avr/io.h>
#include <util/delay.h>
#include "heightSensorDefs.h"
#include "IICconnection.h"

struct callibration
{
    uint16_t dig_T1 = 0;
    int16_t dig_T2 = 0;
    int16_t dig_T3 = 0;
    
    uint16_t dig_P1 = 0;
    int16_t dig_P2 = 0;
    int16_t dig_P3 = 0;
    int16_t dig_P4 = 0;
    int16_t dig_P5 = 0;
    int16_t dig_P6 = 0;
    int16_t dig_P7 = 0;
    int16_t dig_P8 = 0;
    int16_t dig_P9 = 0;

    int32_t t_fine = 0;
};

struct  dataRaw
{
    int32_t pressure;
    int32_t temperature;
};

struct  dataFinal
{
    uint8_t* pressure;
    uint8_t* temperature;
};


void heightInit(callibration* callib);
void getRawData(dataRaw *dataMeas);
void compensate(const dataRaw *dataRaw, dataFinal *dataFinal, callibration *callib);
