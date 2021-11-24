//
// Created by koerriva on 2021/5/1.
//

#include "NodeEditor.h"

#define RAYGUI_IMPLEMENTATION

#include <raygui.h>

#define NK_INCLUDE_FIXED_TYPES
//#define NK_INCLUDE_STANDARD_IO
//#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
//#define NK_INCLUDE_DEFAULT_FONT
//#define NK_BUTTON_TRIGGER_ON_RELEASE
#define NK_INCLUDE_STANDARD_VARARGS
//
#define RAYLIB_NUKLEAR_IMPLEMENTATION

#include "raylib-nuklear.h"

namespace GamePlay {
    static struct nk_context *nkContext;

    void NodeEditor::InitGUI() {
        nkContext = InitNuklearEx(m_UiFont, 16.0f);
    }

    void NodeEditor::UpdateGUI() {
        UpdateNuklear(nkContext);
    }

    void NodeEditor::ShowAddMenu(Menu &menu) {
        if (menu.type != MenuType::AddNode)return;
        float start_x = menu.position.x;
        float start_y = menu.position.y;
        float w = menu.rec.width;
        float h = menu.rec.height;

        static int nodeTypeActive = 0;
        if (nk_begin(nkContext, "添加", nk_rect(start_x, start_y, w, h), NK_WINDOW_BORDER|NK_WINDOW_BACKGROUND)) {
            nk_layout_row_dynamic(nkContext, 0, 3);
            if (nk_select_label(nkContext, "输入", NK_TEXT_LEFT,nodeTypeActive == 0)) {
                nodeTypeActive = 0;
            }
            if (nk_select_label(nkContext, "神经元",NK_TEXT_LEFT,nodeTypeActive == 1)) {
                nodeTypeActive = 1;
            }
            if (nk_select_label(nkContext, "输出",NK_TEXT_LEFT,nodeTypeActive == 2)) {
                nodeTypeActive = 2;
            }

            nk_layout_row_dynamic(nkContext, h-60, 1);
            if (nodeTypeActive == 0) {
                int category = -1;
                if (nk_group_begin(nkContext, TextFormat("输入"), NK_WINDOW_BORDER)) {
                    nk_layout_row_dynamic(nkContext, 20, 1);
                    if (nk_button_label(nkContext,"感光器")){
                        category = 0;
                    }
                    if (nk_button_label(nkContext,"定位器")){
                        category = 1;
                    }
                    if (nk_button_label(nkContext,"网络节点")){
                        category = 2;
                    }
                    if (nk_button_label(nkContext,"文件节点")){
                        category = 3;
                    }

                    nk_group_end(nkContext);

                    if (category != -1) {
                        AddNode(UiNodeType::input, category);
                        m_Editing = false;
                        m_Menus.pop();
                    }
                }
            }
            if (nodeTypeActive == 1) {
                int category = -1;
                if (nk_group_begin(nkContext, TextFormat("神经元"), NK_WINDOW_BORDER)) {
                    nk_layout_row_dynamic(nkContext, 20, 1);
                    if (nk_button_label(nkContext,"通用")){
                        category = 0;
                    }
                    if (nk_button_label(nkContext,"记忆体")){
                        category = 1;
                    }
                    if (nk_button_label(nkContext,"异或")){
                        category = 2;
                    }

                    nk_group_end(nkContext);

                    if (category != -1) {
                        AddNode(UiNodeType::neural, category);
                        m_Editing = false;
                        m_Menus.pop();
                    }
                }
            }
            if (nodeTypeActive == 2){
                int category = -1;
                if (nk_group_begin(nkContext, TextFormat("输出"), NK_WINDOW_BORDER)) {
                    nk_layout_row_dynamic(nkContext, 20, 1);
                    if (nk_button_label(nkContext,"停止")){
                        category = 0;
                    }
                    if (nk_button_label(nkContext,"移动")){
                        category = 1;
                    }
                    if (nk_button_label(nkContext,"跳跃")){
                        category = 2;
                    }
                    if (nk_button_label(nkContext,"左转")){
                        category = 3;
                    }
                    if (nk_button_label(nkContext,"右转")){
                        category = 4;
                    }

                    nk_group_end(nkContext);

                    if (category != -1) {
                        AddNode(UiNodeType::output, category);
                        m_Editing = false;
                        m_Menus.pop();
                    }
                }
            }
        } else {
            m_Menus.pop();
            m_Editing = false;
        }
        nk_end(nkContext);
    }

