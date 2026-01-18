// Copyright (C) 2026 刘恺知
// SPDX-License-Identifier: AGPL-3.0-or-later


//LogSink.hpp 日志输出目标抽象基类头文件

// include/log/LogSink.hpp


#pragma once

#include "include/LogMessage.hpp"
#include <memory>

namespace Log
{

    // 前向声明
    class Formatter;

    // 输出目标抽象基类
    class Sink
    {
    public:
        virtual ~Sink() = default;

        // 写入日志消息
        virtual void Write(const Message& msg) = 0;

        // 刷新缓冲区
        virtual void Flush() = 0;

        // 检查是否应该记录该级别
        virtual bool ShouldLog(Level level) const = 0;

        // 设置格式化器
        virtual void SetFormatter(std::unique_ptr<Formatter> formatter) = 0;
    };

} // namespace Log