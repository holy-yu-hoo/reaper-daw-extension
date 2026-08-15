cmake_minimum_required(VERSION 3.30)
project(reaex CXX)
set(CMAKE_CXX_STANDARD 23)


add_library(reaex MODULE)
target_include_directories(reaex PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/sdk)

target_sources(reaex
	PRIVATE
	main.cpp
	commands.cpp
)

set_target_properties(reaex PROPERTIES OUTPUT_NAME "reaper_reaex")
if(WIN32)
    set_target_properties(reaex PROPERTIES SUFFIX ".dll")
elseif(APPLE)
    set_target_properties(reaex PROPERTIES SUFFIX ".dylib")
else()
    set_target_properties(reaex PROPERTIES SUFFIX ".so")
endif()


