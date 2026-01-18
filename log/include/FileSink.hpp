// Copyright (C) 2026 刘恺知
// SPDX-License-Identifier: AGPL-3.0-or-later


//FileSink.hpp 文件日志输出目标头文件

// include/log/FileSink.hpp



#pragma once

#include "include/LogSink.hpp"
#include "include/LogFormatter.hpp"
#include <fstream>
#include <mutex>
#include <filesystem>

namespace Log
{

    class FileSink : public Sink
    {
    public:
        explicit FileSink(const std::string& filename,
            Level min_level = Level::LVL_INFO,
            const std::string& pattern = "");
        ~FileSink();

        void Write(const Message& msg) override;
        void Flush() override;
        bool ShouldLog(Level level) const override;
        void SetFormatter(std::unique_ptr<Formatter> formatter) override;

    private:
        // 确保文件打开
        void EnsureFileOpen();

    private:
        mutable std::mutex mutex_;
        std::ofstream file_;
        std::string filename_;
        Level min_level_;
        std::unique_ptr<Formatter> formatter_;
    };

} // namespace Log