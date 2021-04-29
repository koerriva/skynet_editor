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
        void RunAsync();
        void PushLayer(Layer* layer);
        void PopLayer(Layer* layer);

        static Application* Instance() {return s_Instance;}

    private:
        LayerStack m_LayerStack;
        bool m_Running = true;

        static Application* s_Instance;
    };

    Application* Create();

    static void AsyncRun(Application* application){
        application->RunAsync();
    }
}

#endif //SKYNET_EDITOR_APPLICATION_H
