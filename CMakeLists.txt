cmake_minimum_required(VERSION 3.30)
project(reaex CXX)
set(CMAKE_CXX_STANDARD 23)

set(REAPER_USER_PLUGINS "C:/Users/yuhoo/AppData/Roaming/REAPER/UserPlugins")

add_library(reaex MODULE)
target_include_directories(reaex PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/sdk)

target_sources(reaex
	PRIVATE
	main.cpp
	core/api_load.cpp
	commands/actions.cpp
	commands/commands.cpp
	utils/utils.cpp
)


set_target_properties(reaex PROPERTIES OUTPUT_NAME "reaper_reaex")
if (WIN32)
	set_target_properties(reaex PROPERTIES SUFFIX ".dll")
elseif (APPLE)
	set_target_properties(reaex PROPERTIES SUFFIX ".dylib")
else ()
	set_target_properties(reaex PROPERTIES SUFFIX ".so")
endif ()


add_custom_command(TARGET reaex POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_if_different
	$<TARGET_FILE:reaex> "${REAPER_USER_PLUGINS}/$<TARGET_FILE_NAME:reaex>"
	VERBATIM
)

