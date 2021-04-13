#include <iostream>
#include <raylib.h>
#include <mycolor.h>
#include <raymath.h>
#include <glm/glm.hpp>
#include <spdlog/spdlog.h>
//#include <lua.hpp>
//#include <entt/entt.hpp>
//#include <sol.hpp>
//#include <cassert>

#include <mygame.h>
#include <myinput.h>
//using namespace glm;
//using namespace spdlog;

#ifdef _WIN32
// N卡使用独显运行
extern "C" __declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
#endif

#ifndef __APPLE__
// A显卡使用独显运行
extern "C" __declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 0x00000001;
#endif

int main() {
#ifdef _WIN32
    system("chcp 65001");
#endif
    MyGame myGame = {};
    myGame.width = 1280;
    myGame.height = 720;
    myGame.world_width = 4000;

    SetConfigFlags(FLAG_VSYNC_HINT | FLAG_MSAA_4X_HINT | FLAG_WINDOW_HIGHDPI);
    InitWindow(myGame.width,myGame.height,"Skynet Editor");
    SetWindowPosition(100,100);
    SetTargetFPS(60);

    myGame.Init();
    while(!WindowShouldClose()){
        myGame.Input();
        myGame.Update();
        myGame.Render();
    }

    myGame.Cleanup();

    CloseWindow();
    return 0;
}
