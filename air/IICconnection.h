#define __AVR_ATmega328P__ 
#include <avr/io.h>
#include <util/delay.h>

#define F_CPU 16000000UL
#define F_SCL 100000UL // Частота SCL (100 кГц)

void I2C_Init();
void I2C_Start();
void I2C_Stop();
void I2C_Write(uint8_t data);
uint8_t I2C_Read_ACK();
uint8_t I2C_Read_NACK();
void I2C_Write_Byte(uint8_t dev_addr, uint8_t reg_addr, uint8_t data);
uint8_t I2C_Read_Byte(uint8_t dev_addr, uint8_t reg_addr);
void I2C_Write_Array(uint8_t dev_addr, uint8_t reg_addr, uint8_t* data, uint8_t len);
void I2C_Read_Array(uint8_t dev_addr, uint8_t reg_addr, uint8_t* data, uint8_t len);
//uint8_t* I2C_Read_Array(uint8_t dev_addr, uint8_t reg_addr, uint8_t len);