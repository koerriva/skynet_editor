//
// Created by koerriva on 2021/5/1.
//

#include "NodeEditor.h"
#define RAYGUI_IMPLEMENTATION
#include <raygui/raygui.h>

namespace GamePlay{
    static int nodeTypeActive=0;
    static int scrollIndex = 0;
    void NodeEditor::ShowAddMenu(Menu &menu) {
        if(menu.type!=MenuType::AddNode)return;
        float start_x = menu.position.x;
        float start_y = menu.position.y;
        float w = menu.rec.width;
        float h = menu.rec.height;
        GuiPanel(menu.rec);
        GuiSetFont(m_UiFont);

        nodeTypeActive = GuiToggleGroup(Rectangle{start_x+5,start_y+5,50,20},"输入;神经元;输出",nodeTypeActive);
        if(nodeTypeActive==0){
            Rectangle rec = {start_x+5,start_y+30,w-10,18};
            bool r0 = GuiButton(rec,"感光器");
            rec.y += 20;
            bool r1 = GuiButton(rec,"定位器");
            rec.y += 20;
            bool r2 = GuiButton(rec,"网络节点");
            rec.y += 20;
            bool r3 = GuiButton(rec,"文件节点");
            int category = -1;
            if(r0){
                TraceLog(LOG_INFO,"选择 感光器");
                category = 0;
            }
            if(r1){
                TraceLog(LOG_INFO,"选择 定位器");
                category = 1;
            }
            if(r2){
                TraceLog(LOG_INFO,"选择 网络节点");
                category = 2;
            }
            if(r3){
                TraceLog(LOG_INFO,"选择 文件节点");
                category = 3;
            }
            if(category!=-1){
                AddNode(UiNodeType::input,category);
                m_Editing = false;
                m_Menus.pop();
            }
        }
        if(nodeTypeActive==1){
            Rectangle rec = {start_x+5,start_y+30,w-10,18};
            bool r0 = GuiButton(rec,"通用节点");
            rec.y += 20;
            bool r1 = GuiButton(rec,"记忆体");
            rec.y += 20;
            bool r2 = GuiButton(rec,"异或节点");
            int category = -1;
            if(r0){
                TraceLog(LOG_INFO,"选择 通用节点");
                category = 0;
            }
            if(r1){
                TraceLog(LOG_INFO,"选择 记忆体");
                category = 1;
            }
            if(r2){
                TraceLog(LOG_INFO,"选择 异或节点");
                category = 2;
            }
            if(category!=-1){
                AddNode(UiNodeType::neural,category);
                m_Editing = false;
                m_Menus.pop();
            }
        }
        if(nodeTypeActive==2){
            Rectangle rec = {start_x+5,start_y+30,w-10,18};
            int category = -1;
            if(GuiButton(rec,"停止")){
                category = 0;
            }
            rec.y += 20;
            if(GuiButton(rec,"移动")){
                category = 1;
            }
            rec.y += 20;
            if(GuiButton(rec,"跳跃")){
                category = 2;
            }
            rec.y += 20;
            if(GuiButton(rec,"左转")){
                category = 3;
            }
            rec.y += 20;
            if(GuiButton(rec,"右转")){
                category = 4;
            }
            if(category!=-1){
                AddNode(UiNodeType::output,category);
                m_Editing = false;
                m_Menus.pop();
            }
        }
    }

    static bool neuralEditMode = false;
    void NodeEditor::ShowNeuralMenu(Menu& menu){
        if(menu.type!=MenuType::Neural)return;
        auto uiNode = m_UiNodes.find(menu.uiNode);
        if(uiNode->type!=UiNodeType::neural)return;
        auto neural = m_Nodes.find(menu.uiNode);

        Vector2 screen_pos = menu.position;
        float start_x = screen_pos.x;
        float start_y = screen_pos.y;
        float w = menu.rec.width;
        float h = menu.rec.height+10;

        bool windows = GuiWindowBox({ start_x, start_y, w, h },TextFormat("神经元 0x%X",uiNode));
        uiNode->editType = GuiToggleGroup({ start_x+5, start_y+20+7, 62, 20 }, "行为;外观", uiNode->editType);
        if(uiNode->editType==0){
            start_x +=5;start_y+=20+43;
            GuiLabel(Rectangle{ start_x, start_y, 53, 25 }, "阈值");
//            if (GuiSpinner(Rectangle{ start_x+89, start_y+20+43, 104, 25 }, nullptr, &neural->threshold, -100, 100, thresholdEditMode)){
//                thresholdEditMode = !thresholdEditMode;
//            }
            neural->threshold = GuiSlider({ start_x+89, start_y, 104, 25 },TextFormat("%d",neural->threshold),"",neural->threshold,-100,100);
            start_y += 45;
            neural->isLearn = GuiCheckBox({start_x,start_y,15,16},"学习",neural->isLearn);
            start_y += 30;
            GuiLabel(Rectangle{ start_x, start_y, 53, 25 }, "类型");
            if(GuiDropdownBox({start_x+89,start_y,104,25},"步进;线性;误差;S函数;",&neural->activeType, neuralEditMode)){
                TraceLog(LOG_INFO,"Dropdown Select");
                neuralEditMode = !neuralEditMode;
            }
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
        bool thresholdEditMode = true;

        bool windows = GuiWindowBox({ start_x, start_y, w, h },TextFormat("突触 0x%X",uiNode));
        uiNode->editType = GuiToggleGroup({ start_x+5, start_y+20+7, 62, 20 }, "行为;外观", uiNode->editType);
        if(uiNode->editType==0){
            start_x +=5;start_y+=20+43;
            GuiLabel({ start_x, start_y, 53, 25 }, "权重");
//            if (GuiSpinner(Rectangle{ start_x+89, start_y+20+43, 104, 25 }, nullptr, &nodeLink->weight, -100, 100, thresholdEditMode)){
//                thresholdEditMode = !thresholdEditMode;
//            }
            nodeLink->weight = GuiSlider({ start_x+89, start_y, 104, 25 },TextFormat("%d",nodeLink->weight),"",nodeLink->weight,-100,100);
            start_y+=35;
            GuiLabel(Rectangle{ start_x, start_y, 53, 25 }, "衰减");
            synapse->decay = GuiSlider({start_x+89, start_y, 104, 25}, TextFormat("%d",synapse->decay),"",synapse->decay,-100,100);
            start_y+=35;
            synapse->isLearn = GuiCheckBox({start_x, start_y, 15, 16}, "学习", synapse->isLearn);
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
        auto w = static_cast<float>(width);
        auto h = static_cast<float>(height);
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
}