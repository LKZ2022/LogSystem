// Copyright (C) 2026 刘恺知
// SPDX-License-Identifier: AGPL-3.0-or-later


// ColoredFormatter.cpp 日志着色格式化器实现文件

// src/log/ColoredFormatter.cpp



#include "include/ColoredFormatter.hpp"
#include "include/LogLevel.hpp"
#include <sstream>
#include <iomanip>

namespace Log {

    ColoredFormatter::ColoredFormatter() {
        // 默认格式和着色
        std::vector<ColorRegion> default_regions = {
            ColorRegion(ColorRegion::Type::TIMESTAMP, false),
            ColorRegion(" ", false),
            ColorRegion("[", false),
            ColorRegion(ColorRegion::Type::LEVEL, true),
            ColorRegion("]", false),
            ColorRegion(" ", false),
            ColorRegion("[", false),
            ColorRegion(ColorRegion::Type::THREAD_ID, false),
            ColorRegion("]", false),
            ColorRegion(" ", false),
            ColorRegion("[", false),
            ColorRegion(ColorRegion::Type::LOCATION, false),
            ColorRegion("]", false),
            ColorRegion(" ", false),
            ColorRegion(ColorRegion::Type::MESSAGE, true)
        };

        SetFormat("%t [%l] [%T] [%f:%n:%F] %m", default_regions);
    }

    ColoredFormatter::ColoredFormatter(const std::string& pattern) : Formatter(pattern) {
        // 如果没有明确设置区域，使用默认区域配置
        std::vector<ColorRegion> default_regions = {
            ColorRegion(ColorRegion::Type::TIMESTAMP, false),
            ColorRegion(" ", false),
            ColorRegion("[", false),
            ColorRegion(ColorRegion::Type::LEVEL, true),
            ColorRegion("]", false),
            ColorRegion(" ", false),
            ColorRegion("[", false),
            ColorRegion(ColorRegion::Type::THREAD_ID, false),
            ColorRegion("]", false),
            ColorRegion(" ", false),
            ColorRegion("[", false),
            ColorRegion(ColorRegion::Type::LOCATION, false),
            ColorRegion("]", false),
            ColorRegion(" ", false),
            ColorRegion(ColorRegion::Type::MESSAGE, true)
        };

        regions_ = default_regions;
    }

    void ColoredFormatter::SetFormat(const std::string& pattern,
        const std::vector<ColorRegion>& regions) {
        Formatter::SetPattern(pattern);
        regions_ = regions;
    }

    std::string ColoredFormatter::Format(const Message& msg) const {
        if (!use_colors_) {
            return Formatter::Format(msg);
        }

        return ApplyColoring(msg);
    }

    std::string ColoredFormatter::ApplyColoring(const Message& msg) const {
        std::ostringstream result;

        for (const auto& region : regions_) {
            std::string text = GetTextForRegion(region, msg);

            if (region.colorize) {
                result << GetColorCode(msg.level) << text << GetResetCode();
            }
            else {
                result << text;
            }
        }

        std::string formatted = result.str();
        if (!formatted.empty() && formatted.back() != '\n') {
            formatted.push_back('\n');
        }

        return formatted;
    }

    std::string ColoredFormatter::GetTextForRegion(const ColorRegion& region, const Message& msg) const {
        switch (region.type) {
        case ColorRegion::Type::TIMESTAMP: {
            auto time = std::chrono::system_clock::to_time_t(msg.timestamp);
            auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                msg.timestamp.time_since_epoch()) % 1000;

            std::tm tm_buf;
#ifdef _WIN32
            localtime_s(&tm_buf, &time);
#else
            localtime_r(&time, &tm_buf);
#endif

            std::ostringstream oss;
            oss << std::put_time(&tm_buf, "%Y-%m-%d %H:%M:%S");
            oss << '.' << std::setfill('0') << std::setw(3) << ms.count();
            return oss.str();
        }

        case ColorRegion::Type::LEVEL:
            return LevelToString(msg.level);

        case ColorRegion::Type::THREAD_ID: {
            std::ostringstream oss;
            oss << msg.thread_id;
            return oss.str();
        }

        case ColorRegion::Type::LOCATION: {
            std::string filename = msg.file;
            size_t pos = filename.find_last_of("/\\");
            if (pos != std::string::npos) {
                filename = filename.substr(pos + 1);
            }
            return filename + ":" + std::to_string(msg.line) + ":" + msg.function;
        }

        case ColorRegion::Type::MESSAGE:
            return msg.content;

        case ColorRegion::Type::CUSTOM_TEXT:
            return region.custom_text;

        default:
            return "";
        }
    }

    std::string ColoredFormatter::GetColorCode(Level level) {
        switch (level) {
        case Level::LVL_TRACE: return "\033[90m";    // 灰色
        case Level::LVL_DEBUG: return "\033[36m";    // 青色
        case Level::LVL_INFO:  return "\033[32m";    // 绿色
        case Level::LVL_WARN:  return "\033[33m";    // 黄色
        case Level::LVL_ERROR: return "\033[31m";    // 红色
        case Level::LVL_FATAL: return "\033[35m";    // 紫色
        default: return "\033[0m";
        }
    }

    std::string ColoredFormatter::GetResetCode() {
        return "\033[0m";
    }

} // namespace Log