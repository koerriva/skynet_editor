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

#define MAX_LIGHTS 4
#include "graphics/lighting.h"

#define DARKRED ColorFromHSV(0,1.0,0.5)
#define DARKYELLOW ColorFromHSV(58,0.8,0.5)
#define SHENHAILV Color{26, 59, 50, 255}

namespace GamePlay{
    class NodeEditor {
    public:
        void Load(Font font);
        void Render();
        void RenderGUI();
        void Update();
        void Save();

        void Run();

        int GetNodeCount() { return m_UiNodes.size();}
    private:
        void AddNode(UiNodeType type,int category);
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
        void RayMarching();
        void DrawViewport(Viewport& viewport);

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

        static Light CreateLight(int type, Vector3 position, Vector3 target, Color color, Shader shader);
        static void UpdateLightValues(Shader shader, Light light);
        static Material LoadMaterialPBR(Color albedo, float metalness, float roughness);
        static Material LoadMaterialPhong(Color ambient,Color diffuse,Color specular);
    private:
        void Init2D();
        void Init3D();
        void Update2D();
        void Update3D();
        void Render2D(Viewport& viewport);
        void Render3D(Viewport& viewport);
    private:
        void UpdateBug();
        void StopBug(ActionSignal actionSignal);
        void MoveBug(ActionSignal actionSignal);
        void JumpBug(ActionSignal actionSignal);
        void TurnBug(ActionSignal actionSignal);
        void PlayBugAnimation(Animation& animation);
    private:
        int selected=0;
        Vector2 selected_point;
        Vector2 drag_point;
        int m_Hovering=0;

        int width=1440,height=900;

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
        Camera m_Camera3dShadowMap;
        RenderTexture m_ShadowMapRenderTexture;
        Vector2  m_MousePosition;
        Font m_UiFont;
        bool editorMode = true;
        bool fullMode = false;
        Viewport m_MainCanvas;
        Viewport m_2dCanvas;
        Viewport m_3dCanvas;

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
        Texture2D m_OutputIcons[5];
        Shader m_BlankShader;
        Shader m_BaseLightingShader;
        Shader m_RayMarchShader;
        Light m_SunLight;
        Vector3 m_SunLightDir;

        Model m_Playground;
        Model m_Bug;
        Vector3 m_BugPosition;
        Vector3 m_BugDirection = {0,0,1};
        Vector3 m_BugVelocity = {0,0,0};
        float m_BugRotation=0.0f;
        bool m_BugStop = false;
        //jump
        bool m_BugJumping = false;
        bool m_BugJumpingUp = false;
        bool m_BugJumpingDown = false;

        std::vector<Animation> m_BugAnimation;
        std::vector<ActionSignal> m_BugSignal;
    };
}


#endif //SKYNET_EDITOR_NODEEDITOR_H
