#include <iostream>
#include <raylib.h>
#include <mycolor.h>
#include <raymath.h>
//#include <glm/glm.hpp>
#include <spdlog/spdlog.h>

//using namespace glm;
using namespace spdlog;

bool IsInside(Vector2 center, Vector2 pos){
    float x = pos.x - center.x;
    float y = pos.y - center.y;
    return 100.f>x*x+y*y;
}

int width = 1280,height=720;

#ifndef __APPLE__
// N卡使用独显运行
extern "C" __declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;

// A显卡使用独显运行
//extern "C" __declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 0x00000001;
#endif

int main() {
//    SetConfigFlags(FLAG_VSYNC_HINT | FLAG_MSAA_4X_HINT | FLAG_WINDOW_HIGHDPI);
    SetConfigFlags(FLAG_VSYNC_HINT|FLAG_WINDOW_HIGHDPI);
    InitWindow(width,height,"Skynet Editor");
    SetWindowPosition(100,100);
    SetTargetFPS(60);
    Camera2D camera;
    camera.zoom = 1.0f;
    camera.target = {0.f,0.f};
    camera.offset = {width/2.f,height/2.f};
    float radius = 10.0f;
    int row = width/20,col = height/20;
    Vector2 entities[row*col];
    for (int i = 0; i < row; ++i) {
        for (int j = 0; j < col; ++j) {
            entities[i*col+j].x = i*radius*2+radius;
            entities[i*col+j].y = j*radius*2+radius;
        }
    }
    Vector2 lastMousePos = {0};
    Vector2 deltaMousePos = {0};
    while(!WindowShouldClose()){
        auto  mPos = GetMousePosition();
        deltaMousePos = Vector2Subtract(mPos,lastMousePos);
        lastMousePos = mPos;
        auto wPos = GetScreenToWorld2D(mPos,camera);

        BeginDrawing();
        ClearBackground(SHENHAILV);
        BeginMode2D(camera);

        int index=0;
        for (auto center : entities) {
            Color color;
            if(IsInside(center,wPos)){
                color = GREEN;
            }else{
                color = DARKGREEN;
            }

            DrawCircleV(center,radius,color);
            DrawText(TextFormat("%d",index),center.x,center.y,9,WHITE);
            index++;
        }
        EndMode2D();
        DrawFPS(5,5);
        DrawText(TextFormat("Mouse Y %2.f",GetMouseWheelMove()),5,20,16,DARKBROWN);
        camera.zoom +=GetMouseWheelMove();
        camera.zoom = Clamp(camera.zoom,1,5);
        DrawText(TextFormat("Mouse Zoom %2.f",camera.zoom),5,40,16,DARKBROWN);
        DrawText(TextFormat("World Pos %2.f,%2.f",wPos.x,wPos.y),5,60,16,DARKBROWN);

        if(IsMouseButtonDown(MOUSE_LEFT_BUTTON)){
            camera.offset = Vector2Add(Vector2Scale(deltaMousePos,1.f),camera.offset);
            DrawText("Move Scene",5,80,16,WHITE);
        }

        EndDrawing();
    }
    CloseWindow();
    return 0;
}
