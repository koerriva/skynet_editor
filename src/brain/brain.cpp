//
// Created by 50409 on 2021/4/27.
//
#include "NodeEditor.h"

namespace GamePlay{
    void NodeEditor::Run(){
        m_SignalTick++;
        int t = m_SignalTick.load();
        for(int node:inputs){
            auto in = m_Nodes.find(node);
            if(m_SignalTick%21==0){
                in->isActive = !in->isActive;
            }
            if(in->isActive){
                in->value = 100;
            }else{
                in->value = 0;
            }
            in->t = t;
            for (int toNode:m_LinkMap[node]) {
                auto linkNode = m_NodeLinks.find((node<<16)+toNode);
                int signal = in->isActive?linkNode->weight:0;
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
                in->t = t;
                //累计信号量
                auto iter = m_Signals[node].begin();
                while (iter!=m_Signals[node].end()){
                    if(iter->t==t){
                        in->value += iter->value;
                        iter=m_Signals[node].erase(iter);
                    }else{
                        iter++;
                    }
                }
                if(in->value>=in->threshold&&!in->isActive){
                    in->isActive = true;
                    in->value = 100;
                    for (int toNode:m_LinkMap[node]) {
                        auto linkNode = m_NodeLinks.find((node<<16)+toNode);
                        int signal = linkNode->weight;
                        if(linkNode->type==44){
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
                m_BugSignal.push_back(ActionSignal{4,in->value});
            }
        }

        auto iter = m_BugSignal.begin();
        while (iter!=m_BugSignal.end()){
            ActionSignal signal = ActionSignal{iter->type,iter->value};
            StopBug(signal);
            MoveBug(signal);
            TurnBug(signal);
            iter = m_BugSignal.erase(iter);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<long>((1.0f/60.0f)*1000)));
    }
}