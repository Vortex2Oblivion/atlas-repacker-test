#include "Packer.hpp"

Packer::Packer(float width, float height){
    this->width = width;
    this->height = height;
    areaUsed = 0;
    freeRectangles = {Rectangle{.x = 0, .y = 0, .width = (float)width, .height = (float)height}};
}

Packer::~Packer() = default;

Rectangle Packer::insert(float x, float y){
    return {};
};