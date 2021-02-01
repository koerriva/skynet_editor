#include <iostream>
#include <raylib.h>
#include <mycolor.h>
#include <raymath.h>
//#include <glm/glm.hpp>
//#include <spdlog/spdlog.h>
#include <mygame.h>
//#include <lua.hpp>

#include <sol.hpp>
#include <cassert>

//using namespace glm;
//using namespace spdlog;

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

//static int test_hello(lua_State* L){
//    printf("test hell0");
//    return 0;
//}
//static const struct luaL_Reg gameLib[] = {
//        {"test_hello",test_hello},
//        {NULL,NULL}
//};
//
//extern "C" __declspec(dllexport)
//int luaopen_gamelib(lua_State* L){
//    luaL_register(L,"gamelib",gameLib);
//    return 1;
//}

int main() {
#ifdef _WIN32
    system("chcp 65001");
#endif
//    lua_State* luaState = luaL_newstate();
//    luaL_openlibs(luaState);
//    luaL_dofile(luaState,"data/script/a.lua");
//    lua_close(luaState);

    sol::state lua;
    lua.open_libraries();
    lua.new_usertype<Vector2>("Vector2","x",&Vector2::x,"y",&Vector2::y);
    lua.set_function("GetMousePos",GetMousePosition);
    lua.set_function("GetDeltaTime",GetFrameTime);
    lua.script_file("data/script/a.lua");

    sol::function update = lua["update"];

//    SetConfigFlags(FLAG_VSYNC_HINT | FLAG_MSAA_4X_HINT | FLAG_WINDOW_HIGHDPI);
    SetConfigFlags(FLAG_WINDOW_HIGHDPI);
    InitWindow(width,height,"Skynet Editor");
    SetWindowPosition(100,100);
    SetTargetFPS(60);
    Camera2D camera;
    camera.zoom = 1.0f;
    camera.target = {0};
    camera.offset = {width/2.f,height/2.f};
    camera.rotation = {0};
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
    CursorState cursorState = CursorState::OnGround;
    Neural* selected = nullptr;

    NeuralLink* neuralLinks = (NeuralLink*)MemAlloc(sizeof(NeuralLink)*10000*10000);
    int neuralLinkCount=0;
    NeuralLink neuralLink;
    NeuralLinkState neuralLinkState = UNLINK;
    while(!WindowShouldClose()){
        update();
        {
            curentMousePos = GetMousePosition();
            deltaMousePos = Vector2Subtract(curentMousePos,lastMousePos);
            lastMousePos = curentMousePos;
            worldMousePos = GetScreenToWorld2D(curentMousePos,camera);
            camera.zoom +=GetMouseWheelMove();
            camera.zoom = Clamp(camera.zoom,1,5);
        }

        {
            selected = nullptr;
            
            for (size_t i = 0; i < neuralCount; i++)
            {
                Neural* n = &neurals[i];
                if(IsInside(n->center,worldMousePos)){
                    cursorState = CursorState::InNode;
                    selected = n;
                    break;
                }
            }
            if(selected==nullptr){
                cursorState = OnGround;
            }
        }

        {
            // if(IsMouseButtonDown(MOUSE_LEFT_BUTTON)&&Vector2Length(deltaMousePos)>1.0
            //     &&cursorState==OnGround){
            //     action = PlayerAction::MoveScene;
            // }
            if(IsMouseButtonDown(MOUSE_LEFT_BUTTON)){
                if(cursorState==InNode){
//                    if(action==PlayerAction::LinkNode){
//                        action=PlayerAction::Idle;
//                    }
                    if(action!=PlayerAction::LinkNode){
                        action = PlayerAction::LinkNode;
                        neuralLink.start = selected->center;
                        neuralLink.isFinish = false;
                        neuralLinkState = NeuralLinkState::BEGIN;
                    }else{
                        neuralLink.end = selected->center;
                        neuralLink.isFinish = true;
                        neuralLinkState = NeuralLinkState::END;
                        neuralLinks[neuralLinkCount++] = neuralLink;
                    }
                }
            }
            if(IsMouseButtonReleased(MOUSE_LEFT_BUTTON)){
                action = PlayerAction::Idle;
                neuralLinkState = NeuralLinkState::UNLINK;
            }
//            if(IsMouseButtonUp(MOUSE_LEFT_BUTTON)){
//                action = PlayerAction::Idle;
//            }

            if(IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)){
                action = PlayerAction::AddNode;
            }
            if(IsMouseButtonReleased(MOUSE_RIGHT_BUTTON)){
                action = PlayerAction::Idle;
            }
            if(IsMouseButtonPressed(MOUSE_MIDDLE_BUTTON)){
                action = PlayerAction::MoveScene;
            }
            if(IsMouseButtonReleased(MOUSE_MIDDLE_BUTTON)){
                action = PlayerAction::Idle;
            }
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

        if(action==PlayerAction::LinkNode){
            DrawLineBezier(neuralLink.start,worldMousePos,2.0,GRAY);
        }

        for (size_t i = 0; i < neuralCount; i++)
        {
            Neural* n = &neurals[i];
            DrawTextureV(neural_dark_texture,Vector2SubtractValue(n->center,radius),n->color);
            if(selected==n){
//                DrawCircleV(n->center,radius+1,RED);
                DrawCircleLines(n->center.x,n->center.y,radius,RED);
            }
        }

        for (size_t i = 0; i < neuralLinkCount; ++i) {
            NeuralLink* link = &neuralLinks[i];
            DrawLineBezier(link->start,link->end,2.0,GRAY);
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
        if(action==PlayerAction::LinkNode){
            DrawText("Link Node",5,80,16,WHITE);
        }

        EndDrawing();
    }

    UnloadTexture(neural_dark_texture);
    UnloadTexture(neural_light_texture);
    MemFree(neurals);
    MemFree(neuralLinks);

    CloseWindow();
    return 0;
}
