cmake_minimum_required (VERSION 3.10)

project ("DSCSTools")

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

if(NOT MSVC)
	set(CMAKE_CXX_FLAGS -pthread)
endif()

# --- Libraries ---
# doboz
add_subdirectory("libs/doboz")

# boost
set(Boost_USE_STATIC_LIBS ON) 
FIND_PACKAGE(Boost 1.66 REQUIRED COMPONENTS filesystem system date_time regex)

INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR} )

# Qt5

FIND_PACKAGE(Qt5 COMPONENTS Core Widgets)

if(Qt5_FOUND)
	set(CMAKE_AUTOMOC ON)
	set(CMAKE_AUTORCC ON)
	set(CMAKE_AUTOUIC ON)
endif()

# Include sub-projects.
add_subdirectory ("DSCSTools")

install (FILES LICENSE THIRD-PARTY-NOTICE DESTINATION ${CMAKE_BINARY_DIR}/target/license)