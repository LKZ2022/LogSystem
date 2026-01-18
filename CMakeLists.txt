# Copyright (C) 2026 刘恺知
# SPDX-License-Identifier: AGPL-3.0-or-later

# CMakeLists.txt C++日志库构建配置

# /CMakeLists.txt


cmake_minimum_required(VERSION 3.10)
project(LoggingLibrary VERSION 1.0.0 LANGUAGES CXX)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 项目信息
set(PROJECT_DESCRIPTION "一个高性能、可扩展的C++日志库，支持多输出目标、格式化、颜色输出和配置文件。")
set(PROJECT_HOMEPAGE_URL "https://github.com/LKZ2022/LogSystem")

# 输出目录设置
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# 配置编译选项
if(MSVC)
    add_compile_options(/W4 /WX /EHsc /MP)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    add_compile_options("$<$<CONFIG:RELEASE>:/O2>")
    add_compile_options("$<$<CONFIG:DEBUG>:/Od /Zi>")
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror -Wno-unused-parameter)
    add_compile_options("$<$<CONFIG:RELEASE>:-O3>")
    add_compile_options("$<$<CONFIG:DEBUG>:-O0 -g>")
    add_compile_options(-fvisibility=hidden)
endif()

# 库配置选项
option(LOG_BUILD_SHARED "Build shared library" OFF)
option(LOG_BUILD_STATIC "Build static library" ON)
option(LOG_BUILD_TESTS "Build tests" OFF)
option(LOG_BUILD_EXAMPLES "Build examples" OFF)
option(LOG_INSTALL "Generate install target" ON)

# 包含目录
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/log)

# 源文件列表
set(LOG_SOURCES
    log/src/ColoredFormatter.cpp
    log/src/ConsoleSink.cpp
    log/src/FileSink.cpp
    log/src/LogConfig.cpp
    log/src/LogFormatter.cpp
    log/src/Logger.cpp
    log/src/LogLevel.cpp
)

# 创建静态库
if(LOG_BUILD_STATIC)
    add_library(LoggingLibrary_static STATIC ${LOG_SOURCES})
    set_target_properties(LoggingLibrary_static PROPERTIES
        OUTPUT_NAME "logging"
        VERSION ${PROJECT_VERSION}
        SOVERSION ${PROJECT_VERSION_MAJOR}
    )
    target_include_directories(LoggingLibrary_static PUBLIC 
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/log>
        $<INSTALL_INTERFACE:include>
    )
    target_compile_definitions(LoggingLibrary_static PUBLIC LOGGING_STATIC)
endif()

# 创建共享库
if(LOG_BUILD_SHARED)
    add_library(LoggingLibrary_shared SHARED ${LOG_SOURCES})
    set_target_properties(LoggingLibrary_shared PROPERTIES
        OUTPUT_NAME "logging"
        VERSION ${PROJECT_VERSION}
        SOVERSION ${PROJECT_VERSION_MAJOR}
        WINDOWS_EXPORT_ALL_SYMBOLS ON
    )
    target_include_directories(LoggingLibrary_shared PUBLIC 
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/log>
        $<INSTALL_INTERFACE:include>
    )
    
    # 设置共享库的导出宏
    if(WIN32)
        target_compile_definitions(LoggingLibrary_shared PRIVATE LOGGING_EXPORTS)
    endif()
    
    # 设置别名以便于使用
    add_library(LoggingLibrary ALIAS LoggingLibrary_shared)
elseif(LOG_BUILD_STATIC)
    add_library(LoggingLibrary ALIAS LoggingLibrary_static)
endif()

# 平台特定设置
if(WIN32)
    # Windows平台设置
    add_definitions(-D_WIN32_WINNT=0x0601)  # Windows 7+
    
    # 链接Windows库
    if(LOG_BUILD_SHARED)
        target_link_libraries(LoggingLibrary_shared PRIVATE kernel32 user32)
    endif()
    if(LOG_BUILD_STATIC)
        target_link_libraries(LoggingLibrary_static PRIVATE kernel32 user32)
    endif()
else()
    # Unix/Linux平台设置
    find_package(Threads REQUIRED)
    
    if(LOG_BUILD_SHARED)
        target_link_libraries(LoggingLibrary_shared PRIVATE Threads::Threads)
    endif()
    if(LOG_BUILD_STATIC)
        target_link_libraries(LoggingLibrary_static PRIVATE Threads::Threads)
    endif()
endif()

