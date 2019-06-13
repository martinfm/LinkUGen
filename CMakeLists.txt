cmake_minimum_required(VERSION 2.6)

project(Link)

add_library(Link SHARED src/Link.cpp)

set (CMAKE_CXX_STANDARD 11)

set(SC3_SRC "${PROJECT_SOURCE_DIR}/supercollider")
set(LINK_SRC "${PROJECT_SOURCE_DIR}/link")

add_definitions(-DASIO_HAS_STD_TYPE_TRAITS 
                -DASIO_HAS_STD_ARRAY
                -DASIO_HAS_STD_SHARED_PTR
                -DASIO_HAS_CSTDINT
                -DASIO_HAS_STD_SHARED_PTR
                -DASIO_HAS_STD_ADDRESSOF
                -DASIO_HAS_STD_ATOMIC)

target_include_directories(Link PRIVATE
	"/usr/local/include"
	"${SC3_SRC}/include/plugin_interface"
	"${SC3_SRC}/include/common"
	"${LINK_SRC}/include"
	"${LINK_SRC}/modules/asio-standalone/asio/include"
)

if(APPLE)
	if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
		set_target_properties(Link PROPERTIES SUFFIX ".scx")
		target_compile_definitions(Link PRIVATE LINK_PLATFORM_MACOSX)
	elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
		message( FATAL_ERROR "Only Mac OS X supported when __APPLE__ is defined" )
	endif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin") 
elseif(UNIX)
	if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
		set (CMAKE_SHARED_LINKER_FLAGS ${CMAKE_SHARED_LINKER_FLAGS_INIT} "-lpthread -latomic")
		set_target_properties(Link PROPERTIES SUFFIX ".so")
		target_compile_definitions(Link PRIVATE LINK_PLATFORM_LINUX=1)
	elseif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
		message( FATAL_ERROR "Only Linux supported for non-Apple Unix platforms" )
	endif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
elseif(WIN32)
	set_target_properties(Link PROPERTIES SUFFIX ".scx")
	target_compile_definitions(Link PRIVATE 
		LINK_PLATFORM_WINDOWS=1
		_CRT_SECURE_NO_WARNINGS
		WIN32
		_WINDOWS
		_WIN32_WINNT=0x600
		NOMINMAX=1
		$<$<CONFIG:Debug>:DEBUG>
		$<$<CONFIG:Debug>:_DEBUG>
		$<$<CONFIG:Release>:NDEBUG>
	)
endif(APPLE)

if(WIN32)

target_include_directories(Link PRIVATE 
	"${SC3_SRC}/include/plugin_interface"
	"${SC3_SRC}/common"
	"${LINK_SRC}/include"
	"${LINK_SRC}/modules/asio-standalone/asio/include"
)

install(TARGETS Link DESTINATION "./AppData/Local/SuperCollider/Extensions")
install(FILES "${PROJECT_SOURCE_DIR}/Classes/Link.sc" DESTINATION "./AppData/Local/SuperCollider/Extensions")
#install(DESTINATION )
endif(WIN32)






