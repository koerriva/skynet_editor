//
// Created by 50409 on 2021/4/27.
//
#include "NodeEditor.h"

namespace GamePlay{
    void NodeEditor::Run(){
        m_SignalTick++;
        for(int node:inputs){
            auto in = m_Nodes.find(node);
            if(m_SignalTick%11==0){
                in->isActive = !in->isActive;
                if(in->isActive){
                    in->value = GetRandomValue(0,100);
                }else{
                    in->value = 0;
                }
                in->t = m_SignalTick.load();
                if(in->isActive){
                    for (int toNode:m_LinkMap[node]) {
                        int linkId = (node<<16)+toNode;
                        int signal = in->value * m_NodeLinks.find(linkId)->weight * 0.01f;
                        m_Signals[toNode].push_back(NodeSignal{node,in->t,signal});
                    }
                }
            }
        }
        for (int i = 0; i < neurals.size(); ++i) {
            for (int node:neurals) {
                auto in = m_Nodes.find(node);
                in->t = m_SignalTick.load();
                auto iter = m_Signals[node].begin();
                while (iter!=m_Signals[node].end()){
                    if(iter->t==m_SignalTick){
                        in->value += iter->value;
                        iter=m_Signals[node].erase(iter);
                    }else{
                        iter++;
                    }
                }
                if(in->value>=in->threshold){
                    in->isActive = true;
                }else{
                    in->isActive = false;
                }
                if(in->isActive){
                    for (int toNode:m_LinkMap[node]) {
                        //auto out = m_Nodes.find(toNode);
                        //if toNode's type is Neural,t+1.
                        int linkId = (node<<16)+toNode;
                        int signal = in->value * m_NodeLinks.find(linkId)->weight;
                        m_Signals[toNode].push_back(NodeSignal{node,m_SignalTick.load(),signal});
                        in->value = 0;
                    }
                }
            }
        }
        for (int node:outputs) {
            auto in = m_Nodes.find(node);
            in->value = 0;
            in->t = m_SignalTick.load();
            in->isActive = false;
            auto iter = m_Signals[node].begin();
            while (iter!=m_Signals[node].end()){
                if(iter->t==m_SignalTick){
                    in->value += iter->value;
                    iter=m_Signals[node].erase(iter);
                    in->isActive = true;
                }else{
                    iter++;
                }
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<long>((1.0f/30.0f)*1000)));
    }
}