//
// Created by 50409 on 2021/4/24.
//

#ifndef SKYNET_EDITOR_NODEEDITOR_H
#define SKYNET_EDITOR_NODEEDITOR_H

#include "atomic"
#include "thread"
#include "vector"
#include "unordered_map"
#include "graph.h"
#define BEZIER_LINE_DIVISIONS 32
#include "raylib.h"
#include "node.h"
#include "raymath.h"

#define DARKRED ColorFromHSV(0,1.0,0.5)
#define DARKYELLOW ColorFromHSV(58,0.8,0.5)

namespace GamePlay{
    struct Light{
        int enabled;
        int type;
        Vector3 position;
        Vector3 target;
        Color color;
    };

    class NodeEditor {
    public:
        void Load(Font font);
        void Render2D();
        void Render3D();
        void RenderGUI();
        void Update();
        void Save();

        void Run();

        int GetNodeCount() { return m_UiNodes.size();}
    private:
        void AddNode(UiNodeType type);
        void LinkNode(int from,int to);
        void DelNode();
        void UnLinkNode(int from,int to);

        void ShowAddMenu(Menu& menu);
        void ShowNeuralMenu(Menu& menu);
        void ShowSynapseMenu(Menu& menu);
        void ShowStatusBar();

        void DrawBgGrid();
        void DrawNode(const UiNode& uiNode);
        void DrawLink(const UiLink& uiLink);
        void DrawLight();

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

        static Material LoadMaterialPBR(Color albedo, float metalness, float roughness);

    private:
        void MoveBug(ActionSignal actionSignal){
            if(actionSignal.type==1&&!m_BugStop){
                float speed = 0.02f;
                m_BugPosition  = Vector3Add(m_BugPosition,Vector3Multiply(m_BugDirection,{speed,0,speed}));
            }
        }
        void TurnBug(ActionSignal actionSignal){
            float angle = 0;
            float unitDeg = 1.0f/10.f;
            if(actionSignal.type==3&&!m_BugStop){
                //左转
                angle = -unitDeg*PI/180.f;
            }
            if(actionSignal.type==4&&!m_BugStop){
                //右转
                angle = unitDeg*PI/180.f;
            }
            m_BugRotation += angle;
            m_BugDirection = Vector3Transform(m_BugDirection,MatrixRotateY(angle));
        }
        void StopBug(ActionSignal actionSignal){
            if(actionSignal.type==0){
                m_BugStop = true;
            }
        }

        void PlayBugAnimation(Animation& animation){
            animation.frameCounter++;
            UpdateModelAnimation(m_Bug,animation.data,animation.frameCounter);
            if (animation.frameCounter >= animation.data.frameCount) {
                animation.frameCounter = 0;
            }
        }
    private:
        int selected=0;
        Vector2 selected_point;
        int m_Hovering=0;

        int width=800,height=600;

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
        Camera m_Camera3d;
        RenderTexture2D m_RenderTarget;
        Vector2 m_TargetSize = {400,300};
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

        Texture2D m_Icons;
        Shader m_LightingShader;
        Texture2D m_LightingTexture;
        Shader m_BaseShader;
        Model m_Playground;
        Model m_Bug;
        Vector3 m_BugPosition;
        Vector3 m_BugDirection = {0,0,1};
        float m_BugRotation=0.0f;
        bool m_BugStop = false;
        Texture2D m_BugActionIcons[5];
        std::vector<Animation> m_BugAnimation;
        std::vector<ActionSignal> m_BugSignal;
    };
}


#endif //SKYNET_EDITOR_NODEEDITOR_H