    void NodeEditor::ShowInputMenu(Menu &menu) {
        if (menu.type != MenuType::Input)return;
        auto uiNode = m_UiNodes.find(menu.uiNode);
        if (uiNode->type != UiNodeType::input)return;
        auto input = m_Nodes.find(menu.uiNode);
        if (input->inputAction != 2)return;

        Vector2 screen_pos = menu.position;
        float start_x = screen_pos.x;
        float start_y = screen_pos.y;
        float w = menu.rec.width;
        float h = menu.rec.height + 10;

        static int selected = 0;
        if (nk_begin(nkContext, TextFormat("输入节点 0x%X", uiNode), nk_rect(start_x, start_y, w, h), NK_WINDOW_CLOSABLE)) {
            nk_layout_row_dynamic(nkContext, 0, 2);
            if (nk_select_label(nkContext, "行为", NK_TEXT_LEFT,uiNode->editType == BEHAVIOR)) {
                uiNode->editType = BEHAVIOR;
            }
            if (nk_select_label(nkContext, "外观",NK_TEXT_LEFT,uiNode->editType == APPEARANCE)) {
                uiNode->editType = APPEARANCE;
            }

            nk_layout_row_dynamic(nkContext, h-20, 1);
            if (uiNode->editType == BEHAVIOR) {
                if (nk_group_begin(nkContext, TextFormat("行为"), NK_WINDOW_BORDER)) {
                    nk_layout_row_begin(nkContext, NK_STATIC, 25, 2);
                    {
                        nk_layout_row_push(nkContext, w/4);
                        nk_label(nkContext, "在线节点", NK_TEXT_LEFT);
                        nk_layout_row_push(nkContext, w/2);
                        const char * list[] = {"--不连接--","node01", "node02", "node03", "node04"};

                        int curr_selected = nk_combo(nkContext, list, 5, selected, 20, {w/2, 20*5});
                        if(curr_selected==0){
                            input->inputPeer.status = 0;
                        }
                        if(curr_selected!=selected){
                            if(curr_selected==0){
                                TraceLog(LOG_INFO, "断开连接");
                            }else{
                                TraceLog(LOG_INFO, list[curr_selected]);
                                input->inputPeer.status = 0;
                                strcpy(input->inputPeer.sub_topic, list[curr_selected]);
                                printf("change topic : %s\n", input->inputPeer.sub_topic);
                                Peer *peer = static_cast<Peer *>(input->inputPeer.peer);
                                peer->Sub(input->inputPeer.sub_topic);
                                input->inputPeer.status = 1;
                            }
                        }

                        selected = curr_selected;
                    }
                    nk_layout_row_end(nkContext);

                    nk_group_end(nkContext);
                }
            }
            if (uiNode->editType == APPEARANCE) {
                if (nk_group_begin(nkContext, TextFormat("外观"), NK_WINDOW_BORDER)) {
                    nk_group_end(nkContext);
                }
            }
        } else {
            m_Menus.pop();
            m_Editing = false;
        }
        nk_end(nkContext);
    }

