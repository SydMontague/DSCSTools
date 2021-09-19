cmake_minimum_required (VERSION 3.12)

project ("DSCSTools" 
            VERSION 1.0.0 
            DESCRIPTION "Tools for Digimon Story Cyber Sleuth"
            HOMEPAGE_URL "https://github.com/SydMontague/DSCSTools")

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
set(CMAKE_CXX_STANDARD 14)

if (MSVC)
	add_definitions("-D_CRT_SECURE_NO_WARNINGS -D_WIN32_WINNT=0x0601") # Windows 7
else()
	set(CMAKE_CXX_FLAGS -pthread)
endif()

option(BUILD_SHARED "Build Using Shared Libs" OFF)

# --- Libraries ---
# doboz
add_subdirectory("libs/doboz")

# python
FIND_PACKAGE(Python3)
FIND_PACKAGE(PythonLibs REQUIRED)

# boost
if(WIN32)
    set(Boost_USE_STATIC_LIBS ON) 
elseif(BUILD_SHARED)
    set(Boost_USE_STATIC_LIBS OFF) 
endif()

FIND_PACKAGE(Boost 1.66 REQUIRED COMPONENTS filesystem system date_time regex python${Python3_VERSION_MAJOR}${Python3_VERSION_MINOR})

INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR} ${PYTHON_INCLUDE_DIRS} )

# Include sub-projects.
add_subdirectory ("DSCSTools")
add_subdirectory ("DSCSToolsCLI")

install (FILES LICENSE THIRD-PARTY-NOTICE DESTINATION ${CMAKE_BINARY_DIR}/target/license)
