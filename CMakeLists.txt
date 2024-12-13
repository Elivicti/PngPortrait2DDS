# CMakeList.txt: 顶层 CMake 项目文件，在此处执行全局配置
# 并包含子项目。
#
cmake_minimum_required(VERSION 3.16)

# 如果支持，请为 MSVC 编译器启用热重载。
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project("PngPortrait2DDS" VERSION 2.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Python COMPONENTS Interpreter Development)
find_package(pybind11 REQUIRED)
find_package(soil2 REQUIRED)

find_package(Qt6 REQUIRED COMPONENTS Widgets Concurrent LinguistTools)
qt_standard_project_setup()

set(HEADERS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(RESOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/resource)

include_directories(${HEADERS_DIR})
add_subdirectory("src")
