//
// Created by 50409 on 2021/4/24.
//

#include "Application.h"

#define NK_INCLUDE_FIXED_TYPES
//#define NK_INCLUDE_STANDARD_IO
//#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
//#define NK_INCLUDE_DEFAULT_FONT
//#define NK_BUTTON_TRIGGER_ON_RELEASE
#define NK_INCLUDE_STANDARD_VARARGS

#define RAYLIB_NUKLEAR_IMPLEMENTATION
#include "raylib-nuklear.h"

#include "node_editor.c"

namespace Engine{
    Application* Application::s_Instance = nullptr;

    Application::Application() {
        if(s_Instance){
            TraceLog(LOG_ERROR,"程序实例已存在");
        }
        s_Instance = this;

        int w=800,h=600;
        SetConfigFlags(FLAG_VSYNC_HINT | FLAG_MSAA_4X_HINT);
        InitWindow(w,h,"Frog Brain");
        Image icon = LoadImage("data/neural.png");
        Image icon2 = GenImageCellular(64,64,32);
        SetWindowIcon(icon2);
        int monitor = GetCurrentMonitor();
        int screenWidth = GetMonitorWidth(monitor);
        int screenHeight = GetMonitorHeight(monitor);

        SetWindowPosition(screenWidth/2-w/2,screenHeight/2-h/2);
        SetTargetFPS(60);

        LoadFont();

        //nuklear
        // Create the Nuklear Context
        nkContext = InitNuklear();
    }

    void Application::Run() {
        while (m_Running){
            UpdateNuklear(nkContext);
            node_editor(nkContext);

            if(IsKeyPressed(KEY_F11)){
                ToggleFullscreen();
            }
            for (Layer* layer:m_LayerStack){
                layer->OnUpdate();
            }

            BeginDrawing();
                for (Layer* layer:m_LayerStack) {
                    layer->OnRender();
                }

                for (Layer* layer:m_LayerStack) {
                    layer->OnGUIRender();
                }

                DrawNuklear(nkContext);
                DrawFPS(5,5);
            EndDrawing();
            m_Running = !WindowShouldClose();
        }
    }

    void Application::RunAsync() {
        while (m_Running){
            for (Layer* layer:m_LayerStack) {
                layer->OnAsyncUpdate();
            }
        }
    }

    void Application::PushLayer(Layer *layer) {
        m_LayerStack.PushLayer(layer);
        layer->OnAttach();
    }

    void Application::PopLayer(Layer *layer) {
        m_LayerStack.PopLayer(layer);
        layer->OnDeAttach();
    }

    Application::~Application() {
        CloseWindow();
    }

    void Application::LoadFont() {
        //基本汉字 4E00-9FA5
        //兼容汉字 F900-FAD9
        //基本字符 0-ff
        std::vector<int> fontChars;

        TraceLog(LOG_INFO,"Init SC Font Memory");
        for (int i = 0; i <= 0xFF; ++i) {
            fontChars.push_back(i);
        }
        fontChars.push_back(0x3000);
        for (int i = 0xff01; i <= 0xff0f; ++i) {
            fontChars.push_back(i);
        }
        for (int i = 0x4E00; i <= 0x9FA5; ++i) {
            fontChars.push_back(i);
        }

        TraceLog(LOG_INFO,"Load SC Font");
//        m_Font = LoadFontEx("data/font/NotoSansSC-Regular.otf",16,fontChars.data(),fontChars.size());
        m_Font = LoadFontEx("data/font/LiHeiPro.ttf",16,fontChars.data(),fontChars.size());
        TraceLog(LOG_INFO,"SC Font Load");
        fontChars.clear();
    }
}