#include "SpritesheetLoadMenu.hpp"

#include "FileUtil.hpp"
#include "MaxRectsBinPack.h"
#include "nfd.hpp"
#include "pugixml.hpp"
#include "raygui.h"
#include "raylib.h"
#include "raymath.h"

#ifndef RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT
#define RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT 24
#endif

float SpritesheetLoadMenu::padding = 5;

SpritesheetLoadMenu::SpritesheetLoadMenu(const int x, const int y, const float width, const float height) {
	this->x = x;
	this->y = y;
	this->width = width;
	this->height = height;


	auto buttonWidth = width / 3 - padding * 3;
	constexpr auto buttonHeight = 48.0f;

	this->loadSpritesheet = std::make_unique<Button>(10, 10, buttonWidth, buttonHeight, "Load Spritesheet", "", BLACK);
	this->loadXML = std::make_unique<Button>(10, 10, buttonWidth, buttonHeight, "Load XML", "", BLACK);
	this->repack = std::make_unique<Button>(10, 10, buttonWidth, buttonHeight, "Repack", "Repack the spritesheet.",
	                                        BLACK);

	const auto pos = Vector2{.x = static_cast<float>(x), .y = static_cast<float>(y)};

	this->camPreview = Camera2D{
		.offset = pos,
		.target = pos,
		.rotation = 0.0,
		.zoom = 1.0
	};
}

SpritesheetLoadMenu::~SpritesheetLoadMenu() = default;

