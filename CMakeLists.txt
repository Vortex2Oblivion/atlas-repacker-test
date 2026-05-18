cmake_minimum_required(VERSION 3.10.0)
project(atlasrepackertest VERSION 0.1.0 LANGUAGES C CXX)

add_executable(atlasrepackertest 
    src/Main.cpp 
    src/packers/Packer.cpp 
    src/packers/MaxRectsBin.cpp)

add_subdirectory(external/raylib)
target_link_libraries(${PROJECT_NAME} raylib)

add_subdirectory(external/pugixml)
target_link_libraries(${PROJECT_NAME} pugixml)