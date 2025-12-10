#define __AVR_ATmega328P__ 
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "definesForRadio.h"
#include "uart.h"
#include "radioAir.h"
#include "IICconnection.h"
#include "heightSensor.h"
//screen /dev/ttyUSB1 9600


#define BAUD 9600        // Скорость передачи
#define UBRR_VALUE ((F_CPU / 16 / BAUD) - 1) // Значение для регистра UBRR

struct Power
{
  uint8_t motor = 0;
  uint8_t motorL = 0;
  uint8_t motorR = 0;
  uint8_t direct = 'n';
  uint8_t grad = 1;
};


void MOTOR_init(){
  DDRD |= (1<<PD5) | (1<<PD6);
  TCCR0A = (1<<COM0A1) | (1<<COM0B1) | (1<<WGM01) | (1<<WGM00) | (0<<WGM02);
  TCCR0B =  (1<<CS01) | (1<<CS00);
}

void MOTOR_delete(){
  DDRD &= ~((1<<PD5) | (1<<PD6));
  TCCR0A = 0;
  TCCR0B = 0;
}


void SERVO_init(){
  DDRB |= (1<<PB1);
  TCCR1A = (1<<COM1A1)|(1<<WGM11); 
  TCCR1B = (1<<WGM13)|(1<<WGM12)|(1<<CS11);
  ICR1 = 39999;
}

void SERVO_delete(){
  DDRB &= ~(1<<PB1);
  TCCR1A = 0;
  TCCR1B = 0;
  ICR1 = 0;
}

void powerR(uint8_t value1){
    OCR0A = value1;     // port PD6
}

void powerL(uint8_t value1){
    OCR0B = value1;    // port PD5
}

void servo(uint8_t grad){
    powerL(0);
    powerR(0);
    _delay_ms(30);
    SERVO_init();
    PORTD |= (1<<PD2);
    OCR1A = 850 +  1925 * grad;     // max = 4500     min = 850
    _delay_ms(500);
    PORTD &= ~(1<<PD2);
    PORTD |= (1<<PD3);
    OCR1A = 850 +  1925 * (2- grad); 
    _delay_ms(500);
    PORTD &= ~(1<<PD3);
    SERVO_delete();
    radioModulInit();
    _delay_ms(10);
}

void check_command(Power *power, callibration *callib){
    uint8_t change;
    uint8_t* message;
    message = receive(1);
    change = message[0];
    UART_Transmit(change);
    if(change == 'w'){                // увеличиваем мощность
      if((power->motor + 51) < 255){
        power->motor+= 51;
      }
      else{
        power->motor = 255;
      } 
    }

    else if (change == 's')           // уменьшаем мощность
    {
      if((power->motor-51) > 0){
        power->motor -= 51;
      }
      else{
        power->motor = 0;
      } 

    }

    else if(change == 'u'){           // поворачиваем серву вверх
      if((power->grad + 1) < 2){
        power->grad  += 1;
      }
      else{
        power->grad  = 2;
      }
      servo(power->grad);
    }

    else if(change == 'd'){           // поворачиваем серву вниз
      if((power->grad -1) > 0){
        power->grad  -= 1;
      }
      else{
        power->grad  = 0;
      }  
      servo(power->grad);
    }

    else if(change == 'l'){           // выключаем левый мотор
      if(power->direct == 'r'){
        power->direct = 'n';
      }
      else{
        power->direct = 'l';
      }
    }

    else if(change == 'r'){           // выключаем правый мотор
      if(power->direct == 'l'){
        power->direct = 'n';
      }
      else{
        power->direct = 'r';
      }
    }

    else if(change == 'q' || change == 'o'){    // выключаем питание и поворачиваем серву на горизонатльное положение (либо вышли из программы, либо выключили моторы)
      power->motor = 0;
      power->grad  = 1;
      power->direct = 'n';
      servo(power->grad);
    }

    else if(change == 'g'){           // экстренное снижение при потере связи
      power->motor = 255;
      power->grad = 0;
      power->direct = 'n';
      servo(power->grad);
    }

    else if(change == 'h'){
      dataRaw rawData;
      dataFinal finalData;
      UART_Transmit('4');
      getRawData(&rawData);
      UART_Transmit('3');
      compensate(&rawData, &finalData, callib);
      UART_Transmit('1');
      _delay_ms(1);
      uint8_t data[8] = {0};
      for(uint8_t i = 0; i<4; i++){
        data[i] = finalData.pressure[i];
        UART_Transmit(data[i]);
      }
      for(uint8_t i = 4; i<8; i++){
        data[i] = finalData.temperature[i-4];
        UART_Transmit(data[i]);
      }
      transmit(data, 8);
      UART_Transmit('2');
    }

    if(power->direct == 'n'){
      power->motorL = power->motor;
      power->motorR = power->motor;
    }
    else if(power->direct == 'r'){
      power->motorL = 0;
      power->motorR = power->motor;
    }
    else{
      power->motorR = 0;
      power->motorL = power->motor;
    }
    
    powerL(power->motorL);
    powerR(power->motorR);
    message = nullptr;
    
}

void init(){
  DDRD |= (1<<PD2) | (1<<PD3);
  UART_Init();
  servo(1);
  MOTOR_init();
  _delay_ms(10);
  I2C_Init();
  _delay_ms(10);
}

int main(){
  init();
  Power power;
  callibration callib;
  //heightInit(&callib);

  while(1){
    check_command(&power, &callib);
  }
  
  return 0;
}