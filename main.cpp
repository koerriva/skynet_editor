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

    Camera2D camera;
    camera.zoom = 1.0f;
    camera.target = {0};
    camera.offset = {(float)myGame.width/2,(float)myGame.height/2};
    camera.rotation = 0;
    float radius = 32.0f;

    myGame.icons = LoadTexture("data/icons.png");
    Texture2D neural_texture = LoadTexture("data/neural.png");

    InputManager im={};

    myGame.action = PlayerAction::Idle;

    CursorState cursorState = CursorState::OnGround;
    Neural* selected = nullptr;
    Neural* picked = nullptr;
    myGame.editNode = nullptr;
    Neural* linkFrom = nullptr;
    Neural* linkTo = nullptr;
    NeuralLinkState linkState = UNLINK;

    NeuralNetwork nn{};
    nn.Init();
    nn.neurals = (Neural*)MemAlloc(sizeof(Neural)*MAX_NEURAL_SIZE);

    while(!WindowShouldClose()){
//        {
//            long t = GetFileModTime("data/script/a.lua");
//            if(script_last_modify_time!=t){
//                TraceLog(LOG_INFO,"modify lua script");
//                auto result =lua.safe_script_file("data/script/a.lua");
//                if(result.valid()){
//                    update = lua["update"];
//                    script_last_modify_time = t;
//                }else{
//                    sol::error err = result;
//                    std::cerr << "The code has failed to run!\n" << err.what() << "\nPanicking and exiting..." << std::endl;
//                    TraceLog(LOG_ERROR,err.what());
//                }
//            }
//        }
//        update();
        if(myGame.state==LOAD_DATA){
            BeginDrawing();
            ClearBackground(SHENHAILV);
            DrawRectangle(0,0,myGame.width,100,RED);
            EndDrawing();
        }
        if(myGame.state==READY){
            BeginDrawing();
            ClearBackground(SHENHAILV);
            myGame.DrawDebugText("开始",{5,60});
            EndDrawing();
        }
        if(myGame.state==RUNNING){
            {
                camera.rotation = 0;
                im.Update(camera,GetFrameTime());
                if(im.mouse.MB_SCROLL){
                    camera.zoom += im.mouse.Y*GetFrameTime()*10;
                    camera.zoom = Clamp(camera.zoom,1,5);
                }
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
                if(myGame.action!=PlayerAction::EditNode){
                    if(IsMouseButtonReleased(MOUSE_LEFT_BUTTON)){
                        myGame.action = PlayerAction::Idle;
                        picked = nullptr;
                        linkState = UNLINK;
                        linkFrom = nullptr;
                        linkTo = nullptr;
                    }

                    if(im.mouse.LB_PRESS){
                        if(cursorState==InNode){
                            if(IsInsideInner(selected->radius,selected->center,im.mouse.world_pos)){
                                picked = selected;
                                myGame.action = PlayerAction::MoveNode;
                            }else{
                                myGame.action = PlayerAction::LinkNode;
                            }
                        }else{
                            myGame.action = PlayerAction::MoveScene;
                        }
                    }

                    if(im.mouse.RB_PRESS){
                        if(cursorState==InNode){
                            myGame.editNode = selected;
                            myGame.action = PlayerAction::EditNode;
                        }else{
                            myGame.action = PlayerAction::AddNode;
                        }
                    }
                }
            }

            if(myGame.action==PlayerAction::MoveScene){
                if(im.mouse.screen_delta_pos.x!=0||im.mouse.screen_delta_pos.y!=0){
                    if(im.mouse.screen_delta_pos.x>0){
                        im.mouse.world_delta_pos.x = abs(im.mouse.world_delta_pos.x);
                    }
                    if(im.mouse.screen_delta_pos.x<0){
                        im.mouse.world_delta_pos.x = -abs(im.mouse.world_delta_pos.x);
                    }

                    if(im.mouse.screen_delta_pos.y>0){
                        im.mouse.world_delta_pos.y = abs(im.mouse.world_delta_pos.y);
                    }
                    if(im.mouse.screen_delta_pos.y<0){
                        im.mouse.world_delta_pos.y = -abs(im.mouse.world_delta_pos.y);
                    }
                    camera.target = Vector2Subtract(camera.target,im.mouse.world_delta_pos);
                    int sw = myGame.width;
                    int sh = myGame.height;
                    int ww = myGame.world_width;
                    int wh = myGame.world_height;
                    camera.target.x = Clamp(camera.target.x,-ww/2+sw/2,ww/2-sw/2);
                    camera.target.y = Clamp(camera.target.y,-wh/2+sh/2,wh/2-sh/2);
                }
            }

            if(myGame.action==PlayerAction::MoveNode){
                if(picked){
                    picked->center = Vector2Add(picked->center,im.mouse.world_delta_pos);
                }
            }

            if(myGame.action==PlayerAction::AddNode){
                if(cursorState!=InNode){
                    Neural neural{};
                    float x = im.mouse.world_pos.x;
                    float y = im.mouse.world_pos.y;
                    neural.center = {x,y};
                    neural.isActive = false;
                    neural.radius = radius;
                    nn.AddNeural(neural);
                }
            }

            if(myGame.action==PlayerAction::LinkNode){
                if(linkState==BEGIN){
                    if(selected != nullptr && linkFrom!=selected){
                        TraceLog(LOG_INFO,"BEGIN");
                        linkTo = selected;
                        linkFrom->Link(linkTo);
                        linkState = NeuralLinkState::END;
                    }
                }
                if(linkState==UNLINK){
                    TraceLog(LOG_INFO,"UNLINK");
                    linkFrom = selected;
                    linkState = BEGIN;
                }
            }

            BeginDrawing();
            ClearBackground(SHENHAILV);
            BeginMode2D(camera);
            myGame.DrawGrid();
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

            if(myGame.action==PlayerAction::LinkNode){
                auto* pIn = linkFrom;
                DrawLineBezier(pIn->center,im.mouse.world_pos,1.0,GRAY);
            }

            int chosen = GetRandomValue(0,nn.neural_count+nn.neural_count*2);
            for (int i = 0; i < nn.neural_count; ++i) {
                Neural* p = &nn.neurals[i];
                if(chosen==i&&p->in_count==0&&p->out_count!=0){
                    p->Active();
                }
                for (int j = 0; j < p->out_count; ++j) {
                    Neural* to = p->out[j];
                    if(p->isActive)to->Active();
                    DrawLineBezier(p->center,to->center,2.0,p->isActive?WHITE:GRAY);
                }
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
            myGame.DrawDebugText(TextFormat("Camera target %2.f,%2.f",camera.target.x,camera.target.y),{5,20});
            myGame.DrawDebugText(TextFormat("Mouse Zoom %2.f",camera.zoom),{5,40});
            myGame.DrawDebugText(TextFormat("Mouse World Pos %2.f,%2.f",im.mouse.world_pos.x,im.mouse.world_pos.y),{5,60});
            myGame.DrawDebugText(TextFormat("Neural Size %d",nn.neural_count),{5,80});
            myGame.DrawDebugText("无边落木萧萧下，不尽长江滚滚来。",{5,240});

            {
                if(myGame.action==PlayerAction::EditNode){
                    Vector2 screen_pos = GetWorldToScreen2D(myGame.editNode->center,camera);
                    myGame.OpenNeuralMenu(myGame.editNode,screen_pos);
                }
                myGame.ShowToolBar();
            }

            if(myGame.action==PlayerAction::MoveScene){
                myGame.DrawDebugText("Move Scene",{5,100});
            }
            if(myGame.action==PlayerAction::MoveNode){
                myGame.DrawDebugText("Move Node",{5,100});
            }
            if(myGame.action==PlayerAction::AddNode){
                myGame.DrawDebugText("Add Node",{5,100});
            }
            if(myGame.action==PlayerAction::LinkNode){
                myGame.DrawDebugText("Link Node",{5,100});
            }

            if(myGame.action==AddNode){
                myGame.action = PlayerAction::Idle;
            }
            if(myGame.action==LinkNode&&linkState==END){
                myGame.action = PlayerAction::Idle;
            }

            EndDrawing();

            for (int i = 0; i < nn.neural_count; ++i) {
                nn.neurals[i].DeActive();
            }
            im.Clean();
        }
    }

    UnloadTexture(neural_texture);
    myGame.Cleanup();
    MemFree(nn.neurals);
    CloseWindow();
    return 0;
}