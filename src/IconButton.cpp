#include "IconButton.hpp"

#ifndef RAYGUI_IMPLEMENTATION
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#endif
#include "raymath.h"

IconButton::IconButton(const int x, const int y, const float width, const float height, const std::string &tooltip, const int icon, const Color color) {
	this->x = x;
	this->y = y;
	this->width = width;
	this->height = height;
	this->tooltip = tooltip;
	this->icon = icon;
	this->color = color;
}

IconButton::~IconButton() = default;

void IconButton::draw() {
	const auto rect = Rectangle{.x = static_cast<float>(x), .y = static_cast<float>(y), .width = height, .height = height};
	pressed = GuiButton(rect, "");
	GuiDrawIcon(icon, x, y, static_cast<int>(width) / 16, color);
	if (CheckCollisionPointRec(GetMousePosition(), rect)) {
		GuiSetTooltip(tooltip.c_str());
		GuiEnableTooltip();
		auto distance = GetMousePosition() - Vector2{rect.x, rect.y};
		GuiTooltip(Rectangle{.x = rect.x + distance.x, .y = rect.y + distance.y, .width = 0, .height = 10});
		GuiDisableTooltip();
	}
}