// Copyright (C) 2026 刘恺知
// SPDX-License-Identifier: AGPL-3.0-or-later


//Logger.hpp 日志系统头文件


// include/log/Logger.hpp




#pragma once

#include "include/LogLevel.hpp"
#include "include/LogSink.hpp"
#include <string>
#include <memory>
#include <vector>
#include <mutex>

namespace Log
{

    // 前向声明
    struct Config;

    // 日志器单例类
    class Logger
    {
    public:
        // 获取单例
        static Logger& Instance();

        // 禁用拷贝和移动
        Logger(const Logger&) = delete;
        Logger& operator=(const Logger&) = delete;
        Logger(Logger&&) = delete;
        Logger& operator=(Logger&&) = delete;

        // 基础接口
        void Initialize(Level level = Level::LVL_INFO);
        void SetLevel(Level level);
        Level GetLevel() const;

        // 日志记录（同步）
        void Log(Level level,
            const char* file,
            int line,
            const char* function,
            const std::string& message);

        // 添加输出目标
        void AddSink(std::unique_ptr<Sink> sink);

        // 便捷方法：添加控制台输出
        void AddConsoleSink(Level min_level = Level::LVL_INFO,
            bool use_colors = true,
            const std::string& pattern = "");

        // 便捷方法：添加文件输出
        void AddFileSink(const std::string& filename,
            Level min_level = Level::LVL_INFO,
            const std::string& pattern = "");

        // 配置接口
        void Initialize(const Config& config);
        void InitializeFromFile(const std::string& config_path);
        void InitializeFromEnv();

        // 刷新所有输出目标
        void Flush();

        // 递归锁测试方法
        void RecursiveTest(int depth = 3);

    private:
        Logger();
        ~Logger();

        // 辅助方法
        bool ShouldLog(Level level) const;

        // 写入所有sink
        void WriteToSinks(const Message& msg);

        // 根据配置创建Sink
        std::unique_ptr<Sink> CreateSink(const struct SinkConfig& config);

    private:
        mutable std::recursive_mutex mutex_;
        std::vector<std::unique_ptr<Sink>> sinks_;
        Level current_level_{ Level::LVL_INFO };
        bool initialized_{ false };
    };

} // namespace Log