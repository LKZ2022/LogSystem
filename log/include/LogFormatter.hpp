// Copyright (C) 2026 刘恺知
// SPDX-License-Identifier: AGPL-3.0-or-later


//LogFormatter.hpp 日志格式化器头文件

//	include/log/LogFormatter.hpp


#pragma once

#include "include/LogMessage.hpp"
#include <string>
#include <vector>
#include <map>

namespace Log {

    // 格式化模式占位符
    enum class FormatToken {
        TIMESTAMP,    // %t 时间戳
        LEVEL,        // %l 日志级别
        LEVEL_SHORT,  // %L 日志级别（短）
        FILE,         // %f 文件名
        LINE,         // %n 行号
        FUNCTION,     // %F 函数名
        THREAD_ID,    // %T 线程ID
        MESSAGE,      // %m 消息内容
        PERCENT,      // %% 百分号
        TEXT          // 普通文本
    };

    struct FormatPart {
        FormatToken token;
        std::string text;

        FormatPart(FormatToken t, const std::string& txt = "")
            : token(t), text(txt) {
        }
    };

    class Formatter {
    public:
        Formatter();
        explicit Formatter(const std::string& pattern);
        virtual ~Formatter() = default;

        // 设置格式
        virtual void SetPattern(const std::string& pattern);

        // 格式化消息
        virtual std::string Format(const Message& msg) const;

        // 获取当前模式
        std::string GetPattern() const { return pattern_; }

    protected:
        // 解析模式字符串
        void ParsePattern(const std::string& pattern);

        // 替换占位符
        virtual std::string ReplaceToken(FormatToken token, const Message& msg) const;

    protected:
        std::string pattern_;
        std::vector<FormatPart> parts_;
    };

} // namespace Log