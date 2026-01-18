// Copyright (C) 2026 刘恺知
// SPDX-License-Identifier: AGPL-3.0-or-later


//Logger.cpp 日志系统实现文件

//	src/log/Logger.cpp




#include "include/Logger.hpp"
#include "include/ConsoleSink.hpp"
#include "include/FileSink.hpp"
#include "include/LogConfig.hpp"

namespace Log
{

    Logger::Logger() = default;

    Logger::~Logger()
    {
        Flush();
    }

    Logger& Logger::Instance()
    {
        static Logger instance;
        return instance;
    }

    void Logger::Initialize(Level level)
    {
        std::lock_guard<std::recursive_mutex> lock(mutex_);
        current_level_ = level;
        initialized_ = true;

        Log(Level::LVL_INFO, __FILE__, __LINE__, __func__,
            "Logger initialized with level: " + std::string(LevelToString(level)));
    }

    void Logger::SetLevel(Level level)
    {
        std::lock_guard<std::recursive_mutex> lock(mutex_);
        current_level_ = level;

        Log(Level::LVL_INFO, __FILE__, __LINE__, __func__,
            "Log level set to: " + std::string(LevelToString(level)));
    }

    Level Logger::GetLevel() const
    {
        std::lock_guard<std::recursive_mutex> lock(mutex_);
        return current_level_;
    }

    void Logger::Log(Level level,
        const char* file,
        int line,
        const char* function,
        const std::string& message)
    {

        if (!ShouldLog(level))
        {
            return;
        }

        Message msg(level, file, line, function, message);
        WriteToSinks(msg);
    }

    void Logger::AddSink(std::unique_ptr<Sink> sink)
    {
        std::lock_guard<std::recursive_mutex> lock(mutex_);
        sinks_.push_back(std::move(sink));
    }

    void Logger::AddConsoleSink(Level min_level, bool use_colors, const std::string& pattern)
    {
        auto sink = std::make_unique<ConsoleSink>(min_level, use_colors, pattern);
        AddSink(std::move(sink));
    }

    void Logger::AddFileSink(const std::string& filename, Level min_level, const std::string& pattern)
    {
        auto sink = std::make_unique<FileSink>(filename, min_level, pattern);
        AddSink(std::move(sink));
    }

    void Logger::Initialize(const Config& config)
    {
        std::lock_guard<std::recursive_mutex> lock(mutex_);

        sinks_.clear();
        current_level_ = config.global_level;

        for (const auto& sink_config : config.sinks)
        {
            auto sink = CreateSink(sink_config);
            if (sink) {
                sinks_.push_back(std::move(sink));
            }
        }

        initialized_ = true;

        Log(Level::LVL_INFO, __FILE__, __LINE__, __func__,
            "Logger initialized from config with level: " + std::string(LevelToString(current_level_)));
    }

    void Logger::InitializeFromFile(const std::string& config_path)
    {
        auto config = Config::LoadFromFile(config_path);
        Initialize(config);
    }

    void Logger::InitializeFromEnv()
    {
        auto config = Config::LoadFromEnv();
        Initialize(config);
    }

    void Logger::Flush()
    {
        std::lock_guard<std::recursive_mutex> lock(mutex_);
        for (auto& sink : sinks_)
        {
            sink->Flush();
        }
    }

    bool Logger::ShouldLog(Level level) const
    {
        std::lock_guard<std::recursive_mutex> lock(mutex_);
        return level >= current_level_;
    }

    void Logger::WriteToSinks(const Message& msg)
    {
        std::lock_guard<std::recursive_mutex> lock(mutex_);

        for (const auto& sink : sinks_)
        {
            if (sink->ShouldLog(msg.level))
            {
                sink->Write(msg);
            }
        }
    }

    std::unique_ptr<Sink> Logger::CreateSink(const struct SinkConfig& config)
    {
        if (config.type == "console")
        {
            auto sink = std::make_unique<ConsoleSink>(config.min_level, config.use_colors, config.pattern.empty() ? "" : config.pattern);
            return sink;
        }
        else if (config.type == "file")
        {
            if (!config.filename.empty())
            {
                auto sink = std::make_unique<FileSink>(config.filename, config.min_level, config.pattern.empty() ? "" : config.pattern);
                return sink;
            }
        }

        return nullptr;
    }

} // namespace Log