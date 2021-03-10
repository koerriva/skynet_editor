//
// Created by 50409 on 2021/2/8.
//
#include <mycolor.h>
#include "mygame.h"

void MyGame::Render() {
    BeginDrawing();
    ClearBackground(SHENHAILV);
    BeginMode2D(camera);
    DrawGrid();

    //连接线渲染
    if(action==PlayerAction::LinkNode){
        auto* pIn = linkFrom;
        DrawLineBezier(pIn->center,im.mouse.world_pos,1.0,GRAY);
    }

    //随机模拟
    for (int i = 0; i < nn.input_count; ++i) {
        Neural* p = nn.inputs[i];
        p->Active();
        for (int j = 0; j < p->out_count; ++j) {
            Neural* to = p->out[j];
            if(p->isActive)to->Active();
            DrawLineBezier(p->center,to->center,2.0,p->isActive?WHITE:GRAY);
        }
    }

    //主渲染
    for (size_t i = 0; i < nn.neural_count; i++)
    {
        Neural* p = &nn.neurals[i];
        DrawTextureV(neural_texture, Vector2SubtractValue(p->center, radius), p->color);
    }

    //gui
    if(selected){
        if(IsInsideEdge(selected->radius,selected->center,im.mouse.world_pos)){
            DrawRing(selected->center,selected->radius-5,selected->radius,0,360,24,GRAY);
        }else{
            DrawRing(selected->center,selected->radius-2,selected->radius,0,360,24,GREEN);
        }
    }

    EndMode2D();
    DrawDebugInfo();
    DrawToolBar();
    EndDrawing();
}

void MyGame::Cleanup() const {
    UnloadTexture(neural_texture);
    UnloadFont(font);

    MemFree(nn.neurals);
}

//private
void MyGame::UpdatePlayerAction() {
    if(action!=PlayerAction::EditNode){
        if(IsMouseButtonReleased(MOUSE_LEFT_BUTTON)){
            action = PlayerAction::Idle;
            picked = nullptr;
            linkState = UNLINK;
            linkFrom = nullptr;
            linkTo = nullptr;
        }

        if(im.mouse.LB_PRESS){
            if(cursorState==InNode){
                if(IsInsideInner(selected->radius,selected->center,im.mouse.world_pos)){
                    picked = selected;
                    action = PlayerAction::MoveNode;
                }else{
                    action = PlayerAction::LinkNode;
                }
            }else{
                action = PlayerAction::MoveScene;
            }
        }

        if(im.mouse.RB_PRESS){
            if(cursorState==InNode){
                editNode = selected;
                action = PlayerAction::EditNode;
            }else{
                action = PlayerAction::AddNode;
            }
        }

        if(im.key.DEL_PRESS){
            action = PlayerAction::DelNode;
        }
    }

    if(action==PlayerAction::MoveScene){
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

            int sw = width;
            int sh = height;
            int ww = world_width;
            int wh = world_height;
            camera.target.x = Clamp(camera.target.x,-ww/2+sw/2,ww/2-sw/2);
            camera.target.y = Clamp(camera.target.y,-wh/2+sh/2,wh/2-sh/2);
        }
    }

    if(action==PlayerAction::MoveNode){
        if(picked){
            picked->center = Vector2Add(picked->center,im.mouse.world_delta_pos);
        }
    }

    if(action==PlayerAction::AddNode){
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

    if(action==PlayerAction::LinkNode){
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

    if(action==PlayerAction::DelNode){
        if(selected!= nullptr){
            nn.DelNeural(selected);
        }
        selected = nullptr;
    }
}