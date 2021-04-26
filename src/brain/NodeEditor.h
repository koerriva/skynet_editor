//
// Created by 50409 on 2021/4/24.
//

#ifndef SKYNET_EDITOR_NODEEDITOR_H
#define SKYNET_EDITOR_NODEEDITOR_H

#include "thread"
#include "vector"
#include "unordered_map"
#include "graph.h"
#include "raylib.h"

namespace GamePlay{
    //logic
    enum class NodeType{
        Input,Pin,Synapse,Neural,Output,Node
    };
    struct Node{
        NodeType type;
        int value = 0;
        int weight = 100;
        bool isLearn = false;
        bool isActive = false;
        Node():type(NodeType::Node){}
        explicit Node(NodeType t):type(t){}
    };
    struct NodeLink{
        int id;
        int form;
        int to;
    };

    //ui
    enum class UiNodeType
    {
        input,
        neural,
        pin,
        synapse,
        node,
        output
    };
    struct UiNode
    {
        UiNodeType type;
        int id;
        Vector2 position;
        float radius;
        bool cursorIn = false;
        bool cursorOut = false;
        //editor
        int editType = 0;
        int editColorType = 0;
        bool editColorMode = false;
        Color colors[2] = {DARKGREEN,GREEN};

        int parent;
        int children[8] = {};

        UiNode(){}
        UiNode(UiNodeType t,int id):type(t),id(id){}

        void CursorIn(){ cursorIn = true;cursorOut= false;}
        void CursorOut() {cursorIn = false;cursorOut = true;}
    };
    struct UiLink{
        int id;
        int form;
        int to;
    };
    enum class MenuType{
        AddNode,Input,Neural,Output
    };
    struct Menu{
        MenuType type;
        Vector2 position;
        Rectangle rec;
        int uiNode;
    };

    class NodeEditor {
    public:
        void Load(Camera2D camera,Font font);
        void Show();
        void ShowMenu();
        void Update();
        void Save();

        void Run(){
            TraceLog(LOG_INFO,"running...");
            for(auto node:inputs){
                auto in = m_Nodes.find(node);
                TraceLog(LOG_INFO,TextFormat("%d",in->isActive));
                in->isActive = !in->isActive;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }

        int GetNodeCount() { return m_UiNodes.size();}
    private:
        void AddNode(UiNodeType type);
        void LinkNode(int from,int to);
        void DelNode();

        void ShowAddMenu(Menu& menu);
        void ShowNodeMenu(Menu& menu);

        void DrawGrid();
        void DrawNode(const UiNode& uiNode);
        void DrawLink(const UiLink& uiLink);

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
    private:
        int selected=0;
        Vector2 selected_point;
        int m_Hovering=0;

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
    };

    static bool IsInside(float r,Vector2 center, Vector2 pos){
        float x = pos.x - center.x;
        float y = pos.y - center.y;
        return r*r>x*x+y*y;
    }

    static bool IsInsideInner(float r0,Vector2 center, float r1,Vector2 pos){
        float x = pos.x - center.x;
        float y = pos.y - center.y;
        return (r0-r1)*(r0-r1)>x*x+y*y;
    }

    static bool IsInsideEdge(float r,Vector2 center, Vector2 pos){
        float x = pos.x - center.x;
        float y = pos.y - center.y;
        float a = x*x+y*y;
        return r*r>a&&a>=(r-5)*(r-5);
    }

    static bool IsInRect(Rectangle rec,Vector2 pos){
        return pos.x>rec.x&&pos.x<rec.x+rec.width
        && pos.y>rec.y&&pos.y<rec.y+rec.height;
    }
}


#endif //SKYNET_EDITOR_NODEEDITOR_H
