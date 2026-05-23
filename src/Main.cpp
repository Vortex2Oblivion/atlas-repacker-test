#include <iostream>

#include "external/glad.h"
#include "nfd.hpp"
#include "pugixml.hpp"
#include "raylib.h"
#include "raymath.h"

#define RAYGUI_IMPLEMENTATION
#include <vector>

#include "raygui.h"

#include "Button.hpp"
#include "Frame.hpp"

#include "Windows.hpp"
#include "packers/MaxRectsBin.hpp"

int main() {
    NFD_Init();
    InitWindow(1280, 720, "raylib example - basic window");
    setDarkMode();
    
    SetTargetFPS(GetMonitorRefreshRate(GetCurrentMonitor()));

    GLint maxTextureSize = 0;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);

#ifdef false
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
#endif

    Image spritesheetImage{};

    auto screenCenter = Vector2{.x = static_cast<float>(GetRenderWidth()) / 2, .y = static_cast<float>(GetRenderHeight()) / 2};

    auto camPreview = Camera2D{
        .offset = screenCenter,
        .target = screenCenter,
        .rotation = 0.0,
        .zoom = 1.0
    };

    constexpr auto padding = 5;
    constexpr auto popupWindowWidth = 650.0f;
    // ReSharper disable once CppTooWideScope
    constexpr auto popupWindowHeight = 480.0f;

    auto loadFiles = Button(10, 10, 48, 48, "", "Load Files", BLACK);
    loadFiles.icon = ICON_FILE_OPEN;

    constexpr auto buttonWidth = popupWindowWidth / 3 - padding * 3;
    constexpr auto buttonHeight = 48.0f;

    auto loadSpritesheet = Button(10, 10, buttonWidth, buttonHeight, "Load Spritesheet", "", BLACK);
    auto loadXML = Button(10, 10, buttonWidth, buttonHeight, "Load XML", "", BLACK);
    auto repack = Button(10, 10, buttonWidth, buttonHeight, "Repack", "Repack the spritesheet.", BLACK);

    bool inSheetSelect = false;

    Texture selectedSpritesheetPreview{};

    int lastCursor = MOUSE_CURSOR_DEFAULT;

    std::vector<Frame> frames = {};
    std::vector<Rectangle> rectsToDraw = {};

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(WHITE);

        loadFiles.draw();

        if (loadFiles.pressed) {
            inSheetSelect = true;
#ifdef false
            nfdu8char_t *outPath;
            constexpr nfdu8filteritem_t filters[] = { { "PNG or XML file", "png,xml" } };
            nfdopendialogu8args_t args = {nullptr};
            args.filterList = filters;
            args.filterCount = std::size(filters);
            if (const nfdresult_t result = NFD_OpenDialogU8_With(&outPath, &args); result == NFD_OKAY)
            {
                if (std::string pathAsString = outPath; pathAsString.ends_with(".png")) {
                    UnloadTexture(preview);
                    preview = LoadTexture(outPath);
                }
                else if (pathAsString.ends_with(".xml")) {
                    if (const pugi::xml_parse_result xmlParseResult = doc.load_file(outPath); !xmlParseResult) {
                        TraceLog(LOG_ERROR, ("Error loading file: " + std::string(xmlParseResult.description())).c_str());
                    }
                }
                NFD_FreePathU8(outPath);
            }
            else
            {
                TraceLog(LOG_ERROR, NFD_GetError());
            }
#endif
        }

        if (inSheetSelect) {
            int x = (GetRenderWidth() - static_cast<int>(popupWindowWidth)) / 2;
            int y = (GetRenderHeight() - static_cast<int>(popupWindowHeight)) / 2;
            const auto position = Rectangle{
                .x = static_cast<float>(x),
                .y = static_cast<float>(y),
                .width = popupWindowWidth,
                .height = popupWindowHeight
            };
            inSheetSelect = !GuiWindowBox(position, "Select files");


            if (loadSpritesheet.pressed) {
                nfdu8char_t *outPath;
                constexpr nfdu8filteritem_t filters[] = { { "Image file", "png" } };
                nfdopendialogu8args_t args = {nullptr};
                args.filterList = filters;
                args.filterCount = std::size(filters);
                if (const nfdresult_t result = NFD_OpenDialogU8_With(&outPath, &args); result == NFD_OKAY)
                {
                    loadSpritesheet.tooltip = outPath;
                    UnloadImage(spritesheetImage);
                    UnloadTexture(selectedSpritesheetPreview);
                    spritesheetImage = LoadImage(outPath);
                    selectedSpritesheetPreview = LoadTexture(outPath);
                    NFD_FreePathU8(outPath);
                }
                else
                {
                    TraceLog(LOG_ERROR, NFD_GetError());
                }
            }

            if (loadXML.pressed) {
                nfdu8char_t *outPath;
                constexpr nfdu8filteritem_t filters[] = { { "XML file", "xml" } };
                nfdopendialogu8args_t args = {nullptr};
                args.filterList = filters;
                args.filterCount = std::size(filters);
                if (const nfdresult_t result = NFD_OpenDialogU8_With(&outPath, &args); result == NFD_OKAY)
                {
                    loadXML.tooltip = outPath;
                    rectsToDraw.clear();
                    pugi::xml_document doc;
                    doc.load_file(outPath);

                    for (auto frame : doc.child("TextureAtlas").children("SubTexture")) {
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
                    NFD_FreePathU8(outPath);
                }
                else
                {
                    TraceLog(LOG_ERROR, NFD_GetError());
                }
            }

            if (repack.pressed) {
                auto packerWidth = 8192;
                auto packerHeight = 8192;

                uint16_t croppedWidth = 0;
                uint16_t croppedHeight = 0;

                auto outputImage = GenImageColor(packerWidth, packerHeight, BLANK);

                auto packer = packers::MaxRectsBin(packerWidth, packerHeight);

                std::vector<Frame> packedFrames = {};

                bool skip = false;

                for (const auto& frame : frames) {
                    skip = false;
                    for (const auto& packedFrame : packedFrames) {
                        if (frame.x == packedFrame.x && frame.y == packedFrame.y && frame.width == packedFrame.width && frame.height == packedFrame.height) {
                            skip = true;
                            break;
                        }
                    }
                    if (skip) {
                        continue;
                    }
                    // ReSharper disable once CppUseStructuredBinding
                    auto packedRect = packer.insert(static_cast<uint16_t>(frame.width), static_cast<uint16_t>(frame.height));
                    if (packedRect.width != 0 && packedRect.height != 0) {
                        // ReSharper disable once CppUseStructuredBinding
                        ImageDraw(&outputImage, spritesheetImage,
                            Rectangle{.x = frame.x, .y = frame.y, .width = frame.width, .height = frame.height},
                            {.x = packedRect.x, .y = packedRect.y, .width = packedRect.width, .height = packedRect.height},
                            WHITE);
                        packedFrames.push_back(frame);
                        croppedWidth = std::max(croppedWidth, static_cast<uint16_t>(packedRect.x + packedRect.width));
                        croppedHeight = std::max(croppedHeight, static_cast<uint16_t>(packedRect.y + packedRect.height));
                    }
                }
                ImageCrop(&outputImage, Rectangle{.x = 0, .y = 0, .width = static_cast<float>(croppedWidth), .height = static_cast<float>(croppedHeight)});
                ExportImage(outputImage, "output.png");
                UnloadImage(outputImage);
                //UnloadTexture(selectedSpritesheetPreview);
            }

            if (IsTextureValid(selectedSpritesheetPreview)) {
                if (GetMouseWheelMove() != 0) {
                    camPreview.target = screenCenter + GetMousePosition();
                }
                camPreview.zoom = Clamp(camPreview.zoom + GetMouseWheelMove() / 10.0f, 0.1f, 3.0f);
                if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
                    camPreview.target -= GetMouseDelta() / camPreview.zoom;
                }
                auto clipRect = Rectangle{
                    .x = static_cast<float>(x) + padding,
                    .y = static_cast<float>(y) + RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT + loadSpritesheet.height + padding * 2,
                    .width = popupWindowWidth - padding * 2,
                    .height = popupWindowHeight - RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT - loadSpritesheet.height - padding * 3
                };
                if (CheckCollisionPointRec(GetMousePosition(), clipRect)) {
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
                }
                BeginScissorMode(static_cast<int>(clipRect.x), static_cast<int>(clipRect.y), static_cast<int>(clipRect.width), static_cast<int>(clipRect.height));
                BeginMode2D(camPreview);
                DrawTexture(selectedSpritesheetPreview, x, y, WHITE);
                for (auto rect : rectsToDraw) {
                    DrawRectanglePro(rect, Vector2Zero(), 0.0f, ColorAlpha(BLUE, 0.1));
                }
                EndMode2D();
                DrawRectangleLinesEx(clipRect, static_cast<float>(GuiGetStyle(STATUSBAR, BORDER_WIDTH)), BLACK);
                EndScissorMode();
            }

            loadSpritesheet.x = x + padding * 3 + (static_cast<int>(popupWindowWidth) - x * 2) / 3;
            loadSpritesheet.y = static_cast<int>(position.y) + RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT + padding;
            loadSpritesheet.draw();

            loadXML.x = loadSpritesheet.x + static_cast<int>(loadSpritesheet.width) + padding;
            loadXML.y = loadSpritesheet.y;
            loadXML.draw();

            repack.x = loadXML.x + static_cast<int>(loadXML.width) + padding;
            repack.y = loadXML.y;
            repack.draw();
        }

        EndDrawing();
    }

    CloseWindow();
    NFD_Quit();

    return 0;
}
