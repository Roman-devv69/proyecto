#include "IICconnection.h"

void I2C_Init() {
    // Установка частоты SCL
    TWBR = (uint8_t)(((F_CPU / F_SCL) - 16) / 2);
    TWSR = 0x00; // Предделитель = 1
}

void I2C_Start() {
    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
    while (!(TWCR & (1 << TWINT)));
}

void I2C_Stop() {
    TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
}

void I2C_Write(uint8_t data) {
    TWDR = data;
    TWCR = (1 << TWINT) | (1 << TWEN);
    while (!(TWCR & (1 << TWINT)));
}

uint8_t I2C_Read_ACK() {
    TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);
    while (!(TWCR & (1 << TWINT)));
    return TWDR;
}

uint8_t I2C_Read_NACK() {
    TWCR = (1 << TWINT) | (1 << TWEN);
    while (!(TWCR & (1 << TWINT)));
    return TWDR;
}

void I2C_Write_Byte(uint8_t dev_addr, uint8_t reg_addr, uint8_t data) {
    I2C_Start();
    I2C_Write(dev_addr << 1); // Адрес + бит записи (0)
    I2C_Write(reg_addr);
    I2C_Write(data);
    I2C_Stop();
    _delay_ms(5); // Ожидание записи
}

void I2C_Write_Array(uint8_t dev_addr, uint8_t reg_addr, uint8_t* data, uint8_t len){
    I2C_Start();
    I2C_Write(dev_addr << 1); // Адрес + бит записи (0)
    for(uint8_t i = 0; i<len; i++){
        I2C_Write(reg_addr);
        I2C_Write(data[i]);
        _delay_ms(5);
    }
    I2C_Stop();

}

uint8_t I2C_Read_Byte(uint8_t dev_addr, uint8_t reg_addr) {
    uint8_t data;
    
    I2C_Start();
    I2C_Write(dev_addr << 1); // Режим записи
    I2C_Write(reg_addr);
    
    I2C_Start();
    I2C_Write((dev_addr << 1) | 0x01); // Режим чтения
    data = I2C_Read_NACK();
    I2C_Stop();
    
    return data;
}

void I2C_Read_Array(uint8_t dev_addr, uint8_t reg_addr, uint8_t* data, uint8_t len){

    I2C_Start();
    I2C_Write(dev_addr << 1); // Режим записи
    I2C_Write(reg_addr);
    
    I2C_Start();
    I2C_Write((dev_addr << 1) | 0x01); // Режим чтения
    for(uint8_t i = 0; i<len-1; i++){
        data[i] = I2C_Read_ACK();
    }
    data[len-1] = I2C_Read_NACK();
    I2C_Stop();
}