#pragma once

#include <string>

struct Frame {
	float x;
	float y;
	float width;
	float height;
	float frameX;
	float frameY;
	float frameWidth;
	float frameHeight;
	bool rotated;
	std::string name;
};
