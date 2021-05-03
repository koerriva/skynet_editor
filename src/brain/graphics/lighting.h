//
// Created by 50409 on 2021/5/3.
//

#ifndef SKYNET_EDITOR_LIGHTING_H
#define SKYNET_EDITOR_LIGHTING_H

namespace GamePlay{// Light type
    typedef enum {
        LIGHT_DIRECTIONAL,
        LIGHT_POINT
    } LightType;

    struct Light {
        int type = 0;
        Vector3 position = {};
        Vector3 target = {};
        Color color = {255,255,255,255};
        bool enabled = true;

        // Shader locations
        int enabledLoc{};
        int typeLoc{};
        int posLoc{};
        int targetLoc{};
        int colorLoc{};
    };
}

#endif //SKYNET_EDITOR_LIGHTING_H
