//
// Created by 50409 on 2021/4/24.
//

#ifndef SKYNET_EDITOR_NODEEDITOR_H
#define SKYNET_EDITOR_NODEEDITOR_H

#include "thread"
#include "vector"
#include "unordered_map"
#include "graph.h"
#include "raylib.h"
#include "node.h"

namespace GamePlay{
    class NodeEditor {
    public:
        void Load(Camera2D camera,Font font);
        void Show();
        void ShowMenu();
        void Update();
        void Save();

        void Run(){
            TraceLog(LOG_INFO,"running...");
            m_SignalTick++;
            for(int node:inputs){
                auto in = m_Nodes.find(node);
                in->isActive = !in->isActive;
                in->value = GetRandomValue(0,100);
                in->t = m_SignalTick.load();
                if(in->isActive){
                    for (int toNode:m_LinkMap[node]) {
                        m_Signals[toNode].push_back(NodeSignal{node,in->t,in->value});
                    }
                }
            }
            for (int node:neurals) {
                auto in = m_Nodes.find(node);
                in->value = 0;
                in->t = m_SignalTick.load();
                auto iter = m_Signals[node].begin();
                while (iter!=m_Signals[node].end()){
                    if(iter->t==m_SignalTick){
                        in->value += iter->value;
                        iter=m_Signals[node].erase(iter);
                    }
                }
                if(in->value>=in->threshold){
                    in->isActive = true;
                }else{
                    in->isActive = false;
                }
                if(in->isActive){
                    for (int toNode:m_LinkMap[node]) {
                        m_Signals[toNode].push_back(NodeSignal{node,m_SignalTick.load(),in->value});
                        //if toNode's type is Neural,t+1.
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
                    }
                }
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }

        int GetNodeCount() { return m_UiNodes.size();}
    private:
        void AddNode(UiNodeType type);
        void LinkNode(int from,int to);
        void DelNode();

        void ShowAddMenu(Menu& menu);
        void ShowNodeMenu(Menu& menu);

        void DrawGrid();
        void DrawNode(const UiNode& uiNode);
        void DrawLink(const UiLink& uiLink);

        int debugTextLine = 0;
        void DrawDebugText(const char* text){
            DrawTextEx(m_UiFont,text,Vector2{5,static_cast<float>(30+debugTextLine*16)},static_cast<float>(m_UiFont.baseSize),1.0,GREEN);
            debugTextLine++;
        }

        void ClearMenu(){
            int size = m_Menus.size();
            for (int i = 0; i < size; ++i) {
                m_Menus.pop();
            }
        }
    private:
        int selected=0;
        Vector2 selected_point;
        int m_Hovering=0;

        IdMap<UiNode> m_UiNodes;
        IdMap<Node> m_Nodes;
        IdMap<UiLink> m_UiLinks;
        IdMap<NodeLink> m_NodeLinks;
        std::stack<Menu> m_Menus;
        int m_UiNodeUniqueId=1;

        Texture2D m_NeuralTexture;
        int m_WorldWidth = 4000;
        int m_WorldHeight = 4000;
        Camera2D m_Camera;
        Vector2  m_MousePosition;
        Font m_UiFont;

        bool m_Linking = false;
        bool m_Dragging = false;
        bool m_Editing = false;

        int m_InputNum = 0;
        int m_NeuralNum = 0;
        int m_OutputNum = 0;

        std::vector<int> inputs;
        std::vector<int> neurals;
        std::vector<int> outputs;
        std::unordered_map<int,std::vector<int>> m_LinkMap;

        std::unordered_map<int,std::vector<NodeSignal>> m_Signals;
        std::atomic_int m_SignalTick;
    };
}


#endif //SKYNET_EDITOR_NODEEDITOR_H
