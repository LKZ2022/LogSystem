// Copyright (C) 2026 刘恺知
// SPDX-License-Identifier: AGPL-3.0-or-later


//LogConfig.cpp 日志配置系统实现文件

// src/log/LogConfig.cpp



#include "include/LogConfig.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cstdlib>

namespace Log {

    Config Config::LoadFromFile(const std::string& path) {
        Config config;
        std::ifstream file(path);

        if (!file.is_open()) {
            return DefaultConfig();
        }

        std::string line;
        SinkConfig* current_sink = nullptr;

        while (std::getline(file, line)) {
            // 移除注释
            size_t comment_pos = line.find('#');
            if (comment_pos != std::string::npos) {
                line = line.substr(0, comment_pos);
            }

            // 移除前后空格
            line.erase(0, line.find_first_not_of(" \t"));
            line.erase(line.find_last_not_of(" \t") + 1);

            if (line.empty()) {
                continue;
            }

            // 解析配置项
            size_t eq_pos = line.find('=');
            if (eq_pos == std::string::npos) {
                if (line == "[console]") {
                    config.sinks.emplace_back("console");
                    current_sink = &config.sinks.back();
                }
                else if (line == "[file]") {
                    config.sinks.emplace_back("file");
                    current_sink = &config.sinks.back();
                }
                continue;
            }

            std::string key = line.substr(0, eq_pos);
            std::string value = line.substr(eq_pos + 1);

            key.erase(0, key.find_first_not_of(" \t"));
            key.erase(key.find_last_not_of(" \t") + 1);
            value.erase(0, value.find_first_not_of(" \t"));
            value.erase(value.find_last_not_of(" \t") + 1);

            if (key == "level") {
                if (value == "TRACE") config.global_level = Level::LVL_TRACE;
                else if (value == "DEBUG") config.global_level = Level::LVL_DEBUG;
                else if (value == "INFO") config.global_level = Level::LVL_INFO;
                else if (value == "WARN") config.global_level = Level::LVL_WARN;
                else if (value == "ERROR") config.global_level = Level::LVL_ERROR;
                else if (value == "FATAL") config.global_level = Level::LVL_FATAL;
            }
            else if (current_sink) {
                if (key == "level") {
                    if (value == "TRACE") current_sink->min_level = Level::LVL_TRACE;
                    else if (value == "DEBUG") current_sink->min_level = Level::LVL_DEBUG;
                    else if (value == "INFO") current_sink->min_level = Level::LVL_INFO;
                    else if (value == "WARN") current_sink->min_level = Level::LVL_WARN;
                    else if (value == "ERROR") current_sink->min_level = Level::LVL_ERROR;
                    else if (value == "FATAL") current_sink->min_level = Level::LVL_FATAL;
                }
                else if (key == "pattern") {
                    current_sink->pattern = value;
                }
                else if (key == "colors" && current_sink->type == "console") {
                    current_sink->use_colors = (value == "true" || value == "1");
                }
                else if (key == "filename" && current_sink->type == "file") {
                    current_sink->filename = value;
                }
                else if (key == "max_size" && current_sink->type == "file") {
                    current_sink->max_size = std::stoull(value);
                }
                else if (key == "max_files" && current_sink->type == "file") {
                    current_sink->max_files = std::stoi(value);
                }
            }
        }

        file.close();
        return config;
    }

    Config Config::LoadFromEnv() {
        Config config = DefaultConfig();

        const char* level = std::getenv("LOG_LEVEL");
        if (level) {
            std::string level_str = level;
            if (level_str == "TRACE") config.global_level = Level::LVL_TRACE;
            else if (level_str == "DEBUG") config.global_level = Level::LVL_DEBUG;
            else if (level_str == "INFO") config.global_level = Level::LVL_INFO;
            else if (level_str == "WARN") config.global_level = Level::LVL_WARN;
            else if (level_str == "ERROR") config.global_level = Level::LVL_ERROR;
            else if (level_str == "FATAL") config.global_level = Level::LVL_FATAL;
        }

        const char* file = std::getenv("LOG_FILE");
        if (file) {
            SinkConfig file_sink("file");
            file_sink.filename = file;
            file_sink.min_level = config.global_level;
            config.sinks.push_back(file_sink);
        }

        return config;
    }

    Config Config::DefaultConfig() {
        Config config;
        config.global_level = Level::LVL_INFO;

        SinkConfig console_sink("console");
        console_sink.min_level = Level::LVL_INFO;
        console_sink.use_colors = true;
        console_sink.pattern = "%t [%l] [%T] [%f:%n:%F] %m";
        config.sinks.push_back(console_sink);

        return config;
    }

} // namespace Log