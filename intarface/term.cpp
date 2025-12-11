#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <SFML/Graphics.hpp>
#include <cstring>
#include <string>
#include <algorithm>
#include <vector>
#include <iostream>
#include "term.h"

Term::Term(){
    serial_port = open("/dev/ttyUSB0", O_RDWR);
    memset(&tty, 0, sizeof(tty));

    cfsetispeed(&tty, B9600);
    cfsetospeed(&tty, B9600);

    tty.c_cflag &= ~PARENB;    // Без контроля чётности
    tty.c_cflag &= ~CSTOPB;    // 1 стоп-бит
    tty.c_cflag &= ~CSIZE;     // Очистка битов размера данных
    tty.c_cflag |= CS8;        // 8 бит данных
    tty.c_cflag &= ~CRTSCTS;   // Без аппаратного управления потоком
    tty.c_cflag |= CREAD | CLOCAL; // Включить приём и игнорировать управляющие сигналы

    // Настройка неканонического режима (raw mode)
    tty.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    tty.c_oflag &= ~OPOST;     // Без постобработки вывода

    // Установка времени ожидания (в децисекундах)
    tty.c_cc[VMIN] = 4;   // Блокировать read() до получения хотя бы 4 байта
    tty.c_cc[VTIME] = 10; // Таймаут ожидания (1 секунда)

    tcsetattr(serial_port, TCSANOW, &tty);
}

void Term::send_air(const char *com){
    std::cout<<*com<<std::endl;
    write(serial_port, com, 1);
}

void Term::get_air(uint8_t* com, int len){
    read(serial_port, com, len);
}

Term::~Term(){
    close(serial_port);
}