    void NodeEditor::ShowNeuralMenu(Menu &menu) {
        if (menu.type != MenuType::Neural)return;
        auto uiNode = m_UiNodes.find(menu.uiNode);
        if (uiNode->type != UiNodeType::neural)return;
        auto neural = m_Nodes.find(menu.uiNode);

        Vector2 screen_pos = menu.position;
        float start_x = screen_pos.x;
        float start_y = screen_pos.y;
        float w = menu.rec.width;
        float h = menu.rec.height + 10;

        static int selected = 1;
        if (nk_begin(nkContext, TextFormat("神经元 0x%X", uiNode), nk_rect(start_x, start_y, w, h), NK_WINDOW_CLOSABLE)) {
            nk_layout_row_dynamic(nkContext, 0, 2);
            if (nk_select_label(nkContext, "行为", NK_TEXT_LEFT,uiNode->editType == BEHAVIOR)) {
                uiNode->editType = BEHAVIOR;
            }
            if (nk_select_label(nkContext, "外观",NK_TEXT_LEFT,uiNode->editType == APPEARANCE)) {
                uiNode->editType = APPEARANCE;
            }

            nk_layout_row_dynamic(nkContext, h-20, 1);
            if (uiNode->editType == BEHAVIOR) {
                if (nk_group_begin(nkContext, TextFormat("行为"), NK_WINDOW_BORDER)) {
                    nk_layout_row_begin(nkContext, NK_STATIC, 25, 2);
                    {
                        nk_layout_row_push(nkContext, w/4);
                        nk_label(nkContext, "阈值", NK_TEXT_LEFT);
                        nk_layout_row_push(nkContext, w/2);
                        nk_slider_int(nkContext, -100, &neural->threshold, 100, 1);

                        nk_layout_row_push(nkContext, w/4);
                        if(nk_checkbox_label(nkContext,"学习",&selected)){
                            neural->isLearn = selected==0;
                        }
                        if(neural->isLearn){
                            nk_layout_row_push(nkContext, w/2);
                            const char * list[] = {"步进", "线性", "误差", "S函数"};
                            neural->activeType = nk_combo(nkContext, list, 4, neural->activeType, 20, {w/2, 20*5});
                        }
                    }
                    nk_layout_row_end(nkContext);

                    nk_group_end(nkContext);
                }
            }
            if (uiNode->editType == APPEARANCE) {
                if (nk_group_begin(nkContext, TextFormat("外观"), NK_WINDOW_BORDER)) {
                    nk_layout_row_begin(nkContext, NK_STATIC, 50, 2);
                    {
                        nk_layout_row_push(nkContext, w/4);
                        nk_label(nkContext, "激活", NK_TEXT_LEFT);
                        nk_layout_row_push(nkContext, w/2);
                        Color color = uiNode->colors[1];
                        nk_colorf active_color = nk_color_cf({color.r,color.g,color.b,color.a});
                        active_color = nk_color_picker(nkContext, active_color, NK_RGBA);
                        uiNode->colors[1].r = active_color.r*255;
                        uiNode->colors[1].g = active_color.g*255;
                        uiNode->colors[1].b = active_color.b*255;
                        uiNode->colors[1].a = active_color.a*255;
                    }
                    nk_layout_row_end(nkContext);

                    nk_layout_row_begin(nkContext, NK_STATIC, 50, 2);
                    {
                        nk_layout_row_push(nkContext, w/4);
                        nk_label(nkContext, "未激活", NK_TEXT_LEFT);
                        nk_layout_row_push(nkContext, w/2);
                        Color color = uiNode->colors[0];
                        nk_colorf active_color = nk_color_cf({color.r,color.g,color.b,color.a});
                        active_color = nk_color_picker(nkContext, active_color, NK_RGBA);
                        uiNode->colors[0].r = active_color.r*255;
                        uiNode->colors[0].g = active_color.g*255;
                        uiNode->colors[0].b = active_color.b*255;
                        uiNode->colors[0].a = active_color.a*255;
                    }
                    nk_layout_row_end(nkContext);

                    nk_group_end(nkContext);
                }
            }
        } else {
            m_Menus.pop();
            m_Editing = false;
        }
        nk_end(nkContext);
    }

