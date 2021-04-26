//
// Created by 50409 on 2021/4/24.
//

#ifndef SKYNET_EDITOR_APPLICATION_H
#define SKYNET_EDITOR_APPLICATION_H

#include "raylib.h"
#include "LayerStack.h"

namespace Engine{
    class Application{
    public:
        Application();
        virtual ~Application();;

        void Run();
        void PushLayer(Layer* layer);
        void PopLayer(Layer* layer);

        static Application* Instance() {return s_Instance;}

    protected:
        Camera2D m_Camera2D{};

    private:
        LayerStack m_LayerStack;
        bool m_Running = true;

        static Application* s_Instance;
    };

    Application* Create();
}

#endif //SKYNET_EDITOR_APPLICATION_H
