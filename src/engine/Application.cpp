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
        SetConfigFlags(FLAG_VSYNC_HINT | FLAG_MSAA_4X_HINT | FLAG_WINDOW_HIGHDPI);
        InitWindow(800,600,"tech");
        int monitor = GetCurrentMonitor();
        int screenWidth = GetMonitorWidth(monitor);
        int screenHeight = GetMonitorHeight(monitor);

        SetWindowPosition((screenWidth-800)/2,(screenHeight-600)/2);
        SetTargetFPS(60);

        m_Camera2D.target = {0,0};
        m_Camera2D.offset = {400,300};
        m_Camera2D.rotation = 0;
        m_Camera2D.zoom = 1;

        s_Instance = this;
    }

    void Application::Run() {
        while (m_Running){
            for (Layer* layer:m_LayerStack){
                layer->OnUpdate();
            }

            BeginDrawing();
            ClearBackground({26, 59, 50, 255});

            BeginMode2D(m_Camera2D);
            for (Layer* layer:m_LayerStack) {
                layer->OnRender();
            }
            EndMode2D();
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