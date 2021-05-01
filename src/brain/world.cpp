//
// Created by 50409 on 2021/4/29.
//

#include "NodeEditor.h"

namespace GamePlay{
    void NodeEditor::Init3DWorld() {
        m_Camera3d.position = { 0.0f, 10.0f, 20.0f };
        m_Camera3d.target = {0.0f, 0.0f, 0.0f};
        m_Camera3d.up = {0.0f, 1.0f, 0.0f};
        m_Camera3d.fovy = 45.0f;
        m_Camera3d.projection = CAMERA_PERSPECTIVE;

        m_Playground = LoadModelFromMesh(GenMeshPlane(200,200,10,10));
        m_Bug = LoadModel("data/Model/GLTF/Frog.glb");
        static int animCount=0;
        ModelAnimation * anim = LoadModelAnimations("data/Model/GLTF/Frog.glb",&animCount);
        TraceLog(LOG_INFO,TextFormat("animation count %d",animCount));
        for (int i = 0; i < animCount; ++i) {
            m_BugAnimation.push_back(Animation{anim[0],0});
        }
    }
    void NodeEditor::Render3D() {
        float cameraPos[3] = { m_Camera3d.position.x, m_Camera3d.position.y, m_Camera3d.position.z };
        Shader shader = m_Playground.materials[0].shader;
        SetShaderValue(shader, shader.locs[SHADER_LOC_VECTOR_VIEW], cameraPos, SHADER_UNIFORM_VEC3);
        shader = m_Bug.materials[0].shader;
        SetShaderValue(shader, shader.locs[SHADER_LOC_VECTOR_VIEW], cameraPos, SHADER_UNIFORM_VEC3);

        BeginTextureMode(m_RenderTarget);
        ClearBackground(SKYBLUE);

        BeginMode3D(m_Camera3d);

        DrawModel(m_Playground,{0,0,0},1.0,DARKGREEN);
        DrawModelEx(m_Bug,m_BugPosition,Vector3{0,1,0},m_BugRotation,{1.0f,1.0f,1.0f},WHITE);
        PlayBugAnimation(m_BugAnimation[0]);

        DrawRay(Ray{m_BugPosition,m_BugDirection},RED);

        EndMode3D();
        EndTextureMode();

        BeginShaderMode(m_BaseShader);
        DrawTextureRec(m_RenderTarget.texture,{0,0,m_TargetSize.x,-m_TargetSize.y},{0,0},BLACK);
        EndShaderMode();
    }

    Material NodeEditor::LoadMaterialPBR(Color albedo, float metalness, float roughness)
    {
        Material mat = LoadMaterialDefault();   // Initialize material to default
        return mat;
    }
}