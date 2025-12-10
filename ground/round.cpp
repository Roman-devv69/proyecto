#include <fcntl.h> 
#include <fcntl2.h> 
#include <stdlib.h>     // Управление файловыми дескрипторами
#include <unistd.h>     // read(), write(), close()
#include <termios.h>    // Настройка последовательного порта
#include <cstring>  
#include <SFML/Graphics.hpp>
#include <iostream> 
#include "map.h" //подключили код с картой
 
using namespace sf;

const float size = 128;
 
void term_init(){
	int serial_port = open("/dev/ttyUSB1", O_RDWR);
}
 
////////////////////////////////////////////////////КЛАСС ИГРОКА////////////////////////
class Player { 
public:
	float x, y, w, h, dx, dy, speed ; 
	int dir ; 
	String File;
	Image image;
	Texture texture;
	Sprite sprite;
	Player(String F, float X, float Y, float W, float H){
                dx=0; dy=0; speed=1; dir = 0;
		File = F;
		w = W; h = H;
		image.loadFromFile(File);
		image.createMaskFromColor(Color(41, 33, 59));
		texture.loadFromImage(image);
		sprite.setTexture(texture);
		x = X; y = Y;
		sprite.setTextureRect(IntRect(50, 50, w, h)); 
	}
	void update(float time) 
	{
		switch (dir)
		{
		case 0: dx = speed; dy = 0; break;
		case 1: dx = -speed; dy = 0; break;
		case 2: dx = 0; dy = speed; break;
		case 3: dx = 0; dy = -speed; break;
		}
 
		x += dx*time;
		y += dy*time;
 
		speed = 0;
		sprite.setPosition(x,y);
	}
};
 
 
int main()
{
	RenderWindow window(sf::VideoMode(1280, 800), "Игра");
 
	Image map_image;//объект изображения для карты
	map_image.loadFromFile("/home/andrew0808/Изображения/map.png");//загружаем файл для карты
	Texture map;//текстура карты
	map.loadFromImage(map_image);//заряжаем текстуру картинкой
	Sprite s_map;//создаём спрайт для карты
	s_map.setTexture(map);//заливаем текстуру спрайтом
 
 
	Player p("dog.png", 0, 0, size, size);
 
	float CurrentFrame = 0;
	Clock clock;
	int counter = 0;
 
	while (window.isOpen())
	{
 
		float time = clock.getElapsedTime().asMicroseconds();
		clock.restart();
		time = time / 800;
 
 
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}
 
 
		///////////////////////////////////////////Управление персонажем с анимацией////////////////////////////////////////////////////////////////////////
		if ((Keyboard::isKeyPressed(Keyboard::Left) || (Keyboard::isKeyPressed(Keyboard::A)))) {
			p.dir = 1; p.speed = 0.1;
			p.update(time);
			
		}

		if ((Keyboard::isKeyPressed(Keyboard::Right) || (Keyboard::isKeyPressed(Keyboard::D)))) {
			p.dir = 0; p.speed = 0.1;
			p.update(time);
		}
 
		if ((Keyboard::isKeyPressed(Keyboard::Up) || (Keyboard::isKeyPressed(Keyboard::W)))) {
			p.dir = 3; p.speed = 0.1;
			p.update(time);
 
		}
 
		if ((Keyboard::isKeyPressed(Keyboard::Down) || (Keyboard::isKeyPressed(Keyboard::S)))) { 
			p.dir = 2; p.speed = 0.1;
			p.update(time);
 
		}
		p.update(time);
 
	
		window.clear();
 
		/////////////////////////////Рисуем карту/////////////////////
		for (int i = 0; i < HEIGHT_MAP; i++)
		for (int j = 0; j < WIDTH_MAP; j++)
		{
			if((p.x <= (j)* 32) & ((j+1)*32 <= (p.x + p.w)) & (p.y <= (i)*32) & ((i+1)*32 <= (p.y + p.h)) & (TileMap[i][j] == 's')){
				TileMap[i][j] = ' ';
				counter++;
			}
			if (TileMap[i][j] == ' ')  s_map.setTextureRect(IntRect(0, 0, 32, 32)); //если встретили символ пробел, то рисуем 1й квадратик
			if (TileMap[i][j] == 's')  s_map.setTextureRect(IntRect(32, 0, 32, 32));//если встретили символ s, то рисуем 2й квадратик
			if ((TileMap[i][j] == '0')) s_map.setTextureRect(IntRect(64, 0, 32, 32));//если встретили символ 0, то рисуем 3й квадратик
 
 
			s_map.setPosition(j * 32, i * 32);//по сути раскидывает квадратики, превращая в карту. то есть задает каждому из них позицию. если убрать, то вся карта нарисуется в одном квадрате 32*32 и мы увидим один квадрат
 
			window.draw(s_map);//рисуем квадратики на экран
		}
		if(counter >= 5){
			std::cout<<"Вы собрали 5 камней!";
			break;
		}
		
 
 
		window.draw(p.sprite);
		window.display();
	}
 
	return 0;
}

