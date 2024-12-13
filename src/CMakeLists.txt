
cmake_minimum_required(VERSION 3.16)

file(GLOB SRC *.cpp)
file(GLOB UI ${CMAKE_CURRENT_SOURCE_DIR}/ui/*.ui)

qt_add_executable(
	PngPortrait2DDS
		${SRC}
		${UI}
)

target_link_libraries(
	PngPortrait2DDS
	PRIVATE
		Qt6::Widgets
)