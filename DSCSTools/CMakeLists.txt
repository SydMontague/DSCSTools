cmake_minimum_required (VERSION 3.8)

SET(EXE_SOURCE_FILES
		DSCSTools.cpp
		DSCSTools.h
        )

SET(SOURCE_FILES
		MDB1.cpp
		MDB1.h
		EXPA.cpp
		EXPA.h
		SaveFile.cpp
		SaveFile.h
		AFS2.cpp
		AFS2.h
		)

# Add source to this project's executable.
add_executable (DSCSTools ${EXE_SOURCE_FILES} ${SOURCE_FILES})
target_link_libraries(DSCSTools PUBLIC doboz ${Boost_LIBRARIES})

# Install into /target/ folder
install (TARGETS DSCSTools DESTINATION ${CMAKE_BINARY_DIR}/target)
install (DIRECTORY structures/ DESTINATION ${CMAKE_BINARY_DIR}/target/structures)

# Install Qt5 dependencies on Windows
if(WIN32)
	#include(DeployQt)
	#windeployqt(DSCSTools ${CMAKE_BINARY_DIR}/target)
endif()
