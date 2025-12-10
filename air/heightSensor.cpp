#include "heightSensor.h"
#include "uart.h"

void heightInit(callibration* callib){
    uint8_t data;

    data = 0b010<<OSRS_T | 0b101<<OSRS_P | 0b11<<MODE;
    I2C_Write_Byte(ADRESS_HEIGHT, CTRL_MEAS ,data);

    data = 0b11<<FILTER;
    I2C_Write_Byte(ADRESS_HEIGHT, CONFIG_HEIGHT, data);

    callib->dig_T1 += (uint16_t)I2C_Read_Byte(ADRESS_HEIGHT, 0x88);
    callib->dig_T1 += ((uint16_t)I2C_Read_Byte(ADRESS_HEIGHT, 0x89)) << 8;

    callib->dig_T2 += (int16_t)I2C_Read_Byte(ADRESS_HEIGHT, 0x8A);
    callib->dig_T2 += ((int16_t)I2C_Read_Byte(ADRESS_HEIGHT, 0x8B)) << 8;

    callib->dig_T3 += (int16_t)I2C_Read_Byte(ADRESS_HEIGHT, 0x8C);
    callib->dig_T3 += ((int16_t)I2C_Read_Byte(ADRESS_HEIGHT, 0x8D)) << 8;

    callib->dig_P1 += (uint16_t)I2C_Read_Byte(ADRESS_HEIGHT, 0x8E);
    callib->dig_P1 += ((uint16_t)I2C_Read_Byte(ADRESS_HEIGHT, 0x8F)) << 8;

    callib->dig_P2 += (int16_t)I2C_Read_Byte(ADRESS_HEIGHT, 0x90);
    callib->dig_P2 += ((int16_t)I2C_Read_Byte(ADRESS_HEIGHT, 0x91)) << 8;

    callib->dig_P3 += (int16_t)I2C_Read_Byte(ADRESS_HEIGHT, 0x92);
    callib->dig_P3 += ((int16_t)I2C_Read_Byte(ADRESS_HEIGHT, 0x93)) << 8;

    callib->dig_P4 += (int16_t)I2C_Read_Byte(ADRESS_HEIGHT, 0x94);
    callib->dig_P4 += ((int16_t)I2C_Read_Byte(ADRESS_HEIGHT, 0x95)) << 8;

    callib->dig_P5 += (int16_t)I2C_Read_Byte(ADRESS_HEIGHT, 0x96);                 
    callib->dig_P5 += ((int16_t)I2C_Read_Byte(ADRESS_HEIGHT, 0x97)) << 8;

    callib->dig_P6 += (int16_t)I2C_Read_Byte(ADRESS_HEIGHT, 0x98);
    callib->dig_P6 += ((int16_t)I2C_Read_Byte(ADRESS_HEIGHT, 0x99)) << 8;

    callib->dig_P7 += (int16_t)I2C_Read_Byte(ADRESS_HEIGHT, 0x9A);
    callib->dig_P7 += ((int16_t)I2C_Read_Byte(ADRESS_HEIGHT, 0x9B)) << 8;

    callib->dig_P8 += (int16_t)I2C_Read_Byte(ADRESS_HEIGHT, 0x9C);
    callib->dig_P8 += ((int16_t)I2C_Read_Byte(ADRESS_HEIGHT, 0x9D)) << 8;

    callib->dig_P9 += (int16_t)I2C_Read_Byte(ADRESS_HEIGHT, 0x9E);
    callib->dig_P9 += ((int16_t)I2C_Read_Byte(ADRESS_HEIGHT, 0x9F)) << 8;
}

void getRawData(dataRaw *dataMeas){
    uint8_t data[6];
    I2C_Read_Array(ADRESS_HEIGHT, PRESS_MSB, data, 6);
    dataMeas->pressure = (((int32_t)data[0]) << 12) + (((int32_t)data[1]) << 4) + (((int32_t)data[2]) >> 4);
    dataMeas->temperature = (((int32_t)data[3]) << 12) + (((int32_t)data[4]) << 4) + (((int32_t)data[5]) >> 4);
}

void compensate(const dataRaw *dataRaw, dataFinal *dataFinal, callibration *callib){
    int32_t var1, var2, T;
    uint32_t p;
    var1 = ((((dataRaw->temperature >>3) - ((int32_t)callib->dig_T1<<1))) * ((int32_t)callib->dig_T2)) >> 11;
    var2 = (((((dataRaw->temperature>>4) - ((int32_t)callib->dig_T1)) * ((dataRaw->temperature >> 4) - ((int32_t)callib->dig_T1))) >> 12) *((int32_t)callib->dig_T3)) >> 14;
    callib->t_fine = var1 + var2;
    T = (callib->t_fine * 5 + 128) >> 8;
    

    var1 = (((int32_t)callib->t_fine)>>1) - (int32_t)64000;
    var2 = (((var1>>2) * (var1>>2)) >> 11 ) * ((int32_t)callib->dig_P6);
    var2 = var2 + ((var1*((int32_t)callib->dig_P5))<<1);
    var2 = (var2>>2)+(((int32_t)callib->dig_P4)<<16);
    var1 = (((callib->dig_P3 * (((var1>>2) * (var1>>2)) >> 13 )) >> 3) + ((((int32_t)callib->dig_P2) * var1)>>1))>>18;
    var1 =((((32768+var1))*((int32_t)callib->dig_P1))>>15);
    p = (uint32_t)(((((int32_t)1048576) - dataRaw->pressure) - (var2>>12)))*3125;

    if (p < 0x80000000)
    {
        p = (p << 1) / ((uint32_t)var1);
    }
    else
    {
        p = (p / (uint32_t)var1) * 2;
    }
    var1 = (((int32_t)callib->dig_P9) * ((int32_t)(((p>>3) * (p>>3))>>13)))>>12;
    var2 = (((int32_t)(p>>2)) * ((int32_t)callib->dig_P8))>>13;
    p = (uint32_t)((int32_t)p + ((var1 + var2 + callib->dig_P7) >> 4));
    for(uint8_t i = 0; i<4; i++){
        dataFinal->pressure[i] = uint8_t(p>>(8*(3-i)));
        dataFinal->temperature[i] = uint8_t(T>>(8*(3-i)));
    }
}