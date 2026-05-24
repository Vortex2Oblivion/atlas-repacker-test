cmake_minimum_required(VERSION 3.10.0)
project(atlasrepackertest VERSION 0.1.0 LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 20)


add_executable(atlasrepackertest 
    src/Main.cpp
    src/Exporter.cpp
    src/Exporter.hpp
    src/Frame.hpp
    src/Windows.cpp
    src/Button.hpp
    src/Button.cpp
        src/SpritesheetLoadMenu.cpp
        src/SpritesheetLoadMenu.hpp
        src/FileUtil.cpp
        src/FileUtil.hpp
)

if(WIN32)
target_link_libraries(${PROJECT_NAME} dwmapi)
endif()

add_subdirectory(external/raylib)
target_link_libraries(${PROJECT_NAME} raylib)

include_directories(external/raygui/src)

add_subdirectory(external/pugixml)
target_link_libraries(${PROJECT_NAME} pugixml)

add_subdirectory(external/json)
target_link_libraries(${PROJECT_NAME} nlohmann_json)

add_subdirectory(external/nativefiledialog-extended)
target_link_libraries(${PROJECT_NAME} nfd)

add_subdirectory(external/RectangleBinPack)
target_link_libraries(${PROJECT_NAME} RectangleBinPack)