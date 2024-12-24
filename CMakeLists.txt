# CMakeList.txt: 顶层 CMake 项目文件，在此处执行全局配置
# 并包含子项目。
#
cmake_minimum_required(VERSION 3.16)

# 如果支持，请为 MSVC 编译器启用热重载。
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project("PngPortrait2DDS" VERSION 2.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Python3 COMPONENTS Interpreter Development)
find_package(pybind11 REQUIRED)
find_package(soil2 REQUIRED)

find_package(Qt6 REQUIRED COMPONENTS Widgets Concurrent LinguistTools)
qt_standard_project_setup()

set(HEADERS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(RESOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/resource)

set(EXECUTABLE_TARGET_NAME "${CMAKE_PROJECT_NAME}")

include_directories(${HEADERS_DIR})
add_subdirectory("src")

if ("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
	string(REGEX MATCH "^([0-9]+)\\.([0-9]+)\\.[0-9]+" PYTHON_VERSION_MATCH ${Python3_VERSION})
	set(PYTHON_MAJOR_VERSION ${CMAKE_MATCH_1})
	set(PYTHON_MINOR_VERSION ${CMAKE_MATCH_2})
	set(PYTHON_EMBED_DEPS
		"python${PYTHON_MAJOR_VERSION}${CMAKE_SHARED_LIBRARY_SUFFIX}"
		"python${PYTHON_MAJOR_VERSION}${PYTHON_MINOR_VERSION}${CMAKE_SHARED_LIBRARY_SUFFIX}"
		"python${PYTHON_MAJOR_VERSION}${PYTHON_MINOR_VERSION}.zip"
	)
	add_custom_target(
		python_embed_package ALL
		COMMAND
			${CMAKE_COMMAND}
			"-DPYTHON_VERSION=${Python3_VERSION}"
			"-DDEPENDENCIES=${PYTHON_EMBED_DEPS}"
			-P ${CMAKE_SOURCE_DIR}/cmake/download_python_embed_package.cmake
		COMMENT "Preparing Embeddable Package for Python ${Python3_VERSION}"
		USES_TERMINAL
	)
	add_dependencies(
		${EXECUTABLE_TARGET_NAME}
		python_embed_package
	)

	qt_generate_deploy_app_script(
		TARGET ${EXECUTABLE_TARGET_NAME}
		OUTPUT_SCRIPT deploy_script
		NO_UNSUPPORTED_PLATFORM_ERROR
		NO_TRANSLATIONS
		NO_COMPILER_RUNTIME
	)
	install(
		TARGETS ${EXECUTABLE_TARGET_NAME}
		DESTINATION .
	)
	install(SCRIPT ${deploy_script})
	set(INSTALL_DEPS
		"zlib1${CMAKE_SHARED_LIBRARY_SUFFIX}" ${PYTHON_EMBED_DEPS}
	)
	foreach (DEP ${INSTALL_DEPS})
		install(
			FILES ${CMAKE_BINARY_DIR}/${DEP}
			DESTINATION .
		)
	endforeach()
	install(
		FILES "${RESOURCE_DIR}/scripts/Documentation.md"
		DESTINATION scripts
	)

	set(CPACK_PACKAGE_NAME "${CMAKE_PROJECT_NAME}")
	set(CPACK_PACKAGE_VERSION "${CMAKE_PROJECT_VERSION}")
	set(CPACK_GENERATOR "ZIP")
	include(CPack)
endif()