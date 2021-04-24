//
// Created by 50409 on 2021/2/4.
//

#ifndef SKYNET_EDITOR_MYINPUT_H
#define SKYNET_EDITOR_MYINPUT_H

#include <raylib.h>
#include <raymath.h>

struct MouseState{
    Vector2 screen_pos;
    Vector2 screen_delta_pos;
    Vector2 world_pos;
    Vector2 world_delta_pos;

    bool LB_PRESS;
    bool LB_REPEAT;
    float LB_REPEAT_TIME;
    bool MB_PRESS;
    bool RB_PRESS;
    bool RB_RELEASE;
    bool MB_SCROLL;
    float Y;

    void CleanState(){
        LB_PRESS = false;
        MB_PRESS = false;
        MB_SCROLL = false;
        Y = 0;
        RB_PRESS = false;
        RB_RELEASE = false;

        screen_delta_pos = {0};
        world_delta_pos = {0};
    };
};

struct KeyState{
    bool DEL_PRESS;

    void CleanState(){
        DEL_PRESS = false;
    }
};

struct InputManager{
    MouseState mouse;
    KeyState key;
    void Update(Camera2D camera,float deltaTime){
        Vector2 mouseScreenPos = GetMousePosition();
        mouse.screen_delta_pos = Vector2Subtract(mouseScreenPos,mouse.screen_pos);
        mouse.screen_pos = mouseScreenPos;

        Vector2 mouseWorldPos = GetScreenToWorld2D(mouseScreenPos,camera);
        mouse.world_delta_pos = Vector2Subtract(mouseWorldPos,mouse.world_pos);
        mouse.world_pos = mouseWorldPos;

        if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
            mouse.LB_PRESS = true;
        }

        if(IsMouseButtonDown(MOUSE_LEFT_BUTTON)){
            mouse.LB_REPEAT_TIME += deltaTime;
            if(mouse.LB_REPEAT_TIME>0.1){
                mouse.LB_REPEAT = true;
            }
        }

        if(IsMouseButtonReleased(MOUSE_LEFT_BUTTON)){
            mouse.LB_REPEAT_TIME=0.0;
            mouse.LB_REPEAT = false;
        }

        if(IsMouseButtonPressed(MOUSE_MIDDLE_BUTTON)){
            mouse.MB_PRESS = true;
        }

        if(IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)){
            mouse.RB_PRESS = true;
        }

        if(IsMouseButtonReleased(MOUSE_RIGHT_BUTTON)){
            mouse.RB_RELEASE = true;
        }

        mouse.Y = GetMouseWheelMove();
        if(mouse.Y!=0){
            mouse.MB_SCROLL = true;
        }

        if(IsKeyPressed(KEY_DELETE)){
            key.DEL_PRESS = true;
        }
    };

    void Clean(){
        mouse.CleanState();
        key.CleanState();
    }
};

#endif //SKYNET_EDITOR_MYINPUT_H