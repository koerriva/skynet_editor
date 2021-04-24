//
// Created by 50409 on 2021/4/24.
//

#include <raymath.h>
#include "NodeEditor.h"
#include "graph.h"

namespace GamePlay{

    void NodeEditor::Load(Camera2D camera) {
        m_NeuralTexture = LoadTexture("data/neural.png");
        m_Camera = camera;
    }

    void NodeEditor::Update() {
        if(IsKeyPressed(KEY_A)){
            TraceLog(LOG_INFO,"NodeEditor::AddNode");
            AddNode();
        }

        if(IsKeyPressed(KEY_X)){
            TraceLog(LOG_INFO,"NodeEditor::DelNode");
            DelNode();
        }

        m_MousePosition = GetScreenToWorld2D(GetMousePosition(), m_Camera);
        for(const auto& uiNode:m_UiNodes){
            auto node = m_Nodes.find(uiNode.id);
            if(IsInside(uiNode.radius,uiNode.position,m_MousePosition)){
                node->isActive = true;
            } else{
                node->isActive = false;
            }

            if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                if(uiNode.type==UiNodeType::synapse){
                    TraceLog(LOG_INFO, "Selected Synapse");
                    selected = uiNode.id;
                }
            }
        }
    }

    void NodeEditor::Show() {
        DrawGrid();
        for(const auto& uiNode:m_UiNodes){
            DrawNode(uiNode);
        }
        if(IsMouseButtonDown(MOUSE_LEFT_BUTTON)){
            if(selected){
                auto s = m_Nodes.find(selected);
                if(s->type==NodeType::Synapse){
                    auto synapse = m_UiNodes.find(selected);
                    DrawLineBezier(synapse->position,m_MousePosition,2,WHITE);
                }
            }
        }
    }

    void NodeEditor::Save() {

    }

    void NodeEditor::AddNode() {
        Vector2 position = m_MousePosition;

        UiNode uiNode(UiNodeType::neural,m_UiNodeUniqueId++);
        uiNode.radius = 32;
        uiNode.position = position;
        m_UiNodes.insert(uiNode.id,uiNode);

        Node neural(NodeType::Neural);
        m_Nodes.insert(uiNode.id,neural);

        //(p+r*cos(2π*(n-1)/n),q+r*sin(2π*(n-1)/n))
        int n = 8;
        for (int i = 1; i <= n; ++i) {
            UiNode pin(UiNodeType::synapse,m_UiNodeUniqueId++);
            pin.radius = 5;
            pin.position.x = position.x+uiNode.radius*cos(2*PI*(i-1)/n);
            pin.position.y = position.y+uiNode.radius*sin(2*PI*(i-1)/n);
            m_UiNodes.insert(pin.id,pin);

            Node synapse(NodeType::Synapse);
            m_Nodes.insert(pin.id,synapse);
        }
    }

    void NodeEditor::LinkNode() {

    }

    void NodeEditor::DelNode() {
        if(!m_UiNodes.empty()){
            m_UiNodes.clear();
        }
    }

    void NodeEditor::DrawGrid() {
        int start_x = -m_WorldWidth/2;
        int start_y = -m_WorldHeight/2;

        int end_x = m_WorldWidth/2;
        int end_y = m_WorldHeight/2;
        //draw grid
        int seg = 20;
        for (int i = 0; i < m_WorldWidth / seg; ++i) {
            DrawLine(start_x+i*seg,start_y
                    ,start_x+i*seg,end_y
                    ,DARKGRAY);
        }
        for (int i = 0; i < m_WorldWidth / (seg*5); ++i) {
            DrawLine(start_x+i*seg*5,start_y
                    ,start_x+i*seg*5,end_y
                    ,BLACK);
        }
        for (int i = 0; i < m_WorldHeight/seg; ++i) {
            DrawLine(start_x,i*seg+start_y
                    ,end_x,i*seg+start_y
                    ,DARKGRAY);
        }
        for (int i = 0; i < m_WorldHeight/(seg*5); ++i) {
            DrawLine(start_x,i*seg*5+start_y
                    ,end_x,i*seg*5+start_y
                    ,BLACK);
        }
    }

    void NodeEditor::DrawNode(const UiNode &uiNode) {
        float scale = uiNode.radius/32;
        auto node = m_Nodes.find(uiNode.id);

        if(uiNode.type==UiNodeType::neural){
            Color color = node->isActive?GREEN:DARKGREEN;
            DrawTextureEx(m_NeuralTexture,Vector2SubtractValue(uiNode.position,uiNode.radius),0,scale,color);
        }
        if(uiNode.type==UiNodeType::synapse){
            Color color = node->isActive?BLUE:DARKBLUE;
            DrawCircleV(uiNode.position,uiNode.radius,color);
//            DrawTextureEx(m_NeuralTexture,uiNode.position,0,scale,color);
        }
    }
}
