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

void MyGame::DrawDebugText(const char* text,Vector2 screen_pos){
    DrawTextEx(font,text,screen_pos,font.baseSize,1.0,GREEN);
}

void MyGame::ShowToolBar() {
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