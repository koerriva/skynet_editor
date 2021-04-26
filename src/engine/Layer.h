//
// Created by 50409 on 2021/4/24.
//

#ifndef SKYNET_EDITOR_LAYER_H
#define SKYNET_EDITOR_LAYER_H

namespace Engine{
    class Layer{
    public:
        virtual ~Layer() = default;
        virtual void OnAttach() = 0;
        virtual void OnUpdate() = 0;
        virtual void OnAsyncUpdate() = 0;
        virtual void OnRender() = 0;
        virtual void OnGUIRender() = 0;
        virtual void OnDeAttach() = 0;
    };
}

#endif //SKYNET_EDITOR_LAYER_H
