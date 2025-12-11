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

using namespace sf;

class Scale
{
    protected:
        int x;
        int y;
        int width;
        int height;
        int level;
        std::string name;
        RectangleShape rect;
        Text text;
        Font font;
        std::vector<RectangleShape> filling1;
        std::vector<RectangleShape> filling2;
    public:
        Scale(int x_, int y_, int width_, int height_, std::string name_, const Font font_);
        void show(RenderWindow &window);
        virtual void adding_less() = 0;
        virtual void adding_more() = 0;
};

class Height: public Scale{
    protected:
        int width = 240;
        int height = 420;
    public:
        Height(int x_, int y_, const Font font);
        void adding_more();
        void adding_less();
};

class Direction: public Scale{
    protected:
        int width = 560;
        int height = 150;
    public:
        Direction(int x_, int y_, const Font font);
        void adding_more();
        void adding_less();
};

class Power: public Scale{
    protected:
        int width = 240;
        int height = 420;
    public:
        Power(int x_, int y_, const Font font);
        void adding_more();
        void adding_less();
};

class Sign
{
    protected:
        int width;
        int height;
        int x;
        int y;
        std::string name;
        RectangleShape rect;
        Text text;
        Font font;
        Text textInside;
    public:
        float number = 0;
        Sign(int width_, int height_, int x_, int y_, const Font font_, std::string name_);
        void show(RenderWindow &window);
};
