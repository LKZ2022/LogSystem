// Copyright (C) 2026 刘恺知
// SPDX-License-Identifier: AGPL-3.0-or-later


// Format.hpp 格式化函数头文件


// include/log/Format.hpp



#pragma once

#include <string>
#include <sstream>
#include <type_traits>

namespace Log
{

    namespace detail
    {
        template<typename T>
        std::string ToString(const T& value)
        {
            if constexpr (std::is_same_v<T, std::string>)
            {
                return value;
            }
            else if constexpr (std::is_same_v<T, const char*> || std::is_same_v<T, char*>)
            {
                return value ? std::string(value) : "";
            }
            else if constexpr (std::is_arithmetic_v<T>)
            {
                return std::to_string(value);
            }
            else {
                std::ostringstream oss;
                oss << value;
                return oss.str();
            }
        }
    }

    // 基础格式化函数
    template<typename... Args>
    std::string FormatString(const std::string& fmt, Args... args)
    {
        std::ostringstream oss;
        size_t pos = 0;

        std::string args_str[] = { detail::ToString(args)... };
        size_t arg_count = sizeof...(args);

        for (size_t i = 0; i < arg_count; ++i) {
            std::string placeholder = "{" + std::to_string(i) + "}";
            size_t placeholder_pos = fmt.find(placeholder, pos);

            if (placeholder_pos != std::string::npos) {
                oss << fmt.substr(pos, placeholder_pos - pos);
                oss << args_str[i];
                pos = placeholder_pos + placeholder.length();
            }
        }

        oss << fmt.substr(pos);

        return oss.str();
    }

    // 特化：无参数版本
    inline std::string FormatString(const std::string& fmt) {
        return fmt;
    }

} // namespace Log