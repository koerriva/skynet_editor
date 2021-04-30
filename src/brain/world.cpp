//
// Created by 50409 on 2021/4/29.
//

#include "NodeEditor.h"

namespace GamePlay{
    void NodeEditor::Render3D() {
        float cameraPos[3] = { m_Camera3d.position.x, m_Camera3d.position.y, m_Camera3d.position.z };
        Shader shader = m_Playground.materials[0].shader;
        SetShaderValue(shader, shader.locs[SHADER_LOC_VECTOR_VIEW], cameraPos, SHADER_UNIFORM_VEC3);
        shader = m_Bug.materials[0].shader;
        SetShaderValue(shader, shader.locs[SHADER_LOC_VECTOR_VIEW], cameraPos, SHADER_UNIFORM_VEC3);

        BeginTextureMode(m_RenderTarget);
        UpdateCamera(&m_Camera3d);
        ClearBackground(SKYBLUE);

        BeginMode3D(m_Camera3d);

        DrawModel(m_Playground,{0,0,0},1.0,DARKGREEN);
        DrawModelEx(m_Bug,m_BugPosition,Vector3{0,1,0},m_BugRotation,{1.0f,1.0f,1.0f},WHITE);
        PlayBugAnimation(m_BugAnimation[0]);

        DrawRay(Ray{m_BugPosition,m_BugDirection},RED);
//        DrawGrid(10,5.0);

        EndMode3D();
        EndTextureMode();

        BeginShaderMode(m_BaseShader);
        DrawTextureRec(m_RenderTarget.texture,{0,0,m_TargetSize.x,-m_TargetSize.y},{0,0},BLACK);
        EndShaderMode();
    }

    Material NodeEditor::LoadMaterialPBR(Color albedo, float metalness, float roughness)
    {
        Material mat = LoadMaterialDefault();   // Initialize material to default

        // Load PBR shader (requires several maps)
        mat.shader = LoadShader("data/shader/pbr.vert","data/shader/pbr.frag");

        // Get required locations points for PBR material
        // NOTE: Those location names must be available and used in the shader code
        mat.shader.locs[SHADER_LOC_MAP_ALBEDO] = GetShaderLocation(mat.shader, "albedo.sampler");
        mat.shader.locs[SHADER_LOC_MAP_METALNESS] = GetShaderLocation(mat.shader, "metalness.sampler");
        mat.shader.locs[SHADER_LOC_MAP_NORMAL] = GetShaderLocation(mat.shader, "normals.sampler");
        mat.shader.locs[SHADER_LOC_MAP_ROUGHNESS] = GetShaderLocation(mat.shader, "roughness.sampler");
        mat.shader.locs[SHADER_LOC_MAP_OCCLUSION] = GetShaderLocation(mat.shader, "occlusion.sampler");
        mat.shader.locs[SHADER_LOC_MAP_EMISSION] = GetShaderLocation(mat.shader, "emission.sampler");
        mat.shader.locs[SHADER_LOC_MAP_HEIGHT] = GetShaderLocation(mat.shader, "height.sampler");
        mat.shader.locs[SHADER_LOC_MAP_IRRADIANCE] = GetShaderLocation(mat.shader, "irradianceMap");
        mat.shader.locs[SHADER_LOC_MAP_PREFILTER] = GetShaderLocation(mat.shader, "prefilterMap");
        mat.shader.locs[SHADER_LOC_MAP_BRDF] = GetShaderLocation(mat.shader, "brdfLUT");

        // Set view matrix location
        mat.shader.locs[SHADER_LOC_MATRIX_MODEL] = GetShaderLocation(mat.shader, "matModel");
        //mat.shader.locs[SHADER_LOC_MATRIX_VIEW] = GetShaderLocation(mat.shader, "view");
        mat.shader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(mat.shader, "viewPos");

        // Set PBR standard maps
        Texture texture1 = LoadTextureFromImage(GenImageColor(1,1,albedo));
        mat.maps[MATERIAL_MAP_ALBEDO].texture = texture1;
        mat.maps[MATERIAL_MAP_NORMAL].texture = texture1;
        mat.maps[MATERIAL_MAP_METALNESS].texture = texture1;
        mat.maps[MATERIAL_MAP_ROUGHNESS].texture = texture1;
        mat.maps[MATERIAL_MAP_OCCLUSION].texture = texture1;

        // Set textures filtering for better quality
        SetTextureFilter(mat.maps[MATERIAL_MAP_ALBEDO].texture, FILTER_BILINEAR);
        SetTextureFilter(mat.maps[MATERIAL_MAP_NORMAL].texture, FILTER_BILINEAR);
        SetTextureFilter(mat.maps[MATERIAL_MAP_METALNESS].texture, FILTER_BILINEAR);
        SetTextureFilter(mat.maps[MATERIAL_MAP_ROUGHNESS].texture, FILTER_BILINEAR);
        SetTextureFilter(mat.maps[MATERIAL_MAP_OCCLUSION].texture, FILTER_BILINEAR);

        // Enable sample usage in shader for assigned textures
        static int sampler = 1;
        SetShaderValue(mat.shader, GetShaderLocation(mat.shader, "albedo.useSampler"), &sampler, SHADER_UNIFORM_INT);
        SetShaderValue(mat.shader, GetShaderLocation(mat.shader, "normals.useSampler"), &sampler, SHADER_UNIFORM_INT);
        SetShaderValue(mat.shader, GetShaderLocation(mat.shader, "metalness.useSampler"), &sampler, SHADER_UNIFORM_INT);
        SetShaderValue(mat.shader, GetShaderLocation(mat.shader, "roughness.useSampler"), &sampler, SHADER_UNIFORM_INT);
        SetShaderValue(mat.shader, GetShaderLocation(mat.shader, "occlusion.useSampler"), &sampler, SHADER_UNIFORM_INT);

        int renderModeLoc = GetShaderLocation(mat.shader, "renderMode");
        sampler = 0;
        SetShaderValue(mat.shader, renderModeLoc, &sampler, SHADER_UNIFORM_INT);

        // Set up material properties color
        mat.maps[MATERIAL_MAP_ALBEDO].color = albedo;
        mat.maps[MATERIAL_MAP_NORMAL].color = Color{ 128, 128, 255, 255 };
        mat.maps[MATERIAL_MAP_METALNESS].value = metalness;
        mat.maps[MATERIAL_MAP_ROUGHNESS].value = roughness;
        mat.maps[MATERIAL_MAP_OCCLUSION].value = 1.0f;
        mat.maps[MATERIAL_MAP_EMISSION].value = 0.5f;
        mat.maps[MATERIAL_MAP_HEIGHT].value = 0.5f;

        return mat;
    }
}