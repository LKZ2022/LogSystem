// Copyright (C) 2026 刘恺知
// SPDX-License-Identifier: AGPL-3.0-or-later


//LogFormatter.cpp 日志格式化器实现文件

// src/log/LogFormatter.cpp


#include "include/LogFormatter.hpp"
#include "include/LogLevel.hpp"
#include <iomanip>
#include <sstream>

namespace Log {

    Formatter::Formatter() {
        // 默认模式
        SetPattern("%t [%l] [%T] [%f:%n:%F] %m");
    }

    Formatter::Formatter(const std::string& pattern) {
        SetPattern(pattern);
    }

    void Formatter::SetPattern(const std::string& pattern) {
        pattern_ = pattern;
        ParsePattern(pattern);
    }

    std::string Formatter::Format(const Message& msg) const {
        std::ostringstream oss;

        for (const auto& part : parts_) {
            if (part.token == FormatToken::TEXT) {
                oss << part.text;
            }
            else {
                oss << ReplaceToken(part.token, msg);
            }
        }

        std::string result = oss.str();
        if (!result.empty() && result.back() != '\n') {
            result.push_back('\n');
        }

        return result;
    }

    void Formatter::ParsePattern(const std::string& pattern) {
        parts_.clear();

        std::string text_buffer;

        for (size_t i = 0; i < pattern.size(); ++i) {
            if (pattern[i] == '%') {
                if (!text_buffer.empty()) {
                    parts_.emplace_back(FormatToken::TEXT, text_buffer);
                    text_buffer.clear();
                }

                if (i + 1 < pattern.size()) {
                    char next = pattern[i + 1];
                    FormatToken token = FormatToken::TEXT;
                    switch (next) {
                    case 't': token = FormatToken::TIMESTAMP; break;
                    case 'l': token = FormatToken::LEVEL; break;
                    case 'L': token = FormatToken::LEVEL_SHORT; break;
                    case 'f': token = FormatToken::FILE; break;
                    case 'n': token = FormatToken::LINE; break;
                    case 'F': token = FormatToken::FUNCTION; break;
                    case 'T': token = FormatToken::THREAD_ID; break;
                    case 'm': token = FormatToken::MESSAGE; break;
                    case '%': token = FormatToken::PERCENT; break;
                    default:
                        text_buffer += '%';
                        text_buffer += next;
                        i++;
                        continue;
                    }
                    parts_.emplace_back(token);
                    i++; // 跳过占位符字符
                }
            }
            else {
                text_buffer += pattern[i];
            }
        }

        if (!text_buffer.empty()) {
            parts_.emplace_back(FormatToken::TEXT, text_buffer);
        }
    }

    std::string Formatter::ReplaceToken(FormatToken token, const Message& msg) const {
        switch (token) {
        case FormatToken::TIMESTAMP: {
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

        case FormatToken::LEVEL:
            return LevelToString(msg.level);

        case FormatToken::LEVEL_SHORT:
            return LevelToShortString(msg.level);

        case FormatToken::FILE: {
            std::string filename = msg.file;
            size_t pos = filename.find_last_of("/\\");
            if (pos != std::string::npos) {
                filename = filename.substr(pos + 1);
            }
            return filename;
        }

        case FormatToken::LINE:
            return std::to_string(msg.line);

        case FormatToken::FUNCTION:
            return msg.function;

        case FormatToken::THREAD_ID: {
            std::ostringstream oss;
            oss << msg.thread_id;
            return oss.str();
        }

        case FormatToken::MESSAGE:
            return msg.content;

        case FormatToken::PERCENT:
            return "%";

        default:
            return "";
        }
    }

} // namespace Log