//
// Created by 50409 on 2021/4/24.
//

#ifndef SKYNET_EDITOR_ENTRYPOINT_H
#define SKYNET_EDITOR_ENTRYPOINT_H

#include "Application.h"
#include "thread"

//#ifdef _WIN32
//// N卡使用独显运行
//extern "C" __declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
//#endif
//
//#ifndef __APPLE__
//// A显卡使用独显运行
//extern "C" __declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 0x00000001;
//#endif

int main(int argc,char** argv){
#ifdef _WIN32
    system("chcp 65001");
#endif
    auto app = Engine::Create();
    std::thread async_worker(Engine::AsyncRun,app);
    app->Run();
    async_worker.join();
    delete app;
    return 0;
}

#endif //SKYNET_EDITOR_ENTRYPOINT_H
