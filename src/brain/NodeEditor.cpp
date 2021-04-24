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

        for(const auto& uiNode:m_UiNodes){
            auto node = m_Nodes.find(uiNode.id);
            if(IsInside(uiNode.radius,uiNode.position,m_MousePosition)){
                if(uiNode.type==UiNodeType::pin){
                    node->isActive = true;
                    hovering = uiNode.id;
                }
            } else{
                if(uiNode.type==UiNodeType::pin){
                    node->isActive = false;
//                    hovering = 0;
                }
            }

            if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                if(uiNode.type==UiNodeType::pin && node->isActive){
                    TraceLog(LOG_INFO, "Selected Synapse");
                    selected = uiNode.id;
                }
            }
        }
    }

    void NodeEditor::Show() {
        m_MousePosition = GetScreenToWorld2D(GetMousePosition(), m_Camera);

        DrawGrid();
        for(const auto& uiNode:m_UiNodes){
            DrawNode(uiNode);
        }
        for(const auto& uiLink:m_UiLinks){
            DrawLink(uiLink);
        }
        if(IsMouseButtonDown(MOUSE_LEFT_BUTTON)){
            if(selected>0){
                auto from = m_UiNodes.find(selected);
                if(from->type==UiNodeType::pin){
                    DrawLineBezier(from->position,m_MousePosition,4,WHITE);
                    m_Linking = true;
                }
            }
        }
        if(IsMouseButtonUp(MOUSE_LEFT_BUTTON)&&m_Linking){
            if(selected>0&&hovering>0&&hovering!=selected){
                auto from = m_UiNodes.find(selected);
                auto to = m_UiNodes.find(hovering);
                if(m_Nodes.find(to->id)->isActive){
                    from->type = UiNodeType::node;
                    to->type = UiNodeType::synapse;
                    LinkNode(from->id,to->id);
                }
            }
            m_Linking = false;
            selected = 0;
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
            UiNode pin(UiNodeType::pin, m_UiNodeUniqueId++);
            pin.radius = 5;
            pin.position.x = position.x+uiNode.radius*cos(2*PI*(i-1)/n);
            pin.position.y = position.y+uiNode.radius*sin(2*PI*(i-1)/n);
            m_UiNodes.insert(pin.id,pin);

            Node pin_node(NodeType::Pin);
            m_Nodes.insert(pin.id,pin_node);
        }
    }

    void NodeEditor::LinkNode(int from,int to) {
        UiLink uiLink = {static_cast<int>((from<<16)+to),from,to};
        if(!m_UiLinks.contains(uiLink.id)){
            m_UiLinks.insert(uiLink.id,uiLink);
        }
    }

    void NodeEditor::DelNode() {
        if(!m_UiNodes.empty()){
            m_UiNodes.clear();
            m_Nodes.clear();
            m_UiLinks.clear();
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
        if(uiNode.type==UiNodeType::pin){
            Color color = node->isActive?BLUE:DARKBLUE;
            DrawCircleV(uiNode.position,uiNode.radius,color);
//            DrawTextureEx(m_NeuralTexture,uiNode.position,0,scale,color);
        }
        if(uiNode.type==UiNodeType::synapse){
            Color color = node->isActive?GREEN:DARKGREEN;
            DrawCircleV(uiNode.position,uiNode.radius,color);
        }
        if(uiNode.type==UiNodeType::node){
//            Color color = node->isActive?YELLOW:Color{153, 149, 10, 255};
//            DrawCircleV(uiNode.position,uiNode.radius,color);
        }
    }

    void NodeEditor::DrawLink(const UiLink &uiLink) {
        auto from = m_UiNodes.find(uiLink.form);
        auto to = m_UiNodes.find(uiLink.to);
        DrawLineBezier(from->position,to->position,4,GRAY);
    }
}
