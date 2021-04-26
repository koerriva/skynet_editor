//
// Created by 50409 on 2021/4/24.
//

#include <raymath.h>
#include "NodeEditor.h"
#include "graph.h"

#define RAYGUI_IMPLEMENTATION
#define RAYGUI_SUPPORT_ICONS
#include <raygui.h>
#include <ricons.h>

namespace GamePlay{

    void NodeEditor::Load(Camera2D camera,Font font) {
        m_NeuralTexture = LoadTexture("data/neural.png");
        m_Camera = camera;
        m_UiFont = font;

        GuiSetFont(m_UiFont);
    }
    void NodeEditor::Save() {}

    void NodeEditor::Update() {
        if(IsKeyPressed(KEY_X)){
            TraceLog(LOG_INFO,"NodeEditor::DelNode");
            DelNode();
            ClearMenu();
        }
        if(IsKeyPressed(KEY_A)){
            TraceLog(LOG_INFO,"NodeEditor::PopMenu");
            ClearMenu();
            Vector2 pos = GetWorldToScreen2D(m_MousePosition,m_Camera);
            Menu menu{MenuType::AddNode,pos,{pos.x,pos.y,100,80},0};
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
            Menu menu{MenuType::Neural,pos,{pos.x,pos.y,200,260},selected};
            m_Menus.push(menu);
            m_Editing = true;
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
            auto node = m_Nodes.find(uiNode.id);
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
            } else{
                m_UiNodes.find(uiNode.id)->CursorOut();
            }
        }

