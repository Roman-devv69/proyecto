#include <avr/io.h>
#define LEN 8

void radioInit();           // настраиваем основные порты 

uint8_t writeReadSpi(uint8_t data);  // передача данных на модуль через SPI

uint8_t radio_read_buf(uint8_t cmd, uint8_t * buf, uint8_t count);

uint8_t radio_write_buf(uint8_t cmd, uint8_t * buf, uint8_t count);

uint8_t radio_readreg(uint8_t reg);

uint8_t radio_writereg(uint8_t reg);

uint8_t radio_readreg_buf(uint8_t reg, uint8_t * buf, uint8_t count);

uint8_t radio_writereg_buf(uint8_t reg, uint8_t * buf, uint8_t count);

uint8_t radio_cmd(uint8_t cmd);

void radioModulInit();

void transmit(uint8_t* dataT, uint8_t len);

uint8_t* receive(uint8_t len);