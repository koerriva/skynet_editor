﻿//
// Created by 50409 on 2021/4/24.
//

#include <raymath.h>
#include "NodeEditor.h"
#include "graph.h"

#define RAYGUI_IMPLEMENTATION
#include <raygui.h>

namespace GamePlay{

    void NodeEditor::Load(Font font) {
        width = GetScreenWidth();
        height = GetScreenHeight();

        m_NeuralTexture = LoadTexture("data/neural.png");
        m_Camera.offset = {};
        m_Camera.zoom  = 1.0;
        m_Camera.target = {};
        m_Camera.rotation = 0.0;

        m_Camera3d.position = { 0.0f, 25.0f, 50.0f };
        m_Camera3d.target = {0.0f, 0.0f, 0.0f};
        m_Camera3d.up = {0.0f, 1.0f, 0.0f};
        m_Camera3d.fovy = 45.0f;
        m_Camera3d.type = CAMERA_PERSPECTIVE;
        SetCameraMode(m_Camera3d,CAMERA_FREE);

        m_Playground = LoadModelFromMesh(GenMeshPlane(100,100,10,10));
        m_Playground.materials[0] = LoadMaterialPBR(DARKGREEN,0.0,1.0);
        m_Bug  = LoadModelFromMesh(GenMeshCube(1,1,1));
        m_Bug.materials[0]= LoadMaterialPBR(DARKBROWN,0.9,0.1);
//        mainLight = {1,0,{10,10,10},{0,0,0},BLACK};
//
//        Shader shader = m_Playground.materials[0].shader;
//        SetShaderValue(shader,GetShaderLocation(shader,"lights[0].enabled"),&mainLight.enabled,UNIFORM_INT);
//        SetShaderValue(shader,GetShaderLocation(shader,"lights[0].type"),&mainLight.type,UNIFORM_INT);
//        auto* position = new float[3] {mainLight.position.x,mainLight.position.y,mainLight.position.z};
//        SetShaderValue(shader,GetShaderLocation(shader,"lights[0].position"),position,UNIFORM_VEC3);
//        auto* target = new float[3] {mainLight.target.x,mainLight.target.y,mainLight.target.z};
//        SetShaderValue(shader,GetShaderLocation(shader,"lights[0].target"),target,UNIFORM_VEC3);
//        auto* color = new float[4] {mainLight.color.r/255.0f,mainLight.color.g/255.0f,mainLight.color.b/255.0f,mainLight.color.a/255.0f};
//        SetShaderValue(shader,GetShaderLocation(shader,"lights[0].color"),color,UNIFORM_VEC4);

        m_UiFont = font;

        m_LightingShader = LoadShader(nullptr,"data/shader/lighting.glsl.frag");
        m_LightingTexture = LoadTextureFromImage(GenImageColor(width,height,BLACK));

        m_BaseShader = LoadShader(nullptr,"data/shader/base.frag");
        m_RenderTarget = LoadRenderTexture(m_TargetSize.x,m_TargetSize.y);

        GuiSetFont(m_UiFont);
    }
    void NodeEditor::Save() {}

    void NodeEditor::Update() {
        width = GetScreenWidth();
        height = GetScreenHeight();

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
    }

    void NodeEditor::Render2D() {
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

        Render3D();

        EndMode2D();
//        DrawLight();
    }

