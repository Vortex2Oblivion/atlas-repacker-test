#pragma once

#include <memory>
#include <vector>

#include "Button.hpp"
#include "Frame.hpp"

class SpritesheetLoadMenu {
	public:
		SpritesheetLoadMenu(float x, float y, float width, float height);
		~SpritesheetLoadMenu();

		float x;
		float y;
		float width;
		float height;

		bool open = false;

		std::unique_ptr<Button> loadSpritesheet;
		std::unique_ptr<Button> loadXML;
		std::unique_ptr<Button> repack;

		static float padding;

		void draw();
		void screenCenter();
	protected:
		Image spritesheetImage{};
		Texture selectedSpritesheetPreview{};

		std::vector<Rectangle> rectsToDraw = {};
		std::vector<Frame> frames = {};

		Camera2D camPreview{};

};
