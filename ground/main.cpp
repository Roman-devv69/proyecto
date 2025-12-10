#define __AVR_ATmega328P__ 
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>

#include "definesForRadio.h"
#include "uart.h"
#include "radioGround.h"
//screen /dev/ttyUSB0 9600

#define BAUD 9600        // Скорость передачи
#define UBRR_VALUE ((F_CPU / 16 / BAUD) - 1) // Значение для регистра UBRR


uint8_t get_and_send_command(){
  uint8_t command[1];
  uint8_t* data;
  command[0] = UART_Receive();
  transmit(command, 1);
  return command[0];
}

void send_pressure_and_temp(){
    uint8_t* data = nullptr;
    data = receive(8);
    for(uint8_t i = 0; i < 8; i++){
      UART_Transmit(data[i]);
    }
    
}

void init(){
  UART_Init();
  _delay_ms(500);
  radioModulInit();
  _delay_ms(10);
}

int main() {
  init();
  uint8_t command = 0;
  while(1){
    command = get_and_send_command();
    if(command == 'h'){
        send_pressure_and_temp();
        }
    }
   
  return 0;
  
}