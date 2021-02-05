#ifndef MYGAME_H
#define MYGAME_H
#include <easings.h>

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
    bool isActive;
    float radius;

    void Active(){
        isActive = true;
        color = GREEN;
    }

    void DeActive(){
        isActive = false;
        color = DARKGREEN;
    }
};

enum NeuralLinkState{
    UNLINK,BEGIN,END
};

struct NeuralLink{
    Vector2 points[BEZIER_LINE_DIVISIONS+1];
    Neural* form;
    Neural* to;
    Color color;
    bool isActive;
    NeuralLinkState state;

    void Init(){
        form = nullptr;
        to = nullptr;
        isActive = false;
        state = UNLINK;
    }

    void Active(){
        isActive = true;
        color = WHITE;
        form->Active();
        to->Active();
    }

    void DeActive(){
        isActive = false;
        color = ColorAlpha(GRAY,0.3);
        form->DeActive();
        to->DeActive();
    }
};

struct NeuralNetwork{
    Neural* neurals;
    int neural_count;
    NeuralLink* links;
    int link_count;

    void Init(){
        neural_count = 0;
        link_count = 0;
    }

    void AddNeural(Neural neural){
        neurals[neural_count++] = neural;
    }

    void AddLink(NeuralLink neuralLink){
        links[link_count++] = neuralLink;
    }
};

enum PlayerAction{
    Idle,MoveScene,AddNode,MoveNode,LinkNode
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

    void DrawGrid() const{
        int start_x = -world_width/2;
        int start_y = -world_height/2;

        int end_x = world_width/2;
        int end_y = world_height/2;
        //draw grid
        int seg = 20;
        for (int i = 0; i < world_width / seg; ++i) {
            DrawLine(start_x+i*seg,start_y
                    ,start_x+i*seg,end_y
                    ,DARKGRAY);
        }
        for (int i = 0; i < world_width / (seg*5); ++i) {
            DrawLine(start_x+i*seg*5,start_y
                    ,start_x+i*seg*5,end_y
                    ,BLACK);
        }
        for (int i = 0; i < world_height/seg; ++i) {
            DrawLine(start_x,i*seg+start_y
                    ,end_x,i*seg+start_y
                    ,DARKGRAY);
        }
        for (int i = 0; i < world_height/(seg*5); ++i) {
            DrawLine(start_x,i*seg*5+start_y
                    ,end_x,i*seg*5+start_y
                    ,BLACK);
        }
    }
};
#endif