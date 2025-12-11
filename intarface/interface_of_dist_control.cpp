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
#include "scale.h"
#include "term.h"
#include <chrono>
#include <thread>
#include <math.h>

//g++ interface_of_dist_control.cpp term.cpp scale.cpp -o game -lsfml-graphics -lsfml-window -lsfml-system
using namespace sf;

const float mu = 0.029;   // молярная масса воздуха
const float R = 8.314; // универсальная газовая посоянная
const float g = 9.816; // ускорение свободного падения в Москве
bool first = false;
bool second = false;
float pressure0 = 10;
float temperature0 = 10;

float calcHeightIsoTerm(uint8_t* press, uint8_t* temp){    // изотермическая атмосфера
    uint32_t pressure1 = 0;
    int32_t temperature1 = 0;
    float height = 0;
    for(int i = 0; i<4; i++){
        pressure1 = (uint32_t(press[0]) << 24) | (uint32_t(press[1]) << 16) | (uint32_t(press[2]) << 8) | uint32_t(press[3]);
        temperature1 = ((uint32_t(temp[0])) << 24) | (uint32_t(temp[1]) << 16) | (uint32_t(temp[2]) << 8) | uint32_t(temp[3]);
    }
    float pressure = float(pressure1);
    float temperature = float(temperature1) / 100 + 273.15;
    if(first == false){
        if(second == true){
            pressure0 = pressure;
            temperature0 = temperature;

            first = true;
        }
        second = true;
    }
    else{
        height = R * temperature / mu / g * log(pressure0/pressure);
    }
    
    std::cout<<pressure<<" "<<temperature<<std::endl;
    return height;
    
}

int main(){
    RenderWindow window(sf::VideoMode(1920, 1080), "Управление");
    Font font;
    font.loadFromFile("/usr/share/fonts/truetype/liberation/LiberationMono-BoldItalic.ttf");
    char com;
    Term term;
    int xPower = 160;
    int yPower = 300;
    int xVertical= 1400;
    int yVertical = 300;
    int xDirect = 600;
    int yDirect = 650;
    int widthHeight = 240;
    int heightHeight = 84;
    int xHeight = xDirect - widthHeight/2 + 280;
    int yHeight = yPower;

    Power power(xPower, yPower, font);
    Height vertical(xVertical, yVertical, font);
    Direction direction(xDirect, yDirect, font);
    Sign levelOfHeight(widthHeight, heightHeight, xHeight, yHeight, font, "height");
    float height;
    uint8_t press[4];
    uint8_t temp[4];
    /*com = 'h';
    usleep(1000000);
	term.send_air(&com);
    usleep(100000);
    term.get_air(press, 4);
    term.get_air(temp, 4);
    height = calcHeightIsoTerm(press, temp);*/
    
    auto start = std::chrono::steady_clock::now();

    while(window.isOpen()){
        sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}
        auto now = std::chrono::steady_clock::now();
        if((now - start) >= std::chrono::seconds(25)){      
            com = ' ';
			term.send_air(&com);
            start = now;
        }

        if ((Keyboard::isKeyPressed(Keyboard::Left) || (Keyboard::isKeyPressed(Keyboard::A))) & ((now - start) >= std::chrono::milliseconds(600))) {    // отключаем левый мотор
			com = 'r';          // поменять обратно поворот вправо и влево и изменить наоборот при  повороте включение моторов (если сделать здесь 'l', а в следуюзем условии 'r', то моторы поворачивают в противоположную от нужной стороны)
			term.send_air(&com);
			direction.adding_more();
            start = now;
            usleep(300000);
		}

		if ((Keyboard::isKeyPressed(Keyboard::Right) || (Keyboard::isKeyPressed(Keyboard::D)))& ((now - start) >= std::chrono::milliseconds(600))) {  // отключаем правый мотор 
			com = 'l';
			term.send_air(&com);
            direction.adding_less();
            start = now;
            usleep(300000);
		}
 
		if (Keyboard::isKeyPressed(Keyboard::W) & ((now - start) >= std::chrono::milliseconds(600))) {          // увеличиваем мощность
			com = 'w';
			term.send_air(&com);
            power.adding_more();
            start = now;
            usleep(300000);
		}
 
		if (Keyboard::isKeyPressed(Keyboard::S) & ((now - start) >= std::chrono::milliseconds(600))) {          // уменьшаем мощность
			com = 's';
			term.send_air(&com);
            power.adding_less();
            start = now;
            usleep(300000);
		}

        if(Keyboard::isKeyPressed(Keyboard::Up) & ((now - start) >= std::chrono::milliseconds(600))){           // поворачиваем серву вверх
            com = 'u';
            term.send_air(&com);
            vertical.adding_more();
            start = now;
            usleep(300000);
        }

        if(Keyboard::isKeyPressed(Keyboard::Down) & ((now - start) >= std::chrono::milliseconds(600))){         // поворачиваем серву вниз
            com = 'd';
            term.send_air(&com);
            vertical.adding_less();
            start = now;
            usleep(300000);
        }

        if(Keyboard::isKeyPressed(Keyboard::H) & ((now - start) >= std::chrono::milliseconds(600))){         //  вручную узнаем высоту
            com = 'h';
			term.send_air(&com);
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            term.get_air(press, 4);
            term.get_air(temp, 4);
            printf("Pressure bytes: ");
            for(int i = 0; i < 4; i++) printf("%02X ", press[i]);

            printf("\nTemperature bytes: ");
            for(int i = 0; i < 4; i++) printf("%02X ", temp[i]);
            printf("\n");
            height = calcHeightIsoTerm(press, temp);
            start = now;
            usleep(300000);
        }

		if (Keyboard::isKeyPressed(Keyboard::Q)) {          // закрываем программу
			com = 'q';
			term.send_air(&com);
			window.clear();
			break;
		}

        if (Keyboard::isKeyPressed(Keyboard::O)) {          // выключаем моторы и поворачиваем их в горизонтальное положение
			com = 'o';
			term.send_air(&com);
			for(int i = 0; i<5; i++){
                power.adding_less();
                vertical.adding_more();
                direction.adding_more();
            }
            vertical.adding_less();
            direction.adding_less();
            usleep(300000);
		}

        levelOfHeight.number = height;

        usleep(1000);
        window.clear(Color::White);
        power.show(window);
        vertical.show(window);
        direction.show(window);
        levelOfHeight.show(window);
        window.display();

    }
    return 0;
}