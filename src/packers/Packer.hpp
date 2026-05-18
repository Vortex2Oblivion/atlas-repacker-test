#pragma once

#include <vector>
#include "raylib.h"
#include <cstdint>

class Packer
{
    public:
        Packer(float width, float height);
        ~Packer();

        virtual Rectangle insert(float x, float y);

        float width;
        float height;
        std::vector<Rectangle> freeRectangles;
    protected:
        std::uint32_t areaUsed;
};