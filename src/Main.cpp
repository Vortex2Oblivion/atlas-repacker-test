#include <iostream>
#include <thread>

#include "raylib.h"
#include "raymath.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#include "external/glad.h"

#include "pugixml.hpp"

#include  "nfd.hpp"

#include "packers/MaxRectsBin.hpp"
#include "Exporter.hpp"

#include "Windows.hpp"

int main() {
    InitWindow(1280, 720, "raylib example - basic window");
    setDarkMode();
    
    SetTargetFPS(GetMonitorRefreshRate(GetCurrentMonitor()));


    pugi::xml_document doc;

    if (const pugi::xml_parse_result result = doc.load_file("spritesheet.xml"); !result) {
        TraceLog(LOG_ERROR, ("Error loading file: " + std::string(result.description())).c_str());
        return 1;
    }

    GLint maxTextureSize = 0;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);

    uint16_t width = 8192;
    uint16_t height = 8192;

    uint16_t croppedWidth = 0;
    uint16_t croppedHeight = 0;

    Image image = LoadImage("spritesheet.png");
    Image imageData = GenImageColor(width, height, BLANK);

    auto packer = packers::MaxRectsBin(width, height);

    std::vector<Rectangle> drawnFrames = {};

    auto exporter = Exporter();

    for (auto frame : doc.child("TextureAtlas").children("SubTexture")) {
        const float x = frame.attribute("x").as_float();
        const float y = frame.attribute("y").as_float();
        const float w = frame.attribute("width").as_float();
        const float h = frame.attribute("height").as_float();
        auto f = Rectangle{.x = x, .y = y, .width = static_cast<float>(width), .height = static_cast<float>(height)};
        bool skip = false;
        for(auto ff : drawnFrames){
            if(f.x == ff.x && f.y == ff.y && f.width == ff.width && f.height == ff.height){
                skip = true;
                break;
            }
        }

        if(!skip){
            auto rect = packer.insert(w, h);
            std::cout << rect.width << " / " << rect.height << std::endl;

            if(rect.width != 0 && rect.height != 0){
                ImageDraw(&imageData, image, Rectangle{.x = x, .y = y, .width = w, .height = h}, {.x = rect.x, .y = rect.y, .width = rect.width, .height = rect.height}, WHITE);
                croppedWidth = std::max(croppedWidth, static_cast<uint16_t>(rect.x + rect.width));
                croppedHeight = std::max(croppedHeight, static_cast<uint16_t>(rect.y + rect.height));
                drawnFrames.push_back(f);
            }
        }
        exporter.frames.push_back(Frame{.name = frame.attribute("name").as_string()});
    }

    UnloadImage(image);

    std::cout << "\n" << croppedWidth  << " / " << croppedHeight << std::endl;
    ImageCrop(&imageData, Rectangle{.x = 0, .y = 0, .width = static_cast<float>(croppedWidth), .height = static_cast<float>(croppedHeight)});

    const Texture preview = LoadTextureFromImage(imageData);
    SetTextureFilter(preview, TEXTURE_FILTER_BILINEAR);

    auto t = std::thread([imageData, exporter](){
        exporter.exportToFile("output", imageData);
        UnloadImage(imageData);
    });
    t.detach();

    auto cam = Camera2D{.offset = Vector2Zero(), .target = Vector2Zero(), .rotation = 0.0, .zoom = 1.0};

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(WHITE);

        cam.zoom += GetMouseWheelMove() / 10.0f;
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            cam.target -= GetMouseDelta() / cam.zoom;
        }
        BeginMode2D(cam);
        DrawTexture(preview, 0, 0, WHITE);
        EndMode2D();

        GuiButton(Rectangle{.x = 0, .y = 0, .width = 48, .height = 48}, "");
        GuiDrawIcon(ICON_FILE_OPEN, 0, 0, 3, BLACK);

        DrawFPS(0, 0);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}