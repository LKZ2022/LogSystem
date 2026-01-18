// Copyright (C) 2026 刘恺知
// SPDX-License-Identifier: AGPL-3.0-or-later


//FileSink.cpp 文件日志输出目标实现文件

//	src/log/FileSink.cpp


#include "include/FileSink.hpp"
#include <iomanip>
#include <iostream>


namespace Log
{

    FileSink::FileSink(const std::string& filename, Level min_level, const std::string& pattern)
        : filename_(filename), min_level_(min_level)
    {

        if (pattern.empty())
        {
            formatter_ = std::make_unique<Formatter>();
        }
        else
        {
            formatter_ = std::make_unique<Formatter>(pattern);
        }

        EnsureFileOpen();
    }

    FileSink::~FileSink()
    {
        if (file_.is_open())
        {
            file_.close();
        }
    }

    void FileSink::Write(const Message& msg)
    {
        if (!ShouldLog(msg.level))
        {
            return;
        }

        std::lock_guard<std::mutex> lock(mutex_);

        EnsureFileOpen();
        if (file_.is_open())
        {
            file_ << formatter_->Format(msg);
        }
    }

    void FileSink::Flush()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (file_.is_open())
        {
            file_.flush();
        }
    }

    bool FileSink::ShouldLog(Level level) const
    {
        return level >= min_level_;
    }

    void FileSink::SetFormatter(std::unique_ptr<Formatter> formatter)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        formatter_ = std::move(formatter);
    }

    void FileSink::EnsureFileOpen()
    {
        if (!file_.is_open()) {
            // 创建目录（如果不存在）
            std::filesystem::path path(filename_);
            if (path.has_parent_path()) {
                std::filesystem::create_directories(path.parent_path());
            }

            file_.open(filename_, std::ios::app);
            if (!file_.is_open()) {
                std::cerr << "Failed to open log file: " << filename_ << std::endl;
            }
        }
    }

} // namespace Log