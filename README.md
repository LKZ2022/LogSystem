# LoggingLibrary

一个高性能、可扩展的C++日志库，支持多输出目标、格式化、颜色输出和配置文件。

## 特性

-  **高性能**：零分配格式化，优化同步日志记录
-  **彩色输出**：支持控制台彩色日志，Windows/Linux/macOS兼容
-  **多输出目标**：支持控制台和文件输出，可扩展
-  **灵活配置**：支持配置文件和环境变量
-  **线程安全**：完整的线程安全保证，支持递归调用
-  **现代C++**：使用C++17标准，RAII设计
-  **易用API**：提供便捷的日志宏和格式化功能

## 快速开始

### 1. 安装

#### 使用CMake集成（推荐）

```cmake
# CMakeLists.txt
find_package(LoggingLibrary REQUIRED)

add_executable(myapp main.cpp)
target_link_libraries(myapp PRIVATE LoggingLibrary)
```

#### 作为子模块使用

```bash
# 将LoggingLibrary添加为子模块
git submodule add https://github.com/LKZ2022/LogSystem.git LoggingLibrary
```

```cmake
# CMakeLists.txt
add_subdirectory(LoggingLibrary)

add_executable(myapp main.cpp)
target_link_libraries(myapp PRIVATE LoggingLibrary)
```

### 2. 基础使用

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
    LOG_DEBUG("User {} logged in", "alice");
    LOG_ERROR("Failed to open file: {}", "data.txt");
    
    return 0;
}
```

### 3. 格式化输出

```cpp
LOG_INFO_FMT("Processing {} items, progress: {:.1f}%", count, progress * 100);
LOG_WARN_FMT("Memory usage: {} MB / {} MB", used_memory, total_memory);
```

## 构建指南

### 前提条件

- CMake 3.10+
- C++17兼容编译器
  - GCC 7+
  - Clang 5+
  - MSVC 2017+

### 构建步骤

```bash
# 克隆仓库
git clone https://github.com/LKZ2022/LogSystem.git
cd LogSystem

# 创建构建目录
mkdir build && cd build

# 配置和构建
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release

# 安装（可选）
cmake --install . --prefix /usr/local
```

### 构建选项
    
|         选项         | 默认值 |     说明     |
|----------------------|--------|--------------|
| `LOG_BUILD_SHARED`   |  `OFF` | 构建共享库   |
| `LOG_BUILD_STATIC`   |  `ON`  | 构建静态库   |
| `LOG_BUILD_TESTS`    |  `OFF` | 构建测试     |
| `LOG_BUILD_EXAMPLES` |  `OFF` | 构建示例程序 |
| `LOG_INSTALL`        |  `ON`  | 生成安装目标 |

示例：
```bash
cmake .. \
    -DLOG_BUILD_STATIC=ON \
    -DLOG_BUILD_SHARED=OFF \
    -DLOG_BUILD_EXAMPLES=ON \
    -DCMAKE_BUILD_TYPE=Release
```

## 配置系统

### 配置文件

创建 `config/logging.conf`：

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
max_size = 104857600  # 100MB
max_files = 10
```

使用配置文件初始化：
```cpp
Log::Logger::Instance().InitializeFromFile("config/logging.conf");
```

### 环境变量

```bash
# 设置日志级别
export LOG_LEVEL=DEBUG

# 设置日志文件
export LOG_FILE=/var/log/myapp.log
```

## API文档

### 日志级别

```cpp
enum class Level {
    LVL_TRACE,  // 最详细的调试信息
    LVL_DEBUG,  // 开发调试信息
    LVL_INFO,   // 常规运行信息
    LVL_WARN,   // 警告信息
    LVL_ERROR,  // 错误信息
    LVL_FATAL   // 致命错误
};
```

### 核心API

#### 初始化
```cpp
// 基础初始化
Log::Logger::Instance().Initialize(Log::Level::LVL_DEBUG);

// 从配置文件初始化
Log::Logger::Instance().InitializeFromFile("config/logging.conf");

// 从环境变量初始化
Log::Logger::Instance().InitializeFromEnv();
```

#### 添加输出目标
```cpp
// 添加控制台输出
Log::Logger::Instance().AddConsoleSink(
    Log::Level::LVL_DEBUG,    // 最小级别
    true,                     // 使用颜色
    "[%l] %m"                 // 自定义格式
);

// 添加文件输出
Log::Logger::Instance().AddFileSink(
    "logs/app.log",           // 文件名
    Log::Level::LVL_INFO,     // 最小级别
    "%t %l %m"                // 自定义格式
);
```

