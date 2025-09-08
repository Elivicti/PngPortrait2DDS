cmake_minimum_required(VERSION 3.21)

# Add hot reload support for MSVC compiler if available.
if (POLICY CMP0141)
	cmake_policy(SET CMP0141 NEW)
	set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()


project("PngPortrait2DDS" VERSION 3.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Qt project required settings
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
find_package(Qt6 REQUIRED COMPONENTS Widgets)
qt_standard_project_setup()


if (NOT GIT_URL)
	set(GIT_URL "git@github.com:")
endif()


include(FetchContent)

find_package(SimpleReflect QUIET)

if(NOT SimpleReflect_FOUND)
	message(STATUS "SimpleReflect not found, fetching v1.2 from github")
	FetchContent_Declare(
		SimpleReflect
		GIT_REPOSITORY ${GIT_URL}Elivicti/SimpleReflect.git
		GIT_TAG        v1.2
	)
	FetchContent_MakeAvailable(SimpleReflect)
endif()


set(INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")
include_directories(include)
add_subdirectory(source)
