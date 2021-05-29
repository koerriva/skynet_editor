//
// Created by 50409 on 2021/4/24.
//

#ifndef SKYNET_EDITOR_ENTRYPOINT_H
#define SKYNET_EDITOR_ENTRYPOINT_H

#include "Application.h"
#include "thread"

extern "C" {
    __declspec(dllexport) unsigned long NvOptimusEnablement = 1;
    __declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}

int main(int argc,char** argv){
#ifdef _WIN32
    system("chcp 65001");
#endif
    Engine::Log::Init();

    auto app = Engine::Create();
    std::thread async_worker(Engine::AsyncRun,app);
    app->Run();
    async_worker.join();
    delete app;
    return 0;
}

#endif //SKYNET_EDITOR_ENTRYPOINT_H
