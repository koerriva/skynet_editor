#ifndef MYGAME_H
#define MYGAME_H
#include <easings.h>
#include <raylib.h>
#include <glm/glm.hpp>
#include <iostream>
#include "myinput.h"

#ifndef BEZIER_LINE_DIVISIONS
#define BEZIER_LINE_DIVISIONS 24
#endif

#ifndef MAX_NEURAL_SIZE
#define MAX_NEURAL_SIZE 10000
#endif

#ifndef MAX_NEURAL_SYNAPSE_SIZE
#define MAX_NEURAL_SYNAPSE_SIZE 10
#endif

#ifndef MAX_NEURAL_LINK_SIZE
#define MAX_NEURAL_LINK_SIZE MAX_NEURAL_SIZE * MAX_NEURAL_SYNAPSE_SIZE
#endif

struct Neural{
    Vector2 center;
    Color color;
    Color colors[2] = {DARKGREEN,GREEN};
    bool isActive;
    float radius;
    int weight;
    bool isLearn;

    Neural* in[MAX_NEURAL_SYNAPSE_SIZE];
    int in_count;
    Neural* out[MAX_NEURAL_SYNAPSE_SIZE];
    int out_count;

    void Active(){
        isActive = true;
        color = colors[1];
    }

    void DeActive(){
        isActive = false;
        color = colors[0];
    }

    void Link(Neural* neural){
        if(out_count<MAX_NEURAL_SYNAPSE_SIZE){
            out[out_count++] = neural;
            neural->LinkIn(this);
        }
    }

private:
    void LinkIn(Neural* neural){
        if(in_count<MAX_NEURAL_SYNAPSE_SIZE){
            in[in_count++] = neural;
        }
    }
};

enum NeuralLinkState{
    UNLINK,BEGIN,END
};

struct NeuralNetwork{
    Neural* neurals;
    int neural_count;

    void Init(){
        neural_count = 0;
    }

    void AddNeural(Neural neural){
        neurals[neural_count++] = neural;
    }

    void DelNeural(Neural* pNeural){
        size_t idx = pNeural-neurals;
        std::cout << "idx" << idx << std::endl;
        neurals[idx] = Neural{};
        if(idx==0){
            neural_count = 0;
        }else{
            neural_count -=1;
        }
    }
};

enum PlayerAction{
    Idle,MoveScene,AddNode,EditNode,MoveNode,LinkNode,DelNode
};


enum CursorState{
    OnGround,InNode,InNodeInner,InNodeEdge
};

using namespace glm;

static void getBezierPoints2D(Vector2 startPos,Vector2 endPos,Vector2* points){
    Vector2 previous = startPos;
    Vector2 current;

    points[0]=previous;
    for (int i = 1; i <= BEZIER_LINE_DIVISIONS; i++)
    {
        // Cubic easing in-out
        // NOTE: Easing is calculated only for y position value
        current.y = EaseCubicInOut((float)i, startPos.y, endPos.y - startPos.y, (float)BEZIER_LINE_DIVISIONS);
        current.x = previous.x + (endPos.x - startPos.x)/ (float)BEZIER_LINE_DIVISIONS;

//        DrawLineEx(previous, current, thick, color);
        points[i]=current;

        previous = current;
    }
}

struct MyGame{
    int width = 1280;
    int height=720;
    int world_width = 4000;
    int world_height = height*world_width/width;
    float radius = 32;
    Font font = {};
    Texture icons = {};
    Camera2D camera = {};
    Texture2D neural_texture = {};

    CursorState cursorState = CursorState::OnGround;

    Neural* selected = nullptr;
    Neural* picked = nullptr;
    Neural* linkFrom = nullptr;
    Neural* linkTo = nullptr;
    NeuralLinkState linkState = UNLINK;

    Neural* editNode = nullptr;
    int editType=0;
    int editActiveColorType = 0;
    bool editActiveColorMode = false;
    PlayerAction action = PlayerAction::Idle;;

    InputManager im={};

    NeuralNetwork nn{};

    void Init(){
        //基本汉字 4E00-9FA5
        int charsCount = 0xFFF0+1;
        int* fontChars = (int*)MemAlloc(sizeof(int)*charsCount);
        TraceLog(LOG_INFO,"Init SC Font Memory");
        for (int i = 0; i < charsCount; ++i) {
            fontChars[i] = i;
        }
        TraceLog(LOG_INFO,"Load SC Font");
//    Font scFont = LoadFontEx("data/font/NotoSansSC-Regular.otf",16,fontChars,charsCount);
        font = LoadFontEx("data/font/LiHeiPro.ttf",16,fontChars,charsCount);
        TraceLog(LOG_INFO,"SC Font Load");
        MemFree(fontChars);

        camera.zoom = 1.0f;
        camera.target = {0};
        camera.offset = {(float)width/2,(float)height/2};
        camera.rotation = 0;

        icons = LoadTexture("data/icons.png");
        neural_texture = LoadTexture("data/neural.png");

        nn.Init();
        nn.neurals = (Neural*)MemAlloc(sizeof(Neural)*MAX_NEURAL_SIZE);
    }

    void Input(){
        im.Clean();
        camera.rotation = 0;
        im.Update(camera,GetFrameTime());
        if(im.mouse.MB_SCROLL){
            camera.zoom += im.mouse.Y*GetFrameTime()*10;
            camera.zoom = Clamp(camera.zoom,1,5);
        }
    }

    void Update(){
        for (size_t i = 0; i < nn.neural_count; i++)
        {
            nn.neurals[i].DeActive();

            Neural* pN = &nn.neurals[i];
            if(IsInside(pN->radius,pN->center,im.mouse.world_pos)){
                cursorState = CursorState::InNode;
                selected = pN;
                break;
            }
            cursorState = OnGround;
            selected = nullptr;
        }

        UpdatePlayerAction();
    }

    void Render();

    void DrawGrid() const;

    void DrawDebugText(const char* text,Vector2 pos) const;

    void DrawDebugInfo();

    void OpenNeuralMenu(Neural* neural,Vector2 pos);

    void CloseNeuralMenu(Neural* neural);

    void ShowToolBar() const;

    void Cleanup() const;
private:
    static bool IsInside(float r,Vector2 center, Vector2 pos){
        float x = pos.x - center.x;
        float y = pos.y - center.y;
        return r*r>x*x+y*y;
    }

    static bool IsInsideInner(float r,Vector2 center, Vector2 pos){
        float x = pos.x - center.x;
        float y = pos.y - center.y;
        return (r-5)*(r-5)>x*x+y*y;
    }

    static bool IsInsideEdge(float r,Vector2 center, Vector2 pos){
        float x = pos.x - center.x;
        float y = pos.y - center.y;
        float a = x*x+y*y;
        return r*r>a&&a>=(r-5)*(r-5);
    }

    void UpdatePlayerAction();
};

#endif