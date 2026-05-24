#pragma once

#include "raylib.h"
#include <string>

class Button {
	public:
		Button(float x, float y, float width, float height, const std::string &text, const std::string& tooltip, Color color);
		~Button();
		void draw();

		float x;
		float y;
		float width;
		float height;
		Color color = BLACK;
		std::string tooltip;
		std::string text;
		int icon = -1;

		bool pressed = false;
};
