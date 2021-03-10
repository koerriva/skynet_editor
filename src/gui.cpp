//
// Created by 50409 on 2021/2/8.
//
#define RAYGUI_IMPLEMENTATION
#define RAYGUI_SUPPORT_ICONS
#include <raygui.h>
#include "mygame.h"

void MyGame::OpenNeuralMenu(Neural *neural,Vector2 screen_pos) {
    float start_x = screen_pos.x;
    float start_y = screen_pos.y;
    float w = 200;
    float h = 260;
    bool weightEditMode = false;

    GuiSetFont(font);
    bool windows = GuiWindowBox((Rectangle){ start_x, start_y, w, h },TextFormat("神经元 0x%X",neural));
    editType = GuiToggleGroup((Rectangle){ start_x+5, start_y+20+7, 62, 20 }, "行为;外观", editType);
    if(editType==0){
        GuiLabel((Rectangle){ start_x+5, start_y+20+43, 53, 25 }, "权重");
        if (GuiSpinner((Rectangle){ start_x+89, start_y+20+43, 104, 25 }, nullptr, &neural->weight, 0, 100, weightEditMode)){
            weightEditMode = !weightEditMode;
        }
        neural->isLearn = GuiCheckBox({start_x+5,start_y+20+43+45,15,15},"学习",neural->isLearn);
    }

    if(editType==1){
        auto r = GuiDropdownBox({start_x+5,start_y+20+43,128,25},"未激活颜色;激活颜色",&editActiveColorType, editActiveColorMode);
        if(r){
            editActiveColorMode = !editActiveColorMode;
        }
        int padding = 0;
        if(editActiveColorMode){
            padding=70;
        }
        neural->colors[editActiveColorType] = GuiColorPicker({start_x+5,start_y+20+43+25+padding,128,95},neural->colors[editActiveColorType]);
    }
    if(windows){
        CloseNeuralMenu(neural);
    }
}

void MyGame::DrawDebugText(const char* text,Vector2 screen_pos) const{
    DrawTextEx(font,text,screen_pos,font.baseSize,1.0,GREEN);
}

void MyGame::ShowToolBar() const {
    int id  = 131;
    int id_x = id%16;
    int id_y = id/16;
    GuiImageButtonEx({width/2,0,32,32},"",icons,{1+18*id_x,1+18*id_y,16,16});
    id  = 132;
    id_x = id%16;
    id_y = id/16;
    GuiImageButtonEx({width/2+32,0,32,32},"",icons,{1+18*id_x,1+18*id_y,16,16});
    id  = 133;
    id_x = id%16;
    id_y = id/16;
    GuiImageButtonEx({width/2+32+32,0,32,32},"",icons,{1+18*id_x,1+18*id_y,16,16});
}

void MyGame::CloseNeuralMenu(Neural *neural) {
    if(editNode==neural){
        editNode = nullptr;
        editType = 0;
        action = PlayerAction::Idle;
    }
}

void MyGame::DrawGrid() const {
    int start_x = -world_width/2;
    int start_y = -world_height/2;

    int end_x = world_width/2;
    int end_y = world_height/2;
    //draw grid
    int seg = 20;
    for (int i = 0; i < world_width / seg; ++i) {
        DrawLine(start_x+i*seg,start_y
                ,start_x+i*seg,end_y
                ,DARKGRAY);
    }
    for (int i = 0; i < world_width / (seg*5); ++i) {
        DrawLine(start_x+i*seg*5,start_y
                ,start_x+i*seg*5,end_y
                ,BLACK);
    }
    for (int i = 0; i < world_height/seg; ++i) {
        DrawLine(start_x,i*seg+start_y
                ,end_x,i*seg+start_y
                ,DARKGRAY);
    }
    for (int i = 0; i < world_height/(seg*5); ++i) {
        DrawLine(start_x,i*seg*5+start_y
                ,end_x,i*seg*5+start_y
                ,BLACK);
    }
}

void MyGame::DrawDebugInfo(){
    DrawFPS(5,5);
    DrawDebugText(TextFormat("Camera target %2.f,%2.f",camera.target.x,camera.target.y),{5,20});
    DrawDebugText(TextFormat("Mouse Zoom %2.f",camera.zoom),{5,40});
    DrawDebugText(TextFormat("Mouse World Pos %2.f,%2.f",im.mouse.world_pos.x,im.mouse.world_pos.y),{5,60});
    DrawDebugText(TextFormat("Neural Size %d",nn.neural_count),{5,80});
    DrawDebugText("无边落木萧萧下，不尽长江滚滚来。",{5,240});

    if(action==PlayerAction::EditNode){
        Vector2 screen_pos = GetWorldToScreen2D(editNode->center,camera);
        OpenNeuralMenu(editNode,screen_pos);
    }
    ShowToolBar();

    if(action==PlayerAction::MoveScene){
        DrawDebugText("Move Scene",{5,100});
    }
    if(action==PlayerAction::MoveNode){
        DrawDebugText("Move Node",{5,100});
    }
    if(action==PlayerAction::AddNode){
        DrawDebugText("Add Node",{5,100});
    }
    if(action==PlayerAction::LinkNode){
        DrawDebugText("Link Node",{5,100});
    }
    if(action==PlayerAction::DelNode){
        DrawDebugText("Del Node",{5,100});
    }

    if(action==AddNode){
        action = PlayerAction::Idle;
    }
    if(action==LinkNode&&linkState==END){
        action = PlayerAction::Idle;
    }
    if(action==DelNode){
        action = PlayerAction::Idle;
    }
}
