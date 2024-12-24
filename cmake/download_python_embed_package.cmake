cmake_minimum_required(VERSION 3.20)

#### Variable Options
# PYTHON_VERSION (REQUIRED) recommand set to ${Python_VERSION} or ${Python3_VERSION}
# TARGET_DIR   where to extract the downloaded package [default: ${CMAKE_BINARY_DIR}] 
# DOWNLOAD_DST where to store the downloaded zip file  [default: ${CMAKE_BINARY_DIR}/external_deps]
# DEPENDENCIES dependency file to extract from the zip file (type: List)
#              [default(example): python3.dll;python311.dll;python311.zip]


if (NOT TARGET_DIR)
	set(TARGET_DIR ${CMAKE_BINARY_DIR})
endif()

# Prepare directories
if (NOT DOWNLOAD_DST)
	set(DOWNLOAD_DST ${CMAKE_BINARY_DIR}/external_deps)
endif()
file(MAKE_DIRECTORY ${DOWNLOAD_DST})

if (NOT EXISTS TARGET_DIR)
	file(MAKE_DIRECTORY ${TARGET_DIR})
endif()


string(REGEX MATCH "^([0-9]+)\\.([0-9]+)\\.([0-9]+)" PYTHON_VERSION_MATCH ${PYTHON_VERSION})
set(PYTHON_MAJOR_VERSION ${CMAKE_MATCH_1})
set(PYTHON_MINOR_VERSION ${CMAKE_MATCH_2})
set(PYTHON_PATCH_VERSION ${CMAKE_MATCH_3})

set(DOWNLOAD_SUCCESS FALSE)
set(VERSION_MAIN  "${PYTHON_MAJOR_VERSION}.${PYTHON_MINOR_VERSION}")
set(VERSION_SUFFIX ${PYTHON_PATCH_VERSION})

set(DST_DIR ${DOWNLOAD_DST})

message(STATUS "Downloading suitable Python Embeddable Package")
while (NOT DOWNLOAD_SUCCESS AND VERSION_SUFFIX GREATER -1)
	# 拼接下载 URL
	set(CURRENT_VERSION "${VERSION_MAIN}.${VERSION_SUFFIX}")
	set(PACKAGE_NAME "python-${CURRENT_VERSION}-embed-amd64.zip")
	set(PACKAGE_URL  "https://www.python.org/ftp/python/${CURRENT_VERSION}/${PACKAGE_NAME}")
	set(PACKAGE_PATH "${DOWNLOAD_DST}/${PACKAGE_NAME}")
		
	if (EXISTS "${PACKAGE_PATH}")
		file(SIZE "${PACKAGE_PATH}" PACKAGE_SIZE)
		if (PACKAGE_SIZE GREATER 8388608) # 8 * 1024 * 1024 Bytes
			message(STATUS "> Using existing Python Embeddable Package: ${PACKAGE_PATH}")
			set(DOWNLOAD_SUCCESS TRUE)
			break()
		endif()
	endif()
	# 尝试下载
	message(STATUS "> Attempting to download Python Embeddable Package for ${CURRENT_VERSION}")
	file(DOWNLOAD ${PACKAGE_URL} ${PACKAGE_PATH} SHOW_PROGRESS STATUS DL_STATUS)

	# 检查是否成功

	list(GET DL_STATUS 0 DL_RESULT)
	if (DL_RESULT EQUAL 0)
		set(DOWNLOAD_SUCCESS TRUE)
		message(STATUS "> Succeeded: ${PACKAGE_PATH}")
	else()
		# 如果失败，尝试下一个版本
		message(STATUS "> Failed: ${DL_RESULT}")
		file(REMOVE "${PACKAGE_PATH}")
		math(EXPR VERSION_SUFFIX "${VERSION_SUFFIX} - 1")
	endif()
endwhile()

# 如果所有版本都失败，则报错
if (NOT DOWNLOAD_SUCCESS)
	message(FATAL_ERROR "Failed to download Python Embeddable Package for Python ${VERSION_MAIN}. No suitable patch version found.")
endif()

message(STATUS "Extracting ${PACKAGE_PATH}")


set(LIB_SUFFIX "${CMAKE_SHARED_LIBRARY_SUFFIX}")

if (NOT DEPENDENCIES)
	set(DEPENDENCIES
		"python${PYTHON_MAJOR_VERSION}${LIB_SUFFIX}"
		"python${PYTHON_MAJOR_VERSION}${PYTHON_MINOR_VERSION}${LIB_SUFFIX}"
		"python${PYTHON_MAJOR_VERSION}${PYTHON_MINOR_VERSION}.zip"
	)
else()
	#set(DEPENDENCIES LIST ${DEPENDENCIES})
endif()

foreach(DEP ${DEPENDENCIES})
	if (EXISTS "${TARGET_DIR}/${DEP}")
		message(STATUS "> ${TARGET_DIR}/${DEP} exists, skipping")
		continue()
	endif()
	message(STATUS "> extracting ${DEP}")
	execute_process(
		COMMAND
			${CMAKE_COMMAND} -E tar xzf ${PACKAGE_PATH} ${DEP}
		WORKING_DIRECTORY ${TARGET_DIR}
		RESULT_VARIABLE EXTRACT_RESULT
	)
	if (NOT DL_RESULT EQUAL 0)
		message(FATAL_ERROR "Error while extracting ${DEP}: ${EXTRACT_RESULT}")
	endif()
endforeach()