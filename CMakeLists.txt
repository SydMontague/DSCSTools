cmake_minimum_required (VERSION 3.14)

project ("DSCSTools" 
            VERSION 1.0.0 
            DESCRIPTION "Tools for Digimon Story Cyber Sleuth"
            HOMEPAGE_URL "https://github.com/SydMontague/DSCSTools")

set(CMAKE_CXX_STANDARD 17)

include(cmake/CPM.cmake)

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

CPMAddPackage(
  NAME Boost
  VERSION 1.80.0
  GITHUB_REPOSITORY "boostorg/boost"
  GIT_TAG "boost-1.80.0"
)

CPMAddPackage(
  NAME BoostPython
  VERSION 1.80.0
  GITHUB_REPOSITORY "boostorg/python"
  GIT_TAG "boost-1.80.0"
)

INCLUDE_DIRECTORIES(${PYTHON_INCLUDE_DIRS})

# Include sub-projects.
add_subdirectory ("DSCSTools")
add_subdirectory ("DSCSToolsCLI")

install (FILES LICENSE THIRD-PARTY-NOTICE DESTINATION ${CMAKE_BINARY_DIR}/target/license)

# Ugly hack to make sure install works
file(TOUCH ${CMAKE_BINARY_DIR}/_deps/boost-build/cmake_install.cmake)