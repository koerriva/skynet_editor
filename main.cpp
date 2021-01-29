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

#ifdef _WIN32
// N卡使用独显运行
extern "C" __declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
#endif

#ifndef __APPLE__
// A显卡使用独显运行
extern "C" __declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 0x00000001;
#endif

int main() {
//    SetConfigFlags(FLAG_VSYNC_HINT | FLAG_MSAA_4X_HINT | FLAG_WINDOW_HIGHDPI);
    SetConfigFlags(FLAG_WINDOW_HIGHDPI);
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
    Vector2 deltaMousePos;

    Image mask = GenImageColor(20,20,WHITE);
    Image neural_dark_img = GenImageColor(20, 20,WHITE);
    ImageDrawCircleV(&neural_dark_img,{10,10},10,DARKGREEN);

//    ImageAlphaMask(&neural_dark_img,mask);
    Image neural_light_img = GenImageColor(20,20,WHITE);
    ImageDrawCircleV(&neural_light_img,{10,10},10,GREEN);
//    ImageAlphaMask(&neural_light_img,mask);

    Texture2D neural_dark_texture = LoadTextureFromImage(neural_dark_img);
    Texture2D neural_light_texture = LoadTextureFromImage(neural_light_img);

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
            Color color = SHENHAILV;
            if(IsInside(center,wPos)){
//                color = GREEN;
                DrawTextureV(neural_light_texture,Vector2SubtractValue(center,radius),color);
            }else{
//                color = DARKGREEN;
                DrawTextureV(neural_dark_texture,Vector2SubtractValue(center,radius),color);
            }

//            DrawCircleV(center,radius,RED);

//            DrawText(TextFormat("%d",index),center.x,center.y,9,WHITE);
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
            float t = GetFrameTime();
            float f = 40/camera.zoom;
            camera.target.x -= deltaMousePos.x*f*t;
            camera.target.y -= deltaMousePos.y*f*t;
            DrawText("Move Scene",5,80,16,WHITE);
        }

        EndDrawing();
    }
    CloseWindow();
    return 0;
}
