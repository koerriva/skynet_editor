#ifndef MYGAME_H
#define MYGAME_H

struct Neural{
    Vector2 center;
    Color color=WHITE;
    bool isActive;
};

enum PlayerAction{
    Idle,MoveScene,AddNode,MoveNode
};

#endif