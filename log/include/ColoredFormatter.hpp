// Copyright (C) 2026 刘恺知
// SPDX-License-Identifier: AGPL-3.0-or-later


// ColoredFormatter.hpp 日志着色格式化器头文件

// include/log/ColoredFormatter.hpp


#pragma once

#include "include/LogFormatter.hpp"
#include <string>
#include <vector>
#include <memory>

namespace Log {

    // 着色区域配置
    struct ColorRegion {
        enum class Type {
            TIMESTAMP,
            LEVEL,
            THREAD_ID,
            LOCATION,
            MESSAGE,
            CUSTOM_TEXT
        };

        Type type;
        std::string custom_text;  // 仅当type为CUSTOM_TEXT时使用
        bool colorize{ true };      // 是否着色

        ColorRegion(Type t, bool c = true) : type(t), colorize(c) {}
        ColorRegion(const std::string& text, bool c = true)
            : type(Type::CUSTOM_TEXT), custom_text(text), colorize(c) {
        }
    };

    class ColoredFormatter : public Formatter {
    public:
        ColoredFormatter();
        explicit ColoredFormatter(const std::string& pattern);

        // 设置格式和着色区域
        void SetFormat(const std::string& pattern,
            const std::vector<ColorRegion>& regions);

        // 格式化消息，带颜色代码
        std::string Format(const Message& msg) const override;

        // 设置是否使用颜色
        void SetUseColors(bool use_colors) { use_colors_ = use_colors; }

        // 获取颜色代码
        static std::string GetColorCode(Level level);
        static std::string GetResetCode();

    private:
        // 着色区域的实现
        std::string ApplyColoring(const Message& msg) const;

        // 根据类型获取文本
        std::string GetTextForRegion(const ColorRegion& region, const Message& msg) const;

    private:
        std::vector<ColorRegion> regions_;
        bool use_colors_{ true };
    };

} // namespace Log