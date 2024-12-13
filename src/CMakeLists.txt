
cmake_minimum_required(VERSION 3.16)

file(GLOB SRC *.cpp)
file(GLOB HEADERS ${HEADERS_DIR}/*.h)
file(GLOB UI  ${CMAKE_CURRENT_SOURCE_DIR}/ui/*.ui)
file(GLOB RC  ${RESOURCE_DIR}/*.rc)
file(GLOB QRC ${RESOURCE_DIR}/*.qrc)
file(GLOB TS  ${RESOURCE_DIR}/*.ts)

qt_add_executable(
	PngPortrait2DDS
		${SRC}
		${HEADERS}
		${UI}
		${RC}
		${QRC}
)

qt_add_translations(
	PngPortrait2DDS
		${TS}
)

target_link_libraries(
	PngPortrait2DDS
	PRIVATE
		Qt6::Widgets
		Qt6::Concurrent
		pybind11::embed
		soil2
)