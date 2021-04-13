//
// Created by 50409 on 2021/3/13.
//

#ifndef SKYNET_EDITOR_MUSCLECELL_H
#define SKYNET_EDITOR_MUSCLECELL_H


#include "Cell.h"

class MuscleCell : public Cell {
public:
    void init() override;

    void update(float delta) override;

    void render(Texture2D texture) override;

    void cleanup() override;
};


#endif //SKYNET_EDITOR_MUSCLECELL_H
