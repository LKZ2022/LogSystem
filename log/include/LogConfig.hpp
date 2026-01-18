// Copyright (C) 2026 刘恺知
// SPDX-License-Identifier: AGPL-3.0-or-later


// LogConfig.hpp 日志配置系统头文件

// include/log/LogConfig.hpp



#pragma once

#include "include/LogLevel.hpp"
#include <string>
#include <vector>
#include <map>

namespace Log
{

    // Sink配置
    struct SinkConfig
    {
        std::string type;  // "console", "file"
        Level min_level{ Level::LVL_INFO };

        // 通用配置
        std::string pattern;  // 格式化模式

        // 控制台配置
        bool use_colors{ true };

        // 文件配置
        std::string filename;
        size_t max_size{ 100 * 1024 * 1024 };  // 100MB
        int max_files{ 10 };

        SinkConfig() = default;
        SinkConfig(const std::string& t) : type(t) {}
    };

    // 日志配置
    struct Config
    {
        Level global_level{ Level::LVL_INFO };
        std::vector<SinkConfig> sinks;
        std::map<std::string, Level> module_levels;

        // 从文件加载
        static Config LoadFromFile(const std::string& path);

        // 从环境变量加载
        static Config LoadFromEnv();

        // 默认配置
        static Config DefaultConfig();
    };

} // namespace Log