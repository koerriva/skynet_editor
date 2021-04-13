//
// Created by 50409 on 2021/3/13.
//

#ifndef SKYNET_EDITOR_CELLSIGNAL_H
#define SKYNET_EDITOR_CELLSIGNAL_H
class Cell;

class CellSignal {
public:
    int type;
    int value;
    Cell* from;
    Cell* to;
};


#endif //SKYNET_EDITOR_CELLSIGNAL_H
