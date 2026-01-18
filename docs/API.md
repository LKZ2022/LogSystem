# Copyright (C) 2026 刘恺知
# SPDX-License-Identifier: AGPL-3.0-or-later

# LoggingLibrary API 文档

## 概述

LoggingLibrary 是一个高性能、可扩展的C++日志库，支持多输出目标、格式化、颜色输出和配置文件。采用单例设计模式，线程安全，支持同步日志记录。

## 快速开始

### 基础使用

```cpp
#include "log/Log.hpp"

int main() {
    // 初始化日志系统
    Log::Logger::Instance().Initialize(Log::Level::LVL_DEBUG);
    
    // 添加控制台输出（带颜色）
    Log::Logger::Instance().AddConsoleSink(Log::Level::LVL_DEBUG);
    
    // 添加文件输出
    Log::Logger::Instance().AddFileSink("app.log", Log::Level::LVL_INFO);
    
    // 记录日志
    LOG_INFO("Application started");
    LOG_DEBUG("Debug information: {}", 42);
    LOG_ERROR("An error occurred");
    
    return 0;
}
```

## 日志级别

### 枚举定义

```cpp
enum class Level : uint8_t {
    LVL_TRACE = 0,  // 最详细的调试信息
    LVL_DEBUG = 1,  // 开发调试信息
    LVL_INFO = 2,   // 常规运行信息
    LVL_WARN = 3,   // 警告信息
    LVL_ERROR = 4,  // 错误信息
    LVL_FATAL = 5   // 致命错误
};
```

### 级别转换函数

```cpp
// 将级别转换为完整字符串
const char* LevelToString(Level level);

// 将级别转换为短字符串
const char* LevelToShortString(Level level);
```

## 核心类说明

### 1. Logger（日志器单例）

#### 获取实例
```cpp
// 获取Logger单例
static Logger& Instance();
```

#### 初始化方法
```cpp
// 基础初始化
void Initialize(Level level = Level::LVL_INFO);

// 从配置文件初始化
void InitializeFromFile(const std::string& config_path);

// 从环境变量初始化
void InitializeFromEnv();

// 从配置结构体初始化
void Initialize(const Config& config);
```

#### 输出目标管理
```cpp
// 添加任意类型的输出目标
void AddSink(std::unique_ptr<Sink> sink);

// 添加控制台输出（便捷方法）
void AddConsoleSink(Level min_level = Level::LVL_INFO,
                   bool use_colors = true,
                   const std::string& pattern = "");

// 添加文件输出（便捷方法）
void AddFileSink(const std::string& filename,
                Level min_level = Level::LVL_INFO,
                const std::string& pattern = "");
```

#### 级别控制
```cpp
// 设置全局日志级别
void SetLevel(Level level);

// 获取当前日志级别
Level GetLevel() const;
```

#### 其他操作
```cpp
// 刷新所有输出目标
void Flush();

// 递归锁测试（主要用于调试）
void RecursiveTest(int depth = 3);
```

### 2. Sink（输出目标抽象基类）

#### 纯虚函数接口
```cpp
class Sink {
public:
    virtual ~Sink() = default;
    virtual void Write(const Message& msg) = 0;
    virtual void Flush() = 0;
    virtual bool ShouldLog(Level level) const = 0;
    virtual void SetFormatter(std::unique_ptr<Formatter> formatter) = 0;
};
```

### 3. ConsoleSink（控制台输出目标）

#### 构造函数
```cpp
ConsoleSink(Level min_level = Level::LVL_INFO,
           bool use_colors = true,
           const std::string& pattern = "");
```

#### 公共方法
```cpp
// 设置是否使用颜色
void SetUseColors(bool use_colors);
```

### 4. FileSink（文件输出目标）

#### 构造函数
```cpp
FileSink(const std::string& filename,
        Level min_level = Level::LVL_INFO,
        const std::string& pattern = "");
```