# 构建示例程序
if(LOG_BUILD_EXAMPLES)
    # 示例1: 基础使用
    add_executable(logging_example_basic examples/basic_example.cpp)
    if(LOG_BUILD_SHARED)
        target_link_libraries(logging_example_basic PRIVATE LoggingLibrary_shared)
    else()
        target_link_libraries(logging_example_basic PRIVATE LoggingLibrary_static)
    endif()
    
    # 示例2: 高级配置
    add_executable(logging_example_advanced examples/advanced_example.cpp)
    if(LOG_BUILD_SHARED)
        target_link_libraries(logging_example_advanced PRIVATE LoggingLibrary_shared)
    else()
        target_link_libraries(logging_example_advanced PRIVATE LoggingLibrary_static)
    endif()
    
    # 示例3: 多线程
    add_executable(logging_example_threads examples/threads_example.cpp)
    if(LOG_BUILD_SHARED)
        target_link_libraries(logging_example_threads PRIVATE LoggingLibrary_shared)
    else()
        target_link_libraries(logging_example_threads PRIVATE LoggingLibrary_static)
    endif()
endif()

# 构建测试
if(LOG_BUILD_TESTS)
    enable_testing()
    
    # 查找测试框架
    find_package(GTest QUIET)
    
    if(GTest_FOUND)
        # 单元测试
        add_executable(logging_tests
            tests/test_log_level.cpp
            tests/test_formatter.cpp
            tests/test_sinks.cpp
            tests/test_logger.cpp
            tests/test_config.cpp
        )
        
        if(LOG_BUILD_SHARED)
            target_link_libraries(logging_tests PRIVATE
                LoggingLibrary_shared
                GTest::gtest
                GTest::gtest_main
            )
        else()
            target_link_libraries(logging_tests PRIVATE
                LoggingLibrary_static
                GTest::gtest
                GTest::gtest_main
            )
        endif()
        
        add_test(NAME logging_tests COMMAND logging_tests)
        
        # 性能测试
        add_executable(logging_benchmark tests/benchmark.cpp)
        if(LOG_BUILD_SHARED)
            target_link_libraries(logging_benchmark PRIVATE
                LoggingLibrary_shared
                GTest::gtest
            )
        else()
            target_link_libraries(logging_benchmark PRIVATE
                LoggingLibrary_static
                GTest::gtest
            )
        endif()
    else()
        message(WARNING "GTest not found, tests will not be built")
    endif()
endif()

# 安装配置
if(LOG_INSTALL)
    # 安装Log.hpp
    install(FILES log/Log.hpp DESTINATION include/log)
    
    # 安装其他头文件
    install(DIRECTORY log/include/ DESTINATION include/log/include)
    
    # 安装库文件
    if(LOG_BUILD_STATIC)
        install(TARGETS LoggingLibrary_static
                EXPORT LoggingLibraryTargets
                ARCHIVE DESTINATION lib
                LIBRARY DESTINATION lib
                RUNTIME DESTINATION bin)
    endif()
    
    if(LOG_BUILD_SHARED)
        install(TARGETS LoggingLibrary_shared
                EXPORT LoggingLibraryTargets
                ARCHIVE DESTINATION lib
                LIBRARY DESTINATION lib
                RUNTIME DESTINATION bin)
    endif()
    
    # 生成和安装配置包
    include(CMakePackageConfigHelpers)
    write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/LoggingLibraryConfigVersion.cmake"
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY AnyNewerVersion
    )
    
    configure_package_config_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/LoggingLibraryConfig.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/LoggingLibraryConfig.cmake"
        INSTALL_DESTINATION lib/cmake/LoggingLibrary
    )
    
    install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/LoggingLibraryConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/LoggingLibraryConfigVersion.cmake"
        DESTINATION lib/cmake/LoggingLibrary
    )
    
    install(EXPORT LoggingLibraryTargets
            FILE LoggingLibraryTargets.cmake
            DESTINATION lib/cmake/LoggingLibrary)
endif()

# 创建配置摘要
message(STATUS "=========================================")
message(STATUS "Logging Library Configuration Summary")
message(STATUS "=========================================")
message(STATUS "Project:        ${PROJECT_NAME}")
message(STATUS "Version:        ${PROJECT_VERSION}")
message(STATUS "Build Type:     ${CMAKE_BUILD_TYPE}")
message(STATUS "Build shared:   ${LOG_BUILD_SHARED}")
message(STATUS "Build static:   ${LOG_BUILD_STATIC}")
message(STATUS "Build tests:    ${LOG_BUILD_TESTS}")
message(STATUS "Build examples: ${LOG_BUILD_EXAMPLES}")
message(STATUS "Install:        ${LOG_INSTALL}")
message(STATUS "C++ Standard:   ${CMAKE_CXX_STANDARD}")
message(STATUS "=========================================")

# 添加自定义目标
add_custom_target(check
    COMMAND ctest --output-on-failure
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Running tests..."
)

add_custom_target(doc
    COMMAND doxygen ${CMAKE_SOURCE_DIR}/Doxyfile
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Generating documentation..."
)