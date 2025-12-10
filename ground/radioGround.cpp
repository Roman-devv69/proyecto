#define __AVR_ATmega328P__ 
#include <avr/io.h>
#include <util/delay.h>
#include <time.h>
#include "radioGround.h"
#include "definesForRadio.h"
#include "uart.h"

#define CE PB0
#define CSN PB2
#define MOSI PB3
#define SCK PB5
#define IRQ PD2


// писал отсюда https://gizmosnack.blogspot.com/2013/04/tutorial-nrf24l01-and-avr.html
// все регистры можно посмотреть на сайте https://aterlux.ru/article/nrf24l01p
void radioInit(){
    DDRB |= (1<<CE) | (1<<CSN) | (1<<SCK) | (1<<MOSI); // set pins Mosi(PB3), Sck(PB5), Ce(PB0), Csn(PB1) as output  
    SPCR |= (1<<SPE) | (1<<MSTR);
    PORTB |= (1<<CSN);
    PORTB &= ~(1<<CE);
}
 
uint8_t writeReadSpi(uint8_t data){
    SPDR = data;
    while(!(SPSR & (1<<SPIF)));
    return SPDR;
}

// Выполняет команду cmd, и читает count байт ответа, помещая их в буфер buf, возвращает регистр статуса
uint8_t radio_read_buf(uint8_t cmd, uint8_t * buf, uint8_t count) {
    PORTB &= ~(1<<CSN);
    uint8_t status = writeReadSpi(cmd);
    while (count--) {
      *(buf++) = writeReadSpi(NOP);
    }
    PORTB |= (1<<CSN);
    return status;
}

// Выполняет команду cmd, и передаёт count байт параметров из буфера buf, возвращает регистр статуса
uint8_t radio_write_buf(uint8_t cmd, uint8_t * buf, uint8_t count) {
    PORTB &= ~(1<<CSN);
    uint8_t status = writeReadSpi(cmd);
    while (count--) {
        writeReadSpi(*(buf++));
    }
    PORTB |= (1<<CSN);
    return status;
}


// Читает значение однобайтового регистра reg (от 0 до 31) и возвращает его
uint8_t radio_readreg(uint8_t reg) {
    PORTB &= ~(1<<CSN);
    writeReadSpi((reg & 31) | R_REGISTER);
    uint8_t answ = writeReadSpi(0xFF);
    PORTB |= (1<<CSN);
    return answ;
}

// Записывает значение однобайтового регистра reg (от 0 до 31), возвращает регистр статуса
uint8_t radio_writereg(uint8_t reg, uint8_t val) {
    PORTB &= ~(1<<CSN);
    uint8_t status = writeReadSpi((reg & 31) | W_REGISTER);
    writeReadSpi(val);
    PORTB |= (1<<CSN);
    return status;
}

// Читает count байт многобайтового регистра reg (от 0 до 31) и сохраняет его в буфер buf,
// возвращает регистр статуса
uint8_t radio_readreg_buf(uint8_t reg, uint8_t * buf, uint8_t count) {
    return radio_read_buf((reg & 31) | R_REGISTER, buf, count);
}
  
// Записывает count байт из буфера buf в многобайтовый регистр reg (от 0 до 31), возвращает регистр статуса
uint8_t radio_writereg_buf(uint8_t reg, uint8_t * buf, uint8_t count) {
    return radio_write_buf((reg & 31) | W_REGISTER, buf, count);
}

// Выполняет команду. Возвращает регистр статуса (отправив NOP)
uint8_t radio_cmd(uint8_t cmd) {
    PORTB &= ~(1<<CSN);
    uint8_t status = writeReadSpi(cmd);
    PORTB |= (1<<CSN);
    return status;
}

void radioModulInit(){
    radioInit();
    UART_Init();
    uint8_t remote_addr[] = {0xE7, 0xE7, 0xE7, 0xE7, 0xE7}; // Адрес удалённой стороны
    uint8_t self_addr[] = {0xC2, 0xC2, 0xC2, 0xC2, 0xC2}; //  Собственный адрес
    uint8_t chan = 3; // Номер радио-канала (в диапазоне 0 - 125)

    radio_writereg(EN_AA, (1 << ENAA_P1)); // включение автоподтверждения только по каналу 1
    radio_writereg(EN_RXADDR, (1 << ERX_P0) | (1 << ERX_P1)); // включение каналов 0 и 1
    radio_writereg(SETUP_AW, 3); // выбор длины адреса 5 байт
    radio_writereg(SETUP_RETR, 1);  
    radio_writereg(RF_CH, chan); // Выбор частотного канала
    radio_writereg(RF_SETUP, (0<<RF_DR_HIGH) | (3<<RF_PWR)); // выбор скорости 1 Мбит/с и мощности 0dBm
    
    radio_writereg_buf(RX_ADDR_P0, &remote_addr[0], 5);
    radio_writereg_buf(TX_ADDR, &remote_addr[0], 5);

    radio_writereg_buf(RX_ADDR_P1, &self_addr[0], 5);
    
    radio_writereg(RX_PW_P0, LEN);
    radio_writereg(RX_PW_P1, LEN); 
    radio_writereg(DYNPD, (1 << DPL_P0) | (1 << DPL_P1)); // включение произвольной длины для каналов 0 и 1
    radio_writereg(FEATURE, (1<<EN_DPL) | (1<<EN_DYN_ACK)); // разрешение произвольной длины пакета данных

    radio_writereg(CONFIG, (1 << EN_CRC) | (1 << CRCO) | (1 << PWR_UP) | (1 << PRIM_RX)); // Включение питания
}

void transmit(uint8_t* dataT, uint8_t len){
    uint8_t conf = radio_readreg(CONFIG);
    radio_writereg(CONFIG, conf & ~(1<<PRIM_RX));  // включаем режим передатчика
    _delay_us(500);
    radio_cmd(FLUSH_TX);  // очищаем очередь на отправку
    radio_write_buf(W_TX_PAYLOAD, dataT, len);
    PORTB |= (1<<CE);
    _delay_ms(1);
    PORTB &= ~(1<<CE);
    uint8_t a = radio_readreg(STATUS);
    UART_Transmit(a+48);
    _delay_ms(10);
    radio_writereg(STATUS, radio_cmd(NOP));
}

uint8_t* receive(uint8_t len){
    uint8_t conf = radio_readreg(CONFIG);
    radio_writereg(CONFIG, conf | (1<<PRIM_RX));  // включаем режим приемника
    _delay_ms(2);
    static uint8_t data[LEN];
    PORTB |= (1<<CE);
    while((radio_readreg(STATUS) & (1<<RX_DR)) == 0);
    _delay_ms(1);
    PORTB &= ~(1<<CE);
    radio_read_buf(R_RX_PAYLOAD, &data[0], len);
    radio_writereg(STATUS, radio_cmd(NOP));
    _delay_us(500);
    return data;
}



