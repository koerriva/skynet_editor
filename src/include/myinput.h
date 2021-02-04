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
};

struct InputManager{
    MouseState mouse;
    void Update(Camera2D camera,float deltaTime){
        Vector2 mouseScreenPos = GetMousePosition();
        mouse.screen_delta_pos = Vector2Subtract(mouseScreenPos,mouse.screen_pos);
        mouse.screen_pos = mouseScreenPos;

        Vector2 mouseWorldPos = GetScreenToWorld2D(mouseScreenPos,camera);
        mouse.world_delta_pos = Vector2Subtract(mouseWorldPos,mouse.world_pos);
        mouse.world_pos = mouseWorldPos;
    };
};

#endif //SKYNET_EDITOR_MYINPUT_H
