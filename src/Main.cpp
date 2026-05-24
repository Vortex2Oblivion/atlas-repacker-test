#include <iostream>
#include <vector>

#include "external/glad.h"
#include "nfd.hpp"
#include "pugixml.hpp"
#include "raylib.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#include "Button.hpp"
#include "Frame.hpp"
#include "SpritesheetLoadMenu.hpp"

#include "Windows.hpp"

int main() {
    NFD_Init();
    InitWindow(1280, 720, "raylib example - basic window");
    setDarkMode();
    
    SetTargetFPS(GetMonitorRefreshRate(GetCurrentMonitor()));

    GLint maxTextureSize = 0;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);

    constexpr auto popupWindowWidth = 650.0f;
    constexpr auto popupWindowHeight = 480.0f;

    auto loadFiles = Button(10, 10, 48, 48, "", "Load Files", BLACK);
    loadFiles.icon = ICON_FILE_OPEN;

    auto spritesheetLoadMenu = SpritesheetLoadMenu(0, 0, popupWindowWidth, popupWindowHeight);
    spritesheetLoadMenu.screenCenter();

    bool inSheetSelect = false;

    std::vector<Frame> frames = {};

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(WHITE);

        loadFiles.draw();

        if (loadFiles.pressed) {
            inSheetSelect = true;
        }

        if (inSheetSelect) {
            spritesheetLoadMenu.draw();
        }

        EndDrawing();
    }

    CloseWindow();
    NFD_Quit();

    return 0;
}
