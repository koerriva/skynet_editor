#version 330

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;
in vec4 fragColor;

// Input uniform values
uniform sampler2D texture0;
uniform vec4 colDiffuse;

// Output fragment color
out vec4 finalColor;

const vec2 lightTexCoord = vec2(0.5,0.5);

#define TWO_PI 6.28318530718f
#define N 64
#define MAX_STEP 100
#define MAX_DISTANCE 1.0f
#define EPSILON 1e-6f

struct Light{
    vec2 pos;
    float radius;
} lights[2];

struct Result{
    float sd;
    float emissive;
};

float random (vec2 st) {
    return fract(sin(dot(st.xy,
    vec2(12.9898,78.233)))*
    43758.5453123);
}

float circleSDF(vec2 p, Light light) {
    vec2 u = p-light.pos;
    return length(u) - light.radius;
}

Result unionOp(Result a, Result b) {
    return a.sd < b.sd ? a : b;
}

Result scene(vec2 p){
    Result r1 = Result(circleSDF(p,lights[0]),2.0f);
    Result r2 = Result(circleSDF(p,lights[1]),0.8f);
    Result r3 = Result(circleSDF(p,Light(vec2(0.5,0.7),0.1f)),0.2f);
    return unionOp(unionOp(r1,r2),r3);
}

float trace(vec2 origin,vec2 dir){
    float t = 0.0f;
    for (int i = 0; i < MAX_STEP && t < MAX_DISTANCE; i++) {
        Result r = scene(origin+dir*t);
        if (r.sd < EPSILON){
            return r.emissive;
        }
        t+=r.sd;
    }
    return 0.0f;
}

float sampler(vec2 p) {
    float sum = 0.0f;
    for (int i = 0; i < N; i++) {
        float a = TWO_PI * i*1.0/N;
        sum += trace(p, vec2(cos(a), sin(a)));
    }
    return sum / N;
}

void main()
{
    for(int i=0;i<2;i++){
        lights[i].pos = vec2(0.2+i*0.1,0.2+i*0.1);
        lights[i].radius = 0.05f;
    }

    float a = sampler(fragTexCoord.xy);

    finalColor = vec4(vec3(a),1.0);
}