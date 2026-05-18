#include <iostream>

#include "external/glad.h"
#include "pugixml.hpp"
#include "raylib.h"
#include "raymath.h"
#include "packers/MaxRectsBin.hpp"


int main(void) {
    InitWindow(1280, 720, "raylib example - basic window");
    SetTargetFPS(GetMonitorRefreshRate(GetCurrentMonitor()));

    pugi::xml_document doc;
    const pugi::xml_parse_result result = doc.load_file("spritesheet.xml");

    if (!result) {
        std::cerr << "Error loading file: " << result.description() << std::endl;
        return 1;
    }

    GLint maxTextureSize = 0;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);

    float width = 8192;
    float height = 8192;

    Image image = LoadImage("spritesheet.png");
    Image imageData = GenImageColor(width, height, WHITE);

    auto packer = packers::MaxRectsBin(width, height);

    for (auto frame : doc.child("TextureAtlas").children("SubTexture")) {
        const float x = frame.attribute("x").as_float();
        const float y = frame.attribute("y").as_float();
        const float w = frame.attribute("width").as_float();
        const float h = frame.attribute("height").as_float();
        auto rect = packer.insert(w, h);
        std::cout << rect.width << "/" << rect.height << std::endl;
        if(rect.width != 0 && rect.height != 0){
            ImageDraw(&imageData, image, Rectangle{.x = x, .y = y, .width = w, .height = h}, {.x = rect.x, .y = rect.y, .width = rect.width, .height = rect.height}, WHITE);
        }
    }

    Texture preview = LoadTextureFromImage(imageData);
    UnloadImage(image);
    UnloadImage(imageData);

    Camera2D cam = Camera2D{.offset = Vector2Zero(), .target = Vector2Zero(), .rotation = 0.0, .zoom = 1.0};

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(WHITE);

        cam.zoom += GetMouseWheelMove() / 10.0;
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            cam.target -= GetMouseDelta() / cam.zoom;
        }
        BeginMode2D(cam);
        DrawTexture(preview, 0, 0, WHITE);
        EndMode2D();
        EndDrawing();
    }

    CloseWindow();

    return 0;
}