// Copyright (C) 2026 刘恺知
// SPDX-License-Identifier: AGPL-3.0-or-later


// LogLevel.cpp 文件日志级别枚举实现文件

// src/log/LogLevel.cpp

#include "include/LogLevel.hpp"

namespace Log {

    const char* LevelToString(Level level)
    {
        switch (level)
        {
        case Level::LVL_TRACE: return "TRACE";
        case Level::LVL_DEBUG: return "DEBUG";
        case Level::LVL_INFO:  return "INFO ";
        case Level::LVL_WARN:  return "WARN ";
        case Level::LVL_ERROR: return "ERROR";
        case Level::LVL_FATAL: return "FATAL";
        default: return "UNKNOWN";
        }
    }


    const char* LevelToShortString(Level level)
    {
        switch (level)
        {
        case Level::LVL_TRACE: return "T";
        case Level::LVL_DEBUG: return "D";
        case Level::LVL_INFO:  return "I";
        case Level::LVL_WARN:  return "W";
        case Level::LVL_ERROR: return "E";
        case Level::LVL_FATAL: return "F";
        default: return "?";
        }
    }

} // namespace Log