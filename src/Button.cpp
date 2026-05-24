#include "Button.hpp"

#include "raygui.h"

Button::Button(const float x, const float y, const float width, const float height, const std::string &text, const std::string& tooltip, const Color color) {
	this->x = x;
	this->y = y;
	this->width = width;
	this->height = height;
	this->text = text;
	this->tooltip = tooltip;
	this->color = color;
	if (!tooltip.empty()) {
		GuiEnableTooltip();
		GuiSetTooltip(this->tooltip.c_str());
	}
}

Button::~Button() = default;


void Button::draw() {
	const auto rect = Rectangle{.x = x, .y = y, .width = width, .height = height};
	pressed = GuiButton(rect, text.c_str());
	if (icon != -1) {
		GuiDrawIcon(icon, static_cast<int>(x), static_cast<int>(y), static_cast<int>(width) / 16, color);
	}
	if (CheckCollisionPointRec(GetMousePosition(), rect)) {
		if (tooltip.empty()) {
			GuiDisableTooltip();
		}
		else {
			GuiEnableTooltip();
		}
		GuiSetTooltip(tooltip.c_str());
	}
}
