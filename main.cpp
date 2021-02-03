#include <iostream>
#include <raylib.h>
#include <mycolor.h>
#include <raymath.h>
#include <glm/glm.hpp>
#include <spdlog/spdlog.h>
#include <mygame.h>
//#include <lua.hpp>

#include <sol.hpp>
#include <cassert>

//using namespace glm;
//using namespace spdlog;

bool IsInside(Vector2 center, Vector2 pos){
    float x = pos.x - center.x;
    float y = pos.y - center.y;
    return 32*32>x*x+y*y;
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
    long script_last_modify_time = GetFileModTime("data/script/a.lua");

    sol::function update = lua["update"];

    SetConfigFlags(FLAG_VSYNC_HINT | FLAG_MSAA_4X_HINT | FLAG_WINDOW_HIGHDPI);
    InitWindow(width,height,"Skynet Editor");
    SetWindowPosition(100,100);
    SetTargetFPS(60);
    Camera2D camera;
    camera.zoom = 1.0f;
    camera.target = {0};
    camera.offset = {(float)width/2.f,(float)height/2.f};
    camera.rotation = {0};
    float radius = 32.0f;

//    Neural* pNeural = (Neural*)malloc(sizeof(Neural) * MAX_NEURAL_SIZE);
    auto* pNeural = (Neural*)MemAlloc(sizeof(Neural) * MAX_NEURAL_SIZE);
    int neuralCount = 0;

    Texture2D neural_texture = LoadTexture("data/neural.png");

    Vector2 lastMousePos = {0};
    Vector2 deltaMousePos;
    Vector2 curentMousePos;
    Vector2 worldMousePos;

    PlayerAction action = PlayerAction::Idle;
    CursorState cursorState = CursorState::OnGround;
    Neural* selected = nullptr;

    auto* neuralLinks = (NeuralLink*)MemAlloc(sizeof(NeuralLink) * MAX_NEURAL_LINK_SIZE);
    int neuralLinkCount=0;
    NeuralLink neuralLink = {{},GRAY, false, false,{},0,{},0};
    NeuralLinkState neuralLinkState = UNLINK;

    bool isRMBTriggerOnce = false;
    while(!WindowShouldClose()){
        {
            long t = GetFileModTime("data/script/a.lua");
            if(script_last_modify_time!=t){
                TraceLog(LOG_INFO,"modify lua script");
                auto result =lua.safe_script_file("data/script/a.lua",sol::script_pass_on_error);
                if(result.valid()){
                    update = lua["update"];
                    script_last_modify_time = t;
                }else{
                    sol::error err = result;
                    std::cerr << "The code has failed to run!\n" << err.what() << "\nPanicking and exiting..." << std::endl;
                    TraceLog(LOG_ERROR,err.what());
                }
            }
        }
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
                Neural* n = &pNeural[i];
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
            if(IsMouseButtonDown(MOUSE_LEFT_BUTTON)){
                if(cursorState==InNode){
                    if(IsKeyDown(KEY_LEFT_SHIFT)){
                        action = PlayerAction::LinkNode;
                    }else{
                        action = PlayerAction::MoveNode;
                    }
                }
            }
            if(IsMouseButtonReleased(MOUSE_LEFT_BUTTON)){
                action = PlayerAction::Idle;
                neuralLinkState = NeuralLinkState::UNLINK;
                neuralLink.isFinish = false;
            }

            if(IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)){
                isRMBTriggerOnce = true;
            }
            if(isRMBTriggerOnce){
                action = PlayerAction::AddNode;
                isRMBTriggerOnce = false;
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

        if(action==PlayerAction::MoveNode){
            if(selected){
                selected->center = Vector2Add(selected->center,deltaMousePos);
            }
        }

        if(action==PlayerAction::AddNode){
            pNeural[neuralCount].center = worldMousePos;
            pNeural[neuralCount].color = DARKGREEN;
            pNeural[neuralCount].isActive = false;
            neuralCount++;
            action = PlayerAction::Idle;
        }

        if(action==PlayerAction::LinkNode){
            if(neuralLinkState==BEGIN){
                auto* pLast = neuralLink.in[neuralLink.in_synapse_count-1];
                if(selected != nullptr && pLast!=selected){
                    TraceLog(LOG_INFO,"BEGIN");
                    LinkOut(&neuralLink,selected);
                    neuralLink.isFinish = true;
                    neuralLinkState = NeuralLinkState::END;
                    neuralLinks[neuralLinkCount++] = neuralLink;
                }
            }
            if(neuralLinkState==UNLINK){
                TraceLog(LOG_INFO,"UNLINK");
                LinkIn(&neuralLink,selected);
                neuralLink.isFinish = false;
                neuralLink.isActive = false;
                neuralLinkState = NeuralLinkState::BEGIN;
            }
        }

        BeginDrawing();
        ClearBackground(SHENHAILV);
        BeginMode2D(camera);
        {
            //draw grid
//            for (int i = 0; i < width / 20; ++i) {
//                DrawLine(i*10+camera.target.x-width/2,0+camera.target.y-height/2
//                         ,i*10+camera.target.x-width/2,height+camera.target.y-height/2
//                         ,DARKGRAY);
//            }
//            for (int i = 0; i < height/20; ++i) {
//                DrawLine(0+camera.target.x-width/2,i*20+camera.target.y-height/2
//                         ,width+camera.target.x-width/2,i*20+camera.target.y-height/2
//                         ,DARKGRAY);
//            }
        }

        if(action==PlayerAction::LinkNode){
            auto* pIn = neuralLink.in[neuralLink.in_synapse_count-1];
            DrawLineBezier(pIn->center,worldMousePos,1.0,GRAY);
        }

        for (size_t i = 0; i < neuralLinkCount; ++i) {
            NeuralLink* link = &neuralLinks[i];
            DeActiveNeuralLink(link);
            int s = GetRandomValue(0,neuralLinkCount-1);
            if(s==i){
                ActiveNeuralLink(link);
            }
            auto* pIn = link->in[link->in_synapse_count-1];
            auto* pOut = link->out[link->out_synapse_count-1];
            DrawLineBezier(pIn->center,pOut->center,2.0,link->color);
        }

        for (size_t i = 0; i < neuralCount; i++)
        {
            Neural* p = &pNeural[i];
            float scale = 1.0f;
//            DrawTextureV(neural_texture, Vector2SubtractValue(p->center, radius), p->color);
            if(selected==p){
                scale = 1.1f;
            }
            DrawTextureEx(neural_texture,Vector2SubtractValue(p->center, radius),0,scale,p->color);
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

    UnloadTexture(neural_texture);
    MemFree(pNeural);
    MemFree(neuralLinks);

//    free(pNeural);
//    free(neuralLinks);

    CloseWindow();
    return 0;
}
