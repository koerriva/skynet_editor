//
// Created by 50409 on 2021/3/13.
//

#ifndef SKYNET_EDITOR_VISIONCELL_H
#define SKYNET_EDITOR_VISIONCELL_H


#include "Cell.h"

class VisionCell : public Cell{
private:
    float last_check_time = 0;
    bool is_check_signal = false;
public:
    VisionCell(const Vector2 &pos, const Color &color);

    void init() override;

    void update(float delta) override;

    void render(Texture2D texture) override;

    void cleanup() override;

    void receive(CellSignal &signal) override;
};


#endif //SKYNET_EDITOR_VISIONCELL_H
