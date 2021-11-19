//
// Created by koerriva on 2021/4/26.
//

#ifndef SKYNET_EDITOR_NODE_H
#define SKYNET_EDITOR_NODE_H

#include <easings.h>
#include <glm/glm.hpp>
#define PEER_TOPIC_MAX_LEN 45
using namespace glm;
namespace GamePlay{
    enum class NodeType{
        Input,Pin,Synapse,Neural,Output,Node
    };
    struct NodePeer{
        void* peer = nullptr;//Peer
        char sub_topic[PEER_TOPIC_MAX_LEN] = {0};
        int status = 0;//0未就绪,1已就绪
    };
    struct NodeFile{
        void* handle;
    };
    struct Node{
        NodeType type;
        int value = 0;
        int threshold = 50;
        int t=0;
        bool isLearn = false;
        bool isActive = false;

        //input
        int inputFrequency = 11;
        int inputAction = 0;//1视觉遮挡,0视觉正常,2网络节点
        NodePeer inputPeer{};
        //output
        int outputAction=0;
        //synapse
        int decay=0;
        int link=0;
        //neural
        int activeType=0;//0step,1linear,2error cals,3sigmoid

        Node():type(NodeType::Node){}
        explicit Node(NodeType t):type(t){}
    };
    struct NodeLink{
        int id=0;
        int from=0;
        int to=0;
        int weight=0;
        int type=0;//14 input->neural,44 neural->neural,45 neural->output
        bool circle = false;
    };
    struct NodeSignal{
        int from=0;
        int t=0;
        int value=0;
    };
    struct ActionSignal{
        int type = 0;//0停止,1移动,2跳跃,3左转,4右转
        int value = 0;
    };

    //render
    struct Animation{
        ModelAnimation data;
        int frameCounter=0;

        bool isOver() const{return frameCounter>=data.frameCount;}
        void reset() {frameCounter=0;}
    };

    //ui
    enum class UiNodeType
    {
        input,
        neural,
        pin,
        synapse,
        node,
        output,
        viewport
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
        int linkFrom=0;
        int linkId=0;

        UiNode() = default;
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
    struct Viewport{
        int id;
        Rectangle rec{};
        Rectangle source{};
        RenderTexture2D framebuffer{};
        Viewport(){};
        Viewport(int width,int height){
            id = 0;
            source = {0,0,static_cast<float>(width),static_cast<float>(-height)};
            rec = {0,0,static_cast<float>(width),static_cast<float>(height)};
            framebuffer = LoadRenderTexture(width,height);
            SetTextureWrap(framebuffer.texture,TEXTURE_WRAP_CLAMP);
        }
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

//    static vec2 smooth3(float t,vec2 p0,vec2 p1,vec2 p2,vec2 p3){
//        float x = (1-t)*(1-t)*(1-t)*p0.x+3*(1-t)*(1-t)*t*p1.x+3*(1-t)*t*t*p2.x+t*t*t*p3.x;
//        float y = (1-t)*(1-t)*(1-t)*p0.y+3*(1-t)*(1-t)*t*p1.y+3*(1-t)*t*t*p2.y+t*t*t*p3.y;
//        return vec2(x,y);
//    }

//    static void DrawMyBezierLine(Vector2 startPos,Vector2 startPinPos,Vector2 endPos,Vector2 endPinPos,float thick,Color color){
//        vec2 p0 = {startPos.x,startPos.y};
//        vec2 p3 = {endPos.x,endPos.y};
//
//        float length = glm::length(p3-p0);
////        float start_k = (startPinPos.y-startPinPos.y)/(startPinPos.x-startPos.x);
////        float end_k = (endPinPos.y-endPos.y)/(endPinPos.x-endPos.x);
//        float k1,k2;
//        if(startPinPos.x<startPos.x){
//            k1=-0.2;
//        }else if(startPinPos.x==startPos.x){
//            k1 = 0;
//        }else{
//            k1=0.2;
//        }
//
//        if(startPinPos.y<startPos.y){
//            k2 = -0.2f;
//        }else if(startPos.y==startPinPos.y){
//            k2 = 0;
//        }else{
//            k2 = 0.2f;
//        }
//
//        vec2 offset = {k1*length,k2*length};
//        vec2 p1 = p0+offset;
//        vec2 p2 = p3-offset;
//
//        std::vector<Vector2> points;
//        points.push_back(startPos);
//        for (int i = 0; i < BEZIER_LINE_DIVISIONS; i++)
//        {
//            float t = float(i)/BEZIER_LINE_DIVISIONS;
//            vec2 p = smooth3(t,p0,p1,p2,p3);
//            points[i] = {p.x,p.y};
//            points.push_back({p.x,p.y});
//        }
//        points.push_back(endPos);
//        DrawLineStrip(points.data(),points.size(),color);
//    }

    static void DrawMyBezierLine(Vector2 startPos,Vector2 startPinPos,Vector2 endPos,Vector2 endPinPos,float thick,Color color){
        DrawLineEx(startPos,startPinPos,thick,color);
        DrawCircleV(startPinPos,thick*0.6f,color);
        DrawLineEx(startPinPos,endPinPos,thick,color);
        DrawCircleV(endPinPos,thick*0.6f,color);
        DrawLineEx(endPinPos,endPos,thick,color);
    }

    //bug controller


}
#endif //SKYNET_EDITOR_NODE_H
