cmake_minimum_required(VERSION 3.21)

file(GLOB WIDGET_HEADERS CONFIGURE_DEPENDS "${INCLUDE_DIR}/widgets/*.h")
file(GLOB WIDGET_SRCS    CONFIGURE_DEPENDS "widgets/*.cpp")

file(GLOB WINDOW_HEADERS CONFIGURE_DEPENDS "${INCLUDE_DIR}/windows/*.h")
file(GLOB WINDOW_SRCS    CONFIGURE_DEPENDS "windows/*.cpp")
file(GLOB UIS            CONFIGURE_DEPENDS "windows/ui/*.ui")

file(GLOB HEADERS CONFIGURE_DEPENDS "${INCLUDE_DIR}/*.h")
file(GLOB SRCS    CONFIGURE_DEPENDS "*.cpp")

qt_add_executable(
	${PROJECT_NAME}
		${SRCS}
		${HEADERS}
		${UIS}
		${WIDGET_HEADERS}
		${WIDGET_SRCS}
		${WINDOW_HEADERS}
		${WINDOW_SRCS}
)

target_link_libraries(
	${PROJECT_NAME}
	PRIVATE
		Qt6::Widgets
		SimpleReflect
)

# Disables console window on Windows if ENABLE_CONSOLE is set to OFF (Qt Specific)
if (NOT ${ENABLE_CONSOLE})
	set_target_properties(
		${PROJECT_NAME}
		PROPERTIES
			WIN32_EXECUTABLE ON
	)
endif()
