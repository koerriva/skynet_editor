//
// Created by 50409 on 2021/4/27.
//
#include "NodeEditor.h"

namespace GamePlay{
    void NodeEditor::Run(){
        m_SignalTick++;
        for(int node:inputs){
            auto in = m_Nodes.find(node);
            if(m_SignalTick%21==0){
                in->isActive = !in->isActive;
            }
            if(in->isActive){
                //发射(信号量100)
                in->value = 100;
            }else{
                in->value = 0;
            }
            in->t = m_SignalTick.load();
            for (int toNode:m_LinkMap[node]) {
                int linkId = (node<<16)+toNode;
                int signal = in->value * m_NodeLinks.find(linkId)->weight * 0.01f;
                m_Signals[toNode].push_back(NodeSignal{node,in->t,signal});
            }
        }
        for (int node:neurals) {
            auto in = m_Nodes.find(node);
            in->value = 0;
            in->isActive = false;
        }
        for (int i = 0; i < neurals.size(); ++i) {
            for (int node:neurals) {
                auto in = m_Nodes.find(node);
                if(in->isActive&&in->t==m_SignalTick.load()&&m_Signals[node].empty())continue;
                in->t = m_SignalTick.load();
                //参与激活的神经元节点
                std::vector<int> activeNodes;
                //累计信号量
                auto iter = m_Signals[node].begin();
                while (iter!=m_Signals[node].end()){
                    if(iter->t==m_SignalTick){
                        auto linkNode = m_NodeLinks.find((iter->from<<16)+node);
                        if(linkNode->type==44){
                            activeNodes.push_back(iter->from);
                        }
                        in->value += iter->value;
                        iter=m_Signals[node].erase(iter);
                    }else{
                        iter++;
                    }
                }
                if(in->value>=in->threshold){
                    //激发神经元(信号量100)
                    in->isActive = true;
                    in->value = 100;
                    for (int toNode:m_LinkMap[node]) {
                        auto linkNode = m_NodeLinks.find((node<<16)+toNode);
                        int signal = in->value * linkNode->weight*0.01f;
//                        auto r = std::find(activeNodes.begin(),activeNodes.end(),toNode);
//                        if(r!=activeNodes.end()){
                        if(linkNode->type==44){
                            //如果是环形连接,则t+1
                            m_Signals[toNode].push_back(NodeSignal{node,in->t+1,signal});
                        }else{
                            m_Signals[toNode].push_back(NodeSignal{node,in->t,signal});
                        }
                    }
                }
            }
        }
        for (int node:outputs) {
            auto in = m_Nodes.find(node);
            in->value = 0;
            in->isActive = false;
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
            if(in->value>0){
                in->isActive = true;
                in->value = 100;
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<long>((1.0f/60.0f)*1000)));
    }
}