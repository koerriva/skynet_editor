//
// Created by koerriva on 2021/4/26.
//

#ifndef SKYNET_EDITOR_NODE_H
#define SKYNET_EDITOR_NODE_H

#include <easings.h>

namespace GamePlay{
    enum class NodeType{
        Input,Pin,Synapse,Neural,Output,Node
    };
    struct Node{
        NodeType type;
        int value = 0;
        int threshold = 50;
        int t=0;
        bool isLearn = false;
        bool isActive = false;
        Node():type(NodeType::Node){}
        explicit Node(NodeType t):type(t){}
    };
    struct NodeLink{
        int id;
        int from;
        int to;
        int weight;
        int type;//14 input->neural,44 neural->neural,45 neural->output
    };
    struct NodeSignal{
        int from=0;
        int t=0;
        int value=0;
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
        Vector2 pinPosition={};
        int children[8] = {};
        int linkId;

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
        AddNode,Input,Neural,Output,Synapse
    };
    struct Menu{
        MenuType type;
        Vector2 position;
        Rectangle rec;
        int uiNode;
    };

    inline static bool IsInside(float r,Vector2 center, Vector2 pos){
        float x = pos.x - center.x;
        float y = pos.y - center.y;
        return r*r>x*x+y*y;
    }

    inline static bool IsInsideInner(float r0,Vector2 center, float r1,Vector2 pos){
        float x = pos.x - center.x;
        float y = pos.y - center.y;
        return (r0-r1)*(r0-r1)>x*x+y*y;
    }

    inline static bool IsInsideEdge(float r,Vector2 center, Vector2 pos){
        float x = pos.x - center.x;
        float y = pos.y - center.y;
        float a = x*x+y*y;
        return r*r>a&&a>=(r-5)*(r-5);
    }

    inline static bool IsInRect(Rectangle rec,Vector2 pos){
        return pos.x>rec.x&&pos.x<rec.x+rec.width
               && pos.y>rec.y&&pos.y<rec.y+rec.height;
    }

    inline static float easeCubicInOut(float t, float b, float c, float d)
    {
        if ((t/=d*0.5f) < 1.0f) return (c*0.5f*t*t*t + b);
        t -= 2.0f; return (c*0.5f*(t*t*t + 2.0f) + b);
    }

    static void DrawMyBezierLine(Vector2 startPos,Vector2 endPos,float thick,Color color){
        Vector2 previous = startPos;
        Vector2 current;

        for (int i = 1; i <= BEZIER_LINE_DIVISIONS; i++)
        {
            // Cubic easing in-out
            // NOTE: Easing is calculated only for y position value
            current.y = easeCubicInOut((float)i, startPos.y, endPos.y - startPos.y, (float)BEZIER_LINE_DIVISIONS);
//            current.x = EaseCubicInOut((float)i, startPos.x, endPos.x - startPos.x, (float)BEZIER_LINE_DIVISIONS);
            current.x = previous.x + (endPos.x - startPos.x)/ (float)BEZIER_LINE_DIVISIONS;

            DrawLineEx(previous, current, thick, color);
            previous = current;
        }
    }
}
#endif //SKYNET_EDITOR_NODE_H
