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
#include <cmath>

using namespace sf;

Scale::Scale(int x_, int y_, int width_, int height_, std::string name_, const Font font_): x(x_), y(y_), width(width_), height(height_), name(name_), font(font_){
    rect.setSize(Vector2f(width, height));
    rect.setOutlineColor(Color(0, 0, 0));
    rect.setOutlineThickness(5);
    rect.setFillColor(Color(0, 0, 0, 100));
    rect.setPosition(x, y);

    text.setString(name);
    text.setFont(font);
    text.setCharacterSize(50);
    text.setFillColor(Color::Black);
};

void Scale::show(RenderWindow & window){
    window.draw(rect);
    window.draw(text);
    for(int i = 0; i<filling1.size(); i++){
        window.draw(filling1[i]);
    }
    for(int i = 0; i<filling2.size(); i++){
        window.draw(filling2[i]);
    }
}



Direction::Direction(int x_, int y_, const Font font): Scale(x_, y_, 560, 150, "direction", font){
    level = 3;
    text.setPosition(x+150, y - 75);
    RectangleShape rect1(Vector2f(width/level, height));
    rect1.setPosition(x + width/level*(level/2), y);
    rect1.setFillColor(Color(0, 255, 0));
    rect1.setOutlineColor(Color(0, 0, 0));
    rect1.setOutlineThickness(3);
    filling1.push_back(rect1);
};

void Direction::adding_more(){
    if(filling2.size()){
        filling2.pop_back();
    }
    else if(filling1.size() < level/2 +1){
        RectangleShape rect1;
        int x1;
        int y1;
        rect1.setSize(Vector2f(width/level, height));
        x1 = x + width/level*(level/2) - width/level*filling1.size();
        y1 = y;
        rect1.setPosition(x1, y1);
        //rect1.setPosition(x, y+420-420/level - 420/level*filling.size());
        rect1.setFillColor(Color(0, 255, 0));
        rect1.setOutlineColor(Color(0, 0, 0));
        rect1.setOutlineThickness(3);
        filling1.push_back(rect1);
    }
}

void Direction::adding_less(){
    if(filling1.size()-1){
        filling1.pop_back();
    }
    else if(filling2.size() < level/2){
        RectangleShape rect1;
        int x1;
        int y1;
        rect1.setSize(Vector2f(width/level, height));
        x1 = x + width/level * (level/2 + 1) + width/level * filling2.size();
        y1 = y;
        rect1.setPosition(x1, y1);
        rect1.setFillColor(Color(0, 255, 0));
        rect1.setOutlineColor(Color(0, 0, 0));
        rect1.setOutlineThickness(3);
        filling2.push_back(rect1);
    }
}



Power::Power(int x_, int y_, const Font font): Scale(x_, y_, 240, 420, "power", font){
    width = 240;
    height = 420;
    text.setPosition(x+50, y - 75);
    level = 5;
};

void Power::adding_more(){
    if(filling1.size() < level){
        RectangleShape rect1;
        int x1;
        int y1;
        rect1.setSize(Vector2f(width, height/level));
        x1 = x;
        y1 = y+height-height/level - height/level*filling1.size();
        rect1.setPosition(x1, y1);
        rect1.setFillColor(Color(0, 255, 0));
        rect1.setOutlineColor(Color(0, 0, 0));
        rect1.setOutlineThickness(3);
        filling1.push_back(rect1);
    }
    
}

void Power::adding_less(){
    if(filling1.size()){
        filling1.pop_back();
    }
}
        


Height::Height(int x_, int y_, const Font font): Scale(x_, y_, 240, 420, "vertical", font){
    text.setPosition(x+50, y - 75);
    level = 3;
    RectangleShape rect1(Vector2f(width, height/level));
    rect1.setPosition(x, y + height - height * 2/level);
    rect1.setFillColor(Color(0, 255, 0));
    rect1.setOutlineColor(Color(0, 0, 0));
    rect1.setOutlineThickness(3);
    filling1.push_back(rect1);
};

void Height::adding_more(){
    if(filling2.size()){
        filling2.pop_back();
    }
    else if(filling1.size() < level/2 +1){
        RectangleShape rect1;
        int x1;
        int y1;
        rect1.setSize(Vector2f(width, height/level));
        x1 = x;
        y1 = y+height-2*height/level - height/level*filling1.size();
        rect1.setPosition(x1, y1);
        //rect1.setPosition(x, y+420-420/level - 420/level*filling.size());
        rect1.setFillColor(Color(0, 255, 0));
        rect1.setOutlineColor(Color(0, 0, 0));
        rect1.setOutlineThickness(3);
        filling1.push_back(rect1);
    }
}

void Height::adding_less(){
    if(filling1.size()-1){
        filling1.pop_back();
    }
    else if(filling2.size() < level/2){
        RectangleShape rect1;
        int x1;
        int y1;
        rect1.setSize(Vector2f(width, height/level));
        x1 = x;
        y1 = y + height/level * (level/2 + 1) + height/level * filling2.size();
        rect1.setPosition(x1, y1);
        rect1.setFillColor(Color(0, 255, 0));
        rect1.setOutlineColor(Color(0, 0, 0));
        rect1.setOutlineThickness(3);
        filling2.push_back(rect1);
    }
}

Sign::Sign(int width_, int height_, int x_, int y_, const Font font_, std::string name_): width(width_), height(height_), x(x_), y(y_), font(font_), name(name_){
    rect.setSize(Vector2f(width, height));
    rect.setOutlineColor(Color(0, 0, 0));
    rect.setOutlineThickness(5);
    rect.setFillColor(Color(0, 0, 0, 100));
    rect.setPosition(x, y);

    text.setString(name);
    text.setFont(font);
    text.setCharacterSize(50);
    text.setFillColor(Color::Black);
    text.setPosition(x+50, y - 75);

    textInside.setString("0");
    textInside.setFont(font);
    textInside.setCharacterSize(50);
    textInside.setFillColor(Color::Black);
    textInside.setPosition(x + 10, y);
}

void Sign::show(RenderWindow &window){
    number = round(number * 10) / 10;
    std::string stringHeight = std::to_string(number);
    stringHeight.erase(stringHeight.size() - 5, 5);
    textInside.setString(stringHeight);
    window.draw(rect);
    window.draw(text);
    window.draw(textInside);
}