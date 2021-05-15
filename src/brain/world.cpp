//
// Created by 50409 on 2021/4/29.
//

#include "NodeEditor.h"

namespace GamePlay{
    void NodeEditor::Init3D() {
        m_Camera3d.position = { 0.0f, 10.0f, 20.0f };
        m_Camera3d.target = {0.0f, 0.0f, 0.0f};
        m_Camera3d.up = {0.0f, 1.0f, 0.0f};
        m_Camera3d.fovy = 45.0f;
        m_Camera3d.projection = CAMERA_PERSPECTIVE;

        m_BaseLightingShader = LoadShader("data/shader/base_lighting.vert","data/shader/base_lighting.frag");
        m_BaseLightingShader.locs[SHADER_LOC_MATRIX_MODEL] = GetShaderLocation(m_BaseLightingShader, "matModel");
        m_BaseLightingShader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(m_BaseLightingShader,"viewPos");
        m_BaseLightingShader.locs[SHADER_LOC_COLOR_AMBIENT] = GetShaderLocation(m_BaseLightingShader,"ambient");

        int ambientLoc = m_BaseLightingShader.locs[SHADER_LOC_COLOR_AMBIENT];
        auto* ambientColor = new float [4]{102/255.0f,192/255.0f,1.0f,1.0f};
        SetShaderValue(m_BaseLightingShader,ambientLoc,ambientColor,SHADER_UNIFORM_VEC4);

        m_Playground = LoadModelFromMesh(GenMeshPlane(200,200,10,10));
        m_Playground.materials[0].shader = m_BaseLightingShader;

        m_Bug = LoadModel("data/Model/GLTF/Frog.glb");
        m_Bug.materials[0].shader = m_BaseLightingShader;

        Vector3 pos = {0,5,0};
        Vector3 target = {0,0,0};
        m_SunLightDir = Vector3Normalize(Vector3Subtract(target,pos));
        m_SunLight = CreateLight(LIGHT_POINT, pos,target,WHITE,m_BaseLightingShader);

        static int animCount=0;
        ModelAnimation * anim = LoadModelAnimations("data/Model/GLTF/Frog.glb",&animCount);
        TraceLog(LOG_INFO,TextFormat("animation count %d",animCount));
        for (int i = 0; i < animCount; ++i) {
            m_BugAnimation.push_back(Animation{anim[i],0});
        }

        m_HeartChart = new HeartChart();
    }

    static float runTime = 0;
    void NodeEditor::Update3D() {
        UpdateCamera(&m_Camera3d);
        float cameraPos[3] = { m_Camera3d.position.x, m_Camera3d.position.y, m_Camera3d.position.z };
        float cameraTarget[3] = { m_Camera3d.target.x, m_Camera3d.target.y, m_Camera3d.target.z };
        Shader shader = m_BaseLightingShader;
        SetShaderValue(shader, shader.locs[SHADER_LOC_VECTOR_VIEW], cameraPos, SHADER_UNIFORM_VEC3);

//        SetShaderValue(shader, GetShaderLocation(shader,"viewEye"),cameraPos,SHADER_UNIFORM_VEC3);
//        SetShaderValue(shader,GetShaderLocation(shader,"viewCenter"),cameraTarget,SHADER_UNIFORM_VEC3);
        float deltaTime = GetFrameTime();
        runTime += deltaTime;
//        SetShaderValue(shader,GetShaderLocation(shader,"runTime"),&runTime,SHADER_UNIFORM_FLOAT);
//        float resolution[2] = { (float)width, (float)height };
//        SetShaderValue(shader,GetShaderLocation(shader,"resolution"),resolution,SHADER_UNIFORM_VEC2);

        m_SunLight.position.x = sin(runTime)*5;
        m_SunLight.position.z = cos(runTime)*5;
        UpdateLightValues(shader,m_SunLight);

        m_Camera3dShadowMap.position = m_SunLight.position;
        m_Camera3dShadowMap.target = m_SunLight.target;

        UpdateCamera(&m_Camera3dShadowMap);

        m_HeartChart->Update();
    }
    void NodeEditor::Render3D(Viewport& viewport) {
        m_Playground.materials[0].shader = m_BaseLightingShader;
        m_Bug.materials[0].shader = m_BaseLightingShader;
        ClearBackground(SKYBLUE);
        BeginMode3D(m_Camera3d);
        DrawModel(m_Playground,{0,0,0},1.0,DARKGREEN);
        DrawModelEx(m_Bug,m_BugPosition,Vector3{0,1,0},m_BugRotation,{1.0f,1.0f,1.0f},WHITE);

        if(m_BugJumping){
            if(m_BugAnimation[1].isOver()){
                m_BugJumping = false;
                m_BugAnimation[1].reset();
            }else{
                PlayBugAnimation(m_BugAnimation[1], false);
            }
        }else{
            PlayBugAnimation(m_BugAnimation[0],true);
        }

        Vector3 pos = m_BugPosition;
        pos.y += 3.0;
        m_HeartChart->Render(m_Camera3d,pos);

        DrawRay(Ray{m_BugPosition,m_BugDirection},RED);

        DrawSphere(m_SunLight.position,0.5,WHITE);
        DrawRay(Ray{m_SunLight.position,m_SunLightDir},WHITE);
        EndMode3D();
    }

