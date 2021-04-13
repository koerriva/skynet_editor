//
// Created by 50409 on 2021/3/13.
//

#ifndef SKYNET_EDITOR_CELL_H
#define SKYNET_EDITOR_CELL_H


#include <raylib.h>
#include <atomic>
#include "CellSignal.h"

class Cell {
protected:
    std::atomic<int> id = 0;
    float age = 0;
    float size = 0;
    Color color = GRAY;
    Vector2 pos = {0};
public:
    Cell(Vector2 pos,Color color){
        id++;
        this->pos = pos;
        this->color = color;
    }

    virtual void init() = 0;
    virtual void update(float delta) = 0;
    virtual void render(Texture2D texture) = 0;
    virtual void cleanup() = 0;

    virtual void receive(CellSignal& signal) = 0;

    [[nodiscard]] int getId() const{
        return id;
    };
};


#endif //SKYNET_EDITOR_CELL_H
