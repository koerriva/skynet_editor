//
// Created by 50409 on 2021/4/24.
//

#ifndef SKYNET_EDITOR_NODEEDITOR_H
#define SKYNET_EDITOR_NODEEDITOR_H

#include "vector"
#include "unordered_map"
#include "raylib.h"
#include "graph.h"

namespace GamePlay{
    //logic
    enum class NodeType{
        Input,Pin,Synapse,Neural,Output,Node
    };
    struct Node{
        NodeType type;
        int value = 0;
        int weight = 100;
        bool isActive = false;
        Node():type(NodeType::Node){}
        explicit Node(NodeType t):type(t){}
    };
    struct NodeLink{
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

        UiNode(){}
        UiNode(UiNodeType t,int id):type(t),id(id){}
    };
    struct UiLink{
        int id;
        int form;
        int to;
    };

    class NodeEditor {
    public:
        void Load(Camera2D camera);
        void Show();
        void Update();
        void Save();

        int GetNodeCount() { return m_UiNodes.size();}
    private:
        void AddNode();
        void LinkNode(int form,int to);
        void DelNode();

        void DrawGrid();
        void DrawNode(const UiNode& uiNode);
        void DrawLink(const UiLink& uiLink);
    private:
        unsigned int selected=0;
        unsigned int hovering=0;

        IdMap<UiNode> m_UiNodes;
        IdMap<Node> m_Nodes;
        IdMap<UiLink> m_UiLinks;
        unsigned int m_UiNodeUniqueId=1;

        Texture2D m_NeuralTexture;
        int m_WorldWidth = 4000;
        int m_WorldHeight = 4000;
        Camera2D m_Camera;
        Vector2  m_MousePosition;
        bool m_Linking = false;
    };

    static bool IsInside(float r,Vector2 center, Vector2 pos){
        float x = pos.x - center.x;
        float y = pos.y - center.y;
        return r*r>x*x+y*y;
    }
}


#endif //SKYNET_EDITOR_NODEEDITOR_H
