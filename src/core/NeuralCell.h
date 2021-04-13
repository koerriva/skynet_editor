//
// Created by 50409 on 2021/3/13.
//

#ifndef SKYNET_EDITOR_NEURALCELL_H
#define SKYNET_EDITOR_NEURALCELL_H


#include "Cell.h"

class NeuralCell : public Cell {
public:
    void init() override;

    void update(float delta) override;

    void render(Texture2D texture) override;

    void cleanup() override;
};


#endif //SKYNET_EDITOR_NEURALCELL_H
