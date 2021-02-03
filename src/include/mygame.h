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
};

void ActiveNeural(Neural* neural){
    neural->isActive = true;
    neural->color = GREEN;
}

void DeActiveNeural(Neural* neural){
    neural->isActive = false;
    neural->color = DARKGREEN;
}

struct NeuralLink{
    Vector2 points[BEZIER_LINE_DIVISIONS+1];
    Color color;
    bool isFinish;
    bool isActive;

    Neural* in[MAX_NEURAL_SYNAPSE_SIZE];
    size_t in_synapse_count;
    Neural* out[MAX_NEURAL_SYNAPSE_SIZE];
    size_t out_synapse_count;
};

void LinkIn(NeuralLink* neuralLink,Neural* neural){
    neuralLink->in[neuralLink->in_synapse_count++]=neural;
}

void LinkOut(NeuralLink* neuralLink,Neural* neural){
    neuralLink->out[neuralLink->out_synapse_count++]=neural;
}

void ActiveNeuralLink(NeuralLink* neuralLink){
    neuralLink->isActive = true;
    neuralLink->color = WHITE;
    for (int i = 0; i < neuralLink->in_synapse_count; ++i) {
        auto* pN = neuralLink->in[i];
        ActiveNeural(pN);
    }
    for (int i = 0; i < neuralLink->out_synapse_count; ++i) {
        auto* pN = neuralLink->out[i];
        ActiveNeural(pN);
    }
}

void DeActiveNeuralLink(NeuralLink* neuralLink){
    neuralLink->isActive = false;
    neuralLink->color = GRAY;
    for (int i = 0; i < neuralLink->in_synapse_count; ++i) {
        auto* pN = neuralLink->in[i];
        DeActiveNeural(pN);
    }
    for (int i = 0; i < neuralLink->out_synapse_count; ++i) {
        auto* pN = neuralLink->out[i];
        DeActiveNeural(pN);
    }
}

enum PlayerAction{
    Idle,MoveScene,AddNode,MoveNode,LinkNode
};


enum CursorState{
    OnGround,InNode
};

enum NeuralLinkState{
    UNLINK,BEGIN,END
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
#endif