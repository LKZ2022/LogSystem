// Copyright (C) 2026 刘恺知
// SPDX-License-Identifier: AGPL-3.0-or-later


// LogMessage.hpp 定义了日志消息的结构体 Message，包含时间戳、日志级别、文件名、行号、函数名、线程ID 和日志内容等字段

// include/log/LogMessage.hpp
#pragma once

#include "include/LogLevel.hpp"
#include <chrono>
#include <string>
#include <thread>

namespace Log
{

    // 日志消息结构
    struct Message
    {
        std::chrono::system_clock::time_point timestamp;
        Level level;
        std::string file;
        int line;
        std::string function;
        std::thread::id thread_id;
        std::string content;

        // 构造函数
        Message(Level lvl, const char* f, int l,
            const char* func, std::string_view msg)
            : timestamp(std::chrono::system_clock::now())
            , level(lvl)
            , file(f)
            , line(l)
            , function(func ? func : "")
            , thread_id(std::this_thread::get_id())
            , content(msg) {
        }
    };

} // namespace Log