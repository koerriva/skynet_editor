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
        Input,Synapse,Neural,Output,Value
    };
    struct Node{
        NodeType type;
        int value = 0;
        int weight = 100;
        bool isActive = false;
        Node():type(NodeType::Value){}
        explicit Node(NodeType t):type(t){}
    };
    struct NodeLink{
        unsigned int form;
        unsigned int to;
    };

    //ui
    enum class UiNodeType
    {
        input,
        neural,
        synapse,
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
        unsigned int form;
        unsigned int to;
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
        void LinkNode();
        void DelNode();

        void DrawGrid();
        void DrawNode(const UiNode& uiNode);
    private:
        int selected=-1;

        IdMap<UiNode> m_UiNodes;
        IdMap<Node> m_Nodes;
        IdMap<UiLink> m_UiLinks;
        unsigned int m_UiNodeUniqueId=1;

        Texture2D m_NeuralTexture;
        int m_WorldWidth = 4000;
        int m_WorldHeight = 4000;
        Camera2D m_Camera;
        Vector2  m_MousePosition;
    };

    static bool IsInside(float r,Vector2 center, Vector2 pos){
        float x = pos.x - center.x;
        float y = pos.y - center.y;
        return r*r>x*x+y*y;
    }
}


#endif //SKYNET_EDITOR_NODEEDITOR_H