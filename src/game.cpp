//
// Created by 50409 on 2021/2/8.
//
#include "mygame.h"

void MyGame::CloseNeuralMenu(Neural *neural) {
    if(editNode==neural){
        editNode = nullptr;
        editType = 0;
        action = PlayerAction::Idle;
    }
}
