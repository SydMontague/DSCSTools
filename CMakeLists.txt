cmake_minimum_required (VERSION 3.12)

project ("DSCSTools")

# --- Libraries ---
# doboz
add_subdirectory("libs/doboz")

# boost
set(Boost_USE_STATIC_LIBS ON) 

FIND_PACKAGE( Boost 1.71 REQUIRED COMPONENTS filesystem)
INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR} )

# Include sub-projects.
add_subdirectory ("DSCSTools")