### 5. Formatter（格式化器基类）

#### 构造函数
```cpp
Formatter();
explicit Formatter(const std::string& pattern);
```

#### 公共方法
```cpp
// 设置格式模式
virtual void SetPattern(const std::string& pattern);

// 格式化消息
virtual std::string Format(const Message& msg) const;

// 获取当前模式
std::string GetPattern() const;
```

### 6. ColoredFormatter（着色格式化器）

#### 构造函数
```cpp
ColoredFormatter();
explicit ColoredFormatter(const std::string& pattern);
```

#### 公共方法
```cpp
// 设置格式和着色区域
void SetFormat(const std::string& pattern,
               const std::vector<ColorRegion>& regions);

// 设置是否使用颜色
void SetUseColors(bool use_colors);

// 获取颜色代码（静态方法）
static std::string GetColorCode(Level level);
static std::string GetResetCode();
```

## 数据结构

### 1. Message（日志消息）

```cpp
struct Message {
    std::chrono::system_clock::time_point timestamp;  // 时间戳
    Level level;                                      // 日志级别
    std::string file;                                 // 文件名
    int line;                                         // 行号
    std::string function;                             // 函数名
    std::thread::id thread_id;                        // 线程ID
    std::string content;                              // 消息内容
};
```

### 2. Config（配置结构体）

```cpp
struct Config {
    Level global_level{ Level::LVL_INFO };            // 全局级别
    std::vector<SinkConfig> sinks;                    // 输出目标配置
    std::map<std::string, Level> module_levels;       // 模块级别
    
    // 静态工厂方法
    static Config LoadFromFile(const std::string& path);
    static Config LoadFromEnv();
    static Config DefaultConfig();
};
```

### 3. SinkConfig（输出目标配置）

```cpp
struct SinkConfig {
    std::string type;        // "console" 或 "file"
    Level min_level;         // 最小日志级别
    
    // 通用配置
    std::string pattern;     // 格式化模式
    
    // 控制台特有配置
    bool use_colors{ true }; // 是否使用颜色
    
    // 文件特有配置
    std::string filename;    // 文件名
    size_t max_size;         // 最大文件大小（默认100MB）
    int max_files;           // 最大文件数量（默认10）
};
```

## 格式化模式

### 占位符说明

|占位符|     说明     |           示例          |
|------|--------------|-------------------------|
| `%t` |    时间戳    | 2024-01-18 14:30:45.123 |
| `%l` | 完整日志级别 |           INFO          |
| `%L` |  短日志级别  |            I            |
| `%f` |    文件名    |         main.cpp        |
| `%n` |     行号     |            42           |
| `%F` |    函数名    |           main          |
| `%T` |    线程ID    |         12345678        |
| `%m` |   消息内容   |   Application started   |
| `%%` |  百分号字符  |            %            |

### 默认格式
```
%t [%l] [%T] [%f:%n:%F] %m
```

示例输出：
```
2024-01-18 14:30:45.123 [INFO ] [0x7fff12345678] [main.cpp:42:main] Application started
```

## 日志宏

### 基础宏

```cpp
// 直接记录字符串消息
LOG_TRACE(msg)     // 跟踪级别
LOG_DEBUG(msg)     // 调试级别
LOG_INFO(msg)      // 信息级别
LOG_WARN(msg)      // 警告级别
LOG_ERROR(msg)     // 错误级别
LOG_FATAL(msg)     // 致命级别
```

### 格式化宏

```cpp
// 格式化记录消息（支持 {} 占位符）
LOG_TRACE_FMT(fmt, ...)    // 跟踪级别
LOG_DEBUG_FMT(fmt, ...)    // 调试级别
LOG_INFO_FMT(fmt, ...)     // 信息级别
LOG_WARN_FMT(fmt, ...)     // 警告级别
LOG_ERROR_FMT(fmt, ...)    // 错误级别
LOG_FATAL_FMT(fmt, ...)    // 致命级别
```