        //deselect
        if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
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
                        Vector2 pos = m_UiNodes.find(i)->position;
                        m_UiNodes.find(i)->position = Vector2Subtract(pos,offset);
                    }
                    m_Dragging = true;
                }
            }
        }
        if(IsMouseButtonUp(MOUSE_LEFT_BUTTON)){
            m_Dragging = false;
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
            if(selected>0 && m_Hovering > 0 && m_Hovering != selected){
                LinkNode(selected,m_Hovering);
            }
            m_Linking = false;
            selected = 0;
        }
    }

    void NodeEditor::ShowMenu() {
        if(!m_Menus.empty()){
            auto& menu = m_Menus.top();
            ShowAddMenu(menu);
            ShowNodeMenu(menu);
        }

        DrawDebugText(TextFormat("输入数\t%d",m_InputNum));
        DrawDebugText(TextFormat("输出数\t%d",m_OutputNum));
        DrawDebugText(TextFormat("神经元数\t%d",m_NeuralNum));
        DrawDebugText(TextFormat("连接数\t%d",m_UiLinks.size()));
        debugTextLine=0;
    }

    void NodeEditor::ShowAddMenu(Menu &menu) {
        if(menu.type!=MenuType::AddNode)return;
        float start_x = menu.position.x;
        float start_y = menu.position.y;
        float w = menu.rec.width;
        float h = menu.rec.height;
        GuiPanel(menu.rec);
        GuiSetFont(m_UiFont);
        if(GuiButton(Rectangle{start_x+5,start_y+5,w-10,20},"输入节点")){
            AddNode(UiNodeType::input);
            m_Editing = false;
            m_Menus.pop();
        }
        if(GuiButton(Rectangle{start_x+5,start_y+30,w-10,20},"神经元")){
            AddNode(UiNodeType::neural);
            m_Editing = false;
            m_Menus.pop();
        }
        if(GuiButton(Rectangle{start_x+5,start_y+55,w-10,20},"输出节点")){
            AddNode(UiNodeType::output);
            m_Editing = false;
            m_Menus.pop();
        }
    }

    void NodeEditor::ShowNodeMenu(Menu& menu){
        if(menu.type!=MenuType::Neural)return;
        auto uiNode = m_UiNodes.find(menu.uiNode);
        if(uiNode->type!=UiNodeType::neural)return;
        auto neural = m_Nodes.find(menu.uiNode);

        Vector2 screen_pos = menu.position;
        float start_x = screen_pos.x;
        float start_y = screen_pos.y;
        float w = 200;
        float h = 260;
        bool weightEditMode = false;

        bool windows = GuiWindowBox(Rectangle{ start_x, start_y, w, h },TextFormat("神经元 0x%X",uiNode));
        uiNode->editType = GuiToggleGroup(Rectangle{ start_x+5, start_y+20+7, 62, 20 }, "行为;外观", uiNode->editType);
        if(uiNode->editType==0){
            GuiLabel(Rectangle{ start_x+5, start_y+20+43, 53, 25 }, "权重");
            if (GuiSpinner(Rectangle{ start_x+89, start_y+20+43, 104, 25 }, nullptr, &neural->weight, 0, 100, weightEditMode)){
                weightEditMode = !weightEditMode;
            }
            neural->isLearn = GuiCheckBox({start_x+5,start_y+20+43+45,15,15},"学习",neural->isLearn);
        }

        if(uiNode->editType==1){
            Rectangle bounds = {start_x+5,start_y+20+43,128,25};
            int state = neural->isActive?1:0;
            bool r = GuiDropdownBox(bounds,"未激活;激活",&uiNode->editColorType, uiNode->editColorMode);
            if(r){
                uiNode->editColorMode = !uiNode->editColorMode;
            }
            int padding = 0;
            if(uiNode->editColorMode){
                padding=70;
            }
            uiNode->colors[state] = GuiColorPicker({start_x+5,start_y+20+43+25+padding,128,95},uiNode->colors[state]);
        }
        if(windows){
            m_Menus.pop();
            m_Editing = false;
        }
    }

    void NodeEditor::AddNode(UiNodeType uiNodeType) {
        Vector2 position = m_MousePosition;

        UiNode uiNode(uiNodeType,m_UiNodeUniqueId++);
        uiNode.radius = 32;
        uiNode.position = position;

        NodeType nodeType;

        switch (uiNodeType) {
            case UiNodeType::neural:{
                m_NeuralNum++;
                neurals.push_back(uiNode.id);
                nodeType=NodeType::Neural;break;
            }
            case UiNodeType::input:{
                m_InputNum++;
                inputs.push_back(uiNode.id);
                nodeType=NodeType::Input;break;
            }
            case UiNodeType::output:{
                m_OutputNum++;
                outputs.push_back(uiNode.id);
                nodeType=NodeType::Output;break;
            }
            default:return;
        }
        Node node(nodeType);
        m_Nodes.insert(uiNode.id,node);

        //(p+r*cos(2π*(n-1)/n),q+r*sin(2π*(n-1)/n))
        int n = 8;
        for (int i = 1; i <= n; ++i) {
            UiNode pin(UiNodeType::pin, m_UiNodeUniqueId++);
            pin.radius = 5;
            pin.position.x = position.x+uiNode.radius*cos(2*PI*(i-1)/n);
            pin.position.y = position.y+uiNode.radius*sin(2*PI*(i-1)/n);
            pin.parent = uiNode.id;
            m_UiNodes.insert(pin.id,pin);

            Node pin_node(NodeType::Pin);
            m_Nodes.insert(pin.id,pin_node);

            uiNode.children[i-1] = pin.id;
        }

        m_UiNodes.insert(uiNode.id,uiNode);
    }

    void NodeEditor::LinkNode(int fromId,int toId) {
        auto from = m_UiNodes.find(fromId);
        auto to = m_UiNodes.find(toId);
        if(from->type!=UiNodeType::pin||to->type!=UiNodeType::pin){
            return;
        }
        auto fromParent = m_UiNodes.find(from->parent);
        auto toParent = m_UiNodes.find(to->parent);
        if(fromParent->type==UiNodeType::input&&toParent->type==UiNodeType::neural
           ||fromParent->type==UiNodeType::neural&&toParent->type==UiNodeType::neural
           ||fromParent->type==UiNodeType::neural&&toParent->type==UiNodeType::output){
            if(to->cursorIn&&from->parent!=to->parent){
                UiLink uiLink = {(fromId<<16)+toId,fromId,toId};
                NodeLink nodeLink = {(from->parent<<16)+to->parent,from->parent,to->parent};
                if(!m_UiLinks.contains(uiLink.id)&&!m_NodeLinks.contains(nodeLink.id)){
                    m_UiLinks.insert(uiLink.id,uiLink);
                    m_NodeLinks.insert(nodeLink.id,nodeLink);
                    from->type = UiNodeType::node;
                    to->type = UiNodeType::synapse;
                }
            }
        }
    }

    void NodeEditor::DelNode() {
        if(!m_UiNodes.empty()){
            m_UiNodes.clear();
            m_Nodes.clear();
            m_UiLinks.clear();
            m_NodeLinks.clear();
        }
        selected = 0;
        m_Hovering = 0;
        selected_point = {0};

        m_InputNum = 0;
        m_NeuralNum = 0;
        m_OutputNum = 0;
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
        if(selected>0){
            auto _uiNode = m_UiNodes.find(selected);
            DrawCircleLines(_uiNode->position.x,_uiNode->position.y,_uiNode->radius,WHITE);
        }
        float scale = uiNode.radius/32;
        auto node = m_Nodes.find(uiNode.id);

        if(uiNode.type==UiNodeType::input){
            Color color = uiNode.cursorIn?RED:Color{ 115, 20, 27, 255 };
            color = node->isActive?RED:color;
            DrawTextureEx(m_NeuralTexture,Vector2SubtractValue(uiNode.position,uiNode.radius),0,scale,color);
        }
        if(uiNode.type==UiNodeType::neural){
            Color color = uiNode.cursorIn?GREEN:DARKGREEN;
            DrawTextureEx(m_NeuralTexture,Vector2SubtractValue(uiNode.position,uiNode.radius),0,scale,color);
        }
        if(uiNode.type==UiNodeType::output){
            Color color = uiNode.cursorIn?YELLOW:Color{126, 124, 0, 255};
            DrawTextureEx(m_NeuralTexture,Vector2SubtractValue(uiNode.position,uiNode.radius),0,scale,color);
        }
        if(uiNode.type==UiNodeType::pin){
            Color color = uiNode.cursorIn?BLUE:DARKBLUE;
            DrawCircleV(uiNode.position,uiNode.radius,color);
//            DrawTextureEx(m_NeuralTexture,uiNode.position,0,scale,color);
        }
        if(uiNode.type==UiNodeType::synapse){
            Color color = uiNode.cursorIn?GREEN:DARKGREEN;
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
