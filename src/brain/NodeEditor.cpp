//
// Created by 50409 on 2021/4/24.
//

#include "NodeEditor.h"

namespace GamePlay{
    void NodeEditor::Load(Font font) {
        m_UiFont = font;
        m_MainCanvas = Viewport(width,height);
        m_2dCanvas = Viewport(width,height);
        m_3dCanvas = Viewport{width/2,height/2};
        Init2D();
        Init3D();
    }
    void NodeEditor::Save() {
        UnloadFont(m_UiFont);
        UnloadTexture(m_Icons);
        for(auto& texture:m_OutputIcons){
            UnloadTexture(texture);
        }
        UnloadModel(m_Bug);
        UnloadModel(m_Playground);
    }
    void NodeEditor::Update() {
        if(IsKeyPressed(KEY_F10)){
            TraceLog(LOG_INFO,"NodeEditor::Change Viewport");
            editorMode = !editorMode;
        }
        m_Editing = false;
        Update3D();
//        if(editorMode){
//            Update2D();
//        }else{
//            ClearMenu();
//            m_Editing = false;
//            Update3D();
//        }
    }
    void NodeEditor::Render(){
        SetCameraMode(m_Camera3d,CAMERA_FREE);
        UpdateCamera(&m_Camera3d);
        RayMarching();
//        Viewport viewport;
//        if(editorMode){
//            Render2D(m_MainCanvas);
//            BeginTextureMode(m_3dCanvas.framebuffer);
//            SetCameraMode(m_Camera3d,CAMERA_FREE);
//            Render3D(m_MainCanvas);
//            EndTextureMode();
//
//            viewport = m_3dCanvas;
//            viewport.source = {0,0,400,-300};
//            viewport.rec = {0,0,400,300};
//        }else{
//            SetCameraMode(m_Camera3d,CAMERA_FREE);
//            UpdateCamera(&m_Camera3d);
//            Render3D(m_MainCanvas);
//            BeginTextureMode(m_2dCanvas.framebuffer);
//            Render2D(m_MainCanvas);
//            EndTextureMode();
//
//            viewport = m_2dCanvas;
//            viewport.source = {0,0,400,-300};
//            viewport.rec = {0,0,400,300};
//        }
//        DrawViewport(viewport);
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

    void NodeEditor::AddNode(UiNodeType uiNodeType,int category) {
        Vector2 position = m_MousePosition;

        UiNode uiNode(uiNodeType,m_UiNodeUniqueId++);
        uiNode.radius = 32;
        uiNode.position = position;

        Node node;

        int startAngle = 1;
        int endAngle = 8;
        switch (uiNodeType) {
            case UiNodeType::neural:{
                m_NeuralNum++;
                neurals.push_back(uiNode.id);
                node.type=NodeType::Neural;
                Vector3 c = ColorToHSV(GREEN);
                uiNode.colors[0] = ColorFromHSV(c.x,c.y,0.1);
                uiNode.colors[1] = ColorFromHSV(c.x,c.y,1.0);
                break;
            }
            case UiNodeType::input:{
                m_InputNum++;
                inputs.push_back(uiNode.id);
                node.type=NodeType::Input;
                node.inputFrequency = GetRandomValue(11,41);
                node.inputAction = category;
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
                node.type=NodeType::Output;
                node.outputAction  = category;
                Vector3 c = ColorToHSV(YELLOW);
                uiNode.colors[0] = ColorFromHSV(c.x,c.y,0.1);
                uiNode.colors[1] = ColorFromHSV(c.x,c.y,1.0);
                break;
            }
            default:return;
        }

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
                        toNode->link = nodeLink.id;
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
                                    break;
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
        toNode->decay = 0;
        toNode->isLearn = false;

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
            DrawTextureEx(m_NeuralTexture,Vector2SubtractValue(uiNode.position,uiNode.radius),0,scale,DARKRED);
            Vector2 iconPos = Vector2SubtractValue(uiNode.position,8);
            Color color = node->isActive?RED:BLACK;
            int index = 12+node->inputAction;
            DrawTextureRec(m_Icons,{static_cast<float>(16*index+index*2),16*2+2*2,16,16},iconPos,color);
        }
        if(uiNode.type==UiNodeType::neural){
            Color color = uiNode.cursorIn?GREEN:DARKGREEN;
            color = node->isActive?GREEN:color;
            DrawTextureEx(m_NeuralTexture,Vector2SubtractValue(uiNode.position,uiNode.radius),0,scale,color);
        }
        if(uiNode.type==UiNodeType::output){
            DrawTextureEx(m_NeuralTexture,Vector2SubtractValue(uiNode.position,uiNode.radius),0,scale,DARKYELLOW);
            Vector2 iconPos = Vector2SubtractValue(uiNode.position,8);
            Color color = node->isActive?YELLOW:BLACK;
            DrawTextureEx(m_OutputIcons[node->outputAction], iconPos, 0, scale, color);
        }
        if(uiNode.type==UiNodeType::pin){
//            DrawCircleGradient(uiNode.position.x,uiNode.position.y,uiNode.radius,BLUE,RAYWHITE);
            if(uiNode.cursorIn){
                DrawCircleLines(uiNode.position.x,uiNode.position.y,uiNode.radius,BLUE);
            }
        }
        if(uiNode.type==UiNodeType::synapse){
            auto nodeLink = m_NodeLinks.find(node->link);
            Color color1 = nodeLink->weight>=0?BLUE:RED;
            Color color2 = nodeLink->weight>=0?DARKBLUE:DARKRED;

            DrawCircleGradient(uiNode.position.x,uiNode.position.y,uiNode.radius,color1,color2);
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

    void NodeEditor::DrawViewport(Viewport& viewport){
        DrawTextureRec(viewport.framebuffer.texture,viewport.source,{viewport.rec.x,viewport.rec.y},WHITE);
    }
}
