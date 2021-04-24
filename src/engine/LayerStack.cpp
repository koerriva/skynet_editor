//
// Created by 50409 on 2021/4/24.
//

#include "LayerStack.h"

namespace Engine{

    void LayerStack::PushLayer(Layer *layer) {
        m_Layers.emplace(m_Layers.begin()+m_LayerInsertIndex,layer);
        m_LayerInsertIndex++;
    }

    void LayerStack::PopLayer(Layer *layer) {
        auto it = std::find(m_Layers.begin(),m_Layers.end(),layer);
        if(it!=m_Layers.end()){
            m_Layers.erase(it);
            m_LayerInsertIndex--;
        }
    }

    LayerStack::~LayerStack() {
        for (Layer* layer : m_Layers){
            delete layer;
        }
    }
}