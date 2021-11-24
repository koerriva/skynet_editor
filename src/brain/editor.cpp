//
// Created by koerriva on 2021/5/1.
//

#include "NodeEditor.h"

namespace GamePlay{
    void NodeEditor::Init2D() {
        m_NeuralTexture = LoadTexture("data/neural.png");
        m_Camera.offset = {width/2.0f,height/2.0f};
        m_Camera.zoom  = 1.0;
        m_Camera.target = {};
        m_Camera.rotation = 0.0;

        m_Icons = LoadTexture("data/icons.png");

        m_OutputIcons[0] = LoadTexture("data/stop.png");
        m_OutputIcons[1] = LoadTexture("data/move.png");
        m_OutputIcons[2] = LoadTexture("data/jump.png");
        m_OutputIcons[3] = LoadTexture("data/left-turn.png");
        m_OutputIcons[4] = LoadTexture("data/right-turn.png");

//        m_LightingShader = LoadShader(0,"data/shader/lighting.frag");
//        m_LightingTexture = LoadTextureFromImage(GenImageColor(width,height,BLACK));
    }
    void NodeEditor::Update2D() {
        if(IsKeyPressed(KEY_X)){
            TraceLog(LOG_INFO,"NodeEditor::DelNode");
            DelNode();
            ClearMenu();
        }
        if(IsKeyPressed(KEY_A)){
            TraceLog(LOG_INFO,"NodeEditor::PopMenu");
            ClearMenu();
            Vector2 pos = GetWorldToScreen2D(m_MousePosition,m_Camera);
            Menu menu{MenuType::AddNode,pos,{pos.x,pos.y,200,250},0};
            m_Menus.push(menu);
            m_Editing = true;
        }
        if(selected>0&&IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)){
            auto uiNode = m_UiNodes.find(selected);
            if(uiNode->cursorOut){
                selected = 0;
                selected_point = {0};
                return;
            }
            TraceLog(LOG_INFO,"NodeEditor::NodeMenu");
            Vector2 pos = GetWorldToScreen2D(m_MousePosition,m_Camera);
            if(uiNode->type==UiNodeType::input){
                Menu menu{MenuType::Input,pos,{pos.x,pos.y,200,260},selected};
                m_Menus.push(menu);
                m_Editing = true;
            }
            if(uiNode->type==UiNodeType::neural){
                Menu menu{MenuType::Neural,pos,{pos.x,pos.y,200,260},selected};
                m_Menus.push(menu);
                m_Editing = true;
            }
            if(uiNode->type==UiNodeType::synapse){
                Menu menu{MenuType::Synapse,pos,{pos.x,pos.y,200,260},selected};
                m_Menus.push(menu);
                m_Editing = true;
            }
        }
        if(m_Editing){
            auto& menu = m_Menus.top();
            Vector2 pos = GetWorldToScreen2D(m_MousePosition,m_Camera);
            if(!IsInRect(menu.rec,pos)&&IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
                ClearMenu();
                m_Editing = false;
            }
        }
        if(m_Editing)return;
        for(const auto& uiNode:m_UiNodes){
            if(IsInside(uiNode.radius,uiNode.position,m_MousePosition)){
                if(uiNode.type==UiNodeType::pin){
                    m_UiNodes.find(uiNode.id)->CursorIn();
                    m_Hovering = uiNode.id;
                    //select
                    if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
                        selected = uiNode.id;
                        selected_point = m_MousePosition;
                    }
                }
                if(uiNode.type==UiNodeType::neural||uiNode.type==UiNodeType::input||uiNode.type==UiNodeType::output){
                    if(IsInsideInner(uiNode.radius,uiNode.position,8,m_MousePosition)){
                        m_UiNodes.find(uiNode.id)->CursorIn();
                        m_Hovering = uiNode.id;
                        //select
                        if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
                            selected = uiNode.id;
                            selected_point = m_MousePosition;
                        }
                    }
                }
                if(uiNode.type==UiNodeType::synapse){
                    m_UiNodes.find(uiNode.id)->CursorIn();
                    m_Hovering = uiNode.id;
                    //select
                    if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
                        selected = uiNode.id;
                        selected_point = {};
                    }
                }
            } else{
                m_UiNodes.find(uiNode.id)->CursorOut();
            }
        }
        //deselect
        if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
            m_MouseLastPosition = m_MousePosition;
            if(selected>0){
                auto uiNode = m_UiNodes.find(selected);
                if(uiNode->cursorOut){
                    selected = 0;
                    selected_point = {0};
                }
            }
        }

        //drag
        if(IsMouseButtonDown(MOUSE_LEFT_BUTTON)){
            if(selected>0){
                auto from = m_UiNodes.find(selected);
                if(from->cursorIn&&(from->type==UiNodeType::neural||from->type==UiNodeType::input||from->type==UiNodeType::output)){
                    Vector2 offset = Vector2Subtract(selected_point,m_MousePosition);
                    selected_point = m_MousePosition;
                    from->position = Vector2Subtract(from->position,offset);

                    for (int i : from->children) {
                        if(i==0)continue;
                        auto uiNode = m_UiNodes.find(i);
                        Vector2 pos = uiNode->position;
                        Vector2 pinPos = uiNode->pinPosition;
                        uiNode->position = Vector2Subtract(pos,offset);
                        uiNode->pinPosition = Vector2Subtract(pinPos,offset);
                    }
                    m_Dragging = true;
                }
                if(from->cursorOut&&from->type==UiNodeType::synapse){
                    UnLinkNode(from->linkFrom,from->id);
                }
            }
            if(selected==0){
                Vector2 offset = Vector2Subtract(m_MousePosition,m_MouseLastPosition);
                m_Camera.offset = Vector2Add(m_Camera.offset,offset);
                m_Camera.offset.x = Clamp(m_Camera.offset.x,width-m_WorldWidth/2,m_WorldWidth/2);
                m_Camera.offset.y = Clamp(m_Camera.offset.y,height-m_WorldHeight/2,m_WorldHeight/2);
            }
        }

        //link
        if(IsMouseButtonUp(MOUSE_LEFT_BUTTON)&&m_Linking){
            if(selected>0 && m_Hovering > 0 && m_Hovering != selected){
                LinkNode(selected,m_Hovering);
            }
            m_Linking = false;
            selected = 0;
        }

        if(IsMouseButtonUp(MOUSE_LEFT_BUTTON)){
            m_Dragging = false;
        }

        //zoom camera
        float delta = GetMouseWheelMove();
        m_Camera.zoom = Clamp(m_Camera.zoom+delta*GetFrameTime(),0.6,2.0);

        //reset camera
        if(IsKeyPressed(KEY_Z)){
            m_Camera.offset = {width/2.0f,height/2.0f};
            m_Camera.zoom = 1.0;
        }
    }
    void NodeEditor::Render2D() {
        ClearBackground(SHENHAILV);
        BeginMode2D(m_Camera);

        m_MousePosition = GetScreenToWorld2D(GetMousePosition(), m_Camera);

        DrawBgGrid();
        if(selected>0){
            auto _uiNode = m_UiNodes.find(selected);
            DrawCircleLines(_uiNode->position.x,_uiNode->position.y,_uiNode->radius,RAYWHITE);
        }
        for(const auto& uiLink:m_UiLinks){
            DrawLink(uiLink);
        }
        for(const auto& uiNode:m_UiNodes){
            DrawNode(uiNode);
        }
        if(IsMouseButtonDown(MOUSE_LEFT_BUTTON)){
            if(selected>0){
                auto from = m_UiNodes.find(selected);
                if(from->type==UiNodeType::pin){
                    DrawMyBezierLine(from->position,from->pinPosition,m_MousePosition,Vector2Add(m_MousePosition,{2,0}),4,WHITE);
                    m_Linking = true;
                }
            }
        }
        EndMode2D();
    }
}