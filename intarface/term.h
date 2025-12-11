#ifndef TERM_H
#define TERM_H
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

class Term{
    protected:
        struct termios tty;
        int serial_port;
    
    public:
        Term();
        ~Term();
        void send_air(const char *com);
        void get_air(uint8_t* com, int len);

};

#endif