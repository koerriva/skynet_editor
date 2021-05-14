//
// Created by 50409 on 2021/4/24.
//

#include "Application.h"

namespace Engine{
    Application* Application::s_Instance = nullptr;

    Application::Application() {
        if(s_Instance){
            TraceLog(LOG_ERROR,"程序实例已存在");
        }
        int w=1440,h=900;
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

        s_Instance = this;
    }

    void Application::Run() {
        while (m_Running){
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
}