    void NodeEditor::RayMarching() {
        ClearBackground(SKYBLUE);
        BeginShaderMode(m_RayMarchShader);
        DrawRectangle(0,0,width,height,WHITE);
        EndShaderMode();
    }

    Material NodeEditor::LoadMaterialPBR(Color albedo, float metalness, float roughness)
    {
        Material mat = LoadMaterialDefault();   // Initialize material to default
        return mat;
    }

    Material  NodeEditor::LoadMaterialPhong(Color ambient, Color diffuse, Color specular) {
        Material mat = LoadMaterialDefault();

        return mat;
    }

    Light NodeEditor::CreateLight(int type, Vector3 position, Vector3 target, Color color, Shader shader) {
        Light light = { 0 };
        int lightsCount = 1;
        if (lightsCount < MAX_LIGHTS)
        {
            light.enabled = true;
            light.type = type;
            light.position = position;
            light.target = target;
            light.color = color;

            // TODO: Below code doesn't look good to me,
            // it assumes a specific shader naming and structure
            // Probably this implementation could be improved
            char enabledName[32] = "lights[x].enabled\0";
            char typeName[32] = "lights[x].type\0";
            char posName[32] = "lights[x].position\0";
            char targetName[32] = "lights[x].target\0";
            char colorName[32] = "lights[x].color\0";

            // Set location name [x] depending on lights count
            enabledName[7] = '0' + lightsCount;
            typeName[7] = '0' + lightsCount;
            posName[7] = '0' + lightsCount;
            targetName[7] = '0' + lightsCount;
            colorName[7] = '0' + lightsCount;

            light.enabledLoc = GetShaderLocation(shader, enabledName);
            light.typeLoc = GetShaderLocation(shader, typeName);
            light.posLoc = GetShaderLocation(shader, posName);
            light.targetLoc = GetShaderLocation(shader, targetName);
            light.colorLoc = GetShaderLocation(shader, colorName);

            UpdateLightValues(shader, light);
        }

        return light;
    }

    void NodeEditor::UpdateLightValues(Shader shader, Light light) {
        // Send to shader light enabled state and type
        SetShaderValue(shader, light.enabledLoc, &light.enabled, SHADER_UNIFORM_INT);
        SetShaderValue(shader, light.typeLoc, &light.type, SHADER_UNIFORM_INT);

        // Send to shader light position values
        float position[3] = { light.position.x, light.position.y, light.position.z };
        SetShaderValue(shader, light.posLoc, position, SHADER_UNIFORM_VEC3);

        // Send to shader light target position values
        float target[3] = { light.target.x, light.target.y, light.target.z };
        SetShaderValue(shader, light.targetLoc, target, SHADER_UNIFORM_VEC3);

        // Send to shader light color values
        float color[4] = { (float)light.color.r/(float)255, (float)light.color.g/(float)255,
                           (float)light.color.b/(float)255, (float)light.color.a/(float)255 };
        SetShaderValue(shader, light.colorLoc, color, SHADER_UNIFORM_VEC4);
    }
}