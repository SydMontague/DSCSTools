cmake_minimum_required (VERSION 3.8)

SET(SOURCE_FILES
		DSCSTools.cpp
		DSCSTools.h
		MDB1.cpp
		MDB1.h
		EXPA.cpp
		EXPA.h
		SaveFile.cpp
		SaveFile.h
		)

# Add source to this project's executable.
add_executable (DSCSTools ${SOURCE_FILES})
target_link_libraries(DSCSTools PUBLIC doboz ${Boost_LIBRARIES})

install (TARGETS DSCSTools DESTINATION bin)
