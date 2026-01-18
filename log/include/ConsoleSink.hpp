// Copyright (C) 2026 刘恺知
// SPDX-License-Identifier: AGPL-3.0-or-later


//ConsoleSink.hpp 控制台日志输出目标头文件

// include/log/ConsoleSink.hpp



#pragma once

#include "include/LogSink.hpp"
#include "include/ColoredFormatter.hpp"
#include <mutex>
#include <iostream>

namespace Log
{

    class ConsoleSink : public Sink
    {
    public:
        ConsoleSink(Level min_level = Level::LVL_INFO,
            bool use_colors = true,
            const std::string& pattern = "");

        ~ConsoleSink() = default;

        void Write(const Message& msg) override;
        void Flush() override;
        bool ShouldLog(Level level) const override;
        void SetFormatter(std::unique_ptr<Formatter> formatter) override;

        // 设置着色模式
        void SetUseColors(bool use_colors);

    private:
#ifdef _WIN32
        void EnableANSIColorSupport() const;
#endif

    private:
        mutable std::mutex mutex_;
        Level min_level_;
        bool use_colors_;
        std::unique_ptr<ColoredFormatter> colored_formatter_;
        std::unique_ptr<Formatter> formatter_;
    };

} // namespace Log