    void NodeEditor::ShowSynapseMenu(Menu &menu) {
        if (menu.type != MenuType::Synapse)return;
        auto uiNode = m_UiNodes.find(menu.uiNode);
        if (uiNode->type != UiNodeType::synapse)return;
        auto synapse = m_Nodes.find(menu.uiNode);
        auto nodeLink = m_NodeLinks.find(uiNode->linkId);

        Vector2 screen_pos = menu.position;
        float start_x = screen_pos.x;
        float start_y = screen_pos.y;
        float w = menu.rec.width;
        float h = menu.rec.height;
        bool thresholdEditMode = true;

        bool windows = GuiWindowBox({start_x, start_y, w, h}, TextFormat("突触 0x%X", uiNode));
        uiNode->editType = GuiToggleGroup({start_x + 5, start_y + 20 + 7, 62, 20}, "行为;外观", uiNode->editType);
        if (uiNode->editType == 0) {
            start_x += 5;
            start_y += 20 + 43;
            GuiLabel({start_x, start_y, 53, 25}, "权重");
//            if (GuiSpinner(Rectangle{ start_x+89, start_y+20+43, 104, 25 }, nullptr, &nodeLink->weight, -100, 100, thresholdEditMode)){
//                thresholdEditMode = !thresholdEditMode;
//            }
            nodeLink->weight = GuiSlider({start_x + 89, start_y, 104, 25}, TextFormat("%d", nodeLink->weight), "",
                                         nodeLink->weight, -100, 100);
            start_y += 35;
            GuiLabel(Rectangle{start_x, start_y, 53, 25}, "衰减");
            synapse->decay = GuiSlider({start_x + 89, start_y, 104, 25}, TextFormat("%d", synapse->decay), "",
                                       synapse->decay, -100, 100);
            start_y += 35;
            synapse->isLearn = GuiCheckBox({start_x, start_y, 15, 16}, "学习", synapse->isLearn);
        }

        if (uiNode->editType == 1) {
            Rectangle bounds = {start_x + 5, start_y + 20 + 43, 128, 25};
            int state = synapse->isActive ? 1 : 0;
            bool r = GuiDropdownBox(bounds, "未激活;激活", &uiNode->editColorType, uiNode->editColorMode);
            if (r) {
                uiNode->editColorMode = !uiNode->editColorMode;
            }
            int padding = 0;
            if (uiNode->editColorMode) {
                padding = 70;
            }
            uiNode->colors[state] = GuiColorPicker({start_x + 5, start_y + 20 + 43 + 25 + padding, 128, 95},
                                                   uiNode->colors[state]);
        }
        if (windows) {
            m_Menus.pop();
            m_Editing = false;
        }
    }

    void NodeEditor::ShowStatusBar() {
        auto w = static_cast<float>(width);
        auto h = static_cast<float>(height);
        float stride = 32;

        if (nk_begin(nkContext, "状态栏", nk_rect(0, h - stride, w, stride),
                     NK_WINDOW_BORDER | NK_WINDOW_NO_INPUT | NK_WINDOW_NO_SCROLLBAR | NK_WINDOW_BACKGROUND)) {
            nk_layout_row_dynamic(nkContext, stride, 5);
            const char *opStatusText = "";
            if (selected > 0) {
                auto uiNode = m_UiNodes.find(selected);
                auto node = m_Nodes.find(selected);
                if (node->type == NodeType::Input) {
                    opStatusText = TextFormat("I:%d, S:%d", uiNode->id, node->value);
                } else if (node->type == NodeType::Neural) {
                    opStatusText = TextFormat("N:%d, S:%d, T:%d", uiNode->id, node->value, node->threshold);
                } else if (node->type == NodeType::Output) {
                    opStatusText = TextFormat("O:%d, S:%d", uiNode->id, node->value);
                } else if (node->type == NodeType::Synapse) {
                    auto nodeLink = m_NodeLinks.find(uiNode->linkId);
                    opStatusText = TextFormat("SY:%d, W:%d", uiNode->id, nodeLink->weight);
                } else {
                    opStatusText = TextFormat("N:%d, T:%d", uiNode->id, uiNode->type);
                }
            }else{
                opStatusText = TextFormat("--");
            }
            nk_label_colored(nkContext, opStatusText, NK_TEXT_ALIGN_CENTERED, nk_color({255, 255, 255, 255}));
            nk_label_colored(nkContext, TextFormat("输入数\t%d", m_InputNum), NK_TEXT_ALIGN_CENTERED,
                             nk_color({255, 123, 123, 255}));
            nk_label_colored(nkContext, TextFormat("神经元数\t%d", m_NeuralNum), NK_TEXT_ALIGN_CENTERED,
                             nk_color({123, 255, 123, 255}));
            nk_label_colored(nkContext, TextFormat("输出数\t%d", m_OutputNum), NK_TEXT_ALIGN_CENTERED,
                             nk_color({123, 123, 255, 255}));
            nk_label_colored(nkContext, TextFormat("连接数\t%d", m_UiLinks.size()), NK_TEXT_ALIGN_CENTERED,
                             nk_color({123, 123, 123, 255}));
        }
        nk_end(nkContext);
    }

    void NodeEditor::DrawGUI() {
        DrawNuklear(nkContext);
    }
}