#include <iostream>
#include <raylib.h>
#include <mycolor.h>
#include <raymath.h>
#include <glm/glm.hpp>
#include <spdlog/spdlog.h>
//#include <lua.hpp>
//#include <entt/entt.hpp>
#include <sol.hpp>
#include <cassert>

#include <mygame.h>
#include <myinput.h>
//using namespace glm;
//using namespace spdlog;

bool IsInside(float r,Vector2 center, Vector2 pos){
    float x = pos.x - center.x;
    float y = pos.y - center.y;
    return r*r>x*x+y*y;
}

bool IsInsideInner(float r,Vector2 center, Vector2 pos){
    float x = pos.x - center.x;
    float y = pos.y - center.y;
    return (r-5)*(r-5)>x*x+y*y;
}

bool IsInsideEdge(float r,Vector2 center, Vector2 pos){
    float x = pos.x - center.x;
    float y = pos.y - center.y;
    float a = x*x+y*y;
    return r*r>a&&a>=(r-5)*(r-5);
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
    lua.new_usertype<Vector2>("vec2","x",&Vector2::x,"y",&Vector2::y);
    lua.new_usertype<Color>("color","r",&Color::r,"g",&Color::g,"b",&Color::b,"a",&Color::a);
    lua.set_function("GetMousePos",GetMousePosition);
    lua.set_function("GetDeltaTime",GetFrameTime);
    lua.script_file("data/script/id.lua");
    lua.script_file("data/script/tiny.lua");
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

    Texture2D neural_texture = LoadTexture("data/neural.png");

    InputManager im={};

    PlayerAction action = PlayerAction::Idle;
    CursorState cursorState = CursorState::OnGround;
    Neural* selected = nullptr;
    Neural* picked = nullptr;
    NeuralLink curNeuralLink = {};
    curNeuralLink.Init();

    NeuralNetwork nn{};
    nn.Init();
    nn.neurals = (Neural*)MemAlloc(sizeof(Neural)*MAX_NEURAL_SIZE);
    nn.links =(NeuralLink*)MemAlloc(sizeof(NeuralLink)*MAX_NEURAL_LINK_SIZE);

    while(!WindowShouldClose()){
        {
            long t = GetFileModTime("data/script/a.lua");
            if(script_last_modify_time!=t){
                TraceLog(LOG_INFO,"modify lua script");
                auto result =lua.safe_script_file("data/script/a.lua");
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
//        update();
        {
            im.Update(camera,GetFrameTime());
            camera.zoom +=GetMouseWheelMove();
            camera.zoom = Clamp(camera.zoom,1,5);
        }

        {
            for (size_t i = 0; i < nn.neural_count; i++)
            {
                Neural* pN = &nn.neurals[i];
                if(IsInside(pN->radius,pN->center,im.mouse.world_pos)){
                    cursorState = CursorState::InNode;
                    selected = pN;
                    break;
                }
                cursorState = OnGround;
                selected = nullptr;
            }
        }

        {
            if(IsMouseButtonReleased(MOUSE_LEFT_BUTTON)){
                action = PlayerAction::Idle;
                picked = nullptr;

                curNeuralLink.Init();
            }

            if(im.mouse.LB_PRESS){
                if(cursorState==InNode){
                    if(IsInsideInner(selected->radius,selected->center,im.mouse.world_pos)){
                        picked = selected;
                        action = PlayerAction::MoveNode;
                    }else{
                        action = PlayerAction::LinkNode;
                    }
                } else{
                    action = PlayerAction::MoveScene;
                }
            }

            if(im.mouse.RB_PRESS){
                action = PlayerAction::AddNode;
            }
        }

        if(action==PlayerAction::MoveScene){
            camera.target = Vector2Subtract(camera.target,im.mouse.screen_delta_pos);
        }

        if(action==PlayerAction::MoveNode){
            if(picked){
                picked->center = Vector2Add(picked->center,im.mouse.world_delta_pos);
            }
        }

        if(action==PlayerAction::AddNode){
            int batchSize = 1000;
            if(MAX_NEURAL_SIZE-nn.neural_count<1000){
                batchSize = MAX_NEURAL_SIZE-nn.neural_count;
            }
            for (int i = 0; i < batchSize; ++i) {
                Neural neural{};
                neural.center = {static_cast<float>(GetRandomValue(-5000,5000)),static_cast<float>(GetRandomValue(-5000,5000))};
                neural.color = DARKGREEN;
                neural.isActive = false;
                neural.radius = radius;
                nn.AddNeural(neural);
            }
        }

        if(action==PlayerAction::LinkNode){
            if(curNeuralLink.state==BEGIN){
                if(selected != nullptr && curNeuralLink.form!=selected){
                    TraceLog(LOG_INFO,"BEGIN");
                    curNeuralLink.to = selected;
                    curNeuralLink.state = NeuralLinkState::END;
                    nn.AddLink(curNeuralLink);
                }
            }
            if(curNeuralLink.state==UNLINK){
                TraceLog(LOG_INFO,"UNLINK");
                curNeuralLink.form = selected;
                curNeuralLink.isActive = false;
                curNeuralLink.state = NeuralLinkState::BEGIN;
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

        {
//            sol::table neural_pool = lua["neural"];
//            for (auto& n:neural_pool) {
//                int id = n.first.as<int>();
//                sol::table val = n.second;
//                bool isActive = val["isActive"];
//                Vector2 pos = val["pos"].get<Vector2>();
//                Color color = val["color"].get<Color>();
//                float r = val["radius"].get<float>();
//
//                DrawTextureEx(neural_texture,Vector2SubtractValue(pos, radius),0,1.0f,color);
//            }
        }

        if(action==PlayerAction::LinkNode){
            auto* pIn = curNeuralLink.form;
            DrawLineBezier(pIn->center,im.mouse.world_pos,1.0,GRAY);
        }

        for (size_t i = 0; i < nn.link_count; ++i) {
            NeuralLink* link = &nn.links[i];
            link->DeActive();
            int s = GetRandomValue(0,nn.link_count-1);
            if(s==i){
                link->Active();
            }
            DrawLineBezier(link->form->center,link->to->center,2.0,link->color);
        }

        for (size_t i = 0; i < nn.neural_count; i++)
        {
            Neural* p = &nn.neurals[i];
            DrawTextureV(neural_texture, Vector2SubtractValue(p->center, radius), p->color);
            if(selected==p){
                if(IsInsideEdge(p->radius,p->center,im.mouse.world_pos)){
                    DrawRing(p->center,p->radius-5,p->radius,0,360,24,GRAY);
                }else{
                    DrawRing(p->center,p->radius-2,p->radius,0,360,24,GREEN);
                }
            }
        }

        EndMode2D();
        DrawFPS(5,5);
        DrawText(TextFormat("Mouse Y %2.f",GetMouseWheelMove()),5,20,16,DARKBROWN);
        DrawText(TextFormat("Mouse Zoom %2.f",camera.zoom),5,40,16,DARKBROWN);
        DrawText(TextFormat("World Pos %2.f,%2.f",im.mouse.world_pos.x,im.mouse.world_pos.y),5,60,16,DARKBROWN);
        DrawText(TextFormat("Neural Size %d",nn.neural_count),5,80,16,DARKBROWN);
        if(action==PlayerAction::MoveScene){
            DrawText("Move Scene",5,100,16,WHITE);
        }
        if(action==PlayerAction::MoveNode){
            DrawText("Move Node",5,100,16,WHITE);
        }
        if(action==PlayerAction::AddNode){
            DrawText("Add Node",5,100,16,WHITE);
        }
        if(action==PlayerAction::LinkNode){
            DrawText("Link Node",5,100,16,WHITE);
        }

        if(action==AddNode){
            action = PlayerAction::Idle;
        }
        if(action==LinkNode&&curNeuralLink.state==END){
            action = PlayerAction::Idle;
        }

        EndDrawing();

        im.Clean();
    }

    UnloadTexture(neural_texture);
    MemFree(nn.neurals);
    MemFree(nn.links);

    CloseWindow();
    return 0;
}