### 格式化函数

```cpp
// 基础格式化函数（支持 {} 占位符）
template<typename... Args>
std::string FormatString(const std::string& fmt, Args... args);

// 特化：无参数版本
std::string FormatString(const std::string& fmt);
```

示例：
```cpp
LOG_INFO_FMT("User {} logged in from IP {}", username, ip);
LOG_INFO_FMT("Value: {}, Count: {}, Ratio: {}", value, count, ratio);
```

## 配置系统

### 配置文件格式

默认配置文件路径：`config/logging.conf`

```ini
# 全局配置
level = LVL_DEBUG

# 控制台输出
[console]
level = LVL_DEBUG
colors = true
pattern = %t [%l] [%T] [%f:%n] %m

# 文件输出
[file]
level = LVL_INFO
filename = logs/app.log
max_size = 104857600
max_files = 10
```

### 环境变量配置

|   环境变量  |     说明     |        示例        |
|-------------|--------------|--------------------|
| `LOG_LEVEL` | 全局日志级别 | `DEBUG`            |
| `LOG_FILE`  | 文件输出路径 | `/var/log/app.log` |

## 构建与安装

### 构建选项

```bash
# 创建构建目录
mkdir build && cd build

# 配置CMake（默认构建静态库）
cmake .. \
    -DLOG_BUILD_SHARED=OFF \
    -DLOG_BUILD_STATIC=ON \
    -DLOG_BUILD_TESTS=OFF \
    -DLOG_BUILD_EXAMPLES=OFF \
    -DCMAKE_BUILD_TYPE=Release

# 编译
cmake --build . --config Release
```

#### 主要构建选项

| 选项 | 默认值 | 说明 |
|------|--------|------|
| `LOG_BUILD_SHARED` | `OFF` | 构建共享库 |
| `LOG_BUILD_STATIC` | `ON` | 构建静态库 |
| `LOG_BUILD_TESTS` | `OFF` | 构建测试 |
| `LOG_BUILD_EXAMPLES` | `OFF` | 构建示例程序 |
| `LOG_INSTALL` | `ON` | 生成安装目标 |

### 安装

```bash
# 安装到系统目录
cmake --install . --prefix /usr/local

# 或指定安装目录
cmake --install . --prefix /path/to/install
```

### 在项目中集成

#### CMake集成（find_package）

```cmake
find_package(LoggingLibrary REQUIRED)

add_executable(myapp main.cpp)
target_link_libraries(myapp PRIVATE LoggingLibrary)
```

#### CMake集成（子模块）

```cmake
add_subdirectory(LoggingLibrary)

add_executable(myapp main.cpp)
target_link_libraries(myapp PRIVATE LoggingLibrary)
target_include_directories(myapp PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/LoggingLibrary/log
)
```

## 多线程支持

日志库是线程安全的，支持在多线程环境中使用：

```cpp
#include <thread>
#include <vector>

void worker(int id) {
    LOG_INFO("Worker {} started", id);
    // ... 工作 ...
    LOG_INFO("Worker {} finished", id);
}

int main() {
    Log::Logger::Instance().Initialize(Log::Level::LVL_INFO);
    
    std::vector<std::thread> threads;
    for (int i = 0; i < 10; ++i) {
        threads.emplace_back(worker, i);
    }
    
    for (auto& t : threads) {
        t.join();
    }
    
    return 0;
}
```

## 性能特性

1. **同步日志**：当前实现为同步日志，保证日志顺序
2. **递归锁**：使用 `std::recursive_mutex` 支持递归调用
3. **条件判断**：在日志级别检查时加锁，避免不必要的格式化和输出
4. **流式输出**：使用 `std::ostringstream` 进行高效字符串构建

## 平台支持

### 支持的操作系统
- Windows (需要Windows 7+)
- Linux
- macOS

