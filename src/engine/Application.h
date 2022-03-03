//
// Created by 50409 on 2021/4/24.
//

#ifndef SKYNET_EDITOR_APPLICATION_H
#define SKYNET_EDITOR_APPLICATION_H

#include "raylib.h"
#include "LayerStack.h"

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_STANDARD_VARARGS
#include "raylib-nuklear.h"

namespace Engine{
    class Application{
    public:
        Application();
        virtual ~Application();;

        void Run();
        void RunAsync();
        void PushLayer(Layer* layer);
        void PopLayer(Layer* layer);

        static Application* Instance() {return s_Instance;}

    private:
        void LoadFont();

    private:
        LayerStack m_LayerStack;
        bool m_Running = true;

        static Application* s_Instance;
    protected:
        Font m_Font;
        int m_FontSize = 16;
    };

    Application* Create();

    static void AsyncRun(Application* application){
        application->RunAsync();
    }
}

#endif //SKYNET_EDITOR_APPLICATION_H