void SpritesheetLoadMenu::draw() {
	int x = (GetRenderWidth() - static_cast<int>(this->width)) / 2;
	int y = (GetRenderHeight() - static_cast<int>(this->height)) / 2;
	const auto position = Rectangle{
		.x = static_cast<float>(x),
		.y = static_cast<float>(y),
		.width = this->width,
		.height = this->height
	};
	open = !GuiWindowBox(position, "Select files");


	if (loadSpritesheet->pressed) {

		auto [outPath, result] = FileUtil::openFileDialog({{"Image file", "png"}});

		if (result == NFD_OKAY) {
			loadSpritesheet->tooltip = outPath;
			UnloadImage(spritesheetImage);
			UnloadTexture(selectedSpritesheetPreview);
			spritesheetImage = LoadImage(outPath.c_str());
			selectedSpritesheetPreview = LoadTexture(outPath.c_str());
		} else {
			TraceLog(LOG_ERROR, NFD_GetError());
		}
	}

	if (loadXML->pressed) {
		auto [outPath, result] = FileUtil::openFileDialog({{"XML file", "xml"}});

		if (result == NFD_OKAY) {
			loadXML->tooltip = outPath;
			rectsToDraw.clear();
			pugi::xml_document doc;
			doc.load_file(outPath.c_str());

			for (auto frame: doc.child("TextureAtlas").children("SubTexture")) {
				rectsToDraw.push_back(Rectangle{
					.x = frame.attribute("x").as_float() + static_cast<float>(x),
					.y = frame.attribute("y").as_float() + static_cast<float>(y),
					.width = frame.attribute("width").as_float(),
					.height = frame.attribute("height").as_float()
				});
				frames.push_back(Frame{
					.x = frame.attribute("x").as_float(),
					.y = frame.attribute("y").as_float(),
					.width = frame.attribute("width").as_float(),
					.height = frame.attribute("height").as_float(),
					.frameX = frame.attribute("frameX").as_float(),
					.frameY = frame.attribute("frameY").as_float(),
					.frameWidth = frame.attribute("frameWidth").as_float(),
					.frameHeight = frame.attribute("frameHeight").as_float(),
					.rotated = frame.attribute("rotated").as_bool(),
					.name = frame.attribute("name").as_string()
				});
			}
		} else {
			TraceLog(LOG_ERROR, NFD_GetError());
		}
	}

	if (repack->pressed) {
		constexpr auto packerWidth = 8192;
		constexpr auto packerHeight = 8192;

		uint16_t croppedWidth = 0;
		uint16_t croppedHeight = 0;

		auto outputImage = GenImageColor(packerWidth, packerHeight, BLANK);

		auto packer = rbp::MaxRectsBinPack(packerWidth, packerHeight, false);

		std::vector<Frame> packedFrames = {};


		for (const auto &frame: frames) {
			rbp::Rect _packedRect{};
			Rectangle packedRect{};
			for (const auto &packedFrame: packedFrames) {
				if (frame.x == packedFrame.x && frame.y == packedFrame.y && frame.width == packedFrame.width && frame.
				    height == packedFrame.height) {
					goto end;
				}
			}

			// ReSharper disable once CppUseStructuredBinding
			_packedRect = packer.Insert(static_cast<uint16_t>(frame.width),
			                                       static_cast<uint16_t>(frame.height),
			                                       rbp::MaxRectsBinPack::RectBottomLeftRule);
			// ReSharper disable once CppUseStructuredBinding
			packedRect = Rectangle{
				.x = static_cast<float>(_packedRect.x),
				.y = static_cast<float>(_packedRect.y),
				.width = static_cast<float>(_packedRect.width),
				.height = static_cast<float>(_packedRect.height)
			};
			if (packedRect.width != 0 && packedRect.height != 0) {
				// ReSharper disable once CppUseStructuredBinding
				ImageDraw(&outputImage, spritesheetImage,
				          Rectangle{.x = frame.x, .y = frame.y, .width = frame.width, .height = frame.height},
				          {
					          .x = packedRect.x, .y = packedRect.y, .width = packedRect.width,
					          .height = packedRect.height
				          },
				          WHITE);
				packedFrames.push_back(frame);
				croppedWidth = std::max(croppedWidth, static_cast<uint16_t>(packedRect.x + packedRect.width));
				croppedHeight = std::max(croppedHeight, static_cast<uint16_t>(packedRect.y + packedRect.height));
			}
			end:;
		}
		ImageCrop(&outputImage, Rectangle{
			          .x = 0, .y = 0, .width = static_cast<float>(croppedWidth),
			          .height = static_cast<float>(croppedHeight)
		          });
		ExportImage(outputImage, "output.png");
		UnloadImage(outputImage);
		//UnloadTexture(selectedSpritesheetPreview);
	}

	if (IsTextureValid(selectedSpritesheetPreview)) {
		if (GetMouseWheelMove() != 0) {
			camPreview.target = Vector2{.x = static_cast<float>(x) / 2.0f, .y = static_cast<float>(y) / 2.0f} + GetMousePosition();
		}
		camPreview.zoom = Clamp(camPreview.zoom + GetMouseWheelMove() / 10.0f, 0.1f, 3.0f);
		if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
			camPreview.target -= GetMouseDelta() / camPreview.zoom;
		}
		const auto clipRect = Rectangle{
			.x = static_cast<float>(x) + padding,
			.y = static_cast<float>(y) + RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT + loadSpritesheet->height + padding * 2,
			.width = this->width - padding * 2,
			.height = this->height - RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT - loadSpritesheet->height - padding * 3
		};
		/*if (CheckCollisionPointRec(GetMousePosition(), clipRect)) {
		    if (lastCursor != MOUSE_CURSOR_RESIZE_ALL) {
		        SetMouseCursor(MOUSE_CURSOR_RESIZE_ALL);
		        lastCursor = MOUSE_CURSOR_RESIZE_ALL;
		    }
		}
		else {
		    if (lastCursor != MOUSE_CURSOR_DEFAULT) {
		        SetMouseCursor(MOUSE_CURSOR_DEFAULT);
		        lastCursor = MOUSE_CURSOR_DEFAULT;
		    }
		}*/
		BeginScissorMode(static_cast<int>(clipRect.x), static_cast<int>(clipRect.y), static_cast<int>(clipRect.width),
		                 static_cast<int>(clipRect.height));
		BeginMode2D(camPreview);
		DrawTexture(selectedSpritesheetPreview, x, y, WHITE);
		for (const auto rect: rectsToDraw) {
			DrawRectanglePro(rect, Vector2Zero(), 0.0f, ColorAlpha(BLUE, 0.1));
		}
		EndMode2D();
		DrawRectangleLinesEx(clipRect, static_cast<float>(GuiGetStyle(STATUSBAR, BORDER_WIDTH)), BLACK);
		EndScissorMode();
	}

	loadSpritesheet->x = x + padding * 3 + (static_cast<int>(this->width) - x * 2) / 3;
	loadSpritesheet->y = static_cast<int>(position.y) + RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT + padding;
	loadSpritesheet->draw();

	loadXML->x = loadSpritesheet->x + static_cast<int>(loadSpritesheet->width) + padding;
	loadXML->y = loadSpritesheet->y;
	loadXML->draw();

	repack->x = loadXML->x + static_cast<int>(loadXML->width) + padding;
	repack->y = loadXML->y;
	repack->draw();
}