#### 日志宏
```cpp
// 基础日志宏
LOG_TRACE("Trace message");
LOG_DEBUG("Debug message");
LOG_INFO("Info message");
LOG_WARN("Warning message");
LOG_ERROR("Error message");
LOG_FATAL("Fatal message");

// 格式化日志宏
LOG_INFO_FMT("User {} logged in", username);
LOG_ERROR_FMT("Failed to open {}: {}", filename, strerror(errno));
```

### 格式化占位符

| 占位符 | 说明 | 示例 |
|--------|------|------|
| `%t` | 时间戳 | 2024-01-18 14:30:45.123 |
| `%l` | 完整日志级别 | INFO |
| `%L` | 短日志级别 | I |
| `%f` | 文件名 | main.cpp |
| `%n` | 行号 | 42 |
| `%F` | 函数名 | main |
| `%T` | 线程ID | 0x7fff12345678 |
| `%m` | 消息内容 | Application started |
| `%%` | 百分号字符 | % |

## 高级用法

### 多线程示例

```cpp
#include <thread>
#include <vector>
#include "log/Log.hpp"

void worker(int id) {
    LOG_INFO("Worker {} started", id);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
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

### 自定义配置

```cpp
#include "log/Log.hpp"

int main() {
    // 创建自定义配置
    Log::Config config;
    config.global_level = Log::Level::LVL_DEBUG;
    
    // 控制台配置
    Log::SinkConfig console_config;
    console_config.type = "console";
    console_config.min_level = Log::Level::LVL_DEBUG;
    console_config.use_colors = true;
    console_config.pattern = "[%T] %l: %m";
    
    // 文件配置
    Log::SinkConfig file_config;
    file_config.type = "file";
    file_config.min_level = Log::Level::LVL_WARN;
    file_config.filename = "logs/errors.log";
    file_config.max_size = 10 * 1024 * 1024;  // 10MB
    file_config.max_files = 5;
    file_config.pattern = "%t %l [%f:%n] %m";
    
    config.sinks.push_back(console_config);
    config.sinks.push_back(file_config);
    
    // 初始化
    Log::Logger::Instance().Initialize(config);
    
    // 使用日志
    LOG_DEBUG("Debug message - visible in console");
    LOG_WARN("Warning message - visible in console and file");
    LOG_ERROR("Error message - visible in console and file");
    
    return 0;
}
```

## 性能优化

### 日志级别过滤
日志级别检查在加锁前进行，避免不必要的锁争用：
```cpp
if (Log::Logger::Instance().GetLevel() <= Log::Level::LVL_DEBUG) {
    LOG_DEBUG("Debug message");
}
```

### 条件日志记录
对于复杂的日志消息，可以使用条件判断：
```cpp
if (should_log_debug) {
    LOG_DEBUG("Complex debug info: {}", compute_expensive_debug_info());
}
```

## 常见问题

### 1. 控制台颜色不显示（Windows）
确保使用支持ANSI颜色的终端，如Windows Terminal或PowerShell。

### 2. 文件无法写入
检查文件路径权限，确保程序有写入权限。

### 3. 多线程性能
对于高并发场景，建议使用适当的日志级别，避免频繁的DEBUG级别日志。

## 贡献指南

欢迎贡献！请遵循以下步骤：

1. Fork 仓库
2. 创建特性分支 (`git checkout -b feature/amazing-feature`)
3. 提交更改 (`git commit -m 'Add amazing feature'`)
4. 推送到分支 (`git push origin feature/amazing-feature`)
5. 开启 Pull Request

### 开发环境设置

```bash
# 克隆并设置开发环境
git clone https://github.com/LKZ2022/LogSystem.git
cd LogSystem

# 使用开发配置构建
mkdir build && cd build
cmake .. -DLOG_BUILD_TESTS=ON -DLOG_BUILD_EXAMPLES=ON -DCMAKE_BUILD_TYPE=Debug
cmake --build .

# 运行测试
ctest --output-on-failure
```

## 许可证

本项目采用 AGPL-3.0-or-later 许可证。

```
Copyright (C) 2026 刘恺知
SPDX-License-Identifier: AGPL-3.0-or-later
```

详细许可证信息请查看 [LICENSE](LICENSE) 文件。

## 联系方式

- **项目主页**: [https://github.com/LKZ2022/LogSystem](https://github.com/LKZ2022/LogSystem)
- **问题反馈**: [GitHub Issues](https://github.com/LKZ2022/LogSystem/issues)
- **作者**: 刘恺知
- **电子邮件**: 3251626446@qq.com

## 相关项目



---

**Star** ⭐ 这个项目如果你觉得它有用！

[![GitHub stars](https://img.shields.io/github/stars/LKZ2022/LogSystem?style=social)](https://github.com/LKZ2022/LogSystem/stargazers)
[![License](https://img.shields.io/badge/License-AGPL%203.0-blue.svg)](https://opensource.org/licenses/AGPL-3.0)