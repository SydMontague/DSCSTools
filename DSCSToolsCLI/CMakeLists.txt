cmake_minimum_required (VERSION 3.8)

include_directories(../DSCSTools/include)

SET(EXE_SOURCE_FILES
		DSCSTools.cpp
        )

# Add source to this project's executable.
add_executable (DSCSToolsCLI ${EXE_SOURCE_FILES})
target_link_libraries(DSCSToolsCLI PRIVATE DSCSTools)

# Install into /target/ folder
install (TARGETS DSCSToolsCLI DESTINATION ${CMAKE_BINARY_DIR}/target)
