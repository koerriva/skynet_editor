//
// Created by 50409 on 2021/4/24.
//

#ifndef SKYNET_EDITOR_LAYERSTACK_H
#define SKYNET_EDITOR_LAYERSTACK_H


#include <vector>
#include "Layer.h"

namespace Engine{
    class LayerStack {
    public:
        ~LayerStack();
        void PushLayer(Layer* layer);
        void PopLayer(Layer* layer);

        std::vector<Layer*>::iterator begin() {return m_Layers.begin();}
        std::vector<Layer*>::iterator end() {return m_Layers.end();}
    private:
        std::vector<Layer*> m_Layers;
        unsigned int m_LayerInsertIndex = 0;
    };
}


#endif //SKYNET_EDITOR_LAYERSTACK_H
