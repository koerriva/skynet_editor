#ifndef MYGAME_H
#define MYGAME_H

struct Neural{
    Vector2 center;
    Color color=WHITE;
    bool isActive = false;
};

struct NeuralLink{
    Vector2 start;
    Vector2 end;
    bool isFinish = false;
    bool isActive = false;
};

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

static vec2 calcBezierPoint2D(float t,vec2 p0,vec2 p1,vec2 p2,vec2 p3){
    float u = 1-t;
    float tt = t*t;
    float uu = u*u;
    float uuu = uu*u;
    float ttt = tt*t;

    vec2 p = uuu*p0;
    p += 3*uu*t*p1;
    p += 3*u*tt*p2;
    p += ttt * p3;
    return p;
}

static vec3 calcBezierPoint3D(float t,vec3 p0,vec3 p1,vec3 p2,vec3 p3){
    float u = 1-t;
    float tt = t*t;
    float uu = u*u;
    float uuu = uu*u;
    float ttt = tt*t;

    vec3 p = uuu*p0;
    p += 3*uu*t*p1;
    p += 3*u*tt*p2;
    p += ttt * p3;
    return p;
}

static void getBezierPoints2D(vec2 start,vec2 end,size_t segment_count,vec2* buffer){
//    float t = 0;
//    int idx = 0;
//    vec2 p0 = calcBezierPoint2D(0,start,start,end,end);
//    buffer[idx++] = p0;
//    for (int i = 0; i < segment_count; ++i) {
//        t = i/(float)segment_count;
//        vec2 p1 = calcBezierPoint2D(t,start,start,end,end);
//        buffer[idx++] = p1;
//        p0 = p1;
//    }
}
#endif