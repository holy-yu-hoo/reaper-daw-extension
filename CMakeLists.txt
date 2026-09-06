cmake_minimum_required(VERSION 3.30)
project(reaex CXX)
set(CMAKE_CXX_STANDARD 11)

set(REAPER_USER_PLUGINS "C:/Users/yuhoo/AppData/Roaming/REAPER/UserPlugins")

add_library(reaex MODULE)

target_include_directories(reaex
	PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}/sdk
)

set(SOURCES
	main.cpp
	api.cpp
	reaper.cpp
	commands.cpp
	actions/fx.cpp
	midi/midi.cpp
	midi/utils.cpp
	item/item.cpp
	misc/misc.cpp
	misc/utils.cpp
	fx/fx_context.cpp
	fx/fx_target.cpp
	fx/utils.cpp
)

set(HEADERS
	api.h
	commands.h
	<<<<<<< HEAD
	midi/midi.h
	midi/utils.h
	item/item.h
	config.h
	misc/misc.h
	misc/utils.h
	fx/fx_context.h
	fx/fx_target.h
	fx/utils.h
)


target_sources(reaex
	PRIVATE
	${SOURCES}
)


set_target_properties(reaex PROPERTIES OUTPUT_NAME "reaper_reaex")


if (MSVC)
	set(PCH_INPUT "api.h")
	set(PCH_OUTPUT "api.pch")
	target_compile_options(reaex PRIVATE /Yu${PCH_INPUT} /Fp${PCH_OUTPUT})
	set_source_files_properties(reaper.cpp PROPERTIES COMPILE_OPTIONS "/Y-")

	set_source_files_properties(api.cpp PROPERTIES
		COMPILE_OPTIONS "/Yc${PCH_INPUT};/Fp${PCH_OUTPUT}"
		OBJECT_OUTPUTS "${PCH_OUTPUT}"
	)
	target_compile_options(reaex PRIVATE /permissive-)
endif ()

if (WIN32)
	set_target_properties(reaex PROPERTIES SUFFIX ".dll")
elseif (APPLE)
	set_target_properties(reaex PROPERTIES SUFFIX ".dylib")
else ()
	set_target_properties(reaex PROPERTIES SUFFIX ".so")
endif ()


add_custom_command(TARGET reaex POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy
	$<TARGET_FILE:reaex> "${REAPER_USER_PLUGINS}/$<TARGET_FILE_NAME:reaex>"
	VERBATIM
)
