﻿//
// Created by koerriva on 2021/5/28.
//

#ifndef SKYNET_EDITOR_LOG_H
#define SKYNET_EDITOR_LOG_H

#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"

namespace Engine {
    class Log {
    private:
        static std::shared_ptr<spdlog::logger> s_CoreLogger;
        static std::shared_ptr<spdlog::logger> s_ClientLogger;
    public:
        static void Init();
        inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
        inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }
    };
}

#define CORE_TRACE(...)		::Engine::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define CORE_INFO(...)		::Engine::Log::GetCoreLogger()->info(__VA_ARGS__)
#define CORE_WARN(...)		::Engine::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define CORE_ERROR(...)		::Engine::Log::GetCoreLogger()->error(__VA_ARGS__)
#define CORE_CRITICAL(...)	::Engine::Log::GetCoreLogger()->critical(__VA_ARGS__)

#define LOG_TRACE(...)		::Engine::Log::GetClientLogger()->trace(__VA_ARGS__)
#define LOG_INFO(...)		::Engine::Log::GetClientLogger()->info(__VA_ARGS__)
#define LOG_WARN(...)		::Engine::Log::GetClientLogger()->warn(__VA_ARGS__)
#define LOG_ERROR(...)		::Engine::Log::GetClientLogger()->error(__VA_ARGS__)
#define LOG_CRITICAL(...)	::Engine::Log::GetClientLogger()->critical(__VA_ARGS__)

#endif //SKYNET_EDITOR_LOG_H