    void NodeEditor::RenderGUI() {
        if(!m_Menus.empty()){
            auto& menu = m_Menus.top();
            ShowAddMenu(menu);
            ShowNeuralMenu(menu);
            ShowSynapseMenu(menu);
        }
        ShowStatusBar();
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

    void NodeEditor::ShowNeuralMenu(Menu& menu){
        if(menu.type!=MenuType::Neural)return;
        auto uiNode = m_UiNodes.find(menu.uiNode);
        if(uiNode->type!=UiNodeType::neural)return;
        auto neural = m_Nodes.find(menu.uiNode);

        Vector2 screen_pos = menu.position;
        float start_x = screen_pos.x;
        float start_y = screen_pos.y;
        float w = menu.rec.width;
        float h = menu.rec.height;
        bool thresholdEditMode = false;

        bool windows = GuiWindowBox(Rectangle{ start_x, start_y, w, h },TextFormat("神经元 0x%X",uiNode));
        uiNode->editType = GuiToggleGroup(Rectangle{ start_x+5, start_y+20+7, 62, 20 }, "行为;外观", uiNode->editType);
        if(uiNode->editType==0){
            GuiLabel(Rectangle{ start_x+5, start_y+20+43, 53, 25 }, "阈值");
//            if (GuiSpinner(Rectangle{ start_x+89, start_y+20+43, 104, 25 }, nullptr, &neural->threshold, -100, 100, thresholdEditMode)){
//                thresholdEditMode = !thresholdEditMode;
//            }
            neural->threshold = GuiSlider(Rectangle{ start_x+89, start_y+20+43, 104, 25 },TextFormat("%d",neural->threshold),"",neural->threshold,-100,100);
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

    void NodeEditor::ShowSynapseMenu(Menu& menu){
        if(menu.type!=MenuType::Synapse)return;
        auto uiNode = m_UiNodes.find(menu.uiNode);
        if(uiNode->type!=UiNodeType::synapse)return;
        auto synapse = m_Nodes.find(menu.uiNode);
        auto nodeLink = m_NodeLinks.find(uiNode->linkId);

        Vector2 screen_pos = menu.position;
        float start_x = screen_pos.x;
        float start_y = screen_pos.y;
        float w = menu.rec.width;
        float h = menu.rec.height;
        bool thresholdEditMode = false;

        bool windows = GuiWindowBox(Rectangle{ start_x, start_y, w, h },TextFormat("突触 0x%X",uiNode));
        uiNode->editType = GuiToggleGroup(Rectangle{ start_x+5, start_y+20+7, 62, 20 }, "行为;外观", uiNode->editType);
        if(uiNode->editType==0){
            GuiLabel(Rectangle{ start_x+5, start_y+20+43, 53, 25 }, "权重");
//            if (GuiSpinner(Rectangle{ start_x+89, start_y+20+43, 104, 25 }, nullptr, &nodeLink->weight, -100, 100, thresholdEditMode)){
//                thresholdEditMode = !thresholdEditMode;
//            }
            nodeLink->weight = GuiSlider(Rectangle{ start_x+89, start_y+20+43, 104, 25 },TextFormat("%d",nodeLink->weight),"",nodeLink->weight,-100,100);
            synapse->isLearn = GuiCheckBox({start_x + 5, start_y + 20 + 43 + 45, 15, 15}, "学习", synapse->isLearn);
        }

        if(uiNode->editType==1){
            Rectangle bounds = {start_x+5,start_y+20+43,128,25};
            int state = synapse->isActive ? 1 : 0;
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

    void NodeEditor::ShowStatusBar() {
        float h = height;
        float w = width;
        Rectangle rec1 = {0,h-16,w*0.5f,16};
        Rectangle rec2 = {w*0.5f,h-16,w*0.125f,16};
        Rectangle rec3 = {w*0.625f,h-16,w*0.125f,16};
        Rectangle rec4 = {w*0.75f,h-16,w*0.125f,16};
        Rectangle rec5 = {w*0.875f,h-16,w*0.125f,16};
        GuiSetFont(m_UiFont);
        if(selected>0){
            auto uiNode = m_UiNodes.find(selected);
            auto node = m_Nodes.find(selected);
            if(node->type==NodeType::Input){
                GuiStatusBar(rec1,TextFormat("Input %d, State %d",uiNode->id,node->value));
            }else if(node->type==NodeType::Neural){
                GuiStatusBar(rec1,TextFormat("Neural %d, State %d, Threshold %d",uiNode->id,node->value,node->threshold));
            }else if(node->type==NodeType::Output){
                GuiStatusBar(rec1,TextFormat("Output %d, State %d",uiNode->id,node->value));
            }else if(node->type==NodeType::Synapse){
                auto nodeLink = m_NodeLinks.find(uiNode->linkId);
                GuiStatusBar(rec1,TextFormat("Synapse %d, Weight %d",uiNode->id,nodeLink->weight));
            }else{
                GuiStatusBar(rec1,TextFormat("Node %d, Type %d",uiNode->id,uiNode->type));
            }
        } else{
            GuiStatusBar(rec1,"");
        }
        GuiStatusBar(rec2,TextFormat("输入数\t%d",m_InputNum));
        GuiStatusBar(rec3,TextFormat("神经元数\t%d",m_NeuralNum));
        GuiStatusBar(rec4,TextFormat("输出数\t%d",m_OutputNum));
        GuiStatusBar(rec5,TextFormat("连接数\t%d",m_UiLinks.size()));
    }

    void NodeEditor::AddNode(UiNodeType uiNodeType) {
        Vector2 position = m_MousePosition;

        UiNode uiNode(uiNodeType,m_UiNodeUniqueId++);
        uiNode.radius = 32;
        uiNode.position = position;

        NodeType nodeType;

        int startAngle = 1;
        int endAngle = 8;
        switch (uiNodeType) {
            case UiNodeType::neural:{
                m_NeuralNum++;
                neurals.push_back(uiNode.id);
                nodeType=NodeType::Neural;
                Vector3 c = ColorToHSV(GREEN);
                uiNode.colors[0] = ColorFromHSV(c.x,c.y,0.1);
                uiNode.colors[1] = ColorFromHSV(c.x,c.y,1.0);
                break;
            }
            case UiNodeType::input:{
                m_InputNum++;
                inputs.push_back(uiNode.id);
                nodeType=NodeType::Input;
                Vector3 c = ColorToHSV(RED);
                uiNode.colors[0] = ColorFromHSV(c.x,c.y,0.1);
                uiNode.colors[1] = ColorFromHSV(c.x,c.y,1.0);
                break;
            }
            case UiNodeType::output:{
                startAngle = 5;
                endAngle = 5;
                m_OutputNum++;
                outputs.push_back(uiNode.id);
                nodeType=NodeType::Output;
                Vector3 c = ColorToHSV(YELLOW);
                uiNode.colors[0] = ColorFromHSV(c.x,c.y,0.1);
                uiNode.colors[1] = ColorFromHSV(c.x,c.y,1.0);
                break;
            }
            default:return;
        }
        Node node(nodeType);
        m_Nodes.insert(uiNode.id,node);

        //(p+r*cos(2π*(n-1)/n),q+r*sin(2π*(n-1)/n))
        int n = 8;
        for (int i = startAngle; i <= endAngle; ++i) {
            UiNode pin(UiNodeType::pin, m_UiNodeUniqueId++);
            pin.radius = 5;
            pin.position.x = position.x+(uiNode.radius-pin.radius*0.2f)*cos(2.0f*PI*(i-1)/n);
            pin.position.y = position.y+(uiNode.radius-pin.radius*0.2f)*sin(2.0f*PI*(i-1)/n);
            pin.parent = uiNode.id;
            pin.pinPosition.x = pin.position.x+pin.radius*cos(2.0f*PI*(i-1)/n);
            pin.pinPosition.y = pin.position.y+pin.radius*sin(2.0f*PI*(i-1)/n);

            Vector3 c = ColorToHSV(BLUE);
            pin.colors[0] = ColorFromHSV(c.x,c.y,0.3);
            pin.colors[1] = ColorFromHSV(c.x,c.y,1.0);

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

        int linkType = 0;
        if(fromParent->type==UiNodeType::input&&toParent->type==UiNodeType::neural){
            linkType = 14;
        }
        if(fromParent->type==UiNodeType::neural&&toParent->type==UiNodeType::neural){
            linkType = 44;
        }
        if(fromParent->type==UiNodeType::neural&&toParent->type==UiNodeType::output){
            linkType = 45;
        }
        if(linkType>0){
            if(to->cursorIn&&from->parent!=to->parent){
                UiLink uiLink = {(fromId<<16)+toId,fromId,toId};

                NodeLink nodeLink = {(from->parent<<16)+to->parent,from->parent,to->parent,100,linkType};
                if(!m_UiLinks.contains(uiLink.id)&&!m_NodeLinks.contains(nodeLink.id)){
                    from->type = UiNodeType::node;
                    auto fromNode = m_Nodes.find(from->id);
                    fromNode->type = NodeType::Node;

                    if(linkType==45){
                        to->type = UiNodeType::node;
                        auto toNode = m_Nodes.find(to->id);
                        toNode->type = NodeType::Node;
                        nodeLink.weight = 100;
                    }else{
                        to->type = UiNodeType::synapse;
                        auto toNode = m_Nodes.find(to->id);
                        toNode->type = NodeType::Synapse;
                    }

                    to->linkFrom = fromId;
                    to->linkId = nodeLink.id;

                    m_LinkMap[from->parent].push_back(to->parent);

                    {
                        std::stack<int> stack;

                        stack.push(to->parent);
                        bool stop = false;

                        while (!stack.empty()&&!stop)
                        {
                            const int current_node = stack.top();
                            stack.pop();

                            for (const int toNode : m_LinkMap[current_node])
                            {
                                stop = toNode==from->parent;
                                if(stop){
                                    TraceLog(LOG_INFO,TextFormat("找到循环连接 %d->%d",to->parent,toNode));
                                    nodeLink.circle = true;
                                }
                                stack.push(toNode);
                            }
                        }
                    }

                    m_UiLinks.insert(uiLink.id,uiLink);
                    m_NodeLinks.insert(nodeLink.id,nodeLink);
                }
            }
        }
    }

    void NodeEditor::UnLinkNode(int fromId,int toId){
        TraceLog(LOG_INFO,TextFormat("UnLink %d->%d",fromId,toId));
        auto from = m_UiNodes.find(fromId);
        auto to = m_UiNodes.find(toId);

        m_UiLinks.erase((fromId<<16)+toId);
        m_NodeLinks.erase((from->parent<<16)+to->parent);

        from->type = UiNodeType::pin;
        auto fromNode = m_Nodes.find(from->id);
        fromNode->type = NodeType::Pin;
        to->type = UiNodeType::pin;
        auto toNode = m_Nodes.find(to->id);
        toNode->type = NodeType::Pin;
        to->linkFrom = 0;
        to->linkId = 0;

        auto& links = m_LinkMap[from->parent];
        auto iter = std::find(links.begin(),links.end(),to->parent);
        if(iter!=links.end()){
            links.erase(iter);
        }
    }

    void NodeEditor::DelNode() {
        if(!m_UiNodes.empty()){
            m_UiNodes.clear();
            m_Nodes.clear();
            m_UiLinks.clear();
            m_NodeLinks.clear();

            inputs.clear();
            neurals.clear();
            outputs.clear();
            m_LinkMap.clear();
            m_Signals.clear();

            m_SignalTick.store(0);
        }
        selected = 0;
        m_Hovering = 0;
        selected_point = {0};

        m_InputNum = 0;
        m_NeuralNum = 0;
        m_OutputNum = 0;
    }

    void NodeEditor::DrawBgGrid() {
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

        if(uiNode.type==UiNodeType::input){
            Color color = uiNode.cursorIn?RED:Color{ 115, 20, 27, 255 };
            color = node->isActive?RED:color;
            DrawTextureEx(m_NeuralTexture,Vector2SubtractValue(uiNode.position,uiNode.radius),0,scale,color);
        }
        if(uiNode.type==UiNodeType::neural){
            Color color = uiNode.cursorIn?GREEN:DARKGREEN;
            color = node->isActive?GREEN:color;
            DrawTextureEx(m_NeuralTexture,Vector2SubtractValue(uiNode.position,uiNode.radius),0,scale,color);
        }
        if(uiNode.type==UiNodeType::output){
            Color color = uiNode.cursorIn?YELLOW:Color{126, 124, 0, 255};
            color = node->isActive?YELLOW:color;
            DrawTextureEx(m_NeuralTexture,Vector2SubtractValue(uiNode.position,uiNode.radius),0,scale,color);
        }
        if(uiNode.type==UiNodeType::pin){
            DrawCircleGradient(uiNode.position.x,uiNode.position.y,uiNode.radius,BLUE,DARKBLUE);
            if(uiNode.cursorIn){
                DrawCircleLines(uiNode.position.x,uiNode.position.y,uiNode.radius,BLUE);
            }
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
        int linkid = (from->parent<<16)+to->parent;
        auto nodeLink = m_NodeLinks.find(linkid);

        float factor = abs(nodeLink->weight*0.01f);
        float thick = factor*2+2;
        unsigned char gray = static_cast<unsigned char>(factor*(255-130))+130;
        Color color = {gray,gray,gray,255};

        DrawMyBezierLine(from->position,from->pinPosition,to->position,to->pinPosition,thick,color);
    }

    void NodeEditor::DrawLight() {
        BeginShaderMode(m_LightingShader);
            DrawTextureRec(m_LightingTexture,{0,0,static_cast<float>(width),static_cast<float>(-height)},{-400,-300},BLACK);
        EndShaderMode();
    }
}
