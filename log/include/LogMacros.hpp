// Copyright (C) 2026 刘恺知
// SPDX-License-Identifier: AGPL-3.0-or-later

//LogMacros.hpp 日志宏定义文件



// include/log/LogMacros.hpp
#pragma once

#include "include/Logger.hpp"
#include "include/Format.hpp"

// 基础日志宏
#define LOG_TRACE(msg) \
    Log::Logger::Instance().Log(Log::Level::LVL_TRACE, \
                                          __FILE__, __LINE__, __func__, std::string(msg))

#define LOG_DEBUG(msg) \
    Log::Logger::Instance().Log(Log::Level::LVL_DEBUG, \
                                          __FILE__, __LINE__, __func__, std::string(msg))

#define LOG_INFO(msg) \
    Log::Logger::Instance().Log(Log::Level::LVL_INFO, \
                                          __FILE__, __LINE__, __func__, std::string(msg))

#define LOG_WARN(msg) \
    Log::Logger::Instance().Log(Log::Level::LVL_WARN, \
                                          __FILE__, __LINE__, __func__, std::string(msg))

#define LOG_ERROR(msg) \
    Log::Logger::Instance().Log(Log::Level::LVL_ERROR, \
                                          __FILE__, __LINE__, __func__, std::string(msg))

#define LOG_FATAL(msg) \
    Log::Logger::Instance().Log(Log::Level::LVL_FATAL, \
                                          __FILE__, __LINE__, __func__, std::string(msg))

// 带格式化的日志宏
#define LOG_TRACE_FMT(fmt, ...) \
    LOG_TRACE(Log::FormatString(fmt, ##__VA_ARGS__))

#define LOG_DEBUG_FMT(fmt, ...) \
    LOG_DEBUG(Log::FormatString(fmt, ##__VA_ARGS__))

#define LOG_INFO_FMT(fmt, ...) \
    LOG_INFO(Log::FormatString(fmt, ##__VA_ARGS__))

#define LOG_WARN_FMT(fmt, ...) \
    LOG_WARN(Log::FormatString(fmt, ##__VA_ARGS__))

#define LOG_ERROR_FMT(fmt, ...) \
    LOG_ERROR(Log::FormatString(fmt, ##__VA_ARGS__))

#define LOG_FATAL_FMT(fmt, ...) \
    LOG_FATAL(Log::FormatString(fmt, ##__VA_ARGS__))
                                 