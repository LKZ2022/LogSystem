// Copyright (C) 2026 刘恺知
// SPDX-License-Identifier: AGPL-3.0-or-later

//LogLevel.hpp 日志级别枚举

// include/log/LogLevel.hpp
#pragma once

#include<cstdint>

namespace Log
{

    // 日志级别枚举
    enum class Level : uint8_t
    {
        LVL_TRACE = 0,  // 最详细的调试信息
        LVL_DEBUG = 1,  // 开发调试信息
        LVL_INFO = 2,  // 常规运行信息
        LVL_WARN = 3,  // 警告信息
        LVL_ERROR = 4,  // 错误信息
        LVL_FATAL = 5   // 致命错误
    };

    // 级别转字符串
    const char* LevelToString(Level level);

    // 级别转短字符串
    const char* LevelToShortString(Level level);

} // namespace Log