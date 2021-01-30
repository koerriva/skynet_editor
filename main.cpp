#include <iostream>
#include <raylib.h>
#include <mycolor.h>
#include <raymath.h>
//#include <glm/glm.hpp>
#include <spdlog/spdlog.h>
#include <mygame.h>

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

    Neural* neurals = (Neural*)MemAlloc(sizeof(Neural)*10000);
    int neuralCount = 0;
    Texture2D neural_dark_texture = LoadTexture("data/neural_dark.png");
    Texture2D neural_light_texture =LoadTexture("data/neural_light.png");

    Vector2 lastMousePos = {0};
    Vector2 deltaMousePos;
    Vector2 curentMousePos;
    Vector2 worldMousePos;

    PlayerAction action = PlayerAction::Idle;
    
    while(!WindowShouldClose()){
        {
            curentMousePos = GetMousePosition();
            deltaMousePos = Vector2Subtract(curentMousePos,lastMousePos);
            lastMousePos = curentMousePos;
            worldMousePos = GetScreenToWorld2D(curentMousePos,camera);
            camera.zoom +=GetMouseWheelMove();
            camera.zoom = Clamp(camera.zoom,1,5);
        }

        {
            action = PlayerAction::Idle;
        }
        
        if(IsMouseButtonDown(MOUSE_LEFT_BUTTON)&&Vector2Length(deltaMousePos)>1.0){
            action = PlayerAction::MoveScene;
        }
        if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
            action = PlayerAction::AddNode;
        }

        if(action==PlayerAction::MoveScene){
            float t = GetFrameTime();
            float f = 40/camera.zoom;
            camera.target.x -= deltaMousePos.x*f*t;
            camera.target.y -= deltaMousePos.y*f*t;
        }

        if(action==PlayerAction::AddNode){
            neurals[neuralCount++]=Neural{worldMousePos,WHITE,false};
        }

        BeginDrawing();
        ClearBackground(SHENHAILV);
        BeginMode2D(camera);

        int index=0;
        for (size_t i = 0; i < neuralCount; i++)
        {
            Neural* n = &neurals[i];
            if(IsInside(n->center,worldMousePos)){
                DrawTextureV(neural_light_texture,Vector2SubtractValue(n->center,radius),n->color);
            }else{
                DrawTextureV(neural_dark_texture,Vector2SubtractValue(n->center,radius),n->color);
            }
        }

        EndMode2D();
        DrawFPS(5,5);
        DrawText(TextFormat("Mouse Y %2.f",GetMouseWheelMove()),5,20,16,DARKBROWN);
        DrawText(TextFormat("Mouse Zoom %2.f",camera.zoom),5,40,16,DARKBROWN);
        DrawText(TextFormat("World Pos %2.f,%2.f",worldMousePos.x,worldMousePos.y),5,60,16,DARKBROWN);
        if(action==PlayerAction::MoveScene){
            DrawText("Move Scene",5,80,16,WHITE);
        }
        if(action==PlayerAction::AddNode){
            DrawText("Add Node",5,80,16,WHITE);
        }

        EndDrawing();
    }
    CloseWindow();
    return 0;
}
