//
// Created by 50409 on 2021/3/13.
//

#include <raymath.h>
#include "VisionCell.h"

void VisionCell::init() {
    size = 16;
}

void VisionCell::update(float delta) {
    age += delta;
    if(last_check_time>=1.0){
        is_check_signal = true;
        last_check_time = 0;
    }else{
        is_check_signal = false;
    }
}

void VisionCell::render(Texture2D texture) {
    if(is_check_signal){
        color = RED;
    }else{
        color = {120,40,50,255};
    }
    DrawTextureV(texture, Vector2SubtractValue(pos, size), color);
}

void VisionCell::cleanup() {

}

void VisionCell::receive(CellSignal &signal) {

}

VisionCell::VisionCell(const Vector2 &pos, const Color &color) : Cell(pos, color) {}
