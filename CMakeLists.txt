cmake_minimum_required(VERSION 3.24)

project(standby-blocker VERSION 0.1.0)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS OFF)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cpm")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

option(USE_LTO "Enable LTO" ON)

set(BUILD_STATIC ON)

if(USE_LTO AND NOT (CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_BUILD_TYPE STREQUAL "MinSizeDbg"))
	include(CheckIPOSupported)
	check_ipo_supported(RESULT supported OUTPUT error)
	set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()

# configure_file(assets/app.rc.in app.rc @ONLY)

add_executable(standby_blocker
	src/main.cpp
)

# if(WIN32)
# 	target_sources(standby_blocker PRIVATE
# 		assets/manifest.manifest
# 		${CMAKE_CURRENT_BINARY_DIR}/app.rc
# 	)
# endif()

target_include_directories(standby_blocker PRIVATE
	inc
)

target_compile_options(standby_blocker PRIVATE
	"$<$<CONFIG:RELEASE>:-O3;-DNDEBUG>"
	"$<$<CONFIG:DEBUG>:-Og;-DDEBUG;-funwind-tables;-fno-inline;-fno-omit-frame-pointer>"
)

include(crow)
include(fmt)
include(spdlog)
include(compiler_warnings)

target_link_libraries(standby_blocker PRIVATE
	Crow
	fmt
	spdlog
)