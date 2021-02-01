#ifndef MYGAME_H
#define MYGAME_H

struct Neural{
    Vector2 center;
    Color color=WHITE;
    bool isActive = false;
};

struct NeuralLink{
    Vector2 start;
    Vector2 end;
    bool isFinish = false;
    bool isActive = false;
};

enum PlayerAction{
    Idle,MoveScene,AddNode,MoveNode,LinkNode
};


enum CursorState{
    OnGround,InNode
};

enum NeuralLinkState{
    UNLINK,BEGIN,END
};

#endif