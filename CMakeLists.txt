cmake_minimum_required (VERSION 3.10)

project ("DSCSTools")

if(NOT MSVC)
	set(CMAKE_CXX_FLAGS -pthread)
endif()

# --- Libraries ---
# doboz
add_subdirectory("libs/doboz")

# boost
set(Boost_USE_STATIC_LIBS ON) 

FIND_PACKAGE( Boost 1.65 REQUIRED COMPONENTS filesystem regex)
INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR} )

# Include sub-projects.
add_subdirectory ("DSCSTools")