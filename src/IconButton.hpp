#pragma once

#include "raylib.h"
#include <string>

class IconButton {
	public:
		IconButton(int x, int y, float width, float height, const std::string& tooltip, const int icon, const Color color);
		~IconButton();
		void draw();

		int x;
		int y;
		float width;
		float height;
		Color color = BLACK;
		std::string tooltip;
		int icon;

		bool pressed = false;
};
