#define __AVR_ATmega328P__ 
#include <avr/io.h>
#include <util/delay.h>
#include "uart.h"
#define BAUD 9600        // Скорость передачи
#define UBRR_VALUE ((F_CPU / 16 / BAUD) - 1) // Значение для регистра UBRR

void UART_Init() {
    // Установка скорости передачи
    UBRR0H = (UBRR_VALUE >> 8);
    UBRR0L = UBRR_VALUE;
  
    // Включение приемника и передатчика
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);
  
    // Формат кадра: 8 бит данных, 1 стоп-бит, без контроля четности
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}
  
void UART_Transmit(uint8_t data) {
    // Ожидание готовности передатчика
    while (!(UCSR0A & (1 << UDRE0))) {}
    // Запись данных в буфер передатчика
    UDR0 = data;
}
  
uint8_t UART_Receive() {
    // Ожидание приема данных
    while (!(UCSR0A & (1 << RXC0))) {}
    // Чтение данных из буфера приемника
    return UDR0;
}