### 编译器要求
- C++17 兼容编译器
- MSVC 2017+ (Windows)
- GCC 7+ (Linux)
- Clang 5+ (macOS/Linux)

## 示例程序

### 基础示例

```cpp
#include "log/Log.hpp"
#include <thread>

int main() {
    // 从配置文件初始化
    Log::Logger::Instance().InitializeFromFile("config/logging.conf");
    
    // 或者手动配置
    // Log::Logger::Instance().Initialize(Log::Level::LVL_DEBUG);
    // Log::Logger::Instance().AddConsoleSink(Log::Level::LVL_DEBUG);
    // Log::Logger::Instance().AddFileSink("app.log", Log::Level::LVL_INFO);
    
    // 记录不同级别的日志
    LOG_TRACE("This is a trace message");
    LOG_DEBUG("Debug information: x = {}, y = {}", 10, 20);
    LOG_INFO("Application initialized successfully");
    LOG_WARN("Warning: disk space low");
    LOG_ERROR("Failed to open file: {}", "data.txt");
    LOG_FATAL("Critical system error occurred");
    
    // 格式化示例
    std::string user = "admin";
    int attempts = 3;
    double memory_usage = 75.5;
    
    LOG_INFO_FMT("User {} logged in after {} attempts", user, attempts);
    LOG_WARN_FMT("Memory usage: {:.1f}%", memory_usage);
    
    return 0;
}
```

### 高级配置示例

```cpp
#include "log/Log.hpp"

int main() {
    // 创建自定义配置
    Log::Config config;
    config.global_level = Log::Level::LVL_DEBUG;
    
    // 控制台输出配置
    Log::SinkConfig console_config;
    console_config.type = "console";
    console_config.min_level = Log::Level::LVL_DEBUG;
    console_config.use_colors = true;
    console_config.pattern = "[%T] %l: %m";
    
    // 文件输出配置
    Log::SinkConfig file_config;
    file_config.type = "file";
    file_config.min_level = Log::Level::LVL_INFO;
    file_config.filename = "logs/application.log";
    file_config.max_size = 50 * 1024 * 1024;  // 50MB
    file_config.max_files = 5;
    file_config.pattern = "%t %l [%f:%n] %m";
    
    config.sinks.push_back(console_config);
    config.sinks.push_back(file_config);
    
    // 初始化日志系统
    Log::Logger::Instance().Initialize(config);
    
    // 使用日志
    for (int i = 0; i < 100; ++i) {
        LOG_DEBUG("Processing item {}", i);
        if (i % 10 == 0) {
            LOG_INFO("Progress: {}%", i);
        }
    }
    
    LOG_INFO("Processing completed");
    
    return 0;
}
```

## 错误处理

### 常见问题

1. **文件无法打开**：如果文件输出目标无法打开文件，错误信息会输出到 `std::cerr`
2. **配置错误**：配置文件解析失败时，会使用默认配置
3. **内存不足**：如果内存分配失败，会抛出 `std::bad_alloc` 异常

### 调试建议

1. 启用 `TRACE` 级别日志进行详细调试
2. 使用不同颜色的控制台输出来区分日志级别
3. 定期检查日志文件大小，避免磁盘空间不足
4. 在多线程环境中测试递归锁功能

## 许可证

本项目采用 AGPL-3.0-or-later 许可证。

```
Copyright (C) 2026 刘恺知
SPDX-License-Identifier: AGPL-3.0-or-later
```

## 贡献指南

1. Fork 项目仓库
2. 创建特性分支
3. 提交更改
4. 确保代码通过测试
5. 创建 Pull Request

## 联系方式

- 项目主页：[GitHub Repository](https://github.com/LKZ2022/LogSystem)
- 问题反馈：[GitHub Issues](https://github.com/LKZ2022/LogSystem/issues)

---

*文档版本：1.0.0*
*最后更新：2026-01-18*