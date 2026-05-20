#pragma once

#include <string>

struct Frame {
	bool rotated;
	float x;
	float y;
	float width;
	float height;
	float frameX;
	float frameY;
	float frameWidth;
	float frameHeight;
	std::string name;
};
