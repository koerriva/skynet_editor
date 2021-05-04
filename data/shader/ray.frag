#version 330

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;
in vec4 fragColor;

// Input uniform values
uniform sampler2D texture0;
uniform vec4 colDiffuse;

// Output fragment color
out vec4 finalColor;

// NOTE: Add here your custom variables

#define     MAX_LIGHTS              4
#define     LIGHT_DIRECTIONAL       0
#define     LIGHT_POINT             1

struct MaterialProperty {
    vec3 color;
    int useSampler;
    sampler2D sampler;
};

struct Light {
    int enabled;
    int type;
    vec3 position;
    vec3 target;
    vec4 color;
};

// Input lighting values
uniform Light lights[MAX_LIGHTS];
uniform vec4 ambient;
uniform vec3 viewPos;

uniform vec3 viewEye;
uniform vec3 viewCenter;
uniform float runTime;
uniform vec2 resolution;

mat3 setCamera( in vec3 ro, in vec3 ta, float cr )
{
    vec3 cw = normalize(ta-ro);
    vec3 cp = vec3(sin(cr), cos(cr),0.0);
    vec3 cu = normalize( cross(cw,cp) );
    vec3 cv = normalize( cross(cu,cw) );
    return mat3( cu, cv, cw );
}

float dot2( in vec2 v ) { return dot(v,v); }
float dot2( in vec3 v ) { return dot(v,v); }
float ndot( in vec2 a, in vec2 b ) { return a.x*b.x - a.y*b.y; }

#define MAX_STEPS 1000
#define MAX_DIST 1000.
#define SURF_DIST .01

float udTriangle( vec3 p, vec3 a, vec3 b, vec3 c )
{
    vec3 ba = b - a; vec3 pa = p - a;
    vec3 cb = c - b; vec3 pb = p - b;
    vec3 ac = a - c; vec3 pc = p - c;
    vec3 nor = cross( ba, ac );

    return sqrt(
    (sign(dot(cross(ba,nor),pa)) +
    sign(dot(cross(cb,nor),pb)) +
    sign(dot(cross(ac,nor),pc))<2.0)
    ?
    min( min(
    dot2(ba*clamp(dot(ba,pa)/dot2(ba),0.0,1.0)-pa),
    dot2(cb*clamp(dot(cb,pb)/dot2(cb),0.0,1.0)-pb) ),
    dot2(ac*clamp(dot(ac,pc)/dot2(ac),0.0,1.0)-pc) )
    :
    dot(nor,pa)*dot(nor,pa)/dot2(nor) );
}

float GetDist(vec3 p){
    vec4 s = vec4(0,1,6,1);
    float sphereDist = length(p-s.xyz)-s.w;
    float planeDist = p.y;
    float triangleDist = udTriangle(p,vec3(0),vec3(0,2,0),vec3(0,0,2));

    float d = min(sphereDist,planeDist);
    d = min(d,triangleDist);
    return d;
}

float RayMarch(vec3 ro,vec3 rd){
    float dO = 0;
    for(int i=0;i<MAX_STEPS;i++){
        vec3 p = ro + rd* dO;
        float dS = GetDist(p);
        dO += dS;
        if(dO>MAX_DIST||dS<SURF_DIST) break;
    }
    return dO;
}

vec3 GetNormal(vec3 p){
    float d = GetDist(p);
    vec2 e = vec2(.01,0);
    vec3 n = d - vec3(GetDist(p-e.xyy),GetDist(p-e.yxy),GetDist(p-e.yyx));
    return normalize(n);
}

float GetLight(vec3 p){
    vec3 lightPos = vec3(0,5,5);
    lightPos.xz += 5*vec2(sin(runTime),cos(runTime));
    vec3 l = normalize(lightPos-p);
    vec3 n = GetNormal(p);

    float diffuse = clamp(dot(n,l),0.,1.);
    float d = RayMarch(p+n*SURF_DIST*2.,l);
    if(d<length(lightPos-p)) diffuse *= .1;
    return diffuse;
}

void main()
{
    vec2 uv = (gl_FragCoord.xy-0.5*resolution.xy)/resolution.y;

    vec3 col = vec3(0);

    vec3 ro = viewEye;
    vec3 ta = viewCenter;
    // camera-to-world transformation
    mat3 ca = setCamera( ro, ta, 0.0 );

    vec3 rd = ca*normalize(vec3(uv,1));

    float d = RayMarch(ro,rd);

    vec3 p = ro+rd*d;
    float diffuse = GetLight(p);
    col = vec3(diffuse);

    finalColor = vec4(col,1.0)+ambient;
    // Gamma correction
    finalColor = pow(finalColor, vec4(1.0/2.2));
}
