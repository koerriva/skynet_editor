//
// Created by 50409 on 2021/3/13.
//

#ifndef SKYNET_EDITOR_BRAIN_H
#define SKYNET_EDITOR_BRAIN_H

#include <unordered_map>
#include <vector>
#include <memory>
#include "CellSignal.h"
#include "Cell.h"
#include "VisionCell.h"

class Brain {
private:
    std::vector<Cell> cells;
    std::unordered_map<int,Cell*> visionCells;
    std::unordered_map<int,CellSignal> hub;

public:
    void init(){}
    void update(){
        std::vector<int> _cells;
        for (auto& entry:hub){
            _cells.push_back(entry.first);
            entry.second.to->receive(entry.second);
        }
        for (int & cell : _cells) {
            hub.erase(cell);
        }
    }

    void addVisionCell(Vector2 pos,Color color){
        VisionCell cell(pos,color);
        cells.push_back(cell);
        visionCells[cell.getId()] = &cells[cells.size()-1];
    }

    void saveSignal(CellSignal signal){
        hub[signal.from->getId()] = signal;
    }
};

#endif //SKYNET_EDITOR_BRAIN_H
